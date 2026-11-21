/*

OOOpenALController.cpp


Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#include "OpenALController.h"

#include <memory>
#include <utility>

#include "ALSoundMixer.h"
#include "oofnd/Log.hpp"
#include "oofnd/Process.hpp"

namespace {

// +1, never released (the retained singleton)
oo::OpenALController* sSingleton = nullptr;

}  // namespace

oo::OpenALController* oo::OpenALController::sharedController() {
  if (sSingleton == nullptr) {
    // -init's [self release]; return nil; is init() answering false: the
    // controller is freed.
    std::shared_ptr<oo::OpenALController> controller =
        oo::makeShared<oo::OpenALController>();
    // Never destroyed: a heap std::shared_ptr that is never deleted.
    if (controller->init()) {
      // never deleted
      static std::shared_ptr<oo::OpenALController>* sSingletonHolder = nullptr;
      sSingletonHolder =
          new std::shared_ptr<oo::OpenALController>(std::move(controller));
      sSingleton = sSingletonHolder->get();
    }
  }

  return sSingleton;
}

bool oo::OpenALController::init() {
  if (oo::process::hasArgument("-nosound") ||
      oo::process::hasArgument("--nosound")) {
    return false;
  }

  ALuint error = AL_NO_ERROR;
  device = alcOpenDevice(nullptr);  // default device
  if (!device) {
    OO_LOG(oo::kLogSoundInitError, "{}", "Failed to open default sound device");
    return false;
  }
  context = alcCreateContext(device, nullptr);  // default context
  if (!alcMakeContextCurrent(context)) {
    OO_LOG(oo::kLogSoundInitError, "{}",
           "Failed to create default sound context");
    return false;
  }
  // out of the condition (clang-tidy bugprone-assignment-in-if-condition)
  error = static_cast<ALuint>(alGetError());
  if (error != AL_NO_ERROR) {
    OO_LOG(oo::kLogSoundInitError, "Error {} creating sound context", error);
  }
  OOAL(alDistanceModel(AL_NONE));
  return true;
}

void oo::OpenALController::setMasterVolume(ALfloat fraction) {
  OOAL(alListenerf(AL_GAIN, fraction));
}

ALfloat oo::OpenALController::masterVolume() {
  ALfloat fraction = 0.0;
  OOAL(alGetListenerf(AL_GAIN, &fraction));
  return fraction;
}

// only to be called at app shutdown
// is there a better way to handle this?
void oo::OpenALController::shutdown() {
  if (oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer()) mixer->shutdown();
  OOAL(alcMakeContextCurrent(nullptr));
  OOAL(alcDestroyContext(context));
  OOAL(alcCloseDevice(device));
}
