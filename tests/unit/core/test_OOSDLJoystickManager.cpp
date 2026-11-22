/*	test_OOSDLJoystickManager.mm
        Unit tests for OOSDLJoystickManager (src/SDL/OOSDLJoystickManager.h),
   the OOJoystickManager subclass: the class derives from the C++
   OOJoystickManager and the tests call its members.

        The SDL joystick layer runs against one SDL virtual joystick
   (SDL_AttachVirtualJoystick), with every hardware driver hinted off so a pad
   plugged into the machine cannot change the answers. The stick is opened and
   named, SDL ids map to stick indices, the three event makers copy the event
   and translate its id, the axis is read from SDL, and handleSDLEvent: routes
   an event of a known stick to the superclass's decoder (whose state the test
   reads back) and refuses everything else. Run: bash tests/run.sh unit
*/

#include <SDL3/SDL.h>  // IWYU pragma: keep

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "SDLJoystickManager.h"

namespace {

const char* const kStickName = "virtual stick";

// The virtual stick, attached once before any manager is made (a manager opens
// the sticks that exist when it is initialised), and a handle of the test's own
// to drive its axes.
struct VirtualStick {
  SDL_JoystickID id = 0;
  SDL_Joystick* handle = nullptr;
};

VirtualStick& Stick() {
  static VirtualStick* stick = [] {
    auto* s = new VirtualStick;
    SDL_SetHint(SDL_HINT_JOYSTICK_RAWINPUT, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_DIRECTINPUT, "0");
    SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_WGI, "0");
    if (!SDL_Init(SDL_INIT_JOYSTICK)) return s;

    SDL_VirtualJoystickDesc desc;
    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.naxes = 2;
    desc.nbuttons = 4;
    desc.nhats = 1;
    desc.name = kStickName;
    s->id = SDL_AttachVirtualJoystick(&desc);
    if (s->id != 0) s->handle = SDL_OpenJoystick(s->id);
    return s;
  }();
  return *stick;
}

SDL_Event AxisEvent(SDL_JoystickID which, Uint8 axis, Sint16 value) {
  SDL_Event evt = {};
  evt.jaxis.type = SDL_EVENT_JOYSTICK_AXIS_MOTION;
  evt.jaxis.which = which;
  evt.jaxis.axis = axis;
  evt.jaxis.value = value;
  return evt;
}

SDL_Event ButtonEvent(SDL_JoystickID which, Uint8 button, bool down) {
  SDL_Event evt = {};
  evt.jbutton.type =
      down ? SDL_EVENT_JOYSTICK_BUTTON_DOWN : SDL_EVENT_JOYSTICK_BUTTON_UP;
  evt.jbutton.which = which;
  evt.jbutton.button = button;
  evt.jbutton.down = down;
  return evt;
}

SDL_Event HatEvent(SDL_JoystickID which, Uint8 hat, Uint8 value) {
  SDL_Event evt = {};
  evt.jhat.type = SDL_EVENT_JOYSTICK_HAT_MOTION;
  evt.jhat.which = which;
  evt.jhat.hat = hat;
  evt.jhat.value = value;
  return evt;
}

// As +alloc then -init made it: the SDL constructor, then the superclass's
// -init body.
std::shared_ptr<oo::SDLJoystickManager> NewManager() {
  std::shared_ptr<oo::SDLJoystickManager> manager = oo::makeShared<oo::SDLJoystickManager>();
  manager->init();
  return manager;
}

// The factory the game registers (MyOpenGLView.mm).
std::shared_ptr<oo::JoystickManager> MakeSDLManager() {
  return oo::makeShared<oo::SDLJoystickManager>();
}

}  // namespace

TEST(OOSDLJoystickManager, opensAndNamesTheSticks) {
  VirtualStick& vs = Stick();
  EXPECT_TRUE(vs.id != 0 && vs.handle != nullptr);

  {
    std::shared_ptr<oo::SDLJoystickManager> manager = NewManager();
    EXPECT_TRUE(manager != nullptr);
    EXPECT_TRUE(manager->joystickCount() == 1);
    EXPECT_TRUE(manager->nameOfJoystick(0) ==
             std::optional<std::string>(kStickName));
    EXPECT_TRUE(manager->nameOfJoystick(1) ==
             std::optional<std::string>("(unknown joystick)"));
    EXPECT_TRUE(manager->nameOfJoystick(MAX_STICKS) ==
             std::optional<std::string>("(unknown joystick)"));

    // The superclass lists the sticks through the overrides.
    EXPECT_TRUE(manager->listSticks() == std::vector<std::string>({kStickName}));

    EXPECT_TRUE(manager->getJoystickIndexFromId(vs.id) == 0);
    EXPECT_TRUE(manager->getJoystickIndexFromId(vs.id + 100) == -1);
  }
}

TEST(OOSDLJoystickManager, makesEvents) {
  VirtualStick& vs = Stick();
  {
    std::shared_ptr<oo::SDLJoystickManager> manager = NewManager();

    SDL_Event axis = AxisEvent(vs.id, 1, -1234);
    SDL_JoyAxisEvent a = manager->makeJoyAxisEvent(&axis.jaxis);
    EXPECT_TRUE(a.type == SDL_EVENT_JOYSTICK_AXIS_MOTION && a.which == 0 &&
             a.axis == 1 && a.value == -1234);

    SDL_Event button = ButtonEvent(vs.id, 3, true);
    SDL_JoyButtonEvent b = manager->makeJoyButtonEvent(&button.jbutton);
    EXPECT_TRUE(b.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN && b.which == 0 &&
             b.button == 3 && b.down);

    SDL_Event hat = HatEvent(vs.id, 0, SDL_HAT_UP);
    SDL_JoyHatEvent h = manager->makeJoyHatEvent(&hat.jhat);
    EXPECT_TRUE(h.type == SDL_EVENT_JOYSTICK_HAT_MOTION && h.which == 0 &&
             h.hat == 0 && h.value == SDL_HAT_UP);

    // An unknown stick's id becomes -1 (as an SDL_JoystickID, which is what the
    // caller tests).
    SDL_Event stray = AxisEvent(vs.id + 100, 0, 1);
    EXPECT_TRUE((int)manager->makeJoyAxisEvent(&stray.jaxis).which == -1);
  }
}

TEST(OOSDLJoystickManager, readsAxesFromSDL) {
  VirtualStick& vs = Stick();
  {
    std::shared_ptr<oo::SDLJoystickManager> manager = NewManager();
    SDL_SetJoystickVirtualAxis(vs.handle, 0, 5000);
    SDL_SetJoystickVirtualAxis(vs.handle, 1, -300);
    SDL_UpdateJoysticks();
    EXPECT_TRUE(manager->getAxisWithStick(0, 0) == 5000);
    EXPECT_TRUE(manager->getAxisWithStick(0, 1) == -300);
  }
}

TEST(OOSDLJoystickManager, handlesEventsThroughTheSuperclass) {
  VirtualStick& vs = Stick();
  {
    // As the game makes it: the SDL class registered with the superclass, which
    // creates it.
    EXPECT_TRUE(oo::JoystickManager::setStickHandlerFactory(MakeSDLManager));
    oo::SDLJoystickManager* manager = dynamic_cast<oo::SDLJoystickManager*>(
        oo::JoystickManager::sharedStickHandler());
    EXPECT_TRUE(manager != nullptr);
    EXPECT_TRUE(manager->joystickCount() == 1);

    SDL_Event down = ButtonEvent(vs.id, 2, true);
    EXPECT_TRUE(manager->handleSDLEvent(&down));
    EXPECT_TRUE(manager->isButtonDown(2, 0));
    SDL_Event up = ButtonEvent(vs.id, 2, false);
    EXPECT_TRUE(manager->handleSDLEvent(&up));
    EXPECT_TRUE(!manager->isButtonDown(2, 0));

    // A hat is decoded as four buttons past the real ones.
    SDL_Event hat = HatEvent(vs.id, 0, SDL_HAT_UP);
    EXPECT_TRUE(manager->handleSDLEvent(&hat));
    EXPECT_TRUE(manager->isButtonDown(MAX_REAL_BUTTONS, 0));
    SDL_Event centred = HatEvent(vs.id, 0, SDL_HAT_CENTERED);
    EXPECT_TRUE(manager->handleSDLEvent(&centred));
    EXPECT_TRUE(!manager->isButtonDown(MAX_REAL_BUTTONS, 0));

    SDL_Event axis = AxisEvent(vs.id, 0, 100);
    EXPECT_TRUE(manager->handleSDLEvent(&axis));

    // An event it does not handle is refused. (Not pinned: an event of a stick
    // it did not open. Its index is -1 in an unsigned SDL_JoystickID, so the
    // `which >= 0` guard lets it through to the decoder, which indexes its
    // tables with it.)
    SDL_Event key = {};
    key.type = SDL_EVENT_KEY_DOWN;
    EXPECT_TRUE(!manager->handleSDLEvent(&key));
  }
}

// An event of a stick the manager did not open (one hot-plugged after it was
// made) is refused and leaves the superclass's state alone. Its index, -1, used
// to be stored in the unsigned SDL_JoystickID, so the `which >= 0` guard let it
// through to the decoder, which indexed its per-stick tables with it.
TEST(OOSDLJoystickManager, refusesEventsOfAStickItDidNotOpen) {
  VirtualStick& vs = Stick();
  const SDL_JoystickID stray = vs.id + 100;
  {
    std::shared_ptr<oo::SDLJoystickManager> manager = NewManager();
    EXPECT_TRUE(manager->getJoystickIndexFromId(stray) == -1);

    SDL_Event down = ButtonEvent(vs.id, 0, true);
    EXPECT_TRUE(manager->handleSDLEvent(&down));
    EXPECT_TRUE(manager->isButtonDown(0, 0));

    SDL_Event strayUp = ButtonEvent(stray, 0, false);
    EXPECT_TRUE(!manager->handleSDLEvent(&strayUp));
    EXPECT_TRUE(manager->isButtonDown(0, 0));

    SDL_Event strayHat = HatEvent(stray, 0, SDL_HAT_UP);
    EXPECT_TRUE(!manager->handleSDLEvent(&strayHat));
    EXPECT_TRUE(!manager->isButtonDown(MAX_REAL_BUTTONS, 0));

    SDL_Event strayAxis = AxisEvent(stray, 0, 100);
    EXPECT_TRUE(!manager->handleSDLEvent(&strayAxis));

    SDL_Event up = ButtonEvent(vs.id, 0, false);
    EXPECT_TRUE(manager->handleSDLEvent(&up));
    EXPECT_TRUE(!manager->isButtonDown(0, 0));
  }
}

// Upstream #783: an SDL button or hat event of a button given a GUI function
// sets that function's state, which the GUI screens read.
TEST(OOSDLJoystickManager, decodesGuiButtonsFromSDL) {
  VirtualStick& vs = Stick();
  {
    std::shared_ptr<oo::SDLJoystickManager> manager = NewManager();
    auto buttonFn = [](int button) {
      oo::PList::Dict fn;
      fn[std::string(STICK_ISAXIS)] = oo::PList(false);
      fn[std::string(STICK_NUMBER)] = oo::PList(0);
      fn[std::string(STICK_AXBUT)] = oo::PList(button);
      return oo::PList(std::move(fn));
    };
    manager->setFunction(BUTTON_GUISELECT, buttonFn(3));
    manager->setFunction(BUTTON_GUIUP, buttonFn(MAX_REAL_BUTTONS));
    EXPECT_TRUE(!manager->getButtonState(BUTTON_GUISELECT));

    SDL_Event down = ButtonEvent(vs.id, 3, true);
    EXPECT_TRUE(manager->handleSDLEvent(&down));
    EXPECT_TRUE(manager->getButtonState(BUTTON_GUISELECT));
    EXPECT_TRUE(manager->getAllButtonStates()[BUTTON_GUISELECT]);
    SDL_Event up = ButtonEvent(vs.id, 3, false);
    EXPECT_TRUE(manager->handleSDLEvent(&up));
    EXPECT_TRUE(!manager->getButtonState(BUTTON_GUISELECT));

    SDL_Event hat = HatEvent(vs.id, 0, SDL_HAT_UP);
    EXPECT_TRUE(manager->handleSDLEvent(&hat));
    EXPECT_TRUE(manager->getButtonState(BUTTON_GUIUP));
    SDL_Event centred = HatEvent(vs.id, 0, SDL_HAT_CENTERED);
    EXPECT_TRUE(manager->handleSDLEvent(&centred));
    EXPECT_TRUE(!manager->getButtonState(BUTTON_GUIUP));
  }
}
