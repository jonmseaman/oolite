/*

OOMusicController.m

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

#include "MusicController.h"

#include <memory>
#include <string>
#include <utility>

#include "ResourceManager.h"
#include "oofnd/Defaults.hpp"

namespace {

oo::MusicController* sSingleton = nullptr;

}  // namespace

// Values for _special

constexpr int kSpecialNone = 0;
constexpr int kSpecialTheme = kSpecialNone + 1;
constexpr int kSpecialDocking = kSpecialTheme + 1;
constexpr int kSpecialDocked = kSpecialDocking + 1;
constexpr int kSpecialMission = kSpecialDocked + 1;

oo::MusicController* oo::MusicController::sharedController() {
  if (sSingleton == nullptr) {
    static std::shared_ptr<oo::MusicController>* sSingletonHolder =
        nullptr;  // never deleted
    sSingletonHolder = new std::shared_ptr<oo::MusicController>(
        oo::makeShared<oo::MusicController>());
    sSingleton = sSingletonHolder->get();
  }

  return sSingleton;
}

// The body of -init after [super init], which could not fail.
oo::MusicController::MusicController() {
  {
    const std::optional<std::string> modeString =
        oo::Defaults::standard().stringForKey("music mode");
    if (modeString == "off")
      _mode = oo::MusicMode::kOff;
    else if (modeString == "iTunes")
      _mode = oo::MusicMode::kITunes;
    else
      _mode = oo::MusicMode::kOn;

    // Handle unlikely case of taking prefs from iTunes-enabled system to other.
    if (_mode > oo::kMusicModeMax) _mode = oo::kMusicModeMax;

    _missionMusic = "OoliteTheme.ogg";
  }
}

void oo::MusicController::playMusicNamed(const std::string& name, bool loop) {
  playMusicNamed(name, loop, OO_DEFAULT_SOUNDSOURCE_GAIN);
}

void oo::MusicController::playMusicNamed(const std::string& name, bool loop,
                                         float gain) {
  if (isPlaying() && name == playingMusic()) return;

  if (_mode == oo::MusicMode::kOn ||
      (_mode == oo::MusicMode::kITunes && name == "OoliteTheme.ogg")) {
    std::shared_ptr<oo::Music> music =
        ResourceManager::ooMusicNamed(name, std::string("Music"));
    if (music) {
      if (_current) _current->stop();

      music->setMusicGain(oo::Clamp_0_1_f(gain));
      music->playLooped(loop);

      _current = std::move(music);
    }
  }
}

void oo::MusicController::playThemeMusic() {
  _special = kSpecialTheme;
  playMusicNamed("OoliteTheme.ogg", true);
}

void oo::MusicController::playDockingMusic() {
  _special = kSpecialDocking;

  if (_mode == oo::MusicMode::kITunes) {
    playiTunesPlaylist("Oolite-Docking");
  } else {
    playMusicNamed("BlueDanube.ogg", true);
  }
}

void oo::MusicController::playDockedMusic() {
  _special = kSpecialDocked;

  if (_mode == oo::MusicMode::kITunes) {
    playiTunesPlaylist("Oolite-Docked");
  } else {
    playMusicNamed("OoliteDocked.ogg", false);
  }
}

void oo::MusicController::setMissionMusic(
    const std::optional<std::string>& missionMusicName) {
  _missionMusic = missionMusicName;
}

void oo::MusicController::playMissionMusic() {
  if (_missionMusic.has_value()) {
    _special = kSpecialMission;
    playMusicNamed(*_missionMusic, false);
  }
}

// Stop without switching iTunes to in-flight music.
void oo::MusicController::justStop() {
  if (_current) _current->stop();
  _current = nullptr;
  _special = kSpecialNone;
}

void oo::MusicController::stop() {
  justStop();

  if (_mode == oo::MusicMode::kITunes) {
    playiTunesPlaylist("Oolite-Inflight");
  }
}

void oo::MusicController::stopMusicNamed(const std::string& name) {
  if (name == playingMusic()) stop();
}

void oo::MusicController::stopThemeMusic() {
  if (_special == kSpecialTheme) {
    justStop();
    playDockedMusic();
  }
}

void oo::MusicController::stopDockingMusic() {
  if (_special == kSpecialDocking) stop();
}

void oo::MusicController::stopMissionMusic() {
  if (_special == kSpecialMission) stop();
}

void oo::MusicController::toggleDockingMusic() {
  if (_mode != oo::MusicMode::kOn) return;

  if (!isPlaying())
    playDockingMusic();
  else if (_special == kSpecialDocking)
    stop();
}

::oo::SoundSource* oo::MusicController::soundSource() {
  return _current ? _current->musicSoundSource() : nullptr;
}

std::optional<std::string> oo::MusicController::playingMusic() {
  return _current ? _current->name() : std::nullopt;
}

bool oo::MusicController::isPlaying() {
  return _current && _current->isPlaying();
}

oo::MusicMode oo::MusicController::mode() { return _mode; }

void oo::MusicController::setMode(oo::MusicMode mode) {
  if (mode <= oo::kMusicModeMax && _mode != mode) {
    if (_mode == oo::MusicMode::kITunes) pauseiTunes();
    _mode = mode;

    if (_mode == oo::MusicMode::kOff) {
      stop();
    } else {
      switch (_special) {
        case kSpecialNone:
          stop();
          break;

        case kSpecialTheme:
          playThemeMusic();
          break;

        case kSpecialDocked:
          playDockedMusic();
          break;

        case kSpecialDocking:
          playDockingMusic();
          break;

        case kSpecialMission:
          playMissionMusic();
          break;
      }
    }

    std::optional<std::string> modeString;
    switch (_mode) {
      case oo::MusicMode::kOff:
        modeString = "off";
        break;
      case oo::MusicMode::kOn:
        modeString = "on";
        break;
      case oo::MusicMode::kITunes:
        modeString = "iTunes";
        break;
    }
    oo::Defaults::standard().setObject(
        "music mode", modeString ? oo::PList(*modeString) : oo::PList());
  }
}

// The singleton category (+allocWithZone:, -retain and the rest) is not
// translated: the controller has no other creator and is never released.

// The iTunes integration. Upstream's Cocoa build drove iTunes with AppleScript;
// no other build has it, so these do nothing.
void oo::MusicController::playiTunesPlaylist(
    std::string_view /*playlistName*/) {}

void oo::MusicController::pauseiTunes() {}
