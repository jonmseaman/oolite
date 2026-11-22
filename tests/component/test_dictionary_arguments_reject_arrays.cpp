/*	tests/component/test_dictionary_arguments_reject_arrays.cpp
        An array where a script method reads a dictionary is a script error
   (spec 012, triage run 1, row 4). mission.markSystem(),
   mission.unmarkSystem(), Ship.setMaterials() and Ship.setShaders() read keys
   from the object they are given; in 1.93 an array there raises a native
   exception (a dictionary method sent to an array), and the script gets an
   error. The C++ build once read the array as an empty dictionary and
   returned normally. setShaders() reads its argument only while shaders are
   on, so with shaders off (as here, with no GL) it returns normally, as 1.93
   does.
   Run: tests/run.sh component -k dictionary_arguments
*/

#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "oo_component.hpp"

using oo::component::Game;

namespace {

// How a call ended: "returned" or "threw: <message>".
std::string Outcome(Game& game, const std::string& call) {
  return game.evalString(
      "(function(){ try { " + call +
      "; return 'returned'; } catch (e) { return 'threw: ' + e.message; } })()");
}

bool ThrewNativeException(Game& game, const std::string& call) {
  const std::string outcome = Outcome(game, call);
  const bool threw = outcome.rfind("threw: Native exception", 0) == 0;
  if (!threw) std::printf("    %s\n    gave: %s\n", call.c_str(), outcome.c_str());
  return threw;
}

}  // namespace

TEST(dictionary_arguments_reject_arrays, an_array_marker_is_a_script_error) {
  Game game = Game::scenario(1);
  EXPECT_TRUE(ThrewNativeException(game, "mission.markSystem([1, 2])"));
  EXPECT_TRUE(ThrewNativeException(game, "mission.unmarkSystem([1, 2])"));
}

TEST(dictionary_arguments_reject_arrays, an_object_marker_still_marks_and_unmarks) {
  Game game = Game::current();
  const std::string marker = "{system: 7, name: 'test-012'}";
  EXPECT_EQ(Outcome(game, "mission.markSystem(" + marker + ")"), std::string("returned"));
  EXPECT_TRUE(game.evalBool(
      "mission.markedSystems.some(function(m) { return m.name == 'test-012'; })"));
  EXPECT_TRUE(game.evalBool("mission.unmarkSystem(" + marker + ")"));
}

TEST(dictionary_arguments_reject_arrays, array_materials_are_a_script_error) {
  Game game = Game::current();
  EXPECT_TRUE(ThrewNativeException(game, "player.ship.setMaterials([1, 2])"));
}

TEST(dictionary_arguments_reject_arrays, array_shaders_are_a_script_error_while_shaders_are_on) {
  Game game = Game::current();
  if (game.evalBool("['DETAIL_LEVEL_SHADERS', 'DETAIL_LEVEL_EXTRAS']"
                    ".indexOf(oolite.gameSettings.detailLevel) >= 0")) {
    EXPECT_TRUE(ThrewNativeException(game, "player.ship.setShaders([1, 2])"));
  } else {
    EXPECT_EQ(Outcome(game, "player.ship.setShaders([1, 2])"), std::string("returned"));
  }
}

OO_COMPONENT_MAIN()
