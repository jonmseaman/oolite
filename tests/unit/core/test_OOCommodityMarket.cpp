/*	test_OOCommodityMarket.mm
        Unit tests for OOCommodityMarket (src/Core/OOCommodityMarket.h).

        A market is a table of goods, each a trade-goods.plist definition (a
   dictionary): the goods in sort_order, prices and quantities set within the
   good's capacity (127 where it has none), the definitions' texts expanded, the
   mass unit, legality and trumble opinion read back, and the player's and a
   station's amounts saved and loaded, including 1.80-era saves that name goods
   by their display name. The string expander is replaced: it returns its input
   in angle brackets. The tests ask the C++ class (held as a std::shared_ptr); the last
   tests pin the C++ API. Run: tests/run.sh unit
*/

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "CommodityMarket.h"
#include "StringExpander.h"

// The expander: "<string>". A test replacement, not a stub: the names and
// comments go through it.
namespace oo {

std::optional<std::string> ExpandDescriptionString(
    Random_Seed /*seed*/, const std::string& string,
    const oo::PList& /*overrides*/, const oo::PList& /*legacyLocals*/,
    const std::optional<std::string>& /*systemName*/,
    ExpandOptions /*options*/) {
  return "<" + string + ">";
}

Random_Seed StringExpanderDefaultRandomSeed(void) { return Random_Seed{}; }

}  // namespace oo


namespace {

oo::PList Good(
    std::initializer_list<std::pair<const char*, oo::PList>> entries) {
  oo::PList::Dict dict;
  for (const auto& [key, value] : entries) dict[key] = value;
  return oo::PList(std::move(dict));
}

// food (sort 1), furs and gems (both sort 2: key order), alloys (sort 0),
// radioactives (no sort order: 0, and after alloys by key).
std::shared_ptr<oo::CommodityMarket> Market() {
  std::shared_ptr<oo::CommodityMarket> market = oo::makeShared<oo::CommodityMarket>();
  market->setGood("gems", Good({{"name", oo::PList("Gem-stones")},
                                {"sort_order", oo::PList(2)},
                                {"quantity_unit", oo::PList(2)},
                                {"price", oo::PList(500)},
                                {"quantity", oo::PList(3)},
                                {"capacity", oo::PList(10)}}));
  market->setGood("food", Good({{"name", oo::PList("Food")},
                                {"sort_order", oo::PList(1)},
                                {"quantity_unit", oo::PList(0)},
                                {"price", oo::PList(40)},
                                {"quantity", oo::PList(20)},
                                {"trumble_opinion", oo::PList(1.0)},
                                {"comment", oo::PList("Tasty")},
                                {"short_comment", oo::PList("yum")}}));
  market->setGood("furs", Good({{"name", oo::PList("Furs")},
                                {"sort_order", oo::PList(2)},
                                {"quantity_unit", oo::PList(1)},
                                {"price", oo::PList(560)},
                                {"quantity", oo::PList(0)},
                                {"legality_export", oo::PList(1)},
                                {"legality_import", oo::PList(2)},
                                {"trumble_opinion", oo::PList(0.5)}}));
  market->setGood("alloys", Good({{"name", oo::PList("Alloys")},
                                  {"sort_order", oo::PList(0)},
                                  {"quantity_unit", oo::PList(7)}}));
  market->setGood("radioactives", oo::PList("not a dictionary"));
  return market;
}

using Goods = std::vector<std::string>;

}  // namespace

TEST(OOCommodityMarket, goodsAndDefinitions) {
  {
    std::shared_ptr<oo::CommodityMarket> m = Market();
    EXPECT_TRUE(m->count() == 5);
    EXPECT_TRUE(m->goods() ==
             Goods({"alloys", "radioactives", "food", "furs", "gems"}));
    EXPECT_TRUE(m->dictionaryForScripting().count() == 5);
    EXPECT_TRUE(m->definitionForGood("radioactives") ==
             oo::PList(oo::PList::Dict{}));  // not a dictionary: an empty one
    EXPECT_TRUE(m->definitionForGood("unobtainium").isNull());
    EXPECT_TRUE(m->definitionForGood("furs").get<int>("legality_import") == 2);

    // Names and comments, expanded; the defaults for a missing text or good.
    EXPECT_TRUE(m->nameForGood("food") == std::optional<std::string>("<Food>"));
    EXPECT_TRUE(m->nameForGood("radioactives") ==
             std::optional<std::string>("<[oolite-unknown-commodity-name]>"));
    EXPECT_TRUE(m->nameForGood("unobtainium") ==
             std::optional<std::string>("<[oolite-unknown-commodity-name]>"));
    EXPECT_TRUE(m->commentForGood("food") ==
             std::optional<std::string>("<Tasty>"));
    EXPECT_TRUE(m->commentForGood("furs") ==
             std::optional<std::string>("<[oolite-commodity-no-comment]>"));
    EXPECT_TRUE(m->commentForGood("unobtainium") ==
             std::optional<std::string>("<[oolite-unknown-commodity-name]>"));
    EXPECT_TRUE(m->shortCommentForGood("food") ==
             std::optional<std::string>("<yum>"));
    EXPECT_TRUE(
        m->shortCommentForGood("furs") ==
        std::optional<std::string>("<[oolite-commodity-no-short-comment]>"));
    EXPECT_TRUE(m->shortCommentForGood("unobtainium") ==
             std::optional<std::string>("<[oolite-unknown-commodity-name]>"));

    // Numbers, and what an unknown good answers.
    EXPECT_TRUE(m->priceForGood("furs") == 560 && m->priceForGood("alloys") == 0 &&
             m->priceForGood("unobtainium") == 0);
    EXPECT_TRUE(m->quantityForGood("food") == 20 &&
             m->quantityForGood("unobtainium") == 0);
    EXPECT_TRUE(m->massUnitForGood("food") == oo::MassUnit::kTons &&
             m->massUnitForGood("furs") == oo::MassUnit::kKilograms &&
             m->massUnitForGood("gems") == oo::MassUnit::kGrams);
    EXPECT_TRUE(m->massUnitForGood("alloys") == oo::MassUnit::kUnknown &&
             m->massUnitForGood("unobtainium") == oo::MassUnit::kTons);
    EXPECT_TRUE(m->exportLegalityForGood("furs") == 1 &&
             m->importLegalityForGood("furs") == 2);
    EXPECT_TRUE(m->exportLegalityForGood("food") == 0 &&
             m->importLegalityForGood("unobtainium") == 0);
    EXPECT_TRUE(m->capacityForGood("gems") == 10 &&
             m->capacityForGood("food") == MAIN_SYSTEM_MARKET_LIMIT &&
             m->capacityForGood("unobtainium") == 0);
    EXPECT_TRUE(m->trumbleOpinionForGood("food") == 1.0f &&
             m->trumbleOpinionForGood("furs") == 0.5f &&
             m->trumbleOpinionForGood("gems") == 0.0f);
    EXPECT_TRUE(m->trumbleOpinionForGood("unobtainium") == 0.0f);

    // A new good resorts; replacing one keeps the count.
    m->setGood("aardvarks", Good({{"sort_order", oo::PList(1)}}));
    EXPECT_TRUE(m->goods() == Goods({"alloys", "radioactives", "aardvarks", "food",
                                  "furs", "gems"}));
    m->setGood("gems", Good({{"sort_order", oo::PList(-1)}}));
    EXPECT_TRUE(m->count() == 6 && m->goods().front() == "gems");
  }
}

TEST(OOCommodityMarket, pricesAndQuantities) {
  {
    std::shared_ptr<oo::CommodityMarket> m = Market();
    EXPECT_TRUE(m->setPrice(99, "food") && m->priceForGood("food") == 99);
    EXPECT_TRUE(!m->setPrice(99, "unobtainium"));

    EXPECT_TRUE(m->setQuantity(10, "gems") && m->quantityForGood("gems") == 10);
    EXPECT_TRUE(!m->setQuantity(11, "gems") &&
             m->quantityForGood("gems") == 10);  // over capacity
    EXPECT_TRUE(!m->setQuantity(1, "unobtainium"));
    EXPECT_TRUE(!m->addQuantity(1, "gems"));
    EXPECT_TRUE(m->removeQuantity(4, "gems") && m->quantityForGood("gems") == 6);
    EXPECT_TRUE(m->addQuantity(4, "gems") && m->quantityForGood("gems") == 10);
    EXPECT_TRUE(!m->removeQuantity(11, "gems") &&
             m->quantityForGood("gems") == 10);
    EXPECT_TRUE(m->addQuantity(107, "food") && m->quantityForGood("food") == 127);
    EXPECT_TRUE(!m->addQuantity(1, "food"));
    EXPECT_TRUE(!m->addQuantity(1, "unobtainium") &&
             !m->removeQuantity(1, "unobtainium"));

    EXPECT_TRUE(m->setComment("New", "furs") &&
             m->commentForGood("furs") == std::optional<std::string>("<New>"));
    EXPECT_TRUE(m->setShortComment("new", "furs") &&
             m->shortCommentForGood("furs") ==
                 std::optional<std::string>("<new>"));
    EXPECT_TRUE(!m->setComment("x", "unobtainium") &&
             !m->setShortComment("x", "unobtainium"));

    m->removeAllGoods();
    for (const std::string& good : m->goods())
      EXPECT_TRUE(m->quantityForGood(good) == 0);
    EXPECT_TRUE(m->priceForGood("food") == 99);
  }
}

TEST(OOCommodityMarket, savedAmounts) {
  {
    std::shared_ptr<oo::CommodityMarket> m = Market();
    EXPECT_TRUE(m->savePlayerAmounts() ==
             oo::PList(oo::PList::Array{
                 oo::PList(oo::PList::Array{oo::PList("alloys"),
                                            oo::PList::unsignedInteger(0)}),
                 oo::PList(oo::PList::Array{oo::PList("radioactives"),
                                            oo::PList::unsignedInteger(0)}),
                 oo::PList(oo::PList::Array{oo::PList("food"),
                                            oo::PList::unsignedInteger(20)}),
                 oo::PList(oo::PList::Array{oo::PList("furs"),
                                            oo::PList::unsignedInteger(0)}),
                 oo::PList(oo::PList::Array{oo::PList("gems"),
                                            oo::PList::unsignedInteger(3)})}));
    EXPECT_TRUE(m->saveStationAmounts().at(2) != nullptr &&
             *m->saveStationAmounts().at(2) ==
                 oo::PList(oo::PList::Array{oo::PList("food"),
                                            oo::PList::unsignedInteger(20),
                                            oo::PList::unsignedInteger(40)}));

    // Loading zeroes the goods a save does not mention; a 1.80 save names a
    // good by its (expanded) name; a good that no longer exists, or too much of
    // one, is skipped.
    std::shared_ptr<oo::CommodityMarket> player = Market();
    player->loadPlayerAmounts(oo::PList(oo::PList::Array{
        oo::PList(oo::PList::Array{oo::PList("furs"), oo::PList(5)}),
        oo::PList(oo::PList::Array{oo::PList("<Gem-stones>"), oo::PList(2),
                                   oo::PList("extra")}),
        oo::PList(oo::PList::Array{oo::PList("unobtainium"), oo::PList(1)}),
        oo::PList(oo::PList::Array{oo::PList("alloys"), oo::PList(500)}),
        oo::PList(oo::PList::Array{oo::PList(7), oo::PList(1)})}));
    EXPECT_TRUE(player->quantityForGood("furs") == 5 &&
             player->quantityForGood("gems") == 2);
    EXPECT_TRUE(player->quantityForGood("food") == 0 &&
             player->quantityForGood("alloys") == 0);
    player->loadPlayerAmounts(oo::PList());
    EXPECT_TRUE(player->quantityForGood("furs") == 0);

    std::shared_ptr<oo::CommodityMarket> station = Market();
    station->loadStationAmounts(oo::PList(oo::PList::Array{
        oo::PList(
            oo::PList::Array{oo::PList("furs"), oo::PList(5), oo::PList(600)}),
        oo::PList(
            oo::PList::Array{oo::PList("<Food>"), oo::PList(6), oo::PList(41)}),
        oo::PList(oo::PList::Array{oo::PList("unobtainium"), oo::PList(1),
                                   oo::PList(1)})}));
    EXPECT_TRUE(station->quantityForGood("furs") == 5 &&
             station->priceForGood("furs") == 600);
    EXPECT_TRUE(station->quantityForGood("food") == 6 &&
             station->priceForGood("food") == 41);
    EXPECT_TRUE(station->quantityForGood("gems") == 3 &&
             station->priceForGood("gems") == 500);  // untouched
  }
}

TEST(OOCommodityMarket, cxxClass) {
  std::shared_ptr<oo::CommodityMarket> m = oo::makeShared<oo::CommodityMarket>();
  EXPECT_TRUE(m->count() == 0 && m->goods().empty() &&
           m->savePlayerAmounts() == oo::PList(oo::PList::Array{}));
  m->setGood("gems", Good({{"name", oo::PList("Gem-stones")},
                           {"sort_order", oo::PList(2)},
                           {"capacity", oo::PList(10)},
                           {"price", oo::PList(500)}}));
  m->setGood("food",
             Good({{"name", oo::PList("Food")}, {"sort_order", oo::PList(1)}}));
  EXPECT_TRUE(m->goods() == Goods({"food", "gems"}));
  EXPECT_TRUE(m->setQuantity(10, "gems") && !m->addQuantity(1, "gems") &&
           m->removeQuantity(3, "gems"));
  EXPECT_TRUE(m->quantityForGood("gems") == 7 && m->priceForGood("gems") == 500 &&
           m->capacityForGood("food") == MAIN_SYSTEM_MARKET_LIMIT);
  EXPECT_TRUE(m->nameForGood("gems") ==
           std::optional<std::string>("<Gem-stones>"));
  EXPECT_TRUE(m->definitionForGood("unobtainium").isNull());
  EXPECT_TRUE(m->saveStationAmounts() ==
           oo::PList(oo::PList::Array{
               oo::PList(oo::PList::Array{oo::PList("food"),
                                          oo::PList::unsignedInteger(0),
                                          oo::PList::unsignedInteger(0)}),
               oo::PList(oo::PList::Array{oo::PList("gems"),
                                          oo::PList::unsignedInteger(7),
                                          oo::PList::unsignedInteger(500)})}));
}

TEST(OOCommodityMarket, facadeIdentity) {
  {
    // A C++ market sees its goods.
    std::shared_ptr<oo::CommodityMarket> cxxMarket = oo::makeShared<oo::CommodityMarket>();
    cxxMarket->setGood("food", Good({{"price", oo::PList(3)}}));
    EXPECT_TRUE(cxxMarket->priceForGood("food") == 3);
    cxxMarket->setPrice(4, "food");
    EXPECT_TRUE(cxxMarket->priceForGood("food") == 4);

    EXPECT_TRUE(oo::makeShared<oo::CommodityMarket>() !=
             oo::makeShared<oo::CommodityMarket>());  // distinct objects, as before
  }
}
