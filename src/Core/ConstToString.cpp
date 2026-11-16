/*

OOConstToString.m

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version );
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., ); Franklin Street, Fifth Floor, Boston,
MA );-);, USA.

*/

#include "ConstToString.h"

#include <array>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "CommodityMarket.h"
#include "EnumTable.h"
#include "EquipmentType.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oofnd/String.hpp"

namespace oo {

std::string StringFromEntityStatus(oo::EntityStatus value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

std::string StringFromBehaviour(oo::Behaviour value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("** BEHAVIOUR UNKNOWN **")
                      : std::string(name);
}

}  // namespace oo

namespace oo {

std::string StringFromCompassMode(oo::CompassMode value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

std::string StringFromLongRangeChartMode(oo::LongRangeChartMode value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

}  // namespace oo

namespace oo {

std::string StringFromGalacticHyperspaceBehaviour(
    oo::GalacticHyperspaceBehaviour value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

std::string StringFromGUIScreenID(oo::GUIScreenID value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

std::string StringFromScanClass(oo::ScanClass value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

std::string StringFromShipDamageType(oo::ShipDamageType value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

}  // namespace oo

namespace oo {

std::string StringFromLegalStatusReason(oo::LegalStatusReason value) {
  const std::string_view name = OOEnumName(value);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

}  // namespace oo

namespace oo {

oo::EntityStatus EntityStatusFromString(const std::string& string) {
  return oo::kEntityStatusTable.value(string).value_or(
      oo::kEntityStatusDefault);
}

}  // namespace oo

namespace oo {

oo::CompassMode CompassModeFromString(const std::string& string) {
  return oo::kCompassModeTable.value(string).value_or(oo::kCompassModeDefault);
}

}  // namespace oo

namespace oo {

oo::GalacticHyperspaceBehaviour GalacticHyperspaceBehaviourFromString(
    const std::string& string) {
  if (const std::optional<oo::GalacticHyperspaceBehaviour> value =
          oo::kGalacticHyperspaceBehaviourTable.value(string))
    return *value;

  // Transparently (but inefficiently) support american spelling. FIXME: remove
  // in EMMSTRAN.
  if (oo::str::hasPrefix(string, "BEHAVIOR_")) {
    return GalacticHyperspaceBehaviourFromString("BEHAVIOUR_" +
                                                 string.substr(9));
  }

  return oo::kGalacticHyperspaceBehaviourDefault;
}

oo::GUIScreenID GUIScreenIDFromString(const std::string& string) {
  return oo::kGUIScreenIDTable.value(string).value_or(oo::kGUIScreenIDDefault);
}

oo::ScanClass ScanClassFromString(const std::string& string) {
  return oo::kScanClassTable.value(string).value_or(oo::kScanClassDefault);
}

}  // namespace oo

namespace oo {

oo::LongRangeChartMode LongRangeChartModeFromString(const std::string& string) {
  return oo::kLongRangeChartModeTable.value(string).value_or(
      oo::kLongRangeChartModeDefault);
}

}  // namespace oo

std::string RouteTypeToString(oo::RouteType routeType) {
  const std::string_view name = OOEnumName(routeType);
  return name.empty() ? std::string("** ROUTE TYPE UNKNOWN **")
                      : std::string(name);
}

namespace {
// PListView's string rule (OOCollectionExtractors' StringForObject): the
// element if it is a string, or a number's -stringValue; nullopt (was nil) for
// anything else or a missing element.
std::optional<std::string> DescriptionStringAt(const oo::PList& strings,
                                               std::size_t index) {
  const oo::PList* value = strings.at(index);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return strings.at<std::string>(index);
}
}  // namespace

namespace oo {

std::optional<std::string> DisplayStringFromGovernmentID(
    GovernmentID government) {
  // Only the one sub-array of the descriptions; a missing key reads as the null
  // PList PListFrom(nil) gave.
  const oo::PList* stringsEntry = UNIVERSE->descriptions()->find("government");
  const oo::PList strings =
      (stringsEntry != nullptr) ? *stringsEntry : oo::PList();
  if (strings.isArray() && government < strings.count()) {
    if (const std::string* value = strings.at(government)->getIf<std::string>())
      return *value;
  }

  return std::nullopt;
}

std::optional<std::string> DisplayStringFromEconomyID(EconomyID economy) {
  const oo::PList* stringsEntry = UNIVERSE->descriptions()->find("economy");
  const oo::PList strings =
      (stringsEntry != nullptr) ? *stringsEntry : oo::PList();
  if (strings.isArray() && economy < strings.count()) {
    if (const std::string* value = strings.at(economy)->getIf<std::string>())
      return *value;
  }

  return std::nullopt;
}

}  // namespace oo

namespace oo {

std::optional<std::string> StringFromWeaponType(WeaponType weapon) {
  if (weapon == nullptr) {
    return "EQ_WEAPON_NONE";
  } else {
    return weapon->identifier();
  }
}

WeaponType WeaponTypeFromString(const std::string& string) {
  return WeaponTypeFromEquipmentIdentifierSloppy(string);
}

std::optional<std::string> EquipmentIdentifierFromWeaponType(
    WeaponType weapon) {
  // No weapon: a message to nil answered a zeroed std::optional, no identifier.
  oo::EquipmentType* type = weapon;
  return (type != nullptr) ? type->identifier() : std::nullopt;
}

WeaponType WeaponTypeFromEquipmentIdentifierSloppy(const std::string& string) {
  // The weapon type is borrowed: the registry holds it.
  std::shared_ptr<oo::EquipmentType> w =
      oo::EquipmentType::equipmentTypeWithIdentifier(string);
  if (w == nullptr) {
    if (!oo::str::hasPrefix(string, "EQ_")) {
      w = oo::EquipmentType::equipmentTypeWithIdentifier("EQ_" + string);
      if (w != nullptr) {
        return w.get();
      }
    }
    return oo::EquipmentType::equipmentTypeWithIdentifier("EQ_WEAPON_NONE")
        .get();
  }
  return w.get();
}

/* Previous save games will have weapon types stored as ints to the
 * various weapon types */
WeaponType WeaponTypeFromEquipmentIdentifierLegacy(const std::string& string) {
  if (oo::str::intValue(string) > 0) {
    switch (oo::str::intValue(string)) {
      case 2:
        return WeaponTypeFromEquipmentIdentifierSloppy("EQ_WEAPON_PULSE_LASER");
      case 3:
        return WeaponTypeFromEquipmentIdentifierSloppy("EQ_WEAPON_BEAM_LASER");
      case 4:
        return WeaponTypeFromEquipmentIdentifierSloppy(
            "EQ_WEAPON_MINING_LASER");
      case 5:
        return WeaponTypeFromEquipmentIdentifierSloppy(
            "EQ_WEAPON_MILITARY_LASER");
      case 10:
        return WeaponTypeFromEquipmentIdentifierSloppy(
            "EQ_WEAPON_THARGOID_LASER");
      default:
        return WeaponTypeFromEquipmentIdentifierSloppy(string);
    }
  }
  return WeaponTypeFromEquipmentIdentifierSloppy(string);
}

WeaponType WeaponTypeFromEquipmentIdentifierStrict(const std::string& string) {
  // there is no difference between the two any more
  return WeaponTypeFromEquipmentIdentifierSloppy(string);
}

}  // namespace oo

std::string CargoTypeToString(oo::CargoType cargo) {
  const std::string_view name = OOEnumName(cargo);
  return name.empty() ? std::string("Unknown cargo") : std::string(name);
}

oo::CargoType StringToCargoType(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<oo::CargoType, oo::CargoType::kNotCargo,
                        oo::CargoType::kSlaves, oo::CargoType::kAlloy,
                        oo::CargoType::kMinerals, oo::CargoType::kThargoid,
                        oo::CargoType::kRandom, oo::CargoType::kScriptedItem,
                        oo::CargoType::kCharacter>();
  if (const std::optional<oo::CargoType> value = kNames.value(string))
    return *value;

  // Backwards compatibility.
  if (string == "CARGO_CARRIED") return oo::CargoType::kRandom;

  return oo::kCargoTypeDefault;
}

std::string EnergyUnitTypeToString(oo::EnergyUnitType unit) {
  const std::string_view name = OOEnumName(unit);
  return name.empty() ? std::string("Unsupported energy unit")
                      : std::string(name);
}

oo::EnergyUnitType StringToEnergyUnitType(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<oo::EnergyUnitType, oo::EnergyUnitType::kNone,
                        oo::EnergyUnitType::kNormal, oo::EnergyUnitType::kNaval,
                        oo::EnergyUnitType::kNormalDamaged,
                        oo::EnergyUnitType::kNavalDamaged>();
  if (const std::optional<oo::EnergyUnitType> value = kNames.value(string))
    return *value;

  return oo::kEnergyUnitTypeDefault;
}

namespace oo {

std::optional<std::string> DisplayRatingStringFromKillCount(unsigned kills) {
  constexpr unsigned kRatingCount = 9;

  const unsigned killThresholds[kRatingCount - 1] = {
      0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0200, 0x0A00, 0x1900};
  unsigned i;

  const oo::PList* ratingNamesEntry = UNIVERSE->descriptions()->find("rating");
  const oo::PList ratingNames =
      (ratingNamesEntry != nullptr) ? *ratingNamesEntry : oo::PList();
  for (i = 0; i < kRatingCount - 1; ++i) {
    if (kills < killThresholds[i]) return DescriptionStringAt(ratingNames, i);
  }

  return DescriptionStringAt(ratingNames, kRatingCount - 1);
}

}  // namespace oo

std::string KillCountToRatingAndKillString(unsigned kills) {
  // %@ of a nil rating printed "(null)".
  const std::optional<std::string> rating =
      oo::DisplayRatingStringFromKillCount(kills);
  return std::format("{}   ({})",
                     rating.has_value() ? rating->c_str() : "(null)", kills);
}

namespace oo {

std::optional<std::string> DisplayStringFromLegalStatus(int legalStatus) {
  constexpr unsigned kStatusCount = 3;

  const std::array<int, kStatusCount - 1> statusThresholds = {1, 51};
  unsigned i;

  const oo::PList* statusNamesEntry =
      UNIVERSE->descriptions()->find("legal_status");
  const oo::PList statusNames =
      (statusNamesEntry != nullptr) ? *statusNamesEntry : oo::PList();
  for (i = 0; i != kStatusCount - 1; ++i) {
    if (legalStatus < statusThresholds[i])
      return DescriptionStringAt(statusNames, i);
  }

  return DescriptionStringAt(statusNames, kStatusCount - 1);
}

std::optional<std::string> DisplayStringFromAlertCondition(
    AlertCondition alertCondition) {
  const oo::PList* conditionNamesEntry =
      UNIVERSE->descriptions()->find("condition");
  const oo::PList conditionNames =
      (conditionNamesEntry != nullptr) ? *conditionNamesEntry : oo::PList();
  return DescriptionStringAt(
      conditionNames,
      static_cast<std::size_t>(oo::ToUnderlying(alertCondition)));
}

}  // namespace oo

namespace oo {

std::optional<std::string> DisplayStringFromShaderSetting(
    ShaderSetting setting) {
  // DESC() is Universe's description lookup (not migrated yet), converted at
  // the call.
  switch (setting) {
    case ShaderSetting::kNotSupported:
      return OO_DESC("shaderfx-not-available");
    case ShaderSetting::kOff:
      return OO_DESC("shaderfx-off");
    case ShaderSetting::kSimple:
      return OO_DESC("shaderfx-simple");
    case ShaderSetting::kFull:
      return OO_DESC("shaderfx-full");
  }

  return "??";
}

std::string StringFromShaderSetting(ShaderSetting setting) {
  const std::string_view name = OOEnumName(setting);
  return name.empty() ? std::string("UNDEFINED") : std::string(name);
}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<ShaderSetting, ShaderSetting::kOff,
                        ShaderSetting::kSimple, ShaderSetting::kFull,
                        ShaderSetting::kNotSupported>();
  if (const std::optional<ShaderSetting> value = kNames.value(string))
    return *value;

  return kShaderSettingDefault;
}

}  // namespace oo

std::string CommodityDisplayNameForSymbolicName(
    const std::string& symbolicName) {
  // -descriptionForKey: was NSStringOrNil of -descriptionForKey:, so
  // nullopt reads as before.
  const std::optional<std::string> ret =
      (UNIVERSE != nullptr
           ? UNIVERSE->descriptionForKey("commodity-name " +
                                         oo::str::lowercase(symbolicName))
           : std::optional<std::string>());
  return ret.has_value() ? *ret : symbolicName;
}

std::string CommodityDisplayNameForCommodityArray(
    const oo::PList& commodityDefinition) {
  return CommodityDisplayNameForSymbolicName(
      commodityDefinition.at<std::string>(MARKET_NAME));
}

std::optional<std::string> DisplayStringForMassUnit(oo::MassUnit unit) {
  // DESC() is Universe's description lookup (not migrated yet), converted at
  // the call.
  switch (unit) {
    case oo::MassUnit::kTons:
      return OO_DESC("cargo-tons-symbol");
    case oo::MassUnit::kKilograms:
      return OO_DESC("cargo-kilograms-symbol");
    case oo::MassUnit::kGrams:
      return OO_DESC("cargo-grams-symbol");
    case oo::MassUnit::kUnknown:
      break;
  }

  return "??";
}

std::optional<std::string> DisplayStringForMassUnitForCommodity(
    const std::string& commodity) {
  // No market: -massUnitForGood: sent to nil answered 0, OOMassUnit::kTons.
  oo::CommodityMarket* market =
      (UNIVERSE != nullptr ? UNIVERSE->getCommodityMarket() : nullptr);
  return DisplayStringForMassUnit((market != nullptr)
                                      ? market->massUnitForGood(commodity)
                                      : oo::MassUnit::kTons);
}

oo::RouteType StringToRouteType(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<oo::RouteType, oo::RouteType::kNone,
                        oo::RouteType::kJumps, oo::RouteType::kTime>();
  if (const std::optional<oo::RouteType> value = kNames.value(string))
    return *value;

  return oo::kRouteTypeDefault;
}

std::string DockingClearanceStatusToString(
    oo::DockingClearanceStatus dockingClearanceStatus) {
  const std::string_view name = OOEnumName(dockingClearanceStatus);
  return name.empty() ? std::string("DOCKING_CLEARANCE_STATUS_UNKNOWN")
                      : std::string(name);
}

namespace oo {

std::string StringFromGraphicsDetail(GraphicsDetail detail) {
  const std::string_view name = OOEnumName(detail);
  return name.empty() ? std::string("DETAIL_LEVEL_UNKNOWN") : std::string(name);
}

GraphicsDetail GraphicsDetailFromString(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<GraphicsDetail, GraphicsDetail::kMinimum,
                        GraphicsDetail::kNormal, GraphicsDetail::kShaders,
                        GraphicsDetail::kExtras>();
  if (const std::optional<GraphicsDetail> value = kNames.value(string))
    return *value;

  return GraphicsDetail::kMinimum;
}

std::string StringFromHDRToneMapper(oo::HDRToneMapper toneMapper) {
  const std::string_view name = OOEnumName(toneMapper);
  return name.empty() ? std::string("OOHDR_TONEMAPPER_UNDEFINED")
                      : std::string(name);
}

oo::HDRToneMapper HDRToneMapperFromString(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<oo::HDRToneMapper, oo::HDRToneMapper::kNone,
                        oo::HDRToneMapper::kAcesApprox,
                        oo::HDRToneMapper::kDice, oo::HDRToneMapper::kUchimura,
                        oo::HDRToneMapper::kReinhard>();
  if (const std::optional<oo::HDRToneMapper> value = kNames.value(string))
    return *value;

  return oo::HDRToneMapper::kAcesApprox;
}

std::string StringFromSDRToneMapper(oo::SDRToneMapper toneMapper) {
  const std::string_view name = OOEnumName(toneMapper);
  return name.empty() ? std::string("OOSDR_TONEMAPPER_UNDEFINED")
                      : std::string(name);
}

oo::SDRToneMapper SDRToneMapperFromString(const std::string& string) {
  static constexpr auto kNames =
      oo::MakeEnumTable<oo::SDRToneMapper, oo::SDRToneMapper::kNone,
                        oo::SDRToneMapper::kAces, oo::SDRToneMapper::kAgX,
                        oo::SDRToneMapper::kHejlDawson, oo::SDRToneMapper::kUC2,
                        oo::SDRToneMapper::kUchimura,
                        oo::SDRToneMapper::kReinhard>();
  if (const std::optional<oo::SDRToneMapper> value = kNames.value(string))
    return *value;

  return oo::SDRToneMapper::kAces;
}

}  // namespace oo
