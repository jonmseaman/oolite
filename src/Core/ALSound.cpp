/*

OOALSound.m

OOALSound - OpenAL sound implementation for Oolite.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "ALSound.h"

#include <cstddef>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>

#include "ALBufferedSound.h"
#include "ALSoundDecoder.h"
#include "ALSoundMixer.h"
#include "ALStreamedSound.h"
#include "Demangle.h"
#include "FastArithmetic.h"
#include "Logging.h"
#include "OpenALController.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/String.hpp"
static constexpr std::string_view KEY_VOLUME_CONTROL = "volume_control";

static const size_t kMaxBufferedSoundSize = 1 << 20;  // 1 MB

namespace {

bool sIsSetUp = false;
bool sIsSoundOK = false;

}  // namespace

bool oo::Sound::setUp() {
  if (!sIsSetUp) {
    sIsSetUp = true;
    oo::OpenALController* controller = oo::OpenALController::sharedController();
    if (controller != nullptr) {
      sIsSoundOK = true;
      oo::Defaults& prefs = oo::Defaults::standard();
      float volume = prefs.object(KEY_VOLUME_CONTROL).isNull()
                         ? 0.5f
                         : prefs.floatForKey(KEY_VOLUME_CONTROL);
      setMasterVolume(volume);
    }
  }

  return sIsSoundOK;
}

void oo::Sound::setMasterVolume(float fraction) {
  if (!sIsSetUp && !setUp()) return;

  fraction = oo::Clamp_0_1_f(fraction);

  // A null controller answers 0, as a message to nil did (sound set up but
  // refused).
  oo::OpenALController* controller = oo::OpenALController::sharedController();
  if (fraction != (controller != nullptr ? controller->masterVolume() : 0.0f)) {
    if (controller != nullptr) controller->setMasterVolume(fraction);
    oo::Defaults::standard().setFloat(
        KEY_VOLUME_CONTROL,
        controller != nullptr ? controller->masterVolume() : 0.0f);
  }
}

float oo::Sound::masterVolume() {
  if (!sIsSetUp && !setUp()) return 0.0;

  oo::OpenALController* controller = oo::OpenALController::sharedController();
  return controller != nullptr ? controller->masterVolume() : 0.0f;
}

oo::Sound::Sound() {
  if (!sIsSetUp) setUp();
}

/*	The body of -initWithContentsOfFile: after the receiver's release:
   the concrete sound it makes. The decoder is released at the end of the
   scope, as [decoder release] did.
*/

namespace {
bool sNamedWithoutDevice = false;  // oo::Sound::setNamedWithoutDevice

// What a headless game loads in place of a sound: a name and no data.
class OONamedSilentSound final : public oo::Sound {
 public:
  explicit OONamedSilentSound(std::string name) : _name(std::move(name)) {}

  std::optional<std::string> name() override { return _name; }

  ALuint soundBuffer() override { return 0; }

 private:
  std::string _name;
};
}  // namespace

void oo::Sound::setNamedWithoutDevice(bool named) {
  sNamedWithoutDevice = named;
}

std::shared_ptr<oo::Sound> oo::Sound::initWithContentsOfFile(
    const std::optional<std::string>& path) {
  if (!sIsSoundOK) {
    if (sNamedWithoutDevice && path.has_value()) {
      return oo::makeShared<OONamedSilentSound>(
          oo::str::lastPathComponent(*path));
    }
    return nullptr;
  }

  if (!sIsSetUp && !setUp()) return nullptr;

  std::shared_ptr<oo::Sound> self;

  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::initWithPath(path);
  if (nullptr == decoder) return nullptr;

  if (decoder->sizeAsBuffer() <= kMaxBufferedSoundSize) {
    self = oo::ALBufferedSound::initWithDecoder(decoder.get());
  } else {
    self = oo::ALStreamedSound::initWithDecoder(decoder.get());
  }

  if (nullptr != self) {
#ifndef NDEBUG
    OO_LOG(oo::kLogSoundLoadingSuccess, "Loaded sound {}",
           path.value_or("(null)"));
#endif
  } else {
    OO_LOG(oo::kLogSoundLoadingError, "Failed to load sound \"{}\"",
           path.value_or("(null)"));
  }

  return self;
}

std::optional<std::string> oo::Sound::name() {
  OOLogGenericSubclassResponsibility();
  return std::string();
}

void oo::Sound::update() {
  ::oo::SoundMixer* mixer = ::oo::SoundMixer::sharedMixer();
  if (sIsSoundOK && mixer) mixer->update();
}

bool oo::Sound::isSoundOK() { return sIsSoundOK; }

ALuint oo::Sound::soundBuffer() {
  OOLogGenericSubclassResponsibility();
  return 0;
}

bool oo::Sound::soundIncomplete() { return false; }

void oo::Sound::rewind() {
  // doesn't need to do anything on seekable FDs
}

std::optional<std::string> oo::Sound::descriptionComponents() const {
  return std::nullopt;
}

// The C++ class's name, without a namespace, its address and its components.
std::string oo::Sound::description() const {
  std::string name = oo::DescriptionClassName(typeid(*this).name());
  const std::size_t colons = name.rfind("::");
  if (colons != std::string::npos) name.erase(0, colons + 2);

  std::string result =
      std::format("<{} {}>", name, oo::str::pointerDescription(this));
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}
