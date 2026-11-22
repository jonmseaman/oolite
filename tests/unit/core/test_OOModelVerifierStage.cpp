/*	test_OOModelVerifierStage.mm
        Unit tests for OOModelVerifierStage.h/.mm: the model stage, a subclass
   of OOTextureHandlingStage.

        The tests pin: its name, that asking its name registers it (once), its
   neighbours (the intermediate class's), the models other stages name (found in
   the OXP or built in, or not; an empty name is not found; each model and
   context once), when it runs, and what it logs (a placeholder check of each
   model, in the order named). The verifier finds it again by its name. The
   stage is global. The verifier registers and answers the C++ stage itself, so
   nameRegistersTheStage and the facade case ask the stage (its description
   through description()). Run: tests/run.sh unit
*/

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "ModelVerifierStage.h"
#include "OXPVerifierStage.h"
#include "PixMap.h"
#include "TextureLoader.h"
#include "oofnd/Log.hpp"
#include "oofnd/PListParsing.hpp"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked; the base calls this one function of it (a subclass responsibility).
*/
namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
}

}  // namespace oo


/*	The verifier and the resource manager are not linked either (they reach
   the whole game). The stages talk to the verifier through its interface only,
   so the test is the verifier (as in test_OOFileScannerVerifierStage.mm). The
   resource manager has one built-in model.
*/
#include "OOOXPVerifierTestDouble.h"
#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::optional<std::string> ResourceManager::pathForFileNamed(
    const std::string& fileName, const std::optional<std::string>& folderName) {
  if (fileName != "builtin.dat") return std::nullopt;
  return "builtin/" + folderName.value_or("") + "/" + fileName;
}

// The texture stage (linked for the intermediate class) loads through these; no
// model here names a texture.
namespace oo {

void FreePixMap(PixMap* ioPixMap) { *ioPixMap = PixMap{}; }

}  // namespace oo


// The loader factory, answering nil.
std::shared_ptr<oo::TextureLoader> oo::TextureLoader::loaderWithPath(
    const std::optional<std::string>& /*path*/, oo::TextureFlags /*options*/) {
  return nullptr;
}

namespace {

const char* const kConfiguration =
    "{"
    "	knownRootDirectories = (\"Models\");"
    "}";

// The log, as the stage writes it.
std::vector<std::string> gLog;

void Capture(std::string_view line) { gLog.emplace_back(line); }

void StartLog() {
  oo::log::logger().setInitialized(true);
  oo::log::logger().setSink(&Capture);
  gLog.clear();
}

int LogLinesContaining(std::string_view text) {
  int count = 0;
  for (const std::string& line : gLog) {
    if (line.find(text) != std::string::npos) count++;
  }
  return count;
}

void WriteFile(const std::filesystem::path& path, const char* contents) {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream(path, std::ios::binary) << contents;
}

// The scanner, registered with the verifier and run over its OXP, as the
// verifier runs it first.
void RunScanner(oo::OXPVerifier* verifier) {
  oo::FileScannerVerifierStage::nameForDependencyForVerifier(verifier);
  verifier->stageWithName(oo::FileScannerVerifierStage::kName)->run();
}

const std::vector<std::string> kScannerName = {"Scanning files"};
const std::vector<std::string> kUnusedName = {"Checking for unused files"};

const std::filesystem::path kBase =
    std::filesystem::current_path() / "test_OOModelVerifierStage.oxp";

// An OXP with the given files (path, contents), and its verifier with the
// scanner run over it.
oo::OXPVerifier* MakeVerifier(
    std::initializer_list<std::pair<const char*, const char*>> files) {
  std::filesystem::remove_all(kBase);
  std::filesystem::create_directories(kBase);
  for (const auto& [name, contents] : files) WriteFile(kBase / name, contents);
  oo::OXPVerifier* verifier =
      OOOXPVerifierTestAccess::Make(kBase.generic_string(), kConfiguration);
  RunScanner(verifier);
  StartLog();
  return verifier;
}

}  // namespace

TEST(OOModelVerifierStage, nameRegistersTheStage) {
  {
    oo::OXPVerifier* verifier = MakeVerifier({});
    EXPECT_TRUE(verifier->stageWithName("Testing models") == nullptr);
    EXPECT_TRUE(verifier->stageWithName(oo::ModelVerifierStage::kName) == nullptr);
    EXPECT_TRUE(oo::ModelVerifierStage::nameForReverseDependencyForVerifier(
                 verifier) == "Testing models");
    oo::OXPVerifierStage* registered =
        verifier->stageWithName(oo::ModelVerifierStage::kName);
    EXPECT_TRUE(registered != nullptr &&
             verifier->stageWithName("Testing models") == registered);
    EXPECT_TRUE(oo::ModelVerifierStage::nameForReverseDependencyForVerifier(
                 verifier) == "Testing models");
    EXPECT_TRUE(verifier->stageWithName(oo::ModelVerifierStage::kName) ==
             registered);  // once

    EXPECT_TRUE(registered->name() ==
             std::optional<std::string>("Testing models"));
    EXPECT_TRUE(registered->dependencies() == kScannerName);
    EXPECT_TRUE(registered->dependents() ==
             (std::vector<std::string>{"Checking for unused files",
                                       "Testing textures and images"}));
    EXPECT_TRUE(!registered->shouldRun());
  }
  std::filesystem::remove_all(kBase);
}

TEST(OOModelVerifierStage, stagesNameModels) {
  {
    oo::OXPVerifier* verifier = MakeVerifier({{"Models/ship.dat", "model"}});
    const std::shared_ptr<oo::ModelVerifierStage> stage =
        oo::makeShared<oo::ModelVerifierStage>();
    stage->setVerifier(verifier);
    const oo::PList materials = *oo::parsePropertyListData("{ a = b; }");

    EXPECT_TRUE(!stage->modelNamed("", "a", "shipdata.plist", oo::PList(),
                                oo::PList()));
    EXPECT_TRUE(!stage->modelNamed("missing.dat", "a", "shipdata.plist",
                                oo::PList(), oo::PList()));
    EXPECT_TRUE(!stage->shouldRun() &&
             gLog.empty());  // a missing model: the caller complains

    EXPECT_TRUE(stage->modelNamed("ship.dat", "a", "shipdata.plist", materials,
                               oo::PList()));
    EXPECT_TRUE(stage->shouldRun());
    EXPECT_TRUE(stage->modelNamed("ship.dat", "a", "shipdata.plist", materials,
                               oo::PList()));  // again: once
    EXPECT_TRUE(stage->modelNamed("ship.dat", "b", "shipdata.plist", oo::PList(),
                               oo::PList()));  // another entry
    EXPECT_TRUE(stage->modelNamed("builtin.dat", std::nullopt, "demoships.plist",
                               oo::PList(), oo::PList()));
    EXPECT_TRUE(stage->modelNamed("SHIP.DAT", "c", "shipdata.plist", oo::PList(),
                               oo::PList()));
    const size_t caseWarnings =
        gLog.size();  // the scanner's case warning for SHIP.DAT
    EXPECT_TRUE(LogLinesContaining("case mismatch") == 1);

    gLog.clear();
    stage->run();
    EXPECT_TRUE(caseWarnings == 1);
    EXPECT_TRUE(gLog == (std::vector<std::string>{
                         "TODO: implement model verifier.",
                         "- Pretending to verify model ship.dat referenced in "
                         "entry \"a\" of shipdata.plist.",
                         "- Pretending to verify model ship.dat referenced in "
                         "entry \"b\" of shipdata.plist.",
                         "- Pretending to verify model builtin.dat referenced "
                         "in demoships.plist.",
                         "- Pretending to verify model SHIP.DAT referenced in "
                         "entry \"c\" of shipdata.plist."}));

    // The models are forgotten after the run.
    EXPECT_TRUE(!stage->shouldRun());
    gLog.clear();
    stage->run();
    EXPECT_TRUE(gLog ==
             std::vector<std::string>{"TODO: implement model verifier."});
  }
  std::filesystem::remove_all(kBase);
}

// The stage is global: the verifier registers the stage itself, and finds it by
// name for the ship data stage.
TEST(OOModelVerifierStage, facade) {
  {
    oo::OXPVerifier* verifier = MakeVerifier({{"Models/ship.dat", "model"}});
    oo::ModelVerifierStage::nameForReverseDependencyForVerifier(verifier);
    oo::ModelVerifierStage* stage = static_cast<oo::ModelVerifierStage*>(
        verifier->stageWithName(oo::ModelVerifierStage::kName));
    EXPECT_TRUE(stage != nullptr);
    EXPECT_TRUE(stage->description().starts_with("<OOModelVerifierStage 0x"));

    EXPECT_TRUE(stage->name() ==
             std::optional<std::string>(oo::ModelVerifierStage::kName));
    EXPECT_TRUE(stage->dependents() ==
             (std::vector<std::string>{"Checking for unused files",
                                       "Testing textures and images"}));
    EXPECT_TRUE(!stage->shouldRun());
    EXPECT_TRUE(stage->modelNamed("ship.dat", "a", "shipdata.plist", oo::PList(),
                               oo::PList()));
    EXPECT_TRUE(stage->shouldRun());
    gLog.clear();
    stage->dependencyRegistrationComplete();
    stage->performRun();
    EXPECT_TRUE(
        gLog.size() == 2 &&
        LogLinesContaining("- Pretending to verify model ship.dat referenced "
                           "in entry \"a\" of shipdata.plist.") == 1);
  }
  std::filesystem::remove_all(kBase);
}
