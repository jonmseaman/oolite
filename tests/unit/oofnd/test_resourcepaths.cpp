/*	test_resourcepaths.cpp
        Unit tests for oofnd/ResourcePaths.hpp: oo::ResourcePaths,
   checked against the paths the game computes
   (NSHomeDirectory() under GNUstep, SDL/main.mm's HOMEPATH,
   NSFileManagerOOExtensions, OOLogOutputHandler, OOOXZManager,
   ResourceManager), for both the Windows and the POSIX rules, driven by a fake
   environment.
*/

#include <map>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include <gtest/gtest.h>

#include "oofnd/ResourcePaths.hpp"

namespace {

namespace fs = oo::fs;
using Platform = oo::PathEnvironment::Platform;

// Paths compare element-wise: "a/b" and "a\\b" are the same path on Windows,
// and lexically_normal() folds the "x/../y" the game's paths also leave
// in.
bool same(const fs::Path& a, const fs::Path& b) {
  return a.lexically_normal() == b.lexically_normal();
}

oo::ResourcePaths make(Platform platform,
                       std::map<std::string, std::string> vars, fs::Path cwd,
                       bool userFolder = false) {
  oo::PathEnvironment env;
  env.platform = platform;
  env.getenv = [vars](const std::string& name) -> std::optional<std::string> {
    auto it = vars.find(name);
    if (it == vars.end()) return std::nullopt;
    return it->second;
  };
  env.currentDirectory = std::move(cwd);
  env.gameDataToUserFolder = userFolder;
  return oo::ResourcePaths(std::move(env));
}

const std::string kExeDir = "C:/Games/Oolite/oolite.app";

// What SDL/main.mm leaves in the environment on Windows: HOMEPATH is the
// executable's directory.
oo::ResourcePaths windowsGame(std::map<std::string, std::string> extra = {}) {
  std::map<std::string, std::string> vars{
      {"HOMEPATH", kExeDir},
      {"HOMEDRIVE", "C:"},
      {"USERPROFILE", "C:/Users/commander"},
      {"LOCALAPPDATA", "C:/Users/commander/AppData/Local"},
  };
  for (auto& [k, v] : extra) vars[k] = v;
  return make(Platform::kWindows, vars, fs::pathFromUTF8(kExeDir));
}

}  // namespace

TEST(resourcepaths, windowsHomeIsHomepathWhenItHasADrive) {
  EXPECT_TRUE(same(windowsGame().homeDirectory(), "C:/Games/Oolite/oolite.app"));
  // Spelled as GNUstep spells NSHomeDirectory() (probed on gnustep-base 1.31),
  // even from a backslashed HOMEPATH.
  auto p = make(Platform::kWindows, {{"HOMEPATH", "C:\\Games\\X"}}, "C:/x");
  EXPECT_EQ(fs::utf8String(p.homeDirectory()), std::string("C:/Games/X"));
  EXPECT_EQ(fs::utf8String(p.saveDirectory()),
              std::string("C:/Games/X/oolite-saves"));
  EXPECT_EQ(fs::utf8String(p.applicationSupportDirectory()),
              std::string("C:/Games/X/GNUstep/Library/ApplicationSupport"));
}

TEST(resourcepaths, windowsHomeGetsHomedrivePrefixedWhenHomepathHasNoDrive) {
  auto p =
      make(Platform::kWindows,
           {{"HOMEPATH", "\\Users\\commander"}, {"HOMEDRIVE", "D:"}}, "C:/x");
  EXPECT_EQ(fs::utf8String(p.homeDirectory()),
              std::string("D:/Users/commander"));  // GNUstep's spelling
}

TEST(resourcepaths, windowsHomeFallsBackToUserprofileThenCwd) {
  EXPECT_TRUE(same(make(Platform::kWindows, {{"USERPROFILE", "C:/Users/c"}}, "C:/x")
                    .homeDirectory(),
                "C:/Users/c"));
  EXPECT_TRUE(same(make(Platform::kWindows, {}, "C:/x").homeDirectory(), "C:/x"));
}

TEST(resourcepaths, posixHomeIsHomeThenCwd) {
  EXPECT_TRUE(same(make(Platform::kPosix,
                     {{"HOME", "/home/c"}, {"HOMEPATH", "/ignored"}}, "/cwd")
                    .homeDirectory(),
                "/home/c"));
  EXPECT_TRUE(same(make(Platform::kPosix, {}, "/cwd").homeDirectory(), "/cwd"));
}

TEST(resourcepaths, windowsUserDirectoriesLiveNextToTheExecutable) {
  const auto p = windowsGame();
  EXPECT_TRUE(same(p.saveDirectory(), kExeDir + "/oolite-saves"));
  EXPECT_TRUE(same(p.snapshotDirectory(), kExeDir + "/oolite-saves/snapshots"));
  EXPECT_TRUE(same(p.logDirectory(), kExeDir + "/Logs"));
  EXPECT_TRUE(same(p.userLibraryDirectory(), kExeDir + "/GNUstep/Library"));
  EXPECT_TRUE(same(p.cachesDirectory(), kExeDir + "/GNUstep/Library/Caches"));
  EXPECT_TRUE(same(p.applicationSupportDirectory(),
                kExeDir + "/GNUstep/Library/ApplicationSupport"));
  EXPECT_TRUE(same(
      p.managedAddOnsDirectory(),
      kExeDir + "/GNUstep/Library/ApplicationSupport/Oolite/ManagedAddOns"));
}

TEST(resourcepaths, windowsExtractDirectoryIsRelativeUnlessGameDataGoesToTheUserFolder) {
  EXPECT_EQ(fs::utf8String(windowsGame().extractAddOnsDirectory()),
              std::string("../AddOns"));
  auto userFolder =
      make(Platform::kWindows, {{"LOCALAPPDATA", "C:/Users/c/AppData/Local"}},
           "C:/x", true);
  EXPECT_TRUE(same(userFolder.extractAddOnsDirectory(),
                "C:/Users/c/AppData/Local/Oolite/AddOns"));
}

TEST(resourcepaths, posixUserDirectories) {
  const auto p =
      make(Platform::kPosix, {{"HOME", "/home/c"}}, "/opt/oolite/bin");
  EXPECT_TRUE(same(p.saveDirectory(), "/home/c/oolite-saves"));
  EXPECT_TRUE(same(p.snapshotDirectory(), "/home/c/oolite-saves/snapshots"));
  EXPECT_TRUE(same(p.logDirectory(), "/home/c/.Oolite/Logs"));
  EXPECT_TRUE(same(p.extractAddOnsDirectory(), "/home/c/.Oolite/AddOns"));
  EXPECT_TRUE(
      same(p.managedAddOnsDirectory(),
           "/home/c/GNUstep/Library/ApplicationSupport/Oolite/ManagedAddOns"));
}

TEST(resourcepaths, environmentOverridesWinVerbatim) {
  const auto p = windowsGame({
      {"OO_SAVEDIR", "E:/saves"},
      {"OO_SNAPSHOTSDIR", "E:/shots"},
      {"OO_LOGSDIR", "E:/logs"},
      {"OO_MANAGEDADDONSDIR", "E:/managed"},
      {"OO_ADDONSEXTRACTDIR", "E:/extract"},
  });
  EXPECT_EQ(fs::utf8String(p.saveDirectory()), std::string("E:/saves"));
  EXPECT_EQ(fs::utf8String(p.snapshotDirectory()), std::string("E:/shots"));
  EXPECT_EQ(fs::utf8String(p.logDirectory()), std::string("E:/logs"));
  EXPECT_EQ(fs::utf8String(p.managedAddOnsDirectory()),
              std::string("E:/managed"));
  EXPECT_EQ(fs::utf8String(p.extractAddOnsDirectory()),
              std::string("E:/extract"));
  // Overriding the save directory does not move the snapshots (they have their
  // own variable).
  const auto q = windowsGame({{"OO_SAVEDIR", "E:/saves"}});
  EXPECT_TRUE(same(q.snapshotDirectory(), kExeDir + "/oolite-saves/snapshots"));
}

TEST(resourcepaths, additionalAddOnsSplitOnCommaKeepingEmptyComponents) {
  EXPECT_TRUE(windowsGame().additionalAddOnsDirectories().empty());
  const auto dirs = windowsGame({{"OO_ADDITIONALADDONSDIRS", "E:/a,,F:/b"}})
                        .additionalAddOnsDirectories();
  EXPECT_EQ(dirs.size(), 3u);
  if (dirs.size() != 3) return;
  EXPECT_EQ(fs::utf8String(dirs[0]), std::string("E:/a"));
  EXPECT_TRUE(dirs[1].empty());
  EXPECT_EQ(fs::utf8String(dirs[2]), std::string("F:/b"));
}

TEST(resourcepaths, userRootsAreAdditionalThenShareThenCwdThenExtract) {
  const auto roots = windowsGame({{"OO_ADDITIONALADDONSDIRS", "E:/extra"}})
                         .userRootDirectories();
  EXPECT_EQ(roots.size(), 4u);
  if (roots.size() != 4) return;
  EXPECT_TRUE(same(roots[0], "E:/extra"));
  EXPECT_TRUE(same(roots[1], "C:/Games/Oolite/share/oolite/AddOns"));
  EXPECT_TRUE(same(roots[2], kExeDir + "/AddOns"));
  EXPECT_EQ(fs::utf8String(roots[3]), std::string("../AddOns"));
}

TEST(resourcepaths, builtInResourcesPreferCwdResourcesThenShare) {
  std::error_code ec;
  const fs::Path base =
      std::filesystem::temp_directory_path(ec) / "oofnd-resourcepaths-test";
  std::filesystem::remove_all(base, ec);
  const fs::Path cwd = base / "bin";
  std::filesystem::create_directories(cwd, ec);

  const auto p = make(Platform::kWindows, {}, cwd);
  EXPECT_TRUE(same(p.builtInResourcesDirectory(),
                base / "share" / "oolite" / "Resources"));

  std::filesystem::create_directories(cwd / "Resources", ec);
  EXPECT_TRUE(same(p.builtInResourcesDirectory(), cwd / "Resources"));

  std::filesystem::remove_all(base, ec);
}

TEST(resourcepaths, rootDirectoriesAreBuiltInThenManagedThenUserRoots) {
  const auto p = windowsGame();
  const auto roots = p.rootDirectories();
  const auto user = p.userRootDirectories();
  EXPECT_EQ(roots.size(), 2 + user.size());
  if (roots.size() != 2 + user.size()) return;
  EXPECT_TRUE(same(roots[0], p.builtInResourcesDirectory()));
  EXPECT_TRUE(same(roots[1], p.managedAddOnsDirectory()));
  for (std::size_t i = 0; i < user.size(); ++i)
    EXPECT_TRUE(roots[2 + i] == user[i]);
}

TEST(resourcepaths, processEnvironmentReadsTheRealEnvironment) {
  const oo::PathEnvironment env = oo::PathEnvironment::process();
#if defined(_WIN32)
  EXPECT_TRUE(env.platform == Platform::kWindows);
  EXPECT_TRUE(env.getenv("SystemRoot").has_value());  // always set on Windows
#else
  EXPECT_TRUE(env.platform == Platform::kPosix);
#endif
  EXPECT_TRUE(env.getenv("OOFND_SURELY_UNSET_VARIABLE_1f3a") == std::nullopt);
  EXPECT_TRUE(!env.currentDirectory.empty());
  EXPECT_TRUE(
      same(env.currentDirectory, fs::currentDirectory().value_or(fs::Path())));

  // current() is the same computation over process().
  EXPECT_TRUE(same(oo::ResourcePaths::current().saveDirectory(),
                oo::ResourcePaths(env).saveDirectory()));
}
