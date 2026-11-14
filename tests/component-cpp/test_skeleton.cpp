/*	tests/component-cpp/test_skeleton.cpp
        The C++ component tier's first program. It links every game object but
   main's, as every component program does, and boots nothing: if it builds
   and passes on a platform, the suite's build and runner work there.
   Run: meson test -C build/meson_dev test_skeleton
*/

#include <cstdint>

#include "GameController.h"
#include "oo_test.hpp"

// main.cpp defines the debug flags, and the program has its own main.
#ifdef OO_DEBUG_MONITOR_ENABLED
uint32_t gDebugFlags = 0;
#endif

OO_TEST(the_whole_game_links) {
  GameController* (*shared)() = &GameController::sharedController;
  OO_CHECK(shared != nullptr);
}

OO_TEST_MAIN()
