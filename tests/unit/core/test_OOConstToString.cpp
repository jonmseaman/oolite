/*	test_OOConstToString.mm
        Unit tests for the OOConstToString functions that message converted
   classes. OOConstToString.mm has no class; it calls OOEquipmentType and
   OOCommodityMarket members and sends to UNIVERSE.

        Pinned: a weapon type's identifier (none for no weapon, "EQ_WEAPON_NONE"
   for the string form), the sloppy lookup that tries an "EQ_" prefix and falls
   back to EQ_WEAPON_NONE, its strict, string and legacy (save-game number)
   forms, and the mass unit of a commodity in the universe's market (tons when
   there is no market or no such good).

        The game around them is replaced, as in test_OOEquipmentType: UNIVERSE
   hands out the equipment data, the descriptions and a market; the cache
   manager, script loader, player and standards functions are the test's.
   OOEquipmentType, OOCommodityMarket and OOColor are the game's own. Run:
   tests/run.sh unit
*/

#include <cstdlib>
#include <string>

#include <gtest/gtest.h>

#include "CommodityMarket.h"
#include "ConstToString.h"
#include "EquipmentType.h"
#include "StringExpander.h"

// ShipEntity.h's declarations (it imports the whole game).
namespace oo {

typedef oo::EquipmentType* WeaponType;
std::optional<std::string> EquipmentIdentifierFromWeaponType(
    WeaponType weapon);
WeaponType WeaponTypeFromEquipmentIdentifierSloppy(
    const std::string& string);
WeaponType WeaponTypeFromEquipmentIdentifierStrict(
    const std::string& string);
WeaponType WeaponTypeFromEquipmentIdentifierLegacy(
    const std::string& string);
std::optional<std::string> StringFromWeaponType(WeaponType weapon);
WeaponType WeaponTypeFromString(const std::string& string);

}  // namespace oo


// --- The game around the functions
// -----------------------------------------------------------

namespace {

oo::PList Item(int techLevel, int price, const char* name, const char* key) {
  return oo::PList(oo::PList::Array{oo::PList(techLevel), oo::PList(price),
                                    oo::PList(name), oo::PList(key),
                                    oo::PList("about it")});
}

oo::PList EquipmentData() {
  return oo::PList(oo::PList::Array{
      Item(1, 0, "No weapon", "EQ_WEAPON_NONE"),
      Item(2, 4000, "Pulse laser", "EQ_WEAPON_PULSE_LASER"),
      Item(5, 10000, "Beam laser", "EQ_WEAPON_BEAM_LASER"),
      Item(3, 300, "Fuel", "EQ_FUEL"),
      Item(4, 500, "Laser without prefix", "LASER_WITHOUT_PREFIX"),
  });
}

oo::PList Good(
    std::initializer_list<std::pair<const char*, oo::PList>> entries) {
  oo::PList::Dict dict;
  for (const auto& [key, value] : entries) dict[key] = value;
  return oo::PList(std::move(dict));
}

}  // namespace

#include "UniverseStandIn.h"

// The universe as the code under test sees it: a stand-in for the C++ class
// with the header's names and signatures. It answers virtual members, so it has
// the game's vtable (UniverseStandIn.h) and StandInUniverse overrides them.
class Universe : public oo::Object {
 public:
  OO_UNIVERSE_STAND_IN_VIRTUAL_MEMBERS

  std::shared_ptr<oo::CommodityMarket> market;

  oo::PList getEquipmentData();
  oo::PList getEquipmentDataOutfitting();
  ::oo::CommodityMarket* getCommodityMarket();
};

class StandInUniverse : public Universe {
 public:
  const oo::PList* descriptions() override {
    static const oo::PList descriptions = oo::PList(oo::PList::Dict{});
    return &descriptions;
  }
  std::optional<std::string> descriptionForKey(
      const std::string& /*key*/) override {
    return std::nullopt;
  }
};

oo::PList Universe::getEquipmentData() { return EquipmentData(); }
oo::PList Universe::getEquipmentDataOutfitting() { return EquipmentData(); }
::oo::CommodityMarket* Universe::getCommodityMarket() { return market.get(); }

Universe* gSharedUniverse = nullptr;

// The mass-unit symbols, as descriptions.plist spells them.
namespace oo {

std::string LookUpDescriptionPRIV(const std::string& key) {
  if (key == "cargo-tons-symbol") return "t";
  if (key == "cargo-kilograms-symbol") return "kg";
  if (key == "cargo-grams-symbol") return "g";
  return key;
}

// The expander (a whole-game unit); the functions pinned here never expand.
std::optional<std::string> ExpandDescriptionString(
    Random_Seed /*seed*/, const std::string& string,
    const oo::PList& /*overrides*/, const oo::PList& /*legacyLocals*/,
    const std::optional<std::string>& /*systemName*/,
    ExpandOptions /*options*/) {
  return string;
}

Random_Seed StringExpanderDefaultRandomSeed(void) { return Random_Seed(); }

}  // namespace oo


#include "CacheManager.h"

// The cache manager: a C++ stand-in.
oo::CacheManager* oo::CacheManager::sharedCache() {
  static oo::CacheManager* shared = new oo::CacheManager;
  return shared;
}
namespace oo {

CacheManager::~CacheManager() {}

}  // namespace oo

void oo::CacheManager::setPList(const oo::PList& value, const std::string& key,
                              const std::string& cache) {
  (void)value;
  (void)key;
  (void)cache;
}

#include "OOScript.h"

// OOScript's members that OOEquipmentType.mm calls, and the rest of its
// virtual members, for the vtable.
std::optional<std::string> OOScript::descriptionComponents() {
  return std::nullopt;
}
std::optional<std::string> OOScript::name() { return std::nullopt; }
std::optional<std::string> OOScript::scriptDescription() {
  return std::nullopt;
}
std::optional<std::string> OOScript::version() { return std::nullopt; }
bool OOScript::requiresTickle() { return false; }
void OOScript::runWithTarget(Entity*) {}
bool OOScript::callMethod(JSAtom, JSContext*, JSValueConst*, int, JSValue*) {
  return false;
}
std::string OOScript::className() const { return "OOScript"; }
std::string OOScript::description() const { return "<OOScript>"; }
JSValue OOScript::jsValueInContext(JSContext*) {
  std::abort();
}  // never reached (no engine is linked)
void OOScript::clearJSSelf(JSValueConst) {}
void OOScriptAutorelease(std::shared_ptr<OOScript>) {}
std::shared_ptr<OOScript> OOScript::jsScriptFromFileNamed(const std::string&,
                                                  const oo::PList&) {
  return nullptr;
}

// PLAYER: the members the code under test calls, declared as PlayerEntity.h
// declares them (the test imports no game header that defines the class), with
// the same answers.
class PlayerEntity {
 public:
  oo::PList processKeyCode(const oo::PList& key_def);
  std::optional<std::string> validateKey(const std::string& key,
                                         const oo::PList& check_keys);
  oo::PList missionVariableForKey(const std::string& key);
};

oo::PList PlayerEntity::processKeyCode(const oo::PList& key_def) {
  return key_def;
}
std::optional<std::string> PlayerEntity::validateKey(
    const std::string& /*key*/, const oo::PList& /*check_keys*/) {
  return std::nullopt;
}
oo::PList PlayerEntity::missionVariableForKey(const std::string& /*key*/) {
  return oo::PList();
}

PlayerEntity* gOOPlayer = nullptr;

namespace oo {

void StandardsDeprecated(const std::string& /*message*/) {}

bool EnforceStandards(void) { return false; }

}  // namespace oo


oo::PList OOSanitizeLegacyScriptConditions(
    const oo::PList& conditions,
    const std::optional<std::string>& /*context*/) {
  return conditions;
}

namespace {

void LoadEquipment() {
  if (gSharedUniverse == nullptr) gSharedUniverse = new StandInUniverse();
  if (gOOPlayer == nullptr) gOOPlayer = new PlayerEntity;  // never deleted
  oo::EquipmentType::loadEquipment();
}

std::string IdentifierOf(oo::WeaponType weapon) {
  return weapon != nullptr ? weapon->identifier().value_or("-") : "nil";
}

}  // namespace

// --- Weapon types
// ----------------------------------------------------------------------------

TEST(OOConstToString, weaponTypeIdentifiers) {
  {
    LoadEquipment();
    oo::WeaponType pulse =
        oo::EquipmentType::equipmentTypeWithIdentifier("EQ_WEAPON_PULSE_LASER")
            .get();
    EXPECT_TRUE(pulse != nullptr);

    EXPECT_TRUE(oo::StringFromWeaponType(pulse) ==
             std::optional<std::string>("EQ_WEAPON_PULSE_LASER"));
    EXPECT_TRUE(oo::StringFromWeaponType(nullptr) ==
             std::optional<std::string>("EQ_WEAPON_NONE"));

    EXPECT_TRUE(oo::EquipmentIdentifierFromWeaponType(pulse) ==
             std::optional<std::string>("EQ_WEAPON_PULSE_LASER"));
    // A message to nil answered a zeroed std::optional: no identifier.
    EXPECT_TRUE(oo::EquipmentIdentifierFromWeaponType(nullptr) == std::nullopt);
  }
}

TEST(OOConstToString, sloppyLookUp) {
  {
    LoadEquipment();
    oo::WeaponType pulse =
        oo::EquipmentType::equipmentTypeWithIdentifier("EQ_WEAPON_PULSE_LASER")
            .get();
    oo::WeaponType none =
        oo::EquipmentType::equipmentTypeWithIdentifier("EQ_WEAPON_NONE").get();
    EXPECT_TRUE(pulse != nullptr && none != nullptr && pulse != none);

    // The identifier itself, then with "EQ_" put in front, then EQ_WEAPON_NONE.
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy(
                 "EQ_WEAPON_PULSE_LASER") == pulse);
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy(
                 "WEAPON_PULSE_LASER") == pulse);
    EXPECT_TRUE(
        oo::WeaponTypeFromEquipmentIdentifierSloppy("LASER_WITHOUT_PREFIX") ==
        oo::EquipmentType::equipmentTypeWithIdentifier("LASER_WITHOUT_PREFIX")
            .get());
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy("EQ_FUEL") ==
             oo::EquipmentType::equipmentTypeWithIdentifier("EQ_FUEL").get());
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy("PULSE_LASER") ==
             none);
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy(
                 "EQ_NO_SUCH_THING") == none);
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierSloppy("") == none);

    // The other spellings are the same look-up.
    EXPECT_TRUE(
        oo::WeaponTypeFromEquipmentIdentifierStrict("WEAPON_BEAM_LASER") ==
        oo::EquipmentType::equipmentTypeWithIdentifier("EQ_WEAPON_BEAM_LASER")
            .get());
    EXPECT_TRUE(oo::WeaponTypeFromString("WEAPON_PULSE_LASER") == pulse);
    EXPECT_TRUE(oo::WeaponTypeFromString("nonsense") == none);

    // Save games stored weapons as numbers.
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierLegacy("2") == pulse);
    EXPECT_TRUE(IdentifierOf(oo::WeaponTypeFromEquipmentIdentifierLegacy("3")) ==
             "EQ_WEAPON_BEAM_LASER");
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierLegacy("4") ==
             none);  // no mining laser here
    EXPECT_TRUE(oo::WeaponTypeFromEquipmentIdentifierLegacy("EQ_FUEL") ==
             oo::EquipmentType::equipmentTypeWithIdentifier("EQ_FUEL").get());
  }
}

// --- Mass units
// ------------------------------------------------------------------------------

TEST(OOConstToString, massUnitOfACommodity) {
  {
    LoadEquipment();

    // No market: a message to nil answered 0, OOMassUnit::kTons.
    gSharedUniverse->market = nullptr;
    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("gold") ==
             std::optional<std::string>("t"));

    std::shared_ptr<oo::CommodityMarket> market = oo::makeShared<oo::CommodityMarket>();
    market->setGood("gold", Good({{"name", oo::PList("Gold")},
                                  {"quantity_unit", oo::PList(1)}}));
    market->setGood("gems", Good({{"name", oo::PList("Gem-stones")},
                                  {"quantity_unit", oo::PList(2)}}));
    market->setGood("food", Good({{"name", oo::PList("Food")},
                                  {"quantity_unit", oo::PList(0)}}));
    market->setGood("alloys", Good({{"name", oo::PList("Alloys")},
                                    {"quantity_unit", oo::PList(7)}}));
    gSharedUniverse->market = market;

    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("gold") ==
             std::optional<std::string>("kg"));
    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("gems") ==
             std::optional<std::string>("g"));
    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("food") ==
             std::optional<std::string>("t"));
    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("unobtainium") ==
             std::optional<std::string>("t"));
    EXPECT_TRUE(DisplayStringForMassUnitForCommodity("alloys") ==
             DisplayStringForMassUnit(oo::MassUnit::kUnknown));

    gSharedUniverse->market = nullptr;
  }
}

// The JS glue of OOEquipmentType (OOJSPrivateObject), defined in
// OOJSEquipmentInfo.mm, which this test does not link: the vtable names these.
JSValue oo::EquipmentType::jsValueInContext(JSContext*) { return JS_UNDEFINED; }
void oo::EquipmentType::clearJSSelf(JSValueConst) {}
std::optional<std::string> oo::EquipmentType::jsDescription() {
  return std::nullopt;
}
