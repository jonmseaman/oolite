/*

ShipEntityLoadRestore.m


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

#include "ShipEntityLoadRestore.h"

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "AI.h"
#include "OOEquipmentType.h"
#include "OOPListGameTypes.h"
#include "OORoleSet.h"
#include "OOShipGroup.h"
#include "OOShipRegistry.h"
#include "ShipEntityAI.h"
#include "StationEntity.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PListGet.hpp"
#include "oofnd/objc/OOAssert.h"

constexpr std::string_view kKeyShipKey = "ship_key";
constexpr std::string_view kKeyShipdataOverrides = "shipdata_overrides";
constexpr std::string_view kKeyShipdataDeletes = "shipdata_deletes";
constexpr std::string_view kKeyPrimaryRole = "primary_role";
constexpr std::string_view kKeyPosition = "position";
constexpr std::string_view kKeyOrientation = "orientation";
constexpr std::string_view kKeyRoles = "roles";
constexpr std::string_view kKeyFuel = "fuel";
constexpr std::string_view kKeyBounty = "bounty";
constexpr std::string_view kKeyEnergyLevel = "energy_level";
constexpr std::string_view kKeyEquipment = "equipment";
constexpr std::string_view kKeyMissiles = "missiles";
constexpr std::string_view kKeyForwardWeapon = "forward_weapon_type";
constexpr std::string_view kKeyAftWeapon = "aft_weapon_type";
constexpr std::string_view kKeyScanClass = "scan_class";

// AI is a complete pickled AI state.
constexpr std::string_view kKeyAi = "AI";

// Group IDs are numbers synchronised through the context object.
constexpr std::string_view kKeyGroupId = "group";
constexpr std::string_view kKeyGroupName = "group_name";
constexpr std::string_view kKeyIsGroupLeader = "is_group_leader";
constexpr std::string_view kKeyEscortGroupId = "escort_group";

namespace {

void StripIgnoredKeys(oo::PList::Dict& dict);
std::size_t GroupIDForGroup(OOShipGroup* group, OOShipSaveContext& context);
OOShipGroup* GroupForGroupID(std::size_t groupID, OOShipSaveContext& context);
oo::PList::Dict DictFrom(const oo::PList& plist);
oo::PList ArrayFromStrings(const std::vector<std::string>& strings);
std::optional<std::string> OptionalStringForKey(const oo::PList& dict,
                                                std::string_view key);
}  // namespace

/* The category ShipEntity (LoadRestore): members of ShipEntity;
 * +shipRestoredFromDictionary:useFallback:context: is a static member, and the
 * private category LoadRestoreInternal's -simplifyShipdata:andGetDeletes: a
 * member (only the members call it).
 */

oo::PList ShipEntity::savedShipDictionaryWithContext(
    OOShipSaveContext* context) {
  oo::PList::Dict result;
  OOShipSaveContext localContext;
  if (context == nullptr) context = &localContext;

  result[std::string(kKeyShipKey)] =
      _shipKey.value_or(std::string());  // nil as "", as oo::StdString gave

  oo::PList::Dict updatedShipInfo = DictFrom(shipinfoDictionary);

  // A role set without a role string (nil before, which -setObject:forKey:
  // refused) adds no key.
  const std::shared_ptr<OORoleSet> roles = getRoleSet();
  if (const std::optional<std::string> roleString =
          (roles != nullptr) ? roles->roleString() : std::nullopt)
    updatedShipInfo[std::string(kKeyRoles)] = *roleString;
  updatedShipInfo[std::string(kKeyFuel)] = oo::PList::unsignedInteger(fuel);
  updatedShipInfo[std::string(kKeyBounty)] = oo::PList::unsignedInteger(bounty);
  updatedShipInfo[std::string(kKeyForwardWeapon)] =
      OOStringFromWeaponType(forward_weapon_type).value_or("");
  updatedShipInfo[std::string(kKeyAftWeapon)] =
      OOStringFromWeaponType(aft_weapon_type).value_or("");
  updatedShipInfo[std::string(kKeyScanClass)] =
      OOStringFromScanClass(scanClass);

  std::vector<std::string> deletes;
  simplifyShipdata(updatedShipInfo, &deletes);

  result[std::string(kKeyShipdataOverrides)] =
      oo::PList(std::move(updatedShipInfo));
  if (!deletes.empty())
    result[std::string(kKeyShipdataDeletes)] = ArrayFromStrings(deletes);

  if (!HPvector_equal(getPosition(), kZeroHPVector)) {
    result[std::string(kKeyPosition)] = OOPListFromHPVector(getPosition());
  }
  if (!quaternion_equal(normalOrientation(), kIdentityQuaternion)) {
    result[std::string(kKeyOrientation)] =
        OOPListFromQuaternion(normalOrientation());
  }

  // -oo_setFloat:forKey: stored a double.
  if (energy != maxEnergy)
    result[std::string(kKeyEnergyLevel)] =
        oo::PList(static_cast<double>(energy / maxEnergy));

  result[std::string(kKeyPrimaryRole)] = getPrimaryRole().value_or("");

  // Add equipment.
  const std::vector<std::string> equipment = equipmentKeys();
  if (equipment.size() != 0)
    result[std::string(kKeyEquipment)] = ArrayFromStrings(equipment);

  // Add missiles.
  if (missiles > 0) {
    std::vector<std::string> missileArray;
    unsigned i;
    for (i = 0; i < missiles; i++) {
      const std::optional<std::string> missileType =
          (missile_list[i] != nullptr ? missile_list[i]->identifier()
                                      : std::optional<std::string>());
      if (missileType.has_value()) missileArray.push_back(*missileType);
    }
    result[std::string(kKeyMissiles)] = ArrayFromStrings(missileArray);
  }

  // Add groups.
  if (_group != nullptr) {
    result[std::string(kKeyGroupId)] =
        oo::PList::unsignedInteger(GroupIDForGroup(_group.get(), *context));
    if ((_group != nullptr ? _group->leader()
                           : static_cast<::ShipEntity*>(nullptr)) == this)
      result[std::string(kKeyIsGroupLeader)] = oo::PList(true);
    const std::optional<std::string> groupName =
        (_group != nullptr ? _group->name() : std::optional<std::string>());
    if (groupName.has_value()) {
      result[std::string(kKeyGroupName)] = *groupName;
    }
  }
  if (_escortGroup != nullptr) {
    result[std::string(kKeyEscortGroupId)] = oo::PList::unsignedInteger(
        GroupIDForGroup(_escortGroup.get(), *context));
  } else if (_group != nullptr &&
             (_group != nullptr
                  ? _group->leader()
                  : static_cast<::ShipEntity*>(nullptr)) == this) {
    /*	Eric:
    The escortGroup property is removed from the lead ship, on entering
    witchspace. But it is needed in the save file to correctly restore an
    escorted group.
    */
    result[std::string(kKeyEscortGroupId)] =
        oo::PList::unsignedInteger(GroupIDForGroup(_group.get(), *context));
  }

  // FIXME: AI.
  // Eric: I think storing the AI name should be enough. On entering a wormhole,
  // the stack is cleared so there are no preserved AI states. Also the AI
  // restarts itself with the GLOBAL state, so no need to store any old state.
  if ((getAI() != nullptr ? getAI()->name() : std::optional<std::string>())
          .value_or(std::string()) == "nullAI.plist") {
    // might be a JS version (with none, no key: -setObject:forKey: refused nil)
    if (const std::optional<std::string> js =
            (getAI() != nullptr ? getAI()->associatedJS()
                                : std::optional<std::string>()))
      result[std::string(kKeyAi)] = *js;
    // if there isn't, loading nullAI.js will load nullAI.plist anyway
  } else {
    result[std::string(kKeyAi)] =
        (getAI() != nullptr ? getAI()->name() : std::optional<std::string>())
            .value_or(std::string());
  }

  return oo::PList(std::move(result));
}

::ShipEntity* ShipEntity::shipRestoredFromDictionary(
    const oo::PList& dict, bool fallback, OOShipSaveContext* context) {
  if (dict.isNull()) return nullptr;
  OOShipSaveContext localContext;
  if (context == nullptr) context = &localContext;

  ::ShipEntity* ship = nullptr;

  const std::string shipKey =
      dict.get<std::string>(kKeyShipKey);  // "" finds no ship, as nil did
  const oo::PList shipData =
      OOShipRegistry::sharedRegistry()->shipInfoForKey(shipKey);

  if (shipData) {
    oo::PList::Dict mergedData = DictFrom(shipData);

    StripIgnoredKeys(mergedData);
    if (const oo::PList* deletes =
            dict.get<oo::PList::Array>(kKeyShipdataDeletes)) {
      for (const oo::PList& key : *deletes->getIf<oo::PList::Array>()) {
        if (const std::string* name = key.getIf<std::string>())
          mergedData.erase(*name);
      }
    }
    if (const oo::PList* overrides =
            dict.get<oo::PList::Dict>(kKeyShipdataOverrides)) {
      for (const auto& [key, value] : *overrides->getIf<oo::PList::Dict>())
        mergedData.insert_or_assign(key, value);
    }
    mergedData["auto_ai"] = oo::PList(false);
    mergedData["escorts"] = oo::PList::unsignedInteger(0);

    const oo::PList mergedPlist(std::move(mergedData));
    // A station is made in C++ (the universe picked its class).
    if ((UNIVERSE != nullptr ? UNIVERSE->isStationShipDictionary(mergedPlist)
                             : false))
      ship = oo::autorelease(
          StationEntity::newStationObject(shipKey, mergedPlist));
    else
      ship = oo::autorelease(oo::NewShipObject(oo::makeShared<::ShipEntity>(),
                                               shipKey, mergedPlist));

    // FIXME: restore AI.
    if (ship != nullptr)
      ship->setAITo(dict.get<std::string>(kKeyAi, "nullAI.plist"));

    const std::optional<std::string> primaryRole =
        OptionalStringForKey(dict, kKeyPrimaryRole);
    if (primaryRole.has_value()) {
      if (ship != nullptr) {
        ship->setPrimaryRole(*primaryRole);
      }
    } else {
      ship->primaryRole = std::nullopt;  // as -setPrimaryRole: stored nil
    }
  } else {
    // Unknown ship; fall back on role if desired and possible.
    const std::optional<std::string> shipPrimaryRole =
        OptionalStringForKey(dict, kKeyPrimaryRole);
    if (!fallback || !shipPrimaryRole.has_value()) return nullptr;

    ship = oo::ToShip(oo::autorelease(
        (UNIVERSE != nullptr ? UNIVERSE->newShipWithRole(*shipPrimaryRole)
                             : nullptr)));
    if (ship == nullptr) return nullptr;
  }

  // The following stuff is deliberately set up the same way even if using role
  // fallback.
  if (ship != nullptr)
    ship->setPosition(
        OOHPVectorFromPList(dict.find(kKeyPosition), kZeroHPVector));
  if (ship != nullptr)
    ship->setNormalOrientation(
        OOQuaternionFromPList(dict.find(kKeyOrientation), kIdentityQuaternion));

  float energyLevel = dict.get<float>(kKeyEnergyLevel, 1.0f);
  if (ship != nullptr)
    ship->setEnergy(energyLevel *
                    (ship != nullptr ? ship->getMaxEnergy() : 0.0f));

  if (ship != nullptr) ship->removeAllEquipment();
  if (const oo::PList* equipment = dict.get<oo::PList::Array>(kKeyEquipment)) {
    for (const oo::PList& eqKey : *equipment->getIf<oo::PList::Array>()) {
      if (const std::string* key = eqKey.getIf<std::string>()) {
        if (ship != nullptr) ship->addEquipmentItem(*key, false, "loading");
      }  // the saved keys are strings
    }
  }

  if (ship != nullptr) ship->removeMissiles();
  if (const oo::PList* missileList = dict.get<oo::PList::Array>(kKeyMissiles)) {
    for (const oo::PList& eqKey : *missileList->getIf<oo::PList::Array>()) {
      if (const std::string* key = eqKey.getIf<std::string>()) {
        if (ship != nullptr) ship->addEquipmentItem(*key, false, "loading");
      }  // the saved keys are strings
    }
  }

  // Groups.
  std::size_t groupID = static_cast<std::size_t>(
      dict.get<std::ptrdiff_t>(kKeyGroupId, oo::kNotFound));
  if (groupID != oo::kNotFound) {
    ::OOShipGroup* group = GroupForGroupID(groupID, *context);
    if (ship != nullptr) ship->setGroup(group);  // Handles adding to group
    if (dict.get<bool>(kKeyIsGroupLeader)) {
      if (group != nullptr) group->setLeader(ship);
    }
    const std::optional<std::string> groupName =
        OptionalStringForKey(dict, kKeyGroupName);
    if (groupName.has_value()) {
      if (group != nullptr) group->setName(groupName);
    }
    if ((ship != nullptr ? ship->hasPrimaryRole("escort") : false) &&
        ship != (group != nullptr ? group->leader()
                                  : static_cast<::ShipEntity*>(nullptr))) {
      if (ship != nullptr)
        ship->setOwner((group != nullptr
                            ? group->leader()
                            : static_cast<::ShipEntity*>(nullptr)));
    }
  }

  groupID = static_cast<std::size_t>(
      dict.get<std::ptrdiff_t>(kKeyEscortGroupId, oo::kNotFound));
  if (groupID != oo::kNotFound) {
    ::OOShipGroup* group = GroupForGroupID(groupID, *context);
    if (group != nullptr) group->setLeader(ship);
    if (group != nullptr) group->setName(std::string("escort group"));
    if (ship != nullptr) ship->setEscortGroup(group);
  }

  return ship;
}

void ShipEntity::simplifyShipdata(oo::PList::Dict& data,
                                  std::vector<std::string>* deletes) {
  OOCParameterAssert(deletes != nullptr);
  deletes->clear();

  // Get original ship data.
  oo::PList::Dict referenceData =
      DictFrom(OOShipRegistry::sharedRegistry()->shipInfoForKey(
          shipDataKey().value_or(std::string())));

  // Discard stuff that we handle separately.
  StripIgnoredKeys(referenceData);
  StripIgnoredKeys(data);

  // Note items that are in referenceData, but not data (in byte order of the
  // key; hash order before).
  for (const auto& [key, value] : referenceData) {
    if (!data.contains(key)) {
      deletes->push_back(key);
    }
  }
}

namespace {

void StripIgnoredKeys(oo::PList::Dict& dict) {
  static constexpr auto ignoredKeys = std::to_array<std::string_view>(
      {"ai_type", "has_ecm", "has_scoop", "has_escape_pod", "has_energy_bomb",
       "has_fuel_injection", "has_cloaking_device", "has_military_jammer",
       "has_military_scanner_filter", "has_shield_booster",
       "has_shield_enhancer", "escorts", "escort_role", "escort-ship",
       "conditions", "missiles", "auto_ai"});

  for (std::string_view key : ignoredKeys) {
    const auto found = dict.find(key);
    if (found != dict.end()) dict.erase(found);
  }
}

std::size_t GroupIDForGroup(OOShipGroup* group, OOShipSaveContext& context) {
  const auto found = context.groupIDs.find(group);
  unsigned groupID;
  if (found == context.groupIDs.end()) {
    // Assign a new group ID.
    groupID = context.nextGroupID;
    context.nextGroupID = groupID + 1;
    context.groupIDs.emplace(group, groupID);

    /*	Also keep references to the groups. This isn't necessary at the
            time of writing, but would be if we e.g. switched to pickling
            ships in wormholes all the time (each wormhole would then need a
            persistent context).
    */
    context.groups.emplace_back(oo::share(group));
  } else {
    groupID = found->second;
  }

  return groupID;
}

OOShipGroup* GroupForGroupID(std::size_t groupID, OOShipSaveContext& context) {
  std::shared_ptr<OOShipGroup>& group = context.groupsByID[groupID];
  if (group == nullptr) {
    group = OOShipGroup::groupWithName(std::nullopt);  // -init
  }

  return group.get();
}

// A dictionary's entries; empty for anything else (as
// +dictionaryWithDictionary: of nil was).
oo::PList::Dict DictFrom(const oo::PList& plist) {
  if (const oo::PList::Dict* dict = plist.getIf<oo::PList::Dict>())
    return *dict;
  return {};
}

oo::PList ArrayFromStrings(const std::vector<std::string>& strings) {
  oo::PList::Array array;
  array.reserve(strings.size());
  for (const std::string& string : strings) array.emplace_back(string);
  return oo::PList(std::move(array));
}

// get<std::string> where the Foundation code read nil: std::nullopt when the
// key is absent or its value is neither a string nor a number.
std::optional<std::string> OptionalStringForKey(const oo::PList& dict,
                                                std::string_view key) {
  const oo::PList* value = dict.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return dict.get<std::string>(key);
}

}  // namespace
