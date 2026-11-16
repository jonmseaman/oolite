/*	tests/component-cpp/oo_component.cpp
        The C++ component tests' support library: see oo_component.hpp and
   specs/011-headless-component-tests/contracts/component-test-api.md.
*/

#include "oo_component.hpp"

#include <SDL3/SDL.h>
#include <quickjs.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "GameController.h"
#include "MyOpenGLView.h"
#include "OOHeadless.h"
#include "OOJSEngineCore.h"
#include "OOLogOutputHandler.h"
#include "OOLoggingExtended.h"
#include "PlayerEntity.h"
#include "PlayerEntityKeyMapper.h"
#include "oofnd/Defaults.hpp"
#include "GuiDisplayGen.h"
#include "Universe.h"
#include "oo_test.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "ShipEntity.h"
#include "StationEntity.h"
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
// A press: steps with the key down, then up (the game samples keys once a
// frame, and a key not seen up makes the next press a repeat).
constexpr int kPressSteps = 2;
constexpr int kGuiMaxRows = 64;  // a GUI screen has fewer rows than this
// The save Start New Commander's standard scenario loads (scenarios.plist),
// relative to the application directory.
constexpr const char* kStandardScenarioSave =
    "Resources/Scenarios/oolite-standard.oolite-save";

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

// The windowed steps' set-up for a role (world_steps.py): the pinned ship key
// (ROLE_SHIP_KEY), the AI (ROLE_AI), the pirate's bounty (PIRATE_BOUNTY) and
// the radius of an unpositioned spawn (SPAWN_RADIUS_M).
const std::map<std::string, std::string, std::less<>>& RoleShipKey() {
  static const std::map<std::string, std::string, std::less<>> keys = {
      {"police", "[viper]"},
      {"pirate", "[sidewinder]"},
      {"trader", "[boa]"},
      {"escort", "[sidewinder-escort]"}};
  return keys;
}

const std::map<std::string, std::string, std::less<>>& RoleAI() {
  static const std::map<std::string, std::string, std::less<>> ais = {
      {"police", "oolite-policeAI.js"},
      {"pirate", "oolite-pirateAI.js"},
      {"trader", "oolite-traderAI.js"},
      {"shuttle", "oolite-shuttleAI.js"}};
  return ais;
}

constexpr int kPirateBounty = 100;
constexpr int kSpawnRadiusMetres = 5000;
constexpr double kPollSeconds = 0.5;  // POLL_SECONDS

// A string as a JavaScript literal (world_steps.py _js_string).
std::string JsString(std::string_view value) {
  std::string quoted = "\"";
  for (char c : value) {
    if (c == '\\' || c == '"') quoted += '\\';
    quoted += c;
  }
  return quoted + "\"";
}

// FNV-1a, 64 bits: a digest that is the same on every run of one build.
class Fnv1a {
 public:
  void add(const void* data, std::size_t size) {
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
      hash_ ^= bytes[i];
      hash_ *= 0x100000001b3ULL;
    }
  }
  template <class T>
  void addValue(const T& value) {
    add(&value, sizeof value);
  }
  void addString(std::string_view text) {
    add(text.data(), text.size());
    addValue(static_cast<std::uint64_t>(text.size()));
  }
  std::uint64_t value() const { return hash_; }

 private:
  std::uint64_t hash_ = 0xcbf29ce484222325ULL;
};

struct State {
  bool exists = false;
  EventRecord events;
  Fnv1a eventsDigest;  // of every event so far, in order
  std::optional<std::ofstream> digestOut;  // --digest-out
  std::optional<std::array<double, 3>> locus;
  std::vector<std::string> spawnedRoles;  // in first-spawn order
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
  PlayerEntity* player = PlayerEntity::sharedPlayer();
  if (line > 0) {
    std::printf("     at %s:%d:", file, line);
  } else {
    std::printf("     after the case:");  // RunCases, for a plain OO_CHECK
  }
  std::printf(" seed %u, game time %.3f s, step %llu, screen %s\n",
              static_cast<unsigned>(state.seed),
              UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0,
              static_cast<unsigned long long>(state.steps),
              player != nullptr
                  ? oo::StringFromGUIScreenID(player->guiScreen()).c_str()
                  : "(no player)");
  const std::vector<Event>& entries = state.events.entries();
  const std::size_t from = entries.size() > kFailureContextEntries
                               ? entries.size() - kFailureContextEntries
                               : 0;
  if (from < entries.size()) std::printf("     the last events:\n");
  for (std::size_t i = from; i < entries.size(); ++i) PrintEvent(entries[i]);
  std::printf("     rerun: tests/run.sh component --cpp -k %s\n",
              ProgramName().c_str());
  std::fflush(stdout);
}

double GameTimeNow() { return UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0; }

// Into the record, and into the running digest of the record. A log line's
// text carries the wall-clock time, so only its class and error flag count.
void AddEvent(Event event) {
  State& state = TheState();
  Fnv1a& digest = state.eventsDigest;
  digest.addValue(event.time);
  digest.addValue(static_cast<int>(event.kind));
  digest.addString(event.name);
  digest.addValue(event.subjectId);
  digest.addString(event.subjectKey);
  digest.addValue(event.isError);
  state.events.add(std::move(event));
}

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
  AddEvent(std::move(event));
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
    AddEvent(std::move(event));
  }
  void worldEvent(double time, std::string_view name) override {
    Event event;
    event.time = time;
    event.kind = EventKind::kWorldEvent;
    event.name = std::string(name);
    AddEvent(std::move(event));
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

const std::string& programPath() { return ProgramPath(); }

bool hasArgument(std::string_view argument) {
  return std::find(Arguments().begin(), Arguments().end(), argument) !=
         Arguments().end();
}

std::string siblingProgram(std::string_view name) {
  fs::path path = fs::path(ProgramPath()).parent_path() / std::string(name);
#if defined(_WIN32)
  path += ".exe";
#endif
  return path.string();
}

int runChild(const std::string& program, const std::vector<std::string>& arguments,
             const std::string& outputFile, const std::string& errorFile) {
#if defined(_WIN32)
  const char* discard = "NUL";
#else
  const char* discard = "/dev/null";
#endif
  std::string command = "\"" + program + "\"";
  for (const std::string& argument : arguments) command += " \"" + argument + "\"";
  command += " > \"" + (outputFile.empty() ? std::string(discard) : outputFile) + "\"";
  command += " 2> \"" + (errorFile.empty() ? std::string(discard) : errorFile) + "\"";
#if defined(_WIN32)
  // cmd.exe /c drops the first and the last quote of a line that starts
  // with one; a pair round the whole line keeps the rest as written.
  command = "\"" + command + "\"";
#endif
  std::fflush(stdout);
  std::fflush(stderr);
  const int status = std::system(command.c_str());
#if defined(_WIN32)
  return status;
#else
  if (status == -1) return -1;
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  return 128 + (WIFSIGNALED(status) ? WTERMSIG(status) : 0);
#endif
}

std::string glTrapState() {
#if defined(OO_COMPONENT_GL_TRAP)
  return "GL trap: linked";
#elif defined(OO_COMPONENT_GL_TRAP_ABSENT)
  return std::string("GL trap: not linked (") + OO_COMPONENT_GL_TRAP_ABSENT + ")";
#else
  return "GL trap: not linked (unknown)";
#endif
}

ShipHandle::ShipHandle(const std::shared_ptr<ShipEntity>& ship) : ship_(ship) {
  if (ship != nullptr) {
    id_ = static_cast<std::uint32_t>(ship->getUniversalID());
    key_ = ship->shipDataKey().value_or("");
  }
}

bool ShipHandle::alive() const {
  const std::shared_ptr<ShipEntity> ship = ship_.lock();
  if (ship == nullptr || UNIVERSE == nullptr) return false;
  if (ship->status() == oo::EntityStatus::kDead) return false;
  return UNIVERSE->entityForUniversalID(
             static_cast<OOUniversalID>(id_)) == ship.get();
}

ShipEntity* ShipHandle::operator->() const {
  if (!alive()) {
    fail(std::format("ship {} ({}) is gone", id_, key_), __FILE__, __LINE__);
    std::fflush(stdout);
    std::exit(1);  // the call through it cannot go on
  }
  return ship_.lock().get();
}

ShipHandle::operator ShipEntity*() const {
  return alive() ? ship_.lock().get() : nullptr;
}

bool EventRecord::has(std::string_view name, std::uint32_t subjectId) const {
  return firstOf(name, subjectId) != nullptr;
}

bool EventRecord::has(std::string_view name, const ShipHandle& ship) const {
  return ship.id() != 0 && has(name, ship.id());
}

ShipWithKey ShipWithKey::at(HPVector position) const {
  ShipWithKey next = *this;
  next.placement_ = Placement::kAt;
  next.position_ = position;
  return next;
}

ShipWithKey ShipWithKey::aheadOf(Entity* entity, double metres) const {
  ShipWithKey next = *this;
  next.placement_ = Placement::kAheadOf;
  next.reference_ = entity;
  next.metres_ = metres;
  return next;
}

ShipWithKey ShipWithKey::near(Entity* entity, double metres) const {
  ShipWithKey next = *this;
  next.placement_ = Placement::kNear;
  next.reference_ = entity;
  next.metres_ = metres;
  return next;
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

namespace {

// The options every game here shares: headless, seeded, no add-ons, fixed
// step and script limit.
OOHeadlessOptions BaseOptions(std::uint32_t seed) {
  OOHeadlessOptions options;
  options.headless = true;
  options.seed = seed;
  options.addOns = OOHeadlessOptions::AddOns::kNone;
  options.step = kStepSeconds;
  options.scriptTimeLimit = kScriptTimeLimitSeconds;
  return options;
}

}  // namespace

// Starts the one game of the process; false (the test failed) if one exists.
namespace {

// The game's key code for a key as press() names it (the inverse of
// keyForControl): a character's code, or a named key's game code.
std::optional<int> GameKeyCode(std::string_view key) {
  struct Named {
    const char* name;
    int code;
  };
  static constexpr Named kNames[] = {
      {"enter", 13},          {"escape", 27},         {"space", 32},
      {"tab", 9},             {"up", gvArrowKeyUp},   {"down", gvArrowKeyDown},
      {"left", gvArrowKeyLeft}, {"right", gvArrowKeyRight}, {"home", gvHomeKey},
      {"end", gvEndKey},      {"insert", gvInsertKey}, {"delete", gvDeleteKey},
      {"pageup", gvPageUpKey}, {"pagedown", gvPageDownKey},
      {"backspace", gvBackspaceKey},
  };
  for (const Named& named : kNames) {
    if (key == named.name) return named.code;
  }
  if (key.size() >= 2 && key[0] == 'f') {
    const int n = std::atoi(std::string(key.substr(1)).c_str());
    if (n >= 1 && n <= 11) return gvFunctionKey1 + n - 1;
  }
  if (key.size() == 1 && key[0] > 32 && key[0] < 127) return key[0];
  return std::nullopt;
}

void WriteSettings(const std::vector<std::pair<std::string, std::string>>& remaps,
                   const std::optional<std::string>& keyboardCode) {
  oo::Defaults& defaults = oo::Defaults::standard();
  if (!remaps.empty()) {
    const oo::PList existing = defaults.object(std::string(KEYCONFIG_OVERRIDES));
    const oo::PList::Dict* dict = existing.getIf<oo::PList::Dict>();
    oo::PList::Dict overrides = dict != nullptr ? *dict : oo::PList::Dict();
    for (const auto& [control, key] : remaps) {
      const std::optional<int> code = GameKeyCode(key);
      if (!code.has_value()) {
        fail(std::format("remapKey: {} is not a key press() can name", key),
             __FILE__, __LINE__);
        continue;
      }
      oo::PList::Dict definition;
      definition["key"] = oo::PList::unsignedInteger(static_cast<unsigned>(*code));
      definition["shift"] = oo::PList(false);
      definition["mod1"] = oo::PList(false);
      definition["mod2"] = oo::PList(false);
      oo::PList::Array list;
      list.push_back(oo::PList(std::move(definition)));
      overrides[control] = oo::PList(std::move(list));
    }
    defaults.setObject(std::string(KEYCONFIG_OVERRIDES), oo::PList(std::move(overrides)));
  }
  if (keyboardCode.has_value()) {
    defaults.setObject("keyboard-code", oo::PList(*keyboardCode));
  }
  defaults.synchronize();
}

}  // namespace

Settings& Settings::remapKey(std::string_view control, std::string_view key) {
  remaps_.emplace_back(std::string(control), std::string(key));
  return *this;
}

Settings& Settings::keyboardCode(std::string_view code) {
  keyboardCode_ = std::string(code);
  return *this;
}

bool Game::start(const OOHeadlessOptions& options, const Settings& settings) {
  State& state = TheState();
  if (state.exists) {
    fail("a second Game in one process: one game per process; use "
         "Game::current()",
         __FILE__, __LINE__);
    return false;
  }
  const auto startedAt = std::chrono::steady_clock::now();
  PrepareProcess();
  OOLoggingInit();
  oo::log::logger().setSink(&RecordLogLine);
  GameController::sharedController()->setObserver(&TheObserver());
  WriteSettings(settings.remaps_, settings.keyboardCode_);

  const std::uint32_t seed = *options.seed;
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
  for (std::size_t i = 0; i + 1 < Arguments().size(); ++i) {
    if (Arguments()[i] == "--digest-out") {
      state.digestOut.emplace(Arguments()[i + 1], std::ios::out | std::ios::trunc);
      if (!*state.digestOut) {
        fail("cannot write " + Arguments()[i + 1], __FILE__, __LINE__);
      }
    }
  }
  state.startupSeconds =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - startedAt)
          .count();
  return true;
}

Game Game::atStartScreen(std::uint32_t seed) {
  OOHeadlessOptions options = BaseOptions(seed);
  options.populate = true;
  options.startInFlight = false;
  options.inputView = true;
  if (!start(options)) return Game();
  Game game;
  for (std::size_t i = 0; i + 1 < Arguments().size(); ++i) {
    if (Arguments()[i] == "--layout") game.useLayout(Arguments()[i + 1]);
  }
  return game;
}

std::string Game::screen() const {
  PlayerEntity* player = PlayerEntity::sharedPlayer();
  return player != nullptr ? oo::StringFromGUIScreenID(player->guiScreen())
                           : "(no player)";
}

MyOpenGLView* Game::view() const { return controller()->gameView(); }

// ----- Keys --------------------------------------------------------------

namespace {

struct NamedKey {
  const char* name;
  SDL_Scancode scancode;
};

constexpr NamedKey kNamedKeys[] = {
    {"enter", SDL_SCANCODE_RETURN},    {"escape", SDL_SCANCODE_ESCAPE},
    {"space", SDL_SCANCODE_SPACE},     {"tab", SDL_SCANCODE_TAB},
    {"backspace", SDL_SCANCODE_BACKSPACE}, {"delete", SDL_SCANCODE_DELETE},
    {"insert", SDL_SCANCODE_INSERT},   {"home", SDL_SCANCODE_HOME},
    {"end", SDL_SCANCODE_END},         {"pageup", SDL_SCANCODE_PAGEUP},
    {"pagedown", SDL_SCANCODE_PAGEDOWN}, {"up", SDL_SCANCODE_UP},
    {"down", SDL_SCANCODE_DOWN},       {"left", SDL_SCANCODE_LEFT},
    {"right", SDL_SCANCODE_RIGHT},     {"f1", SDL_SCANCODE_F1},
    {"f2", SDL_SCANCODE_F2},           {"f3", SDL_SCANCODE_F3},
    {"f4", SDL_SCANCODE_F4},           {"f5", SDL_SCANCODE_F5},
    {"f6", SDL_SCANCODE_F6},           {"f7", SDL_SCANCODE_F7},
    {"f8", SDL_SCANCODE_F8},           {"f9", SDL_SCANCODE_F9},
    {"f10", SDL_SCANCODE_F10},         {"f11", SDL_SCANCODE_F11},
    {"f12", SDL_SCANCODE_F12},
};

constexpr const char* kModsNames[] = {"-", "shift", "caps", "shift+caps", "altgr"};
constexpr std::uint16_t kModsStates[] = {
    SDL_KMOD_NONE, SDL_KMOD_LSHIFT, SDL_KMOD_CAPS,
    static_cast<std::uint16_t>(SDL_KMOD_LSHIFT | SDL_KMOD_CAPS), SDL_KMOD_MODE};

// The one code point of a UTF-8 string, or nullopt if it is not one.
std::optional<std::uint32_t> SingleCodePoint(std::string_view text) {
  if (text.empty()) return std::nullopt;
  const auto* bytes = reinterpret_cast<const unsigned char*>(text.data());
  std::uint32_t code = 0;
  std::size_t length = 0;
  if (bytes[0] < 0x80) {
    code = bytes[0];
    length = 1;
  } else if ((bytes[0] & 0xE0) == 0xC0) {
    code = bytes[0] & 0x1F;
    length = 2;
  } else if ((bytes[0] & 0xF0) == 0xE0) {
    code = bytes[0] & 0x0F;
    length = 3;
  } else if ((bytes[0] & 0xF8) == 0xF0) {
    code = bytes[0] & 0x07;
    length = 4;
  } else {
    return std::nullopt;
  }
  if (text.size() != length) return std::nullopt;
  for (std::size_t i = 1; i < length; ++i) {
    if ((bytes[i] & 0xC0) != 0x80) return std::nullopt;
    code = (code << 6) | (bytes[i] & 0x3F);
  }
  return code;
}

std::string ModsText(std::uint16_t mods) {
  const int cls = Layout::modsClass(mods);
  return cls == 0 ? "no modifiers" : kModsNames[cls];
}

struct Stroke {
  int scancode = SDL_SCANCODE_UNKNOWN;
  std::uint16_t mods = SDL_KMOD_NONE;
};

std::optional<Layout>& TheLayout() {
  static std::optional<Layout> layout;
  return layout;
}

std::optional<Stroke> StrokeFor(std::string_view key) {
  for (const NamedKey& named : kNamedKeys) {
    if (key == named.name) return Stroke{named.scancode, SDL_KMOD_NONE};
  }
  const std::optional<std::uint32_t> code = SingleCodePoint(key);
  if (!code.has_value()) return std::nullopt;
  if (TheLayout().has_value()) {
    const auto stroke = TheLayout()->strokeFor(*code);
    if (stroke.has_value()) return Stroke{stroke->first, stroke->second};
    // No key of this layout gives the character (n on a Russian board): a
    // player presses the key in its US position, the one the key settings'
    // letter is printed on, and the layout says what that key gives.
  }
  SDL_Keymod mods = SDL_KMOD_NONE;
  const SDL_Scancode scancode = SDL_GetScancodeFromKey(*code, &mods);
  if (scancode == SDL_SCANCODE_UNKNOWN) return std::nullopt;
  return Stroke{scancode, static_cast<std::uint16_t>(mods)};
}

// What a real event would carry in its key field for this layout.
std::uint32_t EventKeyFor(int scancode, std::uint16_t mods) {
  if (TheLayout().has_value()) return TheLayout()->keyFor(scancode, mods);
  return SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode),
                                static_cast<SDL_Keymod>(mods), true);
}

void PushKey(bool down, int scancode, std::uint16_t mods) {
  SDL_Event event{};
  event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
  event.key.timestamp = SDL_GetTicksNS();
  event.key.scancode = static_cast<SDL_Scancode>(scancode);
  event.key.key = EventKeyFor(scancode, mods);
  event.key.mod = static_cast<SDL_Keymod>(mods);
  event.key.down = down;
  if (!SDL_PushEvent(&event)) {
    fail(std::string("SDL_PushEvent failed: ") + SDL_GetError(), __FILE__,
         __LINE__);
  }
}

GuiDisplayGen* ShowingGui() {
  return UNIVERSE != nullptr ? UNIVERSE->getGui() : nullptr;
}

std::string Trimmed(const std::string& text) {
  const std::size_t first = text.find_first_not_of(' ');
  if (first == std::string::npos) return std::string();
  return text.substr(first, text.find_last_not_of(' ') - first + 1);
}

std::string RowText(GuiDisplayGen* gui, OOGUIRow row) {
  if (gui == nullptr || row < 0) return std::string();
  const oo::PList text = gui->objectForRow(row);
  if (const std::string* string = text.getIf<std::string>()) return Trimmed(*string);
  if (text.isArray()) {
    const oo::PList* first = text.at(0);
    if (first != nullptr) {
      if (const std::string* string = first->getIf<std::string>()) {
        return Trimmed(*string);
      }
    }
  }
  return std::string();
}

}  // namespace

int Layout::modsClass(std::uint16_t mods) {
  if ((mods & SDL_KMOD_MODE) != 0) return 4;
  const bool shift = (mods & SDL_KMOD_SHIFT) != 0;
  const bool caps = (mods & SDL_KMOD_CAPS) != 0;
  return shift && caps ? 3 : caps ? 2 : shift ? 1 : 0;
}

Layout Layout::load(std::string_view nameOrPath) {
  Layout layout;
  fs::path path(std::string{nameOrPath});
  if (path.extension() != ".tsv") {
    path = Game::sourceRoot() / "tests" / "component-cpp" / "layouts" /
           (std::string(nameOrPath) + ".tsv");
  } else if (path.is_relative() && !fs::exists(path)) {
    path = Game::sourceRoot() / path;  // the program runs in the application directory
  }
  layout.name_ = path.stem().string();
  std::ifstream in(path);
  if (!in) {
    fail("cannot read layout " + path.generic_string(), __FILE__, __LINE__);
    return layout;
  }
  std::string line;
  int number = 0;
  while (std::getline(in, line)) {
    ++number;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    const std::size_t hash = line.find('#');
    const std::string body = line.substr(0, hash);
    std::istringstream fields(body);
    std::string scancode, mods, keycode;
    if (!(fields >> scancode)) continue;  // blank or comment
    if (!(fields >> mods >> keycode)) {
      fail(std::format("{}:{}: a row needs scancode, mods and keycode",
                       path.generic_string(), number),
           __FILE__, __LINE__);
      continue;
    }
    int cls = -1;
    for (int i = 0; i < 5; ++i) {
      if (mods == kModsNames[i]) cls = i;
    }
    if (cls < 0) {
      fail(std::format("{}:{}: unknown mods \"{}\"", path.generic_string(),
                       number, mods),
           __FILE__, __LINE__);
      continue;
    }
    try {
      layout.rows_[{std::stoi(scancode), cls}] =
          static_cast<std::uint32_t>(std::stoul(keycode, nullptr, 16));
    } catch (const std::exception&) {
      fail(std::format("{}:{}: bad number", path.generic_string(), number),
           __FILE__, __LINE__);
    }
  }
  return layout;
}

std::uint32_t Layout::keyFor(int scancode, std::uint16_t mods) const {
  const auto row = rows_.find({scancode, modsClass(mods)});
  return row != rows_.end() ? row->second : 0;
}

std::optional<std::pair<int, std::uint16_t>> Layout::strokeFor(
    std::uint32_t code) const {
  for (int cls = 0; cls < 5; ++cls) {
    for (const auto& [where, keycode] : rows_) {
      if (where.second == cls && keycode == code) {
        return std::pair<int, std::uint16_t>{where.first, kModsStates[cls]};
      }
    }
  }
  return std::nullopt;
}

void Game::useLayout(std::string_view nameOrPath) {
  TheLayout() = Layout::load(nameOrPath);
  if (view() == nullptr) {
    fail("useLayout: this game has no view to take key events", __FILE__,
         __LINE__);
    return;
  }
  const Layout* layout = &*TheLayout();
  view()->keyLookup = [layout](SDL_Scancode scancode, SDL_Keymod mod) {
    return static_cast<SDL_Keycode>(layout->keyFor(scancode, mod));
  };
}

std::string Game::layoutName() const {
  return TheLayout().has_value() ? TheLayout()->name() : std::string("SDL");
}

void Game::keyDownScancode(int scancode, std::uint16_t mods) {
  if (view() == nullptr) {
    fail("a key press in a game with no view (use atStartScreen, docked or "
         "inFlightWithKeys)",
         __FILE__, __LINE__);
    return;
  }
  PushKey(true, scancode, mods);
}

void Game::keyUpScancode(int scancode, std::uint16_t mods) {
  if (view() == nullptr) return;
  PushKey(false, scancode, mods);
}

void Game::pressScancode(int scancode, std::uint16_t mods) {
  keyDownScancode(scancode, mods);
  step(kPressSteps);
  keyUpScancode(scancode, SDL_KMOD_NONE);
  step(kPressSteps);
}

void Game::press(std::string_view key) {
  const std::optional<Stroke> stroke = StrokeFor(key);
  if (!stroke.has_value()) {
    fail(std::format("no key gives \"{}\" in layout {}", key, layoutName()),
         __FILE__, __LINE__);
    return;
  }
  pressScancode(stroke->scancode, stroke->mods);
}

bool Game::hold(std::string_view key, const std::function<bool()>& done,
                GameTime atMost) {
  const std::optional<Stroke> stroke = StrokeFor(key);
  if (!stroke.has_value()) {
    fail(std::format("no key gives \"{}\" in layout {}", key, layoutName()),
         __FILE__, __LINE__);
    return false;
  }
  keyDownScancode(stroke->scancode, stroke->mods);
  const bool reached = simulateUntil(done, atMost);
  keyUpScancode(stroke->scancode, SDL_KMOD_NONE);
  step(kPressSteps);
  return reached;
}

void Game::type(std::string_view text) {
  std::size_t i = 0;
  while (i < text.size()) {
    std::size_t length = 1;
    const auto lead = static_cast<unsigned char>(text[i]);
    if (lead >= 0xF0) {
      length = 4;
    } else if (lead >= 0xE0) {
      length = 3;
    } else if (lead >= 0xC0) {
      length = 2;
    }
    const std::string_view character = text.substr(i, length);
    press(character == " " ? std::string_view("space") : character);
    i += length;
  }
}

std::string Game::keyForControl(std::string_view control) const {
  PlayerEntity* player = PlayerEntity::sharedPlayer();
  const oo::PList config = player != nullptr ? player->keyConfig() : oo::PList();
  const oo::PList* definitions = config.find(std::string(control));
  const oo::PList* first =
      definitions != nullptr && definitions->isArray() ? definitions->at(0) : nullptr;
  if (first == nullptr) {
    fail(std::format("the key settings have no {}", control), __FILE__, __LINE__);
    return std::string();
  }
  const int code = first->get<int>("key");
  struct Named {
    int code;
    const char* name;
  };
  static constexpr Named kGameKeys[] = {
      {13, "enter"},          {27, "escape"},         {32, "space"},
      {9, "tab"},             {gvArrowKeyUp, "up"},   {gvArrowKeyDown, "down"},
      {gvArrowKeyLeft, "left"}, {gvArrowKeyRight, "right"}, {gvHomeKey, "home"},
      {gvEndKey, "end"},      {gvInsertKey, "insert"}, {gvDeleteKey, "delete"},
      {gvPageUpKey, "pageup"}, {gvPageDownKey, "pagedown"},
      {gvBackspaceKey, "backspace"},
  };
  for (const Named& named : kGameKeys) {
    if (code == named.code) return named.name;
  }
  if (code >= gvFunctionKey1 && code < gvFunctionKey1 + 11) {
    return std::format("f{}", code - gvFunctionKey1 + 1);
  }
  if (code > 32 && code < 127) return std::string(1, static_cast<char>(code));
  fail(std::format("{} is game key code {}, which press() cannot name", control,
                   code),
       __FILE__, __LINE__);
  return std::string();
}

std::string Game::describeKey(std::string_view key) const {
  const std::optional<Stroke> stroke = StrokeFor(key);
  if (!stroke.has_value()) {
    return std::format("layout {}: no key gives {}", layoutName(), key);
  }
  return std::format(
      "layout {}: {} -> scancode {} ({}), {} -> key 0x{:x}", layoutName(), key,
      stroke->scancode,
      SDL_GetScancodeName(static_cast<SDL_Scancode>(stroke->scancode)),
      ModsText(stroke->mods), EventKeyFor(stroke->scancode, stroke->mods));
}

long Game::selectedRow() const {
  GuiDisplayGen* gui = ShowingGui();
  return gui != nullptr ? static_cast<long>(gui->getSelectedRow()) : -1;
}

std::string Game::rowText(long row) const { return RowText(ShowingGui(), row); }

std::string Game::rowKey(long row) const {
  GuiDisplayGen* gui = ShowingGui();
  if (gui == nullptr || row < 0) return std::string();
  return gui->keyForRow(row).value_or(std::string());
}

namespace {

bool SelectRow(Game& game, const std::function<bool()>& selected,
               std::string_view what) {
  if (selected()) return true;
  for (const char* direction : {"down", "up"}) {
    // One full cycle: the GUI has at most kGuiMaxRows rows.
    for (int i = 0; i < kGuiMaxRows; ++i) {
      const long before = game.selectedRow();
      game.press(direction);
      if (selected()) return true;
      if (game.selectedRow() == before) break;  // the end of the range
    }
  }
  fail(std::format("no row {} could be selected at {}", what, game.screen()),
       __FILE__, __LINE__);
  return false;
}

}  // namespace

bool Game::selectRowWithKey(std::string_view key) {
  return SelectRow(
      *this, [&] { return rowKey(selectedRow()) == key; },
      std::format("with key \"{}\"", key));
}

bool Game::selectRowWithText(std::string_view text) {
  return SelectRow(
      *this, [&] { return rowText(selectedRow()) == text; },
      std::format("saying \"{}\"", text));
}

bool Game::selectRow(long row) {
  return SelectRow(
      *this, [&] { return selectedRow() == row; }, std::format("{}", row));
}

std::vector<long> Game::selectableRows() const {
  std::vector<long> rows;
  GuiDisplayGen* gui = ShowingGui();
  if (gui == nullptr) return rows;
  const NSRange range = gui->getSelectableRange();
  for (std::size_t row = range.location; row < range.location + range.length;
       ++row) {
    const std::optional<std::string> key =
        gui->keyForRow(static_cast<OOGUIRow>(row));
    if (key.has_value() && !key->empty() && *key != "SKIP-ROW") {
      rows.push_back(static_cast<long>(row));
    }
  }
  return rows;
}

bool Game::exitRequested() const {
  return controller()->exitRequested().has_value();
}

void Game::checkPressEffect(std::string_view key, bool changed,
                            const char* file, int line) const {
  if (changed) {
    ::oo_test::check(true, "", file, line);
    return;
  }
  const std::string message =
      std::format("pressing {} had no effect at {}, game time {:.3f}\n     {}",
                  key, screen(), time(), describeKey(key));
  ::oo_test::check(false, message.c_str(), file, line);
  PrintState(file, line);
}

Game Game::scenario(std::uint32_t seed) {
  OOHeadlessOptions options = BaseOptions(seed);
  options.populate = false;
  options.startInFlight = true;
  return clearedScenario(options, Settings());
}

Game Game::inFlightWithKeys(std::uint32_t seed, const Settings& settings) {
  OOHeadlessOptions options = BaseOptions(seed);
  options.populate = false;
  options.startInFlight = true;
  options.inputView = true;
  Game game = clearedScenario(options, settings);
  for (std::size_t i = 0; i + 1 < Arguments().size(); ++i) {
    if (Arguments()[i] == "--layout") game.useLayout(Arguments()[i + 1]);
  }
  return game;
}

Game Game::docked(std::uint32_t seed, const Settings& settings) {
  OOHeadlessOptions options = BaseOptions(seed);
  options.populate = true;
  options.startInFlight = false;
  options.inputView = true;
  if (!start(options, settings)) return Game();
  Game game;
  // A new commander: the standard scenario's save, as Start New Commander
  // loads it, docked at the main station.
  PlayerEntity* player = PlayerEntity::sharedPlayer();
  if (player == nullptr ||
      !player->loadPlayerFromFile(kStandardScenarioSave, false) || !player->isDocked()) {
    fail("docked: the new commander is not docked after loading the standard "
         "scenario",
         __FILE__, __LINE__);
  }
  // A load holds every key until one is released (suppressKeysUntilKeyUp):
  // a player's load ends with the release of the key that chose it, so this
  // game's does too.
  game.keyUpScancode(SDL_SCANCODE_RETURN, SDL_KMOD_NONE);
  game.step(kPressSteps);
  for (std::size_t i = 0; i + 1 < Arguments().size(); ++i) {
    if (Arguments()[i] == "--layout") game.useLayout(Arguments()[i + 1]);
  }
  return game;
}

Game Game::clearedScenario(const OOHeadlessOptions& options,
                           const Settings& settings) {
  if (!start(options, settings)) return Game();

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
    // As performGameTick: the view's input first, when there is a view.
    if (controller()->gameView() != nullptr) controller()->gameView()->pollControls();
    controller()->tick(kStepSeconds);
    controller()->fireDueDeferredCalls();  // as the frame loop's pass does
    ++state.steps;
    if (state.digestOut.has_value()) {
      *state.digestOut << state.steps << ' ' << std::hex << digest() << std::dec
                       << '\n';
    }
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

std::uint64_t Game::digest() const {
  Fnv1a digest;
  for (const oo::EntityHold& held : universe()->entityList()) {
    Entity* entity = held.get();
    if (entity == nullptr) continue;
    digest.addValue(entity->getUniversalID());
    if (ShipEntity* ship = dynamic_cast<ShipEntity*>(entity)) {
      digest.addString(ship->shipDataKey().value_or(""));
    }
    const HPVector p = entity->getPosition();
    digest.addValue(p.x);
    digest.addValue(p.y);
    digest.addValue(p.z);
    const Quaternion q = entity->getOrientation();
    digest.addValue(q.w);
    digest.addValue(q.x);
    digest.addValue(q.y);
    digest.addValue(q.z);
    digest.addValue(entity->getEnergy());
  }
  digest.addValue(TheState().eventsDigest.value());
  return digest.value();
}

namespace {

constexpr double kNearPlayerMetres = 5000.0;

// Somewhere within metres of centre, from the game's seeded generator.
HPVector Near(HPVector centre, double metres) {
  const HPVector direction = OORandomUnitHPVector();
  return HPvector_add(centre, HPvector_multiply_scalar(direction, metres * randf()));
}

HPVector PlaceFor(const ShipWithKey::Placement placement, HPVector position,
                  Entity* reference, double metres) {
  switch (placement) {
    case ShipWithKey::Placement::kAt:
      return position;
    case ShipWithKey::Placement::kAheadOf: {
      // A ship's own forward vector: the player's orientation is held the
      // other way round, so its quaternion's forward is not its nose.
      auto* ship = dynamic_cast<ShipEntity*>(reference);
      const Vector forward = ship != nullptr
                                 ? ship->forwardVector()
                                 : vector_forward_from_quaternion(reference->getOrientation());
      return HPvector_add(reference->getPosition(),
                          HPvector_multiply_scalar(vectorToHPVector(forward), metres));
    }
    case ShipWithKey::Placement::kNear:
      return Near(reference->getPosition(), metres);
    case ShipWithKey::Placement::kNearPlayer:
      break;
  }
  return Near(PlayerEntity::sharedPlayer()->getPosition(), kNearPlayerMetres);
}

}  // namespace

ShipHandle Game::add(const ShipWithKey& what) {
  if ((what.placement_ == ShipWithKey::Placement::kAheadOf ||
       what.placement_ == ShipWithKey::Placement::kNear) &&
      what.reference_ == nullptr) {
    fail("ShipWithKey(" + what.key_ + "): placed by a ship that is gone", __FILE__,
         __LINE__);
    return ShipHandle();
  }
  const HPVector position =
      PlaceFor(what.placement_, what.position_, what.reference_, what.metres_);
  return ShipHandle(spawnKey(what.key_, position));
}

std::shared_ptr<ShipEntity> Game::spawnKey(const std::string& shipKey,
                                           HPVector position) {
  std::shared_ptr<ShipEntity> ship = universe()->newShipWithName(shipKey);
  if (ship == nullptr) return nullptr;
  ship->setPosition(position);
  if (!universe()->addEntity(ship.get())) return nullptr;
  return ship;
}

void Game::remove(ShipEntity* ship) {
  if (ship == nullptr || ship->getIsPlayer()) return;
  // What the script API's ship.remove() does (OOJSShip.cpp RemoveOrExplodeShip):
  // the ship dies with no explosion, and the universe takes it out in its next
  // update, as for any ship that dies, so nothing is left pointing at it.
  if (ship == oo::ToShip(universe()->station())) universe()->unMagicMainStation();
  if (ship->status() == oo::EntityStatus::kDocked) {
    ship->setStatus(oo::EntityStatus::kDead);
  } else {
    ship->setSuppressExplosion(true);
    ship->setEnergy(1);
    ship->takeEnergyDamage(500000000.0, nullptr, nullptr, std::string());
  }
  std::shared_ptr<ShipEntity> held = oo::share(ship);
  const std::weak_ptr<ShipEntity> watched = held;
  held.reset();
  const OOUniversalID id = ship->getUniversalID();
  for (int i = 0; i < 60; ++i) {  // a second at most
    const std::shared_ptr<ShipEntity> still = watched.lock();
    if (still == nullptr || universe()->entityForUniversalID(id) != still.get()) return;
    step();
  }
}

std::vector<ShipHandle> Game::shipsWithRole(std::string_view role) {
  std::vector<ShipHandle> ships;
  for (const oo::EntityHold& entity : universe()->entityList()) {
    ShipEntity* ship = dynamic_cast<ShipEntity*>(entity.get());
    if (ship == nullptr || ship->status() == oo::EntityStatus::kDead) continue;
    if (ship->getPrimaryRole().value_or("") == role) ships.emplace_back(oo::share(ship));
  }
  return ships;
}

std::size_t Game::entityCount() const { return universe()->entityCount(); }

std::optional<std::array<double, 3>> Game::locus() const {
  return TheState().locus;
}

std::vector<ShipHandle> Game::spawn(const ShipsWithRole& what) {
  State& state = TheState();
  const std::string& role = what.role_;

  // Every ship there is now, so the new ones can be told apart.
  std::set<const Entity*> before;
  for (const oo::EntityHold& entity : universe()->entityList()) {
    before.insert(entity.get());
  }

  // world_steps.py _spawn: "within K km" measures from the first ship spawned
  // (debugConsole.__ooLocus there; undefined before any spawn), otherwise
  // around the player.
  std::string at = "player.ship.position";
  int radius = kSpawnRadiusMetres;
  if (what.withinKm_.has_value()) {
    radius = *what.withinKm_ * 1000;
    at = state.locus.has_value()
             ? std::format("new Vector3D({:.17g}, {:.17g}, {:.17g})",
                           (*state.locus)[0], (*state.locus)[1],
                           (*state.locus)[2])
             : "undefined";
  }
  const auto pinned = RoleShipKey().find(role);
  const std::string selector = pinned != RoleShipKey().end() ? pinned->second : role;
  std::string perShip = role == "pirate"
                            ? std::format("added[i].bounty = {};", kPirateBounty)
                            : "";
  const auto ai = RoleAI().find(role);
  if (ai != RoleAI().end()) {
    perShip += std::format(" added[i].setAI({});", JsString(ai->second));
  }
  const std::string js = std::format(
      "(function(){{"
      " var at = {0};"
      " var added = system.addShips({1}, {2}, at, {3});"
      " if (!added) return 0;"
      " for (var i = 0; i < added.length; i++) {{"
      "   added[i].primaryRole = {4};"
      "   {5}"
      " }}"
      " return added.length; }})()",
      at, JsString(selector), what.count_, radius, JsString(role), perShip);
  const int added = static_cast<int>(eval(js));
  if (added != what.count_) {
    fail(std::format("asked for {} {}, addShips returned {}", what.count_, role,
                     added),
         __FILE__, __LINE__);
  }

  std::vector<ShipHandle> ships;
  for (const oo::EntityHold& entity : universe()->entityList()) {
    if (before.contains(entity.get())) continue;
    ShipEntity* ship = dynamic_cast<ShipEntity*>(entity.get());
    if (ship == nullptr || ship->getPrimaryRole().value_or("") != role) continue;
    ships.emplace_back(oo::share(ship));
  }
  if (!ships.empty() && !state.locus.has_value()) {
    const HPVector p = static_cast<ShipEntity*>(ships.front())->getPosition();
    state.locus = std::array<double, 3>{p.x, p.y, p.z};
  }
  if (std::find(state.spawnedRoles.begin(), state.spawnedRoles.end(), role) ==
      state.spawnedRoles.end()) {
    state.spawnedRoles.push_back(role);
  }
  return ships;
}

int Game::countWithRole(std::string_view role) {
  // world_steps.py count_with_role.
  return static_cast<int>(
      eval(std::format("system.countShipsWithRole({})", JsString(role))));
}

bool Game::engages(std::string_view hunter, std::string_view quarry) {
  // The expression of the windowed step "within N ticks a ship with role H
  // engages a ship with role Q".
  return eval(std::format(
             "(function(){{ var r = 0;"
             " system.allShips.forEach(function(s){{"
             "   if (s.primaryRole == {0} && s.hasHostileTarget"
             "       && s.target && s.target.primaryRole == {1}) r = 1;"
             " }});"
             " return r; }})()",
             JsString(hunter), JsString(quarry))) == 1.0;
}

void Game::simulateAtMost(GameTime span) {
  // world_steps.py run_simulation: every spawned role except police.
  std::vector<std::string> watched;
  for (const std::string& role : TheState().spawnedRoles) {
    if (role != "police") watched.push_back(role);
  }
  const double deadline = time() + span.count();
  double nextPoll = time() + kPollSeconds;
  while (time() < deadline) {
    step();
    if (time() < nextPoll || watched.empty()) continue;
    nextPoll += kPollSeconds;
    bool allGone = true;
    for (const std::string& role : watched) {
      if (countWithRole(role) != 0) allGone = false;
    }
    if (allGone) return;
  }
}

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

std::vector<std::string> Game::logLinesContaining(std::string_view text) const {
  std::vector<std::string> lines;
  for (const Event& event : TheState().events.entries()) {
    if (event.kind == EventKind::kLog || event.kind == EventKind::kScriptError) {
      if (event.text.find(text) != std::string::npos) lines.push_back(event.text);
    }
  }
  return lines;
}

const OOPresentationRecord& Game::presentation() const {
  return controller()->presentation();
}

std::filesystem::path Game::scratchDirectory() { return TheState().scratch; }

std::filesystem::path Game::buildRoot() { return BuildRoot(); }

std::filesystem::path Game::sourceRoot() {
  if (std::optional<std::string> root = Environment("OO_COMPONENT_SOURCE_ROOT")) {
    return fs::path(*root);
  }
  return (BuildRoot() / ".." / "..").lexically_normal();
}

void Game::reseed(std::uint32_t seed) { ranrot_srand(seed); }

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

namespace {

// oo_test::runAll's loop and output, and after a case that failed, the game's
// state (seed, game time, screen, the last events, the rerun command): a
// plain OO_CHECK knows nothing of the game.
int RunCases(const char* file) {
  const std::vector<::oo_test::Case>& cases = ::oo_test::registry();
  std::signal(SIGABRT, [](int) {
    std::fprintf(stderr, "oo_test: abort() in case %s\n", ::oo_test::currentTest());
    std::fflush(stderr);
  });
  // --case <name>: that case alone (a program whose cases each need a game of
  // their own is registered once per case).
  std::optional<std::string> only;
  for (std::size_t i = 0; i + 1 < Arguments().size(); ++i) {
    if (Arguments()[i] == "--case") only = Arguments()[i + 1];
  }
  std::size_t ran = 0;
  for (const ::oo_test::Case& c : cases) {
    if (only.has_value() && *only != c.name) continue;
    ++ran;
    ::oo_test::currentTest() = c.name;
    const int before = ::oo_test::failures();
    c.fn();
    const bool passed = ::oo_test::failures() == before;
    if (!passed) PrintState(file, 0);
    std::printf("%s %s\n", passed ? "ok  " : "FAIL", c.name);
    std::fflush(stdout);
  }
  std::printf("oo_test: %s: %zu tests, %d checks, %d failures\n", file, ran,
              ::oo_test::checks(), ::oo_test::failures());
  if (ran == 0) {
    std::printf("FAIL %s: no tests registered\n", file);
    return 2;
  }
  return ::oo_test::failures() == 0 ? 0 : 1;
}

}  // namespace

namespace {

void FinishScratch(int result) {
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
}

}  // namespace

void exitProgram(int code) {
  FinishScratch(code);
  std::exit(code);
}

int runMain(int argc, char** argv, const char* file) {
  ProgramPath() = argc > 0 ? argv[0] : "component";
  for (int i = 1; i < argc; ++i) Arguments().emplace_back(argv[i]);
  const int result = RunCases(file);
  FinishScratch(result);
  return result;
}

}  // namespace oo::component
