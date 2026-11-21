/*	tests/component/test_exit_runs_js_leak_check.cpp
        Quitting a debug build runs the JavaScript runtime's leak check
   (upstream #768): the exit tears the runtime down after the defaults are
   written, so a JSValue still alive at exit is reported. A clean shutdown logs
   script.javaScript.shutDown (upstream #580), so the test can tell the check
   ran; without the call in GameController::shutDownForExit the line is
   absent.

        Headless, quitGame() is recorded and not obeyed, and the process is the
   test's, so the case asks for the quit as a script does (it quits once the
   calling script has returned, #580), then runs the exit's teardown itself.
   The scripts are gone after that, so this is the program's only case. A
   build with NDEBUG has no leak check, and the case then checks only the
   quit. The C++ counterpart of tests/component-py/test_exit_runs_js_leak_check.py.
   Run: tests/run.sh component -k exit_runs_js_leak_check
*/

#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "GameController.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

OO_TEST(quitting_logs_the_javascript_leak_check) {
  // With a view: the universe reaches the controller through its view, so a
  // game with none never passes the quit on.
  Game game = Game::inFlightWithKeys(1);
  game.eval("quitGame(), 0");
  OO_CHECK(game.simulateUntil([&game] { return game.exitRequested(); }, 1s));
  OO_COMPONENT_CHECK_NO_ERRORS(game);

  game.controller()->shutDownForExit();

#ifndef NDEBUG
  const std::vector<std::string> shutdown =
      game.logLinesContaining("[script.javaScript.shutDown");
  OO_CHECK(!shutdown.empty());  // the exit shut the runtime down
  bool clean = false;
  for (const std::string& line : shutdown) {
    std::printf("    %s\n", line.c_str());
    if (line.find("[script.javaScript.shutDown]") != std::string::npos &&
        line.find("JavaScript runtime shut down with no leaked objects.") !=
            std::string::npos) {
      clean = true;
    }
  }
  OO_CHECK(clean);  // and nothing was still alive
#endif
}

OO_COMPONENT_MAIN()
