/*

OOALMusic.h

Subclass of OOSound with additional controls specific to music playback. Only
one instance of OOMusic may be playing at a time.

The class is OOMusic, a subclass of OOSound.


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

#pragma once

#include <memory>
#include <string>

#include "ALSound.h"
#include "Foundation.h"
#include "SoundSource.h"

namespace oo {

class Music : public Sound {
 public:
  /*	Was -initWithContentsOfFile:, OOSound's designated initialiser
     overridden: a music that wraps the sound the root's class cluster loads for
     the path. Null where it answered nil. It hides the root's factory of the
     same name.
  */
  static std::shared_ptr<Music> initWithContentsOfFile(
      const std::optional<std::string>& inPath);

  ~Music() override;

  std::optional<std::string> name() override;

  void playLooped(bool looped);
  void stop();
  bool isPlaying();
  void setMusicGain(float newValue);
  float musicGain();
  SoundSource* musicSoundSource();

 private:
  Music() = default;

  // The sound the root's cluster made for the path, retained as before.
  std::shared_ptr<Sound> sound;
};

}  // namespace oo
