/*	test_PlayerEntity.mm
	Unit tests for PlayerEntity (src/Core/Entities/PlayerEntity.h), the player's ship: the class
	shell, which moves the player's state into PlayerEntity (a subclass of the ship's).

	Like the ship's, the player's object needs the game graph, so the test links the whole game but
	main (['*']) and uses a Universe that was never initialised and an empty JavaScript context for
	the events the ship sends its scripts. The player is made in two steps, as the game makes it:
	-init (+sharedPlayer) early in start-up, before there is ship data, and -deferredInit once there
	is. Setting the player up from shipdata, the session and the controls (PlayerEntityControls)
	need the game's data, so a subclass stands in for those three and only counts; the cases test
	the class's own units: sharedPlayer(), the player made (the ship made the player's way, and the
	only player), deferredInit() (the ship's initialiser sent again over the same object, and the
	player's own defaults), and the object's -dealloc (it releases what the player held). The player
	is C++, made as the game makes it (oo::makeShared), and the stand-ins are C++ subclasses
	overriding the members; the test
	reads and sets the members directly. The case after those pins the crossing: the player's object
	is a ship whose C++ part is the player, and it answers the player's selectors called by name.
	Run: bash meson test -C build/meson_test test_PlayerEntity
*/

#include <gtest/gtest.h>

#include "PlayerEntity.h"
#include "WormholeEntity.h"
#include "MyOpenGLView.h"
#include "ConstToString.h"
#include "Universe.h"
#include "PlayerEntityContracts.h"
#include "PlayerEntityKeyMapper.h"
#include "PlayerEntityStickMapper.h"
#include "JoystickManager.h"
#include "CallByName.h"	// oo::EntityRespondsToName / oo::CallEntityByName

#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Defaults.hpp"

#include <cmath>
#include <string>
#include "EntityManualRefs.h"  // the tests' manual entity references


// The player's data members are private: the test reaches each one it reads or sets through
// this friend, one accessor per member answering a reference, so every check reads and writes
// exactly what it did.
#define OO_PLAYER_MEMBER(name)	static auto &name(PlayerEntity *player)	{ return player->name; }

// A one-bit member has no reference: its accessor answers a proxy that reads as the member's value
// and assigns through to it.
template <class Get, class Set>
struct PlayerBitMember
{
	Get get;
	Set set;
	operator unsigned() const						{ return get(); }
	PlayerBitMember &operator=(unsigned value)		{ set(value); return *this; }
};
template <class Get, class Set>
PlayerBitMember<Get, Set> MakePlayerBitMember(Get get, Set set)	{ return { get, set }; }
#define OO_PLAYER_BIT(name)	static auto name(PlayerEntity *player)	{ return MakePlayerBitMember([player]() -> unsigned { return player->name; }, [player](unsigned value) { player->name = value; }); }

struct PlayerEntityTestAccess
{
	OO_PLAYER_MEMBER(ANA_mode)
	OO_PLAYER_MEMBER(_missionBackgroundSpecial)
	OO_PLAYER_MEMBER(aftViewOffset)
	OO_PLAYER_MEMBER(alertFlags)
	OO_PLAYER_MEMBER(cdrDetailArray)
	OO_PLAYER_MEMBER(chart_centre_coordinates)
	OO_PLAYER_MEMBER(chart_focus_coordinates)
	OO_PLAYER_MEMBER(chart_zoom)
	OO_PLAYER_MEMBER(compassTarget)
	OO_PLAYER_MEMBER(contracts)
	OO_PLAYER_MEMBER(credits)
	OO_PLAYER_MEMBER(current_cargo)
	OO_PLAYER_MEMBER(cursor_coordinates)
	OO_PLAYER_MEMBER(customDialSettings)
	OO_PLAYER_MEMBER(customViewOffset)
	OO_PLAYER_MEMBER(custom_chart_centre_coordinates)
	OO_PLAYER_MEMBER(custom_chart_zoom)
	OO_PLAYER_MEMBER(dockingClearanceStatus)
	OO_PLAYER_MEMBER(dockingReport)
	OO_PLAYER_MEMBER(eqScripts)
	OO_PLAYER_MEMBER(extraMissionKeys)
	OO_PLAYER_MEMBER(fleeing_status)
	OO_PLAYER_MEMBER(forwardViewOffset)
	OO_PLAYER_MEMBER(galaxy_coordinates)
	OO_PLAYER_MEMBER(galaxy_number)
	OO_PLAYER_MEMBER(gui_screen)
	OO_PLAYER_MEMBER(hud)
	OO_PLAYER_MEMBER(hyperspeedFactor)
	OO_PLAYER_MEMBER(info_system_id)
	OO_PLAYER_MEMBER(isSpeechOn)
	OO_PLAYER_MEMBER(legalStatusValue)
	OO_PLAYER_MEMBER(market_rnd)
	OO_PLAYER_MEMBER(maxFieldOfView)
	OO_PLAYER_MEMBER(max_passengers)
	OO_PLAYER_MEMBER(missile_entity)
	OO_PLAYER_MEMBER(mission_variables)
	OO_PLAYER_MEMBER(multiFunctionDisplaySettings)
	OO_PLAYER_MEMBER(multiFunctionDisplayText)
	OO_PLAYER_MEMBER(passengers)
	OO_PLAYER_MEMBER(pitch_delta)
	OO_PLAYER_MEMBER(planetSearchString)
	OO_PLAYER_MEMBER(portViewOffset)
	OO_PLAYER_MEMBER(reputation)
	OO_PLAYER_MEMBER(roll_delta)
	OO_PLAYER_MEMBER(save_path)
	OO_PLAYER_MEMBER(scenarioKey)
	OO_PLAYER_MEMBER(ship_clock)
	OO_PLAYER_MEMBER(ship_kills)
	OO_PLAYER_MEMBER(ship_trade_in_factor)
	OO_PLAYER_MEMBER(shipyard_record)
	OO_PLAYER_MEMBER(system_id)
	OO_PLAYER_MEMBER(target_memory_index)
	OO_PLAYER_MEMBER(target_system_id)
	OO_PLAYER_MEMBER(worldScripts)
	OO_PLAYER_MEMBER(wormhole)
	OO_PLAYER_MEMBER(yaw_delta)
	OO_PLAYER_BIT(afterburner_engaged)
	OO_PLAYER_BIT(autopilot_engaged)
	OO_PLAYER_BIT(galactic_witchjump)
	OO_PLAYER_BIT(hyperspeed_engaged)
	OO_PLAYER_BIT(ident_engaged)
	OO_PLAYER_BIT(scoopsActive)
	OO_PLAYER_BIT(suppressTargetLostFlag)
	OO_PLAYER_BIT(travelling_at_hyperspeed)
	OO_PLAYER_BIT(using_mining_laser)
};

#undef OO_PLAYER_MEMBER
#undef OO_PLAYER_BIT



// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

extern PlayerEntity *gOOPlayer;
extern Universe *gSharedUniverse;
extern JSContext *gOOJSMainThreadContext;


namespace {

// What the stand-ins were sent.
int sShipSetUps = 0;
oo::PList sShipSetUpDict;
int sPlayerSetUps = 0;
bool sPlayerSetUpStopOnError = true;
int sInitControls = 0;

}	// namespace

// A player whose set-up from shipdata, session set-up and controls only count
// (those need the game's data).
class TestPlayer : public PlayerEntity
{
public:
	using PlayerEntity::setUpAndConfirmOK;

	bool setUpShipFromDictionary(const oo::PList &dict) override
	{
		sShipSetUps++;
		sShipSetUpDict = dict;
		return true;
	}


	bool setUpAndConfirmOK(bool stopOnError) override
	{
		sPlayerSetUps++;
		sPlayerSetUpStopOnError = stopOnError;
		return true;
	}


	void initControls() override
	{
		sInitControls++;
	}
};


// A ship that stands in for a missile on a pylon.
class TestMissile : public ShipEntity
{
public:
	bool setUpShipFromDictionary(const oo::PList &dict) override	{ (void)dict; return true; }
};


namespace {

void SetUp()
{
	static Universe *universe = nullptr;
	if (universe == nullptr)
	{
		universe = new Universe();	// never released
	}
	gSharedUniverse = universe;
	gOOPlayer = nullptr;	// making the player expects to make the only one
	// The ship's -dealloc sends its (absent) scripts entityDestroyed in a request on the main
	// thread's context, so there is one, with nothing in it.
	if (gOOJSMainThreadContext == nullptr)  gOOJSMainThreadContext = JS_NewContext(JS_NewRuntime());
	sShipSetUps = 0;
	sShipSetUpDict = oo::PList();
	sPlayerSetUps = 0;
	sPlayerSetUpStopOnError = true;
	sInitControls = 0;
}


#ifndef NDEBUG
// deferredInit() sends the ship's initialiser, and so the root's -init, a second time, which counts
// the entity again, as it did.
void UncountSecondInit(Entity *entity)
{
	gLiveEntityCount--;
	gTotalEntityMemory -= oo::EntityObjectSize(entity);	// its object's size
}
#endif


// A player made as the game makes one (PlayerEntity::newPlayerObject()): the C++ player, which the
// caller releases (+1).
template <class T>
T *NewTestPlayer()
{
	std::shared_ptr<T> player = oo::makeShared<T>();
	{
		oo::AutoreleaseScope scope;
		oo::RetainEntity(oo::MadeEntity(player));
	}
	return player.get();
}


void Release(PlayerEntity *player)
{
	oo::ReleaseEntity(player);
}

}	// namespace


// The player made: the ship made the player's way (no initialiser but the root's), not set up from
// shipdata, not yet the player, and not yet the shared player.
TEST(PlayerEntity, initIsTheShipMadeThePlayersWay)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = NewTestPlayer<TestPlayer>();
		EXPECT_TRUE(player != nullptr);
		EXPECT_TRUE(sShipSetUps == 0 && sPlayerSetUps == 0 && sInitControls == 0);
		EXPECT_TRUE(!player->getIsShip() && !player->getIsPlayer());
		EXPECT_TRUE(player->status() == oo::EntityStatus::kCockpitDisplay);
		EXPECT_TRUE(player->shipDataKey() == std::nullopt);
		EXPECT_TRUE(player->temperature() == 0 && player->weaponRechargeRate() == 0);
		EXPECT_TRUE(PlayerEntityTestAccess::maxFieldOfView(player) == 0);
		EXPECT_TRUE(gOOPlayer == nullptr);
		Release(player);
	}
}


// sharedPlayer(): made once, a PlayerEntity, and kept.
TEST(PlayerEntity, sharedPlayer)
{
	PlayerEntity *player = nullptr;
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		player = PlayerEntity::sharedPlayer();
		EXPECT_TRUE(player != nullptr && typeid(*player) == typeid(PlayerEntity));
		EXPECT_TRUE(gOOPlayer == player);
		EXPECT_TRUE(PlayerEntity::sharedPlayer() == player);
		EXPECT_TRUE(!player->getIsPlayer() && player->status() == oo::EntityStatus::kCockpitDisplay);
	}
	gOOPlayer = nullptr;	// the next case makes its own; this one stays in the never-deleted holder
}


// deferredInit(): the ship's initialiser sent again with the player's ship key and an empty
// definition, which keeps the object (and its C++ part) and runs the ship's body over it; then
// the player's own defaults, the session set-up, and the controls.
TEST(PlayerEntity, deferredInit)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = NewTestPlayer<TestPlayer>();
		gOOPlayer = player;
		Entity *asEntity = player;
		void *part = asEntity;
		player->setFuel(5);
		PlayerEntityTestAccess::save_path(player) = std::string("some.oolite-save");

		player->deferredInit();
		EXPECT_TRUE((void *)asEntity == part);
		EXPECT_TRUE(sShipSetUps == 1 && sShipSetUpDict.isDict() && sShipSetUpDict.getIf<oo::PList::Dict>()->empty());
		EXPECT_TRUE(player->shipDataKey() == std::optional<std::string>(std::string(PLAYER_SHIP_DESC)));
		EXPECT_TRUE(player->getIsShip() && player->getIsPlayer());
		EXPECT_TRUE(player->status() == oo::EntityStatus::kStartGame);
		EXPECT_TRUE(player->temperature() == SHIP_MIN_CABIN_TEMP && player->weaponRechargeRate() == 6.0f);
		EXPECT_TRUE(player->getFuel() == 5);	// what the bodies do not set stays
		EXPECT_TRUE(std::fabs(PlayerEntityTestAccess::maxFieldOfView(player) - MAX_FOV) < 1e-6);
		EXPECT_TRUE(player->getCompassMode() == oo::CompassMode::kBasic);
		EXPECT_TRUE(!PlayerEntityTestAccess::scoopsActive(player) && PlayerEntityTestAccess::target_memory_index(player) == 0);
		EXPECT_TRUE(!PlayerEntityTestAccess::save_path(player).has_value());
		for (int i = 0; i < PLAYER_MAX_MISSILES; i++)  EXPECT_TRUE(PlayerEntityTestAccess::missile_entity(player)[i] == nullptr);
		EXPECT_TRUE(sPlayerSetUps == 1 && sPlayerSetUpStopOnError == false);
		EXPECT_TRUE(sInitControls == 1);

		gOOPlayer = nullptr;
#ifndef NDEBUG
		UncountSecondInit(asEntity);
#endif
		Release(player);
	}
}


// The object's -dealloc releases what the player held (here, a missile on a pylon), and the ship's
// -dealloc runs after it.
TEST(PlayerEntity, deallocReleasesWhatThePlayerHeld)
{
	// The missile's object (+1, as +alloc/-init's was); nothing autoreleases it
  // again.
	Entity *missile = nullptr;
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		missile = oo::RetainedEntity(oo::NewShipObject(oo::makeShared<TestMissile>(), "missile", oo::PList()));
		TestPlayer *player = NewTestPlayer<TestPlayer>();
		PlayerEntityTestAccess::missile_entity(player)[2] = oo::AdoptHold(oo::RetainEntity(missile));	// held by oo::EntityHold
		EXPECT_TRUE(missile->weak_from_this().use_count() == 2);
		EXPECT_TRUE(PlayerEntityTestAccess::missile_entity(player)[2].get() == missile);
		Release(player);
	}
	EXPECT_TRUE(missile->weak_from_this().use_count() == 1);
	oo::ReleaseEntity(missile);
}


// The crossing: the player's object is a ship whose C++ part is the player; the
// player answers the player's selectors called by name, which a ship does not.
TEST(PlayerEntity, playerObjectIsAShipWhosePartIsThePlayer)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = NewTestPlayer<TestPlayer>();
		Entity *object = player;	// the drawable
		EXPECT_TRUE(object != nullptr && oo::ToShip(object) != nullptr);	// a ship's object
		EXPECT_TRUE(object == player);
		EXPECT_TRUE(dynamic_cast<PlayerEntity *>(object) == player);
		EXPECT_TRUE(oo::EntityRespondsToName(player, "commanderName_string"));
		EXPECT_TRUE(oo::EntityRespondsToName(player, "credits_number"));
		EXPECT_TRUE(!oo::EntityRespondsToName(player, "noSuchPlayerSelector"));

		::ShipEntity *ship = static_cast<TestMissile *>(oo::ToShip(oo::RetainedEntity(oo::NewShipObject(oo::makeShared<TestMissile>(), "missile", oo::PList()))));
		EXPECT_TRUE(ship != nullptr && !oo::EntityRespondsToName(ship, "commanderName_string"));
		oo::ReleaseEntity(ship);
		Release(player);
	}
}


// Every member starts zeroed, as the runtime zeroed the ivars.
TEST(PlayerEntity, membersStartZeroed)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = NewTestPlayer<TestPlayer>();
		PlayerEntity *part = player;
		EXPECT_TRUE(PlayerEntityTestAccess::hud(part) == nullptr && PlayerEntityTestAccess::compassTarget(part).lock().get() == nullptr && PlayerEntityTestAccess::wormhole(part) == nullptr);
		EXPECT_TRUE(PlayerEntityTestAccess::system_id(part) == 0 && PlayerEntityTestAccess::ship_clock(part) == 0 && PlayerEntityTestAccess::scoopsActive(part) == false);
		for (int i = 0; i < PLAYER_MAX_MISSILES; i++)  EXPECT_TRUE(PlayerEntityTestAccess::missile_entity(part)[i] == nullptr);
		EXPECT_TRUE(!PlayerEntityTestAccess::save_path(part).has_value() && PlayerEntityTestAccess::worldScripts(part).empty());
		Release(player);
	}
}


// --- Cargo pods, commodity data and credits, galaxy and chart
// coordinates, the current and previous system
// ------------------------------------------------------------------

namespace {

// A player made the way sharedPlayer() makes one, but not the shared player (no ship data); its
// object is autoreleased.
TestPlayer *MakePlayer()
{
	gOOPlayer = nullptr;
	TestPlayer *player = NewTestPlayer<TestPlayer>();
	oo::AutoreleaseEntity(player);
	return player;
}

bool Near(NSPoint a, NSPoint b)	{ return std::fabs(a.x - b.x) < 1e-4 && std::fabs(a.y - b.y) < 1e-4; }

}	// namespace


// The player's ship can't be renamed; the base mass falls back to the Cobra III's while the ship has
// none.
TEST(PlayerEntity, slice2NameAndBaseMass)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		const std::optional<std::string> before = player->getName();
		player->setName(std::string("Renamed"));
		EXPECT_TRUE(player->getName() == before);
		EXPECT_TRUE(player->baseMass() == 185580.0f);
	}
}


// The plain accessors read and write the members.
TEST(PlayerEntity, slice2Accessors)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		EXPECT_TRUE(player->getShipCommodityData() == nullptr);
		PlayerEntityTestAccess::credits(part) = 1234;
		EXPECT_TRUE(player->deciCredits() == 1234);
		player->setRandom_factor(77);
		EXPECT_TRUE(player->random_factor() == 77 && PlayerEntityTestAccess::market_rnd(part) == 77);
		PlayerEntityTestAccess::galaxy_number(part) = 3;
		EXPECT_TRUE(player->galaxyNumber() == 3);
		player->setGalaxyCoordinates(NSMakePoint(12.5, 99.0));
		EXPECT_TRUE(Near(player->getGalaxy_coordinates(), NSMakePoint(12.5, 99.0)));
		PlayerEntityTestAccess::cursor_coordinates(part) = NSMakePoint(4, 5);
		PlayerEntityTestAccess::chart_centre_coordinates(part) = NSMakePoint(6, 7);
		EXPECT_TRUE(Near(player->getCursor_coordinates(), NSMakePoint(4, 5)) && Near(player->getChart_centre_coordinates(), NSMakePoint(6, 7)));
		player->setCustomChartZoom(2.5);
		EXPECT_TRUE(player->getCustom_chart_zoom() == 2.5);
		player->setCustomChartCentre(NSMakePoint(30, 40));
		EXPECT_TRUE(Near(player->getCustom_chart_centre_coordinates(), NSMakePoint(30, 40)));
		PlayerEntityTestAccess::ANA_mode(part) = oo::RouteType::kTime;
		EXPECT_TRUE(player->ANAMode() == oo::RouteType::kTime);
		PlayerEntityTestAccess::system_id(part) = 42;
		EXPECT_TRUE(player->systemID() == 42);
		player->setPreviousSystemID(17);
		EXPECT_TRUE(player->previousSystemID() == 17);
	}
}


// The chart's zoom and centre follow a mission screen's chart background.
TEST(PlayerEntity, slice2ChartZoomAndCentre)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::chart_zoom(part) = 1.0;
		PlayerEntityTestAccess::custom_chart_zoom(part) = 3.0;
		PlayerEntityTestAccess::galaxy_coordinates(part) = NSMakePoint(20, 30);
		PlayerEntityTestAccess::custom_chart_centre_coordinates(part) = NSMakePoint(50, 60);
		PlayerEntityTestAccess::chart_centre_coordinates(part) = NSMakePoint(100, 110);
		PlayerEntityTestAccess::chart_focus_coordinates(part) = NSMakePoint(100, 110);

		PlayerEntityTestAccess::_missionBackgroundSpecial(part) = oo::GUIBackgroundSpecial::kNone;
		EXPECT_TRUE(player->getChart_zoom() == 1.0);
		EXPECT_TRUE(Near(player->adjusted_chart_centre(), NSMakePoint(100, 110)));
		PlayerEntityTestAccess::_missionBackgroundSpecial(part) = oo::GUIBackgroundSpecial::kShort;
		EXPECT_TRUE(player->getChart_zoom() == 1.0);
		EXPECT_TRUE(Near(player->adjusted_chart_centre(), NSMakePoint(20, 30)));
		PlayerEntityTestAccess::_missionBackgroundSpecial(part) = oo::GUIBackgroundSpecial::kLongAnaQuickest;
		EXPECT_TRUE(player->getChart_zoom() == (oo::Scalar)CHART_MAX_ZOOM);
		EXPECT_TRUE(Near(player->adjusted_chart_centre(), NSMakePoint(128, 128)));
		PlayerEntityTestAccess::_missionBackgroundSpecial(part) = oo::GUIBackgroundSpecial::kCustom;
		EXPECT_TRUE(player->getChart_zoom() == 3.0);
		EXPECT_TRUE(Near(player->adjusted_chart_centre(), NSMakePoint(50, 60)));
	}
}


// Unloading a commodity with no pods in the hold takes it from the manifest (here, none).
TEST(PlayerEntity, slice2UnloadWithNoPods)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		player->unloadCargoPodsForType("food", 3);
		player->unloadAllCargoPodsForType("food", nullptr);
		EXPECT_TRUE(player->cargo.empty());
	}
}

// --- Target, next-hop and info systems, the wormhole, the commander
// data dictionary
// ---------------------------------------------------------------------------------

// The target and info systems; with no advanced navigational array the next hop
// is the target.
TEST(PlayerEntity, slice3TargetAndInfoSystems)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::system_id(part) = 7;
		PlayerEntityTestAccess::target_system_id(part) = 9;
		PlayerEntityTestAccess::info_system_id(part) = 11;
		PlayerEntityTestAccess::ANA_mode(part) = oo::RouteType::kJumps;
		EXPECT_TRUE(player->targetSystemID() == 9);
		EXPECT_TRUE(player->nextHopTargetSystemID() == 9);
		EXPECT_TRUE(player->infoSystemID() == 11);
		player->setInfoSystemID(11, true);	// the same system: nothing changes
		EXPECT_TRUE(player->infoSystemID() == 11);
	}
}


// The wormhole is retained while the player holds it, and released when replaced.
TEST(PlayerEntity, slice3Wormhole)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		EXPECT_TRUE(player->getWormhole() == nullptr);
		// A C++ wormhole: the player retains it.
		std::shared_ptr<WormholeEntity> holeRef = oo::makeShared<WormholeEntity>();
		Entity *object = oo::RetainEntity(oo::MadeEntity(holeRef));
		WormholeEntity *hole = holeRef.get();
		const long count = object->weak_from_this().use_count();
		player->setWormhole(hole);
		EXPECT_TRUE(player->getWormhole() == hole && object->weak_from_this().use_count() == count + 1);
		player->setWormhole(nullptr);
		EXPECT_TRUE(player->getWormhole() == nullptr && object->weak_from_this().use_count() == count);
		oo::ReleaseEntity(object);
	}
}

// --- Setting the commander data from a dictionary
// ---------------------

// The multi-function displays and dials are reset first; a dictionary without
// the required keys is refused.
TEST(PlayerEntity, slice4RefusesADictionaryWithoutTheRequiredKeys)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::multiFunctionDisplayText(part)["mfd"] = "text";
		PlayerEntityTestAccess::multiFunctionDisplaySettings(part).push_back(std::string("mfd"));
		PlayerEntityTestAccess::customDialSettings(part)["dial"] = oo::PList(1);
		EXPECT_TRUE(!player->setCommanderDataFromDictionary(oo::PList(oo::PList::Dict{})));
		EXPECT_TRUE(PlayerEntityTestAccess::multiFunctionDisplayText(part).empty() && PlayerEntityTestAccess::multiFunctionDisplaySettings(part).empty() && PlayerEntityTestAccess::customDialSettings(part).empty());
		EXPECT_TRUE(!player->setCommanderDataFromDictionary(oo::PList(oo::PList::Dict{ { "ship_desc", oo::PList(std::string("cobra3-player")) } })));
	}
}

// --- Set-up and start-up, ship set-up from the dictionary, the
// session, warning about hostiles
// -----------------------------------------------------------------------

// The player always belongs to the current session, and collides only in
// flight.
TEST(PlayerEntity, slice5SessionAndCollisions)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		EXPECT_TRUE(player->sessionID() == (UNIVERSE != nullptr ? UNIVERSE->sessionID() : 0));
		player->setStatus(oo::EntityStatus::kInFlight);
		EXPECT_TRUE(player->canCollide());
		player->setStatus(oo::EntityStatus::kDocked);
		EXPECT_TRUE(!player->canCollide());
		player->setStatus(oo::EntityStatus::kDead);
		EXPECT_TRUE(!player->canCollide());
		player->setStatus(oo::EntityStatus::kExitingWitchspace);
		EXPECT_TRUE(player->canCollide());
	}
}

// --- Sun glare, the atmosphere, update
// --------------------------------

// The player is always the nearest entity and may always be added; outside an
// atmosphere the fraction inside it is zero.
TEST(PlayerEntity, slice6NearestValidAndAtmosphere)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		EXPECT_TRUE(player->compareZeroDistance(player) == oo::ComparisonResult::kDescending);
		EXPECT_TRUE(player->validForAddToUniverse());
		EXPECT_TRUE(player->insideAtmosphereFraction() == 0.0f);
		EXPECT_TRUE(player->lookingAtSunWithThresholdAngleCos(0.5f) == 0.0f);	// no sun
	}
}

// --- The bookkeeping tick, movement flags
// ------------------------------

// The movement flags compare the position and orientation with the last
// frame's, which they then become.
TEST(PlayerEntity, slice7MovementFlags)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		Entity *entity = player;
		entity->position = make_HPvector(1, 2, 3);
		entity->lastPosition = kZeroHPVector;
		entity->orientation = kIdentityQuaternion;
		entity->lastOrientation = kIdentityQuaternion;
		player->updateMovementFlags();
		EXPECT_TRUE(entity->hasMoved && !entity->hasRotated);
		EXPECT_TRUE(HPvector_equal(entity->lastPosition, entity->position));
		player->updateMovementFlags();
		EXPECT_TRUE(!entity->hasMoved && !entity->hasRotated);
	}
}

// --- Alert conditions, mass lock, fuel scoops, clocks, script and
// trumble ticks, the autopilot and docking requests
// ---------------------------------------------------

// The flight limits also set the player's control rates; without the autopilot
// engaged or a station, disengaging and cancelling a docking request change
// nothing.
TEST(PlayerEntity, slice8FlightLimitsAndAutopilot)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		player->setMaxFlightPitch(1.5f);
		player->setMaxFlightRoll(2.0f);
		player->setMaxFlightYaw(0.5f);
		EXPECT_TRUE(player->max_flight_pitch == 1.5f && PlayerEntityTestAccess::pitch_delta(part) == 3.0f);
		EXPECT_TRUE(player->max_flight_roll == 2.0f && PlayerEntityTestAccess::roll_delta(part) == 4.0f);
		EXPECT_TRUE(player->max_flight_yaw == 0.5f && PlayerEntityTestAccess::yaw_delta(part) == 1.0f);
		player->setStatus(oo::EntityStatus::kInFlight);
		player->disengageAutopilot();
		EXPECT_TRUE(!PlayerEntityTestAccess::autopilot_engaged(part) && player->status() == oo::EntityStatus::kInFlight);
		player->cancelDockingRequest(nullptr);
		EXPECT_TRUE(player->status() == oo::EntityStatus::kInFlight);
	}
}

// --- The autopilot AI, hyperspeed, the per-status updates, game over,
// targeting
// ----------------------------------------------------------------------------------

// The injector and hyperspeed flags and the hyperspeed factor read the members.
TEST(PlayerEntity, slice9HyperspeedFlags)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		EXPECT_TRUE(!player->injectorsEngaged() && !player->hyperspeedEngaged());
		PlayerEntityTestAccess::afterburner_engaged(part) = true;
		PlayerEntityTestAccess::hyperspeed_engaged(part) = true;
		EXPECT_TRUE(player->injectorsEngaged() && player->hyperspeedEngaged());
#if OO_VARIABLE_TORUS_SPEED
		PlayerEntityTestAccess::hyperspeedFactor(part) = 4.5f;
		EXPECT_TRUE(player->getHyperspeedFactor() == 4.5f);
#endif
	}
}

// --- Attitude, view matrices and viewpoints, drawing, mass lock, the
// docked station, the HUD and its custom dials, shield levels
// ----------------------------------------

// The normal orientation is the orientation with w negated, both ways; the
// flags and levels read and write the members; the shield level is clamped to
// its maximum.
TEST(PlayerEntity, slice10OrientationFlagsAndShields)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		Quaternion q = make_quaternion(0.5, 0.5, 0.5, 0.5);
		player->setNormalOrientation(q);
		Quaternion n = player->normalOrientation();
		EXPECT_TRUE(n.w == q.w && n.x == q.x && n.y == q.y && n.z == q.z);
		EXPECT_TRUE(player->getOrientation().w == -q.w);
		player->setOcclusionLevel(0.75f);
		EXPECT_TRUE(player->occlusionLevel() == 0.75f);
		player->setShowDemoShips(true);
		EXPECT_TRUE(player->getShowDemoShips());
		PlayerEntityTestAccess::travelling_at_hyperspeed(part) = true;
		EXPECT_TRUE(player->atHyperspeed());
		PlayerEntityTestAccess::alertFlags(part) = oo::ToUnderlying(oo::AlertFlags::kMassLock);
		EXPECT_TRUE(player->massLocked());
		PlayerEntityTestAccess::alertFlags(part) = 0;
		EXPECT_TRUE(!player->massLocked());
		EXPECT_TRUE(!player->getMassLockable());
		player->setMaxForwardShieldLevel(100.0f);
		EXPECT_TRUE(player->maxForwardShieldLevel() == 100.0f);
		player->setForwardShieldLevel(250.0f);
		EXPECT_TRUE(player->forwardShieldLevel() == 100.0f);
		player->setForwardShieldLevel(-5.0f);
		EXPECT_TRUE(player->forwardShieldLevel() == 0.0f);
	}
}


// The roll and speed dials are fractions of the maxima, clamped.
TEST(PlayerEntity, slice10Dials)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		player->max_flight_roll = 2.0f;
		player->flightRoll = 1.0f;
		EXPECT_TRUE(player->dialRoll() == 0.5f);
		player->flightRoll = -5.0f;
		EXPECT_TRUE(player->dialRoll() == -1.0f);
		player->maxFlightSpeed = 100.0f;
		player->flightSpeed = 250.0f;
		EXPECT_TRUE(player->dialSpeed() == 1.0f);
		player->flightSpeed = 25.0f;
		EXPECT_TRUE(player->dialSpeed() == 0.25f);
	}
}

// --- The dials, fuel leak, the comm log, player roles, system
// memory, the compass target
// ---------------------------------------------------------------------------------

// The clock, its adjustment, the rescue time and the fuel leak (never negative)
// read and write the members; the number of player roles grows with the kills.
TEST(PlayerEntity, slice11ClockFuelLeakAndRoles)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::ship_clock(part) = 1000.0;
		EXPECT_TRUE(player->clockTime() == 1000.0 && !player->clockAdjusting());
		player->addToAdjustTime(60.0);
		EXPECT_TRUE(player->clockAdjusting() && player->clockTimeAdjusted() == 1060.0);
		player->setEscapePodRescueTime(42.0);
		EXPECT_TRUE(player->escapePodRescueTime() == 42.0);
		player->setFuelLeakRate(-1.0f);
		EXPECT_TRUE(player->fuelLeakRate() == 0.0f);
		player->setFuelLeakRate(3.0f);
		EXPECT_TRUE(player->fuelLeakRate() == 3.0f);
		PlayerEntityTestAccess::ship_kills(part) = 0;
		EXPECT_TRUE(player->maxPlayerRoles() == 8);
		PlayerEntityTestAccess::ship_kills(part) = 128;
		EXPECT_TRUE(player->maxPlayerRoles() == 16);
		PlayerEntityTestAccess::ship_kills(part) = 6400;
		EXPECT_TRUE(player->maxPlayerRoles() == 32);
	}
}


// The missile count counts the occupied pylons up to the ship's maximum.
TEST(PlayerEntity, slice11CountMissiles)
{
	TestMissile *missile = nullptr;
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		missile = static_cast<TestMissile *>(oo::ToShip(oo::RetainedEntity(oo::NewShipObject(oo::makeShared<TestMissile>(), "missile", oo::PList()))));
		TestPlayer *player = MakePlayer();
		player->max_missiles = 4;
		EXPECT_TRUE(player->countMissiles() == 0);
		PlayerEntityTestAccess::missile_entity(player)[1] = oo::AdoptHold(oo::RetainEntity(missile));	// held by oo::EntityHold
		PlayerEntityTestAccess::missile_entity(player)[3] = oo::AdoptHold(oo::RetainEntity(missile));	// held by oo::EntityHold
		EXPECT_TRUE(player->countMissiles() == 2);
		player->max_missiles = 2;
		EXPECT_TRUE(player->countMissiles() == 1);
	}
	if (missile != nullptr)  oo::ReleaseEntity(missile);
}

// --- Compass mode, missiles and pylons, special cargo, the
// multi-function displays, alert flags
// ---------------------------------------------------------------------------

// The compass mode, the ident flag, the maximum missiles and the alert flags
// read and write the members.
TEST(PlayerEntity, slice12CompassIdentAndAlertFlags)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		player->setCompassMode(oo::CompassMode::kTarget);
		EXPECT_TRUE(player->getCompassMode() == oo::CompassMode::kTarget);
		player->setDialIdentEngaged((bool)3);
		EXPECT_TRUE(player->dialIdentEngaged());
		player->max_missiles = 5;
		EXPECT_TRUE(player->dialMaxMissiles() == 5);
		player->setAlertFlag(oo::AlertFlags::kMassLock, true);
		EXPECT_TRUE((player->getAlertFlags() & oo::ToUnderlying(oo::AlertFlags::kMassLock)) != 0);
		player->setAlertFlag(oo::AlertFlags::kMassLock, false);
		EXPECT_TRUE((player->getAlertFlags() & oo::ToUnderlying(oo::AlertFlags::kMassLock)) == 0);
		PlayerEntityTestAccess::alertFlags(player) = 0x7;
		player->clearAlertFlags();
		EXPECT_TRUE(player->getAlertFlags() == 0);
	}
}

// --- Alert condition, AI messages, missiles and mines, the cloak,
// ECM, energy units, the main weapons
// ---------------------------------------------------------------------

// The fleeing status reads the member; taking the weapons offline makes every
// missile safe.
TEST(PlayerEntity, slice13FleeingAndWeaponsOnline)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		TestPlayer *player = MakePlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::fleeing_status(part) = oo::PlayerFleeingStatus::kCargo;
		EXPECT_TRUE(player->fleeingStatus() == oo::PlayerFleeingStatus::kCargo);
		player->setWeaponsOnline(true);
		EXPECT_TRUE(player->weaponsOnline());
		player->setWeaponsOnline(false);
		EXPECT_TRUE(!player->weaponsOnline());
	}
}

// --- A player whose sends to itself are recorded --------

namespace {

// What a RecordingPlayer was sent.
struct Sent
{
	int count = 0;
	oo::CreditsQuantity amount = 0;
	std::string text;
	int number = 0;
	bool flag = false;
	std::optional<std::string> optionalText;
};
Sent sSent;

}	// namespace

/*	A player whose calls to itself that lead to the game's data (scripts,
   the GUI, the market) only record what they were sent, so that the class's own
   logic is what the test sees (sends to self stay calls through the virtual
   members).
*/
class RecordingPlayer : public TestPlayer
{
public:
	using TestPlayer::setBounty;
	using TestPlayer::markAsOffender;
	using TestPlayer::setGuiToEquipShipScreen;
	using TestPlayer::noteGUIDidChangeFrom;
	using TestPlayer::setWeaponMount;
	using TestPlayer::addEquipmentItem;

	void setBounty(oo::CreditsQuantity amount, const std::string &reason) override
	{
		sSent.count++; sSent.amount = amount; sSent.text = reason;
	}


	void markAsOffender(int offence_value, oo::LegalStatusReason reason) override
	{
		sSent.count++; sSent.number = offence_value; sSent.text = oo::StringFromLegalStatusReason(reason);
	}


	oo::FuelQuantity fuelRequiredForJump() override
	{
		return (oo::FuelQuantity)sSent.number;
	}


	void setGuiToSystemDataScreenRefreshBackground(bool refreshBackground) override
	{
		sSent.count++; sSent.flag = refreshBackground;
	}


	void setGuiToEquipShipScreen(int skip, const std::optional<std::string> &eqKey) override
	{
		sSent.count++; sSent.number = skip; sSent.optionalText = eqKey;
	}


	void showInformationForSelectedUpgradeWithFormatString(const std::optional<std::string> &formatString) override
	{
		sSent.count++; sSent.optionalText = formatString;
	}


	void noteGUIDidChangeFrom(oo::GUIScreenID fromScreen, oo::GUIScreenID toScreen, bool refresh) override
	{
		sSent.count++; sSent.number = (int)fromScreen * 100 + (int)toScreen; sSent.flag = refresh;
	}


	void noteSwitchToView(oo::ViewID toView, oo::ViewID fromView) override
	{
		sSent.count++; sSent.number = (int)fromView * 100 + (int)toView;
	}


	bool setWeaponMount(oo::WeaponFacing facing, const std::string &eqKey, const std::optional<std::string> &context) override
	{
		sSent.count++; sSent.number = (int)facing; sSent.text = eqKey; sSent.optionalText = context;
		return true;
	}


	oo::CargoQuantity cargoQuantityOnBoard() override
	{
		return 7;
	}


	bool addEquipmentItem(const std::string &equipmentKey, bool validateAddition, const std::string &context) override
	{
		sSent.count++; sSent.text = equipmentKey + "/" + context; sSent.flag = validateAddition;
		return true;
	}


	void setScoopsActive() override
	{
		sSent.count++;
		TestPlayer::setScoopsActive();
	}
};


namespace {

RecordingPlayer *MakeRecordingPlayer()
{
	sSent = Sent();
	gOOPlayer = nullptr;
	RecordingPlayer *player = NewTestPlayer<RecordingPlayer>();
	oo::AutoreleaseEntity(player);
	return player;
}

bool NearV(Vector a, Vector b)	{ return std::fabs(a.x - b.x) < 1e-4 && std::fabs(a.y - b.y) < 1e-4 && std::fabs(a.z - b.z) < 1e-4; }

}	// namespace

// --- Hit testing, damage, the doppelganger, the escape capsule,
// dumping cargo, bounty
// ---------------------------------------------------------------------------------

// Setting the bounty with no reason sends the unknown reason's string; rotating
// an empty hold does nothing.
TEST(PlayerEntity, slice14BountyAndRotateCargo)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		player->setBounty(25);
		EXPECT_TRUE(sSent.count == 1 && sSent.amount == 25);
		EXPECT_TRUE(sSent.text == oo::StringFromLegalStatusReason(oo::LegalStatusReason::kUnknown));
		player->setBounty(30, oo::LegalStatusReason::kUnknown);
		EXPECT_TRUE(sSent.count == 2 && sSent.amount == 30);
		player->rotateCargo();
		EXPECT_TRUE(player->cargo.empty());
	}
}

// --- Legal status, offences, bounties collected, internal damage,
// destruction, ending a scenario, docking and leaving dock
// ------------------------------------------------

// The bounty is the legal status; an offence with no reason sends the unknown
// reason; a scenario ends only with its own key.
TEST(PlayerEntity, slice15LegalStatusOffenceAndScenario)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::legalStatusValue(player) = 64;
		EXPECT_TRUE(player->getBounty() == 64 && player->getLegalStatus() == 64);
		player->markAsOffender(8);
		EXPECT_TRUE(sSent.count == 1 && sSent.number == 8);
		EXPECT_TRUE(sSent.text == oo::StringFromLegalStatusReason(oo::LegalStatusReason::kUnknown));
		EXPECT_TRUE(!player->endScenario("some-scenario"));
		PlayerEntityTestAccess::scenarioKey(player) = std::string("other-scenario");
		EXPECT_TRUE(!player->endScenario("some-scenario"));
	}
}

// --- Witchspace: start, end, checklist, jump type and distance,
// fuel, galactic and wormhole jumps
// -------------------------------------------------------------------

// The jump type sets the galactic flag; the fuel suffices when it reaches what
// the jump needs.
TEST(PlayerEntity, slice16JumpTypeAndFuel)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		player->setJumpType(true);
		EXPECT_TRUE(PlayerEntityTestAccess::galactic_witchjump(player));
		player->setJumpType(false);
		EXPECT_TRUE(!PlayerEntityTestAccess::galactic_witchjump(player));
		player->fuel = 70;
		sSent.number = 50;
		EXPECT_TRUE(player->hasSufficientFuelForJump());
		sSent.number = 70;
		EXPECT_TRUE(player->hasSufficientFuelForJump());
		sSent.number = 71;
		EXPECT_TRUE(!player->hasSufficientFuelForJump());
	}
}

// --- Leaving witchspace, the status screen, the equipment list,
// primed and fast equipment, weapon types
// ----------------------------------------------------------

// With no primable equipment nothing is primed; the fast equipment keys read
// and write the members.
TEST(PlayerEntity, slice17PrimedAndFastEquipment)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		EXPECT_TRUE(player->primedEquipmentCount() == 0);
		EXPECT_TRUE(player->currentPrimedEquipment().empty());
		EXPECT_TRUE(!player->fastEquipmentA().has_value() && !player->fastEquipmentB().has_value());
		player->setFastEquipmentA(std::string("EQ_FUEL_INJECTION"));
		player->setFastEquipmentB(std::string("EQ_ECM"));
		EXPECT_TRUE(player->fastEquipmentA() == std::optional<std::string>("EQ_FUEL_INJECTION"));
		EXPECT_TRUE(player->fastEquipmentB() == std::optional<std::string>("EQ_ECM"));
	}
}

// --- Scripting lists, the system data screen, marked destinations,
// the chart screens
// ---------------------------------------------------------------------------------

// With no passengers, parcels or contracts the scripting lists are empty
// arrays; the system data screen is shown without refreshing the background.
TEST(PlayerEntity, slice18ScriptingListsAndSystemData)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		const oo::PList passengers = player->passengerListForScripting();
		const oo::PList parcels = player->parcelListForScripting();
		const oo::PList contracts = player->contractListForScripting();
		EXPECT_TRUE(passengers.isArray() && passengers.getIf<oo::PList::Array>()->empty());
		EXPECT_TRUE(parcels.isArray() && parcels.getIf<oo::PList::Array>()->empty());
		EXPECT_TRUE(contracts.isArray() && contracts.getIf<oo::PList::Array>()->empty());
		sSent.flag = true;
		player->setGuiToSystemDataScreen();
		EXPECT_TRUE(sSent.count == 1 && !sSent.flag);
	}
}

// The game options and load / save screens, the equip-screen key highlight and
// the available facings have no unit case: every unit reads the GUI, the ship
// registry or the game's data, which the integration tests cover.

// --- The equip-ship screen and upgrade information
// ------------------------

// The short forms send the long ones with no facing selection and no format
// string.
TEST(PlayerEntity, slice20EquipShipScreenShortForms)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		sSent.optionalText = std::string("unset");
		player->setGuiToEquipShipScreen(3);
		EXPECT_TRUE(sSent.count == 1 && sSent.number == 3 && !sSent.optionalText.has_value());
		sSent.optionalText = std::string("unset");
		player->showInformationForSelectedUpgrade();
		EXPECT_TRUE(sSent.count == 2 && !sSent.optionalText.has_value());
	}
}

// --- The interfaces screen, the start screen and intro, the OXZ
// manager, GUI and view change notes
// ---------------------------------------------------------------------

// The short GUI change note sends the long one without a refresh; the view
// change note is the controls' view switch, arguments swapped.
TEST(PlayerEntity, slice21ChangeNotes)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		sSent.flag = true;
		player->noteGUIDidChangeFrom(oo::GUIScreenID::kStatus, oo::GUIScreenID::kMarket);
		EXPECT_TRUE(sSent.count == 1 && sSent.number == (int)oo::GUIScreenID::kStatus * 100 + (int)oo::GUIScreenID::kMarket && !sSent.flag);
		player->noteViewDidChangeFrom(oo::ViewID::kAft, oo::ViewID::kForward);
		EXPECT_TRUE(sSent.count == 2 && sSent.number == (int)oo::ViewID::kAft * 100 + (int)oo::ViewID::kForward);
	}
}

// --- Buying equipment, script price adjustment, weapon mounts,
// passenger berths, removing missiles, trade-in
// ---------------------------------------------------------------

// Mounting a weapon is a purchase; no berth changes by zero, and none is
// removed when there is none.
TEST(PlayerEntity, slice22WeaponMountAndBerths)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		EXPECT_TRUE(player->setWeaponMount(oo::WeaponFacing::kAft, "EQ_WEAPON_PULSE_LASER"));
		EXPECT_TRUE(sSent.count == 1 && sSent.number == (int)oo::WeaponFacing::kAft && sSent.text == "EQ_WEAPON_PULSE_LASER");
		EXPECT_TRUE(sSent.optionalText == std::optional<std::string>("purchase"));
		EXPECT_TRUE(!player->changePassengerBerths(0));
		PlayerEntityTestAccess::max_passengers(player) = 0;
		EXPECT_TRUE(!player->changePassengerBerths(-1));
		EXPECT_TRUE(PlayerEntityTestAccess::max_passengers(player) == 0);
	}
}

// --- Cargo quantities, the local market, market filters and sorters,
// market screen rows
// ---------------------------------------------------------------------------------

// The current cargo is what is on board.
TEST(PlayerEntity, slice23CalculateCurrentCargo)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::current_cargo(player) = 0;
		player->calculateCurrentCargo();
		EXPECT_TRUE(PlayerEntityTestAccess::current_cargo(player) == 7);
	}
}

// --- The market screens, buying and selling commodities, mining and
// speech flags, adding equipment
// ------------------------------------------------------------------------

// The screen, mining and speech flags read the members; adding equipment
// validates it.
TEST(PlayerEntity, slice24FlagsAndAddEquipment)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::gui_screen(part) = oo::GUIScreenID::kMarket;
		EXPECT_TRUE(player->guiScreen() == oo::GUIScreenID::kMarket);
		PlayerEntityTestAccess::using_mining_laser(part) = true;
		EXPECT_TRUE(player->isMining());
		PlayerEntityTestAccess::using_mining_laser(part) = false;
		EXPECT_TRUE(!player->isMining());
		PlayerEntityTestAccess::isSpeechOn(part) = oo::SpeechSettings::kAll;
		EXPECT_TRUE(player->getIsSpeechOn() == oo::SpeechSettings::kAll);
		EXPECT_TRUE(player->addEquipmentItem("EQ_ECM", "purchase"));
		EXPECT_TRUE(sSent.count == 1 && sSent.text == "EQ_ECM/purchase" && sSent.flag);
	}
}

// --- Equipment, pylons, parcels and passengers, comms, fines,
// trade-in factor, renovation, view offsets, trumbles
// -----------------------------------------------------------

// The counts read the members; the trade-in factor stays within 75..100; the
// view offsets come from the bounding box, and the weapon view offset follows
// the facing.
TEST(PlayerEntity, slice25CountsTradeInAndViewOffsets)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntity *part = player;
		EXPECT_TRUE(player->parcelCount() == 0 && player->passengerCount() == 0 && player->getTrumbleCount() == 0);
		PlayerEntityTestAccess::max_passengers(part) = 3;
		EXPECT_TRUE(player->passengerCapacity() == 3);
		PlayerEntityTestAccess::ship_trade_in_factor(part) = 90;
		player->adjustTradeInFactorBy(5);
		EXPECT_TRUE(player->tradeInFactor() == 95);
		player->adjustTradeInFactorBy(50);
		EXPECT_TRUE(player->tradeInFactor() == 100);
		player->adjustTradeInFactorBy(-50);
		EXPECT_TRUE(player->tradeInFactor() == 75);
		player->boundingBox.min = make_vector(-10.0f, -2.0f, -20.0f);
		player->boundingBox.max = make_vector(10.0f, 2.0f, 40.0f);
		player->setDefaultViewOffsets();
		EXPECT_TRUE(NearV(PlayerEntityTestAccess::forwardViewOffset(part), make_vector(0.0f, 0.0f, 10.0f)));
		EXPECT_TRUE(NearV(PlayerEntityTestAccess::aftViewOffset(part), make_vector(0.0f, 0.0f, 10.0f)));
		EXPECT_TRUE(NearV(PlayerEntityTestAccess::portViewOffset(part), make_vector(0.0f, 0.0f, 0.0f)));
		EXPECT_TRUE(NearV(PlayerEntityTestAccess::customViewOffset(part), kZeroVector));
		PlayerEntityTestAccess::aftViewOffset(part) = make_vector(1.0f, 2.0f, 3.0f);
		player->currentWeaponFacing = oo::WeaponFacing::kAft;
		EXPECT_TRUE(NearV(player->weaponViewOffset(), make_vector(1.0f, 2.0f, 3.0f)));
	}
}

// --- Trumble values, checksums, screen modes, target memory, missile
// ident, rotating and panning the custom view
// ----------------------------------------------------------

// The appetite and the flags read and write the members; clearing the target
// memory fills it with empty slots; zooming the custom view out scales its
// offset about the rotation centre, limited by the collision radius.
TEST(PlayerEntity, slice26TargetMemoryAndCustomViewZoom)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntity *part = player;
		player->setTrumbleAppetiteAccumulator(2.5f);
		EXPECT_TRUE(player->trumbleAppetiteAccumulator() == 2.5f);
		player->getSuppressTargetLost();
		EXPECT_TRUE(PlayerEntityTestAccess::suppressTargetLostFlag(part));
		player->setScoopsActive();
		EXPECT_TRUE(PlayerEntityTestAccess::scoopsActive(player));
		player->clearTargetMemory();
		EXPECT_TRUE(player->targetMemory().size() == PLAYER_TARGET_MEMORY_SIZE && PlayerEntityTestAccess::target_memory_index(player) == 0);
		player->setCustomViewRotationCenter(make_vector(0.0f, 0.0f, 1.0f));
		player->setCustomViewOffset(make_vector(0.0f, 0.0f, 3.0f));
		EXPECT_TRUE(NearV(player->getCustomViewRotationCenter(), make_vector(0.0f, 0.0f, 1.0f)));
		player->collision_radius = 10.0f;
		player->customViewZoomOut(2.0f);
		EXPECT_TRUE(NearV(player->getCustomViewOffset(), make_vector(0.0f, 0.0f, 5.0f)));
		player->collision_radius = 0.01f;
		player->customViewZoomOut(2.0f);
		EXPECT_TRUE(NearV(player->getCustomViewOffset(), make_vector(0.0f, 0.0f, 1.0f + CUSTOM_VIEW_MAX_ZOOM_OUT * 0.01f)));
	}
}

// --- Custom view vectors and data, the mission overlay and
// background, world scripts and script events, galactic hyperspace, jump cause,
// names ------------------------------

// The custom view data follow the quaternion; the galactic hyperspace behaviour
// accepts only known values; the fixed coordinates are clamped; scoop override
// turns the scoops on; the names read and write the members.
TEST(PlayerEntity, slice27CustomViewHyperspaceAndNames)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		player->setCustomViewQuaternion(kIdentityQuaternion);
		EXPECT_TRUE(NearV(player->getCustomViewForwardVector(), vector_forward_from_quaternion(kIdentityQuaternion)));
		EXPECT_TRUE(NearV(player->getCustomViewUpVector(), vector_up_from_quaternion(kIdentityQuaternion)));
		EXPECT_TRUE(NearV(player->getCustomViewRightVector(), vector_right_from_quaternion(kIdentityQuaternion)));
		EXPECT_TRUE(!player->scriptsLoaded() && player->worldScriptNames().empty());
		player->setGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kFixedCoordinates);
		EXPECT_TRUE(player->getGalacticHyperspaceBehaviour() == oo::GalacticHyperspaceBehaviour::kFixedCoordinates);
		player->setGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kUnknown);
		EXPECT_TRUE(player->getGalacticHyperspaceBehaviour() == oo::GalacticHyperspaceBehaviour::kFixedCoordinates);
		player->setGalacticHyperspaceFixedCoords(NSMakePoint(300.0, 12.4));
		EXPECT_TRUE(Near(player->getGalacticHyperspaceFixedCoords(), NSMakePoint(255, 12)));
		player->setMissionExitScreen(oo::GUIScreenID::kStatus);
		EXPECT_TRUE(player->missionExitScreen() == oo::GUIScreenID::kStatus);
		player->setScoopOverride(true);
		EXPECT_TRUE(player->getScoopOverride() && sSent.count == 1);
		player->setCommanderName(std::string("Jameson"));
		player->setLastsaveName(std::string("save1"));
		player->setJumpCause(std::string("standard jump"));
		EXPECT_TRUE(player->commanderName() == std::optional<std::string>("Jameson"));
		EXPECT_TRUE(player->lastsaveName() == std::optional<std::string>("save1"));
		EXPECT_TRUE(player->jumpCause() == std::optional<std::string>("standard jump"));
	}
}

// --- Docking clearance, scanned wormholes, mission destinations, the
// shipyard record, extra mission and GUI-screen keys, the state dump
// ---------------------------------------

// Cleared to dock once granted or not required; a marker's key is its system
// and name; the extra mission keys are cleared.
TEST(PlayerEntity, slice28ClearanceMarkerKeyAndExtraKeys)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntity *part = player;
		PlayerEntityTestAccess::dockingClearanceStatus(part) = oo::DockingClearanceStatus::kRequested;
		EXPECT_TRUE(!player->clearedToDock() && player->getDockingClearanceStatus() == oo::DockingClearanceStatus::kRequested);
		PlayerEntityTestAccess::dockingClearanceStatus(part) = oo::DockingClearanceStatus::kGranted;
		EXPECT_TRUE(player->clearedToDock());
		PlayerEntityTestAccess::dockingClearanceStatus(part) = oo::DockingClearanceStatus::kNotRequired;
		EXPECT_TRUE(player->clearedToDock());
		const oo::PList marker(oo::PList::Dict{ { "system", oo::PList(7) }, { "name", oo::PList(std::string("beacon")) } });
		EXPECT_TRUE(player->markerKey(marker) == std::optional<std::string>("7-beacon"));
		EXPECT_TRUE(player->markerKey(oo::PList(oo::PList::Dict{})) == std::optional<std::string>("0-(null)"));
		PlayerEntityTestAccess::extraMissionKeys(part)["oolite-mission-key"] = oo::PList();
		player->clearExtraMissionKeys();
		EXPECT_TRUE(PlayerEntityTestAccess::extraMissionKeys(part).empty());
		EXPECT_TRUE(player->shipyardRecord() == &PlayerEntityTestAccess::shipyard_record(part));
		EXPECT_TRUE(player->getScannedWormholes().empty());
	}
}

// --- The category files:
// PlayerEntitySound.mm, PlayerEntityStickMapper.mm, PlayerEntityControls.mm,
// PlayerEntityKeyMapper.mm, PlayerEntityLegacyScriptEngine.mm,
// PlayerEntityContracts.mm, PlayerEntityLoadSave.mm. Each case pins units a
// player in a never-initialised universe can answer; the screens, the controls'
// polling and the load / save panels read the GUI, the keyboard or the game's
// files, which the integration tests cover. ------------------------

namespace {
Entity *sCategoryTarget = nullptr;	// what a CategoryPlayer answers as its primary target (not retained)
}


/*	A recording player whose script events, roles, missiles and ident sounds only count, so that a
	category unit's own logic is what the case sees (sends to self stay calls through the virtual
	members after the move).
*/
class CategoryPlayer : public RecordingPlayer
{
public:
	using RecordingPlayer::doScriptEvent;
	using RecordingPlayer::addRoleToPlayer;

	void doScriptEvent(OOJSPropID /*message*/, const std::vector<oo::PList> &arguments) override
	{
		sSent.count++; sSent.number = (int)arguments.size();
	}


	void addRoleToPlayer(const std::string &role) override
	{
		sSent.count++; sSent.text = role;
	}


	void safeAllMissiles() override	{ sSent.count++; }
	void noteLostTarget() override	{ sSent.count++; }
	void playIdentOn() override	{ sSent.count++; sSent.flag = true; }
	void playIdentLockedOn() override	{ sSent.count++; }
	void printIdentLockedOnForMissile(bool missile) override	{ sSent.count++; sSent.number = missile ? 1 : 2; }

	// The target the ident button finds: the unit test's universe has no descriptions to expand the
	// "ident-on" message with (expanding one exits), so the case keeps a target.
	Entity *primaryTarget() override	{ return sCategoryTarget; }
};


namespace {

// A string value's text; a value that is not a string never matches.
std::string StringOf(const oo::PList &value)
{
	const std::string *text = value.getIf<std::string>();
	return text != nullptr ? *text : std::string("<not a string>");
}


CategoryPlayer *MakeCategoryPlayer()
{
	sSent = Sent();
	gOOPlayer = nullptr;
	CategoryPlayer *player = NewTestPlayer<CategoryPlayer>();
	oo::AutoreleaseEntity(player);
	return player;
}

}	// namespace

// PlayerEntitySound.mm: with no interface source playing, the
// player is not beeping.
TEST(PlayerEntity, soundIsNotBeepingWithNothingPlaying)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		EXPECT_TRUE(!player->isBeeping());
	}
}


// PlayerEntityStickMapper.mm: hardware flags as words, and the
// GUI entries the function list is made of (a long description cut to 28 units
// and "...", absent functions left out).
TEST(PlayerEntity, stickMapperHardwareAndGuiDicts)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		// The hardware names come from descriptions.plist (upstream #786), which this universe never
		// loaded (loading it with no resources is fatal), so the test supplies the three entries.
		oo::PList &descriptions = UNIVERSE->_descriptions;
		const oo::PList savedDescriptions = descriptions;
		descriptions = oo::PList(oo::PList::Dict{ { "stickmapper-axis", oo::PList(std::string("axis")) }, { "stickmapper-button", oo::PList(std::string("button")) }, { "stickmapper-axis-button", oo::PList(std::string("axis/button")) } });
		EXPECT_TRUE(player->hwToString(HW_AXIS) == "axis");
		EXPECT_TRUE(player->hwToString(HW_BUTTON) == "button");
		EXPECT_TRUE(player->hwToString(HW_AXIS | HW_BUTTON) == "axis/button");
		descriptions = savedDescriptions;
		const oo::PList dict = player->makeStickGuiDict("Roll", HW_AXIS, 3, -1);
		EXPECT_TRUE(dict.get<std::string>(std::string(KEY_GUIDESC)) == "Roll");
		EXPECT_TRUE(dict.get<long long>(std::string(KEY_ALLOWABLE)) == HW_AXIS && dict.get<long long>(std::string(KEY_AXISFN)) == 3);
		EXPECT_TRUE(dict.find(KEY_BUTTONFN) == nullptr);
		const std::string longName(60, 'x');
		const oo::PList cut = player->makeStickGuiDict(longName, HW_BUTTON, -1, 4);
		EXPECT_TRUE(cut.get<std::string>(std::string(KEY_GUIDESC)) == std::string(28, 'x') + "...");
		EXPECT_TRUE(cut.find(KEY_AXISFN) == nullptr && cut.get<long long>(std::string(KEY_BUTTONFN)) == 4);
		const oo::PList header = player->makeStickGuiDictHeader("Flight");
		EXPECT_TRUE(header.get<std::string>(std::string(KEY_HEADER)) == "Flight" && header.get<std::string>(std::string(KEY_AXISFN)).empty());
	}
}


// PlayerEntityControls.mm: the first key code of a
// definition (0 for none), and clearing the planet search string.
TEST(PlayerEntity, controlsSlice1FirstKeyCodeAndPlanetSearch)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		const oo::PList keyDef(oo::PList::Array{ oo::PList(oo::PList::Dict{ { "key", oo::PList(65) } }), oo::PList(oo::PList::Dict{ { "key", oo::PList(66) } }) });
		EXPECT_TRUE(player->getFirstKeyCode(keyDef) == 65);
		EXPECT_TRUE(player->getFirstKeyCode(oo::PList(oo::PList::Array{})) == 0);
		PlayerEntityTestAccess::planetSearchString(player) = std::string("Lave");
		player->clearPlanetSearchString();
		EXPECT_TRUE(!PlayerEntityTestAccess::planetSearchString(player).has_value());
	}
}

// PlayerEntityControls.mm's other units have no unit case: every unit polls the
// keyboard, the joystick or the GUI, which the integration tests cover.

// PlayerEntityControls.mm: the ident button safes the
// missiles, engages ident and, with a target, plays the locked-on sound and
// prints the lock (not for a missile); pressed again it first drops the target.
TEST(PlayerEntity, controlsSlice7IdentButton)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		CategoryPlayer *player = MakeCategoryPlayer();
		sCategoryTarget = player;
		player->handleButtonIdent();
		EXPECT_TRUE(PlayerEntityTestAccess::ident_engaged(player));
		EXPECT_TRUE(sSent.count == 3 && !sSent.flag && sSent.number == 2);
		sSent = Sent();
		player->handleButtonIdent();
		EXPECT_TRUE(sSent.count == 4 && !sSent.flag);
		sCategoryTarget = nullptr;
	}
}


// PlayerEntityKeyMapper.mm: the key-function list's GUI
// entries (a long description cut to 48 units and "...").
TEST(PlayerEntity, keyMapperSlice1GuiDicts)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		const oo::PList dict = player->makeKeyGuiDict("Fire laser", "key_fire_lasers");
		EXPECT_TRUE(dict.get<std::string>(std::string(KEY_KC_GUIDESC)) == "Fire laser");
		EXPECT_TRUE(dict.get<std::string>(std::string(KEY_KC_DEFINITION)) == "key_fire_lasers");
		const oo::PList cut = player->makeKeyGuiDict(std::string(51, 'y'), "k");
		EXPECT_TRUE(cut.get<std::string>(std::string(KEY_KC_GUIDESC)) == std::string(48, 'y') + "...");
		const oo::PList header = player->makeKeyGuiDictHeader("Navigation");
		EXPECT_TRUE(header.get<std::string>(std::string(KEY_KC_HEADER)) == "Navigation");
		EXPECT_TRUE(header.get<std::string>(std::string(KEY_KC_DEFINITION)).empty());
	}
}


// PlayerEntityKeyMapper.mm: custom-equipment entries are
// activate_ and mode_ keys, and their key-definition type.
TEST(PlayerEntity, keyMapperSlice2CustomEquipEntries)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		EXPECT_TRUE(player->entryIsCustomEquip("activate_EQ_ECM") && player->entryIsCustomEquip("mode_EQ_ECM"));
		EXPECT_TRUE(!player->entryIsCustomEquip("key_ecm"));
		EXPECT_TRUE(player->entryIsDictCustomEquip(oo::PList(oo::PList::Dict{ { std::string(KEY_KC_DEFINITION), oo::PList(std::string("mode_EQ_X")) } })));
		EXPECT_TRUE(!player->entryIsDictCustomEquip(oo::PList(oo::PList::Dict{})));
		EXPECT_TRUE(player->getCustomEquipKeyDefType("activate_EQ_ECM") == std::optional<std::string>(std::string(CUSTOMEQUIP_KEYACTIVATE)));
		EXPECT_TRUE(player->getCustomEquipKeyDefType("mode_EQ_ECM") == std::optional<std::string>(std::string(CUSTOMEQUIP_KEYMODE)));
		EXPECT_TRUE(player->getCustomEquipKeyDefType("key_ecm") == std::optional<std::string>(std::string()));
	}
}


// PlayerEntityKeyMapper.mm: two key entries are the same
// when the key and the three modifiers are.
TEST(PlayerEntity, keyMapperSlice3CompareKeyEntries)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		const oo::PList a(oo::PList::Dict{ { "key", oo::PList(65) }, { "shift", oo::PList(true) } });
		const oo::PList b(oo::PList::Dict{ { "key", oo::PList(std::string("65")) }, { "shift", oo::PList(true) } });
		const oo::PList c(oo::PList::Dict{ { "key", oo::PList(65) } });
		EXPECT_TRUE(player->compareKeyEntries(a, a));
		EXPECT_TRUE(player->compareKeyEntries(a, b));
		EXPECT_TRUE(!player->compareKeyEntries(a, c));
		EXPECT_TRUE(!player->compareKeyEntries(c, oo::PList(oo::PList::Dict{ { "key", oo::PList(66) } })));
	}
}


// PlayerEntityLegacyScriptEngine.mm: mission variables
// need the store (nothing is kept before set-up); a null value removes one;
// local variables are per mission.
TEST(PlayerEntity, legacyScriptSlice1MissionAndLocalVariables)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::mission_variables(player) = oo::PList();
		player->setMissionVariable(oo::PList(std::string("1")), "mission_x");
		EXPECT_TRUE(player->missionVariableForKey("mission_x").isNull());
		PlayerEntityTestAccess::mission_variables(player) = oo::PList(oo::PList::Dict{});
		player->setMissionVariable(oo::PList(std::string("1")), "mission_x");
		EXPECT_TRUE(StringOf(player->missionVariableForKey("mission_x")) == "1");
		EXPECT_TRUE(player->missionVariables().count() == 1);
		player->setMissionVariable(oo::PList(), "mission_x");
		EXPECT_TRUE(player->missionVariableForKey("mission_x").isNull());
		player->setLocalVariable(std::string("7"), "local_y", std::string("m1"));
		EXPECT_TRUE(player->localVariableForKey("local_y", std::string("m1")) == std::optional<std::string>("7"));
		EXPECT_TRUE(!player->localVariableForKey("local_y", std::string("m2")).has_value());
		EXPECT_TRUE(!player->localVariableForKey("local_y", std::nullopt).has_value());
		EXPECT_TRUE(player->localVariablesForMission(std::nullopt).isNull());
	}
}


// PlayerEntityLegacyScriptEngine.mm: the credits query
// answers the balance.
TEST(PlayerEntity, legacyScriptSlice2CreditsQuery)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		player->setCreditBalance(42.5);
		const oo::PList credits = player->credits_number();
		EXPECT_TRUE(credits.isNumber() && credits.doubleValue() == 42.5);
	}
}


// PlayerEntityLegacyScriptEngine.mm: set:, increment:,
// decrement: and reset: on a mission variable, and the mission title.
TEST(PlayerEntity, legacyScriptSlice3VariableArithmeticAndTitle)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::mission_variables(player) = oo::PList(oo::PList::Dict{});
		player->set("mission_count 5");
		EXPECT_TRUE(StringOf(player->missionVariableForKey("mission_count")) == "5");
		player->increment("mission_count");
		EXPECT_TRUE(StringOf(player->missionVariableForKey("mission_count")) == "6");
		player->decrement("mission_count");
		player->decrement("mission_count");
		EXPECT_TRUE(StringOf(player->missionVariableForKey("mission_count")) == "4");
		player->set("no_prefix 1");
		EXPECT_TRUE(player->missionVariableForKey("no_prefix").isNull());
		player->reset("mission_count");
		EXPECT_TRUE(player->missionVariableForKey("mission_count").isNull());
		player->setMissionTitle(std::string("A title"));
		EXPECT_TRUE(player->missionTitle() == std::optional<std::string>("A title"));
		player->setMissionTitle(std::nullopt);
		EXPECT_TRUE(!player->missionTitle().has_value());
	}
}


// PlayerEntityLegacyScriptEngine.mm: the mission
// screen's identifier, and the index of an equipment script that is not there
// (the count).
TEST(PlayerEntity, legacyScriptSlice4MissionScreenIDAndEqScripts)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		player->setMissionScreenID(std::string("screen-1"));
		EXPECT_TRUE(player->missionScreenID() == std::optional<std::string>("screen-1"));
		player->clearMissionScreenID();
		EXPECT_TRUE(!player->missionScreenID().has_value());
		EXPECT_TRUE(player->eqScriptIndexForKey("EQ_NONE") == PlayerEntityTestAccess::eqScripts(player).size());
	}
}


// PlayerEntityContracts.mm: a passenger is added once (a
// risky one adds the courier role, and the event is sent with two arguments)
// and removed by name; contracted volume counts a good's contracts; the docking
// report joins messages with a blank line.
TEST(PlayerEntity, contractsSlice1PassengersVolumeAndReport)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		CategoryPlayer *player = MakeCategoryPlayer();
		gOOPlayer = player;	// addPassenger() reads PLAYER's clock, which the debug
                         // build asserts is set
		PlayerEntityTestAccess::max_passengers(player) = 2;
		EXPECT_TRUE(player->addPassenger("Ann", 1, 2, 100.0, 10.0, 1.0, 2));
		EXPECT_TRUE(sSent.count == 2 && sSent.text == "trader-courier+" && sSent.number == 2);
		EXPECT_TRUE(!player->addPassenger("Ann", 1, 2, 100.0, 10.0, 1.0, 0));
		EXPECT_TRUE(PlayerEntityTestAccess::passengers(player).size() == 1);
		EXPECT_TRUE(!player->removePassenger("Bob"));
		EXPECT_TRUE(player->removePassenger("Ann") && PlayerEntityTestAccess::passengers(player).empty());
		PlayerEntityTestAccess::contracts(player).push_back(oo::PList(oo::PList::Dict{ { std::string(CARGO_KEY_TYPE), oo::PList(std::string("food")) }, { std::string(CARGO_KEY_AMOUNT), oo::PList(3) } }));
		PlayerEntityTestAccess::contracts(player).push_back(oo::PList(oo::PList::Dict{ { std::string(CARGO_KEY_TYPE), oo::PList(std::string("food")) }, { std::string(CARGO_KEY_AMOUNT), oo::PList(4) } }));
		EXPECT_TRUE(player->contractedVolumeForGood("food") == 7 && player->contractedVolumeForGood("gold") == 0);
		PlayerEntityTestAccess::dockingReport(player).clear();
		player->addMessageToReport("one");
		player->addMessageToReport("");
		player->addMessageToReport("two");
		EXPECT_TRUE(PlayerEntityTestAccess::dockingReport(player) == "one\n\ntwo");
		gOOPlayer = nullptr;
	}
}


// PlayerEntityContracts.mm: with no record each
// reputation is the middle of its range (unknown counts as the maximum), and
// the record is a dictionary.
TEST(PlayerEntity, contractsSlice2Reputation)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::reputation(player).clear();
		EXPECT_TRUE(player->passengerReputation() == MAX_CONTRACT_REP / 2);
		EXPECT_TRUE(player->parcelReputation() == MAX_CONTRACT_REP / 2);
		EXPECT_TRUE(player->contractReputation() == MAX_CONTRACT_REP / 2);
		EXPECT_TRUE(player->getReputation().isDict());
	}
}


// PlayerEntityContracts.mm: a ship with all its
// subentities loses nothing.
TEST(PlayerEntity, contractsSlice3MissingSubEntities)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		EXPECT_TRUE(player->missingSubEntitiesAdjustment() == 0);
	}
}


// PlayerEntityLoadSave.mm's other units read the save files, the scenarios and
// the GUI, which the integration tests cover.

// PlayerEntityLoadSave.mm: a commander is found by its
// save name, else its name; one not in the list is not found.
TEST(PlayerEntity, loadSaveSlice2FindCommander)
{
	{
		oo::AutoreleaseScope scope;
		::SetUp();
		RecordingPlayer *player = MakeRecordingPlayer();
		PlayerEntityTestAccess::cdrDetailArray(player).clear();
		EXPECT_TRUE(player->findIndexOfCommander("Jameson") == -1);
		PlayerEntityTestAccess::cdrDetailArray(player).push_back(oo::PList(oo::PList::Dict{ { "player_name", oo::PList(std::string("Jameson")) } }));
		PlayerEntityTestAccess::cdrDetailArray(player).push_back(oo::PList(oo::PList::Dict{ { "player_save_name", oo::PList(std::string("Other")) }, { "player_name", oo::PList(std::string("Jameson")) } }));
		EXPECT_TRUE(player->findIndexOfCommander("Jameson") == 0);
		EXPECT_TRUE(player->findIndexOfCommander("Other") == 1);
		PlayerEntityTestAccess::cdrDetailArray(player).clear();
	}
}


// The last save name is remembered in the user defaults (upstream OoliteProject #740):
// setLastsaveName() writes "last_savename", which a new game's set-up reads back instead of
// resetting the name to Jameson.
TEST(PlayerEntity, lastsaveNameIsRememberedInTheDefaults)
{
	{
		oo::AutoreleaseScope scope;	// was @autoreleasepool
		::SetUp();
		TestPlayer *player = MakePlayer();
		oo::Defaults::standard().removeObject("last_savename");
		EXPECT_TRUE(!oo::Defaults::standard().stringForKey("last_savename").has_value());
		player->setLastsaveName(std::string("Commander Save"));
		EXPECT_TRUE(player->lastsaveName() == std::optional<std::string>("Commander Save"));
		EXPECT_TRUE(oo::Defaults::standard().stringForKey("last_savename") == std::optional<std::string>("Commander Save"));
		player->setLastsaveName(std::string("Second"));
		EXPECT_TRUE(oo::Defaults::standard().stringForKey("last_savename") == std::optional<std::string>("Second"));
		oo::Defaults::standard().removeObject("last_savename");
	}
}
