/*

OOCommodityMarket.m

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

#include "CommodityMarket.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "Commodities.h"
#include "StringExpander.h"

namespace {

// The trade-goods.plist keys of OOCommodities.h (kOOCommodityName & co.), as
// C++ strings.
constexpr std::string_view kName = "name";
constexpr std::string_view kContainer = "quantity_unit";
constexpr std::string_view kPriceCurrent = "price";
constexpr std::string_view kQuantityCurrent = "quantity";
constexpr std::string_view kLegalityExport = "legality_export";
constexpr std::string_view kLegalityImport = "legality_import";
constexpr std::string_view kTrumbleOpinion = "trumble_opinion";
constexpr std::string_view kSortOrder = "sort_order";
constexpr std::string_view kCapacity = "capacity";
constexpr std::string_view kComment = "comment";
constexpr std::string_view kShortComment = "short_comment";

// A string element of a saved-game entry, or nullopt where -oo_stringAtIndex:
// gave nil.
std::optional<std::string> SavedGoodKey(const oo::PList& entry) {
  const oo::PList* value = entry.isNull() ? nullptr : entry.at(0);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return entry.at<std::string>(0);
}

}  // namespace

std::size_t oo::CommodityMarket::count() { return _commodityList.size(); }

void oo::CommodityMarket::setGood(const std::string& key,
                                  const oo::PList& info) {
  // A nil info made an empty definition, as +dictionaryWithDictionary:nil did.
  _commodityList[key] = info.isDict() ? info : oo::PList(oo::PList::Dict{});
  _sortedKeys.reset();  // reset
}

std::vector<std::string> oo::CommodityMarket::goods() {
  return sortedGoodKeys();
}

oo::PList oo::CommodityMarket::dictionaryForScripting() {
  return oo::PList(
      oo::PList::Dict(_commodityList.begin(), _commodityList.end()));
}

bool oo::CommodityMarket::setPrice(oo::CreditsQuantity price,
                                   const std::string& good) {
  oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return false;
  }
  (*definition->getIf<oo::PList::Dict>())[std::string(kPriceCurrent)] =
      oo::PList::unsignedInteger(price);
  return true;
}

bool oo::CommodityMarket::setQuantity(oo::CargoQuantity quantity,
                                      const std::string& good) {
  oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr || quantity > capacityForGood(good)) {
    return false;
  }
  (*definition->getIf<oo::PList::Dict>())[std::string(kQuantityCurrent)] =
      oo::PList::unsignedInteger(quantity);
  return true;
}

bool oo::CommodityMarket::addQuantity(oo::CargoQuantity quantity,
                                      const std::string& good) {
  oo::CargoQuantity current = quantityForGood(good);
  if (current + quantity > capacityForGood(good)) {
    return false;
  }
  setQuantity((current + quantity), good);
  return true;
}

bool oo::CommodityMarket::removeQuantity(oo::CargoQuantity quantity,
                                         const std::string& good) {
  oo::CargoQuantity current = quantityForGood(good);
  if (current < quantity) {
    return false;
  }
  setQuantity((current - quantity), good);
  return true;
}

void oo::CommodityMarket::removeAllGoods() {
  for (const auto& entry : _commodityList) {
    setQuantity(0, entry.first);
  }
}

bool oo::CommodityMarket::setComment(const std::string& comment,
                                     const std::string& good) {
  oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return false;
  }
  (*definition->getIf<oo::PList::Dict>())[std::string(kComment)] =
      oo::PList(comment);
  return true;
}

bool oo::CommodityMarket::setShortComment(const std::string& comment,
                                          const std::string& good) {
  oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return false;
  }
  (*definition->getIf<oo::PList::Dict>())[std::string(kShortComment)] =
      oo::PList(comment);
  return true;
}

std::optional<std::string> oo::CommodityMarket::nameForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return OOExpand("[oolite-unknown-commodity-name]");
  }
  return OOExpand(
      definition->get<std::string>(kName, "[oolite-unknown-commodity-name]"));
}

std::optional<std::string> oo::CommodityMarket::commentForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return OOExpand("[oolite-unknown-commodity-name]");
  }
  return OOExpand(
      definition->get<std::string>(kComment, "[oolite-commodity-no-comment]"));
}

std::optional<std::string> oo::CommodityMarket::shortCommentForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return OOExpand("[oolite-unknown-commodity-name]");
  }
  return OOExpand(definition->get<std::string>(
      kShortComment, "[oolite-commodity-no-short-comment]"));
}

oo::CreditsQuantity oo::CommodityMarket::priceForGood(const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  return definition->get<std::uint64_t>(kPriceCurrent);
}

oo::CargoQuantity oo::CommodityMarket::quantityForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  return definition->get<unsigned int>(kQuantityCurrent);
}

oo::MassUnit oo::CommodityMarket::massUnitForGood(const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return oo::MassUnit::kTons;
  }
  return oo::MassUnitFromNumber(definition->get<unsigned int>(kContainer));
}

std::size_t oo::CommodityMarket::exportLegalityForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  return definition->get<std::uint64_t>(kLegalityExport);
}

std::size_t oo::CommodityMarket::importLegalityForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  return definition->get<std::uint64_t>(kLegalityImport);
}

oo::CargoQuantity oo::CommodityMarket::capacityForGood(
    const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  // should only be undefined for main system markets, not secondary stations
  // meaningless for player ship, though
  return definition->get<unsigned int>(kCapacity, MAIN_SYSTEM_MARKET_LIMIT);
}

float oo::CommodityMarket::trumbleOpinionForGood(const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  if (definition == nullptr) {
    return 0;
  }
  return definition->get<float>(kTrumbleOpinion);
}

oo::PList oo::CommodityMarket::definitionForGood(const std::string& good) {
  const oo::PList* definition = definitionPointerForGood(good);
  return definition != nullptr ? *definition : oo::PList();
}

oo::PList oo::CommodityMarket::savePlayerAmounts() {
  oo::PList::Array amounts;
  for (const std::string& good : sortedGoodKeys()) {
    amounts.push_back(oo::PList(oo::PList::Array{
        oo::PList(good), oo::PList::unsignedInteger(quantityForGood(good))}));
  }
  return oo::PList(std::move(amounts));
}

void oo::CommodityMarket::loadPlayerAmounts(const oo::PList& amounts) {
  oo::CargoQuantity q;
  bool loadedOK;
  for (const std::string& good : sortedGoodKeys()) {
    // make sure that any goods not defined in the save game are zeroed
    setQuantity(0, good);
  }

  const oo::PList::Array* loadedAmounts = amounts.getIf<oo::PList::Array>();
  for (const oo::PList& loaded :
       loadedAmounts != nullptr ? *loadedAmounts : oo::PList::Array()) {
    loadedOK = false;
    const std::optional<std::string> good = SavedGoodKey(loaded);
    q = loaded.at<unsigned int>(1);
    // old save games might have more in the array, but we don't care
    if (!good.has_value() || !setQuantity(q, *good)) {
      // then it's an array from a 1.80-or-earlier save game and
      // the good name is the description string (maybe a
      // translated one)
      for (const std::string& key : sortedGoodKeys()) {
        if (good.has_value() && good == nameForGood(key)) {
          setQuantity(q, key);
          loadedOK = true;
          break;
        }
      }
    } else {
      loadedOK = true;
    }
    if (!loadedOK) {
      OO_LOG("setCommanderDataFromDictionary.warning.cargo",
             "Cargo {} ({} units) could not be loaded from the saved game, as "
             "it is no longer defined",
             good.value_or("(null)"), q);
    }
  }
}

oo::PList oo::CommodityMarket::saveStationAmounts() {
  oo::PList::Array amounts;
  for (const std::string& good : sortedGoodKeys()) {
    amounts.push_back(oo::PList(oo::PList::Array{
        oo::PList(good), oo::PList::unsignedInteger(quantityForGood(good)),
        oo::PList::unsignedInteger(priceForGood(good))}));
  }
  return oo::PList(std::move(amounts));
}

void oo::CommodityMarket::loadStationAmounts(const oo::PList& amounts) {
  oo::CargoQuantity q;
  oo::CreditsQuantity p;
  bool loadedOK;

  const oo::PList::Array* loadedAmounts = amounts.getIf<oo::PList::Array>();
  for (const oo::PList& loaded :
       loadedAmounts != nullptr ? *loadedAmounts : oo::PList::Array()) {
    loadedOK = false;
    const std::optional<std::string> good = SavedGoodKey(loaded);
    q = loaded.at<unsigned int>(1);
    p = loaded.at<std::uint64_t>(2);
    // old save games might have more in the array, but we don't care
    if (!good.has_value() || !setQuantity(q, *good)) {
      // then it's an array from a 1.80-or-earlier save game and
      // the good name is the description string (maybe a
      // translated one)
      for (const std::string& key : sortedGoodKeys()) {
        if (good.has_value() && good == nameForGood(key)) {
          setQuantity(q, key);
          setPrice(p, key);
          loadedOK = true;
          break;
        }
      }
    } else {
      setPrice(p, *good);
      loadedOK = true;
    }
    if (!loadedOK) {
      OO_LOG("load.warning.cargo",
             "Station market good {} ({} units) could not be loaded from the "
             "saved game, as it is no longer defined",
             good.value_or("(null)"), q);
    }
  }
}

oo::PList* oo::CommodityMarket::definitionPointerForGood(
    const std::string& good) {
  const auto it = _commodityList.find(good);
  return it != _commodityList.end() ? &it->second : nullptr;
}

// The goods in sort_order; goods of equal sort_order in byte order of their key
// (the Foundation version sorted -allKeys, in hash order).
std::vector<std::string> oo::CommodityMarket::sortedGoodKeys() {
  if (!_sortedKeys.has_value()) {
    std::vector<std::string> keys;
    keys.reserve(_commodityList.size());
    for (const auto& entry : _commodityList) keys.push_back(entry.first);
    std::stable_sort(
        keys.begin(), keys.end(),
        [this](const std::string& a, const std::string& b) {
          return _commodityList.find(a)->second.get<int>(kSortOrder) <
                 _commodityList.find(b)->second.get<int>(kSortOrder);
        });
    _sortedKeys = std::move(keys);
  }
  return *_sortedKeys;
}
