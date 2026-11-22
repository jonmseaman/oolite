/*	test_ResourceManager.mm
	Unit tests for ResourceManager (src/Core/ResourceManager.h).

	ResourceManager is class methods over file-scope state. These tests pin what its units answer:
	the state before any scan (no add-on selection, no errors, no OXP messages, no manifests),
	external paths, which make the search paths without a scan, the root paths (the built-in data,
	the managed add-ons, the user's add-on folders, as oo::ResourcePaths computes them), masking the
	user's name in paths, and resetting. They never scan for add-ons: that would read the machine's
	installed expansions, so the search paths are always given as external paths first.
	ResourceManager is a C++ class (static members), and the last test checks the C++ API.

	ResourceManager.mm reaches the universe, the HUD, the OXZ manager and the scripts, so the test
	links every game object but main's (tests/unit/core/meson.build entry ['*']) and defines
	gDebugFlags. UNIVERSE is nil. The tests run in a scratch game folder (see ScratchGameFolder) and
	add a scan of fixture add-ons whose manifests the scan accepts or rejects. Run: tests/run.sh unit
*/

#include <process.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Description.h"
#include "SystemDescriptionManager.h"
#include "ResourceManager.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Data.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/PListParsing.hpp"
#include "oofnd/ResourcePaths.hpp"
#include "oofnd/String.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

/*	Every folder ResourceManager reads is under a scratch game
   folder, made before main() and so before the first ResourceManager call (the
   root paths are computed once): the home (HOMEPATH), the managed and extract
   add-on folders, and the current directory, whose Resources folder holds only
   an Info-gnustep.plist (version 9.9.9) and whose AddOns folder holds only the
   fixtures the scan test writes. The machine's own add-ons are never read.
*/
struct ScratchGameFolder {
  ScratchGameFolder() {
    namespace stdfs = std::filesystem;
    const stdfs::path root =
        stdfs::temp_directory_path() /
        ("oo-test-resourcemanager-" +
         std::to_string(static_cast<unsigned long>(::_getpid())));
    stdfs::remove_all(root);
    stdfs::create_directories(root / "work" / "Resources");
    stdfs::create_directories(root / "work" / "AddOns");
    ::_putenv_s("HOMEPATH", root.string().c_str());
    ::_putenv_s("OO_MANAGEDADDONSDIR", (root / "managed").string().c_str());
    ::_putenv_s("OO_ADDONSEXTRACTDIR", (root / "extract").string().c_str());
    stdfs::current_path(root / "work");
    const std::string info = "{ CFBundleVersion = \"9.9.9\"; }";
    if (!oo::fs::writeFile(root / "work" / "Resources" / "Info-gnustep.plist",
                           oo::Data(info.data(), info.size()),
                           oo::fs::WriteMode::kDirect)) {
      std::fprintf(stderr,
                   "  could not write the scratch Info-gnustep.plist\n");
    }
  }
};
const ScratchGameFolder sScratchGameFolder;

// The user name the masking functions read, as they read it (the last component
// of the profile path).
std::string UserName() {
  const char* profile = std::getenv("USERPROFILE");
  return oo::str::lastPathComponent(oo::str::format("%s", profile));
}

}  // namespace

TEST(ResourceManager, stateBeforeAnyScan) {
  {
    EXPECT_TRUE(ResourceManager::useAddOns() == std::nullopt);
    EXPECT_TRUE(ResourceManager::errors() == std::nullopt);
    EXPECT_TRUE(ResourceManager::OXPsWithMessagesFound().empty());
    EXPECT_TRUE(ResourceManager::manifestForIdentifier("org.test.none").isNull());
    ResourceManager::clearCaches();
  }
}

TEST(ResourceManager, rootPathsAreTheResourcePaths) {
  {
    const oo::ResourcePaths paths = oo::ResourcePaths::current();
    const std::string builtIn =
        oo::fs::utf8String(paths.builtInResourcesDirectory());
    EXPECT_TRUE(ResourceManager::builtInPath() ==
             std::optional<std::string>(builtIn));

    std::vector<std::string> userRoots;
    for (const oo::fs::Path& path : paths.userRootDirectories())
      userRoots.push_back(oo::fs::utf8String(path));
    EXPECT_TRUE(ResourceManager::userRootPaths() == userRoots);

    const std::vector<std::string> roots = ResourceManager::rootPaths();
    EXPECT_EQ(roots.size(), userRoots.size() + 2);
    EXPECT_TRUE(roots.size() >= 2 && roots[0] == builtIn);
    EXPECT_TRUE(roots.size() >= 2 &&
             roots[1] == oo::fs::utf8String(paths.managedAddOnsDirectory()));
    EXPECT_TRUE(roots.size() >= 2 &&
             std::vector<std::string>(roots.begin() + 2, roots.end()) ==
                 userRoots);
    EXPECT_TRUE(ResourceManager::rootPaths() == roots);  // computed once
  }
}

TEST(ResourceManager, maskUserName) {
  {
    EXPECT_TRUE(ResourceManager::maskUserName("jon", "C:/Users/jon/AddOns") ==
             std::optional<std::string>("C:/Users/*/AddOns"));
    EXPECT_TRUE(ResourceManager::maskUserName("jon", "/home/jon/jon.oxp") ==
             std::optional<std::string>("/home/*/*.oxp"));
    EXPECT_TRUE(ResourceManager::maskUserName("jon", "/srv/oolite") ==
             std::optional<std::string>("/srv/oolite"));
    EXPECT_TRUE(ResourceManager::maskUserName("jon", "") ==
             std::optional<std::string>(""));

    // The array form masks the name of the user running the game, in every
    // path.
    const std::string user = UserName();
    if (!user.empty()) {
      const std::vector<std::string> masked =
          ResourceManager::maskUserNameInPathArray(
              {"/a/" + user + "/b", "/c/d"});
      EXPECT_TRUE(masked == (std::vector<std::string>{"/a/*/b", "/c/d"}));
    }
    EXPECT_TRUE(ResourceManager::maskUserNameInPathArray({}).empty());
  }
}

TEST(ResourceManager, externalPathsAreTheSearchPathsWithoutAScan) {
  {
    ResourceManager::addExternalPath("ext/one.oxp");
    ResourceManager::addExternalPath("ext/two.oxp");
    ResourceManager::addExternalPath("ext/one.oxp");  // already there
    const std::vector<std::string> expected{"ext/one.oxp", "ext/two.oxp"};
    EXPECT_TRUE(ResourceManager::paths() == expected);
    EXPECT_TRUE(ResourceManager::pathsWithAddOns() == expected);
    EXPECT_TRUE(ResourceManager::useAddOns() ==
             std::nullopt);  // no scan, so no selection

    // -reset forgets them (the next -paths would scan again, which the test
    // does not do).
    ResourceManager::reset();
    EXPECT_TRUE(ResourceManager::useAddOns() == std::nullopt);
    EXPECT_TRUE(ResourceManager::errors() == std::nullopt);
    ResourceManager::addExternalPath("ext/three.oxp");
    EXPECT_TRUE(ResourceManager::paths() ==
             std::vector<std::string>{"ext/three.oxp"});
    ResourceManager::reset();
  }
}

TEST(ResourceManager, cxxAPIAnswersTheSame) {
  {
    // The static members.
    EXPECT_TRUE(ResourceManager::builtInPath() == ResourceManager::builtInPath());
    EXPECT_TRUE(ResourceManager::rootPaths() == ResourceManager::rootPaths());
    EXPECT_TRUE(ResourceManager::userRootPaths() ==
             ResourceManager::userRootPaths());
    EXPECT_TRUE(ResourceManager::maskUserName("jon", "/home/jon") ==
             std::optional<std::string>("/home/*"));
    EXPECT_TRUE(ResourceManager::useAddOns() == std::nullopt);
    EXPECT_TRUE(ResourceManager::errors() == std::nullopt);
    EXPECT_TRUE(ResourceManager::manifestForIdentifier("org.test.none").isNull());
    EXPECT_TRUE(ResourceManager::OXPsWithMessagesFound().empty());
    ResourceManager::addExternalPath("ext/cxx.oxp");
    EXPECT_TRUE(ResourceManager::paths() ==
             std::vector<std::string>{"ext/cxx.oxp"});
    EXPECT_TRUE(ResourceManager::paths() ==
             std::vector<std::string>{"ext/cxx.oxp"});
    EXPECT_TRUE(ResourceManager::pathsWithAddOns() ==
             std::vector<std::string>{"ext/cxx.oxp"});
    ResourceManager::reset();
    ResourceManager::clearCaches();
  }
}

// --- OXP manifests: validation, requirements,
// conflicts, dependencies, scenarios --- A scan of a scratch game folder whose
// only add-ons are fixtures the test writes: every root the scan reads (the
// built-in data, the managed, extract and additional add-on folders,
// <cwd>/AddOns) is under the scratch folder, set before the first
// ResourceManager call by ScratchGameFolder.

namespace {

void WriteText(const std::filesystem::path& path, const std::string& text) {
  std::filesystem::create_directories(path.parent_path());
  EXPECT_TRUE(oo::fs::writeFile(path, oo::Data(text.data(), text.size()),
                             oo::fs::WriteMode::kDirect)
               .has_value());
}

oo::PList Manifest(const char* text) {
  auto result = oo::parsePropertyList(text);
  EXPECT_TRUE(result.has_value());
  return result ? *result : oo::PList();
}

bool EndsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() &&
         s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}  // namespace

TEST(ResourceManager, versionsAndCompatibility) {
  {
    const oo::PList range =
        Manifest("{ version = \"1.0\"; maximum_version = \"2.0\"; }");
    EXPECT_TRUE(ResourceManager::matchVersions(range, "1.5"));
    EXPECT_TRUE(ResourceManager::matchVersions(range, "1.0"));
    EXPECT_TRUE(ResourceManager::matchVersions(range, "2.0"));
    EXPECT_TRUE(!ResourceManager::matchVersions(range, "0.9"));
    EXPECT_TRUE(!ResourceManager::matchVersions(range, "2.0.1"));
    EXPECT_TRUE(ResourceManager::matchVersions(Manifest("{}"), ""));
    EXPECT_TRUE(
        !ResourceManager::matchVersions(Manifest("{ version = \"1\"; }"), ""));

    // The game's version is the scratch Info-gnustep.plist's, 9.9.9.
    EXPECT_TRUE(ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"1.80\"; }"), "t"));
    EXPECT_TRUE(!ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"10.0\"; }"), "t"));
    EXPECT_TRUE(!ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"1.0\"; maximum_oolite_version "
                 "= \"9.0\"; }"),
        "t"));
    EXPECT_TRUE(ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"1.0\"; maximum_oolite_version "
                 "= \"\"; }"),
        std::nullopt));
    EXPECT_TRUE(ResourceManager::checkVersionCompatibility(
        Manifest("{}"), "t"));  // nothing required
  }
}

TEST(ResourceManager, scanKeepsTheAddOnsWhoseManifestsAllowThem) {
  {
    const std::filesystem::path addOns =
        std::filesystem::current_path() / "AddOns";
    WriteText(
        addOns / "a.oxp" / "manifest.plist",
        "{ identifier = \"org.test.a\"; version = \"1.0\"; "
        "required_oolite_version = \"1.0\"; title = A; tags = (alpha); }");
    WriteText(addOns / "a.oxp" / "OXPMessages.plist", "( \"hello from a\" )");
    WriteText(addOns / "b.oxp" / "manifest.plist",
              "{ identifier = \"org.test.b\"; version = \"1.0\"; "
              "required_oolite_version = \"1.0\"; title = B; requires_oxps = ( "
              "{ identifier = \"org.test.a\"; version = \"2.0\"; } ); }");
    WriteText(addOns / "c.oxp" / "manifest.plist",
              "{ identifier = \"org.test.c\"; version = \"1.0\"; "
              "required_oolite_version = \"1.0\"; title = C; conflict_oxps = ( "
              "{ identifier = \"org.test.a\"; } ); }");
    WriteText(addOns / "d.oxp" / "manifest.plist",
              "{ identifier = \"org.test.d\"; version = \"1.0\"; "
              "required_oolite_version = \"99.0\"; title = D; }");
    WriteText(addOns / "e.oxp" / "manifest.plist",
              "{ identifier = \"org.test.e\"; version = \"1.0\"; "
              "required_oolite_version = \"1.0\"; }");
    WriteText(addOns / "f.oxp" / "manifest.plist",
              "{ identifier = \"org.test.f\"; version = \"1.0\"; "
              "required_oolite_version = \"1.0\"; title = F; tags = "
              "(\"oolite-scenario-only\"); }");
    WriteText(addOns / "g.oxp" / "requires.plist", "{ version = \"99\"; }");
    WriteText(addOns / "h.oxp" / "manifest.plist",
              "{ identifier = \"org.test.h\"; version = \"3.0\"; "
              "required_oolite_version = \"1.0\"; title = H; requires_oxps = ( "
              "{ identifier = \"org.test.a\"; } ); }");

    ResourceManager::reset();
    const std::vector<std::string> paths = ResourceManager::paths();
    for (const std::string& path : paths)
      std::printf("  search path: %s\n", path.c_str());
    bool sawA = false, sawH = false, sawOther = false;
    for (const std::string& path : paths) {
      if (EndsWith(path, "a.oxp"))
        sawA = true;
      else if (EndsWith(path, "h.oxp"))
        sawH = true;
      else if (EndsWith(path, ".oxp"))
        sawOther = true;
    }
    EXPECT_TRUE(sawA && sawH && !sawOther);
    EXPECT_TRUE(ResourceManager::useAddOns() ==
             std::optional<std::string>(""));  // the scan chose "all"

    const oo::PList a = ResourceManager::manifestForIdentifier("org.test.a");
    EXPECT_TRUE(a.isDict() && EndsWith(a.get<std::string>("file_path"), "a.oxp"));
    EXPECT_TRUE(a.find("required_by") != nullptr &&
             a.find("required_by")->count() == 1);  // h requires it
    EXPECT_TRUE(ResourceManager::manifestForIdentifier("org.test.b").isNull());
    EXPECT_TRUE(ResourceManager::manifestForIdentifier("org.test.c").isNull());
    EXPECT_TRUE(ResourceManager::manifestForIdentifier("org.test.f").isNull());
    EXPECT_TRUE(ResourceManager::OXPsWithMessagesFound() ==
             std::vector<std::string>{"a.oxp"});

    // With a in the list, c's conflict and b's requirement are seen again (not
    // logged).
    const oo::PList c = Manifest(
        "{ identifier = \"org.test.c\"; title = C; conflict_oxps = ( { "
        "identifier = \"org.test.a\"; } ); }");
    EXPECT_TRUE(ResourceManager::manifestHasConflicts(c, false));
    EXPECT_TRUE(!ResourceManager::manifestHasConflicts(
        Manifest("{ conflict_oxps = ( { identifier = \"org.test.zz\"; } ); }"),
        false));
    const oo::PList b = Manifest(
        "{ identifier = \"org.test.b\"; title = B; requires_oxps = ( { "
        "identifier = \"org.test.a\"; version = \"2.0\"; } ); }");
    EXPECT_TRUE(ResourceManager::manifestHasMissingDependencies(b, false));
    EXPECT_TRUE(ResourceManager::manifest(
        b, Manifest("{ identifier = \"org.test.a\"; version = \"2.0\"; }"),
        false));
    EXPECT_TRUE(!ResourceManager::manifest(
        b, Manifest("{ identifier = \"org.test.a\"; }"), false));
    EXPECT_TRUE(
        !ResourceManager::manifestHasMissingDependencies(Manifest("{}"), false));

    // The rejections were recorded; with no universe their texts are empty, and
    // reading them clears them.
    const std::optional<std::string> errors = ResourceManager::errors();
    EXPECT_TRUE(errors == std::optional<std::string>(""));
    EXPECT_TRUE(ResourceManager::errors() == std::nullopt);

    ResourceManager::reset();
  }
}

TEST(ResourceManager, cxxSlice2API) {
  {
    const oo::PList range =
        Manifest("{ version = \"1.0\"; maximum_version = \"2.0\"; }");
    EXPECT_TRUE(ResourceManager::matchVersions(range, "1.5"));
    EXPECT_TRUE(!ResourceManager::matchVersions(range, "3"));
    EXPECT_TRUE(ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"1.0\"; }"), "t"));
    EXPECT_TRUE(!ResourceManager::checkVersionCompatibility(
        Manifest("{ required_oolite_version = \"10.0\"; }"), std::nullopt));
    EXPECT_TRUE(!ResourceManager::manifestHasConflicts(Manifest("{}"), false));
    EXPECT_TRUE(!ResourceManager::manifestHasMissingDependencies(Manifest("{}"),
                                                              false));
    EXPECT_TRUE(ResourceManager::manifest(
        Manifest("{ title = X; }"),
        Manifest("{ identifier = \"org.test.none\"; }"), false));
    EXPECT_TRUE(ResourceManager::matchVersions(range, "1.5") ==
             ResourceManager::matchVersions(range, "1.5"));
  }
}

// --- Plist loading and merging, the configuration
// dictionaries --- Over the scratch game folder of the scan test: the
// built-in data and two fixture add-ons, a.oxp and h.oxp (both kept by the
// scan), each with Config plists that the test writes. No cache: every load is
// asked with the cache off, or for a file name no other test loads.

TEST(ResourceManager, mergedPlistsFromTheSearchPaths) {
  {
    namespace stdfs = std::filesystem;
    const stdfs::path work = stdfs::current_path();
    WriteText(work / "Resources" / "Config" / "oo-test-dict.plist",
              "{ base = 1; shared = { x = 1; y = 1; }; list = (a); }");
    WriteText(work / "AddOns" / "a.oxp" / "Config" / "oo-test-dict.plist",
              "{ fromA = 2; shared = { y = 2; z = 2; }; list = (b); }");
    WriteText(work / "AddOns" / "h.oxp" / "Config" / "oo-test-dict.plist",
              "{ fromH = 3; }");
    WriteText(work / "Resources" / "Config" / "oo-test-array.plist",
              "( one, two )");
    WriteText(work / "AddOns" / "a.oxp" / "Config" / "oo-test-array.plist",
              "( three )");
    WriteText(work / "Resources" / "Config" / "role-categories.plist",
              "{ oolite-trader = ( trader, \"trader-courier\" ); }");
    WriteText(work / "AddOns" / "a.oxp" / "Config" / "role-categories.plist",
              "{ oolite-trader = ( trader, \"a-trader\" ); oolite-test = ( "
              "tester ); }");
    WriteText(work / "Resources" / "Config" / "pirate-victim-roles.plist",
              "( trader, courier )");
    WriteText(work / "Resources" / "Config" / "logcontrol.plist",
              "{ \"oo-test.base\" = yes; \"oo-test.overridden\" = no; }");
    WriteText(work / "AddOns" / "a.oxp" / "Config" / "logcontrol.plist",
              "{ \"oo-test.overridden\" = yes; \"oo-test.fromA\" = no; }");

    ResourceManager::reset();
    const std::vector<std::string> paths = ResourceManager::paths();
    EXPECT_TRUE(paths.size() >= 2);

    // OOResourceMergeMode::kNone: the last file in search order; OOResourceMergeMode::kBasic: top-level keys;
    // OOResourceMergeMode::kSmart: second level.
    const oo::PList last = ResourceManager::dictionaryFromFilesNamed(
        "oo-test-dict.plist", std::string("Config"), oo::ResourceMergeMode::kNone, false);
    std::printf("  MERGE_NONE: %s\n", oo::DescriptionOf(last).c_str());
    const oo::PList basic = ResourceManager::dictionaryFromFilesNamed(
        "oo-test-dict.plist", std::string("Config"), oo::ResourceMergeMode::kBasic, false);
    std::printf("  MERGE_BASIC: %s\n", oo::DescriptionOf(basic).c_str());
    const oo::PList smart = ResourceManager::dictionaryFromFilesNamed(
        "oo-test-dict.plist", std::string("Config"), oo::ResourceMergeMode::kSmart, false);
    std::printf("  MERGE_SMART: %s\n", oo::DescriptionOf(smart).c_str());
    EXPECT_TRUE(basic.find("base") != nullptr && basic.find("fromA") != nullptr &&
             basic.find("fromH") != nullptr);
    EXPECT_TRUE(smart.find("shared") != nullptr &&
             smart.find("shared")->find("x") != nullptr &&
             smart.find("shared")->find("z") != nullptr);
    EXPECT_TRUE(ResourceManager::dictionaryFromFilesNamed(
                 "oo-test-none.plist", std::string("Config"), true)
                 .isNull());

    const oo::PList array = ResourceManager::arrayFromFilesNamed(
        "oo-test-array.plist", std::string("Config"), true, false);
    std::printf("  array merged: %s\n", oo::DescriptionOf(array).c_str());
    EXPECT_TRUE(array.isArray());
    const oo::PList arrayLast = ResourceManager::arrayFromFilesNamed(
        "oo-test-array.plist", std::string("Config"), false, false);
    std::printf("  array last: %s\n", oo::DescriptionOf(arrayLast).c_str());
    EXPECT_TRUE(ResourceManager::arrayFromFilesNamed("oo-test-none.plist",
                                                  std::string("Config"), true)
                 .isNull());

    // The built-in path is not excluded for a core plist; an add-on is, unless
    // it overrides it.
    EXPECT_TRUE(!ResourceManager::corePlist("oo-test-dict.plist",
                                         *ResourceManager::builtInPath()));

    const oo::PList roles = ResourceManager::roleCategoriesDictionary();
    std::printf("  role categories: %s\n", oo::DescriptionOf(roles).c_str());
    EXPECT_TRUE(roles.isDict() && roles.find("oolite-trader") != nullptr &&
             roles.find("oolite-test") != nullptr);

    const oo::PList logControl = ResourceManager::logControlDictionary();
    EXPECT_TRUE(logControl.isDict());
    std::printf(
        "  log control oo-test.*: base=%s overridden=%s fromA=%s\n",
        logControl.find("oo-test.base")
            ? oo::DescriptionOf(*logControl.find("oo-test.base")).c_str()
            : "-",
        logControl.find("oo-test.overridden")
            ? oo::DescriptionOf(*logControl.find("oo-test.overridden")).c_str()
            : "-",
        logControl.find("oo-test.fromA")
            ? oo::DescriptionOf(*logControl.find("oo-test.fromA")).c_str()
            : "-");

    EXPECT_TRUE(ResourceManager::whitelistDictionary()
                 .isNull());  // no whitelist.plist in the scratch data
  }
}

TEST(ResourceManager, equipmentAndStarListMerging) {
  {
    // Equipment lists: an entry of an earlier file whose key (index 3) a later
    // file also has is replaced by the later one.
    oo::PList lists(oo::PList::Array{
        oo::PList(oo::PList::Array{
            oo::PList(oo::PList::Array{oo::PList(1), oo::PList(10),
                                       oo::PList("Old"), oo::PList("EQ_A")}),
            oo::PList(oo::PList::Array{oo::PList(1), oo::PList(10),
                                       oo::PList("Keep"), oo::PList("EQ_B")})}),
        oo::PList(oo::PList::Array{oo::PList(
            oo::PList::Array{oo::PList(2), oo::PList(20), oo::PList("New"),
                             oo::PList("EQ_A")})})});
    ResourceManager::handleEquipmentListMerging(lists, 3);
    std::printf("  equipment lists merged: %s\n",
                oo::DescriptionOf(lists).c_str());
    EXPECT_TRUE(lists.at(0) != nullptr && lists.at(0)->at(0) != nullptr &&
             lists.at(0)->at(0)->at<std::string>(2) == "New");
    EXPECT_TRUE(lists.at(0) != nullptr && lists.at(0)->at(1) != nullptr &&
             lists.at(0)->at(1)->at<std::string>(2) == "Keep");

    // Star and nebula lists: the same by the entry itself (or a dictionary's
    // key / texture).
    oo::PList stars(oo::PList::Array{
        oo::PList(oo::PList::Array{
            oo::PList("star1.png"),
            oo::PList(oo::PList::Dict{{"key", oo::PList("k")},
                                      {"texture", oo::PList("old.png")}})}),
        oo::PList(oo::PList::Array{oo::PList(oo::PList::Dict{
            {"key", oo::PList("k")}, {"texture", oo::PList("new.png")}})})});
    ResourceManager::handleStarNebulaListMerging(stars);
    std::printf("  star lists merged: %s\n", oo::DescriptionOf(stars).c_str());
    EXPECT_TRUE(stars.at(0) != nullptr && stars.at(0)->at(1) != nullptr &&
             stars.at(0)->at(1)->get<std::string>("texture") == "new.png");

    // No equipment-overrides.plist: the lists are unchanged.
    oo::PList equipment(oo::PList::Array{oo::PList(oo::PList::Array{
        oo::PList(1), oo::PList(10), oo::PList("Name"), oo::PList("EQ_A")})});
    const oo::PList before = equipment;
    ResourceManager::handleEquipmentOverrides(equipment);
    EXPECT_TRUE(oo::DescriptionOf(equipment) == oo::DescriptionOf(before));
  }
}

TEST(ResourceManager, cxxSlice3API) {
  {
    EXPECT_TRUE(ResourceManager::dictionaryFromFilesNamed("oo-test-dict.plist",
                                                       std::string("Config"),
                                                       oo::ResourceMergeMode::kBasic, false)
                 .find("fromA") != nullptr);
    EXPECT_TRUE(ResourceManager::dictionaryFromFilesNamed(
                 "oo-test-none.plist", std::string("Config"), true)
                 .isNull());
    EXPECT_TRUE(ResourceManager::arrayFromFilesNamed(
                 "oo-test-array.plist", std::string("Config"), true, false)
                 .isArray());
    EXPECT_TRUE(!ResourceManager::corePlist("oo-test-dict.plist",
                                         *ResourceManager::builtInPath()));
    EXPECT_TRUE(ResourceManager::whitelistDictionary().isNull());
    EXPECT_TRUE(ResourceManager::roleCategoriesDictionary().isDict());
    EXPECT_TRUE(ResourceManager::logControlDictionary().isDict());
  }
}

// --- File lookup, sounds, music, strings, scripts, diagnostics --- Over the
// same scratch game folder.

TEST(ResourceManager, singleFileLookups) {
  {
    namespace stdfs = std::filesystem;
    const stdfs::path work = stdfs::current_path();
    WriteText(work / "Resources" / "Config" / "oo-test-text.txt",
              "hello, built-in");
    WriteText(work / "AddOns" / "a.oxp" / "Config" / "oo-test-text.txt",
              "hello, a.oxp");
    WriteText(work / "Resources" / "oo-test-loose.txt", "loose");
    ResourceManager::reset();
    (void)ResourceManager::paths();

    // The last search path that has the file (an add-on overrides the built-in
    // data), with the folder, else without it; nullopt when no path has it.
    const std::optional<std::string> path = ResourceManager::pathForFileNamed(
        "oo-test-text.txt", std::string("Config"), false);
    EXPECT_TRUE(path.has_value() &&
             EndsWith(*path, "a.oxp/Config/oo-test-text.txt"));
    const std::optional<std::string> loose = ResourceManager::pathForFileNamed(
        "oo-test-loose.txt", std::string("Config"), false);
    EXPECT_TRUE(loose.has_value() &&
             EndsWith(*loose, "Resources/oo-test-loose.txt"));
    EXPECT_TRUE(ResourceManager::pathForFileNamed(
                 "oo-test-missing.txt", std::string("Config")) == std::nullopt);
    EXPECT_TRUE(ResourceManager::pathForFileNamed("oo-test-text.txt", std::nullopt,
                                               false) == std::nullopt);

    EXPECT_TRUE(ResourceManager::stringFromFilesNamed("oo-test-text.txt",
                                                   std::string("Config")) ==
             std::optional<std::string>("hello, a.oxp"));
    EXPECT_TRUE(ResourceManager::stringFromFilesNamed("oo-test-missing.txt",
                                                   std::string("Config"),
                                                   false) == std::nullopt);

    // No such sound or music file: nil.
    EXPECT_TRUE(ResourceManager::ooSoundNamed("oo-test-missing.ogg",
                                           std::string("Sounds")) == nullptr);
    EXPECT_TRUE(!ResourceManager::ooMusicNamed("oo-test-missing.ogg",
                                            std::string("Music")));

    // No shader-uniform-bindings.plist and no material-defaults.plist in the
    // scratch data.
    EXPECT_TRUE(ResourceManager::shaderBindingTypesDictionary().isNull());
    EXPECT_TRUE(ResourceManager::materialDefaults().isNull());
  }
}

TEST(ResourceManager, scriptsAndSystemsAndDiagnostics) {
  {
    // No add-on has world scripts.
    EXPECT_TRUE(ResourceManager::loadScripts().empty());

    // No planetinfo.plist: a manager with nothing set, made anew on each call.
    std::shared_ptr<oo::SystemDescriptionManager> manager =
        ResourceManager::systemDescriptionManager();
    EXPECT_TRUE(manager != nullptr);
    EXPECT_TRUE(manager != ResourceManager::systemDescriptionManager());

    // Diagnostics go under the log folder, which is under the scratch home.
    const std::optional<std::string> logs =
        ResourceManager::diagnosticFileLocation();
    std::printf("  diagnostics at: %s\n", logs ? logs->c_str() : "(none)");
    if (logs.has_value()) {
      EXPECT_TRUE(ResourceManager::writeDiagnosticString(
          "diagnostic text", "oo-test/.hidden/note.txt"));
      const auto written = oo::fs::readFile(oo::fs::pathFromUTF8(*logs) /
                                            "oo-test" / "!hidden" / "note.txt");
      EXPECT_TRUE(written.has_value() &&
               std::string(written->stringView()) == "diagnostic text");
      EXPECT_TRUE(ResourceManager::writeDiagnosticPList(
          oo::PList(oo::PList::Dict{{"k", oo::PList("v")}}),
          "oo-test-plist.plist"));
    }
  }
}

TEST(ResourceManager, cxxSlice4API) {
  {
    EXPECT_TRUE(ResourceManager::stringFromFilesNamed("oo-test-text.txt",
                                                   std::string("Config")) ==
             std::optional<std::string>("hello, a.oxp"));
    EXPECT_TRUE(ResourceManager::pathForFileNamed("oo-test-missing.txt",
                                               std::string("Config"),
                                               false) == std::nullopt);
    EXPECT_TRUE(ResourceManager::ooSoundNamed("oo-test-missing.ogg",
                                           std::string("Sounds")) == nullptr);
    EXPECT_TRUE(ResourceManager::shaderBindingTypesDictionary().isNull());
    EXPECT_TRUE(ResourceManager::loadScripts().empty());
    EXPECT_TRUE(ResourceManager::systemDescriptionManager().get() != nullptr);
    EXPECT_TRUE(ResourceManager::diagnosticFileLocation() ==
             ResourceManager::diagnosticFileLocation());
    // A new C++ manager.
    std::shared_ptr<oo::SystemDescriptionManager> manager =
        ResourceManager::systemDescriptionManager();
    EXPECT_TRUE(manager != nullptr);
  }
}

/*	ooSoundNamed() goes through the sound cache. A sound
   found is kept by the cache and answered borrowed; a missing one is nil.
   Neither reaches retrieveFileNamed()'s uncached oo::autorelease with an
   object, so nothing waits for a scope's drain, and nothing is counted as
        autoreleased without a scope, whether or not a scope is open.
*/
TEST(ResourceManager, soundLookupHandsNothingToTheScope) {
  const std::size_t leaked = oo::AutoreleaseScope::leakedWithoutScope();
  {
    oo::AutoreleaseScope scope;
    EXPECT_TRUE(ResourceManager::ooSoundNamed("oo-test-missing.ogg",
                                           std::string("Sounds")) == nullptr);
    EXPECT_TRUE(scope.pendingCount() == 0);
  }
  EXPECT_TRUE(oo::AutoreleaseScope::current() == nullptr);
  EXPECT_TRUE(ResourceManager::ooSoundNamed("oo-test-missing.ogg",
                                         std::string("Sounds")) == nullptr);
  EXPECT_TRUE(oo::AutoreleaseScope::leakedWithoutScope() == leaked);
}

// --- <name>.json5 where <name>.plist is asked for --- Over the same scratch
// game folder: built-in data in JSON5 merges with an add-on's plist of the same
// name, and a single file is found by its .plist name.

TEST(ResourceManager, json5FilesStandInForPlists) {
  namespace stdfs = std::filesystem;
  const stdfs::path work = stdfs::current_path();
  WriteText(work / "Resources" / "Config" / "oo-test-json5.json5",
            "{ base: 1, shared: { x: 1, y: 1 } }");
  WriteText(work / "AddOns" / "a.oxp" / "Config" / "oo-test-json5.plist",
            "{ fromA = 2; shared = { y = 2; z = 2; }; }");
  WriteText(work / "Resources" / "AIs" / "oo-test-json5AI.json5",
            "{ GLOBAL: { ENTER: [] } }");
  ResourceManager::reset();
  (void)ResourceManager::paths();

  const oo::PList smart = ResourceManager::dictionaryFromFilesNamed(
      "oo-test-json5.plist", std::string("Config"), oo::ResourceMergeMode::kSmart, false);
  std::printf("  JSON5 + plist, MERGE_SMART: %s\n",
              oo::DescriptionOf(smart).c_str());
  EXPECT_TRUE(smart.find("base") != nullptr && smart.find("fromA") != nullptr);
  EXPECT_TRUE(smart.find("shared") != nullptr &&
           smart.find("shared")->find("x") != nullptr &&
           smart.find("shared")->find("z") != nullptr);

  const std::optional<std::string> ai = ResourceManager::pathForFileNamed(
      "oo-test-json5AI.plist", std::string("AIs"), false);
  EXPECT_TRUE(ai.has_value() &&
           EndsWith(*ai, "Resources/AIs/oo-test-json5AI.json5"));
  EXPECT_TRUE(ResourceManager::pathForFileNamed("oo-test-json5-missing.plist",
                                             std::string("AIs"),
                                             false) == std::nullopt);
}
