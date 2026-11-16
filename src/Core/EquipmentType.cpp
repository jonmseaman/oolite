/*

OOEquipmentType.m


Copyright (C) 2008-2013 Jens Ayton and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "EquipmentType.h"

#include <algorithm>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "CacheManager.h"
#include "Color.h"
#include "DebugStandards.h"
#include "OOLegacyScriptWhitelist.h"
#include "OOScript.h"
#include "PlayerEntityLegacyScriptEngine.h"  // (was imported before the Conveniences category)
#include "Universe.h"
#include "oofnd/Object.hpp"
#include "oofnd/String.hpp"

namespace {
std::vector<std::shared_ptr<oo::EquipmentType>> sEquipmentTypes;
std::vector<std::shared_ptr<oo::EquipmentType>> sEquipmentTypesOutfitting;
std::map<std::string, std::shared_ptr<oo::EquipmentType>, std::less<>>
    sEquipmentTypesByIdentifier;
std::map<std::string, std::string, std::less<>>
    sMissilesRegistry;  // ship key -> missile role
}  // namespace

namespace {

// requires_equipment & co.: a string or an array of strings (sorted,
// de-duplicated: was a set); nullopt when absent, and after logging when it is
// anything else. oo_stringForKey:defaultValue: on a dictionary: a string, or a
// number's -stringValue, else the fallback.
std::optional<std::string> StringFor(
    const oo::PList& info, std::string_view key,
    const std::optional<std::string>& fallback = std::nullopt) {
  const oo::PList* value = info.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return fallback;
  return info.get<std::string>(key);
}

// oo_stringAtIndex: on an array: a string, or a number's -stringValue; nullopt
// (nil) otherwise.
std::optional<std::string> StringAt(const oo::PList& array, std::size_t index) {
  const oo::PList* value = array.at(index);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return array.at<std::string>(index);
}

// The items of the universe's equipment data & co. (a non-array has none).
std::vector<oo::PList> EquipmentItems(const oo::PList& equipmentData) {
  std::vector<oo::PList> items;
  if (const oo::PList::Array* elements =
          equipmentData.getIf<oo::PList::Array>())
    items.assign(elements->begin(), elements->end());
  return items;
}

std::optional<std::vector<std::string>> EquipmentKeysFrom(
    const oo::PList& extra, std::string_view key,
    const std::string& identifier) {
  const oo::PList* value = extra.find(key);
  if (value == nullptr) return std::nullopt;
  std::vector<std::string> keys;
  if (const std::string* text = value->getIf<std::string>()) {
    keys.push_back(*text);
  } else if (const oo::PList::Array* elements =
                 value->getIf<oo::PList::Array>()) {
    for (const oo::PList& element : *elements) {
      if (const std::string* elementText = element.getIf<std::string>())
        keys.push_back(*elementText);
    }
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
  } else {
    OO_LOG("equipment.load",
           "***** ERROR: {} for equipment item {} is not a string or an array.",
           key, identifier);
    return std::nullopt;
  }
  return keys;
}
}  // namespace

void oo::EquipmentType::loadEquipment() {
  std::vector<std::shared_ptr<oo::EquipmentType>> equipmentTypes;
  std::vector<std::string> conditionScripts;  // first-seen order
  std::map<std::string, std::shared_ptr<oo::EquipmentType>, std::less<>>
      byIdentifier;

  for (const oo::PList& itemInfo : EquipmentItems((
           UNIVERSE != nullptr ? UNIVERSE->getEquipmentData() : oo::PList()))) {
    std::shared_ptr<oo::EquipmentType> item = createWithInfo(itemInfo);
    if (item != nullptr) {
      equipmentTypes.emplace_back(item);
      byIdentifier[*item->identifier()] = item;
    }
    const std::optional<std::string> condition_script =
        (item != nullptr) ? item->conditionScript() : std::nullopt;
    if (condition_script.has_value()) {
      if (std::find(conditionScripts.begin(), conditionScripts.end(),
                    *condition_script) == conditionScripts.end()) {
        conditionScripts.push_back(*condition_script);
      }
    }
  }

  oo::PList::Array conditionScriptList;  // an array of strings
  for (const std::string& conditionScript : conditionScripts)
    conditionScriptList.emplace_back(conditionScript);
  oo::CacheManager::sharedCache()->setPList(
      oo::PList(std::move(conditionScriptList)), "equipment conditions",
      "condition scripts");

  sEquipmentTypes = equipmentTypes;
  sEquipmentTypesByIdentifier = byIdentifier;

  // same for the outfitting dataset
  equipmentTypes.clear();
  for (const oo::PList& itemInfo : EquipmentItems(
           (UNIVERSE != nullptr ? UNIVERSE->getEquipmentDataOutfitting()
                                : oo::PList()))) {
    std::shared_ptr<oo::EquipmentType> item = createWithInfo(itemInfo);
    if (item != nullptr) {
      equipmentTypes.emplace_back(item);
    }
  }
  sEquipmentTypesOutfitting = equipmentTypes;
}

void oo::EquipmentType::addEquipmentWithInfo(const oo::PList& itemInfo) {
  std::shared_ptr<oo::EquipmentType> item = createWithInfo(itemInfo);
  if (item != nullptr) {
    sEquipmentTypes.emplace_back(item);
    sEquipmentTypesOutfitting.emplace_back(item);
    sEquipmentTypesByIdentifier[*item->identifier()] = item;
  }
}

std::optional<std::string> oo::EquipmentType::getMissileRegistryRoleForShip(
    const std::string& shipKey) {
  const auto entry = sMissilesRegistry.find(shipKey);
  if (entry == sMissilesRegistry.end()) return std::nullopt;
  return entry->second;
}

void oo::EquipmentType::setMissileRegistryRole(const std::string& role,
                                               const std::string& shipKey) {
  // (the nil checks on role and ship key are the bridge's; the empty key is
  // still refused here)
  if (!shipKey.empty()) {
    sMissilesRegistry[shipKey] = role;
  }
}

std::vector<std::shared_ptr<oo::EquipmentType>>
oo::EquipmentType::allEquipmentTypes() {
  return sEquipmentTypes;
}

std::vector<std::shared_ptr<oo::EquipmentType>>
oo::EquipmentType::allEquipmentTypesOutfitting() {
  return sEquipmentTypesOutfitting;
}

std::shared_ptr<oo::EquipmentType>
oo::EquipmentType::equipmentTypeWithIdentifier(const std::string& identifier) {
  const auto entry = sEquipmentTypesByIdentifier.find(identifier);
  return (entry != sEquipmentTypesByIdentifier.end()) ? entry->second : nullptr;
}

// -initWithInfo: on a new object: nullptr where it failed (released self and
// returned nil).
std::shared_ptr<oo::EquipmentType> oo::EquipmentType::createWithInfo(
    const oo::PList& info) {
  std::shared_ptr<oo::EquipmentType> item(new oo::EquipmentType, oo::Dispose{});
  if (!item->initWithInfo(info)) return nullptr;
  return item;
}

bool oo::EquipmentType::initWithInfo(const oo::PList& info) {
  bool OK = true;

  if (OK && info.count() <= EQUIPMENT_LONG_DESC_INDEX) OK = false;

  if (OK) {
    // Read required attributes
    _techLevel = info.at<unsigned int>(EQUIPMENT_TECH_LEVEL_INDEX);
    _price = info.at<unsigned int>(EQUIPMENT_PRICE_INDEX);
    const std::optional<std::string> name =
        StringAt(info, EQUIPMENT_SHORT_DESC_INDEX);
    const std::optional<std::string> identifier =
        StringAt(info, EQUIPMENT_KEY_INDEX);
    const std::optional<std::string> description =
        StringAt(info, EQUIPMENT_LONG_DESC_INDEX);

    if (!name.has_value() || !identifier.has_value() ||
        !description.has_value()) {
      OO_LOG("equipment.load",
             "***** ERROR: Invalid equipment.plist entry - missing name, "
             "identifier or description (\"{}\", {}, \"{}\")",
             name.value_or("(null)"), identifier.value_or("(null)"),
             description.value_or("(null)"));
      OK = false;
    } else {
      _name = *name;
      _identifier = *identifier;
      _description = *description;
    }
  }

  if (OK) {
    // Implied attributes for backwards-compatibility
    if (oo::str::hasSuffix(_identifier, "_MISSILE") ||
        oo::str::hasSuffix(_identifier, "_MINE")) {
      _isMissileOrMine = true;
      _requiresEmptyPylon = true;
    } else if (_identifier == "EQ_PASSENGER_BERTH_REMOVAL") {
      _requiresFreePassengerBerth = true;
    } else if (_identifier == "EQ_FUEL") {
      _requiresNonFullFuel = true;
    }
    _isVisible = true;
    _isAvailableToPlayer = true;
    _isAvailableToNPCs = true;
    _damageProbability = 1.0;
    _hideValues = false;
  }

  if (OK && info.count() > EQUIPMENT_EXTRA_INFO_INDEX) {
    // Read extra info dictionary
    const oo::PList* extra =
        info.at<oo::PList::Dict>(EQUIPMENT_EXTRA_INFO_INDEX);
    if (extra != nullptr) {
      const oo::PList& extraInfo = *extra;

      _isAvailableToAll = static_cast<unsigned char>(
          extraInfo.get<bool>("available_to_all", _isAvailableToAll));
      _isAvailableToPlayer = static_cast<unsigned char>(
          extraInfo.get<bool>("available_to_player", _isAvailableToPlayer));
      _isAvailableToNPCs = static_cast<unsigned char>(
          extraInfo.get<bool>("available_to_NPCs", _isAvailableToNPCs));

      _isMissileOrMine = static_cast<unsigned char>(
          extraInfo.get<bool>("is_external_store", _isMissileOrMine));
      _requiresEmptyPylon = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_empty_pylon", _requiresEmptyPylon));
      _requiresMountedPylon = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_mounted_pylon", _requiresMountedPylon));
      _requiresClean = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_clean", _requiresClean));
      _requiresNotClean = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_not_clean", _requiresNotClean));
      _portableBetweenShips = static_cast<unsigned char>(
          extraInfo.get<bool>("portable_between_ships", _portableBetweenShips));
      _requiresFreePassengerBerth =
          static_cast<unsigned char>(extraInfo.get<bool>(
              "requires_free_passenger_berth", _requiresFreePassengerBerth));
      _requiresFullFuel = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_full_fuel", _requiresFullFuel));
      _requiresNonFullFuel = static_cast<unsigned char>(
          extraInfo.get<bool>("requires_non_full_fuel", _requiresNonFullFuel));
      _isVisible = static_cast<unsigned char>(
          extraInfo.get<bool>("visible", _isVisible));
      _canCarryMultiple = static_cast<unsigned char>(
          extraInfo.get<bool>("can_carry_multiple", false));
      _hideValues =
          static_cast<unsigned char>(extraInfo.get<bool>("hide_values", false));

      _requiredCargoSpace = extraInfo.get<unsigned int>("requires_cargo_space",
                                                        _requiredCargoSpace);

      _installTime = extraInfo.get<unsigned int>("installation_time", 0);
      _repairTime = extraInfo.get<unsigned int>("repair_time", 0);
      if (const oo::PList* provides =
              extraInfo.get<oo::PList::Array>("provides")) {
        for (const oo::PList& element : *provides->getIf<oo::PList::Array>()) {
          if (const std::string* text = element.getIf<std::string>())
            _provides.push_back(*text);
        }
      }

      const oo::PList* dispColor =
          extraInfo.find("display_color");  // absent: a null PList, as nil was
      _displayColor = oo::Color::colorWithDescription(
          (dispColor != nullptr) ? *dispColor : oo::PList());

      const oo::PList* weaponInfo =
          extraInfo.get<oo::PList::Dict>("weapon_info");
      _weaponInfo =
          (weaponInfo != nullptr) ? *weaponInfo : oo::PList(oo::PList::Dict{});

      _damageProbability = extraInfo.get<float>("damage_probability",
                                                (_isMissileOrMine ? 0.0 : 1.0));

      _requiresEquipment =
          EquipmentKeysFrom(extraInfo, "requires_equipment", _identifier);
      _requiresAnyEquipment =
          EquipmentKeysFrom(extraInfo, "requires_any_equipment", _identifier);
      _incompatibleEquipment = EquipmentKeysFrom(
          extraInfo, "incompatible_with_equipment", _identifier);

      oo::PList legacyConditions;
      const oo::PList* value = extraInfo.find("conditions");
      if (value != nullptr && value->isString()) {
        legacyConditions = oo::PList(oo::PList::Array{*value});
      } else if (value != nullptr && value->isArray()) {
        legacyConditions = *value;
      } else if (value != nullptr) {
        OO_LOG("equipment.load",
               "***** ERROR: {} for equipment item {} is not a string or an "
               "array.",
               "conditions", _identifier);
      }
      if (legacyConditions) {
        oo::StandardsDeprecated(std::format(
            "The conditions key is deprecated for equipment {}", _name));
        if (!oo::EnforceStandards()) {
          _conditions = OOSanitizeLegacyScriptConditions(
              legacyConditions, std::format("<equipment type \"{}\">", _name));
        }
      }

      value = extraInfo.find("condition_script");
      if (value != nullptr && value->isString()) {
        _condition_script = *value->getIf<std::string>();
      } else if (value != nullptr) {
        OO_LOG("equipment.load",
               "***** ERROR: {} for equipment item {} is not a string.",
               "condition_script", _identifier);
      }
      /* Condition scripts are shared: all equipment/ships using the
       * same condition script use one shared instance. Equipment
       * scripts and ship scripts are not shared and get one instance
       * per item. */

      if (const oo::PList* scriptInfo =
              extraInfo.get<oo::PList::Dict>("script_info"))
        _scriptInfo = *scriptInfo;

      _script = StringFor(extraInfo, "script");
      if (_script.has_value()) {
        // (the loaded script, which nothing keeps, lived until the pool
        // drained)
        std::shared_ptr<OOScript> loaded =
            OOScript::jsScriptFromFileNamed(*_script, oo::PList());
        if (loaded == nullptr) _script.reset();
        OOScriptAutorelease(std::move(loaded));
      }
      if (_script.has_value()) {
        _fastAffinityA = !!extraInfo.get<bool>("fast_affinity_defensive");
        _fastAffinityB = !!extraInfo.get<bool>("fast_affinity_offensive");

        // look for default activate and mode key settings
        // note: the customEquipmentActivation array is only populated when
        // starting a game so the application of any default key settings on
        // equipment will only happen then
        for (const bool activate : {true, false}) {
          const std::string_view keyName =
              activate ? "default_activate_key" : "default_mode_key";
          oo::PList& defaultKey =
              activate ? _defaultActivateKey : _defaultModeKey;

          const oo::PList* keydef = extraInfo.find(keyName);
          if (keydef != nullptr && !keydef->isArray()) {
            OO_LOG("equipment.load",
                   "***** ERROR: {} for equipment item {} is not an array.",
                   keyName, _identifier);
            keydef = nullptr;
          }

          if (keydef != nullptr) {
            // do processing for key
            defaultKey = (PLAYER != nullptr ? PLAYER->processKeyCode(*keydef)
                                            : oo::PList());
            const std::optional<std::string> checking =
                (PLAYER != nullptr
                     ? PLAYER->validateKey(
                           (activate ? "activate_" : "mode_") + _identifier,
                           defaultKey)
                     : std::optional<std::string>());

            if (checking.has_value()) {
              if (activate) {
                OO_LOG("equipment.load",
                       "***** Error: {} for equipment item {} is already in "
                       "use for {}. Default not applied",
                       keyName, _identifier, *checking);
              } else {
                OO_LOG("equipment.load",
                       "***** Error: {} for equipment item {} is already in "
                       "use for {}. Default not applied.",
                       keyName, _identifier, *checking);
              }
              defaultKey = oo::PList();
            }
          }
        }
      }
    }
  }

  return OK;
}

std::optional<std::string> oo::EquipmentType::descriptionComponents() const {
  return std::format("{} \"{}\"", _identifier, _name);
}

std::string oo::EquipmentType::className() const { return "OOEquipmentType"; }

std::string oo::EquipmentType::description() const {
  std::string result =
      std::format("<OOEquipmentType {}>", oo::str::pointerDescription(this));
  const std::optional<std::string> components = descriptionComponents();
  if (components.has_value()) result += "{" + *components + "}";
  return result;
}

std::optional<std::string> oo::EquipmentType::identifier() {
  return _identifier;
}

std::optional<std::string> oo::EquipmentType::damagedIdentifier() {
  return _identifier + "_DAMAGED";
}

std::optional<std::string> oo::EquipmentType::name() { return _name; }

std::optional<std::string> oo::EquipmentType::descriptiveText() {
  return _description;
}

oo::TechLevelID oo::EquipmentType::techLevel() { return _techLevel; }

oo::CreditsQuantity oo::EquipmentType::price() { return _price; }

bool oo::EquipmentType::isAvailableToAll() { return _isAvailableToAll; }

bool oo::EquipmentType::requiresEmptyPylon() { return _requiresEmptyPylon; }

bool oo::EquipmentType::requiresMountedPylon() { return _requiresMountedPylon; }

bool oo::EquipmentType::requiresCleanLegalRecord() { return _requiresClean; }

bool oo::EquipmentType::requiresNonCleanLegalRecord() {
  return _requiresNotClean;
}

bool oo::EquipmentType::requiresFreePassengerBerth() {
  return _requiresFreePassengerBerth;
}

bool oo::EquipmentType::requiresFullFuel() { return _requiresFullFuel; }

bool oo::EquipmentType::requiresNonFullFuel() { return _requiresNonFullFuel; }

bool oo::EquipmentType::isPrimaryWeapon() {
  return oo::str::hasPrefix(_identifier, "EQ_WEAPON");
}

bool oo::EquipmentType::isMissileOrMine() { return _isMissileOrMine; }

bool oo::EquipmentType::isPortableBetweenShips() {
  return _portableBetweenShips;
}

bool oo::EquipmentType::canCarryMultiple() {
  if (isMissileOrMine()) return true;
  // technically multiple can be fitted, but not to the same mount.
  if (isPrimaryWeapon()) return false;

  // hard-coded as special items
  if (_identifier == "EQ_PASSENGER_BERTH" || _identifier == "EQ_TRUMBLE") {
    return true;
  }

  return _canCarryMultiple;
}

GLfloat oo::EquipmentType::damageProbability() {
  if (isMissileOrMine()) return 0.0;

  return _damageProbability;
}

bool oo::EquipmentType::canBeDamaged() {
  if (isMissileOrMine()) return false;

  if (damageProbability() > 0.0) {
    return true;
  }

  return false;
}

bool oo::EquipmentType::isVisible() { return _isVisible; }

bool oo::EquipmentType::hideValues() { return _hideValues; }

bool oo::EquipmentType::isAvailableToPlayer() { return _isAvailableToPlayer; }

bool oo::EquipmentType::isAvailableToNPCs() { return _isAvailableToNPCs; }

oo::CargoQuantity oo::EquipmentType::requiredCargoSpace() {
  return _requiredCargoSpace;
}

std::optional<std::vector<std::string>> oo::EquipmentType::requiresEquipment() {
  return _requiresEquipment;
}

std::optional<std::vector<std::string>>
oo::EquipmentType::requiresAnyEquipment() {
  return _requiresAnyEquipment;
}

std::optional<std::vector<std::string>>
oo::EquipmentType::incompatibleEquipment() {
  return _incompatibleEquipment;
}

std::shared_ptr<oo::Color> oo::EquipmentType::displayColor() {
  return _displayColor;
}

void oo::EquipmentType::setDisplayColor(oo::Color* color) {
  _displayColor = oo::share(color);
}

oo::PList oo::EquipmentType::conditions() { return _conditions; }

std::optional<std::string> oo::EquipmentType::conditionScript() {
  return _condition_script;
}

oo::PList oo::EquipmentType::scriptInfo() { return _scriptInfo; }

std::optional<std::string> oo::EquipmentType::scriptName() { return _script; }

bool oo::EquipmentType::fastAffinityDefensive() { return _fastAffinityA; }

bool oo::EquipmentType::fastAffinityOffensive() { return _fastAffinityB; }

oo::PList oo::EquipmentType::defaultActivateKey() {
  return _defaultActivateKey;
}

oo::PList oo::EquipmentType::defaultModeKey() { return _defaultModeKey; }

std::size_t oo::EquipmentType::installTime() { return _installTime; }

std::size_t oo::EquipmentType::repairTime() {
  if (_repairTime > 0) {
    return _repairTime;
  } else {
    return _installTime / 2;
  }
}

std::vector<std::string> oo::EquipmentType::providesForScripting() {
  return _provides;
}

bool oo::EquipmentType::provides(const std::string& key) {
  return std::find(_provides.begin(), _provides.end(), key) != _provides.end();
}

// weapon properties follow
bool oo::EquipmentType::isTurretLaser() {
  return _weaponInfo.get<bool>("is_turret_laser", false);
}

bool oo::EquipmentType::isMiningLaser() {
  return _weaponInfo.get<bool>("is_mining_laser", false);
}

oo::PList oo::EquipmentType::weaponInfo() { return _weaponInfo; }

GLfloat oo::EquipmentType::weaponRange() {
  return _weaponInfo.get<float>("range", 12500.0);
}

GLfloat oo::EquipmentType::weaponEnergyUse() {
  return _weaponInfo.get<float>("energy", static_cast<float>(0.8));
}

GLfloat oo::EquipmentType::weaponDamage() {
  return _weaponInfo.get<float>("damage", 15.0);
}

GLfloat oo::EquipmentType::weaponRechargeRate() {
  return _weaponInfo.get<float>("recharge_rate", 0.5);
}

GLfloat oo::EquipmentType::weaponShotTemperature() {
  return _weaponInfo.get<float>("shot_temperature", 7.0);
}

GLfloat oo::EquipmentType::weaponThreatAssessment() {
  return _weaponInfo.get<float>("threat_assessment", 1.0);
}

std::shared_ptr<oo::Color> oo::EquipmentType::weaponColor() {
  const oo::PList* color =
      _weaponInfo.find("color");  // absent: a null PList, as nil was
  return oo::Color::brightColorWithDescription(
      (color != nullptr) ? *color : oo::PList());
}

std::optional<std::string> oo::EquipmentType::fxShotMissName() {
  return StringFor(_weaponInfo, "fx_shot_miss_name", "[player-laser-miss]");
}

std::optional<std::string> oo::EquipmentType::fxShotHitName() {
  return StringFor(_weaponInfo, "fx_shot_hit_name", "[player-laser-hit]");
}

std::optional<std::string> oo::EquipmentType::fxShieldHitName() {
  return StringFor(_weaponInfo, "fx_hitplayer_shielded_name",
                   "[player-hit-by-weapon]");
}

std::optional<std::string> oo::EquipmentType::fxUnshieldedHitName() {
  return StringFor(_weaponInfo, "fx_hitplayer_unshielded_name",
                   "[player-direct-hit]");
}

std::optional<std::string> oo::EquipmentType::fxWeaponLaunchedName() {
  return StringFor(
      _weaponInfo, "fx_weapon_launch_name",
      (oo::str::hasSuffix(_identifier, "_MINE") ? "[mine-launched]"
                                                : "[missile-launched]"));
}

oo::TechLevelID oo::EquipmentType::effectiveTechLevel() {
  oo::TechLevelID tl;

  tl = techLevel();
  if (tl == oo::kVariableTechLevel) {
    oo::StandardsDeprecated(
        std::format("TL99 is deprecated for {}", _identifier));
    if (!oo::EnforceStandards()) {
      const oo::PList missionValue =
          (PLAYER != nullptr
               ? PLAYER->missionVariableForKey("mission_TL_FOR_" + _identifier)
               : oo::PList());  // OOUIntegerFromObject:
                                // OOUnsignedLongLongFromObject on this 64-bit
                                // build
      tl = static_cast<oo::TechLevelID>(
          static_cast<std::ptrdiff_t>(oo::plist_get::unsignedLongLongFrom(
              !missionValue.isNull() ? &missionValue : nullptr, tl)));
    }
  }

  return tl;
}
