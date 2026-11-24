/*	tests/component/test_vector_index.cpp
        Vector3D and Quaternion components read and written by index, as
   under SpiderMonkey: there a numeric property id reached the class's
   property hook, whose ids are x, y, z = 0, 1, 2 (w, x, y, z = 0..3 for a
   Quaternion), so v[0] was v.x and q[0] was q.w. Expansions rely on it: one
   copies player.ship.orientation and the ship's basis vectors by index each
   frame and writes the turned result back; with v[0] undefined that result
   is NaN, the player's orientation becomes NaN and nothing more is drawn.
   Run: tests/run.sh component -k vector_index
*/

#include <string>

#include <gtest/gtest.h>

#include "oo_component.hpp"

using oo::component::Game;

TEST(vector_index, vector_components_read_by_index) {
  Game game = Game::scenario(20261124);
  EXPECT_TRUE(game.evalBool(
      "(function(){ var v = new Vector3D(1.5, -2, 3.25);"
      " return v[0] === 1.5 && v[1] === -2 && v[2] === 3.25; })()"));
  EXPECT_TRUE(game.evalBool(
      "(function(){ var f = player.ship.vectorForward;"
      " return f[0] === f.x && f[1] === f.y && f[2] === f.z; })()"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(vector_index, vector_components_written_by_index) {
  Game game = Game::current();
  EXPECT_TRUE(game.evalBool(
      "(function(){ var v = new Vector3D(1, 2, 3);"
      " v[0] = 7; v[1] = 8; v[2] = 9;"
      " return v.x === 7 && v.y === 8 && v.z === 9; })()"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(vector_index, quaternion_components_read_and_written_by_index) {
  Game game = Game::current();
  EXPECT_TRUE(game.evalBool(
      "(function(){ var q = new Quaternion(0.5, -0.5, 0.25, 0.75);"
      " return q[0] === 0.5 && q[1] === -0.5 && q[2] === 0.25 && q[3] === 0.75; })()"));
  EXPECT_TRUE(game.evalBool(
      "(function(){ var q = new Quaternion(1, 0, 0, 0);"
      " q[0] = 0.25; q[1] = 0.5; q[2] = 0.75; q[3] = 1;"
      " return q.w === 0.25 && q.x === 0.5 && q.y === 0.75 && q.z === 1; })()"));
  EXPECT_TRUE(game.evalBool(
      "(function(){ var o = player.ship.orientation;"
      " return o[0] === o.w && o[1] === o.x && o[2] === o.y && o[3] === o.z; })()"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// The index properties are not enumerated: the keys are only the names.
TEST(vector_index, index_properties_are_not_enumerated) {
  Game game = Game::current();
  EXPECT_EQ(game.evalString("Object.keys(new Vector3D(1, 2, 3)).join()"),
            std::string("x,y,z"));
  EXPECT_EQ(game.evalString("Object.keys(new Quaternion(1, 2, 3, 4)).join()"),
            std::string("w,x,y,z"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// The expansion's per-frame turn: copy the orientation and an axis by index
// into arrays, rotate with plain arithmetic, assign the array back.
TEST(vector_index, orientation_turned_by_index_stays_finite) {
  Game game = Game::current();
  EXPECT_TRUE(game.evalBool(
      "(function(){"
      " var ps = player.ship, q = [], axis = [], r = [];"
      " q[0] = ps.orientation[0]; q[1] = ps.orientation[1];"
      " q[2] = ps.orientation[2]; q[3] = ps.orientation[3];"
      " axis[0] = ps.vectorRight[0]; axis[1] = ps.vectorRight[1];"
      " axis[2] = ps.vectorRight[2];"
      " var c = Math.cos(0.05), s = Math.sin(0.05);"
      " var n = [c, axis[0] * s, axis[1] * s, axis[2] * s];"
      " r[0] = q[0] * n[0] - q[1] * n[1] - q[2] * n[2] - q[3] * n[3];"
      " r[1] = q[0] * n[1] + q[1] * n[0] + q[2] * n[3] - q[3] * n[2];"
      " r[2] = q[0] * n[2] + q[2] * n[0] + q[3] * n[1] - q[1] * n[3];"
      " r[3] = q[0] * n[3] + q[3] * n[0] + q[1] * n[2] - q[2] * n[1];"
      " ps.orientation = r;"
      " var o = ps.orientation;"
      " return isFinite(o.w) && isFinite(o.x) && isFinite(o.y) && isFinite(o.z); })()"));
  game.step(2);
  EXPECT_TRUE(game.evalBool(
      "isFinite(player.ship.position.x) && isFinite(player.ship.orientation.w)"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
