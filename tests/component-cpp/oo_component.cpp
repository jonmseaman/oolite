/*	tests/component-cpp/oo_component.cpp
        The C++ component tests' support library: see oo_component.hpp and
   specs/011-headless-component-tests/contracts/component-test-api.md.
*/

#include "oo_component.hpp"

#include <SDL3/SDL.h>
#include <quickjs.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif

#include "../unit/core/oo_gl_test_context.hpp"
#include "GameController.h"
#include "OOHeadless.h"
#include "OOJSEngineCore.h"
#include "OOLogOutputHandler.h"
#include "OOLoggingExtended.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oo_test.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "ShipEntity.h"
#include "oofnd/Exception.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/Process.hpp"

// main.cpp defines the debug flags, and a component program has its own main.
#ifdef OO_DEBUG_MONITOR_ENABLED
uint32_t gDebugFlags = 0;
#endif

namespace oo::component {

namespace {

namespace fs = std::filesystem;

// The windowed steps' set-up (tests/component/steps/world_steps.py), copied
// so the empty system means the same thing in both halves of the tier.
constexpr double kLaunchSettleSeconds = 2.0;  // "let the launch settle"
constexpr int kClearAttempts = 10;            // CLEAR_ATTEMPTS
constexpr double kClearRetrySeconds = 0.5;    // CLEAR_RETRY_SECONDS
constexpr std::string_view kClearNonPlayerShipsJs =  // CLEAR_NON_PLAYER_SHIPS_JS
    "(function(){"
    " var ships = system.allShips, n = 0;"
    " for (var i = 0; i < ships.length; i++) {"
    "   if (!ships[i].isPlayer) { ships[i].remove(); n++; }"
    " }"
    " return n; })()";

// How often, in real time, a stepping program says how far it got (FR-013):
// a program meson stops at its timeout then shows its last game time.
constexpr double kProgressIntervalSeconds = 10.0;

// The log classes whose lines are errors (data-model.md, Event record), and
// the prefix the game puts on an error line.
constexpr std::string_view kErrorClasses[] = {
    "shipData.load.error", "setup.ship.badEntry", "script.javaScript.exception",
    "exception", "equipment.load"};
constexpr std::string_view kErrorPrefix = "***** ERROR";
constexpr std::string_view kScriptErrorClass = "script.javaScript.exception";

// How many entries a failure prints.
constexpr std::size_t kFailureContextEntries = 10;

bool ClassIs(std::string_view messageClass, std::string_view listed) {
  return messageClass == listed ||
         (messageClass.size() > listed.size() &&
          messageClass.substr(0, listed.size()) == listed &&
          messageClass[listed.size()] == '.');
}

struct State {
  bool exists = false;
  EventRecord events;
  std::uint32_t seed = 0;
  std::uint64_t steps = 0;
  fs::path scratch;
  double startupSeconds = 0.0;
  std::chrono::steady_clock::time_point lastProgress;
};

State& TheState() {
  static State state;
  return state;
}

std::vector<std::string>& Arguments() {
  static std::vector<std::string> arguments;
  return arguments;
}

std::string& ProgramPath() {
  static std::string path;
  return path;
}

void SetEnvironment(const char* name, const fs::path& value) {
  const std::string utf8 = value.generic_string();
  SDL_setenv_unsafe(name, utf8.c_str(), 1);
}

std::optional<std::string> Environment(const char* name) {
  const char* value = SDL_getenv(name);
  if (value == nullptr || *value == '\0') return std::nullopt;
  return std::string(value);
}

std::string ProgramName() {
  return fs::path(ProgramPath()).stem().string();
}

long ProcessId() {
#if defined(_WIN32)
  return static_cast<long>(_getpid());
#else
  return static_cast<long>(getpid());
#endif
}

// <build>: OO_COMPONENT_BUILD_ROOT, or two levels above this program, which
// meson builds in <build>/tests/component-cpp.
fs::path BuildRoot() {
  if (std::optional<std::string> root = Environment("OO_COMPONENT_BUILD_ROOT")) {
    return fs::path(*root);
  }
  const char* base = SDL_GetBasePath();  // the program's directory
  return (fs::path(base != nullptr ? base : ".") / ".." / "..")
      .lexically_normal();
}

// The built application directory: Resources is read from the working
// directory, as the game reads it.
fs::path ApplicationDirectory(const fs::path& buildRoot) {
#if defined(__APPLE__)
  return buildRoot / "oolite.app" / "Contents";
#else
  return buildRoot / "oolite.app";
#endif
}

// The scratch directory, and every per-user directory pointed into it, before
// anything in the game reads one.
void PrepareProcess() {
  State& state = TheState();
  const fs::path buildRoot = BuildRoot();
  if (!Environment("OO_COMPONENT_BUILD_ROOT")) {
    // Run by hand: start where meson would have started the program.
    std::error_code ignored;
    fs::current_path(ApplicationDirectory(buildRoot), ignored);
  }
  state.scratch = buildRoot / "component" / "scratch" /
                  (ProgramName() + "-" + std::to_string(ProcessId()));
  std::error_code error;
  fs::remove_all(state.scratch, error);
  const fs::path home = state.scratch / "home";
  for (const char* sub : {"home", "saves", "logs", "snapshots", "managed-addons",
                          "extracted-addons", "additional-addons", "defaults"}) {
    fs::create_directories(state.scratch / sub, error);
  }
  SetEnvironment("HOME", home);
  SetEnvironment("HOMEPATH", home);
  SetEnvironment("OO_SAVEDIR", state.scratch / "saves");
  SetEnvironment("OO_LOGSDIR", state.scratch / "logs");
  SetEnvironment("OO_SNAPSHOTSDIR", state.scratch / "snapshots");
  SetEnvironment("OO_MANAGEDADDONSDIR", state.scratch / "managed-addons");
  SetEnvironment("OO_ADDONSEXTRACTDIR", state.scratch / "extracted-addons");
  SetEnvironment("OO_ADDITIONALADDONSDIRS", state.scratch / "additional-addons");
#if !defined(_WIN32)
  SetEnvironment("OO_GNUSTEPDEFAULTSDIR", state.scratch / "defaults");
#endif
  // The game sees its own name and -nosound, not the test's arguments.
  const char* gameArguments[] = {ProgramPath().c_str(), "-nosound",
                                 "-nosplash"};
  oo::process::setArguments(3, gameArguments);
}

const char* KindName(EventKind kind) {
  switch (kind) {
    case EventKind::kShipEvent:
      return "shipEvent";
    case EventKind::kWorldEvent:
      return "worldEvent";
    case EventKind::kLog:
      return "log";
    case EventKind::kScriptError:
      return "scriptError";
  }
  return "?";
}

void PrintEvent(const Event& event) {
  std::printf("       %9.3f %-11s %s", event.time, KindName(event.kind),
              event.name.c_str());
  if (event.subjectId != 0) {
    std::printf(" (ship %u %s)", static_cast<unsigned>(event.subjectId),
                event.subjectKey.c_str());
  }
  if (!event.text.empty()) std::printf(": %s", event.text.c_str());
  std::printf("%s\n", event.isError ? "  [error]" : "");
}

void PrintState(const char* file, int line) {
  const State& state = TheState();
  if (!state.exists) return;
  std::printf("     at %s:%d: seed %u, game time %.3f s, step %llu\n", file,
              line, static_cast<unsigned>(state.seed),
              UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0,
              static_cast<unsigned long long>(state.steps));
  const std::vector<Event>& entries = state.events.entries();
  const std::size_t from = entries.size() > kFailureContextEntries
                               ? entries.size() - kFailureContextEntries
                               : 0;
  if (from < entries.size()) std::printf("     the last events:\n");
  for (std::size_t i = from; i < entries.size(); ++i) PrintEvent(entries[i]);
  std::fflush(stdout);
}

double GameTimeNow() { return UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0; }

// Every log line: into the event record, then on to the log file as the
// game's own sink sends it.
void RecordLogLine(std::string_view line) {
  Event event;
  event.time = GameTimeNow();
  event.kind = EventKind::kLog;
  event.text = std::string(line);
  // "<time> [class]: text" (the class is shown in the game's log).
  const std::size_t open = line.find('[');
  const std::size_t close =
      open == std::string_view::npos ? open : line.find(']', open);
  if (close != std::string_view::npos) {
    event.name = std::string(line.substr(open + 1, close - open - 1));
  }
  for (std::string_view listed : kErrorClasses) {
    if (ClassIs(event.name, listed)) event.isError = true;
  }
  if (line.find(kErrorPrefix) != std::string_view::npos) event.isError = true;
  if (ClassIs(event.name, kScriptErrorClass)) {
    event.kind = EventKind::kScriptError;
    event.isError = true;
  }
  TheState().events.add(std::move(event));
  OOLogOutputHandlerPrintLine(line);
}

// The game's script events, as they are dispatched.
class Observer final : public OOGameObserver {
 public:
  void shipEvent(double time, ShipEntity* ship, std::string_view name) override {
    Event event;
    event.time = time;
    event.kind = EventKind::kShipEvent;
    event.name = std::string(name);
    if (ship != nullptr) {
      event.subjectId = static_cast<std::uint32_t>(ship->getUniversalID());
      event.subjectKey = ship->shipDataKey().value_or("");
    }
    TheState().events.add(std::move(event));
  }
  void worldEvent(double time, std::string_view name) override {
    Event event;
    event.time = time;
    event.kind = EventKind::kWorldEvent;
    event.name = std::string(name);
    TheState().events.add(std::move(event));
  }
};

Observer& TheObserver() {
  static Observer observer;
  return observer;
}

std::string ExceptionMessage(JSContext* context) {
  JSValue exception = JS_GetException(context);
  std::string message =
      OOStringFromJSValue(context, exception).value_or("(no message)");
  JS_FreeValue(context, exception);
  return message;
}

}  // namespace

const std::vector<std::string>& programArguments() { return Arguments(); }

bool EventRecord::has(std::string_view name, std::uint32_t subjectId) const {
  return firstOf(name, subjectId) != nullptr;
}

std::size_t EventRecord::count(std::string_view name) const {
  std::size_t n = 0;
  for (const Event& event : entries_) {
    if (event.name == name) ++n;
  }
  return n;
}

const Event* EventRecord::firstOf(std::string_view name,
                                  std::uint32_t subjectId) const {
  for (const Event& event : entries_) {
    if (event.name == name && (subjectId == 0 || event.subjectId == subjectId)) {
      return &event;
    }
  }
  return nullptr;
}

void checkNoErrors(const Game& game, const char* file, int line) {
  const std::vector<Event> errors = game.errors();
  if (errors.empty()) {
    ::oo_test::check(true, "no error in the event record", file, line);
    return;
  }
  ::oo_test::check(false, "no error in the event record", file, line);
  std::printf("     %zu error entries:\n", errors.size());
  for (const Event& event : errors) PrintEvent(event);
  PrintState(file, line);
}

void fail(std::string_view message, const char* file, int line) {
  const std::string text(message);
  ::oo_test::check(false, text.c_str(), file, line);
  PrintState(file, line);
}

Game Game::scenario(std::uint32_t seed) {
  State& state = TheState();
  if (state.exists) {
    fail("a second Game in one process: one game per process; use "
         "Game::current()",
         __FILE__, __LINE__);
    return Game();
  }
  const auto startedAt = std::chrono::steady_clock::now();
  PrepareProcess();
  OOLoggingInit();
  oo::log::logger().setSink(&RecordLogLine);
  GameController::sharedController()->setObserver(&TheObserver());

  // Until the game makes no GL call, a hidden GL window (spec 011 T021
  // removes it); test_boot_nogl is built without it.
#ifndef OO_COMPONENT_NO_GL_WINDOW
  if (!OOTestGLContext()) {
    fail("no OpenGL context for the hidden window", __FILE__, __LINE__);
  }
#endif

  OOHeadlessOptions options;
  options.headless = true;
  options.seed = seed;
  options.addOns = OOHeadlessOptions::AddOns::kNone;
  options.populate = false;
  options.startInFlight = true;
  options.step = kStepSeconds;
  options.scriptTimeLimit = kScriptTimeLimitSeconds;
  state.seed = seed;
  state.lastProgress = std::chrono::steady_clock::now();
  try {
    oo::AutoreleaseScope pool;
    GameController::sharedController()->startHeadless(options);
  } catch (const oo::Exception& exception) {
    fail(std::string("startHeadless raised ") + exception.name() + ": " +
             exception.reason(),
         __FILE__, __LINE__);
    std::fflush(stdout);
    std::exit(1);  // nothing else in the program can run without the game
  }
  state.exists = true;
  state.startupSeconds =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - startedAt)
          .count();

  // As the windowed step: let the launch settle, then clear the system down
  // to the player, repeating until one clear-then-count sees only the player.
  Game game;
  game.simulate(GameTime{kLaunchSettleSeconds});
  double remaining = 0.0;
  for (int attempt = 0; attempt < kClearAttempts; ++attempt) {
    if (attempt > 0) game.simulate(GameTime{kClearRetrySeconds});
    game.eval(kClearNonPlayerShipsJs);
    remaining = game.eval("system.allShips.length");
    if (remaining <= 1.0) break;
  }
  if (remaining > 1.0) {
    fail("the system could not be cleared down to the player", __FILE__,
         __LINE__);
  }
  return game;
}

Game Game::current() {
  if (!TheState().exists) {
    fail("Game::current(): no game was made in this process", __FILE__,
         __LINE__);
  }
  return Game();
}

void Game::step(int steps) {
  State& state = TheState();
  for (int i = 0; i < steps; ++i) {
    oo::AutoreleaseScope pool;  // as performGameTick has, per frame
    controller()->tick(kStepSeconds);
    controller()->fireDueDeferredCalls();  // as the frame loop's pass does
    ++state.steps;
    const auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<double>(now - state.lastProgress).count() >=
        kProgressIntervalSeconds) {
      state.lastProgress = now;
      std::fprintf(stderr, "%s: game time %.1f s\n", ProgramName().c_str(),
                   time());
      std::fflush(stderr);
    }
  }
}

void Game::simulate(GameTime span) {
  const int steps = static_cast<int>(std::ceil(span.count() / kStepSeconds - 1e-9));
  step(steps);
}

bool Game::simulateUntil(const std::function<bool()>& done, GameTime atMost) {
  const double deadline = time() + atMost.count();
  while (time() < deadline) {
    if (done()) return true;
    step();
  }
  return done();
}

double Game::time() const {
  return UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0;
}

Universe* Game::universe() const { return UNIVERSE; }

PlayerEntity* Game::player() const { return PlayerEntity::sharedPlayer(); }

GameController* Game::controller() const {
  return GameController::sharedController();
}

std::uint64_t Game::steps() const { return TheState().steps; }

const EventRecord& Game::events() const { return TheState().events; }

std::vector<Event> Game::errors() const { return errorsSince(0); }

std::vector<Event> Game::errorsSince(std::size_t mark) const {
  std::vector<Event> found;
  const std::vector<Event>& entries = TheState().events.entries();
  for (std::size_t i = mark; i < entries.size(); ++i) {
    if (entries[i].isError) found.push_back(entries[i]);
  }
  return found;
}

const OOPresentationRecord& Game::presentation() const {
  return controller()->presentation();
}

std::filesystem::path Game::scratchDirectory() { return TheState().scratch; }

double Game::startupSeconds() { return TheState().startupSeconds; }

namespace {

// Evaluates source as the debug console does, in the global scope, under the
// limiter; with(result) runs on a value that did not throw.
void Evaluate(std::string_view js,
              const std::function<void(JSContext*, JSValueConst)>& with) {
  const std::string source(js);  // JS_Eval needs the terminating NUL
  JSContext* context = OOJSAcquireContext();
  OOJSStartTimeLimiter();
  JSValue result = JS_Eval(context, source.c_str(), source.size(),
                           "<oo_component>", JS_EVAL_TYPE_GLOBAL);
  OOJSStopTimeLimiter();
  if (JS_IsException(result)) {
    fail("eval threw " + ExceptionMessage(context) + ": " + source, __FILE__,
         __LINE__);
  } else {
    with(context, result);
  }
  JS_FreeValue(context, result);
  OOJSRelinquishContext(context);
}

}  // namespace

bool Game::evalBool(std::string_view js) {
  bool value = false;
  Evaluate(js, [&value](JSContext* context, JSValueConst result) {
    value = JS_ToBool(context, result) > 0;
  });
  return value;
}

std::string Game::evalString(std::string_view js) {
  std::string value;
  Evaluate(js, [&value](JSContext* context, JSValueConst result) {
    value = OOStringFromJSValue(context, result).value_or("");
  });
  return value;
}

double Game::eval(std::string_view js) {
  const std::string source(js);  // JS_Eval needs the terminating NUL
  JSContext* context = OOJSAcquireContext();
  OOJSStartTimeLimiter();
  JSValue result = JS_Eval(context, source.c_str(), source.size(),
                           "<oo_component>", JS_EVAL_TYPE_GLOBAL);
  OOJSStopTimeLimiter();
  double value = std::numeric_limits<double>::quiet_NaN();
  if (JS_IsException(result)) {
    fail("eval threw " + ExceptionMessage(context) + ": " + source, __FILE__,
         __LINE__);
  } else if (JS_IsBool(result)) {
    value = JS_ToBool(context, result) > 0 ? 1.0 : 0.0;
  } else if (JS_ToFloat64(context, &value, result) < 0) {
    fail("eval's result is not a number: " + source, __FILE__, __LINE__);
  }
  JS_FreeValue(context, result);
  OOJSRelinquishContext(context);
  return value;
}

int runMain(int argc, char** argv, const char* file) {
  ProgramPath() = argc > 0 ? argv[0] : "component";
  for (int i = 1; i < argc; ++i) Arguments().emplace_back(argv[i]);
  const int result = ::oo_test::runAll(file);
  const State& state = TheState();
  if (!state.scratch.empty()) {
    if (result == 0) {
      OOLoggingTerminate();  // close the log, so its directory can go
      std::error_code error;
      fs::remove_all(state.scratch, error);
    } else {
      std::printf("scratch kept at %s\n", state.scratch.generic_string().c_str());
    }
  }
  std::fflush(stdout);
  std::fflush(stderr);
  return result;
}

}  // namespace oo::component
