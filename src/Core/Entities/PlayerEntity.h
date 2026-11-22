/*

PlayerEntity.h

Entity subclass nominally representing the player's ship, but also
implementing much of the interaction, menu system etc. Breaking it up into
ten or so different classes is a perennial to-do item.

The player is made in C++ by PlayerEntity::sharedPlayer(). The game finds
selectors by name on the player: legacy-script actions and queries, callObjC(),
shader bindings, deferred calls.

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
#include <string_view>
#include <utility>
#include <vector>

#include "CommodityMarket.h"
#include "Foundation.h"
#include "GuiDisplayGen.h"
#include "LaserShotEntity.h"
#include "OOJSPropID.h"
#include "ShipEntity.h"
#include "Trumble.h"
#include "Types.h"
#include "WormholeEntity.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/objc/Assert.h"

class MyOpenGLView;
class ShipEntity;
class GuiDisplayGen;
#include "HeadUpDisplay.h"  // the player keeps its HUD (std::shared_ptr)

namespace oo {

class Sound;
class SoundSource;

}  // namespace oo

namespace oo {

class Texture;

}  // namespace oo

#include "OOJSGuiScreenKeyDefinition.h"  // extraGuiScreenKeys keeps it (std::shared_ptr)
#include "OOScript.h"      // std::shared_ptr<OOScript> members and results
class StickProfileScreen;  // PlayerEntityStickProfile.h

#define ALLOW_CUSTOM_VIEWS_WHILE_PAUSED 1
inline constexpr double SCRIPT_TIMER_INTERVAL = 10.0;

#ifndef OO_VARIABLE_TORUS_SPEED
#define OO_VARIABLE_TORUS_SPEED 1
#endif

#define GUI_ROW_INIT(GUI) /*int n_rows = [(GUI) rows]*/

inline constexpr double CUSTOM_VIEW_MAX_ZOOM_IN = 1.5;
inline constexpr int CUSTOM_VIEW_MAX_ZOOM_OUT = 25;

// OOGUIScreenID, OOGalacticHyperspaceBehaviour and their defaults (plain
// header).
#include "EntityEnums.h"
#include "EnumFlags.h"

namespace oo {

enum class PrimedEquipmentMode : int { kActivated, kMode };

enum class SpeechSettings : std::int32_t { kOff = 0, kComms = 1, kAll = 2 };

}  // namespace oo

// The numbers the saved game's speech_on sees.
static_assert(oo::ToUnderlying(oo::SpeechSettings::kOff) == 0);
static_assert(oo::ToUnderlying(oo::SpeechSettings::kComms) == 1);
static_assert(oo::ToUnderlying(oo::SpeechSettings::kAll) == 2);

// When fully zoomed in, chart shows area of galaxy that's 64x64 galaxy units.
#define CHART_WIDTH_AT_MAX_ZOOM 64.0
inline constexpr double CHART_HEIGHT_AT_MAX_ZOOM = 64.0;
// Galaxy width / width of chart area at max zoom
inline constexpr auto CHART_MAX_ZOOM = (256.0 / CHART_WIDTH_AT_MAX_ZOOM);
// start scrolling when cursor is this number of units away from centre
inline constexpr double CHART_SCROLL_AT_X = 25.0;
inline constexpr double CHART_SCROLL_AT_Y = 31.0;
inline constexpr double CHART_CLIP_BORDER = 10.0;
inline constexpr auto CHART_SCREEN_VERTICAL_CENTRE = (10 * MAIN_GUI_ROW_HEIGHT);
inline constexpr auto CHART_SCREEN_VERTICAL_CENTRE_COMPACT =
    (7 * MAIN_GUI_ROW_HEIGHT);
inline constexpr double CHART_ZOOM_SPEED_FACTOR = 1.05;

inline constexpr double CHART_ZOOM_SHOW_LABELS = 2.0;

// OO_RESOLUTION_OPTION: true if full screen resolution can be changed.
#define OO_RESOLUTION_OPTION 1

// dictionary keys - used in the custom key config for oxp equipment
inline constexpr std::string_view CUSTOMEQUIP_EQUIPKEY = "equipmentKey";
inline constexpr std::string_view CUSTOMEQUIP_EQUIPNAME = "equipmentName";
inline constexpr std::string_view CUSTOMEQUIP_KEYACTIVATE = "keyActivate";
inline constexpr std::string_view CUSTOMEQUIP_KEYMODE = "keyMode";
inline constexpr std::string_view CUSTOMEQUIP_BUTTONACTIVATE = "buttonActivate";
inline constexpr std::string_view CUSTOMEQUIP_BUTTONMODE = "buttonMode";
inline constexpr std::string_view KEYCONFIG_CUSTOMEQUIP =
    "CustomEquipActivation";  // preferences key (oo::Defaults)

// The rows of the options screen and the game options screen, each one more
// than the row before it; the screens place them with OOGUIOptionsRow() and
// OOGUIGameOptionsRow(): row numbers that are added to, not an enumeration.
inline constexpr int GUI_ROW_OPTIONS_QUICKSAVE = 0;
inline constexpr int GUI_ROW_OPTIONS_SAVE = GUI_ROW_OPTIONS_QUICKSAVE + 1;
inline constexpr int GUI_ROW_OPTIONS_LOAD = GUI_ROW_OPTIONS_SAVE + 1;
inline constexpr int GUI_ROW_OPTIONS_SPACER1 = GUI_ROW_OPTIONS_LOAD + 1;
inline constexpr int GUI_ROW_OPTIONS_GAMEOPTIONS = GUI_ROW_OPTIONS_SPACER1 + 1;
inline constexpr int GUI_ROW_OPTIONS_SPACER2 = GUI_ROW_OPTIONS_GAMEOPTIONS + 1;
inline constexpr int GUI_ROW_OPTIONS_BEGIN_NEW = GUI_ROW_OPTIONS_SPACER2 + 1;
#if OOLITE_SDL
inline constexpr int GUI_ROW_OPTIONS_SPACER3 = GUI_ROW_OPTIONS_BEGIN_NEW + 1;
inline constexpr int GUI_ROW_OPTIONS_QUIT = GUI_ROW_OPTIONS_SPACER3 + 1;
inline constexpr int GUI_ROW_OPTIONS_END_OF_LIST = GUI_ROW_OPTIONS_QUIT + 1;
#else
inline constexpr int GUI_ROW_OPTIONS_END_OF_LIST =
    GUI_ROW_OPTIONS_BEGIN_NEW + 1;
#endif

inline constexpr int STATUS_EQUIPMENT_FIRST_ROW = 10;
inline constexpr int STATUS_EQUIPMENT_MAX_ROWS = 8;
inline constexpr int STATUS_EQUIPMENT_BIGGUI_EXTRA_ROWS = 6;
inline constexpr int GUI_ROW_EQUIPMENT_START = 3;
inline constexpr int GUI_MAX_ROWS_EQUIPMENT = 12;
inline constexpr int GUI_ROW_EQUIPMENT_DETAIL =
    GUI_ROW_EQUIPMENT_START + GUI_MAX_ROWS_EQUIPMENT + 1;
inline constexpr int GUI_ROW_EQUIPMENT_CASH = 1;
inline constexpr int GUI_ROW_MARKET_KEY = 1;
inline constexpr int GUI_ROW_MARKET_START = 2;
inline constexpr int GUI_ROW_MARKET_SCROLLUP = 4;
inline constexpr int GUI_ROW_MARKET_SCROLLDOWN = 16;
inline constexpr int GUI_ROW_MARKET_LAST = 18;
inline constexpr int GUI_ROW_MARKET_END = 19;
inline constexpr int GUI_ROW_MARKET_CASH = 20;
inline constexpr int GUI_ROW_INTERFACES_HEADING = 1;
inline constexpr int GUI_ROW_INTERFACES_START = 3;
inline constexpr int GUI_MAX_ROWS_INTERFACES = 12;
inline constexpr int GUI_ROW_INTERFACES_DETAIL =
    GUI_ROW_INTERFACES_START + GUI_MAX_ROWS_INTERFACES + 1;
inline constexpr int GUI_ROW_NO_INTERFACES = 3;
inline constexpr int GUI_ROW_SCENARIOS_START = 3;
inline constexpr int GUI_MAX_ROWS_SCENARIOS = 12;
inline constexpr int GUI_ROW_SCENARIOS_DETAIL =
    GUI_ROW_SCENARIOS_START + GUI_MAX_ROWS_SCENARIOS + 2;
inline constexpr int GUI_ROW_CHART_SYSTEM = 19;
inline constexpr int GUI_ROW_CHART_SYSTEM_COMPACT = 17;
inline constexpr int GUI_ROW_PLANET_FINDER = 20;

// The row the first item of a list of endOfList rows goes on: the list
// centred in the GUI's rows.
namespace oo {

constexpr int GUIFirstRow(int endOfList) {
  return (GUI_DEFAULT_ROWS - endOfList) / 2;
}

// The row an item of such a list goes on (the menu is moved up four rows).
constexpr int GUIListRow(int endOfList, int item) {
  return GUIFirstRow(endOfList) - 4 + item;
}

constexpr int GUIOptionsRow(int item) {
  return GUIListRow(GUI_ROW_OPTIONS_END_OF_LIST, item);
}

}  // namespace oo

static_assert(oo::GUIFirstRow(GUI_ROW_OPTIONS_END_OF_LIST) >= 0,
              "Too many items in OPTIONS list!");

inline constexpr int GUI_ROW_GAMEOPTIONS_AUTOSAVE = 0;
inline constexpr int GUI_ROW_GAMEOPTIONS_DOCKINGCLEARANCE =
    GUI_ROW_GAMEOPTIONS_AUTOSAVE + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_SPACER1 =
    GUI_ROW_GAMEOPTIONS_DOCKINGCLEARANCE + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_VOLUME =
    GUI_ROW_GAMEOPTIONS_SPACER1 + 1;
#if OOLITE_SPEECH_SYNTH
inline constexpr int GUI_ROW_GAMEOPTIONS_SPEECH =
    GUI_ROW_GAMEOPTIONS_VOLUME + 1;
// FIXME: should have voice option for OS X
inline constexpr int GUI_ROW_GAMEOPTIONS_SPEECH_LANGUAGE =
    GUI_ROW_GAMEOPTIONS_SPEECH + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_SPEECH_GENDER =
    GUI_ROW_GAMEOPTIONS_SPEECH_LANGUAGE + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_MUSIC =
    GUI_ROW_GAMEOPTIONS_SPEECH_GENDER + 1;
#else
inline constexpr int GUI_ROW_GAMEOPTIONS_MUSIC = GUI_ROW_GAMEOPTIONS_VOLUME + 1;
#endif
#if OO_RESOLUTION_OPTION
inline constexpr int GUI_ROW_GAMEOPTIONS_SPACER2 =
    GUI_ROW_GAMEOPTIONS_MUSIC + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_DISPLAY =
    GUI_ROW_GAMEOPTIONS_SPACER2 + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_DISPLAYSTYLE =
    GUI_ROW_GAMEOPTIONS_DISPLAY + 1;
#else
inline constexpr int GUI_ROW_GAMEOPTIONS_DISPLAYSTYLE =
    GUI_ROW_GAMEOPTIONS_MUSIC + 1;
#endif
inline constexpr int GUI_ROW_GAMEOPTIONS_DETAIL =
    GUI_ROW_GAMEOPTIONS_DISPLAYSTYLE + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_WIREFRAMEGRAPHICS =
    GUI_ROW_GAMEOPTIONS_DETAIL + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_SHADEREFFECTS =
    GUI_ROW_GAMEOPTIONS_WIREFRAMEGRAPHICS + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_FOV =
    GUI_ROW_GAMEOPTIONS_SHADEREFFECTS + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_COLORBLINDMODE =
    GUI_ROW_GAMEOPTIONS_FOV + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_SPACER_STICKMAPPER =
    GUI_ROW_GAMEOPTIONS_COLORBLINDMODE + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_STICKMAPPER =
    GUI_ROW_GAMEOPTIONS_SPACER_STICKMAPPER + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_KEYMAPPER =
    GUI_ROW_GAMEOPTIONS_STICKMAPPER + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_SPACER3 =
    GUI_ROW_GAMEOPTIONS_KEYMAPPER + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_BACK = GUI_ROW_GAMEOPTIONS_SPACER3 + 1;
inline constexpr int GUI_ROW_GAMEOPTIONS_END_OF_LIST =
    GUI_ROW_GAMEOPTIONS_BACK + 1;
static_assert(oo::GUIFirstRow(GUI_ROW_GAMEOPTIONS_END_OF_LIST) >= 0,
              "Too many items in GAMEOPTIONS list!");

namespace oo {

constexpr int GUIGameOptionsRow(int item) {
  return GUIListRow(GUI_ROW_GAMEOPTIONS_END_OF_LIST, item);
}

}  // namespace oo

inline constexpr int GUI_ROW_GAMEOPTIONS_HDRPAPERWHITE =
    GUI_ROW_GAMEOPTIONS_WIREFRAMEGRAPHICS;
inline constexpr int GUI_ROW_GAMEOPTIONS_HDRMAXBRIGHTNESS =
    GUI_ROW_GAMEOPTIONS_DETAIL;

namespace oo {

enum class FuelScoopStatus : std::int32_t {
  // Exposed to shaders.
  kNotInstalled = 0,
  kFullHold,
  kOkay,
  kActive
};

}  // namespace oo

// The numbers the shader binding sees.
static_assert(oo::ToUnderlying(oo::FuelScoopStatus::kNotInstalled) == 0);
static_assert(oo::ToUnderlying(oo::FuelScoopStatus::kFullHold) == 1);
static_assert(oo::ToUnderlying(oo::FuelScoopStatus::kOkay) == 2);
static_assert(oo::ToUnderlying(oo::FuelScoopStatus::kActive) == 3);

// The bits of PlayerEntity's alert flags, which the player keeps as a number
// (getAlertFlags(), the by-name "alertFlags"). kYellowLimit is not a bit: flags
// above it are condition red.
namespace oo {

enum class AlertFlags : std::uint16_t {
  kNone = 0,
  kDocked = 0x010,
  kMassLock = 0x020,
  kYellowLimit = 0x03f,
  kTemp = 0x040,
  kAlt = 0x080,
  kEnergy = 0x100,
  kHostiles = 0x200
};

constexpr std::true_type OOEnableFlagOperators(AlertFlags);

}  // namespace oo

// The numbers a by-name "alertFlags" call answers.
static_assert(oo::ToUnderlying(oo::AlertFlags::kDocked) == 0x010);
static_assert(oo::ToUnderlying(oo::AlertFlags::kMassLock) == 0x020);
static_assert(oo::ToUnderlying(oo::AlertFlags::kYellowLimit) == 0x03f);
static_assert(oo::ToUnderlying(oo::AlertFlags::kTemp) == 0x040);
static_assert(oo::ToUnderlying(oo::AlertFlags::kAlt) == 0x080);
static_assert(oo::ToUnderlying(oo::AlertFlags::kEnergy) == 0x100);
static_assert(oo::ToUnderlying(oo::AlertFlags::kHostiles) == 0x200);

namespace oo {

inline bool AlertFlagIsOn(int flags, AlertFlags flag) noexcept {
  return (flags & oo::ToUnderlying(flag)) != 0;
}

enum class MissileStatus : std::int32_t {
  // Exposed to shaders.
  kSafe,
  kArmed,
  kTargetLocked
};

}  // namespace oo

// The numbers the shader binding sees.
static_assert(oo::ToUnderlying(oo::MissileStatus::kSafe) == 0);
static_assert(oo::ToUnderlying(oo::MissileStatus::kArmed) == 1);
static_assert(oo::ToUnderlying(oo::MissileStatus::kTargetLocked) == 2);

namespace oo {

enum class PlayerFleeingStatus : std::int32_t {
  kUnlikely = -1,
  kNone = 0,
  kMaybe = 1,
  kCargo = 2,
  kLikely = 3
};

}  // namespace oo

// The numbers JS sees.
static_assert(oo::ToUnderlying(oo::PlayerFleeingStatus::kUnlikely) == -1);
static_assert(oo::ToUnderlying(oo::PlayerFleeingStatus::kNone) == 0);
static_assert(oo::ToUnderlying(oo::PlayerFleeingStatus::kMaybe) == 1);
static_assert(oo::ToUnderlying(oo::PlayerFleeingStatus::kCargo) == 2);
static_assert(oo::ToUnderlying(oo::PlayerFleeingStatus::kLikely) == 3);

namespace oo {

enum class MarketFilterMode : std::int32_t {
  kOff = 0,
  kTrade = 1,
  kHold = 2,
  kStock = 3,
  kLegal = 4,
  kRestricted = 5  // import or export
};
// The number of modes (the description keys oolite-market-filter-0 to -5).
inline constexpr std::size_t kMarketFilterModeCount = 6;

}  // namespace oo

static_assert(oo::ToUnderlying(oo::MarketFilterMode::kOff) == 0);
static_assert(oo::ToUnderlying(oo::MarketFilterMode::kTrade) == 1);
static_assert(oo::ToUnderlying(oo::MarketFilterMode::kHold) == 2);
static_assert(oo::ToUnderlying(oo::MarketFilterMode::kStock) == 3);
static_assert(oo::ToUnderlying(oo::MarketFilterMode::kLegal) == 4);
static_assert(oo::ToUnderlying(oo::MarketFilterMode::kRestricted) == 5);

namespace oo {

enum class MarketSorterMode : std::int32_t {
  kOff = 0,
  kAlpha = 1,
  kPrice = 2,
  kStock = 3,
  kHold = 4,
  kUnit = 5
};
// The number of modes (the description keys oolite-market-sorter-0 to -5).
inline constexpr std::size_t kMarketSorterModeCount = 6;

}  // namespace oo

static_assert(oo::ToUnderlying(oo::MarketSorterMode::kOff) == 0);
static_assert(oo::ToUnderlying(oo::MarketSorterMode::kAlpha) == 1);
static_assert(oo::ToUnderlying(oo::MarketSorterMode::kPrice) == 2);
static_assert(oo::ToUnderlying(oo::MarketSorterMode::kStock) == 3);
static_assert(oo::ToUnderlying(oo::MarketSorterMode::kHold) == 4);
static_assert(oo::ToUnderlying(oo::MarketSorterMode::kUnit) == 5);

inline constexpr float ECM_ENERGY_DRAIN_FACTOR = 20.0f;
inline constexpr float ECM_DURATION = 2.5f;

inline constexpr float ROLL_DAMPING_FACTOR = 1.0f;
inline constexpr float PITCH_DAMPING_FACTOR = 1.0f;
inline constexpr float YAW_DAMPING_FACTOR = 1.0f;

inline constexpr float PLAYER_MAX_WEAPON_TEMP = 256.0f;
#ifdef OO_DUMP_PLANETINFO
// debugging
inline constexpr int PLAYER_MAX_FUEL = 7000;
#else
inline constexpr int PLAYER_MAX_FUEL = 70;
#endif
inline constexpr int PLAYER_MAX_MISSILES = 16;
inline constexpr int PLAYER_STARTING_MAX_MISSILES = 4;
inline constexpr int PLAYER_STARTING_MISSILES = 3;
inline constexpr double PLAYER_DIAL_MAX_ALTITUDE = 40000.0;
inline constexpr double PLAYER_SUPER_ALTITUDE2 = 10000000000.0;

inline constexpr int PLAYER_MAX_TRUMBLES = 24;

inline constexpr int PLAYER_TARGET_MEMORY_SIZE = 16;

#if OO_VARIABLE_TORUS_SPEED
#define HYPERSPEED_FACTOR \
  (PLAYER != nullptr ? PLAYER->getHyperspeedFactor() : 0.0f)
inline constexpr double MIN_HYPERSPEED_FACTOR = 32.0;
inline constexpr double MAX_HYPERSPEED_FACTOR = 1024.0;
#else
inline constexpr double HYPERSPEED_FACTOR = 32.0;
#endif

inline constexpr std::string_view PLAYER_SHIP_DESC = "cobra3-player";

inline constexpr double ESCAPE_SEQUENCE_TIME = 10.0;

#define FORWARD_FACING_STRING OO_DESC("forward-facing-string")
#define AFT_FACING_STRING OO_DESC("aft-facing-string")
#define PORT_FACING_STRING OO_DESC("port-facing-string")
#define STARBOARD_FACING_STRING OO_DESC("starboard-facing-string")

inline constexpr double KEY_REPEAT_INTERVAL = 0.20;

inline constexpr auto PLAYER_SHIP_CLOCK_START = (2084004 * 86400.0);
// adding or removing a player ship subentity increases or decreases the ship's
// trade-in factor respectively by this amount
inline constexpr int PLAYER_SHIP_SUBENTITY_TRADE_IN_VALUE = 3;

inline constexpr std::string_view CONTRACTS_GOOD_KEY = "contracts_fulfilled";
inline constexpr std::string_view CONTRACTS_BAD_KEY = "contracts_expired";
inline constexpr std::string_view CONTRACTS_UNKNOWN_KEY = "contracts_unknown";
inline constexpr std::string_view PASSAGE_GOOD_KEY = "passage_fulfilled";
inline constexpr std::string_view PASSAGE_BAD_KEY = "passage_expired";
inline constexpr std::string_view PASSAGE_UNKNOWN_KEY = "passage_unknown";
inline constexpr std::string_view PARCEL_GOOD_KEY = "parcels_fulfilled";
inline constexpr std::string_view PARCEL_BAD_KEY = "parcels_expired";
inline constexpr std::string_view PARCEL_UNKNOWN_KEY = "parcels_unknown";

inline constexpr double SCANNER_ZOOM_RATE_UP = 2.0;
inline constexpr double SCANNER_ZOOM_RATE_DOWN = -8.0;
inline constexpr double SCANNER_ECM_FUZZINESS = 1.25;

inline constexpr int PLAYER_INTERNAL_DAMAGE_FACTOR = 31;

inline constexpr std::string_view PLAYER_DOCKING_AI_NAME =
    "oolite-player-AI.plist";

inline constexpr int MANIFEST_SCREEN_ROW_BACK = 1;
#define MANIFEST_SCREEN_ROW_NEXT                        \
  ((PLAYER != nullptr && PLAYER->getHud() != nullptr && \
    PLAYER->getHud()->isHidden())                       \
       ? 27                                             \
       : 20)  // a nil HUD answered NO

inline constexpr std::string_view MISSION_DEST_LEGACY =
    "__oolite_legacy_destinations";

/*	The player.

        Every data member is zero-initialised, except three that share a name
   with a member function the player overrides (alertConditionLevel,
   legalStatusValue, suppressTargetLostFlag).
*/
class PlayerEntity : public ShipEntity {
 public:
  /*	The shared player, made on first use and kept for the process.
     Construction runs nothing but the ship's part; deferredInit() sets up the
     ship.
  */
  static PlayerEntity* sharedPlayer();
  // Out of line, where every member's class is complete (a test makes a
  // player).
  PlayerEntity();
  ~PlayerEntity() override;
  // A new player, retained (+1), as the universe
  // makes one when its entity list is empty; sharedPlayer() keeps the first
  // one.
  static std::shared_ptr<::ShipEntity> newPlayerObject();
  void deferredInit();
  // Run first when the player is destroyed, before the ship's teardown.
  void willDealloc();

  // Members named for a data member the player keeps.
  oo::AlertCondition alertCondition() override;
  int legalStatus() override;
  void suppressTargetLost() override;
  JSClassID jsClassID() override;
  std::optional<std::string> jsClassName() override;

  // cargo pods, commodity data and credits, galaxy and chart coordinates, the
  // current and previous system.
  void setName(const std::optional<std::string>& /*inName*/) override;
  GLfloat baseMass();
  void unloadAllCargoPodsForType(const std::string& type,
                                 ::oo::CommodityMarket* manifest);
  void unloadCargoPodsForType(const std::string& type,
                              oo::CargoQuantity quantity);
  void unloadCargoPods();
  void createCargoPodWithType(const std::string& type,
                              oo::CargoQuantity amount);
  void loadCargoPodsForType(const std::string& type,
                            ::oo::CommodityMarket* manifest);
  void loadCargoPodsForType(const std::string& type,
                            oo::CargoQuantity quantity);
  void loadCargoPods();
  ::oo::CommodityMarket* getShipCommodityData();
  oo::CreditsQuantity deciCredits();
  int random_factor();
  void setRandom_factor(int rf);
  virtual oo::GalaxyID galaxyNumber();
  virtual NSPoint getGalaxy_coordinates();
  void setGalaxyCoordinates(NSPoint newPosition);
  virtual NSPoint getCursor_coordinates();
  NSPoint getChart_centre_coordinates();
  oo::Scalar getChart_zoom();
  oo::Scalar getCustom_chart_zoom();
  void setCustomChartZoom(oo::Scalar zoom);
  NSPoint getCustom_chart_centre_coordinates();
  void setCustomChartCentre(NSPoint coords);
  NSPoint adjusted_chart_centre();
  virtual oo::RouteType ANAMode();
  virtual oo::SystemID systemID();
  void setSystemID(oo::SystemID sid);
  virtual oo::SystemID previousSystemID();
  void setPreviousSystemID(oo::SystemID sid);

  // target, next-hop and info systems, the wormhole, the commander data
  // dictionary (save).
  virtual oo::SystemID targetSystemID();
  virtual void setTargetSystemID(oo::SystemID sid);
  virtual oo::SystemID nextHopTargetSystemID();
  virtual oo::SystemID infoSystemID();
  virtual void setInfoSystemID(oo::SystemID sid, bool moveChart);
  void nextInfoSystem();
  void previousInfoSystem();
  void homeInfoSystem();
  void targetInfoSystem();
  bool infoSystemOnRoute();
  ::WormholeEntity* getWormhole();
  void setWormhole(::WormholeEntity* newWormhole);
  virtual oo::PList commanderDataDictionary();

  // setting the commander data from a dictionary (load).
  bool setCommanderDataFromDictionary(const oo::PList& dict);

  // set-up and start-up, ship set-up from the dictionary, the session, warning
  // about hostiles.
  virtual bool setUpAndConfirmOK(bool stopOnError);
  bool setUpAndConfirmOK(bool stopOnError, bool saveGame);
  void completeSetUp();
  void completeSetUpAndSetTarget(bool /*setTarget*/);
  void startUpComplete();
  bool setUpShipFromDictionary(const oo::PList& shipDict) override;
  std::size_t sessionID() override;
  void warnAboutHostiles() override;
  bool canCollide() override;

  // sun glare, the atmosphere, update:.
  oo::ComparisonResult compareZeroDistance(Entity* /*otherEntity*/) override;
  bool validForAddToUniverse() override;
  GLfloat lookingAtSunWithThresholdAngleCos(GLfloat thresholdAngleCos) override;
  GLfloat insideAtmosphereFraction();
  void update(oo::TimeDelta delta_t) override;

  // the bookkeeping tick (doBookkeeping:), movement flags.
  void doBookkeeping(double delta_t);
  void updateMovementFlags();

  // alert conditions, mass lock, fuel scoops, clocks, script and trumble ticks,
  // the autopilot and docking requests.
  void updateAlertConditionForNearbyEntities();
  void setMaxFlightPitch(GLfloat newValue) override;
  void setMaxFlightRoll(GLfloat newValue) override;
  void setMaxFlightYaw(GLfloat newValue) override;
  bool checkEntityForMassLock(Entity* ent, oo::ScanClass theirClass);
  void updateAlertCondition();
  void updateFuelScoops(oo::TimeDelta delta_t);
  void updateClocks(oo::TimeDelta delta_t);
  void checkScriptsIfAppropriate();
  void updateTrumbles(oo::TimeDelta delta_t);
  void performAutopilotUpdates(oo::TimeDelta delta_t);
  void performDockingRequest(::StationEntity* stationForDocking);
  virtual void requestDockingClearance(::StationEntity* stationForDocking);
  virtual void cancelDockingRequest(::StationEntity* stationForDocking);
  virtual bool engageAutopilotToStation(::StationEntity* stationForDocking);
  void disengageAutopilot() override;

  // the autopilot AI, hyperspeed, in-flight / witchspace / launch / docking /
  // dead updates, game over, the ship model view, targeting.
  void resetAutopilotAI();
#if OO_VARIABLE_TORUS_SPEED
  virtual GLfloat getHyperspeedFactor();
#endif
  virtual bool injectorsEngaged();
  virtual bool hyperspeedEngaged();
  void performInFlightUpdates(oo::TimeDelta delta_t);
  void performWitchspaceCountdownUpdates(oo::TimeDelta delta_t);
  void performWitchspaceExitUpdates(oo::TimeDelta delta_t);
  void performLaunchingUpdates(oo::TimeDelta delta_t);
  void performDockingUpdates(oo::TimeDelta /*delta_t*/);
  void performDeadUpdates(oo::TimeDelta /*delta_t*/);
  void gameOverFadeToBW();
  bool isValidTarget(Entity* target) override;
  void showGameOver();
  void showShipModelWithKey(const std::string& shipKey,
                            const oo::PList& shipDataIn, uint16_t personality,
                            GLfloat factorX, GLfloat factorY, GLfloat factorZ,
                            const std::optional<std::string>& context);
  void updateTargeting();

  // attitude, view matrices and viewpoints, drawing, mass lock, the docked
  // station, the HUD and its custom dials, shield levels.
  void orientationChanged() override;
  void applyAttitudeChanges(double delta_t) override;
  using ShipEntity::applyRoll;  // the yaw overload, which the player does not
                                // override
  void applyRoll(GLfloat roll1, GLfloat climb1) override;
  void applyYaw(GLfloat yaw);
  oo::Matrix drawRotationMatrix() override;
  oo::Matrix drawTransformationMatrix() override;
  Quaternion normalOrientation() override;
  void setNormalOrientation(Quaternion quat) override;
  void moveForward(double amount) override;
  HPVector breakPatternPosition();
  Vector viewpointOffset();
  virtual Vector viewpointOffsetAft();
  virtual Vector viewpointOffsetForward();
  virtual Vector viewpointOffsetPort();
  virtual Vector viewpointOffsetStarboard();
  virtual HPVector viewpointPosition();
  void drawImmediate(bool immediate, bool translucent) override;
  virtual void setMassLockable(bool newValue);
  virtual bool getMassLockable();
  virtual bool massLocked();
  virtual bool atHyperspeed();
  float occlusionLevel();
  void setOcclusionLevel(float level);
  void setDockedAtMainStation();
  virtual ::StationEntity* dockedStation();
  void setDockedStation(::StationEntity* station);
  void setTargetDockStationTo(::StationEntity* value);
  virtual ::StationEntity* getTargetDockStation();
  virtual ::HeadUpDisplay* getHud();
  virtual void resetHud();
  virtual bool switchHudTo(const std::string& hudFileName);
  float dialCustomFloat(const std::string& dialKey);
  std::string dialCustomString(const std::string& dialKey);
  std::shared_ptr<oo::Color> dialCustomColor(const std::string& dialKey);
  virtual void setDialCustom(const oo::PList& value,
                             const std::string& dialKey);
  void setShowDemoShips(bool value);
  bool getShowDemoShips();
  float maxForwardShieldLevel() override;
  float maxAftShieldLevel() override;
  virtual float forwardShieldRechargeRate();
  virtual float aftShieldRechargeRate();
  virtual void setMaxForwardShieldLevel(float newValue);
  virtual void setMaxAftShieldLevel(float newValue);
  virtual void setForwardShieldRechargeRate(float newValue);
  virtual void setAftShieldRechargeRate(float newValue);
  virtual GLfloat forwardShieldLevel();
  virtual GLfloat aftShieldLevel();
  virtual void setForwardShieldLevel(GLfloat level);
  virtual void setAftShieldLevel(GLfloat level);
  oo::PList keyConfig();
  virtual bool isMouseControlOn();
  GLfloat dialRoll();
  GLfloat dialPitch();
  GLfloat dialYaw();
  GLfloat dialSpeed();
  GLfloat dialHyperSpeed();
  virtual GLfloat dialForwardShield();

  // the dials (shields, energy, fuel, heat, altitude, clock, missiles, scoop),
  // fuel leak, the comm log, player roles, system memory, the compass target.
  virtual GLfloat dialAftShield();
  GLfloat dialEnergy();
  GLfloat dialMaxEnergy();
  GLfloat dialFuel();
  GLfloat dialHyperRange();
  GLfloat laserHeatLevel() override;
  GLfloat laserHeatLevelAft() override;
  GLfloat laserHeatLevelForward() override;
  GLfloat laserHeatLevelPort() override;
  GLfloat laserHeatLevelStarboard() override;
  GLfloat dialAltitude();
  virtual double clockTime();
  virtual double clockTimeAdjusted();
  bool clockAdjusting();
  void addToAdjustTime(double seconds);
  double escapePodRescueTime();
  void setEscapePodRescueTime(double seconds);
  std::string dial_clock();
  std::string dial_clock_adjusted();
  std::string dial_fpsinfo();
  std::string dial_objinfo();
  unsigned countMissiles();
  virtual oo::MissileStatus dialMissileStatus();
  bool canScoop(::ShipEntity* other) override;
  virtual oo::FuelScoopStatus dialFuelScoopStatus();
  virtual float fuelLeakRate();
  virtual void setFuelLeakRate(float value);
  std::vector<std::string>* getCommLog();
  std::vector<std::string> getRoleWeights();
  void addRoleForAggression(::ShipEntity* victim);
  void addRoleForMining();
  virtual void addRoleToPlayer(const std::string& role);
  void addRoleToPlayer(const std::string& role, std::size_t slot);
  void clearRoleFromPlayer(bool includingLongRange);
  void clearRolesFromPlayer(float chance);
  std::size_t maxPlayerRoles();
  void updateSystemMemory();
  virtual Entity*
  getCompassTargetEntity();  // the compass target's C++ entity, null for none
                             // or once its object has gone
  void setCompassTargetEntity(Entity* value);
  virtual void validateCompassTarget();
  std::optional<std::string> compassTargetLabel();

  // compass mode, missiles and pylons, special cargo, the multi-function
  // displays, alert flags.
  virtual oo::CompassMode getCompassMode();
  virtual void setCompassMode(oo::CompassMode value);
  void setPrevCompassMode();
  virtual void setNextCompassMode();
  virtual std::size_t getActiveMissile();
  void setActiveMissile(std::size_t value);
  std::size_t dialMaxMissiles();
  virtual bool dialIdentEngaged();
  void setDialIdentEngaged(bool newValue);
  virtual std::optional<std::string> getSpecialCargo();
  std::optional<std::string> dialTargetName();
  virtual std::vector<std::optional<std::string>> multiFunctionDisplayList();
  std::optional<std::string> multiFunctionText(std::size_t i);
  virtual void setMultiFunctionText(const std::optional<std::string>& text,
                                    const std::optional<std::string>& key);
  virtual bool setMultiFunctionDisplay(std::size_t index,
                                       const std::optional<std::string>& key);
  void cycleNextMultiFunctionDisplay(std::size_t index);
  void cyclePreviousMultiFunctionDisplay(std::size_t index);
  void selectNextMultiFunctionDisplay();
  void selectPreviousMultiFunctionDisplay();
  std::size_t getActiveMFD();
  ::ShipEntity* missileForPylon(std::size_t value);
  virtual void safeAllMissiles();
  void tidyMissilePylons();
  void selectNextMissile();
  void clearAlertFlags();
  int getAlertFlags();
  void setAlertFlag(oo::AlertFlags flag, bool value);
  oo::AlertCondition realAlertCondition() override;

  // alert condition, AI messages, mounting and firing missiles and mines, the
  // cloak, ECM, energy units, the main weapons.
  virtual oo::AlertCondition getAlertCondition();
  virtual oo::PlayerFleeingStatus fleeingStatus();
  void interpretAIMessage(const std::string& message) override;
  bool mountMissile(::ShipEntity* missile);
  bool mountMissileWithRole(const std::string& role);
  ::ShipEntity* fireMissile() override;
  ::ShipEntity* launchMine(::ShipEntity* mine);
  virtual bool assignToActivePylon(const std::string& equipmentKey);
  bool activateCloakingDevice() override;
  void deactivateCloakingDevice() override;
  double scannerFuzziness();
  void noticeECM() override;
  bool fireECM() override;
  oo::EnergyUnitType installedEnergyUnitType();
  oo::EnergyUnitType energyUnitType();
  void currentWeaponStats();
  virtual bool weaponsOnline();
  void setWeaponsOnline(bool newValue);
  std::vector<Vector> currentLaserOffset();
  using ShipEntity::fireMainWeapon;  // the ship's fireMainWeapon(range),
                                     // another selector
  bool fireMainWeapon();
  oo::WeaponType weaponForFacing(oo::WeaponFacing facing);
  oo::WeaponType currentWeapon();

  // hit testing, damage, the doppelganger, the escape capsule, dumping cargo,
  // bounty.
  using ShipEntity::doesHitLine;  // the ship's other overloads, which the
                                  // player does not override
  GLfloat doesHitLine(HPVector v0, HPVector v1,
                      ::ShipEntity** hitEntity) override;
  void takeEnergyDamage(double amount, Entity* entPart, Entity* otherPart,
                        const std::string& weaponIdentifier) override;
  void takeScrapeDamage(double amount, Entity* ent) override;
  void takeHeatDamage(double amount) override;
  ::ShipEntity* createDoppelganger();  // the proxy
  ::ShipEntity* launchEscapeCapsule() override;
  void dumpCargo() override;
  void rotateCargo();
  void setBounty(oo::CreditsQuantity amount) override;
  void setBounty(oo::CreditsQuantity amount,
                 oo::LegalStatusReason reason) override;
  void setBounty(oo::CreditsQuantity amount,
                 const std::string& reason) override;

  // legal status, offences, bounties collected, internal damage, destruction,
  // ending a scenario, docking and leaving dock.
  oo::CreditsQuantity getBounty() override;
  int getLegalStatus();
  void markAsOffender(int offence_value) override;
  void markAsOffender(int offence_value, oo::LegalStatusReason reason) override;
  void collectBountyFor(::ShipEntity* other) override;
  virtual bool takeInternalDamage();
  void getDestroyedBy(Entity* whom, oo::ShipDamageType type) override;
  void loseTargetStatus();
  bool endScenario(const std::string& key);
  void enterDock(::StationEntity* station) override;
  void docked();
  void leaveDock(::StationEntity* station) override;

  // witchspace: start, end, checklist, jump type and distance, fuel, galactic
  // and wormhole jumps.
  void witchStart();
  void witchEnd();
  virtual bool witchJumpChecklist(bool isGalacticJump);
  virtual void setJumpType(bool isGalacticJump);
  double hyperspaceJumpDistance();
  virtual oo::FuelQuantity fuelRequiredForJump();
  bool hasSufficientFuelForJump();
  void noteCompassLostTarget();
  void enterGalacticWitchspace();
  using ShipEntity::enterWormhole;  // the ship's other overloads, which the
                                    // player does not override
  void enterWormhole(::WormholeEntity* w_hole) override;
  void enterWitchspace() override;
  void witchJumpTo(oo::SystemID sTo, bool misjump);

  // leaving witchspace, the status screen, the equipment list, primed and fast
  // equipment, weapon types.
  void leaveWitchspace() override;
  void setGuiToStatusScreen();
  std::vector<oo::PList> equipmentList();
  std::size_t primedEquipmentCount();
  std::optional<std::string> primedEquipmentName(std::ptrdiff_t offset);
  virtual std::string currentPrimedEquipment();
  virtual bool setPrimedEquipment(const std::string& eqKey, bool showMsg);
  void activatePrimableEquipment(std::size_t index,
                                 oo::PrimedEquipmentMode mode);
  virtual std::optional<std::string> fastEquipmentA();
  virtual std::optional<std::string> fastEquipmentB();
  virtual void setFastEquipmentA(const std::optional<std::string>& eqKey);
  virtual void setFastEquipmentB(const std::optional<std::string>& eqKey);
  ::oo::EquipmentType* weaponTypeForFacing(oo::WeaponFacing facing,
                                           bool /*strict*/) override;

  // scripting lists (missiles, cargo, contracts), the system data screen,
  // marked destinations, the chart screens.
  std::vector<std::shared_ptr<::oo::EquipmentType>> missilesList() override;
  std::vector<std::string> cargoList();
  oo::PList cargoListForScripting() override;
  unsigned legalStatusOfCargoList();
  oo::PList::Array contractsListForScriptingFromArray(
      const oo::PList::Array& contracts_array, bool forCargo);
  oo::PList passengerListForScripting() override;
  oo::PList parcelListForScripting() override;
  oo::PList contractListForScripting() override;
  void setGuiToSystemDataScreen();
  virtual void setGuiToSystemDataScreenRefreshBackground(
      bool refreshBackground);
  std::optional<std::map<int, std::vector<oo::PList>>> markedDestinations();
  void setGuiToLongRangeChartScreen();
  void setGuiToShortRangeChartScreen();
  void setGuiToChartScreenFrom(oo::GUIScreenID oldScreen);

  // the game options and load / save screens, equip-screen key highlight,
  // available facings.
  void setGuiToGameOptionsScreen();
  void setGuiToLoadSaveScreen();
  void highlightEquipShipScreenKey(const std::string& highlightKey);
  virtual oo::WeaponFacingSet availableFacings();

  // the equip-ship screen and upgrade information.
  virtual void setGuiToEquipShipScreen(
      int skipParam, const std::optional<std::string>& eqKeyForSelectFacing);
  void setGuiToEquipShipScreen(int skip);
  void showInformationForSelectedUpgrade();
  virtual void showInformationForSelectedUpgradeWithFormatString(
      const std::optional<std::string>& formatString);

  // the interfaces screen, the start screen and intro, the OXZ manager, GUI and
  // view change notes.
  void setGuiToInterfacesScreen(int skip);
  void showInformationForSelectedInterface();
  void activateSelectedInterface();
  void setupStartScreenGui();
  void setGuiToIntroFirstGo(bool justCobra);
  void setGuiToOXZManager();
  void noteGUIWillChangeTo(oo::GUIScreenID toScreen);
  void noteGUIDidChangeFrom(oo::GUIScreenID fromScreen,
                            oo::GUIScreenID toScreen);
  virtual void noteGUIDidChangeFrom(oo::GUIScreenID fromScreen,
                                    oo::GUIScreenID toScreen, bool refresh);
  void noteViewDidChangeFrom(oo::ViewID fromView, oo::ViewID toView);

  // buying equipment, script price adjustment, weapon mounts, passenger berths,
  // removing missiles, trade-in.
  void buySelectedItem();
  oo::CreditsQuantity adjustPriceByScriptForEqKey(const std::string& eqKey,
                                                  oo::CreditsQuantity price);
  bool tryBuyingItem(const std::string& eqKey);
  bool setWeaponMount(oo::WeaponFacing facing,
                      const std::string& eqKey) override;
  virtual bool setWeaponMount(oo::WeaponFacing facing, const std::string& eqKey,
                              const std::optional<std::string>& context);
  bool changePassengerBerths(int addRemove);
  oo::CreditsQuantity removeMissiles() override;
  void doTradeIn(oo::CreditsQuantity tradeInValue, double priceFactor);

  // cargo quantities, the local market, market filters and sorters, market
  // screen rows.
  oo::CargoQuantity cargoQuantityForType(const std::string& type);
  oo::CargoQuantity setCargoQuantityForType(const std::string& type,
                                            oo::CargoQuantity amount);
  void calculateCurrentCargo();
  oo::CargoQuantity cargoQuantityOnBoard() override;
  ::oo::CommodityMarket* localMarket();
  std::vector<std::string> applyMarketFilter(
      const std::vector<std::string>& goods, ::oo::CommodityMarket* market);
  std::vector<std::string> applyMarketSorter(
      const std::vector<std::string>& goods, ::oo::CommodityMarket* market);
  void showMarketScreenHeaders();
  void showMarketScreenDataLine(oo::GUIRow row, const std::string& good,
                                ::oo::CommodityMarket* localMarket,
                                oo::CargoQuantity quantity);
  std::optional<std::string> marketScreenTitle();

  // the market screens, buying and selling commodities, mining and speech
  // flags, adding equipment.
  void setGuiToMarketScreen();
  void setGuiToMarketInfoScreen();
  void showMarketCashAndLoadLine();
  oo::GUIScreenID guiScreen();
  bool tryBuyingCommodity(const std::string& index, bool all);
  bool trySellingCommodity(const std::string& index, bool all);
  bool isMining() override;
  oo::SpeechSettings getIsSpeechOn();
  bool canAddEquipment(const std::string& equipmentKey,
                       const std::string& context) override;
  bool addEquipmentItem(const std::string& equipmentKey,
                        const std::string& context) override;

  // adding / removing / custom-activated equipment, pylons, parcels and
  // passengers, comms, fines, trade-in factor, renovation, view offsets,
  // trumbles.
  bool addEquipmentItem(const std::string& equipmentKey, bool validateAddition,
                        const std::string& context) override;
  std::vector<oo::PList>* customEquipmentActivation();
  void addEquipmentWithScriptToCustomKeyArray(const std::string& equipmentKey);
  void validateCustomEquipActivationArray();
  void removeEquipmentItem(const std::string& equipmentKey) override;
  void addEquipmentFromCollection(const oo::PList& equipment);
  using ShipEntity::hasOneEquipmentItem;  // the ship's (itemKey,
                                          // includeWeapons, loading), another
                                          // selector
  bool hasOneEquipmentItem(const std::string& itemKey, bool includeMissiles);
  bool hasPrimaryWeapon(oo::WeaponType weaponType) override;
  bool removeExternalStore(::oo::EquipmentType* eqType) override;
  bool removeFromPylon(std::size_t pylon);
  std::size_t parcelCount() override;
  std::size_t passengerCount() override;
  std::size_t passengerCapacity() override;
  bool hasHostileTarget() override;
  void receiveCommsMessage(const std::string& message_text,
                           ::ShipEntity* other) override;
  void getFined();
  virtual void adjustTradeInFactorBy(int value);
  virtual int tradeInFactor();
  virtual double renovationCosts();
  virtual double renovationFactor();
  void setDefaultViewOffsets();
  void setDefaultCustomViews();
  Vector weaponViewOffset();
  void setUpTrumbles();
  void addTrumble(oo::Trumble* papaTrumble);
  void removeTrumble(oo::Trumble* deadTrumble);
  std::shared_ptr<oo::Trumble>* trumbleArray();
  virtual std::size_t getTrumbleCount();

  // trumble values, checksums, screen modes, target memory, missile ident,
  // rotating and panning the custom view.
  oo::PList trumbleValue();
  void setTrumbleValueFrom(const oo::PList& trumbleValue);
  float trumbleAppetiteAccumulator();
  void setTrumbleAppetiteAccumulator(float value);
  void mungChecksumWithString(const std::optional<std::string>& str);
  std::optional<std::string> screenModeStringForWidth(unsigned width,
                                                      unsigned height,
                                                      float refreshRate);
  void getSuppressTargetLost();
  virtual void setScoopsActive();
  void setFoundTarget(Entity* targetEntity) override;
  void addTarget(Entity* targetEntity) override;
  void clearTargetMemory();
  std::vector<std::weak_ptr<Entity>> targetMemory();
  bool moveTargetMemoryBy(std::ptrdiff_t delta);
  virtual void printIdentLockedOnForMissile(bool missile);
  Quaternion getCustomViewQuaternion();
  void setCustomViewQuaternion(Quaternion q);
  oo::Matrix getCustomViewMatrix();
  Vector getCustomViewOffset();
  void setCustomViewOffset(Vector offset);
  Vector getCustomViewRotationCenter();
  void setCustomViewRotationCenter(Vector center);
  void customViewZoomIn(oo::Scalar rate);
  void customViewZoomOut(oo::Scalar rate);
  void customViewRotateLeft(oo::Scalar angle);
  void customViewRotateRight(oo::Scalar angle);
  void customViewRotateUp(oo::Scalar angle);
  void customViewRotateDown(oo::Scalar angle);
  void customViewRollRight(oo::Scalar angle);
  void customViewRollLeft(oo::Scalar angle);
  void customViewPanUp(oo::Scalar angle);
  void customViewPanDown(oo::Scalar angle);

  // custom view vectors and data, the mission overlay and background, world
  // scripts and script events, galactic hyperspace, jump cause, commander and
  // save names.
  void customViewPanLeft(oo::Scalar angle);
  void customViewPanRight(oo::Scalar angle);
  Vector getCustomViewForwardVector();
  Vector getCustomViewUpVector();
  Vector getCustomViewRightVector();
  std::optional<std::string> getCustomViewDescription();
  virtual void resetCustomView();
  void setCustomViewData();
  virtual void setCustomViewDataFromDictionary(const oo::PList& viewDict,
                                               bool withScaling);
  bool showInfoFlag();
  oo::PList missionOverlayDescriptor();
  oo::PList missionOverlayDescriptorOrDefault();
  void setMissionOverlayDescriptor(const oo::PList& descriptor);
  oo::PList missionBackgroundDescriptor();
  oo::PList missionBackgroundDescriptorOrDefault();
  void setMissionBackgroundDescriptor(const oo::PList& descriptor);
  oo::GUIBackgroundSpecial missionBackgroundSpecial();
  void setMissionBackgroundSpecial(const std::string& special);
  void setMissionExitScreen(oo::GUIScreenID screen);
  oo::GUIScreenID missionExitScreen();
  oo::PList equipScreenBackgroundDescriptor();
  void setEquipScreenBackgroundDescriptor(const oo::PList& descriptor);
  bool scriptsLoaded();
  std::vector<std::string> worldScriptNames();
  std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>
  worldScriptsByName();
  ::OOScript* commodityScriptNamed(
      const std::optional<std::string>& scriptName);
  using ShipEntity::doScriptEvent;  // the ship's other overloads, which the
                                    // player does not override
  void doScriptEvent(OOJSPropID message, JSContext* context, JSValue* argv,
                     int argc) override;
  bool doWorldEventUntilMissionScreen(OOJSPropID message);
  // argv is borrowed: the caller remains responsible for freeing the values.
  void doWorldScriptEvent(OOJSPropID message, JSContext* context, JSValue* argv,
                          int argc, oo::TimeDelta limit);
  virtual void setGalacticHyperspaceBehaviour(
      oo::GalacticHyperspaceBehaviour inBehaviour);
  virtual oo::GalacticHyperspaceBehaviour getGalacticHyperspaceBehaviour();
  virtual void setGalacticHyperspaceFixedCoords(NSPoint point);
  void setGalacticHyperspaceFixedCoordsX(unsigned char x, unsigned char y);
  virtual NSPoint getGalacticHyperspaceFixedCoords();
  virtual void setWitchspaceCountdown(int spin_time);
  virtual oo::LongRangeChartMode getLongRangeChartMode();
  virtual void setLongRangeChartMode(oo::LongRangeChartMode mode);
  virtual bool getScoopOverride();
  virtual void setScoopOverride(bool newValue);
  GLfloat fuelChargeRate()
      override;  // the ship's rate unless MASS_DEPENDENT_FUEL_PRICES
                 // (Universe.h, not seen here)
  void setDockTarget(::ShipEntity* entity);
  std::optional<std::string> jumpCause();
  void setJumpCause(const std::optional<std::string>& value);
  std::optional<std::string> commanderName();
  std::optional<std::string> lastsaveName();
  void setCommanderName(const std::optional<std::string>& value);
  void setLastsaveName(const std::optional<std::string>& value);

  // docking clearance, scanned wormholes, mission destinations, the shipyard
  // record, extra mission and GUI-screen keys, the state dump.
  virtual bool isDocked();
  bool clearedToDock();
  void setDockingClearanceStatus(oo::DockingClearanceStatus newValue);
  oo::DockingClearanceStatus getDockingClearanceStatus();
  void penaltyForUnauthorizedDocking();
  void addScannedWormhole(::WormholeEntity* whole);
  void updateWormholes();
  std::vector<oo::EntityHold>
  getScannedWormholes();  // the wormholes, their objects held
  void initialiseMissionDestinations(const oo::PList& destinations,
                                     const oo::PList& legacy);
  std::optional<std::string> markerKey(const oo::PList& marker);
  void addMissionDestinationMarker(const oo::PList& marker);
  bool removeMissionDestinationMarker(const oo::PList& marker);
  oo::PList getMissionDestinations();
  oo::PList::Dict* shipyardRecord();
  void setLastShot(
      const std::vector<std::shared_ptr<oo::LaserShotEntity>>& shot);
  void clearExtraMissionKeys();
  void setExtraMissionKeys(const oo::PList& keys);
  void clearExtraGuiScreenKeys(oo::GUIScreenID gui, const std::string& key);
  bool setExtraGuiScreenKeys(oo::GUIScreenID gui,
                             ::OOJSGuiScreenKeyDefinition* definition);
#ifdef OO_DEBUG_MONITOR_ENABLED
  void dumpSelfState() override;
#endif

  // Category ScriptMethods (PlayerEntityScriptMethods.cpp): methods for use by
  // scripting mechanisms. std::nullopt is nil in the Foundation-typed forms.
  virtual unsigned score();
  void setScore(unsigned value);
  double creditBalance();
  void setCreditBalance(double value);
  std::optional<std::string> dockedStationName();
  std::optional<std::string> dockedStationDisplayName();
  bool dockedAtMainStation();
  void awardCommodityType(const std::string& type, oo::CargoQuantity amount);
  virtual void resetScannerZoom();
  virtual oo::GalaxyID currentGalaxyID();
  virtual oo::SystemID currentSystemID();
  void setMissionChoice(const std::optional<std::string>& newChoice);
  void setMissionChoice(const std::optional<std::string>& newChoice,
                        bool withEvent);
  void setMissionChoice(const std::optional<std::string>& newChoice,
                        const std::optional<std::string>& keyPress);
  void setMissionChoice(const std::optional<std::string>& newChoice,
                        const std::optional<std::string>& keyPress,
                        bool withEvent);
  void allowMissionInterrupt();
  oo::TimeDelta scriptTimer();
  virtual unsigned systemPseudoRandom100();
  virtual unsigned systemPseudoRandom256();
  virtual double systemPseudoRandomFloat();
  oo::PList passengerContractMarker(oo::SystemID system);
  oo::PList parcelContractMarker(oo::SystemID system);
  oo::PList cargoContractMarker(oo::SystemID system);
  oo::PList defaultMarker(oo::SystemID system);
  oo::PList validatedMarker(const oo::PList& marker);
  std::optional<std::string> keyBindingDescription2(const std::string& binding);
  std::optional<std::string> getKeyBindingDescription(const oo::PList& keyList);
  std::optional<std::string> keyCodeDescription(oo::KeyCode code);
  std::optional<std::string> keyCodeDescriptionShort(oo::KeyCode code);
  std::optional<std::string> commanderKillsAsString();
  std::optional<std::string> commanderBountyAsString();
  std::optional<std::string> creditsFormattedForSubstitution();
  std::optional<std::string> creditsFormattedForLegacySubstitution();

  // PlayerEntitySound.cpp: the interface, warning, damage and weapon sounds,
  // and the afterburner loop.
  void setUpSound();
  void setUpWeaponSounds();
  void destroySound();
  void playInterfaceBeep(const std::string& beepKey);
  bool isBeeping();
  void boop();
  virtual void playIdentOn();
  void playIdentOff();
  virtual void playIdentLockedOn();
  void playMissileArmed();
  void playMineArmed();
  void playMissileSafe();
  void playMissileLockedOn();
  void playNextEquipmentSelected();
  void playNextMissileSelected();
  void playWeaponsOnline();
  void playWeaponsOffline();
  void playCargoJettisioned();
  void playAutopilotOn();
  void playAutopilotOff();
  void playAutopilotOutOfRange();
  void playAutopilotCannotDockWithTarget();
  void playSaveOverwriteYes();
  void playSaveOverwriteNo();
  void playHoldFull();
  void playJumpMassLocked();
  void playTargetLost();
  void playNoTargetInMemory();
  void playTargetSwitched();
  void playHyperspaceNoTarget();
  void playHyperspaceNoFuel();
  void playHyperspaceBlocked();
  void playHyperspaceDistanceTooGreat();
  void playCloakingDeviceOn();
  void playCloakingDeviceOff();
  void playMenuNavigationUp();
  void playMenuNavigationDown();
  void playMenuNavigationNot();
  void playMenuPagePrevious();
  void playMenuPageNext();
  void playDismissedReportScreen();
  void playDismissedMissionScreen();
  void playChangedOption();
  void updateFuelScoopSoundWithInterval(oo::TimeDelta delta_t);
  void updateAfterburnerSound();
  void startAfterburnerSound();
  void stopAfterburnerSound();
  void playCloakingDeviceInsufficientEnergy();
  void playBuyCommodity();
  void playBuyShip();
  void playSellCommodity();
  void playCantBuyCommodity();
  void playCantSellCommodity();
  void playCantBuyShip();
  void playStandardHyperspace();
  virtual void playGalacticHyperspace();
  void playHyperspaceAborted();
  void playHitByECMSound();
  void playFiredECMSound();
  void playLaunchFromStation();
  void playDockWithStation();
  void playExitWitchspace();
  void playHostileWarning();
  void playAlertConditionRed();
  void playIncomingMissile(Vector missileVector);
  void playEnergyLow();
  void playDockingDenied();
  void playWitchjumpFailure();
  void playWitchjumpMisjump();
  void playWitchjumpBlocked();
  void playWitchjumpDistanceTooGreat();
  void playWitchjumpInsufficientFuel();
  void playFuelLeak();
  void playShieldHit(Vector attackVector, const std::string& weaponIdentifier);
  void playDirectHit(Vector attackVector, const std::string& weaponIdentifier);
  void playScrapeDamage(Vector attackVector);
  void playLaserHit(bool hit, Vector weaponOffset,
                    const std::string& weaponIdentifier);
  void playWeaponOverheated(Vector weaponOffset);
  void playMissileLaunched(Vector weaponOffset,
                           const std::string& weaponIdentifier);
  void playMineLaunched(Vector weaponOffset,
                        const std::string& weaponIdentifier);
  void playEscapePodScooped();
  void playAegisCloseToPlanet();
  void playAegisCloseToStation();
  void playGameOver();
  void playLegacyScriptSound(const std::string& key);

  // PlayerEntityStickMapper.cpp, the categories StickMapper: defined in that
  // file.
  void resetStickFunctions();
  void setGuiToStickMapperScreen(unsigned skip);
  void setGuiToStickMapperScreen(unsigned skip, bool resetCurrentRow);
  void stickMapperInputHandler(::GuiDisplayGen* gui, ::MyOpenGLView* gameView);
  void updateFunction(const oo::PList& hwDict);
  void checkCustomEquipButtons(const oo::PList& stickFn, int idx);
  void removeFunction(int idx);
  void displayFunctionList(::GuiDisplayGen* gui, std::size_t skip);
  std::optional<std::string> describeStickDict(const oo::PList* stickDict);
  std::string hwToString(int hwFlags);
  std::vector<oo::PList> stickFunctionList();
  oo::PList makeStickGuiDict(const std::string& what, int allowable, int axisfn,
                             int butfn);
  oo::PList makeStickGuiDictHeader(const std::string& header);

  // PlayerEntityStickProfile.cpp, the categories StickProfile: defined in that
  // file.
  void setGuiToStickProfileScreen(::GuiDisplayGen* gui);
  void stickProfileInputHandler(::GuiDisplayGen* gui, ::MyOpenGLView* gameView);
  void stickProfileGraphAxisProfile(GLfloat alpha, Vector screenAt,
                                    NSSize screenSize);

  // PlayerEntityControls.cpp, the categories Controls, OOControlsPrivate:
  // defined in that file.
  virtual void initControls();
  void initKeyConfigSettings();
  oo::PList processKeyCode(const oo::PList& key_def);
  bool checkNavKeyPress(const oo::PList& key_def);
  bool checkKeyPress(const oo::PList& key_def);
  bool checkKeyPress(const oo::PList& key_def, bool fKey_only);
  bool checkKeyPressIgnoreCtrl(const oo::PList& key_def, bool ignore_ctrl);
  bool checkKeyPress(const oo::PList& key_def, bool fKey_only,
                     bool ignore_ctrl);
  int getFirstKeyCode(const oo::PList& key_def);
  void getPollControls(double delta_t);
  bool handleGUIUpDownArrowKeys();
  void targetNewSystem(int direction, bool whileTyping);
  void clearPlanetSearchString();
  void targetNewSystem(int direction);
  void switchToMainView();
  virtual void noteSwitchToView(oo::ViewID toView, oo::ViewID fromView);
  virtual void beginWitchspaceCountdown(int spin_time);
  void beginWitchspaceCountdown();
  virtual void cancelWitchspaceCountdown();
  void pollApplicationControls();
  void pollFlightControls(double delta_t);
  void pollGuiArrowKeyControls(double delta_t);
  void pollMarketScreenControls();
  void handleGameOptionsScreenKeys();
  void handleKeyMapperScreenKeys();
  void handleKeyboardLayoutKeys();
  void handleStickMapperScreenKeys();
  void pollCustomViewControls();
  void pollViewControls();
  void pollFlightArrowKeyControls(double delta_t);
  void pollGuiScreenControls();
  void pollGuiScreenControlsWithFKeyAlias(bool fKeyAlias);
  void pollGameOverControls(double delta_t);
  void pollAutopilotControls(double delta_t);
  void pollDockedControls(double delta_t);
  void handleUndockControl();
  void pollDemoControls(double delta_t);
  void pollMissionInterruptControls();
  void handleMissionCallback();
  void setGuiToMissionEndScreen();
  void switchToThisView(oo::ViewID viewDirection);
  void switchToThisView(oo::ViewID viewDirection, bool processWeaponFacing);
  void switchToThisView(oo::ViewID viewDirection, oo::ViewID oldViewDirection,
                        bool processWeaponFacing, bool justNotify);
  void handleAutopilotOn(bool fastDocking);
  void handleButtonIdent();
  void handleButtonTargetMissile();

  // PlayerEntityKeyMapper.cpp, the categories KeyMapper: defined in that file.
  void initCheckingDictionary();
  void resetKeyFunctions();
  void setGuiToKeyMapperScreen(unsigned skip);
  void setGuiToKeyMapperScreen(unsigned skip, bool resetCurrentRow);
  void keyMapperInputHandler(::GuiDisplayGen* gui, ::MyOpenGLView* gameView);
  bool entryIsIndexCustomEquip(std::size_t idx);
  bool entryIsDictCustomEquip(const oo::PList& dict);
  bool entryIsCustomEquip(const std::string& entry);
  oo::PList getCustomEquipArray(const std::string& key_def);
  std::size_t getCustomEquipIndex(const std::string& key_def);
  std::optional<std::string> getCustomEquipKeyDefType(
      const std::string& key_def);
  void setGuiToKeyConfigScreen();
  void setGuiToKeyConfigScreen(bool resetSelectedRow);
  void outputKeyDefinition(const std::string& key, const std::string& shift,
                           const std::string& mod1, const std::string& mod2,
                           std::size_t skiprows);
  void handleKeyConfigKeys(::GuiDisplayGen* gui, ::MyOpenGLView* gameView);
  void setGuiToKeyConfigEntryScreen();
  void handleKeyConfigEntryKeys(::GuiDisplayGen* gui, ::MyOpenGLView* gameView);
  void updateKeyDefinition(const std::string& keystring, std::size_t index);
  void updateShiftKeyDefinition(const std::string& key, std::size_t index);
  void setGuiToConfirmClearScreen();
  void handleKeyMapperConfirmClearKeys(::GuiDisplayGen* gui,
                                       ::MyOpenGLView* gameView);
  void displayKeyFunctionList(::GuiDisplayGen* gui, std::size_t skip);
  std::vector<oo::PList> keyFunctionList();
  oo::PList makeKeyGuiDict(const std::string& what, const std::string& key_def);
  oo::PList makeKeyGuiDictHeader(const std::string& header);
  void setGuiToKeyboardLayoutScreen(unsigned skip);
  void setGuiToKeyboardLayoutScreen(unsigned skip, bool resetCurrentRow);
  void handleKeyboardLayoutEntryKeys(::GuiDisplayGen* gui,
                                     ::MyOpenGLView* gameView);
  std::optional<std::string> keyboardDescription(const std::string& kbd);
  std::vector<oo::PList> keyboardLayoutList();
  void displayKeyboardLayoutList(::GuiDisplayGen* gui, std::size_t skip);
  std::vector<std::string> validateAllKeys();
  std::optional<std::string> validateKey(const std::string& key,
                                         const oo::PList& check_keys);
  std::optional<std::string> searchArrayForMatch(
      const std::vector<std::string>& search_list, const std::string& key,
      const oo::PList& check_keys);
  bool entryIsEqualToDefault(const std::string& key);
  bool compareKeyEntries(const oo::PList& first, const oo::PList& second);
  void saveKeySetting(const std::string& key);
  void unsetKeySetting(const std::string& key);
  void deleteKeySetting(const std::string& key);
  void deleteAllKeySettings();
  oo::PList loadKeySettings();
  void reloadPage();

  // PlayerEntityLegacyScriptEngine.cpp, the categories Scripting: defined in
  // that file.
  virtual void setScriptTarget(::ShipEntity* ship);
  // The player its own script target, through the virtual member
  // (OOPlayerForScripting()): not virtual, so the binding's narrow test stands
  // in for it.
  void setScriptTargetToSelf();
  ::ShipEntity* scriptTarget();
  std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>
  getWorldScriptsRequiringTickle();
  void checkScript();
  void runScriptActions(const oo::PList& actions,
                        const std::optional<std::string>& contextName,
                        ::ShipEntity* target);
  virtual void runUnsanitizedScriptActions(
      const oo::PList& actions, bool allowAIMethods,
      const std::optional<std::string>& contextName, ::ShipEntity* target);
  bool scriptTestConditions(const oo::PList& array);
  bool scriptTestCondition(const oo::PList& scriptCondition);
  std::optional<std::string> expandScriptRightHandSide(
      const oo::PList& rhsComponents);
  oo::PList missionVariables();
  oo::PList missionVariableForKey(const std::string& key);
  void setMissionVariable(const oo::PList& value, const std::string& key);
  oo::PList localVariablesForMission(
      const std::optional<std::string>& missionKey);
  std::optional<std::string> localVariableForKey(
      const std::string& variableName,
      const std::optional<std::string>& missionKey);
  void setLocalVariable(const std::optional<std::string>& value,
                        const std::string& variableName,
                        const std::optional<std::string>& missionKey);
  oo::PList missionsList();
  std::optional<std::string> replaceVariablesInString(const std::string& args);
  void setMissionDescription(const std::string& textKey);
  void setMissionDescription(const std::string& textKey,
                             const std::optional<std::string>& key);
  void setMissionInstructions(const std::string& text,
                              const std::optional<std::string>& key);
  void setMissionInstructionsList(const oo::PList& list,
                                  const std::optional<std::string>& key);
  void clearMissionDescription();
  void clearMissionDescriptionForMission(const std::string& key);
  oo::PList mission_string();
  oo::PList status_string();
  oo::PList gui_screen_string();
  oo::PList getGalaxy_number();
  oo::PList planet_number();
  oo::PList score_number();
  oo::PList credits_number();
  oo::PList scriptTimer_number();
  oo::PList shipsFound_number();
  oo::PList commanderLegalStatus_number();
  void setLegalStatus(const std::string& valueString);
  oo::PList commanderLegalStatus_string();
  oo::PList d100_number();
  oo::PList pseudoFixedD100_number();
  oo::PList d256_number();
  oo::PList pseudoFixedD256_number();
  oo::PList clock_number();
  oo::PList clock_secs_number();
  oo::PList clock_mins_number();
  oo::PList clock_hours_number();
  oo::PList clock_days_number();
  oo::PList fuelLevel_number();
  oo::PList dockedAtMainStation_bool();
  oo::PList foundEquipment_bool();
  oo::PList sunWillGoNova_bool();
  oo::PList sunGoneNova_bool();
  oo::PList missionChoice_string();
  oo::PList missionKeyPress_string();
  oo::PList dockedTechLevel_number();
  oo::PList dockedStationName_string();
  oo::PList systemGovernment_string();
  virtual oo::PList systemGovernment_number();
  oo::PList systemEconomy_string();
  virtual oo::PList systemEconomy_number();
  virtual oo::PList systemTechLevel_number();
  virtual oo::PList systemPopulation_number();
  virtual oo::PList systemProductivity_number();
  oo::PList commanderName_string();
  oo::PList commanderRank_string();
  oo::PList commanderShip_string();
  oo::PList commanderShipDisplayName_string();
  std::optional<std::string> expandMessage(const std::string& valueString);
  using ShipEntity::commsMessage;
  void commsMessage(const std::string& valueString) override;
  void commsMessageByUnpiloted(const std::string& valueString) override;
  void consoleMessage3s(const std::string& valueString);
  void consoleMessage6s(const std::string& valueString);
  void awardCredits(const std::string& valueString);
  void awardShipKills(const std::string& valueString);
  void awardEquipment(const std::string& equipString);
  void removeEquipment(const std::string& equipString);
  void setPlanetinfo(const std::string& key_valueString);
  void setSpecificPlanetInfo(const std::string& key_valueString);
  void awardCargo(const std::string& amount_typeString);
  virtual void removeAllCargo();
  void removeAllCargo(bool forceRemoval);
  virtual void useSpecialCargo(const std::string& descriptionString);
  void testForEquipment(const std::string& equipString);
  void awardFuel(const std::string& valueString);
  void messageShipAIs(const std::string& roles_message);
  void ejectItem(const std::string& itemKey);
  void addShips(const std::string& roles_number);
  void addSystemShips(const std::string& roles_number_position);
  virtual void addShipsAt(const std::string& roles_number_system_x_y_z);
  virtual void addShipsAtPrecisely(
      const std::string& roles_number_system_x_y_z);
  virtual void addShipsWithinRadius(
      const std::string& roles_number_system_x_y_z_r);
  void spawnShip(const std::string& ship_key);
  void set(const std::string& missionvariable_value);
  void reset(const std::string& missionvariable);
  void increment(const std::string& missionVariableObject);
  void decrement(const std::string& missionVariableObject);
  void add(const std::string& missionVariableString_value);
  void subtract(const std::string& missionVariableString_value);
  void checkForShips(const std::string& roleString);
  void resetScriptTimer();
  void addMissionText(const std::string& textKey);
  void addLiteralMissionText(const std::string& text);
  void setMissionChoiceByTextEntry(bool enable);
  void setMissionChoices(const std::string& choicesKey);
  void setMissionChoicesDictionary(const oo::PList& choicesDict);
  void resetMissionChoice();
  void clearMissionScreen();
  void addMissionDestination(const std::string& destinations);
  void removeMissionDestination(const std::string& destinations);
  void showShipModel(const std::string& role);
  void setMissionMusic(const std::string& value);
  std::optional<std::string> missionTitle();
  void setMissionTitle(const std::optional<std::string>& value);
  void setMissionImage(const std::string& value);
  void setMissionBackground(const std::string& value);
  void setFuelLeak(const std::string& value);
  oo::PList fuelLeakRate_number();
  void setSunNovaIn(const std::string& time_value);
  virtual void launchFromStation();
  void blowUpStation();
  virtual void sendAllShipsAway();
  void addPlanet(const std::string& planetKey);
  virtual ::oo::PlanetEntity* addPlanetEntity(const std::string& planetKey);
  void addMoon(const std::string& moonKey);
  virtual ::oo::PlanetEntity* addMoonEntity(const std::string& moonKey);
  void debugOn();
  void debugOff();
  void debugMessage(const std::string& args);
  void playSound(const std::string& soundName);
  void doMissionCallback();
  void clearMissionScreenID();
  void setMissionScreenID(const std::optional<std::string>& msid);
  std::optional<std::string> missionScreenID();
  void endMissionScreenAndNoteOpportunity();
  void setGuiToMissionScreen();
  void refreshMissionScreenTextEntry();
  void setGuiToMissionScreenWithCallback(bool callback);
  void setBackgroundFromDescriptionsKey(const std::string& d_key);
  void addScene(const oo::PList& items, Vector off);
  bool processSceneDictionary(const oo::PList& couplet, Vector off);
  bool processSceneString(const std::string& item, Vector off);
  bool addEqScriptForKey(const std::string& eq_key);
  void removeEqScriptForKey(const std::string& eq_key);
  std::size_t eqScriptIndexForKey(const std::string& eq_key);
  void targetNearestHostile();
  void targetNearestIncomingMissile();
  void setGalacticHyperspaceBehaviourTo(
      const std::string& galacticHyperspaceBehaviourString);
  void setGalacticHyperspaceFixedCoordsTo(
      const std::string& galacticHyperspaceFixedCoordsString);

  // PlayerEntityContracts.cpp, the categories Contracts: defined in that file.
  std::optional<std::string> processEscapePods();
  std::optional<std::string> checkPassengerContracts();
  oo::CargoQuantity contractedVolumeForGood(const std::string& good);
  void addMessageToReport(const std::string& report);
  oo::PList getReputation();
  int passengerReputation();
  void increasePassengerReputation(unsigned amount);
  void decreasePassengerReputation(unsigned amount);
  int parcelReputation();
  void increaseParcelReputation(unsigned amount);
  void decreaseParcelReputation(unsigned amount);
  int contractReputation();
  void increaseContractReputation(unsigned amount);
  void decreaseContractReputation(unsigned amount);
  void erodeReputation();
  void normaliseReputation();
  virtual bool addPassenger(const std::string& Name, unsigned start,
                            unsigned Destination, double eta, double fee,
                            double advance, unsigned risk);
  virtual bool removePassenger(const std::string& Name);
  virtual bool addParcel(const std::string& Name, unsigned start,
                         unsigned Destination, double eta, double fee,
                         double premium, unsigned risk);
  virtual bool removeParcel(const std::string& Name);
  virtual bool awardContract(unsigned qty, const std::string& type,
                             unsigned start, unsigned Destination, double eta,
                             double fee, double premium);
  virtual bool removeContract(const std::string& type, unsigned dest);
  std::vector<std::string> passengerList();
  std::vector<std::string> parcelList();
  std::vector<std::string> contractList();
  std::vector<std::string> contractsListFromEntries(
      const oo::PList::Array& contracts_array, bool forCargo, bool forParcels);
  void setGuiToManifestScreen();
  void setManifestScreenRow(const oo::PList& object, ::oo::Color* color,
                            oo::GUIRow row, oo::GUIRow max_rows,
                            oo::GUIRow offset, bool multi);
  void setGuiToDockingReportScreen();
  oo::CreditsQuantity priceForShipKey(const std::string& key);
  void setGuiToShipyardScreen(std::size_t skip);
  void showShipyardInfoForSelection();
  void showTradeInInformationFooter();
  void showShipyardModel(const std::string& shipKey, const oo::PList& shipData,
                         uint16_t personality);
  std::ptrdiff_t missingSubEntitiesAdjustment();
  oo::CreditsQuantity tradeInValue();
  bool buySelectedShip();
  bool replaceShipWithNamedShip(const std::string& shipKey);
  void newShipCommonSetup(const std::string& shipKey,
                          const oo::PList& ship_info,
                          const oo::PList& ship_base_dict);

  // PlayerEntityLoadSave.cpp, the categories LoadSave, OOLoadSavePrivate:
  // defined in that file.
  bool loadPlayer();
  void savePlayer();
  void autosavePlayer();
  void quicksavePlayer();
  void setGuiToScenarioScreen(int page);
  void addScenarioModel(const std::string& shipKey);
  void showScenarioDetails();
  bool startScenario();
  std::optional<std::string> commanderSelector();
  void saveCommanderInputHandler();
  void overwriteCommanderInputHandler();
  bool loadPlayerFromFile(const std::string& fileToOpen, bool asNew);
  void writePlayerToPath(const std::string& path);
  void nativeSavePlayer(const std::string& cdrName);
  void setGuiToLoadCommanderScreen();
  void setGuiToSaveCommanderScreen(const std::string& cdrName);
  void setGuiToOverwriteScreen(const std::string& cdrName);
  void lsCommanders(::GuiDisplayGen* gui, const std::string& directory,
                    int page, const std::optional<std::string>& highlightName);
  bool existingNativeSave(const std::string& cdrName);
  void showCommanderShip(int cdrArrayIndex);
  int findIndexOfCommander(const std::string& cdrName);

#ifdef OO_DEBUG_MONITOR_ENABLED
  // Built only into builds with the debug monitor.
  bool suppressClangStuff() const;
#endif

  // test_PlayerEntity reaches the private members through its
  // PlayerEntityTestAccess.
  friend struct PlayerEntityTestAccess;

 private:
  oo::SystemID system_id = {};
  oo::SystemID target_system_id = {};
  oo::SystemID info_system_id = {};
  oo::SystemID previous_system_id = {};

  float occlusion_dial = {};

  oo::SystemID found_system_id = {};
  int ship_trade_in_factor = {};

  std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>
      worldScripts;  // in load order (+loadScripts)
  std::optional<std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>>
      worldScriptsRequiringTickle;  // PlayerEntityLegacyScriptEngine's cache;
                                    // nullopt: not built
  std::map<std::string, std::shared_ptr<OOScript>, std::less<>>
      commodityScripts;         // by script file name
  oo::PList mission_variables;  // a Dict (saved as mission_variables); null
                                // before set-up
  std::map<std::string, oo::PList, std::less<>>
      localVariables;  // mission key -> that mission's variables (a Dict)
  std::optional<std::string>
      _missionTitle;  // nullopt: the mission screen falls back on its default
  std::ptrdiff_t /*OOGUIRow*/ missionTextRow = {};
  std::optional<std::string> missionChoice;
  std::optional<std::string> missionKeyPress;
  bool _missionWithCallback = {};
  bool _missionAllowInterrupt = {};
  bool _missionTextEntry = {};
  oo::GUIScreenID _missionExitScreen = {};

  std::optional<std::string> specialCargo;

  std::vector<std::string> commLog;  // trimmed by -commLog

  std::vector<std::pair<std::string, std::shared_ptr<OOScript>>>
      eqScripts;  // (key, script), in insertion order

  oo::PList _missionOverlayDescriptor;  // null = none (was nil)
  oo::PList _missionBackgroundDescriptor;
  oo::GUIBackgroundSpecial _missionBackgroundSpecial = {};
  oo::PList _equipScreenBackgroundDescriptor;
  std::optional<std::string> _missionScreenID;

  bool found_equipment = {};

  oo::PList::Dict
      reputation;  // signed integers by key (PlayerEntity (Contracts))

  unsigned max_passengers = {};
  oo::PList::Array passengers;       // Dicts (PlayerEntity (Contracts))
  oo::PList::Dict passenger_record;  // arrival time (double) by passenger name

  oo::PList::Array parcels;       // Dicts (PlayerEntity (Contracts))
  oo::PList::Dict parcel_record;  // arrival time (double) by sender name

  oo::PList::Array
      contracts;  // cargo contract Dicts (PlayerEntity (Contracts))
  oo::PList::Dict contract_record;  // arrival time (double) by cargo ID

  oo::PList::Dict
      shipyard_record;  // shipdata key by shipyard ID of each ship bought

  std::map<std::string, oo::PList, std::less<>>
      missionDestinations;  // validated marker Dicts by -markerKey:
  std::vector<std::string> roleWeights;
  // temporary flags for role actions taking multiple steps, cleared on jump
  // (signed integers)
  oo::PList::Dict roleWeightFlags;
  std::vector<oo::SystemID> roleSystemList;  // list of recently visited sysids

  double script_time = {};
  double script_time_check = {};
  double script_time_interval = {};
  std::optional<std::string> lastTextKey;  // nullopt: none

  double ship_clock = {};
  double ship_clock_adjust = {};

  double escape_pod_rescue_time = {};

  double fps_check_time = {};
  int fps_counter = {};
  double last_fps_check_time = {};

  std::optional<std::string>
      planetSearchString;  // the lower-cased typed prefix; nullopt: no search

  oo::Matrix playerRotMatrix = {};

  bool showingLongRangeChart = {};

  // For OO-GUI based save screen
  std::string commanderNameString;  // owned; the save screen refreshes it from
                                    // the typed string each frame
  std::vector<oo::PList> cdrDetailArray;  // the load/save screen's entries
                                          // (PlayerEntity (LoadSave))
  int currentPage = {};
  bool pollControls = {};
  // ...end save screen

  std::ptrdiff_t marketOffset = {};
  std::optional<std::string> marketSelectedCommodity;
  oo::MarketFilterMode marketFilterMode = {};
  oo::MarketSorterMode marketSorterMode = {};

  std::weak_ptr<Entity>
      _dockedStation;  // a weak reference to the station's C++ part

  /* Used by the DOCKING_CLEARANCE code to implement docking at non-main
   * stations. Could possibly overload use of 'dockedStation' instead
   * but that needs futher investigation to ensure it doesn't break anything. */
  ::StationEntity* targetDockStation = {};

  std::shared_ptr<HeadUpDisplay> hud;
  std::map<std::string, std::string, std::less<>>
      multiFunctionDisplayText;  // MFD key -> text
  std::vector<std::optional<std::string>>
      multiFunctionDisplaySettings;  // one key per MFD; nullopt = inactive (was
                                     // [OONull null])
  std::size_t activeMFD = {};
  oo::PList::Dict
      customDialSettings;  // a mixed configuration: whatever scripts set

  GLfloat roll_delta = {}, pitch_delta = {}, yaw_delta = {};
  GLfloat launchRoll = {};

  GLfloat forward_shield = {}, aft_shield = {};
  GLfloat max_forward_shield = {}, max_aft_shield = {},
          forward_shield_recharge_rate = {}, aft_shield_recharge_rate = {};
  oo::TimeDelta forward_shot_time = {}, aft_shot_time = {}, port_shot_time = {},
                starboard_shot_time = {};

  oo::WeaponFacing chosen_weapon_facing = {};  // for purchasing weapons

  double ecm_start_time = {};
  double last_ecm_time = {};

  oo::GUIScreenID gui_screen = {};
  std::uint16_t alertFlags = {};                // OOAlertFlags bits
  oo::AlertCondition alertConditionLevel = {};  // the ivar alertCondition
  oo::AlertCondition lastScriptAlertCondition = {};
  oo::PlayerFleeingStatus fleeing_status = {};
  oo::MissileStatus missile_status = {};
  std::size_t activeMissile = {};
  std::size_t primedEquipment = {};
  std::optional<std::string> _fastEquipmentA;  // nullopt = never set (was nil)
  std::optional<std::string> _fastEquipmentB;

  oo::CargoQuantity current_cargo = {};

  NSPoint cursor_coordinates = {};
  NSPoint chart_focus_coordinates = {};
  NSPoint chart_centre_coordinates = {};
  NSPoint custom_chart_centre_coordinates = {};
  // where we want the chart centre to be - used for smooth transitions
  NSPoint target_chart_centre = {};
  NSPoint target_chart_focus = {};
  // Chart zoom is 1.0 when fully zoomed in and increases as we zoom out.  The
  // reason I've done it that way round is because we might want to implement
  // bigger galaxies one day, and thus may need to zoom out indefinitely.
  oo::Scalar chart_zoom = {};
  oo::Scalar custom_chart_zoom = {};
  oo::Scalar target_chart_zoom = {};
  oo::Scalar saved_chart_zoom = {};
  oo::RouteType ANA_mode = {};
  oo::TimeDelta witchspaceCountdown = {};

  std::optional<std::string> _jumpCause;

  // player commander data
  std::optional<std::string> _commanderName;
  std::optional<std::string> _lastsaveName;
  NSPoint galaxy_coordinates = {};

  oo::CreditsQuantity credits = {};
  oo::GalaxyID galaxy_number = {};

  std::shared_ptr<oo::CommodityMarket> shipCommodityData;

  oo::EntityHold
      missile_entity[PLAYER_MAX_MISSILES];  // holds the actual missile entities
                                            // or equivalents (held)
  oo::UniversalID _dockTarget = {};         // used by the escape pod code

  int legalStatusValue = {};  // the ivar legalStatus; it both is and isn't an
                              // OOCreditsQuantity, because of quantum.
  int market_rnd = {};
  unsigned ship_kills = {};

  oo::CompassMode compassMode = {};
  std::weak_ptr<Entity>
      compassTarget;  // a weak reference to the target's C++ part

  GLfloat fuel_leak_rate = {};

#if OO_VARIABLE_TORUS_SPEED
  GLfloat hyperspeedFactor = {};
#endif

  // keys!
  oo::PList::Dict keyconfig2_settings;
  std::map<std::string, uint16_t, std::less<>>
      keyCodeLookups;  // lower-case key names -> key codes

  oo::PList n_key_roll_left;
  oo::PList n_key_roll_right;
  oo::PList n_key_pitch_forward;
  oo::PList n_key_pitch_back;
  oo::PList n_key_yaw_left;
  oo::PList n_key_yaw_right;

  oo::PList n_key_view_forward;    // && undock
  oo::PList n_key_view_aft;        // && options menu
  oo::PList n_key_view_port;       // && equipment screen
  oo::PList n_key_view_starboard;  // && interfaces screen

  oo::PList n_key_launch_ship;
  oo::PList n_key_gui_screen_options;
  oo::PList n_key_gui_screen_equipship;
  oo::PList n_key_gui_screen_interfaces;
  oo::PList n_key_gui_screen_status;
  oo::PList n_key_gui_chart_screens;
  oo::PList n_key_gui_system_data;
  oo::PList n_key_gui_market;

  oo::PList n_key_gui_arrow_left;
  oo::PList n_key_gui_arrow_right;
  oo::PList n_key_gui_arrow_up;
  oo::PList n_key_gui_arrow_down;
  oo::PList n_key_gui_page_up;
  oo::PList n_key_gui_page_down;
  oo::PList n_key_gui_select;

  oo::PList n_key_increase_speed;
  oo::PList n_key_decrease_speed;
  oo::PList n_key_inject_fuel;

  oo::PList n_key_fire_lasers;
  oo::PList n_key_launch_missile;
  oo::PList n_key_next_missile;
  oo::PList n_key_ecm;

  oo::PList n_key_prime_next_equipment;
  oo::PList n_key_prime_previous_equipment;
  oo::PList n_key_activate_equipment;
  oo::PList n_key_mode_equipment;
  oo::PList n_key_fastactivate_equipment_a;
  oo::PList n_key_fastactivate_equipment_b;

  oo::PList n_key_target_missile;
  oo::PList n_key_untarget_missile;
  oo::PList n_key_target_incoming_missile;
  oo::PList n_key_ident_system;

  oo::PList n_key_scanner_zoom;
  oo::PList n_key_scanner_unzoom;

  oo::PList n_key_launch_escapepod;

  oo::PList n_key_galactic_hyperspace;
  oo::PList n_key_hyperspace;
  oo::PList n_key_jumpdrive;

  oo::PList n_key_dump_cargo;
  oo::PList n_key_rotate_cargo;

  oo::PList n_key_autopilot;
  oo::PList n_key_autodock;

  oo::PList n_key_snapshot;
  oo::PList n_key_docking_music;

  oo::PList n_key_advanced_nav_array_next;
  oo::PList n_key_advanced_nav_array_previous;
  oo::PList n_key_info_next_system;
  oo::PList n_key_info_previous_system;
  oo::PList n_key_map_home;
  oo::PList n_key_map_end;
  oo::PList n_key_map_next_system;
  oo::PList n_key_map_previous_system;
  oo::PList n_key_map_info;
  oo::PList n_key_map_zoom_in;
  oo::PList n_key_map_zoom_out;

  oo::PList n_key_system_home;
  oo::PList n_key_system_end;
  oo::PList n_key_system_next_system;
  oo::PList n_key_system_previous_system;

  oo::PList n_key_pausebutton;
  oo::PList n_key_show_fps;
  oo::PList n_key_bloom_toggle;
  oo::PList n_key_mouse_control_roll;
  oo::PList n_key_mouse_control_yaw;
  oo::PList n_key_hud_toggle;

  oo::PList n_key_comms_log;
  oo::PList n_key_prev_compass_mode;
  oo::PList n_key_next_compass_mode;

  oo::PList n_key_chart_highlight;
  oo::PList n_key_market_filter_cycle;
  oo::PList n_key_market_sorter_cycle;
  oo::PList n_key_market_buy_one;
  oo::PList n_key_market_sell_one;
  oo::PList n_key_market_buy_max;
  oo::PList n_key_market_sell_max;

  oo::PList n_key_next_target;
  oo::PList n_key_previous_target;

  oo::PList n_key_custom_view;
  oo::PList n_key_custom_view_zoom_out;
  oo::PList n_key_custom_view_zoom_in;
  oo::PList n_key_custom_view_roll_left;
  oo::PList n_key_custom_view_pan_left;
  oo::PList n_key_custom_view_roll_right;
  oo::PList n_key_custom_view_pan_right;
  oo::PList n_key_custom_view_rotate_up;
  oo::PList n_key_custom_view_pan_up;
  oo::PList n_key_custom_view_rotate_down;
  oo::PList n_key_custom_view_pan_down;
  oo::PList n_key_custom_view_rotate_left;
  oo::PList n_key_custom_view_rotate_right;

  oo::PList n_key_docking_clearance_request;
  oo::PList n_key_weapons_online_toggle;

  oo::PList n_key_cycle_next_mfd;
  oo::PList n_key_cycle_previous_mfd;
  oo::PList n_key_switch_next_mfd;
  oo::PList n_key_switch_previous_mfd;

  oo::PList n_key_oxzmanager_setfilter;
  oo::PList n_key_oxzmanager_showinfo;
  oo::PList n_key_oxzmanager_copyurl;
  oo::PList n_key_oxzmanager_extract;

#if OO_FOV_INFLIGHT_CONTROL_ENABLED
  oo::PList n_key_inc_field_of_view;
  oo::PList n_key_dec_field_of_view;
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
  oo::PList n_key_dump_target_state;
  oo::PList n_key_dump_entity_list;
  oo::PList n_key_debug_full;
  oo::PList n_key_debug_collision;
  oo::PList n_key_debug_console_connect;
  oo::PList n_key_debug_bounding_boxes;
  oo::PList n_key_debug_shaders;
  oo::PList n_key_debug_off;
#endif

  // dict to hold custom key config for OXP equipment with activate/mode
  // functions
  std::vector<oo::PList>
      customEquipActivation;                // Dict entries, edited in place by
                                            // KeyMapper/StickMapper/Controls
  std::vector<bool> customActivatePressed;  // parallel to customEquipActivation
  std::vector<bool> customModePressed;

  // dict to hold extra keys for missions screen.
  std::map<std::string, oo::PList, std::less<>>
      extraMissionKeys;  // key name -> processed key definitions

  std::map<oo::GUIScreenID,
           std::vector<std::shared_ptr<::OOJSGuiScreenKeyDefinition>>>
      extraGuiScreenKeys;  // by GUI screen ID

  // save-file
  std::optional<std::string> save_path;
  std::optional<std::string> scenarioKey;

  // position of viewports
  Vector forwardViewOffset = {}, aftViewOffset = {}, portViewOffset = {},
         starboardViewOffset = {};
  Vector _sysInfoLight = {};

  // trumbles
  std::size_t trumbleCount = {};
  std::shared_ptr<oo::Trumble> trumble[PLAYER_MAX_TRUMBLES];
  float _trumbleAppetiteAccumulator = {};

  // smart zoom
  GLfloat scanner_zoom_rate = {};

  // target memory
  // TODO: this should use weakrefs
  std::vector<std::weak_ptr<Entity>>
      target_memory;  // an empty ref = an empty slot (was [OONull null])
  std::size_t target_memory_index = {};

  // custom view points
  Quaternion customViewQuaternion = {};
  oo::Matrix customViewMatrix = {};
  Vector customViewOffset = {}, customViewForwardVector = {},
         customViewUpVector = {}, customViewRightVector = {},
         customViewRotationCenter = {};
  std::optional<std::string> customViewDescription;

  // docking reports
  std::string dockingReport;

  // Woo, flags.
  unsigned suppressTargetLostFlag
      : 1 = 0,  // the ivar suppressTargetLost: smart target lst reports
      scoopsActive : 1 = 0,  // smart fuelscoops

      scoopOverride : 1 = 0,  // scripted to just be on, ignoring normal rules
      game_over : 1 = 0, finished : 1 = 0, bomb_detonated : 1 = 0,
        autopilot_engaged : 1 = 0,

        afterburner_engaged : 1 = 0, afterburnerSoundLooping : 1 = 0,

        hyperspeed_engaged : 1 = 0, travelling_at_hyperspeed : 1 = 0,
        hyperspeed_locked : 1 = 0,

        ident_engaged : 1 = 0,

        galactic_witchjump : 1 = 0,

        ecm_in_operation : 1 = 0,

        show_info_flag : 1 = 0,

        showDemoShips : 1 = 0,

        rolling = {}, pitching = {}, yawing : 1 = 0, using_mining_laser : 1 = 0,

        mouse_control_on : 1 = 0,

        keyboardRollOverride : 1 = 0,  // Handle keyboard roll...
      keyboardPitchOverride : 1 =
          0,  // ...and pitch override separately - (fix for BUG #17490)
      keyboardYawOverride : 1 = 0, waitingForStickCallback : 1 = 0,

        weapons_online : 1 = 0,

        launchingMissile : 1 = 0, replacingMissile : 1 = 0,

        massLockable : 1 = 0;
#if OOLITE_ESPEAK
  unsigned int voice_no = {};
  bool voice_gender_m = {};
#endif
  oo::SpeechSettings isSpeechOn = {};

  // For PlayerEntity (StickMapper)
  int selFunctionIdx = {};
  std::vector<oo::PList> stickFunctions;  // PlayerEntity (StickMapper)'s
                                          // function list; empty until built
  std::vector<oo::PList> keyFunctions;    // PlayerEntity (KeyMapper)'s function
                                          // list; empty until built
  std::vector<oo::PList> kbdLayouts;      // PlayerEntity (KeyMapper)'s keyboard
                                          // layouts; empty until built
  std::string keyShiftText;
  std::string keyMod1Text;
  std::string keyMod2Text;

  oo::GalacticHyperspaceBehaviour galacticHyperspaceBehaviour = {};
  NSPoint galacticHyperspaceFixedCoords = {};

  oo::LongRangeChartMode longRangeChartMode = {};

  std::vector<oo::PList> _customViews;  // the ship's custom view Dicts
  std::size_t _customViewIndex = {};

  oo::DockingClearanceStatus dockingClearanceStatus = {};

  std::vector<oo::EntityHold> scannedWormholes;  // their objects held
  WormholeEntity* wormhole = {};                 // wormholeObject holds it
  oo::EntityHold wormholeObject;                 // the wormhole, held

  oo::EntityHold
      demoShip;  // Used while docked to maintain demo ship rotation. (held)
  std::vector<std::shared_ptr<oo::LaserShotEntity>>
      lastShot;  // used to correctly position laser
                 // shots on first frame of firing

  std::shared_ptr<::StickProfileScreen> stickProfileScreen;

  double maxFieldOfView = {};
  double fieldOfView = {};
#if OO_FOV_INFLIGHT_CONTROL_ENABLED
  double fov_delta = {};
#endif
};

/*	Use PLAYER to refer to the shared player object in cases where it is
        assumed to exist (i.e., except during early initialization).
*/
// The shared player, defined in PlayerEntity.cpp; read through oo::GetPlayer().
extern PlayerEntity* gOOPlayer;

namespace oo {

inline PlayerEntity* GetPlayer() INLINE_CONST_FUNC;

inline PlayerEntity* GetPlayer() {
#if OO_DEBUG
  OOCAssert(
      ::gOOPlayer != nullptr,
      "PLAYER used when PlayerEntity::sharedPlayer() has not been called.");
#endif
  return ::gOOPlayer;
}

}  // namespace oo

#define PLAYER oo::GetPlayer()

#define KILOGRAMS_PER_POD 1000
inline constexpr auto MAX_KILOGRAMS_IN_SAFE = ((KILOGRAMS_PER_POD / 2) - 1);
#define GRAMS_PER_POD (KILOGRAMS_PER_POD * 1000)
inline constexpr auto MAX_GRAMS_IN_SAFE = ((GRAMS_PER_POD / 2) - 1);

// C++ forms, defined in OOConstToString.cpp: std::string results
// (never nil), const std::string & parameters (nil arrived as "" and matched
// nothing: the defaults).
namespace oo {

std::string StringFromGUIScreenID(oo::GUIScreenID screen);
oo::GUIScreenID GUIScreenIDFromString(const std::string& string);

oo::GalacticHyperspaceBehaviour GalacticHyperspaceBehaviourFromString(
    const std::string& string);
std::string StringFromGalacticHyperspaceBehaviour(
    oo::GalacticHyperspaceBehaviour behaviour);

// Rating and legal-status names from descriptions.plist; nullopt: missing (was
// nil).
std::optional<std::string> DisplayRatingStringFromKillCount(unsigned kills);

}  // namespace oo

std::string KillCountToRatingAndKillString(unsigned kills);

namespace oo {

std::optional<std::string> DisplayStringFromLegalStatus(int legalStatus);

}  // namespace oo
