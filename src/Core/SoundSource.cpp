/*

OOSoundSource.m


Copyright (C) 2006-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "SoundSource.h"

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ALSoundChannel.h"
#include "ALSoundMixer.h"
#include "oofnd/String.hpp"

namespace {
// The sources that are playing, each once, retained (a Foundation mutable set
// before; sources compare by identity). Created lazily and dropped by +stopAll,
// as the set was.
std::vector<std::shared_ptr<oo::SoundSource>>* sPlayingSoundSources = nullptr;

/*	The class as the delegate of a stopped source's channel
   (+channel:didFinishPlayingSound:, which -stop made the channel's delegate by
   [_channel setDelegate:[OOSoundSource class]]): it hands the channel back to
   the mixer.
*/
class StoppedSourceHandler final : public oo::SoundChannelDelegate {
 public:
  void channel(oo::SoundChannel* inChannel, oo::Sound* inSound) override {
    oo::SoundSource::channelOfStoppedSource(inChannel, inSound);
  }
};

StoppedSourceHandler sStoppedSourceHandler;

// The text "%@" printed for the sound: "(null)" for none.
std::string SoundDescription(const oo::Sound* sound) {
  return (sound != nullptr) ? sound->description() : std::string("(null)");
}
}  // namespace

std::shared_ptr<oo::SoundSource> oo::SoundSource::sourceWithSound(
    oo::Sound* inSound) {
  return oo::makeShared<oo::SoundSource>(inSound);
}

oo::SoundSource::SoundSource() {
  _positional = false;
  _position = kZeroVector;
  _gain = OO_DEFAULT_SOUNDSOURCE_GAIN;
}

oo::SoundSource::SoundSource(oo::Sound* inSound) : oo::SoundSource() {
  setSound(inSound);
}

// [_sound autorelease] is the std::shared_ptr's release: the sounds the game
// plays are kept by the resource manager's cache, and a playing channel retains
// its own.
namespace oo {

SoundSource::~SoundSource() { stop(); }

}  // namespace oo

/*	A const member, so it reads the ivars that -isPlaying, -loop and
   -repeatCount answered.
*/
std::optional<std::string> oo::SoundSource::descriptionComponents() const {
  if (_channel != nullptr) {
    // The channel as %@ printed it:
    // <OOSoundChannel 0x...>.
    const std::string channel = std::format(
        "<OOSoundChannel {}>", oo::str::pointerDescription(_channel));
    return std::format(
        "sound={}, loop={}, repeatCount={}, playing on channel {}",
        SoundDescription(_sound.get()), _loop ? "YES" : "NO",
        static_cast<unsigned int>(_repeatCount ? _repeatCount : 1), channel);
  } else {
    return std::format(
        "sound={}, loop={}, repeatCount={}, not playing",
        SoundDescription(_sound.get()), _loop ? "YES" : "NO",
        static_cast<unsigned int>(_repeatCount ? _repeatCount : 1));
  }
}

oo::Sound* oo::SoundSource::sound() { return _sound.get(); }

void oo::SoundSource::setSound(oo::Sound* sound) {
  if (_sound.get() != sound) {
    stop();
    _sound = oo::share(sound);  // [_sound autorelease]; [sound retain]
  }
}

bool oo::SoundSource::loop() { return _loop; }

void oo::SoundSource::setLoop(bool loop) { _loop = !!loop; }

uint8_t oo::SoundSource::repeatCount() {
  return _repeatCount ? _repeatCount : 1;
}

void oo::SoundSource::setRepeatCount(uint8_t count) { _repeatCount = count; }

bool oo::SoundSource::isPlaying() { return _channel != nullptr; }

/*	[self retain] and [self release] are retain() and release(), which keep
   this source alive while it plays, as the old object kept itself; the playing
   set holds the source.
*/
void oo::SoundSource::play() {
  if (sound() == nullptr) return;

  if (_channel) stop();

  ::oo::SoundMixer* mixer = ::oo::SoundMixer::sharedMixer();
  _channel = mixer != nullptr ? mixer->popChannel() : nullptr;
  if (nullptr != _channel) {
    _remainingCount = repeatCount();
    _channel->setDelegate(this);
    _channel->setPosition(_position);
    _channel->setGain(_gain);
    _channel->playSound(sound(), loop());
    _playingHold = oo::share(this);
  }

  if (sPlayingSoundSources == nullptr) [[unlikely]] {
    sPlayingSoundSources = new std::vector<std::shared_ptr<oo::SoundSource>>();
  }
  if (std::ranges::find(*sPlayingSoundSources, this, [](const auto& owner) {
        return owner.get();
      }) == sPlayingSoundSources->end()) {
    sPlayingSoundSources->emplace_back(oo::share(this));
  }
}

void oo::SoundSource::playOrRepeat() {
  if (!isPlaying())
    play();
  else
    ++_remainingCount;
}

void oo::SoundSource::stop() {
  if (nullptr != _channel) {
    _channel->setDelegate(&sStoppedSourceHandler);
    _channel->stop();
    _channel = nullptr;

    if (sPlayingSoundSources != nullptr) {
      auto it =
          std::ranges::find(*sPlayingSoundSources, this,
                            [](const auto& owner) { return owner.get(); });
      if (it != sPlayingSoundSources->end()) sPlayingSoundSources->erase(it);
    }
    // The playing hold goes at the end of this block and may free this source:
    // nothing after it touches the object.
    const std::shared_ptr<oo::SoundSource> lastHold = std::move(_playingHold);
  }
}

void oo::SoundSource::stopAll() {
  /*	We're not allowed to mutate sPlayingSoundSources during iteration. The
          normal solution would be to copy the set, but since we know it will
          end up empty we may as well use the original set and let a new one be
          set up lazily.
  */
  const std::unique_ptr<std::vector<std::shared_ptr<oo::SoundSource>>> playing(
      std::exchange(sPlayingSoundSources, nullptr));

  if (playing != nullptr) {
    // In the order they started playing (the set's hash order before).
    for (const std::shared_ptr<oo::SoundSource>& source : *playing)
      source->stop();
  }
}

void oo::SoundSource::playOOSound(oo::Sound* sound) {
  playSound(sound, _repeatCount);
}

void oo::SoundSource::playSound(oo::Sound* sound, uint8_t count) {
  stop();
  setSound(sound);
  setRepeatCount(count);
  play();
}

void oo::SoundSource::playOrRepeatSound(oo::Sound* sound) {
  if (_sound.get() != sound)
    playOOSound(sound);
  else
    playOrRepeat();
}

void oo::SoundSource::setPositional(bool inPositional) {
  if (inPositional) {
    _positional = true;
  } else {
    /* OpenAL doesn't easily do non-positional sounds beyond the
     * stereo/mono distinction, but setting the position to the
     * zero vector is probably close enough */
    _positional = false;
    setPosition(kZeroVector);
  }
}

bool oo::SoundSource::positional() { return _positional; }

void oo::SoundSource::setPosition(Vector inPosition) {
  _position = inPosition;
  if (inPosition.x != 0.0 || inPosition.y != 0.0 || inPosition.z != 0.0) {
    _positional = true;
  }
  if (_channel) {
    _channel->setPosition(_position);
  }
}

Vector oo::SoundSource::position() { return _position; }

void oo::SoundSource::setGain(float gain) {
  _gain = gain;
  if (_channel) {
    _channel->setGain(_gain);
  }
}

float oo::SoundSource::gain() { return _gain; }

/* Following not yet implemented */
void oo::SoundSource::setVelocity(Vector /*inVelocity*/) {}

void oo::SoundSource::setOrientation(Vector /*inOrientation*/) {}

void oo::SoundSource::setConeAngle(float /*inAngle*/) {}

void oo::SoundSource::setGainInsideCone(float /*inInside*/,
                                        float /*inOutside*/) {}

void oo::SoundSource::positionRelativeTo(oo::SoundReferencePoint* /*inPoint*/) {
}

// OOSoundChannelDelegate
void oo::SoundSource::channel(::oo::SoundChannel* channel,
                              oo::Sound* /*sound*/) {
  assert(_channel == channel);

  if (--_remainingCount) {
    _channel->playSound(sound(), false);
  } else {
    _channel->setDelegate(nullptr);
    if (::oo::SoundMixer* mixer = ::oo::SoundMixer::sharedMixer())
      mixer->pushChannel(_channel);
    _channel = nullptr;
    // The playing hold goes at the return and may free this source: nothing
    // after it touches the object.
    const std::shared_ptr<oo::SoundSource> lastHold = std::move(_playingHold);
    return;
  }
}

void oo::SoundSource::channelOfStoppedSource(::oo::SoundChannel* inChannel,
                                             oo::Sound* /*inSound*/) {
  // This delegate is used for a stopped source
  if (::oo::SoundMixer* mixer = ::oo::SoundMixer::sharedMixer())
    mixer->pushChannel(inChannel);
}
