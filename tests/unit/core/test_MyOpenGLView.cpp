/*	test_MyOpenGLView.mm
        Unit tests for MyOpenGLView (src/SDL/MyOpenGLView.h), the game's SDL
   window and OpenGL view (the class shell, the window and OpenGL lifecycle,
   the splash screen).

        The view is made as the game makes it (new and init()), in a scratch
   home (no user defaults, so the splash
   screen is on and -init opens no window) with OpenAL on its null backend. The
   tests pin, through the C++ members, what the view's units answer without a
   window: the state -init leaves
   (no game controller, the window caption, the size the view starts at, the
   input state, a matrix manager), the display it would open on and that
   display's native size, and the projection -updateGLSize: sets up, read back
   from a hidden OpenGL context (oo_gl_test_context.hpp). Opening the game's
   window (-createWindowWithSize:, -initSplashScreen, -endSplashScreen,
   -initialiseGLWithSize:, -updateScreen) would show a real window on the
   desktop, which a unit test must not do; the
   integration tests launch the game, which runs all of them. The view's
   -dealloc quits SDL, so one view serves every test and the last one releases
   it. Further tests cover the accessors, the display-mode list and the
   saved window, full-screen and HDR settings, again without a window, and the
   debug image dumps; the snapshot needs the game window.

        MyOpenGLView.mm reaches the game controller, the universe and the
   player, so the test links every game object but main's
   (tests/unit/core/meson.build entry ['*']) and
   defines gDebugFlags. Run: meson test -C build/meson_test test_MyOpenGLView
*/

#include <process.h>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <numbers>
#include <string>
#include <vector>

#include "GameController.h"
#include "MyOpenGLView.h"
#include "FullScreenController.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/Defaults.hpp"
#include "oofnd/PList.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

MyOpenGLView* sView = nullptr;

// A scratch home (no user defaults) and the null OpenAL backend, then the view,
// made once.
MyOpenGLView* View() {
  if (sView != nullptr) return sView;
  namespace stdfs = std::filesystem;
  EXPECT_TRUE(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  const stdfs::path root =
      stdfs::temp_directory_path() /
      ("oo-test-myopenglview-" +
       std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(root);
  stdfs::create_directories(root);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", root.string().c_str()) == 0);
  stdfs::current_path(root);
  sView = oo::test::NewRetained<MyOpenGLView>();  // [[MyOpenGLView alloc] init]: nil (released)
                               // where init() fails
  if (!sView->init()) {
    oo::test::Release(sView);
    sView = nullptr;
  }
  return sView;
}

bool Near(double a, double b) { return std::fabs(a - b) < 1e-3; }

}  // namespace

TEST(MyOpenGLView, initWithoutAWindow) {
  {
    MyOpenGLView* view = View();
    EXPECT_TRUE(view != nullptr);
    if (view == nullptr) return;
    EXPECT_TRUE(view->getGameController() == nullptr);
    EXPECT_TRUE(!view->inFullScreenMode());
    // No defaults: the window size the game starts windowed at.
    EXPECT_TRUE(view->getViewSize().width == WINDOW_SIZE_DEFAULT_WIDTH &&
             view->getViewSize().height == WINDOW_SIZE_DEFAULT_HEIGHT);
    EXPECT_TRUE(view->getVirtualJoystickPosition().x == 0.0 &&
             view->getVirtualJoystickPosition().y == 0.0);
    EXPECT_TRUE(view->getAllowingStringInput() == StringInput::kNo);
    EXPECT_TRUE(!view->getIsAlphabetKeyDown());
    EXPECT_EQ(view->mouseWheelDelta(), 0.0f);
    // The matrix manager is C++: the C++ view's getter answers it, the same one
    // each time.
    EXPECT_TRUE(view != nullptr && view->getOpenGLMatrixManager() != nullptr);
    EXPECT_TRUE(view != nullptr &&
             view->getOpenGLMatrixManager() == view->getOpenGLMatrixManager());
    EXPECT_TRUE(!view->hdrOutput());  // set when the window is made
    EXPECT_TRUE(!view->msaa());
    EXPECT_TRUE(!view->getScreenSizeArray()
                  .empty());  // the display's modes, the native one first
  }
}

TEST(MyOpenGLView, windowCaption) {
  {
    MyOpenGLView* view = View();
    const std::optional<std::string> caption = view->getWindowCaption();
    EXPECT_TRUE(caption.has_value());
    EXPECT_TRUE(caption.has_value() && caption->rfind("Oolite v", 0) == 0);
    EXPECT_TRUE(caption.has_value() && caption->find(" by ") != std::string::npos);
    EXPECT_TRUE(caption == view->getWindowCaption());
  }
}

TEST(MyOpenGLView, displayAndNativeSize) {
  {
    MyOpenGLView* view = View();
    // No window yet: the first display.
    EXPECT_TRUE(view->getDisplayId() != 0);
    const oo::PList native = view->getNativeSize();
    EXPECT_TRUE(native.isDict());
    EXPECT_TRUE(native.get<int>(std::string(oo::kDisplayWidth), -1) > 0);
    EXPECT_TRUE(native.get<int>(std::string(oo::kDisplayHeight), -1) > 0);
    EXPECT_TRUE(native.get<int>(std::string(oo::kDisplayRefreshRate), -1) == 0);
    EXPECT_EQ(native.count(), 3u);
    // The mode list starts with it.
    const std::vector<oo::PList> modes = view->getScreenSizeArray();
    EXPECT_TRUE(!modes.empty() &&
             modes[0].get<int>(std::string(oo::kDisplayWidth), -1) ==
                 native.get<int>(std::string(oo::kDisplayWidth), -2));
  }
}

TEST(MyOpenGLView, updateGLSizeSetsTheProjection) {
  {
    MyOpenGLView* view = View();
    EXPECT_TRUE(OOTestGLContext());

    // 4:3 or narrower: the depth and x offset are fixed, y follows the aspect.
    view->updateGLSize(NSMakeSize(800, 600));
    EXPECT_TRUE(view->getViewSize().width == 800 &&
             view->getViewSize().height == 600);
    EXPECT_TRUE(view->backingViewSize().width == 800);
    EXPECT_TRUE(Near(view->getDisplay_z(), 640.0));
    EXPECT_TRUE(Near(view->getX_offset(), 320.0));
    EXPECT_TRUE(Near(view->getY_offset(), 240.0));
    GLint viewport[4] = {-1, -1, -1, -1};
    glGetIntegerv(GL_VIEWPORT, viewport);
    EXPECT_TRUE(viewport[0] == 0 && viewport[1] == 0 && viewport[2] == 800 &&
             viewport[3] == 600);

    view->updateGLSize(NSMakeSize(640, 960));
    EXPECT_TRUE(Near(view->getDisplay_z(), 640.0));
    EXPECT_TRUE(Near(view->getX_offset(), 320.0));
    EXPECT_TRUE(Near(view->getY_offset(), 480.0));

    // Wider than 4:3: the depth and x offset follow the aspect, y is fixed.
    view->updateGLSize(NSMakeSize(1600, 900));
    EXPECT_TRUE(Near(view->getDisplay_z(), 480.0 * 1600.0 / 900.0));
    EXPECT_TRUE(Near(view->getX_offset(), 240.0 * 1600.0 / 900.0));
    EXPECT_TRUE(Near(view->getY_offset(), 240.0));
    glGetIntegerv(GL_VIEWPORT, viewport);
    EXPECT_TRUE(viewport[2] == 1600 && viewport[3] == 900);
  }
}

// --- Accessors, display modes, settings, display / HDR, FOV and MSAA ---
// No window: none of these open one (full-screen mode is only
// switched off again, and the screen size is set while windowed). Settings the
// setters save go to the scratch home's defaults.

TEST(MyOpenGLView, slice2AccessorsAndViewSettings) {
  {
    MyOpenGLView* view = View();

    // The game controller is not retained: any controller round-trips (storage
    // nothing constructs: the view only keeps the pointer).
    alignas(GameController) static unsigned char
        controllerStorage[sizeof(GameController)];
    GameController* controller =
        reinterpret_cast<GameController*>(controllerStorage);
    view->setGameController(controller);
    EXPECT_TRUE(view->getGameController() == controller);
    view->setGameController(nullptr);
    EXPECT_TRUE(view->getGameController() == nullptr);

    // FOV: stored as tan(half the angle); in degrees or as that fraction.
    view->setFov(60.0f, false);
    EXPECT_TRUE(Near(view->fov(true), std::tan(30.0 * std::numbers::pi / 180.0)));
    EXPECT_TRUE(Near(view->fov(false), 60.0));
    view->setFov(0.5f, true);
    EXPECT_TRUE(Near(view->fov(true), 0.5));
    EXPECT_TRUE(Near(view->fov(false), 2.0 * std::atan(0.5) * 180.0 / std::numbers::pi));

    view->setMsaa(true);
    EXPECT_TRUE(view->msaa());
    view->setMsaa(false);
    EXPECT_TRUE(!view->msaa());

    // SDR tone mapper: clamped to the enum's range.
    view->setSDRToneMapper((oo::SDRToneMapper)99);
    EXPECT_TRUE(view->sdrToneMapper() == oo::SDRToneMapper::kReinhard);
    view->setSDRToneMapper((oo::SDRToneMapper)-7);
    EXPECT_TRUE(view->sdrToneMapper() == oo::SDRToneMapper::kNone);
    view->setSDRToneMapper(oo::SDRToneMapper::kAgX);
    EXPECT_TRUE(view->sdrToneMapper() == oo::SDRToneMapper::kAgX);

    // Colour saturation: adjusted, clamped to [0, MAX_COLOR_SATURATION]. No
    // window: it starts at 0.
    EXPECT_EQ(view->colorSaturation(), 0.0f);
    view->adjustColorSaturation(0.25f);
    EXPECT_TRUE(Near(view->colorSaturation(), 0.25));
    view->adjustColorSaturation(10.0f);
    EXPECT_TRUE(Near(view->colorSaturation(), MAX_COLOR_SATURATION));
    view->adjustColorSaturation(-10.0f);
    EXPECT_EQ(view->colorSaturation(), 0.0f);

    EXPECT_TRUE(!view->hdrOutput());
    (void)view->isOutputDisplayHDREnabled();  // the display's; not pinned
    view->grabMouseInsideGameWindow(false);

    // +pollShiftKey reads the keyboard: nobody holds shift during a test run.
    EXPECT_TRUE(!::MyOpenGLView::pollShiftKey());
    EXPECT_TRUE(view != nullptr && view->getOpenGLMatrixManager() != nullptr);

#if OOLITE_WINDOWS
    // No window: -isRunningOnPrimaryDisplayDevice answers NO, and
    // -atDesktopResolution is unset.
    EXPECT_TRUE(!view->isRunningOnPrimaryDisplayDevice());
    EXPECT_TRUE(!view->getAtDesktopResolution());
    unsigned width = 7, height = 9;
    view->getDisplayDimensions(&width,
                               &height);  // no window, no display: unchanged
    EXPECT_TRUE(width == 7 && height == 9);

    // HDR settings: clamped and saved to the defaults.
    view->setHDRMaxBrightness(10.0f);
    EXPECT_TRUE(Near(view->hdrMaxBrightness(), MIN_HDR_MAXBRIGHTNESS));
    view->setHDRMaxBrightness(5000.0f);
    EXPECT_TRUE(Near(view->hdrMaxBrightness(), MAX_HDR_MAXBRIGHTNESS));
    view->setHDRMaxBrightness(600.0f);
    EXPECT_TRUE(Near(view->hdrMaxBrightness(), 600.0));
    EXPECT_TRUE(Near(oo::Defaults::standard().floatForKey("hdr-max-brightness"),
                  600.0));
    view->setHDRPaperWhiteBrightness(1.0f);
    EXPECT_TRUE(Near(view->hdrPaperWhiteBrightness(), MIN_HDR_PAPERWHITE));
    view->setHDRPaperWhiteBrightness(1000.0f);
    EXPECT_TRUE(Near(view->hdrPaperWhiteBrightness(), MAX_HDR_PAPERWHITE));
    view->setHDRPaperWhiteBrightness(150.0f);
    EXPECT_TRUE(Near(view->hdrPaperWhiteBrightness(), 150.0));
    EXPECT_TRUE(
        Near(oo::Defaults::standard().floatForKey("hdr-paperwhite-brightness"),
             150.0));
    view->setHDRToneMapper((oo::HDRToneMapper)42);
    EXPECT_TRUE(view->hdrToneMapper() == oo::HDRToneMapper::kReinhard);
    view->setHDRToneMapper((oo::HDRToneMapper)-3);
    EXPECT_TRUE(view->hdrToneMapper() == oo::HDRToneMapper::kNone);
    view->setHDRToneMapper(oo::HDRToneMapper::kDice);
    EXPECT_TRUE(view->hdrToneMapper() == oo::HDRToneMapper::kDice);
#endif
  }
}

TEST(MyOpenGLView, slice2DisplayModesAndSavedSettings) {
  {
    MyOpenGLView* view = View();
    const std::vector<oo::PList> modes = view->getScreenSizeArray();
    EXPECT_TRUE(!modes.empty());
    if (modes.empty()) return;
    const int nativeWidth = modes[0].get<int>(std::string(oo::kDisplayWidth), -1);
    const int nativeHeight =
        modes[0].get<int>(std::string(oo::kDisplayHeight), -1);

    // Mode 0 is the native mode, refresh 0.
    EXPECT_TRUE(view->modeAsSize(0).width == nativeWidth &&
             view->modeAsSize(0).height == nativeHeight);
    EXPECT_EQ(view->indexOfDisplayModeForWidth(nativeWidth, nativeHeight, 0),
                0);
    EXPECT_EQ(view->indexOfDisplayModeForWidth(7, 5, 3),
                0);  // none: the native mode
    const int last = static_cast<int>(modes.size()) - 1;
    const oo::PList& lastMode = modes[last];
    const int lastIndex = view->indexOfDisplayModeForWidth(
        lastMode.get<int>(std::string(oo::kDisplayWidth)),
        lastMode.get<int>(std::string(oo::kDisplayHeight)),
        lastMode.get<int>(std::string(oo::kDisplayRefreshRate)));
    EXPECT_TRUE(lastIndex >= 0 && lastIndex <= last);

    // Repopulating the list gives the same list.
    view->populateFullScreenModelist();
    EXPECT_EQ(view->getScreenSizeArray().size(), modes.size());

    // Windowed: choosing a mode only records it.
    EXPECT_TRUE(!view->inFullScreenMode());
    view->setScreenSize(last);
    EXPECT_TRUE(view->currentScreenMode() == lastMode);
    EXPECT_TRUE(view->currentScreenSize().width ==
             lastMode.get<int>(std::string(oo::kDisplayWidth)));
    view->setDisplayMode(0, false);
    EXPECT_TRUE(!view->inFullScreenMode());
    EXPECT_TRUE(view->currentScreenMode() == modes[0]);
    EXPECT_TRUE(view->currentScreenSize().width == nativeWidth &&
             view->currentScreenSize().height == nativeHeight);

    // Full-screen mode is saved to the defaults; switching it on without a
    // window is not tried.
    view->setFullScreenMode(false);
    EXPECT_TRUE(!view->inFullScreenMode());
    EXPECT_TRUE(oo::Defaults::standard().object("fullscreen").isBool() &&
             !oo::Defaults::standard().boolForKey("fullscreen"));

    // The windowed size round-trips through the defaults.
    view->saveWindowSize(NSMakeSize(1000, 700));
    EXPECT_EQ(oo::Defaults::standard().integerForKey("window_width"), 1000);
    EXPECT_TRUE(view->loadWindowSize().width == 1000 &&
             view->loadWindowSize().height == 700);

    // Full-screen settings: windowed (saved above), the saved mode's index
    // (none saved: 0).
    EXPECT_EQ(view->loadFullscreenSettings(), 0);
    EXPECT_TRUE(!view->inFullScreenMode());
    oo::Defaults::standard().setInteger("display_width", nativeWidth);
    oo::Defaults::standard().setInteger("display_height", nativeHeight);
    oo::Defaults::standard().setInteger("display_refresh", 0);
    EXPECT_EQ(view->loadFullscreenSettings(), 0);
    EXPECT_TRUE(view->currentScreenMode() == modes[0]);
  }
}

// --- Snapshots and the debug image dumps ---
// -snapShot: reads the game window's surface, which a unit test may not
// open (the GUI tests' screenshots run it). The dumps write PNGs under
// <home>/oolite-saves/snapshots.

#ifndef NDEBUG
namespace {

std::filesystem::path DumpPath(const std::string& name) {
  return std::filesystem::current_path() / "oolite-saves" / "snapshots" /
         (name + ".png");
}

}  // namespace

TEST(MyOpenGLView, slice3DebugImageDumps) {
  {
    namespace stdfs = std::filesystem;
    MyOpenGLView* view = View();
    std::filesystem::create_directories(stdfs::current_path() / "oolite-saves" /
                                        "snapshots");
    EXPECT_TRUE(
        stdfs::exists(stdfs::current_path() / "oolite-saves" / "snapshots"));

    // 3 x 2 pixels, rows padded to 16 bytes.
    std::vector<uint8_t> rgba(16 * 2, 0x80);
    view->dumpRGBAToFileNamed("oo-test-rgba", rgba.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-rgba")));
    view->dumpRGBToFileNamed("oo-test-rgb", rgba.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-rgb")));
    view->dumpGrayToFileNamed("oo-test-gray", rgba.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-gray")));
    // -dumpGrayAlpha writes its expansion into the bytes it was given (4 per
    // pixel), so they are a buffer with 4 bytes a pixel; only the file is
    // pinned.
    std::vector<uint8_t> grayAlpha(16 * 2, 0x40);
    view->dumpGrayAlphaToFileNamed("oo-test-grayalpha", grayAlpha.data(), 3, 2,
                                   16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-grayalpha")));

    // Too short rows, no bytes, or no size: nothing is written.
    view->dumpRGBAToFileNamed("oo-test-short", rgba.data(), 3, 2, 11);
    view->dumpRGBToFileNamed("oo-test-short", nullptr, 3, 2, 16);
    view->dumpGrayToFileNamed("oo-test-short", rgba.data(), 0, 2, 16);
    view->dumpGrayAlphaToFileNamed("oo-test-short", rgba.data(), 3, 2, 5);
    EXPECT_TRUE(!stdfs::exists(DumpPath("oo-test-short")));

    // RGBA split: the RGB file always; the gray (alpha) file only when some
    // alpha is neither 0 nor 255.
    std::vector<uint8_t> opaque(16 * 2, 0xFF);
    view->dumpRGBAToRGBFileNamed(std::string("oo-test-split-rgb"),
                                 std::string("oo-test-split-alpha"),
                                 opaque.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-split-rgb")));
    EXPECT_TRUE(!stdfs::exists(DumpPath("oo-test-split-alpha")));
    std::vector<uint8_t> translucent(16 * 2, 0x80);
    view->dumpRGBAToRGBFileNamed(std::nullopt,
                                 std::string("oo-test-split2-alpha"),
                                 translucent.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-split2-alpha")));
    view->dumpRGBAToRGBFileNamed(std::nullopt, std::nullopt, translucent.data(),
                                 3, 2, 16);
  }
}

// A gray+alpha dump reads the caller's bytes (2 a pixel, tightly packed rows)
// and writes neither them nor past their end.
TEST(MyOpenGLView, dumpGrayAlphaLeavesTheCallersBytes) {
  {
    namespace stdfs = std::filesystem;
    MyOpenGLView* view = View();
    stdfs::create_directories(stdfs::current_path() / "oolite-saves" /
                              "snapshots");

    const std::size_t width = 3, height = 2, rowBytes = 2 * width;
    std::vector<uint8_t> buffer(rowBytes * height + 64,
                                0xA5);  // the image, then a guard
    for (size_t i = 0; i < rowBytes * height; i++)
      buffer[i] = static_cast<uint8_t>(i * 17 + 3);
    const std::vector<uint8_t> before = buffer;
    view->dumpGrayAlphaToFileNamed("oo-test-grayalpha-tight", buffer.data(),
                                   width, height, rowBytes);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-grayalpha-tight")));
    EXPECT_TRUE(buffer == before);
  }
}

#endif

// The view's state is private; MyOpenGLView.h names this struct its friend so
// the checks below read the same members.
struct MyOpenGLViewTestAccess {
  static auto& viewSize(MyOpenGLView* view) { return view->viewSize; }
  static auto& display_z(MyOpenGLView* view) { return view->display_z; }
  static auto& matrixManager(MyOpenGLView* view) { return view->matrixManager; }
};

// The C++ view the game made.
TEST(MyOpenGLView, cxxAPI) {
  {
    MyOpenGLView* view = View();
    MyOpenGLView* cxxView = view;
    EXPECT_TRUE(cxxView != nullptr);
    EXPECT_TRUE(cxxView->getWindowCaption() == view->getWindowCaption());
    EXPECT_TRUE(cxxView->getDisplayId() == view->getDisplayId());
    cxxView->updateGLSize(NSMakeSize(1024, 768));
    EXPECT_TRUE(view->getViewSize().width == 1024 &&
             MyOpenGLViewTestAccess::viewSize(cxxView).height == 768);
    EXPECT_TRUE(Near(MyOpenGLViewTestAccess::display_z(cxxView), 640.0));
    // The matrix manager is C++; the view's getter answers it.
    EXPECT_TRUE(cxxView->getOpenGLMatrixManager() ==
             MyOpenGLViewTestAccess::matrixManager(cxxView).get());
  }
}

// The members: accessors, display modes and settings.
TEST(MyOpenGLView, cxxSlice2API) {
  {
    MyOpenGLView* view = View();
    MyOpenGLView* cxxView = view;
    EXPECT_TRUE(cxxView != nullptr);
    if (cxxView == nullptr) return;
    EXPECT_TRUE(cxxView->getViewSize().width == view->getViewSize().width);
    EXPECT_TRUE(cxxView->backingViewSize().height ==
             view->backingViewSize().height);
    EXPECT_TRUE(cxxView->getDisplay_z() == view->getDisplay_z());
    EXPECT_TRUE(cxxView->getX_offset() == view->getX_offset() &&
             cxxView->getY_offset() == view->getY_offset());
    EXPECT_TRUE(cxxView->getGameController() == view->getGameController());
    cxxView->setFov(45.0f, false);
    EXPECT_TRUE(Near(view->fov(false), 45.0) && cxxView->fov(true) == view->fov(true));
    cxxView->setMsaa(true);
    EXPECT_TRUE(view->msaa() && cxxView->msaa());
    cxxView->setMsaa(false);
    cxxView->setSDRToneMapper(oo::SDRToneMapper::kUC2);
    EXPECT_TRUE(view->sdrToneMapper() == oo::SDRToneMapper::kUC2);
    EXPECT_TRUE(cxxView->colorSaturation() == view->colorSaturation());
    EXPECT_TRUE(cxxView->hdrOutput() == static_cast<bool>(view->hdrOutput()));
    EXPECT_TRUE(cxxView->inFullScreenMode() ==
             static_cast<bool>(view->inFullScreenMode()));
    EXPECT_TRUE(cxxView->getScreenSizeArray() == view->getScreenSizeArray());
    EXPECT_TRUE(cxxView->currentScreenMode() == view->currentScreenMode());
    EXPECT_TRUE(cxxView->modeAsSize(0).width == view->modeAsSize(0).width);
    EXPECT_EQ(cxxView->indexOfDisplayModeForWidth(7, 5, 3), 0);
    EXPECT_TRUE(cxxView->loadWindowSize().width == view->loadWindowSize().width);
    EXPECT_TRUE(MyOpenGLView::pollShiftKey() ==
             static_cast<bool>(::MyOpenGLView::pollShiftKey()));
#if OOLITE_WINDOWS
    cxxView->setHDRMaxBrightness(700.0f);
    EXPECT_TRUE(Near(view->hdrMaxBrightness(), 700.0));
    EXPECT_TRUE(cxxView->getAtDesktopResolution() ==
             static_cast<bool>(view->getAtDesktopResolution()));
#endif
  }
}

// The dumps as members.
TEST(MyOpenGLView, cxxSlice3API) {
  {
#ifndef NDEBUG
    namespace stdfs = std::filesystem;
    MyOpenGLView* cxxView = View();
    EXPECT_TRUE(cxxView != nullptr);
    if (cxxView == nullptr) return;
    std::vector<uint8_t> rgba(16 * 2, 0x80);
    cxxView->dumpRGBAToFileNamed("oo-test-cxx-rgba", rgba.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-cxx-rgba")));
    cxxView->dumpRGBAToRGBFileNamed(std::string("oo-test-cxx-rgb"),
                                    std::string("oo-test-cxx-alpha"),
                                    rgba.data(), 3, 2, 16);
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-cxx-rgb")));
    EXPECT_TRUE(stdfs::exists(DumpPath("oo-test-cxx-alpha")));
    cxxView->dumpGrayToFileNamed("oo-test-cxx-short", rgba.data(), 3, 2, 2);
    EXPECT_TRUE(!stdfs::exists(DumpPath("oo-test-cxx-short")));
#endif
  }
}

// Last: -dealloc quits SDL.
TEST(MyOpenGLView, zzDeallocReleasesTheView) {
  {
    MyOpenGLView* view = View();
    if (view != nullptr) oo::test::Release(view);
    sView = nullptr;
  }
}
