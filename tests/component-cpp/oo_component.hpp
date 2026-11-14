/*	tests/component-cpp/oo_component.hpp
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
#include <array>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class GameController;
class PlayerEntity;
class ShipEntity;
class Universe;
struct OOPresentationRecord;

namespace oo::component {

// Game time. The windowed tier's "tick" is one AI think interval, 0.125 s
// (tests/component/steps/world_steps.py TICK_SECONDS), so its "within 900
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
  // An event of that name whose subject is the ship with that id.
  bool has(std::string_view name, std::uint32_t subjectId) const;
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

class Game {
 public:
  // An empty system, the player in flight, no keys: the windowed step
  // "a universe seeded with <seed>". One per process; a second call fails the
  // test and returns the game that exists.
  static Game scenario(std::uint32_t seed);
  // The game an earlier case made. Fails the test if there is none.
  static Game current();

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

  // The world, in the vocabulary of tests/component/steps/world_steps.py:
  // each runs that step's own script.
  std::vector<ShipHandle> spawn(const ShipsWithRole& what);  // the ships added
  int countWithRole(std::string_view role);
  bool engages(std::string_view hunter, std::string_view quarry);
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

  // How many steps this game has taken.
  std::uint64_t steps() const;

  // How long, in real seconds, the game took to start (scenario() up to its
  // first step).
  static double startupSeconds();

  // Where this process's scratch directory is.
  static std::filesystem::path scratchDirectory();

 private:
  Game() = default;
};

// The program's arguments after the name, for options such as --digest-out.
const std::vector<std::string>& programArguments();

// Fail the current case with a message (and the game's state, if there is
// a game).
void fail(std::string_view message, const char* file, int line);

// Fails the case, printing each error entry, if the game logged any error.
void checkNoErrors(const Game& game, const char* file, int line);

// main(): runs the cases, then removes the scratch directory if every case
// passed, or says where it was kept.
int runMain(int argc, char** argv, const char* file);

}  // namespace oo::component

#define OO_COMPONENT_CHECK_NO_ERRORS(game) \
  ::oo::component::checkNoErrors((game), __FILE__, __LINE__)

#define OO_COMPONENT_MAIN()                                        \
  int main(int argc, char** argv) {                                \
    return ::oo::component::runMain(argc, argv, __FILE__);         \
  }
