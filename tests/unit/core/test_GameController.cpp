/*	test_GameController.mm
        Unit tests for GameController (src/Core/GameController.h): the
   singleton, pause, EcoQoS, the mouse interaction modes, the game view and
   the launch flag.

        The controller is the application's process-wide singleton. It needs the
   JavaScript engine (its pause events are named by OOJSID) and the player
   (PLAYER), so the test links every game object but main's
   (tests/unit/core/meson.build entry ['*']), defines
   gDebugFlags, points the user's home (HOMEPATH: the defaults) at a scratch
   folder, and stands in a recording object for the player and another for the
   game view (as test_DustEntity does for the player). The universe is nil, so
   what the controller sends it is a message to nil, as before the game starts.
   Nothing is drawn and no window is opened.

        It pins what the controller does: one shared
   object; the launch flag; the game view it keeps (retained) and tells about
   itself; the mouse interaction modes and the view's notes of each change;
   pause and resume (the mode they save and restore, the player's script events,
   and the process priority class EcoQoS sets on Windows, which a connected
   debug console keeps at normal); and the full-screen
   members and the player-file paths. The cases call the C++ class, the view a
   C++ subclass of the game's. Run:
   meson test -C build/meson_test test_GameController
*/

#if defined(_WIN32)
#include <winsock2.h>  // before windows.h, which a game header brings in
#endif
#include <process.h>

#include <chrono>
#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "GameController.h"
#include "MyOpenGLView.h"
#include "DebugTCPConsoleClient.h"
#include "FullScreenController.h"
#include "OOJSPropID.h"
#include "OOJavaScriptEngine.h"
#include "PlayerEntity.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/Exception.hpp"
#include "EntityManualRefs.h"  // the tests' manual entity references

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity* gOOPlayer;

// PLAYER: records the script events it is sent and answers whether mouse
// control is on.
class TestPlayer : public PlayerEntity {
 public:
  std::vector<OOJSPropID> _events;
  bool _mouseControlOn = false;

  using PlayerEntity::doScriptEvent;
  void doScriptEvent(OOJSPropID message) override {
    _events.push_back(message);
  }
  bool isMouseControlOn() override { return _mouseControlOn; }
};

// A C++ player under the ship's facade, as PlayerEntity::sharedPlayer() makes
// the game's, retained (+1) as +alloc's object was.
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// The game view: records the controller it is given and the mode changes it is
// told of, and answers the screen modes the full-screen members ask for. A C++
// subclass of the game's view, overriding the members it answered, which are
// virtual for it; a view made here never opens SDL.
class TestView : public MyOpenGLView {
 public:
  ::GameController* _controller = nullptr;
  std::vector<std::pair<int, int>> _modeNotes;
  std::vector<oo::PList> _screenModes;
  oo::PList _currentMode;
  bool _inFullScreen = false;
  int _splashEnds = 0;
  int _screenUpdates = 0;

  void setGameController(::GameController* controller) override {
    _controller = controller;
  }
  void noteMouseInteractionModeChangedFrom(
      oo::MouseInteractionMode oldMode, oo::MouseInteractionMode newMode) override {
    _modeNotes.push_back({(int)oldMode, (int)newMode});
  }
  std::vector<oo::PList> getScreenSizeArray() override { return _screenModes; }
  oo::PList currentScreenMode() override { return _currentMode; }
  NSSize currentScreenSize() override { return NSMakeSize(0, 0); }
  bool inFullScreenMode() override { return _inFullScreen; }
  void endSplashScreen() override { _splashEnds++; }
  void updateScreen() override { _screenUpdates++; }
};

// A deferred call's target: records the arguments it is sent, and raises on
// request. A deferred C++ call holding it and the argument (Call below)
// retains them until it runs.
class TestTarget final : public oo::Object {
 public:
  void record(oo::Object* argument) {
    _calls.push_back(argument != nullptr ? "arg" : "nil");
  }
  void raise(oo::Object* ) {
    _calls.push_back("raise");
    oo::RaiseException(oo::GenericException, "%s", "deferred call failure");
  }

  std::vector<std::string> _calls;
};

// A deferred call's argument.
class TestArgument final : public oo::Object {};

// OOScheduleDeferredCall(target, "record:" or "raise:", argument, delay): the
// call holds target and argument (std::shared_ptr, as the performer retained them)
// until it has run.
void Call(TestTarget* target, void (TestTarget::*method)(oo::Object*),
          oo::Object* argument, double delay) {
  oo::ScheduleDeferredCall(
      [target = oo::share(target), method,
       argument = oo::share(argument)]() {
        (target.get()->*method)(argument.get());
      },
      delay);
}

// The controller's state is private; GameController.h names this struct its
// friend so the checks below read and set the same members.
struct GameControllerTestAccess {
  static auto& _playerFileToLoad(GameController* controller) {
    return controller->_playerFileToLoad;
  }
  static auto& pauseSelector(GameController* controller) {
    return controller->pauseSelector;
  }
  static auto& pauseTarget(GameController* controller) {
    return controller->pauseTarget;
  }
  static auto& width(GameController* controller) { return controller->width; }
  static auto& height(GameController* controller) { return controller->height; }
  static auto& refresh(GameController* controller) {
    return controller->refresh;
  }
  static auto& fullscreenDisplayMode(GameController* controller) {
    return controller->fullscreenDisplayMode;
  }
  static auto& fullscreen(GameController* controller) {
    return controller->fullscreen;
  }
  static auto& stayInFullScreenMode(GameController* controller) {
    return controller->stayInFullScreenMode;
  }
};

namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;
TestPlayer* sPlayer = nullptr;

// The scratch home and game folder, the engine and the stand-in player, made
// once, before the controller exists.
void SetUp() {
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-gamecontroller-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  stdfs::create_directories(sRoot / "Resources");
  EXPECT_TRUE(
      oo::fs::writeFile(sRoot / "Resources" / "Info-gnustep.plist",
                        oo::Data("{ CFBundleVersion = \"9.9.9-test\"; }", 35),
                        oo::fs::WriteMode::kDirect)
          .has_value());
  (void)OOJavaScriptEngine::sharedEngine();
  sPlayer = NewTestPlayer<TestPlayer>();  // never released
  gOOPlayer = sPlayer;
}

bool SameId(OOJSPropID a, OOJSPropID b) { return a == b; }

oo::PList Mode(int width, int height, int refresh) {
  oo::PList::Dict mode;
  mode[std::string(oo::kDisplayWidth)] = oo::PList(std::int64_t(width));
  mode[std::string(oo::kDisplayHeight)] = oo::PList(std::int64_t(height));
  mode[std::string(oo::kDisplayRefreshRate)] = oo::PList(std::int64_t(refresh));
  return oo::PList(std::move(mode));
}

}  // namespace

TEST(GameController, sharedControllerIsOneObject) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    EXPECT_TRUE(controller != nullptr);
    EXPECT_TRUE(::GameController::sharedController() == controller);
    EXPECT_TRUE(!controller->finishedLaunching());
    EXPECT_TRUE(!controller->isGamePaused());
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenNoInteraction);
    EXPECT_TRUE(controller->gameView() == nullptr);
    EXPECT_TRUE(!controller->playerFileToLoad().has_value());
  }
}

// main.mm makes the application's controller with alloc/init, apart from
// +sharedController's: once the shared one exists, that raises (and releases
// the receiver).
TEST(GameController, secondControllerRaises) {
  ::SetUp();
  {
    GameController* shared = ::GameController::sharedController();
    bool raised = false;
    try {
      new GameController();
    } catch (const oo::Exception& exception) {
      raised =
          (std::string(exception.name()) == oo::InternalInconsistencyException);
    }
    EXPECT_TRUE(raised);
    EXPECT_TRUE(::GameController::sharedController() == shared);
  }
}

TEST(GameController, gameViewIsKeptAndToldOfTheController) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    TestView* first = oo::test::NewRetained<TestView>();
    TestView* second = oo::test::NewRetained<TestView>();

    controller->setGameView(first);
    EXPECT_TRUE(controller->gameView() == first);
    EXPECT_TRUE(first->_controller == controller);
    EXPECT_TRUE(first->weak_from_this().use_count() == 2);  // the controller retains its view

    controller->setGameView(second);
    EXPECT_TRUE(controller->gameView() == second);
    EXPECT_TRUE(second->_controller == controller);
    EXPECT_TRUE(first->weak_from_this().use_count() == 1);  // and releases the one it replaces
    EXPECT_TRUE(second->weak_from_this().use_count() == 2);

    controller->setGameView(first);
    oo::test::Release(second);
    oo::test::Release(first);
  }
}

TEST(GameController, mouseInteractionModes) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    TestView* view = static_cast<TestView*>(controller->gameView());
    EXPECT_TRUE(view != nullptr);
    view->_modeNotes.clear();

    controller->setMouseInteractionMode(oo::MouseInteractionMode::kFlightNoMouseControl);
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kFlightNoMouseControl);
    EXPECT_EQ(view->_modeNotes.size(), 1u);
    EXPECT_TRUE(view->_modeNotes.back() ==
             std::make_pair((int)oo::MouseInteractionMode::kUiScreenNoInteraction,
                            (int)oo::MouseInteractionMode::kFlightNoMouseControl));

    // The same mode again changes nothing and tells nobody.
    controller->setMouseInteractionMode(oo::MouseInteractionMode::kFlightNoMouseControl);
    EXPECT_EQ(view->_modeNotes.size(), 1u);

    controller->setMouseInteractionModeForUIWithMouseInteraction(true);
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenWithInteraction);
    controller->setMouseInteractionModeForUIWithMouseInteraction(false);
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenNoInteraction);

    // Flight: the player's mouse control chooses the mode.
    sPlayer->_mouseControlOn = true;
    controller->setMouseInteractionModeForFlight();
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kFlightWithMouseControl);
    sPlayer->_mouseControlOn = false;
    controller->setMouseInteractionModeForFlight();
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kFlightNoMouseControl);

    EXPECT_EQ(view->_modeNotes.size(), 5u);
    EXPECT_TRUE(view->_modeNotes.back() ==
             std::make_pair((int)oo::MouseInteractionMode::kFlightWithMouseControl,
                            (int)oo::MouseInteractionMode::kFlightNoMouseControl));
  }
}

TEST(GameController, pauseAndResume) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    controller->setMouseInteractionMode(oo::MouseInteractionMode::kFlightWithMouseControl);
    sPlayer->_events.clear();

    controller->setGamePaused(true);
    EXPECT_TRUE(controller->isGamePaused());
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenNoInteraction);
    EXPECT_EQ(sPlayer->_events.size(), 1u);
    EXPECT_TRUE(!sPlayer->_events.empty() &&
             SameId(sPlayer->_events.back(), OOJSID("gamePaused")));
#if OOLITE_WINDOWS
    // EcoQoS (no "ecoqos" default: on): a paused game runs at idle priority.
    EXPECT_TRUE(GetPriorityClass(GetCurrentProcess()) == IDLE_PRIORITY_CLASS);
#endif

    // Pausing a paused game does nothing.
    controller->setGamePaused(true);
    EXPECT_EQ(sPlayer->_events.size(), 1u);

    controller->setGamePaused(false);
    EXPECT_TRUE(!controller->isGamePaused());
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kFlightWithMouseControl);  // the mode it paused in
    EXPECT_EQ(sPlayer->_events.size(), 2u);
    EXPECT_TRUE(sPlayer->_events.size() == 2 &&
             SameId(sPlayer->_events.back(), OOJSID("gameResumed")));
#if OOLITE_WINDOWS
    EXPECT_TRUE(GetPriorityClass(GetCurrentProcess()) == NORMAL_PRIORITY_CLASS);
#endif

    // Resuming a running game does nothing.
    controller->setGamePaused(false);
    EXPECT_EQ(sPlayer->_events.size(), 2u);

#if OOLITE_WINDOWS
    // "ecoqos" off: EcoQoS leaves the priority alone.
    oo::Defaults::standard().setBool("ecoqos", false);
    controller->setEcoQoS(true);
    EXPECT_TRUE(GetPriorityClass(GetCurrentProcess()) == NORMAL_PRIORITY_CLASS);
    oo::Defaults::standard().removeObject("ecoqos");
#endif

#if OOLITE_WINDOWS && defined(OO_DEBUG_MONITOR_ENABLED)
    // A debug console connected: a paused game stays at normal priority, or on
    // a loaded machine it answers no console command for seconds.
    {
      WSADATA data;
      EXPECT_TRUE(WSAStartup(MAKEWORD(2, 2), &data) == 0);
      SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      sockaddr_in address = {};
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      address.sin_port = 0;  // a free port
      EXPECT_TRUE(bind(listener, reinterpret_cast<sockaddr*>(&address),
                    sizeof address) == 0);
      EXPECT_TRUE(listen(listener, 1) == 0);
      int size = sizeof address;
      EXPECT_TRUE(getsockname(listener, reinterpret_cast<sockaddr*>(&address),
                           &size) == 0);

      std::shared_ptr<oo::DebugTCPConsoleClient> console =
          oo::DebugTCPConsoleClient::clientWithAddress(
              std::string("127.0.0.1"), ntohs(address.sin_port));
      EXPECT_TRUE(console != nullptr);
      EXPECT_TRUE(oo::DebugTCPConsoleIsWaitingForInput());

      controller->setGamePaused(true);
      EXPECT_TRUE(GetPriorityClass(GetCurrentProcess()) == NORMAL_PRIORITY_CLASS);
      controller->setGamePaused(false);

      console.reset();
      closesocket(listener);
      WSACleanup();
    }
#endif
  }
}

// The full-screen category (SDL/GameController+SDLFullScreen.mm): still
// Objective-C, on the state the class keeps.
TEST(GameController, fullScreenDisplayModes) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    TestView* view = static_cast<TestView*>(controller->gameView());
    view->_screenModes = {Mode(320, 200, 60), Mode(1024, 768, 60),
                          Mode(1920, 1080, 60), Mode(1920, 1080, 144),
                          Mode(8000, 5000, 30)};
    view->_currentMode = Mode(1920, 1080, 60);
    controller->setUpDisplayModes();

    // The modes the game can use, in the screen's order.
    const oo::PList modes = controller->getDisplayModes();
    EXPECT_EQ(modes.count(), 3u);
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 1u);
    EXPECT_TRUE(controller->findDisplayModeForWidth(1920, 1080, 144) ==
             Mode(1920, 1080, 144));
    EXPECT_TRUE(controller->findDisplayModeForWidth(320, 200, 60).isNull());

    EXPECT_TRUE(controller->setDisplayWidth(1024, 768, 60));
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 0u);
    EXPECT_EQ(oo::Defaults::standard().integerForKey("display_width"), 1024);
    EXPECT_EQ(oo::Defaults::standard().integerForKey("display_height"), 768);
    EXPECT_TRUE(!controller->setDisplayWidth(800, 600, 60));
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 0u);

    view->_inFullScreen = true;
    EXPECT_TRUE(controller->inFullScreenMode());
    view->_inFullScreen = false;
    EXPECT_TRUE(!controller->inFullScreenMode());

    oo::Defaults::standard().setBool("fullscreen", true);
    controller->exitFullScreenMode();
    EXPECT_TRUE(!oo::Defaults::standard().boolForKey("fullscreen"));
  }
}

// The full-screen members: pausing full-screen mode records the call to make
// and leaves full screen.
TEST(GameController, fullScreenMembers) {
  ::SetUp();
  {
    GameController* controller = GameController::sharedController();
    TestView* view = static_cast<TestView*>(controller->gameView());
    view->_screenModes = {Mode(640, 480, 60), Mode(1280, 720, 60),
                          Mode(1280, 720, 75), Mode(9000, 9000, 60)};
    view->_currentMode = Mode(1280, 720, 75);
    controller->setUpDisplayModes();

    EXPECT_EQ(controller->getDisplayModes().count(), 3u);
    EXPECT_TRUE(controller->getDisplayModes() == controller->getDisplayModes());
    EXPECT_EQ(GameControllerTestAccess::width(controller), 1280u);
    EXPECT_EQ(GameControllerTestAccess::height(controller), 720u);
    EXPECT_EQ(GameControllerTestAccess::refresh(controller), 75u);
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 2u);
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 2u);
    EXPECT_TRUE(controller->findDisplayModeForWidth(640, 480, 60) ==
             Mode(640, 480, 60));
    EXPECT_TRUE(controller->findDisplayModeForWidth(9000, 9000, 60).isNull());

    EXPECT_TRUE(controller->setDisplayWidth(640, 480, 60));
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == 0u);
    EXPECT_TRUE(GameControllerTestAccess::fullscreenDisplayMode(controller) ==
             Mode(640, 480, 60));
    EXPECT_EQ(oo::Defaults::standard().integerForKey("display_refresh"), 60);
    EXPECT_TRUE(!controller->setDisplayWidth(1280, 720, 30));
    EXPECT_EQ(GameControllerTestAccess::width(controller), 640u);

    // No current mode: the screen's size, refresh unchanged; no match is
    // oo::kNotFound.
    view->_currentMode = oo::PList();
    controller->setUpDisplayModes();
    EXPECT_EQ(GameControllerTestAccess::width(controller), 0u);
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == oo::kNotFound);

    view->_inFullScreen = true;
    EXPECT_TRUE(controller->inFullScreenMode());
    view->_inFullScreen = false;
    EXPECT_TRUE(!controller->inFullScreenMode());

    controller->setFullScreenMode(true);
    EXPECT_TRUE(GameControllerTestAccess::fullscreen(controller));
    controller->setFullScreenMode(false);
    EXPECT_TRUE(!GameControllerTestAccess::fullscreen(controller));

    GameControllerTestAccess::stayInFullScreenMode(controller) = true;
    oo::Defaults::standard().setBool("fullscreen", true);
    controller->exitFullScreenMode();
    EXPECT_TRUE(!GameControllerTestAccess::stayInFullScreenMode(controller));
    EXPECT_TRUE(!oo::Defaults::standard().boolForKey("fullscreen"));

    TestTarget* target = oo::test::NewRetained<TestTarget>();
    GameControllerTestAccess::stayInFullScreenMode(controller) = true;
    controller->pauseFullScreenModeToPerform("record:", target);
    EXPECT_TRUE(GameControllerTestAccess::pauseSelector(controller) == "record:");
    EXPECT_TRUE(GameControllerTestAccess::pauseTarget(controller) == target);
    EXPECT_TRUE(!GameControllerTestAccess::stayInFullScreenMode(controller));
    controller->pauseFullScreenModeToPerform("raise:", nullptr);
    EXPECT_TRUE(GameControllerTestAccess::pauseSelector(controller) == "raise:");
    EXPECT_TRUE(GameControllerTestAccess::pauseTarget(controller) == nullptr);
    oo::test::Release(target);
  }
}

// The player-file paths.
TEST(GameController, playerFilePaths) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    controller->setPlayerFileToLoad("saves/Jameson.oolite-save");
    EXPECT_TRUE(controller->playerFileToLoad() ==
             std::optional<std::string>("saves/Jameson.oolite-save"));
    controller->setPlayerFileToLoad("saves/Jameson.txt");
    EXPECT_TRUE(!controller->playerFileToLoad().has_value());

    controller->setPlayerFileDirectory(
        std::optional<std::string>("C:/games/saves/Jameson.oolite-save"));
    EXPECT_TRUE(controller->playerFileDirectory() ==
             std::optional<std::string>("C:/games/saves"));
    EXPECT_TRUE(oo::Defaults::standard().stringForKey("save-directory") ==
             std::optional<std::string>("C:/games/saves"));
    controller->setPlayerFileDirectory(std::nullopt);
    EXPECT_TRUE(
        !oo::Defaults::standard().stringForKey("save-directory").has_value());
  }
}

// The deferred calls fire from -fireDueTimers, one per
// pass, in order; target and argument are kept until then; an OOException from
// the call is swallowed and the target stays retained (as the Foundation
// performer leaked it). The game tick's timer is never started here (it would
// tick the whole game).
TEST(GameController, deferredCalls) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    TestTarget* target = oo::test::NewRetained<TestTarget>();
    TestArgument* argument = oo::test::NewRetained<TestArgument>();

    Call(target, &TestTarget::record, argument, 0.0);
    Call(target, &TestTarget::record, nullptr, -1.0);
    EXPECT_TRUE(target->weak_from_this().use_count() == 3);
    EXPECT_TRUE(argument->weak_from_this().use_count() == 2);
    EXPECT_TRUE(target->_calls.empty());

    // Not due until 0.0001 s have passed.
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller->fireDueTimers();
    EXPECT_TRUE(target->_calls == std::vector<std::string>({"arg"}));
    EXPECT_TRUE(argument->weak_from_this().use_count() == 1);
    EXPECT_TRUE(target->weak_from_this().use_count() == 2);
    controller->fireDueTimers();
    EXPECT_TRUE(target->_calls == std::vector<std::string>({"arg", "nil"}));
    EXPECT_TRUE(target->weak_from_this().use_count() == 1);
    controller->fireDueTimers();  // nothing left
    EXPECT_TRUE(target->_calls.size() == 2);

    // A call not yet due stays.
    Call(target, &TestTarget::record, nullptr, 60.0);
    controller->fireDueTimers();
    EXPECT_TRUE(target->_calls.size() == 2);

    // An OOException from the call is swallowed; the target is not released.
    Call(target, &TestTarget::raise, nullptr, 0.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller->fireDueTimers();
    EXPECT_TRUE(target->_calls.size() == 3 && target->_calls.back() == "raise");
    EXPECT_TRUE(target->weak_from_this().use_count() ==
             3);  // the leaked +1 and the pending 60 s call's

    // A C++ deferred call shares the performs' queue -
    // due calls run one per pass in scheduling order, what the call captures is
    // kept until it has run, and an OOException from it is swallowed and the
    // call kept (its captures stay, as the performer leaked its target). (The
    // pending 60 s call above is not due.)
    TestTarget* cxxTarget = oo::test::NewRetained<TestTarget>();
    std::vector<std::string> order;
    std::shared_ptr<int> kept = std::make_shared<int>(1);

    oo::ScheduleDeferredCall([&order, kept]() { order.push_back("cxx"); }, 0.0);
    Call(cxxTarget, &TestTarget::record, nullptr, -1.0);
    oo::ScheduleDeferredCall(std::function<void()>(),
                           0.0);  // an empty call: nothing scheduled
    EXPECT_TRUE(kept.use_count() == 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller->fireDueTimers();
    EXPECT_TRUE(order == std::vector<std::string>({"cxx"}));
    EXPECT_TRUE(cxxTarget->_calls.empty());
    EXPECT_TRUE(kept.use_count() == 1);
    controller->fireDueTimers();
    EXPECT_TRUE(cxxTarget->_calls == std::vector<std::string>({"nil"}));
    controller->fireDueTimers();  // nothing due
    EXPECT_TRUE(order.size() == 1 && cxxTarget->_calls.size() == 1);

    std::shared_ptr<int> leaked = std::make_shared<int>(2);
    oo::ScheduleDeferredCall(
        [&order, leaked]() {
          order.push_back("raise");
          oo::RaiseException(oo::GenericException, "%s", "deferred call failure");
        },
        0.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller->fireDueTimers();
    EXPECT_TRUE(order.size() == 2 && order.back() == "raise");
    EXPECT_TRUE(leaked.use_count() == 2);  // the call was not destroyed

    oo::test::Release(cxxTarget);

    controller->stopAnimationTimer();
    oo::test::Release(argument);
  }
}

// The splash screen and window messages reach the
// view; with no universe starting up there is no progress line; with no player
// file nothing is loaded.
TEST(GameController, splashAndWindow) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    TestView* view = static_cast<TestView*>(controller->gameView());
    EXPECT_TRUE(view != nullptr);
    const int ends = view->_splashEnds, updates = view->_screenUpdates;

    controller->beginSplashScreen();  // a view exists: none is made
    EXPECT_TRUE(controller->gameView() == view);
    controller->endSplashScreen();
    EXPECT_TRUE(view->_splashEnds == ends + 1);
    controller->windowDidResize();
    EXPECT_TRUE(view->_screenUpdates == updates + 1);

    controller->logProgress("progress");  // no universe doing start-up: nothing
    controller->setPlayerFileToLoad("none.txt");
    controller->loadPlayerIfRequired();  // no player file: nothing
    EXPECT_TRUE(!controller->finishedLaunching());
  }
}

// With no save-directory default (or one that does not exist), the
// save directory is OO_SAVEDIR, made if missing.
TEST(GameController, saveDirectoryLookup) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    const stdfs::path saves =
        stdfs::temp_directory_path() /
        ("oo-test-gamecontroller-saves-" +
         std::to_string(static_cast<unsigned long>(::_getpid())));
    stdfs::remove_all(saves);
    EXPECT_TRUE(::_putenv_s("OO_SAVEDIR", saves.string().c_str()) == 0);

    controller->setPlayerFileDirectory(std::nullopt);
    const std::optional<std::string> found = controller->playerFileDirectory();
    EXPECT_TRUE(found.has_value() && stdfs::is_directory(saves) &&
             stdfs::equivalent(oo::fs::pathFromUTF8(*found), saves));
    EXPECT_TRUE(controller->playerFileDirectory() == found);  // looked up once

    oo::Defaults::standard().setObject(
        "save-directory", oo::PList(std::string("Z:/no/such/directory")));
    controller->setPlayerFileDirectory(std::nullopt);  // clears the default too
    EXPECT_TRUE(
        !oo::Defaults::standard().stringForKey("save-directory").has_value());
    oo::Defaults::standard().setObject(
        "save-directory", oo::PList(std::string("Z:/no/such/directory")));
    const std::optional<std::string> again = controller->playerFileDirectory();
    EXPECT_TRUE(again.has_value() &&
             stdfs::equivalent(oo::fs::pathFromUTF8(*again), saves));

    controller->setPlayerFileDirectory(std::nullopt);
    stdfs::remove_all(saves);
  }
}

// The C++ controller: one shared object, and the members telling the view of
// the controller and keeping the state they read.
TEST(GameController, cxxAPI) {
  ::SetUp();
  {
    GameController* controller = GameController::sharedController();
    EXPECT_TRUE(controller != nullptr);
    EXPECT_TRUE(GameController::sharedController() == controller);

    // The members tell the view of the controller.
    TestView* view = static_cast<TestView*>(controller->gameView());
    oo::test::Retain(view);  // only the controller holds it
    TestView* other = oo::test::NewRetained<TestView>();
    controller->setGameView(other);
    EXPECT_TRUE(other->_controller == controller);
    EXPECT_TRUE(controller->gameView() == other);
    controller->setGameView(view);
    EXPECT_TRUE(other->weak_from_this().use_count() == 1);
    oo::test::Release(other);
    oo::test::Release(view);

    controller->setMouseInteractionMode(oo::MouseInteractionMode::kUiScreenWithInteraction);
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenWithInteraction);
    controller->setMouseInteractionModeForUIWithMouseInteraction(false);
    EXPECT_TRUE(controller->mouseInteractionMode() ==
             oo::MouseInteractionMode::kUiScreenNoInteraction);

    // The state the members keep.
    controller->setPlayerFileToLoad("x.oolite-save");
    EXPECT_TRUE(GameControllerTestAccess::_playerFileToLoad(controller) ==
             std::optional<std::string>("x.oolite-save"));
    controller->setPlayerFileToLoad("x");
  }
}

// The file to load may be handed back from the member itself (the load does
// this): it is read in full before the member changes, kept or cleared as for
// any other name (found by ASan as a heap-use-after-free in the sanitized
// game's launch).
TEST(GameController, playerFileToLoadFromItsOwnValue) {
  ::SetUp();
  {
    GameController* controller = ::GameController::sharedController();
    controller->setPlayerFileToLoad(
        "saves/a-much-longer-name-than-fits-inline.oolite-save");
    controller->setPlayerFileToLoad(
        *GameControllerTestAccess::_playerFileToLoad(controller));
    EXPECT_TRUE(controller->playerFileToLoad() ==
             std::optional<std::string>(
                 "saves/a-much-longer-name-than-fits-inline.oolite-save"));
    controller->setPlayerFileToLoad("x");
    EXPECT_TRUE(!controller->playerFileToLoad().has_value());
  }
}
