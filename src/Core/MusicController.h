/*

OOMusicController.h

Singleton controller for music playback.

A singleton.

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

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "ALMusic.h"
#include "Foundation.h"
#include "SoundSource.h"
#include "oofnd/Object.hpp"

namespace oo {

enum class MusicMode : std::int32_t {
  kOff,
  kOn,
  kITunes,
};

}  // namespace oo

static_assert(std::to_underlying(oo::MusicMode::kOff) == 0);
static_assert(std::to_underlying(oo::MusicMode::kOn) == 1);
static_assert(std::to_underlying(oo::MusicMode::kITunes) == 2);

namespace oo {

inline constexpr MusicMode kMusicModeMax = MusicMode::kOn;

class MusicController : public oo::Object {
 public:
  /*	The one controller (was +sharedController),
     made on first use and never released; borrowed.
  */
  static MusicController* sharedController();

  MusicController();  // was -init

  void playMusicNamed(const std::string& name, bool loop);
  void playMusicNamed(const std::string& name, bool loop, float gain);

  void playThemeMusic();
  void playDockingMusic();
  void playDockedMusic();

  void setMissionMusic(
      const std::optional<std::string>&
          missionMusicName);  // was -setMissionMusic:; nullopt: none
  void playMissionMusic();

  void justStop();
  void stop();
  void stopMusicNamed(
      const std::string& name);  // Stop only if name == playingMusic
  void stopThemeMusic();
  void stopDockingMusic();
  void stopMissionMusic();

  void
  toggleDockingMusic();  // Start docking music if none playing, stop docking
                         // music if currently playing docking music.

  ::oo::SoundSource* soundSource();

  std::optional<std::string> playingMusic();  // nullopt: nothing playing
  bool isPlaying();

  MusicMode mode();
  void setMode(MusicMode mode);

 private:
  // The private category (the iTunes integration, Mac only).
  void playiTunesPlaylist(std::string_view playlistName);
  void pauseiTunes();

  MusicMode _mode = {};
  std::optional<std::string> _missionMusic;
  std::shared_ptr<Music> _current;
  uint8_t _special = {};
};

}  // namespace oo
