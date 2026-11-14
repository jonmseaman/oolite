/*	tests/component-cpp/test_skeleton.cpp
        The C++ component tier's first program. It links every game object but
   main's, as every component program does, and boots nothing: if it builds
   and passes on a platform, the suite's build and runner work there.

        Three probes of the platform's SDL3, which the headless key tests rely
   on: the dummy video driver initialises; a key event pushed onto SDL's queue
   comes back from SDL_PollEvent unchanged; and what SDL's built-in keymap
   gives for N with Caps Lock (printed, since only n or N is a sane answer and
   which one decides how Caps Lock reaches the game).

        And the headless options and the game clock (src/Core/OOHeadless.h),
   which need no game to be running.
   Run: meson test -C build/meson_dev test_skeleton
*/

#include <SDL3/SDL.h>

#include <cstdint>
#include <cstdio>
#include <string>

#include "GameController.h"
#include "OOHeadless.h"
#include "oo_test.hpp"
#include "oofnd/Exception.hpp"

// main.cpp defines the debug flags, and the program has its own main.
#ifdef OO_DEBUG_MONITOR_ENABLED
uint32_t gDebugFlags = 0;
#endif

OO_TEST(the_whole_game_links) {
  GameController* (*shared)() = &GameController::sharedController;
  OO_CHECK(shared != nullptr);
}

// The simulation half of the frame is a member of its own, which a headless
// game steps.
OO_TEST(the_controller_has_tick) {
  void (GameController::*tick)(double) = &GameController::tick;
  OO_CHECK(tick != nullptr);
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

namespace {
// The reason validate() raised with, or "" if it did not raise.
std::string ValidationFailure(const OOHeadlessOptions& options) {
  try {
    options.validate();
  } catch (const oo::Exception& exception) {
    return exception.reason();
  }
  return "";
}
}  // namespace

OO_TEST(default_options_are_the_windowed_game) {
  const OOHeadlessOptions options;
  OO_CHECK(!options.headless);
  OO_CHECK(!options.seed.has_value());
  OO_CHECK(options.addOns == OOHeadlessOptions::AddOns::kAll);
  OO_CHECK(options.populate);
  OO_CHECK(!options.startInFlight);
  OO_CHECK(!options.inputView);
  OO_CHECK_EQ(options.scriptTimeLimit, 0.0);
  OO_CHECK_EQ(ValidationFailure(options), std::string());
}

OO_TEST(headless_without_a_seed_is_rejected) {
  OOHeadlessOptions options;
  options.headless = true;
  options.addOns = OOHeadlessOptions::AddOns::kNone;
  OO_CHECK_EQ(ValidationFailure(options),
              std::string("headless true requires seed to be given"));
  options.seed = 20260910;
  OO_CHECK_EQ(ValidationFailure(options), std::string());
}

OO_TEST(headless_with_all_addons_is_rejected) {
  OOHeadlessOptions options;
  options.headless = true;
  options.seed = 20260910;
  OO_CHECK_EQ(ValidationFailure(options),
              std::string("addOns is never all when headless is true"));
}

OO_TEST(the_manual_clock_moves_only_when_advanced) {
  OOGameClock clock = OOGameClock::manual(20260910);
  OO_CHECK(clock.isManual());
  OO_CHECK_EQ(clock.monotonicSeconds(), 0.0);
  const double startOfDay = OOGameClock::manualTimeOfDayStart(20260910);
  OO_CHECK_EQ(clock.timeOfDay(), startOfDay);
  OO_CHECK(startOfDay >= 0.0 && startOfDay < 86400.0);
  OO_CHECK_EQ(clock.monotonicSeconds(), 0.0);  // reading does not move it
  clock.advance(0.5);
  clock.advance(0.25);
  OO_CHECK_EQ(clock.monotonicSeconds(), 0.75);
  OO_CHECK_EQ(clock.timeOfDay(), startOfDay + 0.75);
  clock.advance(86400.0);  // a day later it is the same time of day
  OO_CHECK(clock.timeOfDay() - (startOfDay + 0.75) < 1e-6 &&
           (startOfDay + 0.75) - clock.timeOfDay() < 1e-6);
  // The system clock is not moved by advance.
  OOGameClock system = OOGameClock::system();
  OO_CHECK(!system.isManual());
  const double before = system.monotonicSeconds();
  system.advance(1000.0);
  OO_CHECK(system.monotonicSeconds() - before < 1000.0);
}

OO_TEST_MAIN()
