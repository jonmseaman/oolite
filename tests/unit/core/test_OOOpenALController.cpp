/*	test_OOOpenALController.mm
        Unit tests for OOOpenALController (src/Core/OOOpenALController.h).

        OpenAL runs on OpenAL Soft's null backend (ALSOFT_DRIVERS=null, set
   before the first ALC call), so the machine's sound hardware cannot change the
   answers and a machine without any still opens a device. The expectations
   cover: -nosound and --nosound refuse the controller, and it is asked again on
   the next call; otherwise the one shared controller opens a device, makes its
   context current with no distance model, reads and writes the listener's gain,
   and shutdown tells the sound mixer and releases the context. OOSoundMixer is
   this file's stub, which counts the shutdown it is sent (the real mixer would
   drag the whole sound system into the link). Run: bash
   tests/run.sh unit
*/

#include <cmath>
#include <cstdlib>  // IWYU pragma: keep

#include <gtest/gtest.h>

#include "ALSoundMixer.h"
#include "OpenALController.h"
#include "oofnd/Process.hpp"

static unsigned sMixerShutdowns = 0;

// The mixer (a C++ stand-in): one, never released, counting the shutdowns the
// controller sends it.
oo::SoundMixer* oo::SoundMixer::sharedMixer() {
  static oo::SoundMixer* mixer = (*new std::shared_ptr<oo::SoundMixer>(oo::makeShared<oo::SoundMixer>())).get();
  return mixer;
}

void oo::SoundMixer::shutdown() { sMixerShutdowns++; }

namespace {

void SetArguments(std::initializer_list<const char*> args) {
  std::vector<const char*> argv(args);
  oo::process::setArguments(static_cast<int>(argv.size()), argv.data());
}

bool Near(float a, float b) { return std::fabs(a - b) < 1e-6f; }

}  // namespace

TEST(OOOpenALController, noSoundArgumentsRefuseTheController) {
  _putenv_s("ALSOFT_DRIVERS", "null");

  SetArguments({"test_OOOpenALController", "-nosound"});
  EXPECT_TRUE(oo::OpenALController::sharedController() == nullptr);
  EXPECT_TRUE(oo::OpenALController::sharedController() == nullptr);
  SetArguments({"test_OOOpenALController", "--nosound"});
  EXPECT_TRUE(oo::OpenALController::sharedController() == nullptr);
  EXPECT_TRUE(alcGetCurrentContext() == nullptr);  // no device was opened

  // A refusal is not remembered: the next call asks again.
  SetArguments({"test_OOOpenALController"});
  EXPECT_TRUE(oo::OpenALController::sharedController() != nullptr);
}

TEST(OOOpenALController, opensTheDefaultDevice) {
  oo::OpenALController* controller = oo::OpenALController::sharedController();
  EXPECT_TRUE(controller != nullptr);
  EXPECT_TRUE(oo::OpenALController::sharedController() == controller);

  ALCcontext* context = alcGetCurrentContext();
  EXPECT_TRUE(context != nullptr);
  ALCdevice* device =
      context != nullptr ? alcGetContextsDevice(context) : nullptr;
  EXPECT_TRUE(device != nullptr);
  const ALCchar* name = device != nullptr
                            ? alcGetString(device, ALC_ALL_DEVICES_SPECIFIER)
                            : nullptr;
  EXPECT_TRUE(name != nullptr &&
           std::string(name).find("No Output") != std::string::npos);
  EXPECT_TRUE(alGetInteger(AL_DISTANCE_MODEL) == AL_NONE);
  EXPECT_TRUE(alGetError() == AL_NO_ERROR);
}

TEST(OOOpenALController, masterVolumeIsTheListenerGain) {
  oo::OpenALController* controller = oo::OpenALController::sharedController();
  EXPECT_TRUE(Near(controller->masterVolume(), 1.0f));  // OpenAL's default gain

  controller->setMasterVolume(0.25f);
  EXPECT_TRUE(Near(controller->masterVolume(), 0.25f));
  ALfloat gain = 0.0f;
  alGetListenerf(AL_GAIN, &gain);
  EXPECT_TRUE(Near(gain, 0.25f));

  alListenerf(AL_GAIN, 0.75f);
  EXPECT_TRUE(Near(controller->masterVolume(), 0.75f));

  // An invalid gain leaves the gain alone. OOAL clears the error before the
  // call, not after.
  controller->setMasterVolume(-1.0f);
  EXPECT_TRUE(alGetError() == AL_INVALID_VALUE);
  EXPECT_TRUE(Near(controller->masterVolume(), 0.75f));
  alGetError();
  controller->setMasterVolume(0.5f);
  EXPECT_TRUE(alGetError() == AL_NO_ERROR);
}

// Last: it destroys the shared controller's context.
TEST(OOOpenALController, shutdownStopsTheMixerAndReleasesTheContext) {
  oo::OpenALController* controller = oo::OpenALController::sharedController();
  EXPECT_TRUE(alcGetCurrentContext() != nullptr);
  EXPECT_TRUE(sMixerShutdowns == 0);

  controller->shutdown();
  EXPECT_TRUE(sMixerShutdowns == 1);
  EXPECT_TRUE(alcGetCurrentContext() == nullptr);

  // The shared controller is not replaced.
  EXPECT_TRUE(oo::OpenALController::sharedController() == controller);
}
