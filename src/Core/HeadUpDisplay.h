/*

HeadUpDisplay.h

Class handling the player ship’s heads-up display, and 2D drawing functions.

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
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "MyOpenGLView.h"
#include "OOFoundation.h"
#include "OOMaths.h"
#include "OOOpenGL.h"
#include "OOPolygonSprite.h"  // OOHUDBeaconIcon
#include "OOTypes.h"
#include "ShipEntity.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/objc/OOObject.h"

// PlayerEntity.h defines it; this header does not include PlayerEntity.h (it
// would close an include cycle).
enum class OOMissileStatus : std::int32_t;

struct OOHUDWidget;  // HeadUpDisplay.cpp

class OOColor;
class OOCrosshairs;

inline constexpr int SCANNER_CENTRE_X = 0;
inline constexpr int SCANNER_CENTRE_Y = -180;
inline constexpr int SCANNER_SCALE = 256;
inline constexpr int SCANNER_WIDTH = 288;
inline constexpr int SCANNER_HEIGHT = 72;

inline constexpr double SCANNER_MAX_ZOOM = 5.0;
inline constexpr int SCANNER_ZOOM_LEVELS = 5;
inline constexpr int ZOOM_INDICATOR_CENTRE_X = 108;
inline constexpr int ZOOM_INDICATOR_CENTRE_Y = -216;
inline constexpr float ZOOM_INDICATOR_WIDTH = 11.0f;
inline constexpr float ZOOM_INDICATOR_HEIGHT = 14.0f;
inline constexpr char ZOOM_LEVELS_IMAGE[] = "zoom.png";

inline constexpr char COMPASS_IMAGE[] = "compass.png";
inline constexpr int COMPASS_CENTRE_X = 132;
inline constexpr int COMPASS_CENTRE_Y = -216;
inline constexpr int COMPASS_SIZE = 56;
inline constexpr int COMPASS_HALF_SIZE = 28;
inline constexpr char COMPASS_REDDOT_IMAGE[] = "reddot.png";
inline constexpr char COMPASS_GREENDOT_IMAGE[] = "greendot.png";
inline constexpr int COMPASS_DOT_SIZE = 16;
inline constexpr int COMPASS_HALF_DOT_SIZE = 8;

inline constexpr char AEGIS_IMAGE[] = "aegis.png";
inline constexpr int AEGIS_CENTRE_X = -132;
inline constexpr int AEGIS_CENTRE_Y = -216;
inline constexpr int AEGIS_WIDTH = 24;
inline constexpr int AEGIS_HEIGHT = 24;

inline constexpr int SPEED_BAR_CENTRE_X = 200;
inline constexpr int SPEED_BAR_CENTRE_Y = -145;
inline constexpr int SPEED_BAR_WIDTH = 80;
inline constexpr int SPEED_BAR_HEIGHT = 8;
#define SPEED_BAR_DRAW_SURROUND true

inline constexpr int ROLL_BAR_CENTRE_X = 200;
inline constexpr int ROLL_BAR_CENTRE_Y = -160;
inline constexpr int ROLL_BAR_WIDTH = 80;
inline constexpr int ROLL_BAR_HEIGHT = 8;
#define ROLL_BAR_DRAW_SURROUND true

inline constexpr int PITCH_BAR_CENTRE_X = 200;
inline constexpr int PITCH_BAR_CENTRE_Y = -170;
inline constexpr int PITCH_BAR_WIDTH = 80;
inline constexpr int PITCH_BAR_HEIGHT = 8;
#define PITCH_BAR_DRAW_SURROUND true

inline constexpr int ENERGY_GAUGE_CENTRE_X = 200;
inline constexpr int ENERGY_GAUGE_CENTRE_Y = -205;
inline constexpr int ENERGY_GAUGE_WIDTH = 80;
inline constexpr int ENERGY_GAUGE_HEIGHT = 48;
#define ENERGY_GAUGE_DRAW_SURROUND true

inline constexpr int FORWARD_SHIELD_BAR_CENTRE_X = -200;
inline constexpr int FORWARD_SHIELD_BAR_CENTRE_Y = -146;
inline constexpr int FORWARD_SHIELD_BAR_WIDTH = 80;
inline constexpr int FORWARD_SHIELD_BAR_HEIGHT = 8;
#define FORWARD_SHIELD_BAR_DRAW_SURROUND true

inline constexpr int AFT_SHIELD_BAR_CENTRE_X = -200;
inline constexpr int AFT_SHIELD_BAR_CENTRE_Y = -162;
inline constexpr int AFT_SHIELD_BAR_WIDTH = 80;
inline constexpr int AFT_SHIELD_BAR_HEIGHT = 8;
#define AFT_SHIELD_BAR_DRAW_SURROUND true

inline constexpr int FUEL_BAR_CENTRE_X = -200;
inline constexpr int FUEL_BAR_CENTRE_Y = -179;
inline constexpr int FUEL_BAR_WIDTH = 80;
inline constexpr int FUEL_BAR_HEIGHT = 8;

inline constexpr int WITCHDEST_CENTRE_X = -200;
inline constexpr int WITCHDEST_CENTRE_Y = -179;
inline constexpr int WITCHDEST_WIDTH = 80;
inline constexpr int WITCHDEST_HEIGHT = 8;

inline constexpr int CABIN_TEMP_BAR_CENTRE_X = -200;
inline constexpr int CABIN_TEMP_BAR_CENTRE_Y = -189;
inline constexpr int CABIN_TEMP_BAR_WIDTH = 80;
inline constexpr int CABIN_TEMP_BAR_HEIGHT = 8;

inline constexpr int WEAPON_TEMP_BAR_CENTRE_X = -200;
inline constexpr int WEAPON_TEMP_BAR_CENTRE_Y = -199;
inline constexpr int WEAPON_TEMP_BAR_WIDTH = 80;
inline constexpr int WEAPON_TEMP_BAR_HEIGHT = 8;

inline constexpr int ALTITUDE_BAR_CENTRE_X = -200;
inline constexpr int ALTITUDE_BAR_CENTRE_Y = -209;
inline constexpr int ALTITUDE_BAR_WIDTH = 80;
inline constexpr int ALTITUDE_BAR_HEIGHT = 8;

inline constexpr int MISSILES_DISPLAY_X = -228;
inline constexpr int MISSILES_DISPLAY_Y = -224;
inline constexpr int MISSILES_DISPLAY_SPACING = 16;
#define MISSILE_ICON_WIDTH 12
#define MISSILE_ICON_HEIGHT MISSILE_ICON_WIDTH

inline constexpr int PRIMED_DISPLAY_X = -144;
inline constexpr int PRIMED_DISPLAY_Y = -256;
inline constexpr int PRIMED_DISPLAY_WIDTH = 12;
inline constexpr int PRIMED_DISPLAY_HEIGHT = 12;

inline constexpr int ASCTARGET_DISPLAY_X = 64;
inline constexpr int ASCTARGET_DISPLAY_Y = -234;
inline constexpr int ASCTARGET_DISPLAY_WIDTH = 10;
inline constexpr int ASCTARGET_DISPLAY_HEIGHT = 10;

inline constexpr int CLOCK_DISPLAY_X = -44;
inline constexpr int CLOCK_DISPLAY_Y = -234;
inline constexpr int CLOCK_DISPLAY_WIDTH = 12;
inline constexpr int CLOCK_DISPLAY_HEIGHT = 12;

inline constexpr int WEAPONSOFFLINETEXT_DISPLAY_X = -175;
inline constexpr int WEAPONSOFFLINETEXT_DISPLAY_Y = 2;
inline constexpr int WEAPONSOFFLINETEXT_WIDTH = 8;
inline constexpr int WEAPONSOFFLINETEXT_HEIGHT = 8;

inline constexpr int FPSINFO_DISPLAY_X = -300;
inline constexpr int FPSINFO_DISPLAY_Y = 220;
inline constexpr int FPSINFO_DISPLAY_WIDTH = 12;
inline constexpr int FPSINFO_DISPLAY_HEIGHT = 12;

inline constexpr int STATUS_LIGHT_CENTRE_X = -108;
inline constexpr int STATUS_LIGHT_CENTRE_Y = -216;
inline constexpr int STATUS_LIGHT_WIDTH = 8;
inline constexpr int STATUS_LIGHT_HEIGHT = 8;

inline constexpr int HIT_INDICATOR_CENTRE_X = 200;
inline constexpr int HIT_INDICATOR_CENTRE_Y = 0;

inline constexpr int SCOOPSTATUS_CENTRE_X = -132;
inline constexpr int SCOOPSTATUS_CENTRE_Y = -152;
inline constexpr double SCOOPSTATUS_WIDTH = 16.0;
inline constexpr double SCOOPSTATUS_HEIGHT = 16.0;

inline constexpr int MFD_TEXT_WIDTH = 10;
inline constexpr int MFD_TEXT_HEIGHT = 10;

inline constexpr char DIALS_KEY[] = "dials";
inline constexpr char LEGENDS_KEY[] = "legends";
inline constexpr char MFDS_KEY[] = "multi_function_displays";
inline constexpr char X_KEY[] = "x";
inline constexpr char Y_KEY[] = "y";
inline constexpr char X_ORIGIN_KEY[] = "x_origin";
inline constexpr char Y_ORIGIN_KEY[] = "y_origin";
inline constexpr char SPACING_KEY[] = "spacing";
inline constexpr char ALPHA_KEY[] = "alpha";
inline constexpr char SELECTOR_KEY[] = "selector";
inline constexpr char IMAGE_KEY[] = "image";
inline constexpr char WIDTH_KEY[] = "width";
inline constexpr char HEIGHT_KEY[] = "height";
inline constexpr char SPRITE_KEY[] = "sprite";
inline constexpr char DRAW_SURROUND_KEY[] = "draw_surround";
inline constexpr char EQUIPMENT_REQUIRED_KEY[] = "equipment_required";
inline constexpr char ALERT_CONDITIONS_KEY[] = "alert_conditions";
inline constexpr char VIEWSCREEN_KEY[] = "viewscreen_only";
inline constexpr char DIAL_REQUIRED_KEY[] = "with_dial";
inline constexpr char LABELLED_KEY[] = "labelled";
inline constexpr char TEXT_KEY[] = "text";
inline constexpr char RGB_COLOR_KEY[] = "rgb_color";
#define COLOR_KEY "color"
#define COLOR_KEY_LOW "color_low"
#define COLOR_KEY_MEDIUM "color_medium"
#define COLOR_KEY_HIGH "color_high"
#define COLOR_KEY_CRITICAL "color_critical"
#define COLOR_KEY_SURROUND "color_surround"
inline constexpr char N_BARS_KEY[] = "n_bars";
inline constexpr char CUSTOM_DIAL_KEY[] = "data_source";

inline constexpr char ROWS_KEY[] = "rows";
inline constexpr char COLUMNS_KEY[] = "columns";
inline constexpr char ROW_HEIGHT_KEY[] = "row_height";
inline constexpr char ROW_START_KEY[] = "row_start";
inline constexpr char TITLE_KEY[] = "title";
inline constexpr char BACKGROUND_RGBA_KEY[] = "background_rgba";
inline constexpr char OVERALL_ALPHA_KEY[] = "overall_alpha";
inline constexpr char NONLINEAR_SCANNER[] = "nonlinear_scanner";

#define Z1                                                   \
  (UNIVERSE != nullptr && UNIVERSE->getGameView() != nullptr \
       ? UNIVERSE->getGameView()->getDisplay_z()             \
       : 0.0f)

inline constexpr double ONE_EIGHTH = 0.125;

inline constexpr int MAX_ACCURACY_RANGE = 7000;  // 7.000km
#define ACCURACY_PROBABILITY_DECREASE_FACTOR \
  0.000035f  // for every 1000km decrease by 3.5% the chance of high accuracy
#define MIN_PROBABILITY_ACCURACY \
  0.35f  // floor value for probability of high accuracy is 35%

inline constexpr int OO_RETICLE_COLOR_TARGET = 0;
inline constexpr int OO_RETICLE_COLOR_TARGET_SENSITIVE =
    OO_RETICLE_COLOR_TARGET + 1;
inline constexpr int OO_RETICLE_COLOR_WORMHOLE =
    OO_RETICLE_COLOR_TARGET_SENSITIVE + 1;

class Entity;
class PlayerEntity;
class GuiDisplayGen;

/*	The HUD itself. The dials are called by name through a table of member
        pointers (HeadUpDisplay.cpp, kDials). Getters named after their state
   are get + the name.
*/
class HeadUpDisplay : public oo::Object {
 public:
  HeadUpDisplay();
  ~HeadUpDisplay() override;

  // -initWithDictionary:inFile: (callers make the HUD, then run this).
  void initWithDictionary(const oo::PList& hudinfo,
                          const std::optional<std::string>& hudFileName);

  void resetGuis(const oo::PList& info);

  std::optional<std::string> getHudName();
  void setHudName(const std::optional<std::string>&
                      newHudName);  // nullopt is ignored, as nil was

  GLfloat scannerZoom();
  void setScannerZoom(GLfloat value);

  GLfloat getOverallAlpha();
  void setOverallAlpha(GLfloat newAlphaValue);

  bool getReticleTargetSensitive();
  void setReticleTargetSensitive(bool newReticleTargetSensitiveValue);
  oo::PList* getPropertiesReticleTargetSensitive();  // the live dictionary

  bool isHidden();
  void setHidden(bool newValue);

  bool getAllowBigGui();

  bool hasHidden(const std::optional<std::string>&
                     selectorName);  // nullopt (was nil): false
  void setHiddenSelector(const std::string& selectorName, bool hide);
  void clearHiddenSelectors();

  bool isCompassActive();
  void setCompassActive(bool newValue);

  bool isUpdating();
  void setDeferredHudName(const std::optional<std::string>& newDeferredHudName);
  std::optional<std::string> getDeferredHudName();
  std::optional<std::string> getCrosshairDefinition();
  bool setCrosshairDefinition(const std::string& newDefinition);

  // Each takes one hud.plist entry; a null PList where the entry was not a
  // dictionary.
  void addLegend(const oo::PList& info);
  void addDial(const oo::PList& info);
  void addMFD(const oo::PList& info);

  std::size_t mfdCount();

  void renderHUD();

  void refreshLastTransmitter();

  void setLineWidth(GLfloat value);
  GLfloat getLineWidth();

  bool minimalisticScanner();
  void setMinimalisticScanner(bool newValue);

  static Vector nonlinearScannerScale(Vector V, GLfloat zoom, double scale);
  bool nonlinearScanner();
  void setNonlinearScanner(bool newValue);

  bool scannerUltraZoom();
  void setScannerUltraZoom(bool newValue);

  std::shared_ptr<OOColor> reticleColorForIndex(std::size_t idx);
  bool setReticleColorForIndex(std::size_t idx, OOColor* newColor);

  bool checkPlayerInFlight();
  bool checkPlayerInSystemFlight();

  // Internal: dials, called by name through the dial table.
  void drawSurround(const oo::PList& info);
  void drawGreenSurround(const oo::PList& info);
  void drawYellowSurround(const oo::PList& info);
  void drawScanner(const oo::PList& info);
  void drawScannerZoomIndicator(const oo::PList& info);
  void drawCompass(const oo::PList& info);
  void drawAegis(const oo::PList& info);
  void drawTargetReticle(const oo::PList& info);
  void drawWaypoints(const oo::PList& info);
  void drawCustomBar(const oo::PList& info);
  void drawCustomText(const oo::PList& info);
  void drawCustomIndicator(const oo::PList& info);
  void drawCustomLight(const oo::PList& info);
  void drawCustomImage(const oo::PList& info);
  void drawSpeedBar(const oo::PList& info);
  void drawRollBar(const oo::PList& info);
  void drawPitchBar(const oo::PList& info);
  void drawYawBar(const oo::PList& info);
  void drawEnergyGauge(const oo::PList& info);
  void drawForwardShieldBar(const oo::PList& info);
  void drawAftShieldBar(const oo::PList& info);
  void drawFuelBar(const oo::PList& info);
  void drawWitchspaceDestination(const oo::PList& info);
  void drawCabinTempBar(const oo::PList& info);
  void drawWeaponTempBar(const oo::PList& info);
  void drawAltitudeBar(const oo::PList& info);
  void drawMissileDisplay(const oo::PList& info);
  void drawStatusLight(const oo::PList& info);
  void drawClock(const oo::PList& info);
  void drawPrimedEquipment(const oo::PList& info);
  void drawASCTarget(const oo::PList& info);
  void drawWeaponsOfflineText(const oo::PList& info);
  void drawFPSInfoCounter(const oo::PList& info);
  void drawScoopStatus(const oo::PList& info);
  void drawStickSensitivityIndicator(const oo::PList& info);
  void drawTrumbles(const oo::PList& info);

  // Internal: the state (the old ivars).
  // Widgets in draw order; were mutable arrays of array tuples.
  std::vector<OOHUDWidget> legendArray;
  std::vector<OOHUDWidget> dialArray;
  std::vector<OOHUDWidget> mfdArray;

  // zoom level
  GLfloat scanner_zoom = {};

  // where to draw it
  GLfloat z1 = {};
  GLfloat lineWidth = {};

  std::optional<std::string> hudName;
  std::optional<std::string>
      deferredHudName;  // Usually it will be nullopt. If engaged, then it means
                        // that we have a deferred HUD waiting to be drawn This
                        // may happen for example when a script handler attempts
                        // to switch HUD while it is being rendered. - Nikos
                        // 20110628
  bool hudUpdating = {};

  GLfloat overallAlpha = {};

  bool reticleTargetSensitive =
      {};  // TO DO: Move this into the propertiesReticleTargetSensitive
           // structure (Getafix - 2010/08/21)
  oo::PList
      propertiesReticleTargetSensitive;  // isAccurate (bool),
                                         // timeLastAccuracyProbabilityCalculation
                                         // (double)

  bool cloakIndicatorOnStatusLight = {};

  bool hudHidden = {};

  bool allowBigGui = {};

  int last_transmitter = {};

  std::set<std::string> _hiddenSelectors;

  // Crosshairs
  std::unique_ptr<OOCrosshairs> _crosshairs;
  OOWeaponType _lastWeaponType = {};
  GLfloat _lastOverallAlpha = {};
  bool _lastWeaponsOnline = {};
  oo::PList _crosshairOverrides;  // null for none
  std::shared_ptr<OOColor> _crosshairColor;
  GLfloat _crosshairScale = {};
  GLfloat _crosshairWidth = {};
  std::optional<std::string> crosshairDefinition;
  bool _compassActive = {};

  std::vector<std::shared_ptr<OOColor>> _reticleColors;

  // essentially scanner without gridlines
  bool minimalistic_scanner = {};

  // Nonlinear scanner
  bool nonlinear_scanner = {};
  bool scanner_ultra_zoom = {};

 private:
  void drawCrosshairs();
  void drawLegends();
  void drawDials();
  void drawMFDs();

  void drawLegend(const oo::PList& info);
  void drawHUDItem(const oo::PList& info);

  void drawMultiFunctionDisplay(const oo::PList& info, const std::string& text,
                                std::size_t index);

  void drawSurroundInternal(const oo::PList& info, const GLfloat color[4]);

  void drawCompassPlanetBlipAt(Vector relativePosition, NSSize siz,
                               GLfloat alpha);
  void drawCompassStationBlipAt(Vector relativePosition, NSSize siz,
                                GLfloat alpha);
  void drawCompassSunBlipAt(Vector relativePosition, NSSize siz, GLfloat alpha);
  void drawCompassTargetBlipAt(Vector relativePosition, NSSize siz,
                               GLfloat alpha);
  void drawCompassBeaconBlipAt(Vector relativePosition, NSSize siz,
                               GLfloat alpha);

  void drawSecondaryTargetReticle(const oo::PList& info);

  void drawIconForMissile(::ShipEntity* missile, bool selected,
                          OOMissileStatus status, int x, int y, GLfloat width,
                          GLfloat height, GLfloat alpha);
  void drawIconForEmptyPylonAtX(int x, int y, GLfloat width, GLfloat height,
                                GLfloat alpha);
  void drawDirectionCue(const oo::PList& info);

  oo::PList crosshairDefinitionForWeaponType(
      OOWeaponType weapon);  // a null PList for none

  void resetGui(::GuiDisplayGen* gui, const oo::PList& gui_info);
  void resetGuiPosition(::GuiDisplayGen* gui, const oo::PList& gui_info);
};

/*	The compass icon of a beacon whose code names no icon: the code's first
   character, drawn as text. The entities hold it by the C++ OOHUDBeaconIcon
   interface (OOPolygonSprite.h).
*/
class OOHUDBeaconCodeIcon : public OOHUDBeaconIcon {
 public:
  explicit OOHUDBeaconCodeIcon(const std::string& text);  // -initWithText:

  void drawHUDBeaconIconAt(NSPoint where, NSSize size, GLfloat alpha,
                           GLfloat z) override;

 private:
  std::string _text;
};

// Draws the sprite as a beacon icon (the sprite's OOHUDBeaconIcon interface).
void OOPolygonSpriteDrawHUDBeaconIcon(OOPolygonSprite* sprite, NSPoint where,
                                      NSSize size, GLfloat alpha, GLfloat z);

void OODrawString(const std::string& text, GLfloat x, GLfloat y, GLfloat z,
                  NSSize siz);
void OODrawStringAligned(const std::string& text, GLfloat x, GLfloat y,
                         GLfloat z, NSSize siz, bool rightAlign);

/* OODrawString(Aligned) handles all the string drawing, but because
 * it does texture application and GL_QUADS beginning once per string
 * it's quite slow.
 *
 * Where efficiency is needed, call OOStartDrawingStrings(), then
 * OODrawStringQuadsAligned for each bit of text, then
 * OOStopDrawingStrings().
 *
 * Trying to draw anything else between OOStartDrawingStrings() and
 * OOStopDrawingStrings() will have messy results. You can safely call
 * Stop, draw the other thing you want, then call Start again - it's
 * just a little inefficient. Similarly calling
 * OODrawStringQuadsAligned() without calling OOStartDrawingStrings()
 * won't work very well.
 *
 * - CIM
 */
void OOStartDrawingStrings();
void OODrawStringQuadsAligned(const std::string& text, GLfloat x, GLfloat y,
                              GLfloat z, NSSize siz, bool rightAlign);
void OOStopDrawingStrings();

void OODrawHilightedString(const std::string& text, GLfloat x, GLfloat y,
                           GLfloat z, NSSize siz);
void OODrawPlanetInfo(int gov, int eco, int tec, GLfloat x, GLfloat y,
                      GLfloat z, NSSize siz);
void OODrawHilightedPlanetInfo(int gov, int eco, int tec, GLfloat x, GLfloat y,
                               GLfloat z, NSSize siz);
NSRect OORectFromString(const std::string& text, GLfloat x, GLfloat y,
                        NSSize siz);

#include "OOStringWidth.h"  // OOStringWidthInEm()

void OOHUDResetTextEngine();

#include "WormholeEntity.h"  // WORMHOLE_SCANINFO (a send below)

class OOVisualEffectEntity;

/*	Free functions that forward one call each to other classes' members
        (HeadUpDisplay.cpp).
*/
// UNIVERSE->getGui()->setGLColorFromSetting() (OODrawPlanetInfo()).
void HeadUpDisplayUniverseGUISetGLColorFromSetting(
    const std::optional<std::string>& setting, OOColor* defaultValue,
    GLfloat alpha);
// The player, entities and universe as the reticles and waypoints read them
// (hudDrawReticleOnTarget(), hudDrawWaypoint(),
// hudRotateViewpointForVirtualDepth()).
OOGUIScreenID HeadUpDisplayPlayerGuiScreen(PlayerEntity* player);
HPVector HeadUpDisplayPlayerViewpointPosition(PlayerEntity* player);
GLfloat HeadUpDisplayPlayerWeaponRange(PlayerEntity* player);
std::optional<std::string> HeadUpDisplayPlayerDialTargetName(
    PlayerEntity* player);
double HeadUpDisplayPlayerClockTimeAdjusted(PlayerEntity* player);
Vector HeadUpDisplayPlayerCustomViewForwardVector(PlayerEntity* player);
Vector HeadUpDisplayPlayerCustomViewUpVector(PlayerEntity* player);
Quaternion HeadUpDisplayPlayerCustomViewQuaternion(PlayerEntity* player);
OOMatrix HeadUpDisplayPlayerRotationMatrix(PlayerEntity* player);
bool HeadUpDisplayEntityIsShip(Entity* entity);
bool HeadUpDisplayEntityIsWormhole(Entity* entity);
bool HeadUpDisplayEntityIsVisualEffect(Entity* entity);
HPVector HeadUpDisplayEntityPosition(Entity* entity);
GLfloat HeadUpDisplayEntityCollisionRadius(Entity* entity);
Quaternion HeadUpDisplayEntityOrientation(Entity* entity);
std::optional<std::string> HeadUpDisplayShipScanDescription(ShipEntity* ship);
bool HeadUpDisplayShipIsCloaked(ShipEntity* ship);
bool HeadUpDisplayShipIsHostileToPlayer(ShipEntity* ship);
GLfloat* HeadUpDisplayShipScannerDisplayColor(ShipEntity* ship, bool isHostile,
                                              bool flash);
GLfloat* HeadUpDisplayVisualEffectScannerDisplayColor(OOVisualEffectEntity* vis,
                                                      bool flash);
WORMHOLE_SCANINFO HeadUpDisplayWormholeScanInfo(WormholeEntity* wormhole);
double HeadUpDisplayWormholeEstimatedArrivalTime(WormholeEntity* wormhole);
double HeadUpDisplayWormholeExpiryTime(WormholeEntity* wormhole);
OOTimeAbsolute HeadUpDisplayUniverseGetTime();
const oo::PList*
HeadUpDisplayUniverseDescriptions();  // MissileIconDefinition()
OOViewID HeadUpDisplayUniverseViewDirection();
Entity* HeadUpDisplayUniverseFirstEntityTargetedByPlayer();
Entity* HeadUpDisplayUniverseFirstEntityTargetedByPlayerPrecisely();
