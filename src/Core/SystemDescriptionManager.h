/*

OOSystemDescriptionManager.h

Class responsible for planet description data.

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

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "Foundation.h"
#include "Types.h"
#include "legacy_random.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

namespace oo {

enum class SystemLayer : std::int32_t {
  kCore = 0,
  kOXPStatic = 1,
  kOXPDynamic = 2,
  kOXPPriority = 3,
};

}  // namespace oo

static_assert(std::to_underlying(oo::SystemLayer::kCore) == 0);
static_assert(std::to_underlying(oo::SystemLayer::kOXPStatic) == 1);
static_assert(std::to_underlying(oo::SystemLayer::kOXPDynamic) == 2);
static_assert(std::to_underlying(oo::SystemLayer::kOXPPriority) == 3);

// A system's concealment: planetinfo.plist's number, compared with these
// thresholds (any value may occur), so typed constants.
inline constexpr int OO_SYSTEMCONCEALMENT_NONE = 0;
inline constexpr int OO_SYSTEMCONCEALMENT_NODATA = 100;
inline constexpr int OO_SYSTEMCONCEALMENT_NONAME = 200;
inline constexpr int OO_SYSTEMCONCEALMENT_NOTHING = 300;

inline constexpr int OO_SYSTEM_LAYERS = 4;
#define OO_SYSTEMS_PER_GALAXY (oo::kMaximumSystemID + 1)
#define OO_GALAXIES_AVAILABLE (oo::kMaximumGalaxyID + 1)
#define OO_SYSTEMS_AVAILABLE OO_SYSTEMS_PER_GALAXY* OO_GALAXIES_AVAILABLE
// don't bother caching interstellar properties
inline constexpr auto OO_SYSTEM_CACHE_LENGTH = OO_SYSTEMS_AVAILABLE;

/*	OOSystemDescriptionEntry is used only by the manager. The universe holds
   the manager as std::shared_ptr.
*/
namespace oo {

class SystemDescriptionEntry : public oo::Object {
 public:
  SystemDescriptionEntry();  // -init

  // value / result null = nil (setting nil removes the property)
  void setProperty(const std::string& property, SystemLayer layer,
                   const oo::PList& value);
  oo::PList getProperty(const std::string& property, SystemLayer layer);

 private:
  // null = nil (validation failed and could not be recovered)
  oo::PList validateProperty(const std::string& property,
                             const oo::PList& value);

  oo::PList layers[OO_SYSTEM_LAYERS] = {};  // each a Dict: property -> value
};

/**
 * Note: forSystem: inGalaxy: returns from the (fast) propertyCache
 *
 * forSystemKey calculates the values - but is necessary for
 * interstellar space
 */
class SystemDescriptionManager : public oo::Object {
 public:
  SystemDescriptionManager();  // -init

  // this needs to be re-called every time system coordinates change
  // changing system coordinates after plist loading is probably
  // too much of a can of worms *anyway*, so currently it's only
  // called just after the manager data is loaded.
  void buildRouteCache();

  // Property dictionaries are oo::PList Dicts whose values are the properties'
  // values (any objects: Object nodes where they are not property-list data); a
  // single value is an oo::PList (null = nil).
  void setUniversalProperties(const oo::PList& properties);
  void setInterstellarProperties(const oo::PList& properties);

  // this is used by planetinfo.plist and has default layer 1
  void setProperties(const oo::PList& properties, const std::string& key);

  // this is used by Javascript property setting (manifest nullopt = nil: the
  // change is not saved)
  void setProperty(const std::string& property, const std::string& key,
                   SystemLayer layer, const oo::PList& value,
                   const std::optional<std::string>& manifest);

  // The save game's scripted overrides: property lists (Dicts) whose values are
  // the properties' values (any objects: Object nodes where they are not
  // property-list data).
  void importScriptedChanges(const oo::PList& scripted);
  void importLegacyScriptedChanges(const oo::PList& scripted);
  oo::PList exportScriptedChanges();  // a Dict, empty when there are none

  // Property dictionaries (Dicts; empty for an invalid system) and single
  // values (null = nil).
  oo::PList getPropertiesForSystemKey(const std::string& key);
  oo::PList getPropertiesForSystem(SystemID s, GalaxyID g);
  oo::PList getPropertiesForCurrentSystem();
  oo::PList getProperty(const std::string& property, const std::string& key);
  oo::PList getProperty(const std::string& property, SystemID s, GalaxyID g);

  NSPoint getCoordinatesForSystem(SystemID s, GalaxyID g);
  std::vector<SystemID> getNeighbourIDsForSystem(
      SystemID s, GalaxyID g);  // empty for an invalid system

  Random_Seed getRandomSeedForCurrentSystem();
  Random_Seed getRandomSeedForSystem(SystemID s, GalaxyID g);

 private:
  // Property dictionaries are oo::PList Dicts; a single property value is an
  // oo::PList (null = nil).
  void setProperties(const oo::PList& properties, SystemDescriptionEntry* desc);
  oo::PList calculatePropertiesForSystemKey(const std::string& key);
  void updateCacheEntry(std::size_t i);
  void updateCacheEntry(std::size_t i, const std::string& property);
  oo::PList getProperty(const std::string& property, const std::string& key,
                        bool universal);
  /* some planetinfo properties have two ways to specify
   * need to get the one with higher layer (if they're both at the same layer,
   * go with property1) */
  oo::PList getProperty(const std::string& property1,
                        const std::string& property2, const std::string& key,
                        bool universal);

  // value null = nil (removes the saved change); manifest nullopt = nil
  // (cancels saving it)
  void saveScriptedChangeToProperty(const std::string& property,
                                    const std::string& key, SystemLayer layer,
                                    const oo::PList& value,
                                    const std::optional<std::string>& manifest);

  oo::PList universalProperties = {};  // a Dict: property -> value
  std::shared_ptr<SystemDescriptionEntry> interstellarSpace = {};
  std::map<std::string, std::shared_ptr<SystemDescriptionEntry>, std::less<>>
      systemDescriptions = {};
  oo::PList propertyCache[OO_SYSTEM_CACHE_LENGTH] =
      {};  // each a Dict: property -> value
  std::set<std::string> propertiesInUse = {};
  NSPoint coordinatesCache[OO_SYSTEM_CACHE_LENGTH] = {};
  std::vector<SystemID> neighbourCache[OO_SYSTEM_CACHE_LENGTH] =
      {};                          // system numbers
  oo::PList scriptedChanges = {};  // a Dict: joined override key -> value
};

}  // namespace oo
