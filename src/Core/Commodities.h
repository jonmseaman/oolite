/*

OOCommodities.h

Commodity price and quantity manager

The universe
holds it as std::shared_ptr, and the markets it makes are
std::shared_ptr<OOCommodityMarket>.

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

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Types.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

inline constexpr int MAIN_SYSTEM_MARKET_LIMIT = 127;

// A trade-goods.plist quantity_unit as an OOMassUnit: 0-2 are the units,
// anything else is OOMassUnit::kUnknown (a bare cast would be undefined for
// those values).
namespace oo {

inline MassUnit MassUnitFromNumber(unsigned n) {
  return (n <= oo::ToUnderlying(MassUnit::kGrams)) ? static_cast<MassUnit>(n)
                                                   : MassUnit::kUnknown;
}

}  // namespace oo

class StationEntity;
class OOScript;

namespace oo {

class CommodityMarket;

class Commodities : public oo::Object {
 public:
  Commodities();  // reads trade-goods.plist

  // always a key (the old method never returned nil)
  static std::optional<std::string> legacyCommodityType(std::size_t i);

  std::shared_ptr<CommodityMarket> generateManifestForPlayer();
  std::shared_ptr<CommodityMarket> generateBlankMarket();
  // nullopt: no script (was nil)
  std::shared_ptr<CommodityMarket> generateMarketForSystemWithEconomy(
      EconomyID economy, const std::optional<std::string>& scriptName);
  std::shared_ptr<CommodityMarket> generateMarketForStation(
      ::StationEntity* station);

  CreditsQuantity samplePriceForCommodity(
      const std::string& commodity, EconomyID economy,
      const std::optional<std::string>& scriptName, SystemID system);

  std::size_t count();
  std::vector<std::string> goods();  // commodity keys, in key order
  bool goodDefined(const std::string& key);
  // nullopt: no good has that (expanded) name
  std::optional<std::string> goodNamed(const std::string& name);
  std::string getRandomCommodity();  // a commodity key
  MassUnit massUnitForGood(const std::string& good);

 private:
  oo::PList modifyGood(const oo::PList& good, ::OOScript* script,
                       ::StationEntity* station, SystemID system, bool local);
  oo::PList createDefinitionFrom(const oo::PList& good, CreditsQuantity p,
                                 CargoQuantity q, const std::string& key,
                                 ::StationEntity* station, SystemID system);

  CargoQuantity generateQuantityForGood(const oo::PList& good,
                                        EconomyID economy);
  CreditsQuantity generatePriceForGood(const oo::PList& good,
                                       EconomyID economy);

  float economicBiasForGood(const oo::PList& good, EconomyID economy);
  oo::PList firstModifierForGood(const std::string& good,
                                 const oo::PList& classes,
                                 const oo::PList& definitions);
  CreditsQuantity adjustPrice(CreditsQuantity price, const oo::PList& rule);
  CargoQuantity adjustQuantity(CargoQuantity quantity, const oo::PList& rule);
  oo::PList updateInfoFor(const oo::PList& good, const oo::PList& rule,
                          CargoQuantity maxCapacity);

  // trade-goods.plist: commodity key -> its info (a Dict)
  std::map<std::string, oo::PList, std::less<>> _commodityLists;
};

}  // namespace oo
