/*

OOALMusic.m


OOALSound - OpenAL sound implementation for Oolite.
Copyright (C) 2005-2013 Jens Ayton

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

#include "ALMusic.h"

#include <memory>
#include <string>

namespace {

oo::Music* sPlayingMusic = nullptr;

// The music's one source, made on first play and never released. A message to
// it while there was none did nothing, so each use checks.
oo::SoundSource* sMusicSource = nullptr;

}  // namespace

namespace oo {

Music::~Music() {
  if (sPlayingMusic == this) stop();
}

}  // namespace oo

std::shared_ptr<oo::Music> oo::Music::initWithContentsOfFile(
    const std::optional<std::string>& inPath) {
  // OOSound's designated initializer, overridden
  std::shared_ptr<oo::Music> self =
      std::shared_ptr<oo::Music>(new oo::Music(), oo::Dispose{});
  {
    self->sound = oo::Sound::initWithContentsOfFile(inPath);
    if (!self->sound) {
      self = nullptr;
    }
  }

  return self;
}

std::optional<std::string> oo::Music::name() {
  return sound != nullptr ? sound->name() : std::nullopt;
}

void oo::Music::setMusicGain(float newValue) {
  if (nullptr != sMusicSource) {
    sMusicSource->setGain(newValue);
  }
}

float oo::Music::musicGain() {
  if (nullptr == sMusicSource) return 0.0f;
  return sMusicSource->gain();
}

void oo::Music::playLooped(bool inLoop) {
  if (sPlayingMusic != this) {
    if (nullptr == sMusicSource) {
      static std::shared_ptr<oo::SoundSource>* sMusicSourceHolder =
          nullptr;  // never deleted
      sMusicSourceHolder = new std::shared_ptr<oo::SoundSource>(
          oo::makeShared<oo::SoundSource>());
      sMusicSource = sMusicSourceHolder->get();
    }
    sMusicSource->stop();
    sMusicSource->setLoop(inLoop);
    sMusicSource->setSound(sound.get());
    sMusicSource->play();

    sPlayingMusic = this;
  }
}

oo::SoundSource* oo::Music::musicSoundSource() { return sMusicSource; }

bool oo::Music::isPlaying() {
  return sPlayingMusic == this && sMusicSource != nullptr &&
         sMusicSource->isPlaying();
}

void oo::Music::stop() {
  if (sPlayingMusic == this) {
    sPlayingMusic = nullptr;
    if (sMusicSource != nullptr) {
      sMusicSource->stop();
      sMusicSource->setSound(nullptr);
    }
  }
}
