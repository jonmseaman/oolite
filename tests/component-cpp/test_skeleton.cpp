/*	tests/component-cpp/test_skeleton.cpp
        The C++ component tier's first program. It links every game object but
   main's, as every component program does, and boots nothing: if it builds
   and passes on a platform, the suite's build and runner work there.

        Three probes of the platform's SDL3, which the headless key tests rely
   on: the dummy video driver initialises; a key event pushed onto SDL's queue
   comes back from SDL_PollEvent unchanged; and what SDL's built-in keymap
   gives for N with Caps Lock (printed, since only n or N is a sane answer and
   which one decides how Caps Lock reaches the game).
   Run: meson test -C build/meson_dev test_skeleton
*/

#include <SDL3/SDL.h>

#include <cstdint>
#include <cstdio>

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

OO_TEST(sdl_dummy_video_driver_initialises) {
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
  const bool initialised = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  if (!initialised) std::printf("SDL_Init: %s\n", SDL_GetError());
  OO_CHECK(initialised);
  const char* driver = SDL_GetCurrentVideoDriver();
  const int version = SDL_GetVersion();
  std::printf("probe: SDL %d.%d.%d, video driver %s\n",
              SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version),
              SDL_VERSIONNUM_MICRO(version), driver != nullptr ? driver : "(none)");
  OO_CHECK(driver != nullptr && SDL_strcmp(driver, "dummy") == 0);
}

OO_TEST(a_pushed_key_event_comes_back) {
  // SDL is still up from the case above; drain whatever start-up queued.
  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
  }
  SDL_Event pushed{};
  pushed.type = SDL_EVENT_KEY_DOWN;
  pushed.key.scancode = SDL_SCANCODE_N;
  pushed.key.mod = SDL_KMOD_LSHIFT;
  pushed.key.key = SDL_GetKeyFromScancode(SDL_SCANCODE_N, SDL_KMOD_LSHIFT, true);
  pushed.key.down = true;
  OO_CHECK(SDL_PushEvent(&pushed));
  bool found = false;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
      found = true;
      OO_CHECK_EQ(event.key.scancode, SDL_SCANCODE_N);
      OO_CHECK_EQ(event.key.mod, SDL_KMOD_LSHIFT);
      OO_CHECK(event.key.down);
    }
  }
  std::printf("probe: pushed key event %s\n", found ? "came back" : "was lost");
  OO_CHECK(found);
}

OO_TEST(caps_lock_n_lookup_is_printed) {
  const SDL_Keycode key = SDL_GetKeyFromScancode(SDL_SCANCODE_N, SDL_KMOD_CAPS, false);
  std::printf("probe: SDL_GetKeyFromScancode(N, caps) = 0x%x\n",
              static_cast<unsigned>(key));
  OO_CHECK(key == SDLK_N || key == static_cast<SDL_Keycode>('N'));
  SDL_Quit();
}

OO_TEST_MAIN()
