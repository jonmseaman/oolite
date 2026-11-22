/*	test_OOJoystickDecoding.cpp
        Unit tests for where the joystick code keeps SDL (upstream #742): the
   core OOJoystickManager (src/Core/OOJoystickManager.h) has the tables and
   states and nothing of SDL's, and the SDL manager
   (src/SDL/OOSDLJoystickManager.h) decodes SDL's own joystick events into
   them. And for the one dead zone #742 left: STICK_DEADZONE is 0.05 on every
   platform (it was 0.0025 on some), its maximum twice that, and a new axis
   profile starts at it. The layout cases are compile-time checks; the dead
   zone case runs the profile (OOJoystickProfile.cpp). Run: bash
   meson test -C build/meson_test test_OOJoystickDecoding
*/

// The core header first, on its own: it must not bring in SDL's event header.

#include <gtest/gtest.h>

#include "JoystickManager.h"

#ifdef SDL_events_h_
constexpr bool kCoreManagerIncludesSDLEvents = true;
#else
constexpr bool kCoreManagerIncludesSDLEvents = false;
#endif

#include <type_traits>

#include "JoystickProfile.h"
#include "SDLJoystickManager.h"

namespace {

template <class T>
constexpr bool kDecodesAxes = requires(T* manager, SDL_JoyAxisEvent* evt) {
  manager->decodeAxisEvent(evt);
};
template <class T>
constexpr bool kDecodesButtons = requires(T* manager, SDL_JoyButtonEvent* evt) {
  manager->decodeButtonEvent(evt);
};
template <class T>
constexpr bool kDecodesHats = requires(T* manager, SDL_JoyHatEvent* evt) {
  manager->decodeHatEvent(evt);
};

}  // namespace

// The core manager decodes nothing and includes nothing of SDL's events.
TEST(OOJoystickDecoding, coreManagerHasNoSDLDecoding) {
  EXPECT_TRUE(!kCoreManagerIncludesSDLEvents);
  EXPECT_TRUE(!kDecodesAxes<oo::JoystickManager>);
  EXPECT_TRUE(!kDecodesButtons<oo::JoystickManager>);
  EXPECT_TRUE(!kDecodesHats<oo::JoystickManager>);
}

// The SDL manager's own members decode SDL's event types, and its event makers
// answer SDL's types.
TEST(OOJoystickDecoding, sdlManagerDecodesSDLEvents) {
  EXPECT_TRUE((std::is_same_v<decltype(&oo::SDLJoystickManager::decodeAxisEvent),
                           void (oo::SDLJoystickManager::*)(SDL_JoyAxisEvent*)>));
  EXPECT_TRUE(
      (std::is_same_v<decltype(&oo::SDLJoystickManager::decodeButtonEvent),
                      void (oo::SDLJoystickManager::*)(SDL_JoyButtonEvent*)>));
  EXPECT_TRUE((std::is_same_v<decltype(&oo::SDLJoystickManager::decodeHatEvent),
                           void (oo::SDLJoystickManager::*)(SDL_JoyHatEvent*)>));
  EXPECT_TRUE((std::is_same_v<decltype(std::declval<oo::SDLJoystickManager&>()
                                        .makeJoyAxisEvent(nullptr)),
                           SDL_JoyAxisEvent>));
  EXPECT_TRUE((std::is_same_v<decltype(std::declval<oo::SDLJoystickManager&>()
                                        .makeJoyButtonEvent(nullptr)),
                           SDL_JoyButtonEvent>));
  EXPECT_TRUE((std::is_same_v<decltype(std::declval<oo::SDLJoystickManager&>()
                                        .makeJoyHatEvent(nullptr)),
                           SDL_JoyHatEvent>));
}

// One dead zone: 0.05, at most 0.1, where a new profile starts.
TEST(OOJoystickDecoding, oneDeadzone) {
  EXPECT_TRUE(STICK_DEADZONE == 0.05);
  EXPECT_TRUE(STICK_MAX_DEADZONE == 0.1);
  std::shared_ptr<oo::JoystickStandardAxisProfile> profile =
      oo::makeShared<oo::JoystickStandardAxisProfile>();
  EXPECT_TRUE(profile->deadzone() == 0.05);
  profile->setDeadzone(1.0);
  EXPECT_TRUE(profile->deadzone() == 0.1);
}
