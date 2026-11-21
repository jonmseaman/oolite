/*	tests/component/test_tutorial_keys.cpp
        The tutorial by its "next" key (spec 011 US3 scenario 3, FR-026): from
   the start screen into the tutorial, then the game's own activate-equipment
   key, looked up from the game's key settings, pressed for every step of
   lessons 0 and 1, each press checked to have moved the tutorial. What the
   windowed walkthrough checks (tests/component-py/steps/tutorial_steps.py) is
   checked here the same way. Registered once per keyboard layout
   (--layout us, fr_azerty, ru).
   Run: tests/run.sh component --cpp -k tutorial_keys
*/

#include <chrono>
#include <string>

#include "oo_component.hpp"
#include "oo_test.hpp"
#include "tutorial_steps.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

namespace {

// Bound on presses (tutorial_steps.py MAX_PRESSES_PER_LESSON, for two lessons).
constexpr int kMaxPresses = 80;

}  // namespace

OO_TEST(the_next_key_advances_every_step_of_lessons_0_and_1) {
  Game game = Game::atStartScreen(20261027);  // tutorial_steps.py TUTORIAL_SEED
  StartTutorial(game);
  OO_CHECK(Progress(game) == (Stage{0, 2}));

  const std::string next = game.keyForControl("key_activate_equipment");
  int presses = 0;
  while (Progress(game) < (Stage{2, 0}) && presses++ < kMaxPresses) {
    const Stage before = Progress(game);
    OO_COMPONENT_PRESS_EXPECT(game, next, Progress(game) != before);
    if (Progress(game) == before) break;  // reported; the rest would repeat it
  }
  OO_CHECK(Progress(game) == (Stage{2, 0}));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
