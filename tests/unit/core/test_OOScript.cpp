/*	test_OOScript.mm
        Unit tests for OOScript (src/Core/Scripting/OOScript.h/.mm).

        OOScript is the abstract root of the scripts: its subclasses (OOJSScript
   and OOPListScript, C++) answer the name, description and version, whether
   they need a tickle, and run on a target; the root answers a display name and
   a description from them, and its class methods load scripts from files,
   folders and lists. The class methods reach the resource manager, the cache
   and the legacy-script sanitizer, so the test links every game object but
   main's (tests/unit/core/meson.build entry ['*']) and defines gDebugFlags. The
   user's directories point at a scratch folder. JavaScript files are not loaded
   (that needs the engine); every path that would reach OOJSScript is one that
   fails first. The expectations cover: the root's own answers, a subclass's
   overrides and the super call, the loaders (no file, a folder, an unknown
   extension, a legacy script from the cache, names not found, a world-scripts
   list, a folder with no scripts). The cases ask the C++ class (the test
   subclass is a C++ one, its super call the root's member). Run: bash
   meson test -C build/meson_test test_OOScript
*/

#include <process.h>

#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entity.h"
#include "CacheManager.h"
#include "OOPListScript.h"
#include "OOScript.h"
#include "oofnd/FileSystem.hpp"
#include "oofnd/String.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

// A subclass that overrides the subclass responsibilities (its super calls are
// the root's members).
class TestObjCScript final : public OOScript {
 public:
  std::optional<std::string> name() override { return std::string("objc"); }
  std::optional<std::string> scriptDescription() override {
    return std::string("an Objective-C test script");
  }
  std::optional<std::string> version() override {
    if (hasVersion) return std::string("1.5");
    return std::nullopt;
  }
  bool requiresTickle() override { return true; }
  void runWithTarget(Entity* target) override {
    runs++;
    lastTarget = target;
  }

  std::optional<std::string> superName() { return OOScript::name(); }
  std::optional<std::string> superDescriptionComponents() {
    return OOScript::descriptionComponents();
  }
  bool superRequiresTickle() { return OOScript::requiresTickle(); }

  int runs = 0;
  Entity* lastTarget = nullptr;  // the C++ entity
  bool hasVersion = false;
};

namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;
const char* const kCacheName = "sanitized legacy scripts";

void WriteText(const stdfs::path& path, const std::string& text) {
  stdfs::create_directories(path.parent_path());
  EXPECT_TRUE(oo::fs::writeFile(path, oo::Data(text.data(), text.size()),
                             oo::fs::WriteMode::kDirect)
               .has_value());
}

// The scratch home and game folder, made once, before the cache manager exists.
void SetUp() {
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-script-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  WriteText(sRoot / "Resources" / "Info-gnustep.plist",
            "{ CFBundleVersion = \"9.9.9-test\"; }");
}

// A folder under the scratch root, as the loaders spell paths (forward
// slashes).
std::string Folder(const char* name) {
  const stdfs::path folder = sRoot / name;
  stdfs::create_directories(folder);
  return folder.generic_string();
}

// A legacy script file that exists, whose sanitized scripts are already in the
// cache under its path.
void CacheLegacyScript(const std::string& path, const char* scriptName) {
  WriteText(stdfs::path(path), "{}");
  oo::PList::Dict entry;
  entry["script"] = oo::PList(oo::PList::Array{});
  oo::PList::Dict scripts;
  scripts[scriptName] = oo::PList(std::move(entry));
  oo::CacheManager::sharedCache()->setPList(oo::PList(std::move(scripts)), path,
                                          kCacheName);
}

std::vector<std::string> Names(const std::vector<std::shared_ptr<OOScript>>& scripts) {
  std::vector<std::string> names;
  for (const auto& script : scripts)
    names.push_back(script->name().value_or("<none>"));
  return names;
}

}  // namespace

TEST(OOScript, abstractRoot) {
  ::SetUp();
  {
    // The root's own answers: none, and an error logged for each subclass
    // responsibility.
    const std::shared_ptr<OOScript> root = oo::makeShared<OOScript>();
    EXPECT_TRUE(root != nullptr);
    EXPECT_TRUE(!root->name().has_value());
    EXPECT_TRUE(!root->scriptDescription().has_value());
    EXPECT_TRUE(!root->version().has_value());
    EXPECT_TRUE(!root->displayName().has_value());
    EXPECT_TRUE(!root->requiresTickle());
    root->runWithTarget(nullptr);
    EXPECT_TRUE(root->description().find("\"(null)\" version (null)") !=
             std::string::npos);
  }
}

TEST(OOScript, objCSubclass) {
  ::SetUp();
  {
    const std::shared_ptr<TestObjCScript> script = oo::makeShared<TestObjCScript>();
    EXPECT_EQ(script->name().value_or("<none>"), "objc");
    EXPECT_EQ(script->displayName().value_or("<none>"),
                "objc");  // no version: just the name
    EXPECT_TRUE(script->description().find("\"objc\" version (null)") !=
             std::string::npos);

    script->hasVersion = true;
    EXPECT_EQ(script->displayName().value_or("<none>"), "objc 1.5");
    EXPECT_EQ(script->superDescriptionComponents().value_or("<none>"),
                "\"objc\" version 1.5");
    EXPECT_TRUE(script->description().find("\"objc\" version 1.5") !=
             std::string::npos);

    // [super ...] reaches the root's own answers.
    EXPECT_TRUE(!script->superName().has_value());
    EXPECT_TRUE(!script->superRequiresTickle());
    EXPECT_TRUE(script->requiresTickle());

    Entity* target = oo::MadeEntity(oo::makeShared<Entity>());
    script->runWithTarget(target);
    EXPECT_EQ(script->runs, 1);
    EXPECT_TRUE(script->lastTarget == target);
  }
}

TEST(OOScript, scriptsFromFileAtPath) {
  ::SetUp();
  {
    const std::string folder = Folder("files");

    // No file, a folder, and an extension that is not a script's: none.
    EXPECT_TRUE(
        !OOScript::scriptsFromFileAtPath(folder + "/missing.js").has_value());
    EXPECT_TRUE(!OOScript::scriptsFromFileAtPath(folder).has_value());
    WriteText(stdfs::path(folder) / "notes.txt", "text");
    EXPECT_TRUE(
        !OOScript::scriptsFromFileAtPath(folder + "/notes.txt").has_value());

    // A legacy script (its extension in any case) loads its property-list
    // scripts.
    const std::string path = folder + "/legacy.PLIST";
    CacheLegacyScript(path, "legacy");
    const auto scripts = OOScript::scriptsFromFileAtPath(path);
    EXPECT_TRUE(scripts.has_value() && scripts->size() == 1);
    if (scripts.has_value() && scripts->size() == 1) {
      EXPECT_TRUE((Names(*scripts) == std::vector<std::string>{"legacy"}));
      EXPECT_TRUE(dynamic_cast<OOPListScript*>((*scripts)[0].get()) != nullptr);
    }
  }
}

TEST(OOScript, scriptsByName) {
  ::SetUp();
  {
    // A name the resource manager does not find: none, and a list of such names
    // loads nothing.
    EXPECT_TRUE(!OOScript::scriptsFromFileNamed("oo-test-missing.js").has_value());
    const std::vector<std::shared_ptr<OOScript>> listed =
        OOScript::scriptsFromList(std::vector<std::string>{
            "oo-test-missing.js", "oo-test-missing.plist"});
    EXPECT_TRUE(listed.empty());
    EXPECT_TRUE(OOScript::scriptsFromList(std::vector<std::string>()).empty());

    // The single JavaScript loaders: nil for no name, a legacy or unknown
    // extension, or a name not found.
    for (int ai = 0; ai < 2; ai++) {
      auto load = [ai](const std::string& name) -> std::shared_ptr<OOScript> {
        if (ai) return OOScript::jsAIScriptFromFileNamed(name, oo::PList());
        return OOScript::jsScriptFromFileNamed(name, oo::PList());
      };
      EXPECT_TRUE(load("") == nullptr);
      EXPECT_TRUE(load("oo-test.plist") == nullptr);
      EXPECT_TRUE(load("oo-test.txt") == nullptr);
      EXPECT_TRUE(load("oo-test-missing.js") == nullptr);
      EXPECT_TRUE(load("oo-test-missing.ES") == nullptr);
    }
  }
}

TEST(OOScript, worldScriptsAtPath) {
  ::SetUp();
  {
    // A folder with no scripts: none.
    EXPECT_TRUE(!OOScript::worldScriptsAtPath(Folder("empty")).has_value());

    // world-scripts.plist names scripts to load by name; names not found load
    // nothing, but the list was found, so the answer is an empty list rather
    // than none.
    const std::string listed = Folder("listed");
    WriteText(stdfs::path(listed) / "world-scripts.plist",
              "( \"oo-test-missing.js\" )");
    const auto fromList = OOScript::worldScriptsAtPath(listed);
    EXPECT_TRUE(fromList.has_value() && fromList->empty());

    // A world-scripts.plist that is not an array is skipped; script.plist is
    // the fallback.
    const std::string legacy = Folder("legacy");
    WriteText(stdfs::path(legacy) / "world-scripts.plist", "{ a = b; }");
    CacheLegacyScript(oo::str::appendingPathComponent(legacy, "script.plist"),
                      "fallback");
    const auto fromLegacy = OOScript::worldScriptsAtPath(legacy);
    EXPECT_TRUE(fromLegacy.has_value() && fromLegacy->size() == 1);
    if (fromLegacy.has_value() && fromLegacy->size() == 1) {
      EXPECT_TRUE((Names(*fromLegacy) == std::vector<std::string>{"fallback"}));
    }

    // world-scripts.plist wins over script.plist.
    WriteText(stdfs::path(legacy) / "world-scripts.plist", "( )");
    const auto listWins = OOScript::worldScriptsAtPath(legacy);
    EXPECT_TRUE(listWins.has_value() && listWins->empty());
  }
}

// The crossing.

namespace {

// A C++ subclass, as a converted OOJSScript or OOPListScript will be.
class TestCxxScript final : public OOScript {
 public:
  std::optional<std::string> name() override { return std::string("cxx"); }
  std::optional<std::string> version() override { return std::string("2"); }
  bool requiresTickle() override { return true; }
  void runWithTarget(Entity* target) override {
    runs++;
    lastTarget = target;
  }

  int runs = 0;
  Entity* lastTarget = nullptr;  // the C++ entity
};

}  // namespace

TEST(OOScript, crossingCxxSubclass) {
  ::SetUp();
  {
    // A C++ subclass's overrides answer.
    const std::shared_ptr<TestCxxScript> script = oo::makeShared<TestCxxScript>();
    EXPECT_EQ(script->name().value_or("<none>"), "cxx");
    EXPECT_EQ(script->version().value_or("<none>"), "2");
    EXPECT_TRUE(!script->scriptDescription()
                  .has_value());  // not overridden: the root's
    EXPECT_EQ(script->displayName().value_or("<none>"), "cxx 2");
    EXPECT_TRUE(script->requiresTickle());
    EXPECT_TRUE(script->description().find("\"cxx\" version 2") !=
             std::string::npos);
    Entity* target = oo::MadeEntity(oo::makeShared<Entity>());
    script->runWithTarget(target);
    EXPECT_EQ(script->runs, 1);
    EXPECT_TRUE(script->lastTarget == target);

    // The loaders as static members.
    EXPECT_TRUE(OOScript::scriptsFromList(
                 std::vector<std::string>{"oo-test-missing.js"})
                 .empty());
    EXPECT_TRUE(!OOScript::scriptsFromFileAtPath(Folder("files") + "/missing.js")
                  .has_value());
    EXPECT_TRUE(OOScript::jsScriptFromFileNamed("", oo::PList()) == nullptr);
  }
}

TEST(OOScript, cleanUp) {
  stdfs::current_path(stdfs::temp_directory_path());
  std::error_code ignored;
  stdfs::remove_all(sRoot, ignored);
  EXPECT_TRUE(true);
}
