/*

SpeechSynthesizer.h

The game's text-to-speech: espeak-ng behind one class. The voice-list logic
is in SpeechSynthesizer.cpp; the calls into espeak are in
SpeechSynthesizer_espeak.cpp, and SpeechSynthesizer_none.cpp stands in for
them (silent, never speaking, no voices) when the build has no espeak. Meson
picks one of the two. No espeak header is included here, so nothing that
includes this sees the library.

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

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace oo {

class SpeechSynthesizer {
 public:
  // One for the process: espeak-ng's state is process-wide.
  static SpeechSynthesizer& shared();

  // Start the engine. dataPath is espeak's data directory; nullopt lets
  // espeak find it (ESPEAK_DATA_PATH).
  void initialize(const std::optional<std::string>& dataPath);
  // No punctuation, this volume, and a copy of espeak's voice list.
  void loadVoices(int volumePercent);
  void setVolume(int volumePercent);

  // The text up to its first NUL.
  void speak(const std::string& text);
  void stop();
  bool isSpeaking() const;

  // "-" for an index out of range.
  std::optional<std::string> voiceName(unsigned int index) const;
  // UINT_MAX for a name that is not a voice.
  unsigned int voiceNumber(const std::string& name) const;
  // Both wrap; with no voices, the previous voice of 0 is UINT_MAX.
  unsigned int nextVoice(unsigned int index) const;
  unsigned int prevVoice(unsigned int index) const;
  // UINT_MAX picks the "espeak-default-voice" description's voice. An index
  // out of range selects nothing. Returns the index.
  unsigned int setVoice(unsigned int index, bool isMale);

  // The pronunciation-guide column to speak: 2 (espeak's own replacement)
  // where the pair has one, else 1.
  std::size_t pronunciationColumn(std::size_t pairSize) const;

  // The test seam: a voice list without espeak.
  void setVoiceNamesForTesting(std::vector<std::string> names);

 private:
  SpeechSynthesizer() = default;
  void selectVoice(const std::string& name, bool isMale);

  std::vector<std::string> _voiceNames;  // copied from espeak at loadVoices
};

}  // namespace oo
