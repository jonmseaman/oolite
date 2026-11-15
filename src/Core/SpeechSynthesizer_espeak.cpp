/*

SpeechSynthesizer_espeak.cpp

oo::SpeechSynthesizer's calls into espeak-ng: the one file that includes it.

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

#include <espeak-ng/speak_lib.h>

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include "SpeechSynthesizer.h"

namespace oo {

void SpeechSynthesizer::initialize(const std::optional<std::string>& dataPath) {
  espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 100,
                    dataPath.has_value() ? dataPath->c_str() : nullptr, 0);
}

void SpeechSynthesizer::loadVoices(int volumePercent) {
  espeak_SetParameter(espeakPUNCTUATION, espeakPUNCT_NONE, 0);
  espeak_SetParameter(espeakVOLUME, volumePercent, 0);
  _voiceNames.clear();
  for (const espeak_VOICE** voices = espeak_ListVoices(nullptr);
       voices != nullptr && *voices != nullptr; ++voices)
    _voiceNames.emplace_back((*voices)->name);
}

void SpeechSynthesizer::setVolume(int volumePercent) {
  espeak_SetParameter(espeakVOLUME, volumePercent, 0);
}

void SpeechSynthesizer::speak(const std::string& text) {
  // the UTF-8 bytes of the text, up to its first NUL (as strlen measured it)
  espeak_Synth(
      text.c_str(), std::string_view(text.c_str()).size() + 1 /* inc. NULL */,
      0, POS_CHARACTER, 0, espeakCHARS_UTF8 | espeakPHONEMES | espeakENDPAUSE,
      nullptr, nullptr);
}

void SpeechSynthesizer::stop() { espeak_Cancel(); }

bool SpeechSynthesizer::isSpeaking() const { return espeak_IsPlaying() != 0; }

std::size_t SpeechSynthesizer::pronunciationColumn(std::size_t pairSize) const {
  return pairSize > 2 ? 2 : 1;
}

void SpeechSynthesizer::selectVoice(const std::string& name, bool isMale) {
  espeak_VOICE voice = {name.c_str(),
                        nullptr,
                        nullptr,
                        static_cast<unsigned char>(isMale ? 1 : 2),
                        0,
                        0,
                        0,
                        0,
                        nullptr};
  espeak_SetVoiceByProperties(&voice);
}

}  // namespace oo
