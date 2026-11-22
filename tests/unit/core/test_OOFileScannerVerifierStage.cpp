/*	test_OOFileScannerVerifierStage.mm
        Unit tests for OOFileScannerVerifierStage.h/.mm:
   OOFileScannerVerifierStage and the intermediate class
   OOFileHandlingVerifierStage that the file-handling stages subclass.

        The tests pin the dependency and dependent a file-handling stage names
   (and registers), the base class's dependents from a subclass, and the
   scanner's answers over a real directory: case-insensitive look-up with
   folder fallback, junk, skipped and Read Me files, listings, data and
   property lists. The verifier answers the C++ stage itself, so
   cxxFileStageBehindTheFacade asks the stage (its description through
   description(), as scannerFindsFiles does). Run: bash
   tests/run.sh unit
*/

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "FileScannerVerifierStage.h"
#include "Logging.h"

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
   so the test is the verifier: the configuration is a verifyOXP.plist of the
   test's, registration keeps stages by name. The resource manager answers the
   one question the scanner asks it, for built-in files.
*/
#include "OOOXPVerifierTestDouble.h"
#include "ResourceManager.h"

// C++ definitions of the ResourceManager statics the code under test calls,
// with the stand-in's answers.
std::optional<std::string> ResourceManager::pathForFileNamed(
    const std::string& fileName, const std::optional<std::string>& folderName) {
  if (fileName != "builtin.png") return std::nullopt;
  return "builtin/" + folderName.value_or("") + "/" + fileName;
}

// A file-handling stage, as the leaf stages are.
class TestFileStage : public oo::FileHandlingVerifierStage {
 public:
  std::optional<std::string> name() override { return "Testing files"; }
};

// Extends its superclass's dependents through the base call, as
// OOTextureHandlingStage does.
class TestSuperCallingFileStage : public TestFileStage {
 public:
  std::optional<std::vector<std::string>> dependents() override {
    std::vector<std::string> result =
        TestFileStage::dependents().value_or(std::vector<std::string>());
    result.push_back("Extra dependent");
    return result;
  }
};

// A converted file-handling stage: a C++ subclass of the intermediate class.
// Global, as a leaf stage is once converted, so Objective-C sees it as an
// OOOXPVerifierStage.
class TestCxxFileStage : public oo::FileHandlingVerifierStage {
 public:
  std::optional<std::string> name() override { return "Testing files in C++"; }
  void run() override { runs++; }

  int runs = 0;
};

namespace {

const char* const kConfiguration =
    "{"
    "	junkFiles = (\".DS_Store\");"
    "	skipDirectories = (\".svn\");"
    "	readMeNames = { stems = (\"readme\"); extensions = (\".txt\"); };"
    "	knownRootDirectories = (\"Config\", \"Textures\", \"Images\");"
    "	knownFiles = { Config = (\"shipdata.plist\"); };"
    "	knownConfigFiles = (\"shipdata.plist\");"
    "}";

void WriteFile(const std::filesystem::path& path, const char* contents) {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream(path, std::ios::binary) << contents;
}

// A small OXP in the working directory: a lowercase textures folder, a root
// file, a Read Me, a junk file, a skipped folder and a nested folder.
std::string MakeOXP() {
  const std::filesystem::path base =
      std::filesystem::current_path() / "test_OOFileScannerVerifierStage.oxp";
  std::filesystem::remove_all(base);
  WriteFile(base / "Config" / "shipdata.plist", "{ a = 1; }");
  WriteFile(base / "Config" / "bad.plist", "{ a = ");
  WriteFile(base / "textures" / "Foo.PNG", "png");
  WriteFile(base / "Images" / "b.png", "b");
  WriteFile(base / "Images" / "A.png", "a");
  WriteFile(base / "Images" / "nested" / "c.png", "c");
  WriteFile(base / "Root.png", "root");
  WriteFile(base / "ReadMe.txt", "read me");
  WriteFile(base / ".DS_Store", "junk");
  WriteFile(base / ".svn" / "entries", "svn");
  return base.generic_string();
}

oo::OXPVerifier* MakeVerifier(const std::string& path) {
  return OOOXPVerifierTestAccess::Make(path, kConfiguration);
}

const std::vector<std::string> kScannerName = {"Scanning files"};
const std::vector<std::string> kUnusedName = {"Checking for unused files"};

// The registered scanner, as the stages find it.
oo::FileScannerVerifierStage* ScannerOf(oo::OXPVerifier* verifier) {
  return dynamic_cast<oo::FileScannerVerifierStage*>(
      verifier->stageWithName(oo::FileScannerVerifierStage::kName));
}

}  // namespace

TEST(OOFileScannerVerifierStage, fileHandlingStageNamesAndRegistersItsNeighbours) {
  {
    oo::OXPVerifier* verifier = MakeVerifier("");
    const std::shared_ptr<TestFileStage> stage = oo::makeShared<TestFileStage>();
    stage->setVerifier(verifier);
    const int before = OOOXPVerifierTestAccess::Registrations();

    EXPECT_TRUE(stage->dependencies() == kScannerName);
    EXPECT_TRUE(stage->dependents() == kUnusedName);
    EXPECT_TRUE(OOOXPVerifierTestAccess::Registrations() == before + 2);
    EXPECT_TRUE(ScannerOf(verifier) != nullptr);
    EXPECT_TRUE(verifier->stageWithName("Checking for unused files") != nullptr);

    // Asked again (by another stage), nothing new is registered.
    EXPECT_TRUE(stage->dependencies() == kScannerName);
    EXPECT_TRUE(stage->dependents() == kUnusedName);
    EXPECT_TRUE(OOOXPVerifierTestAccess::Registrations() == before + 2);

    // The rest is the base's.
    EXPECT_TRUE(stage->name() == std::optional<std::string>("Testing files"));
    EXPECT_TRUE(stage->shouldRun());
  }
}

TEST(OOFileScannerVerifierStage, superDependentsReachTheIntermediateClass) {
  {
    const std::shared_ptr<TestSuperCallingFileStage> stage =
        oo::makeShared<TestSuperCallingFileStage>();
    stage->setVerifier(MakeVerifier(""));
    EXPECT_TRUE(stage->dependents() ==
             (std::vector<std::string>{"Checking for unused files",
                                       "Extra dependent"}));
    EXPECT_TRUE(stage->dependencies() == kScannerName);
  }
}

TEST(OOFileScannerVerifierStage, scannerFindsFiles) {
  {
    const std::string base = MakeOXP();
    oo::OXPVerifier* verifier = MakeVerifier(base);
    EXPECT_TRUE(oo::FileScannerVerifierStage::nameForDependencyForVerifier(
                 verifier) == std::optional<std::string>("Scanning files"));
    oo::FileScannerVerifierStage* scanner = ScannerOf(verifier);
    EXPECT_TRUE(scanner != nullptr && scanner->verifier() == verifier);
    EXPECT_TRUE(scanner->name() == std::optional<std::string>("Scanning files"));
    EXPECT_TRUE(
        scanner->description().starts_with("<OOFileScannerVerifierStage 0x"));
    EXPECT_TRUE(scanner->description().ends_with(">{\"Scanning files\"}"));
    scanner->run();

    // Case-insensitive, in the folder or bare in the root; the result is the
    // path on disk.
    EXPECT_TRUE(scanner->fileExists("foo.png", "Textures", "a test", false));
    EXPECT_TRUE(scanner->pathForFile("FOO.png", "Textures", std::nullopt, false) ==
             std::optional<std::string>(base + "/textures/Foo.PNG"));
    EXPECT_TRUE(
        scanner->pathForFile("root.png", "Textures", std::nullopt, false) ==
        std::optional<std::string>(base + "/Root.png"));
    EXPECT_TRUE(
        scanner->pathForFile("Root.png", std::nullopt, std::nullopt, false) ==
        std::optional<std::string>(base + "/Root.png"));

    // Not found: the built-in files only when asked; no name is no file.
    EXPECT_TRUE(
        !scanner->pathForFile("builtin.png", "Textures", std::nullopt, false)
             .has_value());
    EXPECT_TRUE(
        scanner->pathForFile("builtin.png", "Textures", std::nullopt, true) ==
        std::optional<std::string>("builtin/Textures/builtin.png"));
    EXPECT_TRUE(
        !scanner->pathForFile("missing.png", "Textures", std::nullopt, true)
             .has_value());
    EXPECT_TRUE(
        !scanner->fileExists(std::nullopt, "Textures", std::nullopt, true));

    // Junk, Read Me files and skipped folders are not part of the OXP; a nested
    // folder is ignored.
    EXPECT_TRUE(
        !scanner->fileExists(".DS_Store", std::nullopt, std::nullopt, false));
    EXPECT_TRUE(
        !scanner->fileExists("ReadMe.txt", std::nullopt, std::nullopt, false));
    EXPECT_TRUE(!scanner->filesInFolder(".svn").has_value());
    EXPECT_TRUE(!scanner->fileExists("c.png", "Images", std::nullopt, false));

    // Listings: in the byte order of the lowercase names; the root's is "".
    EXPECT_TRUE(scanner->filesInFolder("IMAGES") ==
             (std::vector<std::string>{"A.png", "b.png"}));
    EXPECT_TRUE(scanner->filesInFolder("") ==
             std::vector<std::string>{"Root.png"});
    EXPECT_TRUE(!scanner->filesInFolder("Sounds").has_value());
    EXPECT_TRUE(!scanner->filesInFolder(std::nullopt).has_value());

    EXPECT_TRUE(scanner->displayNameForFile("f", "d") ==
             std::optional<std::string>("d/f"));
    EXPECT_TRUE(scanner->displayNameForFile("f", std::nullopt) ==
             std::optional<std::string>("f"));
    EXPECT_TRUE(!scanner->displayNameForFile(std::nullopt, "d").has_value());

    // Contents.
    EXPECT_TRUE(
        scanner->dataForFile("shipdata.plist", "config", std::nullopt, false)
            .stringView() == "{ a = 1; }");
    EXPECT_TRUE(
        scanner->dataForFile("missing.plist", "Config", std::nullopt, false)
            .stringView()
            .empty());
    const oo::PList plist =
        scanner->plistNamed("shipdata.plist", "Config", std::nullopt, false);
    EXPECT_TRUE(plist.get<std::string>("a") == "1");
    EXPECT_TRUE(scanner->plistNamed("bad.plist", "Config", std::nullopt, false)
                 .isNull());  // logs the parse error
    EXPECT_TRUE(scanner->plistNamed("missing.plist", "Config", std::nullopt, false)
                 .isNull());

    std::filesystem::remove_all(base);
  }
}

TEST(OOFileScannerVerifierStage, cxxFileStageBehindTheFacade) {
  {
    oo::OXPVerifier* verifier = MakeVerifier("");
    const std::shared_ptr<TestCxxFileStage> stage = oo::makeShared<TestCxxFileStage>();
    stage->setVerifier(verifier);
    EXPECT_TRUE(stage->dependencies() == kScannerName);
    EXPECT_TRUE(stage->dependents() == kUnusedName);
    EXPECT_TRUE(ScannerOf(verifier) != nullptr);

    EXPECT_TRUE(stage->name() ==
             std::optional<std::string>("Testing files in C++"));
    EXPECT_TRUE(stage->dependencies() == kScannerName);
    EXPECT_TRUE(stage->dependents() == kUnusedName);
    stage->dependencyRegistrationComplete();
    stage->performRun();
    EXPECT_TRUE(stage->runs == 1);
    EXPECT_TRUE(stage->description().starts_with("<TestCxxFileStage 0x"));
  }
}

TEST(OOFileScannerVerifierStage, scannerFacade) {
  {
    const std::string base = MakeOXP();
    oo::OXPVerifier* verifier = MakeVerifier(base);
    oo::FileScannerVerifierStage::nameForDependencyForVerifier(verifier);
    oo::FileScannerVerifierStage* cxxScanner = ScannerOf(verifier);
    EXPECT_TRUE(cxxScanner != nullptr);

    // The C++ members answer.
    cxxScanner->run();
    EXPECT_TRUE(cxxScanner->name() ==
             std::optional<std::string>(oo::FileScannerVerifierStage::kName));
    EXPECT_TRUE(
        cxxScanner->pathForFile("foo.png", "Textures", std::nullopt, false) ==
        std::optional<std::string>(base + "/textures/Foo.PNG"));
    EXPECT_TRUE(cxxScanner->fileExists("A.PNG", "images", "a test", false));
    EXPECT_TRUE(cxxScanner->filesInFolder("Images") ==
             (std::vector<std::string>{"A.png", "b.png"}));
    EXPECT_TRUE(
        cxxScanner->dataForFile("Root.png", std::nullopt, std::nullopt, false)
            .stringView() == "root");
    EXPECT_TRUE(
        cxxScanner->plistNamed("shipdata.plist", "Config", std::nullopt, false)
            .get<std::string>("a") == "1");
    EXPECT_TRUE(cxxScanner->displayNameForFile("f", "d") ==
             std::optional<std::string>("d/f"));
    std::filesystem::remove_all(base);
  }
}
