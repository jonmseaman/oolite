/*	test_HeadUpDisplay.mm
        Unit tests for HeadUpDisplay (src/Core/HeadUpDisplay.h).

        The HUD is made from a hud.plist dictionary: its MFDs, its reticle
   colours (from the drawTargetReticle: dial, green/red/cyan without one),
   whether it has a compass, its overall alpha, the reticle sensitivity and its
   properties, big-GUI permission, the crosshair file (a missing file falls back
   to crosshairs.plist) and the scanner flags. The tests pin those, the
        accessors and setters (clamping, the hidden selectors, the deferred HUD
   name, nil colours refused), the reticle colours' identity, and
   +nonlinearScannerScale:Zoom:Scale:. Drawing is GL and is pinned by the
   integration tests, not here.

        The HUD's object references the universe, the player and the GUIs, so
   the test links the whole game but main (tests/unit/core/meson.build entry
   ['*']) on the hidden GL context of oo_gl_test_context.hpp.
   UNIVERSE and PLAYER are nil. The resource manager runs in strict mode on a
   scratch Resources folder the test writes (the current directory and HOMEPATH
        point at it), holding its own whitelist and one crosshair file, so no
   game resource and no add-on is read; the font is missing, so the text engine
   has no texture. The last test pins the C++ API. Run:
   meson test -C build/meson_test test_HeadUpDisplay
*/

#include <process.h>
#include <stdlib.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "HeadUpDisplay.h"
#include "Color.h"
#include "ResourceManager.h"
#include "oo_gl_test_context.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;

void WriteFile(const stdfs::path& path, const char* text) {
  std::ofstream out(path, std::ios::binary);
  out << text;
}

/*	The scratch folder, before the resource manager's first use: a whitelist
   naming the dials the tests use, and a crosshair file.
*/
void SetUp() {
  EXPECT_TRUE(OOTestGLContext());
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-hud-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot / "Resources" / "Config");
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  WriteFile(
      sRoot / "Resources" / "Config" / "whitelist.plist",
      "{\n\thud_dial_methods = (\"drawTrumbles:\", \"drawTargetReticle:\", "
      "\"drawCompass:\", \"drawScanner:\");\n}\n");
  WriteFile(sRoot / "Resources" / "Config" / "testcross.plist",
            "{\n\tWEAPON_OFFLINE = (1, 2, 3, 4, 5, 6);\n}\n");
  ResourceManager::setUseAddOns(std::string(
      SCENARIO_OXP_DEFINITION_NONE));  // strict: the built-in Resources alone
}

// Makes the C++ HUD from a dictionary and file name.
std::shared_ptr<HeadUpDisplay> NewHUD(
    const oo::PList& info,
    const std::optional<std::string>& name = std::string("test-hud.plist")) {
  std::shared_ptr<HeadUpDisplay> hud = oo::makeShared<HeadUpDisplay>();
  hud->initWithDictionary(info, name);
  return hud;
}

oo::PList Dial(const char* selector) {
  return oo::PList(oo::PList::Dict{{"selector", oo::PList(selector)}});
}

bool SameRGBA(oo::Color* color, float r, float g, float b, float a) {
  if (color == nullptr) return false;
  float cr = 0, cg = 0, cb = 0, ca = 0;
  color->getRed(&cr, &cg, &cb, &ca);
  return cr == r && cg == g && cb == b && ca == a;
}

}  // namespace

TEST(HeadUpDisplay, madeFromAnEmptyDictionary) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(oo::PList::Dict{}));
    EXPECT_TRUE(hud != nullptr);
    EXPECT_TRUE(hud->getHudName() == std::optional<std::string>("test-hud.plist"));
    EXPECT_TRUE(!hud->getDeferredHudName().has_value());
    EXPECT_EQ(hud->mfdCount(), 0u);
    EXPECT_TRUE(hud->getOverallAlpha() == 0.75f);
    EXPECT_TRUE(!hud->getReticleTargetSensitive());
    EXPECT_TRUE(!hud->getAllowBigGui());
    EXPECT_TRUE(!hud->isHidden());
    EXPECT_TRUE(!hud->isUpdating());
    EXPECT_TRUE(!hud->isCompassActive());
    EXPECT_TRUE(!hud->minimalisticScanner());
    EXPECT_TRUE(!hud->nonlinearScanner());
    EXPECT_TRUE(!hud->scannerUltraZoom());
    EXPECT_TRUE(hud->getLineWidth() == 1.0f);
    EXPECT_TRUE(hud->scannerZoom() == 0.0f);
    EXPECT_TRUE(!hud->getCrosshairDefinition().has_value());

    // The reticle sensitivity's properties: accurate, last computed at the
    // universe's time (nil: 0).
    oo::PList* properties = hud->getPropertiesReticleTargetSensitive();
    EXPECT_TRUE(properties != nullptr);
    EXPECT_TRUE(properties->get<bool>("isAccurate", false));
    EXPECT_TRUE(properties->get<double>("timeLastAccuracyProbabilityCalculation",
                                     -1.0) == 0.0);

    // No drawTargetReticle: dial: green, red, cyan.
    EXPECT_TRUE(SameRGBA(hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET).get(),
                      0, 1, 0, 1));
    EXPECT_TRUE(SameRGBA(
        hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET_SENSITIVE).get(), 1,
        0, 0, 1));
    EXPECT_TRUE(
        SameRGBA(hud->reticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE).get(), 0,
                 1, 1, 1));
    EXPECT_TRUE(hud->reticleColorForIndex(3) == nullptr);

    // A nil name is kept as nil.
    EXPECT_TRUE(!NewHUD(oo::PList(oo::PList::Dict{}), std::nullopt)
                  ->getHudName()
                  .has_value());
  }
}

TEST(HeadUpDisplay, readsItsConfiguration) {
  ::SetUp();
  {
    const oo::PList mfd(
        oo::PList::Dict{{"x", oo::PList(10)}, {"y", oo::PList(20)}});
    const oo::PList info(oo::PList::Dict{
        {"dials",
         oo::PList(oo::PList::Array{Dial("drawCompass:"), Dial("drawScanner:"),
                                    oo::PList("not a dictionary")})},
        {"multi_function_displays",
         oo::PList(oo::PList::Array{
             mfd, mfd, oo::PList(7)})},  // a non-dictionary entry counts too
        {"overall_alpha", oo::PList(0.5)},
        {"reticle_target_sensitive", oo::PList(true)},
        {"allow_big_gui", oo::PList(true)},
        {"scanner_minimalistic", oo::PList(true)},
        {"scanner_non_linear", oo::PList(true)},
        {"scanner_ultra_zoom", oo::PList(true)},
    });
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(info);
    EXPECT_EQ(hud->mfdCount(), 3u);
    EXPECT_TRUE(hud->getOverallAlpha() == 0.5f);
    EXPECT_TRUE(hud->getReticleTargetSensitive());
    EXPECT_TRUE(hud->getAllowBigGui());
    EXPECT_TRUE(hud->isCompassActive());
    EXPECT_TRUE(hud->minimalisticScanner());
    EXPECT_TRUE(hud->nonlinearScanner());
    EXPECT_TRUE(hud->scannerUltraZoom());
  }
}

TEST(HeadUpDisplay, reticleColoursFromTheTargetReticleDial) {
  ::SetUp();
  {
    oo::PList reticle(
        oo::PList::Dict{{"selector", oo::PList("drawTargetReticle:")},
                        {"target_rgba", oo::PList("blueColor")},
                        {"wormhole_rgba", oo::PList("yellowColor")}});
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(
        oo::PList::Dict{{"dials", oo::PList(oo::PList::Array{reticle})}}));
    EXPECT_TRUE(SameRGBA(hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET).get(),
                      0, 0, 1, 1));
    EXPECT_TRUE(SameRGBA(
        hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET_SENSITIVE).get(), 1,
        0, 0, 1));  // the default
    EXPECT_TRUE(
        SameRGBA(hud->reticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE).get(), 1,
                 1, 0, 1));

    // The list stops at the first colour that does not parse.
    oo::PList shortList(oo::PList::Dict{
        {"selector", oo::PList("drawTargetReticle:")},
        {"target_sensitive_rgba", oo::PList(oo::PList::Array{})}});
    std::shared_ptr<HeadUpDisplay> shortHUD = NewHUD(oo::PList(
        oo::PList::Dict{{"dials", oo::PList(oo::PList::Array{shortList})}}));
    EXPECT_TRUE(
        SameRGBA(shortHUD->reticleColorForIndex(OO_RETICLE_COLOR_TARGET).get(),
                 0, 1, 0, 1));
    EXPECT_TRUE(shortHUD->reticleColorForIndex(
                 OO_RETICLE_COLOR_TARGET_SENSITIVE) == nullptr);
    EXPECT_TRUE(shortHUD->reticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE) == nullptr);
    EXPECT_TRUE(!shortHUD->setReticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE,
                                                oo::Color::whiteColor().get()));
  }
}

TEST(HeadUpDisplay, setReticleColour) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(oo::PList::Dict{}));
    std::shared_ptr<oo::Color> magenta = oo::Color::magentaColor();
    EXPECT_TRUE(
        hud->setReticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE, magenta.get()));
    EXPECT_TRUE(hud->reticleColorForIndex(OO_RETICLE_COLOR_WORMHOLE) ==
             magenta);  // the same object
    EXPECT_TRUE(!hud->setReticleColorForIndex(OO_RETICLE_COLOR_TARGET, nullptr));
    EXPECT_TRUE(SameRGBA(hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET).get(),
                      0, 1, 0, 1));
    EXPECT_TRUE(!hud->setReticleColorForIndex(3, magenta.get()));
  }
}

TEST(HeadUpDisplay, crosshairFile) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(
        oo::PList::Dict{{"crosshair_file", oo::PList("testcross.plist")}}));
    EXPECT_TRUE(hud->getCrosshairDefinition() ==
             std::optional<std::string>("testcross.plist"));

    // A file that is not found falls back to crosshairs.plist.
    std::shared_ptr<HeadUpDisplay> missing = NewHUD(oo::PList(
        oo::PList::Dict{{"crosshair_file", oo::PList("nosuch.plist")}}));
    EXPECT_TRUE(missing->getCrosshairDefinition() ==
             std::optional<std::string>("crosshairs.plist"));
  }
}

TEST(HeadUpDisplay, accessorsAndSetters) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(oo::PList::Dict{}));

    hud->setHudName(std::string("other.plist"));
    EXPECT_TRUE(hud->getHudName() == std::optional<std::string>("other.plist"));
    hud->setHudName(std::nullopt);  // ignored
    EXPECT_TRUE(hud->getHudName() == std::optional<std::string>("other.plist"));

    hud->setDeferredHudName(std::string("deferred.plist"));
    EXPECT_TRUE(hud->getDeferredHudName() ==
             std::optional<std::string>("deferred.plist"));
    hud->setDeferredHudName(std::nullopt);
    EXPECT_TRUE(!hud->getDeferredHudName().has_value());

    hud->setScannerZoom(2.5f);
    EXPECT_TRUE(hud->scannerZoom() == 2.5f);

    hud->setOverallAlpha(1.5f);
    EXPECT_TRUE(hud->getOverallAlpha() == 1.0f);
    hud->setOverallAlpha(-1.0f);
    EXPECT_TRUE(hud->getOverallAlpha() == 0.0f);
    hud->setOverallAlpha(0.25f);
    EXPECT_TRUE(hud->getOverallAlpha() == 0.25f);

    hud->setReticleTargetSensitive(true);
    EXPECT_TRUE(hud->getReticleTargetSensitive());

    // Hidden: big GUIs are allowed while the HUD is hidden.
    hud->setHidden(true);
    EXPECT_TRUE(hud->isHidden());
    EXPECT_TRUE(hud->getAllowBigGui());
    hud->setHidden(false);
    EXPECT_TRUE(!hud->getAllowBigGui());

    hud->setCompassActive(true);
    EXPECT_TRUE(hud->isCompassActive());
    hud->setCompassActive(false);
    EXPECT_TRUE(!hud->isCompassActive());

    hud->setMinimalisticScanner(true);
    EXPECT_TRUE(hud->minimalisticScanner());
    hud->setNonlinearScanner(true);
    EXPECT_TRUE(hud->nonlinearScanner());
    hud->setScannerUltraZoom(true);
    EXPECT_TRUE(hud->scannerUltraZoom());

    hud->setLineWidth(3.0f);
    EXPECT_TRUE(hud->getLineWidth() == 3.0f);

    // Nothing to refresh, nothing to reset, and the player is not flying: no
    // universe.
    hud->refreshLastTransmitter();
    hud->resetGuis(oo::PList(
        oo::PList::Dict{{"message_gui", oo::PList(oo::PList::Dict{})}}));
  }
}

TEST(HeadUpDisplay, hiddenSelectors) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(oo::PList::Dict{}));
    EXPECT_TRUE(!hud->hasHidden(std::string("drawCompass:")));
    EXPECT_TRUE(!hud->hasHidden(std::nullopt));
    hud->setHiddenSelector("drawCompass:", true);
    hud->setHiddenSelector("drawScanner:", true);
    EXPECT_TRUE(hud->hasHidden(std::string("drawCompass:")));
    EXPECT_TRUE(hud->hasHidden(std::string("drawScanner:")));
    hud->setHiddenSelector("drawCompass:", false);
    EXPECT_TRUE(!hud->hasHidden(std::string("drawCompass:")));
    EXPECT_TRUE(hud->hasHidden(std::string("drawScanner:")));
    hud->clearHiddenSelectors();
    EXPECT_TRUE(!hud->hasHidden(std::string("drawScanner:")));
  }
}

TEST(HeadUpDisplay, nonlinearScannerScale) {
  // The direction is kept; the length is the scanner's nonlinear map of the
  // distance.
  const Vector v = HeadUpDisplay::nonlinearScannerScale(
      make_vector(3000.0f, 0.0f, 4000.0f), 1.0f, 256.0);
  const Vector unit = vector_normal(v);
  EXPECT_TRUE(std::fabs(unit.x - 0.6f) < 1e-6f && unit.y == 0.0f &&
           std::fabs(unit.z - 0.8f) < 1e-6f);
  const float length = magnitude(v);
  EXPECT_TRUE(std::fabs(length - 50.0f) <
           1e-4f);  // 49.9999962 on the Objective-C class
}

TEST(HeadUpDisplay, setCrosshairDefinition) {
  ::SetUp();
  {
    // The scripts' crosshair setter. (The frame's dials by name are the dial
    // table's.)
    std::shared_ptr<HeadUpDisplay> hud = NewHUD(oo::PList(oo::PList::Dict{}));
    EXPECT_TRUE(hud->setCrosshairDefinition("testcross.plist"));
    EXPECT_TRUE(hud->getCrosshairDefinition() ==
             std::optional<std::string>("testcross.plist"));
    EXPECT_TRUE(!hud->setCrosshairDefinition("nosuch.plist"));
    EXPECT_TRUE(hud->getCrosshairDefinition() ==
             std::optional<std::string>("crosshairs.plist"));
  }
}

TEST(HeadUpDisplay, beaconCodeIcon) {
  ::SetUp();
  {
    // The entities hold the code icon by the C++ OOHUDBeaconIcon interface.
    std::shared_ptr<oo::HUDBeaconCodeIcon> icon = oo::makeShared<oo::HUDBeaconCodeIcon>("A");
    EXPECT_TRUE(icon != nullptr);
    EXPECT_TRUE(dynamic_cast<oo::HUDBeaconIcon*>(icon.get()) != nullptr);
  }
}

// --- The C++ API
// ------------------------------------------------------------

TEST(HeadUpDisplay, cxxAPI) {
  ::SetUp();
  {
    std::shared_ptr<HeadUpDisplay> hud = oo::makeShared<HeadUpDisplay>();
    hud->initWithDictionary(
        oo::PList(oo::PList::Dict{
            {"dials", oo::PList(oo::PList::Array{Dial("drawCompass:")})},
            {"multi_function_displays",
             oo::PList(oo::PList::Array{oo::PList(oo::PList::Dict{})})},
            {"crosshair_file", oo::PList("testcross.plist")},
        }),
        std::string("cxx.plist"));
    EXPECT_TRUE(hud->getHudName() == std::optional<std::string>("cxx.plist"));
    EXPECT_EQ(hud->mfdCount(), 1u);
    EXPECT_TRUE(hud->isCompassActive());
    EXPECT_TRUE(hud->getOverallAlpha() == 0.75f);
    EXPECT_TRUE(hud->getLineWidth() == 1.0f);
    EXPECT_TRUE(hud->getCrosshairDefinition() ==
             std::optional<std::string>("testcross.plist"));
    EXPECT_TRUE(hud->getPropertiesReticleTargetSensitive()->get<bool>("isAccurate",
                                                                   false));

    std::shared_ptr<oo::Color> target =
        hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET);
    EXPECT_TRUE(target != nullptr && target->redComponent() == 0.0f &&
             target->greenComponent() == 1.0f);
    EXPECT_TRUE(hud->reticleColorForIndex(3) == nullptr);
    std::shared_ptr<oo::Color> blue = oo::Color::blueColor();
    EXPECT_TRUE(hud->setReticleColorForIndex(OO_RETICLE_COLOR_TARGET, blue.get()));
    EXPECT_TRUE(hud->reticleColorForIndex(OO_RETICLE_COLOR_TARGET) == blue);
    EXPECT_TRUE(!hud->setReticleColorForIndex(OO_RETICLE_COLOR_TARGET, nullptr));

    hud->setOverallAlpha(2.0f);
    EXPECT_TRUE(hud->getOverallAlpha() == 1.0f);
    hud->setHidden(true);
    EXPECT_TRUE(hud->getAllowBigGui());
    hud->setHiddenSelector("drawCompass:", true);
    EXPECT_TRUE(hud->hasHidden(std::string("drawCompass:")));

    // Neither the player nor the universe: not in flight. Only a release-style
    // build can ask with no player: the debug build asserts that PLAYER is set
    // (OOGetPlayer()).
#if !OO_DEBUG
    EXPECT_TRUE(!hud->checkPlayerInFlight());
    EXPECT_TRUE(!hud->checkPlayerInSystemFlight());
#endif

    const Vector v = HeadUpDisplay::nonlinearScannerScale(
        make_vector(3000.0f, 0.0f, 4000.0f), 1.0f, 256.0);
    EXPECT_TRUE(std::fabs(magnitude(v) - 50.0f) < 1e-4f);
  }
}
