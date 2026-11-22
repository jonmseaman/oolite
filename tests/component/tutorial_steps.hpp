/*	tests/component/tutorial_steps.hpp
        The tutorial by keys (spec 011 US3 scenario 3, FR-026), for a game made
   with Game::atStartScreen: the tutorial's progress as its own script counts
   it, and the way in from the start screen.
*/

#pragma once

#include <chrono>
#include <compare>
#include <cstddef>
#include <cstdio>
#include <format>
#include <string>

#include <gtest/gtest.h>

#include "PlayerEntity.h"
#include "Universe.h"
#include "oo_component.hpp"

struct Stage {
  int stage = -1;
  int substage = -1;
  auto operator<=>(const Stage&) const = default;
};

inline Stage Progress(oo::component::Game& game) {
  const std::string text = game.evalString(
      "(function(){ var t = worldScripts[\"oolite-tutorial\"];"
      " return t.$tutorialStage + ' ' + t.$tutorialSubstage; })()");
  Stage s;
  if (std::sscanf(text.c_str(), "%d %d", &s.stage, &s.substage) != 2) return Stage{};
  return s;
}

/*	Start New Commander -> the scenario screen -> the tutorial scenario ->
   Enter -> its welcome mission screen, which Enter dismisses -> launch ->
   lesson 0 step 2, the first "press n". Each press is checked. The tutorial
   row is found by the scenario's file, not its title.
*/
inline void StartTutorial(oo::component::Game& game) {
  using namespace std::chrono_literals;
  EXPECT_TRUE(game.selectRowWithText("Start New Commander"));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_NEWGAME");
  // The scenario screen's rows are keyed Scenario:<index into scenarios.plist>.
  const oo::PList scenarios = game.universe()->scenarios();
  std::size_t tutorial = scenarios.count();
  for (std::size_t i = 0; i < scenarios.count(); ++i) {
    const oo::PList* file = scenarios.at(i) != nullptr ? scenarios.at(i)->find("file") : nullptr;
    if (file != nullptr && file->isString() &&
        *file->getIf<std::string>() == "oolite-tutorial.oolite-save") {
      tutorial = i;
    }
  }
  EXPECT_TRUE(tutorial < scenarios.count());
  EXPECT_TRUE(game.selectRowWithKey(std::format("Scenario:{}", tutorial)));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_MISSION");
  EXPECT_TRUE(game.simulateUntil(
      [&] { return game.evalString("mission.screenID") == "oolite-tutorial-0-0"; },
      10s));
  EXPECT_TRUE(Progress(game) == (Stage{0, 0}));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() != "GUI_SCREEN_MISSION");
  // "The player is in flight": in flight, the main
  // screen, and the tutorial's post-launch step.
  EXPECT_TRUE(game.simulateUntil(
      [&] {
        return game.player()->status() == oo::EntityStatus::kInFlight &&
               game.screen() == "GUI_SCREEN_MAIN" && Progress(game) == (Stage{0, 2});
      },
      60s));
}
