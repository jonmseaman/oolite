/*

ResourceManager.h

Singleton class responsible for loading various data files.

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "EnumFlags.h"
#include "Foundation.h"
#include "OOScript.h"  // std::shared_ptr<OOScript> members and results
#include "OpenGL.h"
#include "oofnd/Data.hpp"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

namespace oo {

class SystemDescriptionManager;  // OOSystemDescriptionManager.h
class Sound;
class Music;

enum class ResourceMergeMode : std::int32_t {
  kNone,   // Just use the last file in search order.
  kBasic,  // Merge files by adding the top-level items of each file.
  // Merge files by merging the top-level elements of each file (second-order
  // merge, but not recursive)
  kSmart,
};

}  // namespace oo

static_assert(std::to_underlying(oo::ResourceMergeMode::kNone) == 0);
static_assert(std::to_underlying(oo::ResourceMergeMode::kBasic) == 1);
static_assert(std::to_underlying(oo::ResourceMergeMode::kSmart) == 2);

/* 'All' doesn't quite mean 'all' - OXPs with the tag
 * "oolite-scenario-only" will only be loaded if required by a
 * scenario.
 *
 * Note that this means that the scenario itself must be in a
 * different OXP, or it'll never be loaded when on the start-game
 * screen.
 */
inline constexpr std::string_view SCENARIO_OXP_DEFINITION_ALL = "";
inline constexpr std::string_view SCENARIO_OXP_DEFINITION_NONE = "strict";
inline constexpr std::string_view SCENARIO_OXP_DEFINITION_BYID = "id:";
inline constexpr std::string_view SCENARIO_OXP_DEFINITION_BYTAG = "tag:";
inline constexpr std::string_view SCENARIO_OXP_DEFINITION_NOPLIST = "exc:";

/*	Class methods over file-scope state: the class is never made, and every
 * member is static.
 */
class ResourceManager {
 public:
  ResourceManager() = delete;

  static void reset();
  static void resetManifestKnowledgeForOXZManager();

  static std::vector<std::string>
  rootPaths();  // Places add-ons are searched for, not including add-on paths.
  static std::vector<std::string>
  // Places users are expected to place add-ons, not including built-in data or
  // managed add-ons directory.
  userRootPaths();
  static std::optional<std::string>
  builtInPath();  // Path for built-in data only.
  static std::vector<std::string>
  pathsWithAddOns();  // Root paths + add-on paths.
  static std::vector<std::string>
  paths();  // builtInPath or pathsWithAddOns, depending on useAddOns state.
  // potential privacy concerns
  static std::vector<std::string> maskUserNameInPathArray(
      const std::vector<std::string>& inputPathArray);
  static std::optional<std::string> maskUserName(const std::string& name,
                                                 const std::string& path);
  static std::optional<std::string>
  useAddOns();  // nullopt before the first scan (was nil)
  static std::vector<std::string> OXPsWithMessagesFound();
  static void setUseAddOns(const std::string& useAddOns);
  static void addExternalPath(const std::string& fileName);

  // get manifest data for identifier (a null PList when there is none)
  static oo::PList manifestForIdentifier(const std::string& identifier);

  static std::optional<std::string>
  // Errors which occurred during path scanning - essentially a list of OXPs
  // whose requires.plist is bad. nullopt when there are none.
  errors();

  // Clear ResourceManager-internal caches (not those handled by OOCacheManager)
  static void clearCaches();

  static std::shared_ptr<oo::SystemDescriptionManager>
  systemDescriptionManager();  // a new manager
  static oo::PList shaderBindingTypesDictionary();
  // nullopt when not found (was nil); folderName nullopt where nil was passed.
  static std::optional<std::string> pathForFileNamed(
      const std::string& fileName,
      const std::optional<std::string>& folderName);
  static std::optional<std::string> pathForFileNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      bool useCache);
  static std::shared_ptr<oo::Music> ooMusicNamed(
      const std::string& fileName,
      // a new music each call (not cached); null when none
      const std::optional<std::string>& folderName);
  static oo::Sound* ooSoundNamed(
      const std::string& fileName,
      // borrowed: the cache keeps it (was autoreleased)
      const std::optional<std::string>& folderName);
  // nullopt when no file was found (was nil); folderName nullopt where nil was
  // passed.
  static std::optional<std::string> stringFromFilesNamed(
      const std::string& fileName,
      const std::optional<std::string>& folderName);
  static std::optional<std::string> stringFromFilesNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      bool useCache);
  // World scripts by name, in the order each name was first loaded.
  static std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>
  loadScripts();
  /*	writeDiagnosticData()
          writeDiagnosticString()
          writeDiagnosticPList()

          Write data to the specified path within the log directory. Slashes may
     be used as path separators in name.
   */
  static bool writeDiagnosticData(const oo::Data& data,
                                  const std::string& name);
  static bool writeDiagnosticString(const std::string& string,
                                    const std::string& name);
  static bool writeDiagnosticPList(const oo::PList& plist,
                                   const std::string& name);
  static oo::PList materialDefaults();
  static std::optional<std::string> diagnosticFileLocation();

  static bool corePlist(const std::string& fileName,
                        const std::string& path);  // -corePlist:excludedAt:
  // A null PList when no file was found; folderName nullopt where nil was
  // passed.
  static oo::PList dictionaryFromFilesNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      bool mergeFiles);
  static oo::PList dictionaryFromFilesNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      oo::ResourceMergeMode mergeMode, bool cache);
  static oo::PList arrayFromFilesNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      bool mergeFiles);
  static oo::PList arrayFromFilesNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      bool mergeFiles, bool useCache);
  // In-out: an array of arrays (the merged files), edited in place.
  static void handleEquipmentListMerging(oo::PList& arrayToProcess,
                                         unsigned lookupIndex);
  static void handleEquipmentOverrides(oo::PList& arrayToProcess);
  static void handleStarNebulaListMerging(oo::PList& arrayToProcess);
  // These are deliberately not merged like normal plists for security reasons.
  static oo::PList
  whitelistDictionary();  // a null PList when the file is missing
  // These have special merging rules.
  static oo::PList logControlDictionary();
  static oo::PList
  // category -> array of its roles, each once (a set), in first-seen order
  roleCategoriesDictionary();

  // compatibility checks (a manifest or relation is a Dict; title is nullopt
  // where nil was passed)
  static bool checkVersionCompatibility(
      const oo::PList& manifest, const std::optional<std::string>& title);
  static bool manifestHasConflicts(const oo::PList& manifest, bool logErrors);
  static bool manifestHasMissingDependencies(const oo::PList& manifest,
                                             bool logErrors);
  static bool manifest(
      const oo::PList& manifest, const oo::PList& required,
      bool logErrors);  // -manifest:HasUnmetDependency:logErrors:
  static bool matchVersions(const oo::PList& rangeDict,
                            const std::string& version);

 private:
  // (OOPrivate)
  static void logPaths();
  static void preloadFileLists();
  static void preloadFileListFromOXZ(const std::string& path,
                                     std::span<const std::string> folders);
  static void preloadFileListFromFolder(const std::string& path,
                                        std::span<const std::string> folders);
  static void preloadFilePathFor(const std::string& fileName,
                                 const std::string& subFolder,
                                 const std::string& path);
  static oo::Sound* retrieveFileNamed(
      const std::string& fileName, const std::optional<std::string>& folderName,
      std::map<std::string, std::shared_ptr<oo::Sound>, std::less<>>* ioCache,
      std::optional<std::string> key,
      // the class was always OOSound; borrowed: the cache keeps it
      bool useCache);
  static bool directoryExists(const std::string& inPath, bool inCreate);
  static bool checkCacheUpToDateForPaths(
      std::span<const std::string> searchPaths);
  static void mergeRoleCategories(const oo::PList& catData,
                                  oo::PList& categories);
  static void checkOXPMessagesInPath(const std::string& path);
  static void checkPotentialPath(const std::string& path,
                                 std::vector<std::string>& searchPaths);
  static bool validateManifest(const oo::PList& manifest,
                               const std::string& path);
  static bool areRequirementsFulfilled(const oo::PList& requirements,
                                       const std::optional<std::string>& path,
                                       const std::string& file);
  static void filterSearchPathsForConflicts(
      std::vector<std::string>& searchPaths);
  static bool filterSearchPathsForRequirements(
      std::vector<std::string>& searchPaths);
  static void filterSearchPathsToExcludeScenarioOnlyPaths(
      std::vector<std::string>& searchPaths);
  static void filterSearchPathsByScenario(
      std::vector<std::string>& searchPaths);
  static bool manifestAllowedByScenario(const oo::PList& manifest);
  static bool manifestAllowedByScenario(
      const oo::PList& manifest,
      const std::string& identifier);  // withIdentifier:
  static bool manifestAllowedByScenarioWithTag(
      const oo::PList& manifest,
      const std::string& tag);  // its own name: the overloads would collide
  static void addErrorWithKey(const std::string& descriptionKey,
                              const std::string& param1,
                              const std::string& param2);
};
