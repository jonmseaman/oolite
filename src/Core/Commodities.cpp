/*

OOCommodities.m

Oolite
Copyright (C) 2004-2014 Giles C Williams and contributors

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

#include "Commodities.h"

#include <quickjs.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "CommodityMarket.h"
#include "OOJSEngineCore.h"
#include "OOJSPropID.h"
#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "StationEntity.h"
#include "StringExpander.h"
#include "Types.h"
#include "Universe.h"
#include "legacy_random.h"
#include "oofnd/Log.hpp"
#include "oofnd/Object.hpp"
#include "oofnd/PListDescription.hpp"

namespace {

// keys in trade-goods.plist (sort_order, trumble_opinion, comment and
// short_comment are read only by OOCommodityMarket.cpp)
constexpr std::string_view kCommodityName = "name";
constexpr std::string_view kCommodityClasses = "classes";
constexpr std::string_view kCommodityContainer = "quantity_unit";
constexpr std::string_view kCommodityPeakExport = "peak_export";
constexpr std::string_view kCommodityPeakImport = "peak_import";
constexpr std::string_view kCommodityPriceAverage = "price_average";
constexpr std::string_view kCommodityPriceEconomic = "price_economic";
constexpr std::string_view kCommodityPriceRandom = "price_random";
// next one cannot be set from file - named for compatibility
constexpr std::string_view kCommodityPriceCurrent = "price";
constexpr std::string_view kCommodityQuantityAverage = "quantity_average";
constexpr std::string_view kCommodityQuantityEconomic = "quantity_economic";
constexpr std::string_view kCommodityQuantityRandom = "quantity_random";
// next one cannot be set from file - named for compatibility
constexpr std::string_view kCommodityQuantityCurrent = "quantity";
constexpr std::string_view kCommodityLegalityExport = "legality_export";
constexpr std::string_view kCommodityLegalityImport = "legality_import";
constexpr std::string_view kCommodityCapacity = "capacity";
constexpr std::string_view kCommodityScript = "market_script";
// next one cannot be set from file - named for compatibility
constexpr std::string_view kCommodityKey = "key";

// keys in secondary market definitions
constexpr std::string_view kCommodityMarketType = "type";
constexpr std::string_view kCommodityMarketName = "name";
constexpr std::string_view kCommodityMarketPriceAdder = "price_adder";
constexpr std::string_view kCommodityMarketPriceMultiplier = "price_multiplier";
constexpr std::string_view kCommodityMarketPriceRandomiser = "price_randomiser";
constexpr std::string_view kCommodityMarketQuantityAdder = "quantity_adder";
constexpr std::string_view kCommodityMarketQuantityMultiplier =
    "quantity_multiplier";
constexpr std::string_view kCommodityMarketQuantityRandomiser =
    "quantity_randomiser";
constexpr std::string_view kCommodityMarketLegalityExport = "legality_export";
constexpr std::string_view kCommodityMarketLegalityImport = "legality_import";
constexpr std::string_view kCommodityMarketCapacity = "capacity";

// values for "type" in the plist
constexpr std::string_view kCommodityMarketTypeValueDefault = "default";
constexpr std::string_view kCommodityMarketTypeValueClass = "class";
constexpr std::string_view kCommodityMarketTypeValueGood = "good";

// oo_stringForKey: on a commodity's info: a string, or a number's -stringValue;
// nullopt (nil) otherwise.
std::optional<std::string> StringFor(const oo::PList& info,
                                     std::string_view key) {
  const oo::PList* value = info.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return info.get<std::string>(key);
}

// -setObject:forKey: on a copy of an info dictionary (a Dict node; the per-good
// infos are).
void Set(oo::PList& info, std::string_view key, oo::PList value) {
  if (oo::PList::Dict* entries = info.getIf<oo::PList::Dict>())
    (*entries)[std::string(key)] = std::move(value);
}

// -oo_setUnsignedInteger:forKey: / -oo_setInteger:forKey:
// (+numberWithUnsignedInteger: / +numberWithInteger:, which oo::PListFrom reads
// as an unsigned / signed integer).
void SetUnsigned(oo::PList& info, std::string_view key, std::uint64_t value) {
  Set(info, key, oo::PList::unsignedInteger(value));
}

void SetSigned(oo::PList& info, std::string_view key, std::int64_t value) {
  Set(info, key, oo::PList::signedInteger(value));
}

// [array containsObject:string] over a PList array (an equal string node).
bool ContainsString(const oo::PList& array, const std::string& string) {
  const oo::PList::Array* elements = array.getIf<oo::PList::Array>();
  if (elements == nullptr) return false;
  for (const oo::PList& element : *elements) {
    const std::string* text = element.getIf<std::string>();
    if (text != nullptr && *text == string) return true;
  }
  return false;
}

}  // namespace

std::optional<std::string> oo::Commodities::legacyCommodityType(std::size_t i) {
  switch (i) {
    case 0:
      return "food";
    case 1:
      return "textiles";
    case 2:
      return "radioactives";
    case 3:
      return "slaves";
    case 4:
      return "liquor_wines";
    case 5:
      return "luxuries";
    case 6:
      return "narcotics";
    case 7:
      return "computers";
    case 8:
      return "machinery";
    case 9:
      return "alloys";
    case 10:
      return "firearms";
    case 11:
      return "furs";
    case 12:
      return "minerals";
    case 13:
      return "gold";
    case 14:
      return "platinum";
    case 15:
      return "gem_stones";
    case 16:
      return "alien_items";
  }
  // shouldn't happen
  return "food";
}

oo::Commodities::Commodities() {
  // ResourceManager's dictionary API is not migrated: the merged table arrives
  // through oo::PListFrom.
  // TODO: validation of inputs; convert 't', 'kg', 'g' in quantity_unit to 0,
  // 1, 2 (for now it needs them entering as the ints).
  const oo::PList rawCommodityLists = ResourceManager::dictionaryFromFilesNamed(
      "trade-goods.plist", "Config", oo::ResourceMergeMode::kSmart, true);
  if (const oo::PList::Dict* entries =
          rawCommodityLists.getIf<oo::PList::Dict>())
    _commodityLists = *entries;
}

std::shared_ptr<oo::CommodityMarket>
oo::Commodities::generateManifestForPlayer() {
  std::shared_ptr<oo::CommodityMarket> market =
      oo::makeShared<oo::CommodityMarket>();

  for (const auto& [commodity, info] : _commodityLists) {
    // key order
    oo::PList good =
        info.isDict()
            ? info
            : oo::PList(oo::PList::Dict{});  // +dictionaryWithDictionary:
                                             // of the entry
    SetUnsigned(good, kCommodityPriceCurrent, 0);
    SetUnsigned(good, kCommodityQuantityCurrent, 0);
    /* The actual capacity of the player ship is a total, not
     * per-good, so is managed separately through PlayerEntity */
    SetUnsigned(good, kCommodityCapacity, UINT32_MAX);
    Set(good, kCommodityKey, oo::PList(commodity));

    market->setGood(commodity, good);
  }
  return market;
}

std::shared_ptr<oo::CommodityMarket> oo::Commodities::generateBlankMarket() {
  std::shared_ptr<oo::CommodityMarket> market =
      oo::makeShared<oo::CommodityMarket>();

  for (const auto& [commodity, info] : _commodityLists) {
    // key order
    oo::PList good =
        info.isDict()
            ? info
            : oo::PList(oo::PList::Dict{});  // +dictionaryWithDictionary:
                                             // of the entry
    SetUnsigned(good, kCommodityPriceCurrent, 0);
    SetUnsigned(good, kCommodityQuantityCurrent, 0);
    SetUnsigned(good, kCommodityCapacity, 0);
    Set(good, kCommodityKey, oo::PList(commodity));

    market->setGood(commodity, good);
  }
  return market;
}

oo::PList oo::Commodities::createDefinitionFrom(
    const oo::PList& good, oo::CreditsQuantity p, oo::CargoQuantity q,
    const std::string& key, ::StationEntity* station, oo::SystemID system) {
  oo::PList definition = good;
  SetUnsigned(definition, kCommodityPriceCurrent, p);
  SetUnsigned(definition, kCommodityQuantityCurrent, q);
  if (station == nullptr && definition.find(kCommodityCapacity) == nullptr) {
    SetSigned(definition, kCommodityCapacity, MAIN_SYSTEM_MARKET_LIMIT);
  }

  Set(definition, kCommodityKey, oo::PList(key));
  if (station != nullptr &&
      !(station != nullptr ? station->getMarketMonitored() : false)) {
    // clear legal status indicators if the market is not monitored
    SetUnsigned(definition, kCommodityLegalityExport, 0);
    SetUnsigned(definition, kCommodityLegalityImport, 0);
  }

  const std::optional<std::string> goodScriptName =
      StringFor(definition, kCommodityScript);
  if (!goodScriptName.has_value()) {
    return definition;
  }
  ::OOScript* goodScript =
      (PLAYER != nullptr
           ? PLAYER->commodityScriptNamed(goodScriptName)
           : static_cast<OOScript*>(nullptr));  // (has a value: checked above)
  if (goodScript == nullptr) {
    return definition;
  }
  return modifyGood(definition, goodScript, station, system, false);
}

oo::PList oo::Commodities::modifyGood(const oo::PList& good, ::OOScript* script,
                                      ::StationEntity* station,
                                      oo::SystemID system, bool localMode) {
  JSContext* context = OOJSAcquireContext();
  OOJSVal rval = OOJSUndefinedValue();
  // The JS boundary takes and gives property-list objects: the info crosses as
  // one, and an accepted result comes back through oo::PListFrom (an exact
  // round trip).
  OOJSVal args[] = {
      OOJSValueFromPList(context, good),
      station != nullptr
          ? station->jsValueInContext(context)
          : JS_NULL,  // the C++ station's JS value, which
                      // OOJSValueFromNativeObject() of its object reached
      OOJSValueFromDouble(context, system)};
  bool OK = true;
  std::string_view errorType;

  if (localMode) {
    errorType = "local";
    OK = (script != nullptr
              ? script->callMethod(OOJSID("updateLocalCommodityDefinition"),
                                   context, args, 3, &rval)
              : false);
  } else {
    errorType = "general";
    OK = (script != nullptr
              ? script->callMethod(OOJSID("updateGeneralCommodityDefinition"),
                                   context, args, 3, &rval)
              : false);
  }
  OOJSFreeValue(context, args[0]);
  OOJSFreeValue(context, args[1]);
  OOJSFreeValue(context, args[2]);

  if (!OK) {
    OO_LOG("script.commodityScript.error",
           "Could not update {} commodity definition for {} - unable to call "
           "updateLocalCommodityDefinition",
           errorType, StringFor(good, kCommodityName).value_or("(null)"));
    OOJSFreeValue(context, rval);
    OOJSRelinquishContext(context);
    return good;
  }

  if (!JS_IsObject(rval)) {  // Unlike JSVAL_IS_OBJECT(), false for null, which
                             // gets the same message below.
    OO_LOG("script.commodityScript.error",
           "Could not update {} commodity definition for {} - return value "
           "invalid",
           errorType, StringFor(good, kCommodityKey).value_or("(null)"));
    OOJSFreeValue(context, rval);
    OOJSRelinquishContext(context);
    return good;
  }

  oo::PList result = OOJSPListFromJSObject(context, rval);
  OOJSFreeValue(context, rval);
  OOJSRelinquishContext(context);
  if (result.getIf<oo::PList::Dict>() == nullptr) {
    OO_LOG("script.commodityScript.error",
           "Could not update {} commodity definition for {} - return value "
           "invalid",
           errorType, StringFor(good, kCommodityKey).value_or("(null)"));
    return good;
  }

  return result;
}

std::shared_ptr<oo::CommodityMarket>
oo::Commodities::generateMarketForSystemWithEconomy(
    oo::EconomyID economy, const std::optional<std::string>& scriptName) {
  ::OOScript* script =
      (PLAYER != nullptr ? PLAYER->commodityScriptNamed(scriptName)
                         : static_cast<OOScript*>(nullptr));

  std::shared_ptr<oo::CommodityMarket> market =
      oo::makeShared<oo::CommodityMarket>();

  for (const auto& [commodity, info] : _commodityLists) {
    // key order
    oo::PList good = info;
    oo::CargoQuantity q = generateQuantityForGood(good, economy);
    // main system market limited to 127 units of each item
    oo::CargoQuantity cap =
        good.get<unsigned int>(kCommodityCapacity, MAIN_SYSTEM_MARKET_LIMIT);
    if (q > cap) {
      q = cap;
    }
    oo::CreditsQuantity p = generatePriceForGood(good, economy);
    good = createDefinitionFrom(
        good, p, q, commodity, nullptr,
        (UNIVERSE != nullptr ? UNIVERSE->currentSystemID() : 0));

    if (script != nullptr) {
      good = modifyGood(good, script, nullptr,
                        (UNIVERSE != nullptr ? UNIVERSE->currentSystemID() : 0),
                        true);
    }
    market->setGood(commodity, good);
  }
  return market;
}

std::shared_ptr<oo::CommodityMarket> oo::Commodities::generateMarketForStation(
    ::StationEntity* station) {
  const oo::PList marketDefinition =
      (station != nullptr ? station->getMarketDefinition() : oo::PList());
  ::OOScript* marketScript =
      (PLAYER != nullptr
           ? PLAYER->commodityScriptNamed((station != nullptr
                                               ? station->getMarketScriptName()
                                               : std::optional<std::string>()))
           : static_cast<OOScript*>(nullptr));
  if (!marketDefinition && marketScript == nullptr) {
    std::shared_ptr<oo::CommodityMarket> market = generateBlankMarket();
    return market;
  }

  std::shared_ptr<oo::CommodityMarket> market =
      oo::makeShared<oo::CommodityMarket>();
  oo::CargoQuantity capacity =
      (station != nullptr ? station->getMarketCapacity() : 0);
  oo::CommodityMarket* mainMarket =
      (UNIVERSE != nullptr ? UNIVERSE->getCommodityMarket() : nullptr);

  for (const auto& [commodity, info] : _commodityLists) {
    // key order
    oo::PList good = info;
    oo::CargoQuantity baseCapacity =
        good.get<unsigned int>(kCommodityCapacity, MAIN_SYSTEM_MARKET_LIMIT);

    // important - ensure baseCapacity cannot be zero
    if (!baseCapacity) baseCapacity = MAIN_SYSTEM_MARKET_LIMIT;

    oo::CargoQuantity q =
        (mainMarket != nullptr) ? mainMarket->quantityForGood(commodity) : 0;
    oo::CreditsQuantity p =
        (mainMarket != nullptr) ? mainMarket->priceForGood(commodity) : 0;

    if (marketScript == nullptr) {
      const oo::PList* classes = good.get<oo::PList::Array>(kCommodityClasses);
      const oo::PList modifier = firstModifierForGood(
          commodity, (classes != nullptr) ? *classes : oo::PList(),
          marketDefinition);
      good = updateInfoFor(good, modifier, capacity);
      p = adjustPrice(p, modifier);

      // first, scale to this station's capacity for this good
      oo::CargoQuantity localCapacity =
          good.get<unsigned int>(kCommodityCapacity);
      if (localCapacity > capacity) {
        localCapacity = capacity;
      }
      q = (q * localCapacity) / baseCapacity;
      q = adjustQuantity(q, modifier);
      if (q > localCapacity) {
        q = localCapacity;  // cap
      }
    } else {
      // only scale to market at this stage
      q = (q * capacity) / baseCapacity;
    }

    good = createDefinitionFrom(
        good, p, q, commodity, station,
        (UNIVERSE != nullptr ? UNIVERSE->currentSystemID() : 0));
    if (marketScript != nullptr) {
      good = modifyGood(good, marketScript, station,
                        (UNIVERSE != nullptr ? UNIVERSE->currentSystemID() : 0),
                        true);
    }

    market->setGood(commodity, good);
  }
  return market;
}

std::size_t oo::Commodities::count() { return _commodityLists.size(); }

std::vector<std::string> oo::Commodities::goods() {
  // key order (was -allKeys, hash order)
  std::vector<std::string> keys;
  keys.reserve(_commodityLists.size());
  for (const auto& entry : _commodityLists) keys.push_back(entry.first);
  return keys;
}

bool oo::Commodities::goodDefined(const std::string& key) {
  const auto entry = _commodityLists.find(key);
  return entry != _commodityLists.end() && entry->second.isDict();
}

std::optional<std::string> oo::Commodities::goodNamed(const std::string& name) {
  for (const auto& [key, info] : _commodityLists) {
    // key order (was hash order): the first match wins
    const std::optional<std::string> commodityName =
        StringFor(info, kCommodityName);
    const std::optional<std::string> expanded =
        commodityName.has_value() ? OOExpand(*commodityName) : std::nullopt;
    if (expanded == name) {
      return key;
    }
  }
  return std::nullopt;
}

std::string oo::Commodities::getRandomCommodity() {
  // Ranrot() % count indexes the keys in key order (was -allKeys, hash order).
  std::size_t idx = Ranrot() % _commodityLists.size();
  auto entry = _commodityLists.begin();
  std::advance(entry, idx);
  return entry->first;
}

oo::MassUnit oo::Commodities::massUnitForGood(const std::string& good) {
  const auto entry = _commodityLists.find(good);
  if (entry == _commodityLists.end() || !entry->second.isDict()) {
    return oo::MassUnit::kTons;
  }
  return oo::MassUnitFromNumber(
      entry->second.get<unsigned int>(kCommodityContainer));
}

oo::CargoQuantity oo::Commodities::generateQuantityForGood(
    const oo::PList& good, oo::EconomyID economy) {
  float bias = economicBiasForGood(good, economy);

  float base = good.get<float>(kCommodityQuantityAverage);
  float econ = base * good.get<float>(kCommodityQuantityEconomic) * bias;
  // Two draws, in the order clang evaluated the operands of the former (randf()
  // - randf()).
  float firstDraw = randf();
  float secondDraw = randf();
  float random = base * good.get<float>(kCommodityQuantityRandom) *
                 (firstDraw - secondDraw);
  base += econ + random;
  if (base < 0.0) {
    return 0;
  } else {
    return static_cast<oo::CargoQuantity>(base);
  }
}

oo::CreditsQuantity oo::Commodities::generatePriceForGood(
    const oo::PList& good, oo::EconomyID economy) {
  float bias = economicBiasForGood(good, economy);

  float base = good.get<float>(kCommodityPriceAverage);
  float econ = base * good.get<float>(kCommodityPriceEconomic) * -bias;
  // Two draws, in the order clang evaluated the operands of the former (randf()
  // - randf()).
  float firstDraw = randf();
  float secondDraw = randf();
  float random =
      base * good.get<float>(kCommodityPriceRandom) * (firstDraw - secondDraw);
  base += econ + random;
  if (base < 0.0) {
    return 0;
  } else {
    return static_cast<oo::CreditsQuantity>(base);
  }
}

oo::CreditsQuantity oo::Commodities::samplePriceForCommodity(
    const std::string& commodity, oo::EconomyID economy,
    const std::optional<std::string>& scriptName, oo::SystemID system) {
  const auto entry = _commodityLists.find(commodity);
  if (entry == _commodityLists.end() || !entry->second.isDict()) {
    return 0;
  }
  oo::PList good = entry->second;
  oo::CreditsQuantity p = generatePriceForGood(good, economy);

  good = createDefinitionFrom(good, p, 0, commodity, nullptr, system);
  if (scriptName.has_value()) {
    ::OOScript* script =
        (PLAYER != nullptr ? PLAYER->commodityScriptNamed(scriptName)
                           : static_cast<OOScript*>(
                                 nullptr));  // (has a value: checked above)
    if (script != nullptr) {
      good = modifyGood(good, script, nullptr, system, true);
    }
  }
  return good.get<std::uint64_t>(kCommodityPriceCurrent);
}

// positive = exporter; negative = importer; range -1.0 .. +1.0
float oo::Commodities::economicBiasForGood(const oo::PList& good,
                                           oo::EconomyID economy) {
  oo::EconomyID exporter =
      static_cast<oo::EconomyID>(good.get<int>(kCommodityPeakExport));
  oo::EconomyID importer =
      static_cast<oo::EconomyID>(good.get<int>(kCommodityPeakImport));

  // *2 and /2 to work in ints at this stage
  int exDiff = abs(economy - exporter) * 2;
  int imDiff = abs(economy - importer) * 2;
  int distance = (exDiff + imDiff) / 2;

  if (exDiff == imDiff) {
    // neutral economy
    return 0.0;
  } else if (exDiff > imDiff) {
    // closer to the importer, so return -ve
    return static_cast<float>(
        -(1.0 - (static_cast<float>(imDiff) / static_cast<float>(distance))));
  } else {
    // closer to the exporter, so return +ve
    return static_cast<float>(
        1.0 - (static_cast<float>(exDiff) / static_cast<float>(distance)));
  }
}

oo::PList oo::Commodities::firstModifierForGood(const std::string& good,
                                                const oo::PList& classes,
                                                const oo::PList& definitions) {
  std::size_t i;
  for (i = 0; i < definitions.count(); i++) {
    const oo::PList* definition = definitions.at(i);
    if (definition != nullptr && definition->isDict()) {
      const std::string applicationType =
          StringFor(*definition, kCommodityMarketType)
              .value_or(std::string(kCommodityMarketTypeValueDefault));
      const std::string applicationName =
          StringFor(*definition, kCommodityMarketName).value_or(std::string());

      if (applicationType == kCommodityMarketTypeValueDefault ||
          (applicationType == kCommodityMarketTypeValueGood &&
           applicationName == good) ||
          (applicationType == kCommodityMarketTypeValueClass &&
           ContainsString(classes, applicationName))) {
        return *definition;
      }
    }
  }
  // return a blank dictionary - default values will do the rest
  return oo::PList(oo::PList::Dict{});
}

oo::CreditsQuantity oo::Commodities::adjustPrice(oo::CreditsQuantity price,
                                                 const oo::PList& rule) {
  float p =
      static_cast<float>(price);  // work in floats to avoid rounding problems
  float pa = rule.get<float>(kCommodityMarketPriceAdder, 0.0);
  float pm = rule.get<float>(kCommodityMarketPriceMultiplier, 1.0);
  if (pm <= 0.0 && pa <= 0.0) {
    // setting a price multiplier of 0 forces the price to zero
    return 0;
  }
  float pr = rule.get<float>(kCommodityMarketPriceRandomiser, 0.0);
  p += pa;
  p = (p * pm) + (p * pr * (randf() - randf()));
  if (p < 1.0) {
    // random variation and non-zero price multiplier can't reduce
    // price below 1 decicredit
    p = 1.0;
  }
  return static_cast<oo::CreditsQuantity>(p);
}

oo::CargoQuantity oo::Commodities::adjustQuantity(oo::CargoQuantity quantity,
                                                  const oo::PList& rule) {
  float q = static_cast<float>(
      quantity);  // work in floats to avoid rounding problems
  float qa = rule.get<float>(kCommodityMarketQuantityAdder, 0.0);
  float qm = rule.get<float>(kCommodityMarketQuantityMultiplier, 1.0);
  if (qm <= 0.0 && qa <= 0.0) {
    // setting a price multiplier of 0 forces the price to zero
    return 0;
  }
  float qr = rule.get<float>(kCommodityMarketQuantityRandomiser, 0.0);
  q += qa;
  q = (q * qm) + (q * qr * (randf() - randf()));
  if (q < 0.0) {
    // random variation and non-zero price multiplier can't reduce
    // quantity below zero
    q = 0.0;
  }
  // may be over station capacity - that gets capped later
  return static_cast<oo::CargoQuantity>(q);
}

oo::PList oo::Commodities::updateInfoFor(const oo::PList& good,
                                         const oo::PList& rule,
                                         oo::CargoQuantity maxCapacity) {
  oo::PList tmp = good;
  std::int64_t import =
      rule.get<std::int64_t>(kCommodityMarketLegalityImport, -1);
  if (import >= 0) {
    SetSigned(tmp, kCommodityLegalityImport, import);
  }

  std::int64_t exportValue =
      rule.get<std::int64_t>(kCommodityMarketLegalityExport, -1);
  if (exportValue >= 0) {
    SetSigned(tmp, kCommodityLegalityExport,
              import);  // (sic: upstream writes the import value here)
  }

  std::int64_t capacity = rule.get<std::int64_t>(kCommodityMarketCapacity, -1);
  if (capacity >= 0 && capacity <= static_cast<std::int64_t>(maxCapacity)) {
    SetSigned(tmp, kCommodityCapacity, capacity);
  } else {
    // set to the station max capacity
    SetSigned(tmp, kCommodityCapacity, maxCapacity);
  }

  return tmp;
}
