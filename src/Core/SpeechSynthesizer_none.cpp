/*

SpeechSynthesizer_none.cpp

oo::SpeechSynthesizer in a build without espeak-ng: silent, never speaking,
no voices.

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

#include <cstddef>
#include <optional>
#include <string>

#include "SpeechSynthesizer.h"

namespace oo {

void SpeechSynthesizer::initialize(
    const std::optional<std::string>& /*dataPath*/) {}

void SpeechSynthesizer::loadVoices(int /*volumePercent*/) {}

void SpeechSynthesizer::setVolume(int /*volumePercent*/) {}

void SpeechSynthesizer::speak(const std::string& /*text*/) {}

void SpeechSynthesizer::stop() {}

bool SpeechSynthesizer::isSpeaking() const { return false; }

// Without espeak the plain replacement, column 1, is the one there is.
std::size_t SpeechSynthesizer::pronunciationColumn(
    std::size_t /*pairSize*/) const {
  return 1;
}

void SpeechSynthesizer::selectVoice(const std::string& /*name*/,
                                    bool /*isMale*/) {}

}  // namespace oo
