/*	OOGUIScreenID.h

The values of a GUI screen: JS and callObjC (an int, ValueType.h) see
it. The numbers are the program's ordinals (kMain is 0, which OOJSGlobal reads
as no screen too), not the -1 that OOGUIScreenID.tbl wrote for every screen and
its macro discarded; and the names the game reads and writes for them. It was
OOGUIScreenID.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class GUIScreenID : std::int32_t {
  kMain = 0,
  kIntro1 = 1,
  kShipLibrary = 2,
  kKeyboard = 3,
  kNewGame = 4,
  kOXZManager = 5,
  kStatus = 6,
  kManifest = 7,
  kEquipShip = 8,
  kShipyard = 9,
  kLongRangeChart = 10,
  kShortRangeChart = 11,
  kSystemData = 12,
  kMarket = 13,
  kMarketInfo = 14,
  // kContracts,
  kOptions = 15,
  kGameOptions = 16,
  kLoad = 17,
  kSave = 18,
  kSaveOverwrite = 19,
  kStickMapper = 20,
  kStickProfile = 21,
  kMission = 22,
  kReport = 23,
  kInterfaces = 24,
  kKeyboardConfirmClear = 25,
  kKeyboardConfig = 26,
  kKeyboardEntry = 27,
  kKeyboardLayout = 28,
};

// What an unknown name reads as.
inline constexpr GUIScreenID kGUIScreenIDDefault = GUIScreenID::kMain;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(GUIScreenID value) noexcept {
  switch (value) {
    case GUIScreenID::kMain:
      return "GUI_SCREEN_MAIN";
    case GUIScreenID::kIntro1:
      return "GUI_SCREEN_INTRO1";
    case GUIScreenID::kShipLibrary:
      return "GUI_SCREEN_SHIPLIBRARY";
    case GUIScreenID::kKeyboard:
      return "GUI_SCREEN_KEYBOARD";
    case GUIScreenID::kNewGame:
      return "GUI_SCREEN_NEWGAME";
    case GUIScreenID::kOXZManager:
      return "GUI_SCREEN_OXZMANAGER";
    case GUIScreenID::kStatus:
      return "GUI_SCREEN_STATUS";
    case GUIScreenID::kManifest:
      return "GUI_SCREEN_MANIFEST";
    case GUIScreenID::kEquipShip:
      return "GUI_SCREEN_EQUIP_SHIP";
    case GUIScreenID::kShipyard:
      return "GUI_SCREEN_SHIPYARD";
    case GUIScreenID::kLongRangeChart:
      return "GUI_SCREEN_LONG_RANGE_CHART";
    case GUIScreenID::kShortRangeChart:
      return "GUI_SCREEN_SHORT_RANGE_CHART";
    case GUIScreenID::kSystemData:
      return "GUI_SCREEN_SYSTEM_DATA";
    case GUIScreenID::kMarket:
      return "GUI_SCREEN_MARKET";
    case GUIScreenID::kMarketInfo:
      return "GUI_SCREEN_MARKETINFO";
    // case OOGUIScreenID::kContracts:
    //   return "GUI_SCREEN_CONTRACTS";
    case GUIScreenID::kOptions:
      return "GUI_SCREEN_OPTIONS";
    case GUIScreenID::kGameOptions:
      return "GUI_SCREEN_GAMEOPTIONS";
    case GUIScreenID::kLoad:
      return "GUI_SCREEN_LOAD";
    case GUIScreenID::kSave:
      return "GUI_SCREEN_SAVE";
    case GUIScreenID::kSaveOverwrite:
      return "GUI_SCREEN_SAVE_OVERWRITE";
    case GUIScreenID::kStickMapper:
      return "GUI_SCREEN_STICKMAPPER";
    case GUIScreenID::kStickProfile:
      return "GUI_SCREEN_STICKPROFILE";
    case GUIScreenID::kMission:
      return "GUI_SCREEN_MISSION";
    case GUIScreenID::kReport:
      return "GUI_SCREEN_REPORT";
    case GUIScreenID::kInterfaces:
      return "GUI_SCREEN_INTERFACES";
    case GUIScreenID::kKeyboardConfirmClear:
      return "GUI_SCREEN_KEYBOARD_CONFIRMCLEAR";
    case GUIScreenID::kKeyboardConfig:
      return "GUI_SCREEN_KEYBOARD_CONFIG";
    case GUIScreenID::kKeyboardEntry:
      return "GUI_SCREEN_KEYBOARD_ENTRY";
    case GUIScreenID::kKeyboardLayout:
      return "GUI_SCREEN_KEYBOARD_LAYOUT";
  }
  return {};
}

inline constexpr auto kGUIScreenIDTable = oo::MakeEnumTable<
    GUIScreenID, GUIScreenID::kMain, GUIScreenID::kIntro1,
    GUIScreenID::kShipLibrary, GUIScreenID::kKeyboard, GUIScreenID::kNewGame,
    GUIScreenID::kOXZManager, GUIScreenID::kStatus, GUIScreenID::kManifest,
    GUIScreenID::kEquipShip, GUIScreenID::kShipyard,
    GUIScreenID::kLongRangeChart, GUIScreenID::kShortRangeChart,
    GUIScreenID::kSystemData, GUIScreenID::kMarket, GUIScreenID::kMarketInfo,
    GUIScreenID::kOptions, GUIScreenID::kGameOptions, GUIScreenID::kLoad,
    GUIScreenID::kSave, GUIScreenID::kSaveOverwrite, GUIScreenID::kStickMapper,
    GUIScreenID::kStickProfile, GUIScreenID::kMission, GUIScreenID::kReport,
    GUIScreenID::kInterfaces, GUIScreenID::kKeyboardConfirmClear,
    GUIScreenID::kKeyboardConfig, GUIScreenID::kKeyboardEntry,
    GUIScreenID::kKeyboardLayout>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::GUIScreenID::kMain) == 0);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kIntro1) == 1);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kShipLibrary) == 2);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kKeyboard) == 3);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kNewGame) == 4);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kOXZManager) == 5);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kStatus) == 6);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kManifest) == 7);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kEquipShip) == 8);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kShipyard) == 9);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kLongRangeChart) == 10);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kShortRangeChart) == 11);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kSystemData) == 12);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kMarket) == 13);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kMarketInfo) == 14);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kOptions) == 15);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kGameOptions) == 16);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kLoad) == 17);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kSave) == 18);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kSaveOverwrite) == 19);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kStickMapper) == 20);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kStickProfile) == 21);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kMission) == 22);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kReport) == 23);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kInterfaces) == 24);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kKeyboardConfirmClear) == 25);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kKeyboardConfig) == 26);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kKeyboardEntry) == 27);
static_assert(oo::ToUnderlying(oo::GUIScreenID::kKeyboardLayout) == 28);
