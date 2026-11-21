/*

GameController.m

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

#include "GameController.h"

#include <stdlib.h>

#include "AsyncWorkManager.h"
#include "Foundation.h"
#include "LogOutputHandler.h"
#include "LoggingExtended.h"
#include "MyOpenGLView.h"
#include "OOJSFrameCallbacks.h"
#include "OXPVerifier.h"
#include "OXZManager.h"
#include "OpenALController.h"
#include "OpenGLExtensionManager.h"
#include "ResourceManager.h"
#include "SoundSource.h"
#include "Universe.h"
#include "legacy_random.h"
#ifdef OO_DEBUG_MONITOR_ENABLED
#include "DebugTCPConsoleClient.h"
#endif
#ifndef NDEBUG
#include "OOJavaScriptEngine.h"
#endif
#include <chrono>
#include <cstddef>  // IWYU pragma: keep: NULL, in the Windows code
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>  // IWYU pragma: keep (std::this_thread, off Windows)
#include <utility>
#include <vector>

#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Date.hpp"
#include "oofnd/Defaults.hpp"
#include "oofnd/Exception.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/Process.hpp"
#include "oofnd/ResourcePaths.hpp"
#include "oofnd/String.hpp"
#include "oofnd/Thread.hpp"
#if OOLITE_SDL
#include <SDL3/SDL_init.h>

#include "FullScreenController.h"
#endif

#if (OOLITE_SDL && defined(OO_DEBUG_MONITOR_ENABLED))
#include "DebugMonitor.h"
#endif

namespace {

GameController* sSharedController = nullptr;

}  // namespace

/*	The frame loop.

        The game tick was a repeating Foundation timer on the main run loop,
   which -applicationDidFinishLaunching: then ran for ever. It is now a deadline
   on std::chrono::steady_clock, created and advanced exactly as GNUstep created
        and advanced that timer's fire date:

        * start: first deadline = now + interval (an interval <= 0 is 0.0001 s);
        * fire when now >= deadline; before the tick runs, the next deadline is
          deadline + interval, plus as many further intervals as needed to pass
   the time sampled just before the tick (a late tick skips, it never bursts);
        * stop/start inside a tick (the save/load critical section) replaces the
          deadline, as a new timer did.

        Each pass of the loop fires what is due (the tick first, then the log
        flush, then up to two deferred calls), then runs the run loop once, up
   to the next deadline, for what still lives on it (OXZ downloads); while the
   debug console's socket is open, the wait is on that socket instead.
*/
namespace {
// Held as steady_clock tick counts, as OOLogOutputHandler's flush deadline is:
// a static time_point or duration has a constructor that may throw
// (bugprone-throwing-static-initialization).
using TickClock = std::chrono::steady_clock;
bool sGameTickScheduled = false;
TickClock::rep sNextGameTick = 0;      // ticks since the clock's epoch
TickClock::rep sGameTickInterval = 0;  // ticks

TickClock::time_point NextGameTick() {
  return TickClock::time_point(TickClock::duration(sNextGameTick));
}
}  // namespace

// namespace

/*	Deferred calls: Foundation's timed performers, which were one-shot
   timers on the main run loop. As measured against gnustep-base 1.31: a
   performer's fire date is now + delay (a delay <= 0 is 0.0001 s, the timer
   clamp); each run-loop pass fires the first due timer in the order the timers
   were added, twice (-runMode:beforeDate: asks -limitDateForMode:'s timer step
   once itself and once more before it waits), so due performers fire in
   scheduling order whatever their fire dates, at most two per pass; one
   scheduled while firing goes to the back. The performer retained target and
   argument and released them after the call; an exception from the call was
   logged by NSTimer and swallowed, and the performer was then never released
   (it leaked its target and argument). Anything else thrown propagated.
*/
namespace {

struct DeferredCall {
  double deadline;              // on the game's clock (DeferredCallNow)
  std::function<void()>* call;  // a C++ call
};

// "Now" for deferred calls: the game's clock, so a headless game's calls fall
// due in game time. Before the shared controller exists, the system clock,
// which is the same clock the windowed controller reads.
double DeferredCallNow() {
  return sSharedController != nullptr
             ? sSharedController->clock().monotonicSeconds()
             : oo::date::monotonicSeconds();
}

// call() with the perform's try/catch (below).
bool DeferredCallRunCatching(const std::function<void()>& call,
                             std::string* outName, std::string* outReason);

std::vector<DeferredCall> sDeferredCalls;  // in scheduling order

}  // namespace

// namespace
namespace oo {

void ScheduleDeferredCall(std::function<void()> call, double delay) {
  if (!oo::thread::isMainThread() || !call) return;

  if (delay <= 0.0) delay = 0.0001;  // as the Foundation timer did
  DeferredCall entry = {DeferredCallNow() + delay,
                        new std::function<void()>(std::move(call))};
  sDeferredCalls.push_back(entry);
}

}  // namespace oo

namespace {

// The Foundation timer's line for an exception from a deferred call.
void LogDeferredCallException(std::string_view exceptionName,
                              std::string_view exceptionReason,
                              const void* target) {
  // The game's own exceptions: the same line, name and reason bridged.
  OO_LOG("unclassified",
         "*** NSTimer ignoring exception '{}' (reason '{}') raised during "
         "posting of timer with target {} and selector 'fire'",
         exceptionName, exceptionReason, oo::str::pointerDescription(target));
}

// One step of the run loop's timer firing: the first due call in scheduling
// order.
void FireOneDueDeferredCall() {
  const double now = DeferredCallNow();
  for (std::vector<DeferredCall>::iterator it = sDeferredCalls.begin();
       it != sDeferredCalls.end(); ++it) {
    if (it->deadline <= now) {
      DeferredCall call = *it;
      sDeferredCalls.erase(it);

      // The call and its handler.
      std::string exceptionName, exceptionReason;
      if (!DeferredCallRunCatching(*call.call, &exceptionName,
                                   &exceptionReason)) {
        LogDeferredCallException(exceptionName, exceptionReason, call.call);
        return;  // the call stays, as the performer leaked its target
      }
      delete call.call;
      return;
    }
  }
}

// The earliest pending deferred call, if any: part of the run loop's wait
// limit.
bool NextDeferredCallDeadline(
    std::chrono::steady_clock::time_point* outDeadline) {
  bool found = false;
  double earliest = 0.0;
  for (const DeferredCall& call : sDeferredCalls) {
    if (!found || call.deadline < earliest) {
      earliest = call.deadline;
      found = true;
    }
  }
  if (found) {
    *outDeadline =
        std::chrono::steady_clock::now() +
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(earliest - DeferredCallNow()));
  }
  return found;
}

}  // namespace

const oo::GameClock& GameController::currentClock() {
  static const oo::GameClock systemClock;
  return sSharedController != nullptr ? sSharedController->_clock : systemClock;
}

oo::GameObserver* GameController::currentObserver() {
  return sSharedController != nullptr ? sSharedController->_observer : nullptr;
}

GameController* GameController::sharedController() {
  if (sSharedController == nullptr) {
    // never deleted
    static std::shared_ptr<GameController>* sSharedControllerHolder = nullptr;
    sSharedControllerHolder =
        new std::shared_ptr<GameController>(oo::makeShared<GameController>());
    sSharedController = sSharedControllerHolder->get();
  }
  return sSharedController;
}

GameController::GameController() {
  if (sSharedController != nullptr) {
    oo::RaiseException(
        oo::InternalInconsistencyException,
        "%s: expected only one GameController to exist at a time.",
        __PRETTY_FUNCTION__);
  }

  {
    _finishedLaunching = false;
    // the frame clock: intervals only (-doPerformGameTick)
    last_timeInterval = _clock.monotonicSeconds();
    delta_t = 0.01;  // one hundredth of a second
    {
      oo::Defaults& prefs = oo::Defaults::standard();
      _animationTimerInterval =
          prefs.object("animation_timer_interval").isNull()
              ? MINIMUM_ANIMATION_TICK
              : prefs.doubleForKey("animation_timer_interval");
    }

    // rather than seeding this with the date repeatedly, seed it
    // once here at startup
    // reset randomiser with current time
    ranrot_srand(static_cast<uint32_t>(oo::date::timeIntervalSince1970()));

    _splashStart = oo::date::monotonicSeconds();
  }
}

GameController::~GameController() = default;

bool GameController::isGamePaused() { return gameIsPaused; }

void GameController::setGamePaused(bool value) {
  if (value && !gameIsPaused) {
    _resumeMode = mouseInteractionMode();
    setMouseInteractionModeForUIWithMouseInteraction(false);
    setEcoQoS(true);
    gameIsPaused = true;
    if (PLAYER != nullptr) PLAYER->doScriptEvent(OOJSID("gamePaused"));
  } else if (!value && gameIsPaused) {
    setMouseInteractionMode(_resumeMode);
    setEcoQoS(false);
    gameIsPaused = false;
    if (PLAYER != nullptr) PLAYER->doScriptEvent(OOJSID("gameResumed"));
  }
}

void GameController::setEcoQoS(bool efficiencyModeRequested) {
#if OOLITE_WINDOWS
#ifdef OO_DEBUG_MONITOR_ENABLED
  /*	A paused game a debug console is driving is not idle.
     Efficiency mode puts the process in IDLE_PRIORITY_CLASS, which runs only
     when no other thread wants a CPU: on a loaded machine every frame - and so
     every console command, serviced between frames - then waited seconds per GL
     call (measured: 15-60 s a frame; 0.04 s once the class was put back to
     normal). Stay at normal priority while a console is connected.
  */
  if (efficiencyModeRequested && oo::DebugTCPConsoleIsWaitingForInput()) return;
#endif
  if (oo::Defaults::standard().object("ecoqos").isNull()
          ? true
          : oo::Defaults::standard().boolForKey("ecoqos")) {
    bool setEfficiencyMode = !!efficiencyModeRequested;  // yes or no, not 42
    HANDLE currentProcess = GetCurrentProcess();

    if (!SetPriorityClass(currentProcess, setEfficiencyMode
                                              ? IDLE_PRIORITY_CLASS
                                              : NORMAL_PRIORITY_CLASS))
        [[unlikely]] {
      OO_LOG("gameController.setEcoQos",
             "SetPriorityClass failed with error {}",
             static_cast<std::uint32_t>(GetLastError()));
    }

    PROCESS_POWER_THROTTLING_STATE powerThrottling;
    RtlZeroMemory(&powerThrottling, sizeof(powerThrottling));
    powerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    powerThrottling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
    powerThrottling.StateMask =
        setEfficiencyMode ? PROCESS_POWER_THROTTLING_EXECUTION_SPEED : 0;
    if (!SetProcessInformation(currentProcess, ProcessPowerThrottling,
                               &powerThrottling, sizeof(powerThrottling)))
        [[unlikely]] {
      OO_LOG("gameController.setEcoQos",
             "SetProcessInformation failed with error {}",
             static_cast<std::uint32_t>(GetLastError()));
    }
  }
#endif
}

oo::MouseInteractionMode GameController::mouseInteractionMode() {
  return _mouseMode;
}

void GameController::setMouseInteractionMode(oo::MouseInteractionMode mode) {
  oo::MouseInteractionMode oldMode = _mouseMode;
  if (mode == oldMode) return;

  _mouseMode = mode;
  OO_LOG("input.mouseMode.changed",
         "Mouse interaction mode changed from {} to {}",
         oo::StringFromMouseInteractionMode(oldMode),
         oo::StringFromMouseInteractionMode(mode));

  {
    if (gameView() != nullptr)
      gameView()->noteMouseInteractionModeChangedFrom(oldMode, mode);
  }
}

void GameController::setMouseInteractionModeForFlight() {
  setMouseInteractionMode(
      (PLAYER != nullptr ? PLAYER->isMouseControlOn() : false)
          ? oo::MouseInteractionMode::kFlightWithMouseControl
          : oo::MouseInteractionMode::kFlightNoMouseControl);
}

void GameController::setMouseInteractionModeForUIWithMouseInteraction(
    bool interaction) {
  setMouseInteractionMode(
      interaction ? oo::MouseInteractionMode::kUiScreenWithInteraction
                  : oo::MouseInteractionMode::kUiScreenNoInteraction);
}

::MyOpenGLView* GameController::gameView() { return _gameView.get(); }

void GameController::setGameView(::MyOpenGLView* view) {
  _gameView = oo::share(view);
  if (_gameView != nullptr) _gameView->setGameController(this);
  if (UNIVERSE != nullptr) UNIVERSE->setGameView(_gameView.get());
}

bool GameController::finishedLaunching() { return _finishedLaunching; }

#ifndef NDEBUG
/*	This method exists purely to suppress Clang static analyzer warnings
   that these ivars are unused (but may be used by categories, which they are).
*/
bool GameController::suppressClangStuff() {
  return !pauseSelector.empty() && pauseTarget;
}
#endif

void GameController::performGameTick() {
  oo::AutoreleaseScope pool;

  if (_gameView != nullptr) _gameView->pollControls();
  doPerformGameTick();
}

void GameController::tick(double dt) {
  _clock.advance(dt);  // the system clock ignores it
  // Headless, a step is a frame the game would have drawn.
  if (_options.headless) ++_presentation.framesRequested;
  if (UNIVERSE != nullptr) UNIVERSE->update(dt);
  if ((PLAYER != nullptr ? PLAYER->status() : oo::EntityStatus{}) ==
      oo::EntityStatus::kRestartGame) [[unlikely]] {
    if (UNIVERSE != nullptr) UNIVERSE->reinitAndShowDemo(true);
  }
  ::oo::Sound::update();
  if (!gameIsPaused) {
    OOJSFrameCallbacksInvoke(dt);
  }
}

void GameController::doPerformGameTick() {
  try {
    if (gameIsPaused) {
      delta_t = 0.0;  // no movement!
    } else {
      delta_t = _clock.monotonicSeconds() - last_timeInterval;
      last_timeInterval += delta_t;
      if (delta_t > MINIMUM_GAME_TICK)
        // peg the maximum pause (at 0.5->1.0 seconds) to protect against when
        // the machine sleeps
        delta_t = MINIMUM_GAME_TICK;
    }

    tick(delta_t);
  } catch (const oo::Exception& exception) {
    // was @catch (id): the game throws
    // only oo::Exception
    // -callStackSymbols is Foundation's; an OOException did not answer it
    // (sending it raised out of this handler), so name the exception instead.
    OO_LOG("exception.backtrace", "{} : {}", exception.name(),
           exception.reason());
  }

  try {
    if (_gameView != nullptr) _gameView->updateScreen();
  } catch (...) {
    if (!oo::CurrentOOException()) throw;
  }  // was @catch (id exception) {}: the game's exceptions only
}

void GameController::startAnimationTimer() {
  if (!sGameTickScheduled) {
    // default one two-hundredth of a second
    // (should be a fair bit faster than expected
    // frame rate ~60Hz to avoid problems with
    // phase differences)
    double ti = _animationTimerInterval;

    if (ti <= 0.0) ti = 0.0001;  // as the Foundation timer did

    sGameTickInterval = std::chrono::duration_cast<TickClock::duration>(
                            std::chrono::duration<double>(ti))
                            .count();
    sNextGameTick =
        TickClock::now().time_since_epoch().count() + sGameTickInterval;
    sGameTickScheduled = true;
  }
}

void GameController::stopAnimationTimer() { sGameTickScheduled = false; }

void GameController::performGameTickIfDue() {
  if (!sGameTickScheduled) return;

  const TickClock::rep now = TickClock::now().time_since_epoch().count();
  if (now < sNextGameTick) return;

  TickClock::rep next = sNextGameTick + sGameTickInterval;
  while (next <= now) next += sGameTickInterval;
  sNextGameTick = next;

  performGameTick();
}

void GameController::fireDueDeadlines() {
  performGameTickIfDue();
  oo::LogOutputHandlerFlushIfDue();
}

void GameController::fireDueDeferredCalls() {
  FireOneDueDeferredCall();
  FireOneDueDeferredCall();
}

void GameController::fireDueTimers() {
  fireDueDeadlines();
  FireOneDueDeferredCall();
}

void GameController::runFrameLoop() {
  for (;;) {
    {
      oo::AutoreleaseScope pool;  // was an autorelease-pool block
      fireDueDeadlines();
      fireDueDeferredCalls();

      // Wait for input until the tick or the next deferred call, whichever is
      // first.
      std::chrono::steady_clock::time_point wake;
      bool haveWake = NextDeferredCallDeadline(&wake);
      if (sGameTickScheduled && (!haveWake || NextGameTick() < wake)) {
        wake = NextGameTick();
        haveWake = true;
      }
      // The OXZ download's callbacks, which the run loop delivered.
      oo::OXZManager::sharedManager()->processDownloadEvents();

#ifdef OO_DEBUG_MONITOR_ENABLED
      if (oo::DebugTCPConsoleIsWaitingForInput()) {
        /*	The debug console's socket is not on the run loop: wait on the
           socket as the run loop waited on the console's streams, handling
           what arrives before the next deadline.
        */
        double timeout = -1.0;
        if (haveWake) {
          timeout = std::chrono::duration<double>(
                        wake - std::chrono::steady_clock::now())
                        .count();
          if (timeout < 0.0) timeout = 0.0;
        }
        oo::DebugTCPConsoleServiceInput(timeout);
      } else
#endif
      {
        /*	Nothing else to wait on, as there is no run-loop pump: wait
           until the next deadline, whole milliseconds rounded up as the run
           loop's wait was (not std::this_thread: on this toolchain it sleeps
           in 15.6 ms steps whatever the timer resolution, measured). With no
           deadline at all the pass ends at once, as the run loop returned at
           once with nothing to wait on.
        */
        if (haveWake) {
          const double remaining = std::chrono::duration<double>(
                                       wake - std::chrono::steady_clock::now())
                                       .count();
          if (remaining > 0.0) {
#if OOLITE_WINDOWS
            Sleep(static_cast<DWORD>(ceil(remaining * 1000.0)));
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<std::int64_t>(ceil(remaining * 1000.0))));
#endif
          }
        }
      }
    }
  }
}

void GameController::applicationDidFinishLaunching() {
  // An autorelease pool is pushed here and popped below.
  std::optional<oo::AutoreleaseScope> pool;
  pool.emplace();

  try {
    // if not verifying oxps, ensure that gameView is drawn to using
    // beginSplashScreen OpenGL is initialised and that allows textures to
    // initialise too.

#if OO_OXP_VERIFIER_ENABLED

    if (oo::OXPVerifier::runVerificationIfRequested()) {
      exitAppWithContext("OXP verifier run");
    } else {
      beginSplashScreen();
    }

#else
    beginSplashScreen();
#endif

#if OOLITE_SDL
    setUpDisplayModes();
#else
    [oo::ToObjC(this) setUpDisplayModes];
#endif

    // moved to before the Universe is created
    for (const std::string& expansionPath : expansionPathsToInclude) {
      ResourceManager::addExternalPath(expansionPath);
    }

    // initialise OXZ manager
    oo::OXZManager::sharedManager();

    // moved here to try to avoid initialising this before having an Open GL
    // context logProgress(OO_DESC("Initialising universe")); // DESC expansions
    // only possible after Universe init
    _universe = Universe::newUniverse(
        _gameView
            .get());  // also gSharedUniverse; destroyed with the controller

    loadPlayerIfRequired();

    logProgress("");

    // get the run loop and add the call to performGameTick:
    startAnimationTimer();

    endSplashScreen();
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    reportUnhandledStartupExceptionName(std::string(exception->name()),
                                        std::string(exception->reason()));
    exit(EXIT_FAILURE);
  }

  OO_LOG("startup.complete",
         "========== Loading complete in {:.2f} seconds. ==========",
         oo::date::monotonicSeconds() - _splashStart);

  _finishedLaunching = true;

  // Release anything allocated above that is not required.
  pool.reset();

  runFrameLoop();
}

namespace {
// The save a headless scenario loads, relative to the application directory.
constexpr const char* kHeadlessScenarioSave =
    "Resources/Scenarios/oolite-standard.oolite-save";
// How much game time a headless launch may take before it is an error.
constexpr double kHeadlessLaunchLimitSeconds = 60.0;
}  // namespace

void GameController::startHeadless(const oo::HeadlessOptions& options) {
  if (!options.headless) {
    oo::RaiseException(oo::InvalidArgumentException, "%s",
                       "startHeadless: options.headless is false");
  }
  options.validate();
  if (_universe != nullptr || UNIVERSE != nullptr) {
    oo::RaiseException(oo::InternalInconsistencyException, "%s",
                       "startHeadless: a universe already exists in this "
                       "process; a headless game is one per process");
  }

  _options = options;
  _clock = oo::GameClock::manual(*options.seed);
  last_timeInterval = _clock.monotonicSeconds();
  ranrot_srand(*options.seed);

  // No sound device: -nosound's path, so every sound call is a no-op.
  if (!oo::process::hasArgument("-nosound")) {
    std::vector<std::string> arguments = oo::process::arguments();
    arguments.emplace_back("-nosound");
    std::vector<const char*> argv;
    argv.reserve(arguments.size());
    for (const std::string& argument : arguments) {
      argv.push_back(argument.c_str());
    }
    oo::process::setArguments(static_cast<int>(argv.size()), argv.data());
  }

  oo::OpenGLExtensionManager::setHeadless(true);  // before its first use
  // The windowed start makes the script engine while the view sets up GL (the
  // extension manager's per-GPU look-up uses a regular expression, which needs
  // it); a headless start has no view, so it makes the engine here, at the
  // same point, before the universe.
  OOJSSetHeadlessScripting(options.seed, options.scriptTimeLimit);
  OOJSFrameCallbacksSetIDSeed(options.seed);
  OOJavaScriptEngine::sharedEngine();
  // Each sound asked for, with the game time, in the presentation record. With
  // no device no sound would load, so sounds are names without data.
  oo::Sound::setNamedWithoutDevice(true);
  oo::SoundSource::setPlayObserver([](oo::Sound* sound) {
    GameController* controller = sharedController();
    controller->_presentation.soundsRequested.push_back(
        oo::SoundRequest{UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0,
                         sound != nullptr ? sound->name().value_or("") : ""});
  });
  if (options.inputView) {
    _gameView = oo::makeShared<::MyOpenGLView>();
    if (!_gameView->initHeadless()) {
      _gameView.reset();
      oo::RaiseException(oo::InternalInconsistencyException, "%s",
                         "startHeadless: SDL could not start for the input "
                         "view");
    }
    _gameView->setGameController(this);
    // as the windowed start: the options screen lists them
    setUpDisplayModes();
  }
  _universe = Universe::newUniverse(_gameView.get(), options);
  loadPlayerIfRequired();

  if (options.startInFlight) {
    setPlayerFileToLoad(kHeadlessScenarioSave);
    loadPlayerIfRequired();
    if (PLAYER == nullptr || !PLAYER->isDocked()) {
      oo::RaiseException(oo::InternalInconsistencyException,
                         "startHeadless: the player is not docked after "
                         "loading %s",
                         kHeadlessScenarioSave);
    }
    PLAYER->launchFromStation();
    double launched = 0.0;
    while (PLAYER->status() != oo::EntityStatus::kInFlight) {
      if (launched > kHeadlessLaunchLimitSeconds) {
        oo::RaiseException(oo::InternalInconsistencyException,
                           "startHeadless: the player was not in flight after "
                           "%.0f s of game time",
                           kHeadlessLaunchLimitSeconds);
      }
      tick(options.step);
      fireDueDeferredCalls();
      launched += options.step;
    }
  }

  _finishedLaunching = true;
}

void GameController::loadPlayerIfRequired() {
  if (_playerFileToLoad.has_value()) {
    logProgress(OO_DESC("loading-player"));
    // fix problem with non-shader lighting when starting skips
    // the splash screen
    if (UNIVERSE != nullptr) UNIVERSE->useGUILightSource(true);
    if (UNIVERSE != nullptr) UNIVERSE->useGUILightSource(false);
    // A copy: loading resets the add-ons, whose demo screen resets
    // _playerFileToLoad, and a reference into it would dangle while the load
    // still reads it (an ASan heap-use-after-free).
    const std::string fileToLoad = *_playerFileToLoad;
    if (PLAYER != nullptr) PLAYER->loadPlayerFromFile(fileToLoad, false);
  }
}

void GameController::beginSplashScreen() {
  if (!_gameView) {
    /*	[[MyOpenGLView alloc] init], whose answer was not kept: a view whose
       init() fails (SDL could not start) was released, which left _gameView
       dangling; it is null now, and the sends below are skipped as messages to
       nil.
    */
    _gameView = oo::makeShared<::MyOpenGLView>();
    if (!_gameView->init()) _gameView.reset();
    if (_gameView != nullptr) _gameView->setGameController(this);
    if (_gameView != nullptr) _gameView->initSplashScreen();
  }
}

#if !OOLITE_SDL
#error Unknown environment!
#endif

void GameController::logProgress(const std::string& message) {
  if (!(UNIVERSE != nullptr ? UNIVERSE->doingStartUp() : false)) return;

  if (!message.empty()) {
    OO_LOG("startup.progress", "===== [{:.2f} s] {}",
           oo::date::monotonicSeconds() - _splashStart, message);
  }
}

#if OO_DEBUG

bool GameController::debugMessageTrackingIsOn() {
  return oo::log::willDisplay("startup.progress");
}

std::string GameController::debugMessageCurrentString() { return ""; }

void GameController::debugLogProgress(const std::string& message) {
  logProgress(message);
}

namespace {
std::vector<std::string> sMessageStack;
}

void GameController::debugPushProgressMessage(const std::string& message) {
  if (debugMessageTrackingIsOn()) {
    sMessageStack.push_back(debugMessageCurrentString());
    debugLogProgress(message);
  }

  oo::log::indentIf("startup.progress");
}

void GameController::debugPopProgressMessage() {
  oo::log::outdentIf("startup.progress");

  if (!sMessageStack.empty()) {
    const std::string message = sMessageStack.back();
    if (!message.empty()) logProgress(message);
    sMessageStack.pop_back();
  }
}

#endif

void GameController::endSplashScreen() {
  oo::log::logger().setDisplay("startup.progress", false);

#if OOLITE_SDL
  if (_gameView != nullptr) _gameView->endSplashScreen();
#endif
}

#if OOLITE_SDL
void GameController::exitAppWithContext(const std::string& context) {
  OO_LOG("exit.context", "Exiting: {}.", context);
  if (_options.headless) {
    _exitRequested = context;
    return;
  }
#if (OOLITE_SDL && defined(OO_DEBUG_MONITOR_ENABLED))
  ::oo::DebugMonitor::sharedDebugMonitor()->applicationWillTerminate();
#endif
#if OOLITE_WINDOWS
  // This should not be required normally but we have to ensure that
  // desktop resolution is restored also on some Intel cards on Win10
  if (!(_gameView != nullptr ? _gameView->getAtDesktopResolution() : false)) {
    OO_LOG("gameController.exitApp", "{}", "Restoring desktop resolution.");
    ChangeDisplaySettingsEx(nullptr, nullptr, nullptr, 0, nullptr);
  }
#endif
  shutDownForExit();
  oo::LoggingTerminate();
  SDL_Quit();
  if (oo::OpenALController* controller =
          oo::OpenALController::sharedController())
    controller->shutdown();
  exit(0);
}

void GameController::shutDownForExit() {
  // The work threads are owned by the never-destroyed manager, so they are
  // joined here, before anything they might use is torn down.
  oo::AsyncWorkManager::sharedAsyncWorkManager()->shutdown();
  oo::Defaults::standard().synchronize();
  OO_LOG("gameController.exitApp", "{}", ".GNUstepDefaults synchronized.");
#ifndef NDEBUG
  // Leak check: tears down the JavaScript runtime and logs any objects still
  // alive.
  OOJavaScriptEngine::sharedEngine()->shutDown();
#endif
}

#else
#error Unknown environment!
#endif

void GameController::exitAppCommandQ() { exitAppWithContext("Command-Q"); }

void GameController::windowDidResize() {
  if (_gameView != nullptr) _gameView->updateScreen();
}

std::optional<std::string> GameController::playerFileToLoad() {
  return _playerFileToLoad;
}

void GameController::setPlayerFileToLoad(const std::string& filename) {
  // filename may be *_playerFileToLoad itself (the load passes the file it is
  // loading back here), so it is read in full before the member changes.
  std::optional<std::string> toLoad;
  if (oo::str::lowercase(oo::str::pathExtension(filename)) == "oolite-save")
    toLoad = filename;
  _playerFileToLoad = std::move(toLoad);
}

std::optional<std::string> GameController::playerFileDirectory() {
  if (!_playerFileDirectory.has_value()) {
    // save-directory via oo::Defaults (the shared store).
    _playerFileDirectory =
        oo::Defaults::standard().stringForKey("save-directory");
    if (_playerFileDirectory.has_value() &&
        !oo::fs::fileExists(oo::fs::pathFromUTF8(*_playerFileDirectory))) {
      _playerFileDirectory = std::nullopt;
    }
    // -[defaultCommanderPath]: OO_SAVEDIR or ~/oolite-saves, create if missing,
    // else home.
    if (!_playerFileDirectory.has_value()) {
      const oo::ResourcePaths paths = oo::ResourcePaths::current();
      const oo::fs::Path savedir = paths.saveDirectory();
      const oo::fs::FileType type = oo::fs::fileType(savedir);
      if (type == oo::fs::FileType::kNone) {
        if (oo::fs::createDirectories(savedir)) {
          _playerFileDirectory = oo::fs::utf8String(savedir);
        } else {
          OO_LOG_ERR("savedGame.defaultPath.create.failed",
                     "Unable to create '{}'. Saved games will go to the home "
                     "directory.",
                     oo::fs::utf8String(savedir));
          _playerFileDirectory = oo::fs::utf8String(paths.homeDirectory());
        }
      } else if (type != oo::fs::FileType::kDirectory) {
        OO_LOG_ERR("savedGame.defaultPath.notDirectory",
                   "'{}' is not a directory, saved games will go to the home "
                   "directory.",
                   oo::fs::utf8String(savedir));
        _playerFileDirectory = oo::fs::utf8String(paths.homeDirectory());
      } else {
        _playerFileDirectory = oo::fs::utf8String(savedir);
      }
    }
  }

  return _playerFileDirectory;
}

void GameController::setPlayerFileDirectory(
    const std::optional<std::string>& filename) {
  std::optional<std::string> directory = filename;
  if (directory.has_value() &&
      oo::str::lowercase(oo::str::pathExtension(*directory)) == "oolite-save") {
    directory = oo::str::deletingLastPathComponent(*directory);
  }

  _playerFileDirectory = directory;
  if (directory.has_value())
    oo::Defaults::standard().setObject("save-directory", oo::PList(*directory));
  else
    oo::Defaults::standard().removeObject("save-directory");
}

void GameController::reportUnhandledStartupExceptionName(
    const std::string& name, const std::optional<std::string>& reason) {
  // %@ of a nil reason printed "(null)", as NSStringOrNil's nil still does.
  OO_LOG("startup.exception",
         "***** Unhandled exception during startup: {} ({}).", name,
         reason.value_or("(null)"));
}

namespace {

/*	The out-pointers below point into the exception's name and reason. The
   C++ oo::Exception dies with its handler, so a copy is kept here until the
   next one (main thread only).
*/
std::optional<oo::Exception> sLastDeferredCallException;

bool DeferredCallRunCatching(const std::function<void()>& call,
                             std::string* outName, std::string* outReason) {
  try {
    call();
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    sLastDeferredCallException = *exception->object();
    *outName = sLastDeferredCallException->name();
    *outReason = sLastDeferredCallException->reason();
    return false;
  }
  return true;
}

}  // namespace

// Full screen (SDL): the former FullScreen category of GameController.
#if OOLITE_SDL

namespace {
// The first of modes with this size and refresh rate, or nullptr (modes are
// mode dictionaries).
const oo::PList* FindDisplayMode(const oo::PList::Array& modes,
                                 unsigned int d_width, unsigned int d_height,
                                 unsigned int d_refresh) {
  for (const oo::PList& mode : modes) {
    unsigned int modeWidth =
        static_cast<unsigned int>(mode.get<int>(oo::kDisplayWidth));
    unsigned int modeHeight =
        static_cast<unsigned int>(mode.get<int>(oo::kDisplayHeight));
    unsigned int modeRefresh =
        static_cast<unsigned int>(mode.get<int>(oo::kDisplayRefreshRate));
    if ((modeWidth == d_width) && (modeHeight == d_height) &&
        (modeRefresh == d_refresh)) {
      return &mode;
    }
  }
  return nullptr;
}
}  // namespace

// The former FullScreen category of GameController: members of GameController,
// defined here. Their callers call them.

void GameController::setUpDisplayModes() {
  // The screen's mode dictionaries, in its order.
  const std::vector<oo::PList> modes =
      (_gameView != nullptr ? _gameView->getScreenSizeArray()
                            : std::vector<oo::PList>());

  displayModes.clear();
  for (const oo::PList& mode : modes) {
    unsigned int modeWidth =
        static_cast<unsigned int>(mode.get<int>(oo::kDisplayWidth));
    unsigned int modeHeight =
        static_cast<unsigned int>(mode.get<int>(oo::kDisplayHeight));

    if (modeWidth < DISPLAY_MIN_WIDTH || modeWidth > DISPLAY_MAX_WIDTH ||
        modeHeight < DISPLAY_MIN_HEIGHT || modeHeight > DISPLAY_MAX_HEIGHT)
      continue;
    displayModes.push_back(mode);
  }

  const oo::PList currentMode =
      (_gameView != nullptr ? _gameView->currentScreenMode() : oo::PList());
  if (currentMode) {
    width = static_cast<unsigned int>(currentMode.get<int>(oo::kDisplayWidth));
    height =
        static_cast<unsigned int>(currentMode.get<int>(oo::kDisplayHeight));
    refresh = static_cast<unsigned int>(
        currentMode.get<int>(oo::kDisplayRefreshRate));
  } else {
    NSSize fsmSize =
        (_gameView != nullptr ? _gameView->currentScreenSize() : NSSize{});
    width = static_cast<unsigned int>(fsmSize.width);
    height = static_cast<unsigned int>(fsmSize.height);
  }
}

void GameController::setFullScreenMode(bool fsm) { fullscreen = fsm; }

void GameController::exitFullScreenMode() {
  oo::Defaults::standard().setBool("fullscreen", false);
  stayInFullScreenMode = false;
}

bool GameController::inFullScreenMode() {
  return (_gameView != nullptr ? _gameView->inFullScreenMode() : false);
}

bool GameController::setDisplayWidth(unsigned int d_width,
                                     unsigned int d_height,
                                     unsigned int d_refresh) {
  const oo::PList* d_mode =
      FindDisplayMode(displayModes, d_width, d_height, d_refresh);
  if (d_mode != nullptr) {
    width = d_width;
    height = d_height;
    refresh = d_refresh;
    fullscreenDisplayMode = *d_mode;

    oo::Defaults& userDefaults = oo::Defaults::standard();

    userDefaults.setInteger("display_width", width);
    userDefaults.setInteger("display_height", height);
    userDefaults.setInteger("display_refresh", refresh);

    // Manual synchronization is required for SDL And doesn't hurt much for OS
    // X.
    userDefaults.synchronize();

    return true;
  }
  return false;
}

oo::PList GameController::findDisplayModeForWidth(unsigned int d_width,
                                                  unsigned int d_height,
                                                  unsigned int d_refresh) {
  const oo::PList* mode =
      FindDisplayMode(displayModes, d_width, d_height, d_refresh);
  return (mode != nullptr) ? *mode
                           : oo::PList();  // a mode dictionary; null: none
}

oo::PList GameController::getDisplayModes() {
  return oo::PList(displayModes);  // an array of mode dictionaries
}

std::size_t GameController::indexOfCurrentDisplayMode() {
  const oo::PList* mode = FindDisplayMode(displayModes, width, height, refresh);
  if (mode == nullptr)
    return oo::kNotFound;
  else
    // the first match, as -indexOfObject: found
    return static_cast<std::size_t>(mode - displayModes.data());

  return oo::kNotFound;
}

void GameController::pauseFullScreenModeToPerform(std::string_view selectorName,
                                                  oo::Object* target) {
  pauseSelector = selectorName;
  pauseTarget = target;
  stayInFullScreenMode = false;
}

#endif
