/*

GuiDisplayGen.h

Class handling interface elements, primarily text, that are not part of the 3D
game world, together with GuiDisplayGen.

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

#include <quickjs.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "EnumFlags.h"
#include "Foundation.h"
#include "FunctionAttributes.h"
#include "Maths.h"
#include "Types.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

inline constexpr int GUI_DEFAULT_COLUMNS = 6;
inline constexpr int GUI_DEFAULT_ROWS = 30;

inline constexpr int GUI_MAX_ROWS = 64;
inline constexpr int GUI_MAX_COLUMNS = 40;
inline constexpr int MAIN_GUI_PIXEL_HEIGHT = 480;
inline constexpr int MAIN_GUI_PIXEL_WIDTH = 480;
#define MAIN_GUI_ROW_HEIGHT 16
inline constexpr int MAIN_GUI_ROW_WIDTH = 16;
inline constexpr int MAIN_GUI_PIXEL_ROW_START = 40;

namespace oo {

enum class GUIAlignment : std::int32_t {
  kLeft,
  kRight,
  kCenter,
};

}  // namespace oo

static_assert(std::to_underlying(oo::GUIAlignment::kLeft) == 0);
static_assert(std::to_underlying(oo::GUIAlignment::kRight) == 1);
static_assert(std::to_underlying(oo::GUIAlignment::kCenter) == 2);

namespace oo {

enum class GUIBackgroundSpecial : std::int32_t {
  kNone,
  kCustom,
  kCustomAnaShortest,
  kCustomAnaQuickest,
  kShort,
  kShortAnaShortest,
  kShortAnaQuickest,
  kLong,
  kLongAnaShortest,
  kLongAnaQuickest,
};

}  // namespace oo

static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kNone) == 0);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kCustom) == 1);
static_assert(
    std::to_underlying(oo::GUIBackgroundSpecial::kCustomAnaShortest) == 2);
static_assert(
    std::to_underlying(oo::GUIBackgroundSpecial::kCustomAnaQuickest) == 3);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kShort) == 4);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kShortAnaShortest) ==
              5);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kShortAnaQuickest) ==
              6);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kLong) == 7);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kLongAnaShortest) ==
              8);
static_assert(std::to_underlying(oo::GUIBackgroundSpecial::kLongAnaQuickest) ==
              9);

inline constexpr std::string_view GUI_KEY_OK = "OK";
inline constexpr std::string_view GUI_KEY_SKIP = "SKIP-ROW";

// globals: the gui-settings.plist keys.
inline constexpr const char* kGuiDefaultTextColor = "default_text_color";
inline constexpr const char* kGuiScreenTitleColor = "screen_title_color";
inline constexpr const char* kGuiScreenDividerColor = "screen_divider_color";
inline constexpr const char* kGuiSelectedRowBackgroundColor =
    "selected_row_background_color";
inline constexpr const char* kGuiSelectedRowColor = "selected_row_color";
inline constexpr const char* kGuiTextInputCursorColor =
    "text_input_cursor_color";
// F3
inline constexpr const char* kGuiEquipmentCashColor = "equipment_cash_color";
inline constexpr const char* kGuiEquipmentUnavailableColor =
    "equipment_unavailable_color";
inline constexpr const char* kGuiEquipmentScrollColor =
    "equipment_scroll_color";
inline constexpr const char* kGuiEquipmentOptionColor =
    "equipment_option_color";
inline constexpr const char* kGuiEquipmentRepairColor =
    "equipment_repair_color";
inline constexpr const char* kGuiEquipmentDescriptionColor =
    "equipment_description_color";
inline constexpr const char* kGuiEquipmentLaserColor = "equipment_laser_color";
inline constexpr const char* kGuiEquipmentLaserFittedColor =
    "equipment_laser_fitted_color";
inline constexpr const char* kGuiEquipmentTabs = "equipment_tabs";
// F3 F3
inline constexpr const char* kGuiShipyardHeadingColor =
    "shipyard_heading_color";
inline constexpr const char* kGuiShipyardScrollColor = "shipyard_scroll_color";
inline constexpr const char* kGuiShipyardEntryColor = "shipyard_entry_color";
inline constexpr const char* kGuiShipyardNoshipColor = "shipyard_noship_color";
inline constexpr const char* kGuiShipyardTradeinColor =
    "shipyard_tradein_color";
inline constexpr const char* kGuiShipyardDescriptionColor =
    "shipyard_description_color";
inline constexpr const char* kGuiShipyardTabs = "shipyard_tabs";
// F4
inline constexpr const char* kGuiInterfaceHeadingColor =
    "interface_heading_color";
inline constexpr const char* kGuiInterfaceScrollColor =
    "interface_scroll_color";
inline constexpr const char* kGuiInterfaceEntryColor = "interface_entry_color";
inline constexpr const char* kGuiInterfaceDescriptionColor =
    "interface_description_color";
inline constexpr const char* kGuiInterfaceNoneColor = "interface_none_color";
inline constexpr const char* kGuiInterfaceTabs = "interface_tabs";
// F5
inline constexpr const char* kGuiStatusShipnameColor = "status_shipname_color";
inline constexpr const char* kGuiStatusDataColor = "status_data_color";
inline constexpr const char* kGuiStatusEquipmentHeadingColor =
    "status_equipment_heading_color";
inline constexpr const char* kGuiStatusEquipmentScrollColor =
    "status_equipment_scroll_color";
inline constexpr const char* kGuiStatusEquipmentOkColor =
    "status_equipment_ok_color";
inline constexpr const char* kGuiStatusEquipmentDamagedColor =
    "status_equipment_damaged_color";
inline constexpr const char* kGuiStatusTabs = "status_tabs";
inline constexpr const char* kGuiStatusPrioritiseDamaged =
    "status_prioritise_damaged";
// F5 F5
inline constexpr const char* kGuiManifestSubheadColor =
    "manifest_subhead_color";
inline constexpr const char* kGuiManifestEntryColor = "manifest_entry_color";
inline constexpr const char* kGuiManifestScrollColor = "manifest_scroll_color";
inline constexpr const char* kGuiManifestNoScrollColor =
    "manifest_no_scroll_color";
inline constexpr const char* kGuiManifestTabs = "manifest_tabs";
// F6
inline constexpr const char* kGuiChartLabelScale = "chart_label_scale";
inline constexpr const char* kGuiChartCircleScale = "chart_circle_scale";
inline constexpr const char* kGuiChartLabelColor = "chart_label_color";
inline constexpr const char* kGuiChartLabelReachableColor =
    "chart_labelreachable_color";
inline constexpr const char* kGuiChartRangeColor = "chart_range_color";
inline constexpr const char* kGuiChartCrosshairColor = "chart_crosshair_color";
inline constexpr const char* kGuiChartCursorColor = "chart_cursor_color";
inline constexpr const char* kGuiChartInfoMarkerColor =
    "chart_info_marker_color";
inline constexpr const char* kGuiChartMatchBoxColor = "chart_match_box_color";
inline constexpr const char* kGuiChartMatchLabelColor =
    "chart_match_label_color";
inline constexpr const char* kGuiChartConnectionColor =
    "chart_connection_color";
inline constexpr const char* kGuiChartCurrentJumpStartColor =
    "chart_currentjumpstart_color";
inline constexpr const char* kGuiChartCurrentJumpEndColor =
    "chart_currentjumpend_color";
inline constexpr const char* kGuiChartRouteShortColor =
    "chart_route_short_color";
inline constexpr const char* kGuiChartRouteQuickColor =
    "chart_route_quick_color";
inline constexpr const char* kGuiChartTraveltimeTabs = "chart_traveltime_tabs";

inline constexpr const char* kGuiChartEconomyUColor = "chart_economy_%zu_color";
inline constexpr const char* kGuiChartGovernmentUColor =
    "chart_government_%zu_color";
inline constexpr const char* kGuiChartTechColor = "chart_tech_color";
// F7
inline constexpr const char* kGuiSystemdataFactsColor =
    "systemdata_facts_color";
inline constexpr const char* kGuiSystemdataDescriptionColor =
    "systemdata_description_color";
inline constexpr const char* kGuiSystemdataTabs = "systemdata_tabs";
// F8
inline constexpr const char* kGuiMarketHeadingColor = "market_heading_color";
inline constexpr const char* kGuiMarketCommodityColor =
    "market_commodity_color";
inline constexpr const char* kGuiMarketScrollColor = "market_scroll_color";
inline constexpr const char* kGuiMarketFilteredAllColor =
    "market_filtered_all_color";
inline constexpr const char* kGuiMarketFilterInfoColor =
    "market_filter_info_color";
inline constexpr const char* kGuiMarketCashColor = "market_cash_color";
// F8 F8 extras
inline constexpr const char* kGuiMarketContractedColor =
    "market_contracted_color";
inline constexpr const char* kGuiMarketDescriptionColor =
    "market_description_color";
inline constexpr const char* kGuiMarketTabs = "market_tabs";
// Docking report
inline constexpr const char* kGuiDockingReportColor = "docking_report_color";
inline constexpr const char* kGuiDockingSummaryColor = "docking_summary_color";
inline constexpr const char* kGuiDockingContinueColor =
    "docking_continue_color";

namespace oo {

class Texture;

}  // namespace oo

namespace oo {

class Color;

}  // namespace oo

class HeadUpDisplay;

namespace oo {

class Sound;

class TextureSprite;  // C++ (OOTextureSprite.h)

using GUIRow = std::ptrdiff_t;      // -1 for none
using GUITabStop = std::ptrdiff_t;  // negative value = right align text
using GUITabSettings = GUITabStop[GUI_MAX_COLUMNS];

}  // namespace oo

/*	The members marked virtual are the ones the whole-game unit tests
   (test_OOOXZManager, test_OOJSPlayerShip) stand in for with C++ subclasses
   that override them. The game makes no subclass.
*/
class GuiDisplayGen : public oo::Object {
 public:
  GuiDisplayGen();  // -init
  /*	Titles, row texts and row keys are UTF-8 std::strings, std::optional
     where the old code accepted or returned nil.
  */
  GuiDisplayGen(
      NSSize gui_size, int gui_cols, int gui_rows, int gui_row_height,
      int gui_row_start,
      const std::optional<std::string>&
          gui_title);  // -initWithPixelSize:columns:rows:rowHeight:rowStart:title:
  ~GuiDisplayGen() override;

  void resizeWithPixelSize(NSSize gui_size, int gui_cols, int gui_rows,
                           int gui_row_height, int gui_row_start,
                           const std::optional<std::string>& gui_title);
  void resizeTo(NSSize gui_size, int csize,
                const std::optional<std::string>& gui_title);
  NSSize size();
  unsigned columns();
  unsigned rows();
  unsigned rowHeight();
  int rowStart();

  std::optional<std::string> getTitle();  // nullopt: no title
  virtual void setTitle(
      const std::optional<std::string>& str);  // empty string means no title

  void setDrawPosition(Vector vector);
  Vector getDrawPosition();

  oo::PList userSettings();  // gui-settings.plist, with any colours set by
                             // setGuiColorSettingFromKey()

  void fadeOutFromTime(oo::TimeAbsolute now_time, oo::TimeDelta duration);
  void stopFadeOuts();

  GLfloat alpha();
  void setAlpha(GLfloat an_alpha);
  void setMaxAlpha(GLfloat an_alpha);

  void setBackgroundColor(oo::Color* color);

  virtual oo::Color* getTextColor();
  virtual void setTextColor(oo::Color* color);
  virtual oo::Color* getTextCommsColor();
  virtual void setTextCommsColor(oo::Color* color);
  std::shared_ptr<oo::Color> colorFromSetting(
      const std::optional<std::string>& setting, oo::Color* def);
  void setGLColorFromSetting(const std::optional<std::string>& setting,
                             oo::Color* def, GLfloat alpha);
  void setGuiColorSettingFromKey(const std::string& key, oo::Color* col);

  void setCharacterSize(NSSize character_size);

  void setShowAdvancedNavArray(bool inFlag);

  virtual void setColor(oo::Color* color, oo::GUIRow row);

  oo::PList objectForRow(oo::GUIRow row);  // a string, or an array of column
                                           // strings; null out of range
  std::optional<std::string> keyForRow(oo::GUIRow row);
  oo::GUIRow rowForKey(const std::optional<std::string>& key);
  virtual oo::GUIRow getSelectedRow();
  virtual bool setSelectedRow(oo::GUIRow row);
  bool setNextRow(int direction);
  bool setFirstSelectableRow();
  bool setLastSelectableRow();
  void setNoSelectedRow();
  std::optional<std::string> selectedRowText();
  std::optional<std::string> selectedRowKey();
  void reportSelectedRow(int row);

  void setShowTextCursor(bool yesno);
  void setCurrentRow(oo::GUIRow value);

  NSRange getSelectableRange();
  virtual void setSelectableRange(NSRange range);

  virtual void setTabStops(oo::GUITabSettings stops);
  void overrideTabs(oo::GUITabSettings stops, const std::string& setting,
                    std::size_t len);

  void clear();
  virtual void clearAndKeepBackground(bool keepBackground);

  virtual void setKey(const std::string& str, oo::GUIRow row);
  virtual void setText(const std::string& str, oo::GUIRow row);
  virtual void setText(const std::optional<std::string>& str, oo::GUIRow row,
                       oo::GUIAlignment alignment);  // nullopt: no change

  // A nil text or key is std::nullopt (nothing printed / no key set, as
  // before); text_array, when not nullptr, receives each line printed.
  virtual std::optional<std::string> reflowTextForMFD(
      const std::optional<std::string>& input);
  virtual oo::GUIRow addLongText(const std::optional<std::string>& str,
                                 oo::GUIRow row, oo::GUIAlignment alignment);
  void printLongText(const std::optional<std::string>& str,
                     oo::GUIAlignment alignment, oo::Color* text_color,
                     float text_fade,
                     const std::optional<std::string>& text_key,
                     std::vector<std::string>* text_array);
  void printLineNoScroll(const std::optional<std::string>& str,
                         oo::GUIAlignment alignment, oo::Color* text_color,
                         float text_fade,
                         const std::optional<std::string>& text_key,
                         std::vector<std::string>* text_array);

  virtual void setArray(const std::vector<std::string>& arr,
                        oo::GUIRow row);  // one string per column

  // items: an array of row texts (a string, or an array of column strings);
  // item_keys: null or an array of the same length.
  void insertItemsFromArray(const oo::PList& items, const oo::PList& item_keys,
                            oo::GUIRow row, oo::Color* text_color);

  void scrollUp(int how_much);

  /* allows the use of special dynamic backgrounds */
  void setBackgroundTextureSpecial(oo::GUIBackgroundSpecial spec,
                                   bool withBackground);

  /*
          A background/foreground texture descriptor is a dictionary with a
     string property keyed "name" and optional number properties keyed "width"
     and "height". Descriptors are oo::PList (null = nil).
  */

  bool setBackgroundTextureDescriptor(const oo::PList& descriptor);
  bool setForegroundTextureDescriptor(const oo::PList& descriptor);
  bool setBackgroundTextureKey(const std::optional<std::string>& key);
  bool setForegroundTextureKey(const std::optional<std::string>& key);

  bool preloadGUITexture(const oo::PList& descriptor);

  /*
          Interpret a JavaScript value as a texture descriptor for
          set{Background|Foreground}TextureDescriptor(). Also starts
          preloading the texture. Null: no such texture.

          callerDescription is a string describing the context in which this was
          called, generally a method name (like "mission.runScreen()") for
     warning generation.

          Requires a request on context.
  */
  oo::PList textureDescriptorFromJSValue(
      JSValue value, JSContext* context,
      const std::optional<std::string>& callerDescription);

  void clearBackground();

  void leaveLastLine();
  oo::PList getLastLines();  // text, colour, fade time (x 2); null with no rows

  // The row under a virtual-joystick (pointer) position: the row
  // -drawGUI:drawCursor:YES returns for that position, without rendering. A
  // click uses it so that it activates the row under the pointer NOW, not the
  // row of the last render.
  int rowAtVirtualJoystickPosition(NSPoint vjpos);
  void setStatusPage(std::ptrdiff_t pageNum);
  std::size_t getStatusPage();
  void drawEquipmentList(const oo::PList& eqptList, GLfloat z);

  int drawGUI(GLfloat alpha, bool drawCursor);
  void drawGUIBackground();
  void refreshStarChart();
  void setStarChartTitle();

  oo::SystemID targetNextFoundSystem(int direction);

 private:
  void drawSystemMarkers(const oo::PList& markers, GLfloat x, GLfloat y,
                         GLfloat z, GLfloat alpha, GLfloat scale);

  NSSize size_in_pixels = {};
  unsigned n_columns = {};
  unsigned n_rows = {};
  int pixel_row_center = {};
  unsigned pixel_row_height = {};
  int pixel_row_start = {};
  NSSize pixel_text_size = {};

  bool showAdvancedNavArray = {};

  NSSize pixel_title_size = {};

  std::shared_ptr<oo::Color> backgroundColor;
  std::shared_ptr<oo::Color> textColor;
  std::shared_ptr<oo::Color> textCommsColor;

  std::shared_ptr<oo::TextureSprite> backgroundSprite;
  std::shared_ptr<oo::TextureSprite> foregroundSprite;
  oo::GUIBackgroundSpecial backgroundSpecial = {};

  std::optional<std::string> title;  // none: no title bar

  std::vector<oo::PList>
      rowText;  // each a string, or an array of column strings
  std::vector<std::string> rowKey;
  std::vector<std::shared_ptr<oo::Color>> rowColor;

  Vector drawPosition = {};

  NSPoint rowPosition[GUI_MAX_ROWS] = {};
  oo::GUIAlignment rowAlignment[GUI_MAX_ROWS] = {};
  float rowFadeTime[GUI_MAX_ROWS] = {};

  oo::GUITabSettings tabStops = {};

  // a mixed configuration: plist values and OOColors
  oo::PList guiUserSettings;

  NSRange rowRange = {};

  oo::GUIRow selectedRow = {};
  NSRange selectableRange = {};

  bool showTextCursor = {};
  oo::GUIRow currentRow = {};

  GLfloat max_alpha = {};       // main alpha setting
  GLfloat fade_alpha = {};      // for fade-in / fade-out
  GLfloat fade_sign = {};       //	-1.0 to 1.0
  std::size_t statusPage = {};  // status  screen: paging equipped items
  oo::SystemID foundSystem = {};

  void drawGLDisplay(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha);
  void drawCrossHairsWithSize(GLfloat size, GLfloat x, GLfloat y, GLfloat z);
  void drawStarChart(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha,
                     bool compact);
  void drawSystemMarker(const oo::PList& marker, GLfloat x, GLfloat y,
                        GLfloat z, GLfloat alpha, GLfloat scale);
  void drawAdvancedNavArrayAtX(float x, float y, float z, float alpha,
                               const oo::PList& routeInfo,
                               oo::RouteType optimizeBy, oo::Scalar zoom);
};

// One-line bridge for the file-scope helpers of GuiDisplayGen.cpp, which may
// not message the universe themselves. Deleted with the universe's conversion.
bool GuiDisplayGenUniverseUseShaders();
