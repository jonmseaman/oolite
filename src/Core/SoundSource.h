/*

OOSoundSource.h

A sound source.
Each playing sound is associated with a sound source, either explicitly or by
creating one on the fly. Each sound source can play one sound at a time, and
has a number of attributes related to positional audio (which is currently
unimplemented).

The player, the JS SoundSource, the music, the trumbles and the pools hold and
call the C++ source. A playing source is its channel's delegate
(OOSoundChannelDelegate), and keeps itself alive while it plays (a retain of
its own).


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
OUT OF OR

*/

#pragma once

#include <memory>
#include <string>

#include "ALSoundChannel.h"
#include "Maths.h"
#include "SoundSource.h"
#include "oofnd/Object.hpp"

#ifndef OO_DEFAULT_SOUNDSOURCE_GAIN
#define OO_DEFAULT_SOUNDSOURCE_GAIN 1.0f
#endif

namespace oo {

class SoundReferencePoint;

}  // namespace oo
   // C++ (OOBasicSoundReferencePoint.h)
struct OOSoundSourceTestAccess;

namespace oo {

class SoundSource : public oo::Object, public ::oo::SoundChannelDelegate {
 public:
  SoundSource();                         // was -init
  explicit SoundSource(Sound* inSound);  // was -initWithSound:
  ~SoundSource() override;

  static std::shared_ptr<SoundSource> sourceWithSound(Sound* inSound);

  // These options should be set before playing. Effect of setting them while
  // playing is undefined.
  Sound* sound();
  void setSound(Sound* inSound);
  bool loop();
  void setLoop(bool inLoop);
  uint8_t repeatCount();
  void setRepeatCount(uint8_t inCount);

  bool isPlaying();
  void play();
  void playOrRepeat();
  void stop();

  static void stopAll();

  // Conveniences:
  void playOOSound(Sound* inSound);
  void playSound(Sound* inSound, uint8_t inCount);
  void playOrRepeatSound(Sound* inSound);

  // Positional audio attributes are used in this implementation
  void setPositional(bool inPositional);
  bool positional();
  void setPosition(Vector inPosition);
  Vector position();
  void setGain(float gain);
  float gain();

  // *Advanced* positional audio attributes are ignored in this implementation
  void setVelocity(Vector inVelocity);
  void setOrientation(Vector inOrientation);
  void setConeAngle(float inAngle);
  void setGainInsideCone(float inInside, float inOutside);
  void positionRelativeTo(SoundReferencePoint* inPoint);

  // OOSoundChannelDelegate: was -channel:didFinishPlayingSound:.
  void channel(::oo::SoundChannel* channel, Sound* sound) override;

  /*	Was +channel:didFinishPlayingSound:, the class as the delegate of a
     stopped source's channel. Its first keyword is the instance member's, which
     a static member cannot overload. The channel reaches it through the
     stopped-source handler in OOSoundSource.cpp.
  */
  static void channelOfStoppedSource(::oo::SoundChannel* inChannel,
                                     Sound* inSound);

  // What "%@" prints between the braces of <OOSoundSource 0x...>{...}
  // (OODescription.h).
  std::optional<std::string> descriptionComponents() const;

 private:
  std::shared_ptr<Sound> _sound;  // retained
  ::oo::SoundChannel* _channel =
      {};  // borrowed from the mixer, not retained, as before
  std::shared_ptr<SoundSource>
      _playingHold;  // the source's own while a channel plays it (was retain)
  bool _loop = {};
  uint8_t _repeatCount = {}, _remainingCount = {};
  Vector _position = {};
  bool _positional = {};
  float _gain = {};

  friend struct ::OOSoundSourceTestAccess;  // tests only
};

}  // namespace oo
