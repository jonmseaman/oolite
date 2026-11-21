/*	tests/component/oo_component.hpp
        The interface the C++ component tests are written against
   (specs/011-headless-component-tests/contracts/component-test-api.md).

        A Game starts the real game in this process with no window, no GL
   context and no sound device, and hands the test the game's own objects:
   a test calls Universe, PlayerEntity and ShipEntity directly and uses Game
   only to create, step and observe. One game per process: the game lives
   until the process ends (the limit spec 003 removes), so later cases in the
   same program reach it with Game::current().

        Before the game starts, a scratch directory unique to the process is
   made under <build>/component/scratch/<program>-<pid>/ and every per-user
   directory the game resolves (home, saves, logs, snapshots, add-ons,
   defaults) is pointed into it. It is removed when every case passed and kept
   when one failed.

            #include "oo_component.hpp"
            #include "oo_test.hpp"
            using namespace std::chrono_literals;
            OO_TEST(time_moves) {
              oo::component::Game game = oo::component::Game::scenario(1);
              game.simulate(10s);
              OO_CHECK(game.time() >= 10.0);
            }
            OO_COMPONENT_MAIN()
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <utility>
#include <array>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "OOHPVector.h"

// <windows.h> (through the game's headers) defines near and far as empty
// macros for 16-bit pointers; ShipWithKey::near is the interface's name.
#ifdef near
#undef near
#endif

class Entity;
class GameController;
class MyOpenGLView;
struct OOHeadlessOptions;
class PlayerEntity;
class ShipEntity;
class Universe;
struct OOPresentationRecord;

namespace oo::component {
class ShipHandle;
}

namespace oo::component {

// Game time. The windowed tier's "tick" is one AI think interval, 0.125 s
// (tests/component-py/steps/world_steps.py TICK_SECONDS), so its "within 900
// ticks" is ticks(900).
using GameTime = std::chrono::duration<double>;
constexpr GameTime ticks(int n) { return GameTime{n * 0.125}; }

// The fixed step a headless game advances by.
inline constexpr double kStepSeconds = 1.0 / 60.0;
// The script limiter's limit in a headless game, s.
inline constexpr double kScriptTimeLimitSeconds = 10.0;

// One thing the game raised, in the order it raised it
// (specs/011-headless-component-tests/data-model.md, Event record).
enum class EventKind { kShipEvent, kWorldEvent, kLog, kScriptError };

struct Event {
  double time = 0.0;  // game time, s
  EventKind kind = EventKind::kLog;
  std::string name;              // event name, or log class
  std::uint32_t subjectId = 0;   // the ship's universal id; 0: none
  std::string subjectKey;        // its definition key, if any
  std::string text;              // the log line, for kLog and kScriptError
  bool isError = false;
};

class EventRecord {
 public:
  std::size_t size() const { return entries_.size(); }
  const Event& operator[](std::size_t i) const { return entries_[i]; }
  const std::vector<Event>& entries() const { return entries_; }

  bool has(std::string_view name) const { return count(name) > 0; }
  // An event of that name whose subject is the ship with that id; a handle
  // matches by id, so it works after the ship is gone.
  bool has(std::string_view name, std::uint32_t subjectId) const;
  bool has(std::string_view name, const ShipHandle& ship) const;
  std::size_t count(std::string_view name) const;
  // The first event of that name for that subject (0: any), or null.
  const Event* firstOf(std::string_view name, std::uint32_t subjectId = 0) const;

  void add(Event event) { entries_.push_back(std::move(event)); }

 private:
  std::vector<Event> entries_;
};

// What to spawn, said fluently and passed to one call; the call that takes it
// does the work:
//   game.spawn(ShipsWithRole("pirate").count(2).withinKm(10))
// The windowed step "I spawn N ship(s) with role R [within K km]".
class ShipsWithRole {
 public:
  explicit ShipsWithRole(std::string_view role) : role_(role) {}
  ShipsWithRole count(int n) const {
    ShipsWithRole next = *this;
    next.count_ = n;
    return next;
  }
  // Of the first ship spawned in this game; unset: around the player.
  ShipsWithRole withinKm(int km) const {
    ShipsWithRole next = *this;
    next.withinKm_ = km;
    return next;
  }

 private:
  friend class Game;
  std::string role_;
  int count_ = 1;
  std::optional<int> withinKm_;
};

// One ship by definition key, placed, for game.add():
//   game.add(ShipWithKey("adder").aheadOf(game.player(), 5'000.0))
// Placement: .at(position), .aheadOf(entity, metres) (on its nose) or
// .near(entity, metres) (anywhere within that distance, from the seeded
// generator); unset, near the player within 5 km.
class ShipWithKey {
 public:
  explicit ShipWithKey(std::string_view shipKey) : key_(shipKey) {}
  ShipWithKey at(HPVector position) const;
  ShipWithKey aheadOf(Entity* entity, double metres) const;
  ShipWithKey near(Entity* entity, double metres) const;

  enum class Placement { kNearPlayer, kAt, kAheadOf, kNear };

 private:
  friend class Game;
  std::string key_;
  Placement placement_ = Placement::kNearPlayer;
  HPVector position_ = {0, 0, 0};
  Entity* reference_ = nullptr;
  double metres_ = 0.0;
};

// A ship a test was handed. Use it like a ShipEntity*; ask alive() before
// using it after the ship could have died. It never keeps the ship alive:
// inside it is a std::weak_ptr<ShipEntity>.
class ShipHandle {
 public:
  ShipHandle() = default;
  explicit ShipHandle(const std::shared_ptr<ShipEntity>& ship);

  bool alive() const;  // still in the universe
  // Kept after the ship is gone, so events can still be matched.
  std::uint32_t id() const { return id_; }
  const std::string& key() const { return key_; }
  ShipEntity* operator->() const;  // fails the test, by name, if gone
  operator ShipEntity*() const;    // null if gone

 private:
  std::weak_ptr<ShipEntity> ship_;
  std::uint32_t id_ = 0;
  std::string key_;
};

// A recorded keyboard layout (tests/component/layouts/<name>.tsv): what
// key code each (scancode, modifiers) pair gives. Columns: scancode, mods (-,
// shift, caps, shift+caps, altgr), keycode (hex), comment; # starts a comment.
class Layout {
 public:
  // A name in tests/component/layouts/, or a path to a .tsv file. Fails
  // the test, and gives an empty layout, if it cannot be read.
  static Layout load(std::string_view nameOrPath);

  const std::string& name() const { return name_; }
  // The key code for a physical key and modifier state; 0 (SDLK_UNKNOWN) if
  // the table has no row for it. Modifiers other than shift, caps lock and
  // AltGr (SDL_KMOD_MODE) are ignored, as the table records none.
  std::uint32_t keyFor(int scancode, std::uint16_t mods) const;
  // The physical key and modifiers that give a code point: the first row
  // giving it, trying no modifiers, then shift, caps, shift+caps and altgr.
  std::optional<std::pair<int, std::uint16_t>> strokeFor(std::uint32_t code) const;
  // Every row: (scancode, mods class 0-4) -> key code.
  const std::map<std::pair<int, int>, std::uint32_t>& rows() const { return rows_; }
  // The modifier class of a modifier state: 0 none, 1 shift, 2 caps,
  // 3 shift+caps, 4 altgr.
  static int modsClass(std::uint16_t mods);

 private:
  std::string name_;
  std::map<std::pair<int, int>, std::uint32_t> rows_;
};

// The player's saved settings a game starts with, written to the test's
// private settings before the game starts, in the form the game's own screens
// store them: a remapped key as the keyboard settings screen stores it
// (KeyConfigOverrides, as PlayerEntity::saveKeySetting writes it).
class Settings {
 public:
  // control: a key setting's name ("key_fire_lasers"); key: as press() takes
  // it, a character or a named key.
  Settings& remapKey(std::string_view control, std::string_view key);
  // The keyboard-code preference (which keymappings table the view loads).
  Settings& keyboardCode(std::string_view code);

 private:
  friend class Game;
  std::vector<std::pair<std::string, std::string>> remaps_;
  std::optional<std::string> keyboardCode_;
};

class Game {
 public:
  // An empty system, the player in flight, no keys: the windowed step
  // "a universe seeded with <seed>". One per process; a second call fails the
  // test and returns the game that exists.
  static Game scenario(std::uint32_t seed);
  // The start screen, the system populated, not in flight, with a view that
  // takes key events (no window). One per process, as scenario().
  static Game atStartScreen(std::uint32_t seed);
  // The scenario game (an empty system, the player in flight), with a view
  // that takes key events and the settings given.
  static Game inFlightWithKeys(std::uint32_t seed, const Settings& settings = {});
  // A new commander docked at the main station, with a view that takes key
  // events and the settings given.
  static Game docked(std::uint32_t seed, const Settings& settings = {});
  // The game an earlier case made. Fails the test if there is none.
  static Game current();

  // The player's GUI screen, by name (GUI_SCREEN_...).
  std::string screen() const;
  // The view that takes key events; null unless the game was made with one.
  MyOpenGLView* view() const;

  // Keys, for a game with a view. A key is a character ("n", "1") or a named
  // key: enter, escape, space, tab, backspace, delete, insert, home, end,
  // pageup, pagedown, up, down, left, right, f1-f12. A character is looked up
  // in the installed layout (SDL's own map if none) for its scancode and
  // modifiers; a character no key of the layout gives is pressed at its US
  // position (SDL's built-in map), as a player presses the key the letter is
  // printed on; a named key is its physical key. Every press is
  // SDL_EVENT_KEY_DOWN / SDL_EVENT_KEY_UP pushed onto SDL's queue and read by
  // the game's own pollControls(); the view's key state is never written.
  //
  // Installs a recorded layout as the view's key lookup. --layout <name> on
  // the command line installs one when the game starts.
  void useLayout(std::string_view nameOrPath);
  // The installed layout's name; "SDL" if none.
  std::string layoutName() const;
  // Key down, two steps (the game samples keys once a frame), key up, two
  // steps (so the next press is not a repeat).
  void press(std::string_view key);
  // Key down, simulateUntil(done, atMost), key up, two steps; done's answer.
  bool hold(std::string_view key, const std::function<bool()>& done,
            GameTime atMost);
  // By physical key, for the fault tests: down, two steps, up, two steps.
  void pressScancode(int scancode, std::uint16_t mods);
  void keyDownScancode(int scancode, std::uint16_t mods);
  void keyUpScancode(int scancode, std::uint16_t mods);
  // One press per character.
  void type(std::string_view text);
  // The key the game's own key settings name for a control
  // ("key_activate_equipment"), as press() takes it: its first definition's
  // key, a character or a named key. Fails the test if there is none.
  std::string keyForControl(std::string_view control) const;
  // How a key is pressed under the installed layout, for failure messages:
  // "n -> scancode 17 (N), no modifiers -> key 0x6e".
  std::string describeKey(std::string_view key) const;

  // Menu state, from the GUI showing.
  long selectedRow() const;  // -1 for none
  std::string rowText(long row) const;  // the first column's text, trimmed
  std::string rowKey(long row) const;
  // Presses down, then up, until the row with that key (text) is selected;
  // fails the test after one full cycle each way.
  bool selectRowWithKey(std::string_view key);
  bool selectRowWithText(std::string_view text);
  bool selectRow(long row);
  // The rows the showing GUI lets the player select: its selectable range,
  // less rows that have no key or are skipped (SKIP-ROW), as the GUI's own
  // up and down keys skip them.
  std::vector<long> selectableRows() const;
  // The game asked to quit (headless, the request is recorded, not obeyed).
  bool exitRequested() const;

  // OO_COMPONENT_PRESS_EXPECT's check, after the press.
  void checkPressEffect(std::string_view key, bool changed, const char* file,
                        int line) const;

  // Time. Nothing here waits on the real clock.
  void step(int steps = 1);  // each step is 1/60 s
  void simulate(GameTime span);  // rounded up to whole steps
  bool simulateUntil(const std::function<bool()>& done, GameTime atMost);
  double time() const;  // the universe's game time, s
  // The windowed step "the simulation runs for at most N ticks": steps for the
  // span, stopping early (checked every half second of game time, as the
  // windowed step polls) once every role spawned so far except police has no
  // ship left.
  void simulateAtMost(GameTime span);

  // The world, in the vocabulary of tests/component-py/steps/world_steps.py:
  // each runs that step's own script.
  std::vector<ShipHandle> spawn(const ShipsWithRole& what);  // the ships added
  int countWithRole(std::string_view role);
  bool engages(std::string_view hunter, std::string_view quarry);
  // The world, directly: one ship by key, added to the universe. The test owns
  // nothing: the universe keeps the ship, the handle only watches it (not
  // alive() if it could not be made).
  ShipHandle add(const ShipWithKey& what);
  // As add, at a plain position, answering the owning pointer
  // Universe::newShipWithName made; the census uses it.
  std::shared_ptr<ShipEntity> spawnKey(const std::string& shipKey, HPVector position);
  // Takes it out of the universe with no explosion, as ship.remove() does, and
  // steps (at most a second) until the universe has let it go.
  void remove(ShipEntity* ship);
  // The live ships whose primary role is role.
  std::vector<ShipHandle> shipsWithRole(std::string_view role);
  std::size_t entityCount() const;

  // Where the first ship spawned was, which "within K km" measures from.
  std::optional<std::array<double, 3>> locus() const;

  // The game's own objects; the game owns both for the whole process.
  Universe* universe() const;
  PlayerEntity* player() const;
  GameController* controller() const;

  // Script expressions, evaluated as the debug console evaluates them. An
  // expression that throws fails the test and prints the message.
  double eval(std::string_view js);
  bool evalBool(std::string_view js);
  std::string evalString(std::string_view js);

  // What happened: every script event and log line, in order.
  const EventRecord& events() const;
  std::vector<Event> errors() const;
  // The errors from entry <mark> on (mark: an earlier events().size()).
  std::vector<Event> errorsSince(std::size_t mark) const;
  // What the game would have drawn and played.
  const OOPresentationRecord& presentation() const;
  // The log lines so far that contain text: the windowed step "no ERROR
  // appears in the log" is logLinesContaining("ERROR").empty().
  std::vector<std::string> logLinesContaining(std::string_view text) const;

  // The scenario digest now: a hash of every entity's id, definition key,
  // position, orientation and energy, and of the event record so far. Two runs
  // with the same seed give the same digest at every step. With
  // --digest-out <path> a program writes one per step (data-model.md).
  std::uint64_t digest() const;

  // How many steps this game has taken.
  std::uint64_t steps() const;

  // How long, in real seconds, the game took to start (scenario() up to its
  // first step).
  static double startupSeconds();

  // Where this process's scratch directory is, and the build and source trees
  // (OO_COMPONENT_BUILD_ROOT and OO_COMPONENT_SOURCE_ROOT, or found from the
  // program's own path when run by hand).
  static std::filesystem::path scratchDirectory();
  static std::filesystem::path buildRoot();
  static std::filesystem::path sourceRoot();

  // Seeds RANROT again, as a new game with that seed has it (the census
  // reseeds per definition). The really-random generator is seeded once per
  // process by the game's design (legacy_random.h) and is not reset.
  void reseed(std::uint32_t seed);

 private:
  Game() = default;
  static bool start(const OOHeadlessOptions& options, const Settings& settings = {});
  static Game clearedScenario(const OOHeadlessOptions& options, const Settings& settings);
};

// The program's arguments after the name, for options such as --digest-out,
// and its own path.
const std::vector<std::string>& programArguments();
const std::string& programPath();
bool hasArgument(std::string_view argument);

// Another component program, built beside this one: its path (with .exe on
// Windows).
std::string siblingProgram(std::string_view name);

// Runs program with arguments as a child process, its standard output and
// error written to the files given ("" discards), and answers its exit status
// (nonzero also when it was killed). The child inherits the environment and
// the working directory.
int runChild(const std::string& program, const std::vector<std::string>& arguments,
             const std::string& outputFile, const std::string& errorFile);

// "GL trap: linked", or "GL trap: not linked (<reason>)": whether a GL call
// from this program would fail it by name (Linux and macOS with LTO off).
std::string glTrapState();

// Fail the current case with a message (and the game's state, if there is
// a game).
void fail(std::string_view message, const char* file, int line);

// Fails the case, printing each error entry, if the game logged any error.
void checkNoErrors(const Game& game, const char* file, int line);

// Ends the program now with that code, as main would: the scratch directory
// goes if the code is 0 and stays otherwise. For a program whose output and
// exit code are its own (the census).
[[noreturn]] void exitProgram(int code);

// main(): runs the cases, then removes the scratch directory if every case
// passed, or says where it was kept.
int runMain(int argc, char** argv, const char* file);

}  // namespace oo::component

// Presses key and fails, with "pressing <key> had no effect at <screen>, game
// time <t>" and how the layout translated the key, unless effect then holds.
#define OO_COMPONENT_PRESS_EXPECT(game, key, effect)                      \
  do {                                                                    \
    (game).press(key);                                                    \
    (game).checkPressEffect((key), static_cast<bool>(effect), __FILE__,   \
                            __LINE__);                                    \
  } while (0)

#define OO_COMPONENT_CHECK_NO_ERRORS(game) \
  ::oo::component::checkNoErrors((game), __FILE__, __LINE__)

#define OO_COMPONENT_MAIN()                                        \
  int main(int argc, char** argv) {                                \
    return ::oo::component::runMain(argc, argv, __FILE__);         \
  }
