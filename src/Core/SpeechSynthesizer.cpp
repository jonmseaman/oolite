/*

SpeechSynthesizer.cpp

The voice list: the index arithmetic over the copy of espeak's voices. The
espeak calls are in SpeechSynthesizer_espeak.cpp (or the silent
SpeechSynthesizer_none.cpp).

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

#include "SpeechSynthesizer.h"

#include <climits>
#include <string>
#include <utility>
#include <vector>

#include "Universe.h"

namespace oo {

SpeechSynthesizer& SpeechSynthesizer::shared() {
  static SpeechSynthesizer sShared;
  return sShared;
}

std::optional<std::string> SpeechSynthesizer::voiceName(
    unsigned int index) const {
  if (index >= _voiceNames.size()) return std::string("-");
  return _voiceNames[index];
}

unsigned int SpeechSynthesizer::voiceNumber(const std::string& name) const {
  unsigned int index = 0;
  while (index < _voiceNames.size() && _voiceNames[index] != name) ++index;
  return (index < _voiceNames.size()) ? index : UINT_MAX;
}

unsigned int SpeechSynthesizer::nextVoice(unsigned int index) const {
  if (++index >= _voiceNames.size()) index = 0;
  return index;
}

unsigned int SpeechSynthesizer::prevVoice(unsigned int index) const {
  const auto count = static_cast<unsigned int>(_voiceNames.size());
  if (--index >= count) index = count - 1;
  return index;
}

unsigned int SpeechSynthesizer::setVoice(unsigned int index, bool isMale) {
  if (index == UINT_MAX)
    index = voiceNumber(oo::LookUpDescriptionPRIV("espeak-default-voice"));

  if (index < _voiceNames.size()) selectVoice(_voiceNames[index], isMale);

  return index;
}

void SpeechSynthesizer::setVoiceNamesForTesting(
    std::vector<std::string> names) {
  _voiceNames = std::move(names);
}

}  // namespace oo
