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
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

class GameController;
class PlayerEntity;
class Universe;

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

  // The game's own objects; the game owns both for the whole process.
  Universe* universe() const;
  PlayerEntity* player() const;
  GameController* controller() const;

  // Script expressions, evaluated as the debug console evaluates them. An
  // expression that throws fails the test and prints the message.
  double eval(std::string_view js);
  bool evalBool(std::string_view js);
  std::string evalString(std::string_view js);

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

// main(): runs the cases, then removes the scratch directory if every case
// passed, or says where it was kept.
int runMain(int argc, char** argv, const char* file);

}  // namespace oo::component

#define OO_COMPONENT_MAIN()                                        \
  int main(int argc, char** argv) {                                \
    return ::oo::component::runMain(argc, argv, __FILE__);         \
  }
