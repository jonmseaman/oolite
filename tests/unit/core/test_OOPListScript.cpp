/*	test_OOPListScript.mm
        Unit tests for OOPListScript (src/Core/Scripting/OOPListScript.h/.mm).

        OOPListScript is a legacy (property-list) world script:
   +scriptsInPListFile: reads a file of named script arrays, sanitizes each,
   caches the sanitized scripts in OOCacheManager, and makes one script per
   array, answering the name, the metadata's description and version, and
   running its actions on the player. Its file reaches the cache manager, the
   legacy-script sanitizer and the player, so the test links every game object
   but main's (tests/unit/core/meson.build entry ['*']) and defines gDebugFlags.
   The user's directories point at a scratch folder. The sanitizer needs the
   game's resources, so the scripts come from the cache, as the game loads them
   after the first run, and from files with no script arrays. The expectations
   cover: the cached path (names in key order, metadata, a missing or ill-typed
   script array), the file path (no file, a file that is not a dictionary, a
   dictionary with nothing to sanitize and what it caches), the description and
   version, the display name, -requiresTickle, and a run with a target that is
   not a ship.

        OOPListScript is a C++ subclass of OOScript. The scripts are held as
   std::shared_ptr<OOScript>, the cases ask the C++ class, and plistScriptIsItsOwnObject
   checks that a script is its own object. Run: tests/run.sh unit
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
#include "oofnd/FileSystem.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

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
          ("oo-test-plistscript-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  WriteText(sRoot / "Resources" / "Info-gnustep.plist",
            "{ CFBundleVersion = \"9.9.9-test\"; }");
}

oo::PList Str(const char* string) { return oo::PList(std::string(string)); }

oo::PList Dict(std::vector<std::pair<std::string, oo::PList>> entries) {
  oo::PList::Dict dict;
  for (auto& entry : entries) dict[entry.first] = std::move(entry.second);
  return oo::PList(std::move(dict));
}

std::vector<std::string> Names(const std::vector<std::shared_ptr<OOScript>>& scripts) {
  std::vector<std::string> names;
  for (const auto& script : scripts)
    names.push_back(script->name().value_or("<none>"));
  return names;
}

}  // namespace

TEST(OOPListScript, cachedScripts) {
  ::SetUp();
  {
    const oo::PList metadata =
        Dict({{"description", Str("A test script")}, {"version", Str("1.2")}});
    const oo::PList actions =
        oo::PList(oo::PList::Array{Str("set: mission_x 1")});
    oo::CacheManager::sharedCache()->setPList(
        Dict({
            {"zeta", Dict({{"script", actions}, {"!metadata!", metadata}})},
            {"alpha", Dict({{"script", actions}})},
            {"badScript", Dict({{"script", Str("not an array")}})},
            {"notADict", Str("x")},
        }),
        "cached.plist", kCacheName);

    const auto scripts = OOPListScript::scriptsInPListFile("cached.plist");
    EXPECT_TRUE(scripts.has_value());
    if (!scripts.has_value()) return;
    EXPECT_EQ(scripts->size(), 4u);
    EXPECT_TRUE((Names(*scripts) ==
              std::vector<std::string>{"alpha", "badScript", "notADict",
                                       "zeta"}));  // key order
    for (const auto& script : *scripts)
      EXPECT_TRUE(dynamic_cast<OOPListScript*>(script.get()) != nullptr);

    OOScript* alpha = (*scripts)[0].get();
    OOScript* zeta = (*scripts)[3].get();
    EXPECT_TRUE(!alpha->scriptDescription().has_value());
    EXPECT_TRUE(!alpha->version().has_value());
    EXPECT_EQ(alpha->displayName().value_or("<none>"), "alpha");
    EXPECT_EQ(zeta->scriptDescription().value_or("<none>"), "A test script");
    EXPECT_EQ(zeta->version().value_or("<none>"), "1.2");
    EXPECT_EQ(zeta->displayName().value_or("<none>"), "zeta 1.2");
    EXPECT_TRUE(zeta->requiresTickle());
    EXPECT_TRUE(zeta->description().find("OOPListScript") != std::string::npos);

    // A run whose target is not a ship logs and does nothing (it never reaches
    // the player).
    Entity* notAShip = oo::MadeEntity(oo::makeShared<Entity>());
    zeta->runWithTarget(notAShip);
  }
}

TEST(OOPListScript, metadataTypes) {
  ::SetUp();
  {
    // A description or version that is not a string reads as none.
    const oo::PList metadata =
        Dict({{"description", oo::PList(3)},
              {"version", oo::PList(oo::PList::Array{})}});
    oo::CacheManager::sharedCache()->setPList(
        Dict({{"odd", Dict({{"script", oo::PList(oo::PList::Array{})},
                            {"!metadata!", metadata}})}}),
        "odd.plist", kCacheName);
    const auto scripts = OOPListScript::scriptsInPListFile("odd.plist");
    EXPECT_TRUE(scripts.has_value() && scripts->size() == 1);
    if (!scripts.has_value() || scripts->size() != 1) return;
    OOScript* odd = (*scripts)[0].get();
    EXPECT_TRUE(!odd->scriptDescription().has_value());
    EXPECT_TRUE(!odd->version().has_value());
    EXPECT_EQ(odd->displayName().value_or("<none>"), "odd");
    EXPECT_EQ(odd->name().value_or("<none>"),
                "odd");  // the name is the key, whatever the metadata says

    // The metadata's own name does not win over the key.
    oo::CacheManager::sharedCache()->setPList(
        Dict({{"key", Dict({{"script", oo::PList(oo::PList::Array{})},
                            {"!metadata!", Dict({{"name", Str("other")}})}})}}),
        "named.plist", kCacheName);
    const auto named = OOPListScript::scriptsInPListFile("named.plist");
    EXPECT_TRUE(named.has_value() && named->size() == 1 &&
             (*named)[0]->name().value_or("<none>") == "key");
  }
}

TEST(OOPListScript, files) {
  ::SetUp();
  {
    // No file (and nothing cached): none.
    EXPECT_TRUE(
        !OOPListScript::scriptsInPListFile((sRoot / "missing.plist").string())
             .has_value());

    // A file that is not a dictionary: none.
    WriteText(sRoot / "array.plist", "( a, b )");
    EXPECT_TRUE(
        !OOPListScript::scriptsInPListFile((sRoot / "array.plist").string())
             .has_value());

    // A dictionary with no script arrays: no scripts, and an empty dictionary
    // is cached for it.
    const std::string path = (sRoot / "empty.plist").string();
    WriteText(
        sRoot / "empty.plist",
        "{ \"!metadata!\" = { version = \"2\"; }; notAScript = \"text\"; }");
    const auto scripts = OOPListScript::scriptsInPListFile(path);
    EXPECT_TRUE(scripts.has_value() && scripts->empty());
    const oo::PList cached =
        oo::CacheManager::sharedCache()->pListForKey(path, kCacheName);
    EXPECT_TRUE(cached.isDict() && cached.count() == 0);
    // The second read comes from the cache (the file is gone).
    stdfs::remove(sRoot / "empty.plist");
    const auto again = OOPListScript::scriptsInPListFile(path);
    EXPECT_TRUE(again.has_value() && again->empty());
  }
}

// A plist script is its own object, held as std::shared_ptr<OOScript>.
TEST(OOPListScript, plistScriptIsItsOwnObject) {
  ::SetUp();
  {
    oo::CacheManager::sharedCache()->setPList(
        Dict({{"one", Dict({{"script", oo::PList(oo::PList::Array{})},
                            {"!metadata!", Dict({{"version", Str("3")}})}})}}),
        "facade.plist", kCacheName);
    const auto scripts = OOPListScript::scriptsInPListFile("facade.plist");
    EXPECT_TRUE(scripts.has_value() && scripts->size() == 1);
    if (!scripts.has_value() || scripts->size() != 1) return;
    OOPListScript* script = dynamic_cast<OOPListScript*>((*scripts)[0].get());
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;
    EXPECT_EQ(script->name().value_or("<none>"), "one");
    EXPECT_EQ(script->version().value_or("<none>"), "3");
    EXPECT_TRUE(script->requiresTickle());
    EXPECT_TRUE(script->description().starts_with("<OOPListScript 0x"));
    EXPECT_TRUE(OOScriptInObjectNode(OOScriptObjectNode(script)) ==
             script);  // it travels in plist data as itself
  }
}

TEST(OOPListScript, cleanUp) {
  stdfs::current_path(stdfs::temp_directory_path());
  std::error_code ignored;
  stdfs::remove_all(sRoot, ignored);
  EXPECT_TRUE(true);
}
