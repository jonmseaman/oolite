/*	test_OOMusicController.mm
        Unit tests for OOMusicController (src/Core/OOMusicController.h), a
   singleton.

        The controller plays one music at a time, loaded by name from the
   resource manager: the theme, the docking and docked music and the mission
   music, each remembered as "special" so that a change of mode can start it
   again. Its mode (off, on, or iTunes on the Mac) is the "music mode" default,
   read when it is made and written when it changes. The user's defaults are a
   scratch folder's (HOMEPATH). The resource manager and the music are this
   file's stubs, which record what they are asked (the real ones would bring the
   sound system into the link): "missing.ogg" has no music. The expectations ask
   the C++ class. After them comes the C++ API case. Run: bash
   meson test -C build/meson_test test_OOMusicController
*/

#include <process.h>

#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "MusicController.h"
#include "ResourceManager.h"
#include "oofnd/Defaults.hpp"

namespace stdfs = std::filesystem;

// What the musics are told, in order.
static std::vector<std::string> gLog;
static int gLiveMusics = 0;

// What each live music was asked, keyed by the music (the controller hands its
// sound source on unopened, and the stand-in's sound source is the music
// itself).
struct MusicState {
  std::string _name;
  bool _playing = false;
  float _gain = 0.0f;
};
static std::map<const oo::Music*, MusicState> gMusics;

// The sound root, which the music derives from: a stand-in, as OOALSound.mm
// would bring the sound system into the link.
oo::Sound::Sound() = default;
std::optional<std::string> oo::Sound::name() { return std::nullopt; }
ALuint oo::Sound::soundBuffer() { return 0; }
bool oo::Sound::soundIncomplete() { return false; }
void oo::Sound::rewind() {}
std::optional<std::string> oo::Sound::descriptionComponents() const {
  return std::nullopt;
}

// The music: this file's C++ stand-in for OOMusic (OOALMusic.mm is not linked;
// not linked).
std::shared_ptr<oo::Music> oo::Music::initWithContentsOfFile(
    const std::optional<std::string>& inPath) {
  std::shared_ptr<oo::Music> self = std::shared_ptr<oo::Music>(new oo::Music(), oo::Dispose{});
  gLiveMusics++;
  gMusics[self.get()]._name = inPath.value_or(std::string());
  return self;
}

namespace oo {

Music::~Music() {
  gMusics.erase(this);
  gLiveMusics--;
}

}  // namespace oo


std::optional<std::string> oo::Music::name() { return gMusics[this]._name; }

void oo::Music::setMusicGain(float gain) { gMusics[this]._gain = gain; }

float oo::Music::musicGain() { return gMusics[this]._gain; }

void oo::Music::playLooped(bool loop) {
  MusicState& state = gMusics[this];
  gLog.push_back("play " + state._name + (loop ? " looped" : ""));
  state._playing = true;
}

void oo::Music::stop() {
  MusicState& state = gMusics[this];
  gLog.push_back("stop " + state._name);
  state._playing = false;
}

bool oo::Music::isPlaying() { return gMusics[this]._playing; }

::oo::SoundSource* oo::Music::musicSoundSource() {
  return reinterpret_cast<::oo::SoundSource*>(
      this);  // an object the controller hands on unopened
}

// The resource manager: each name is a new music (as a cache miss is), from the
// "Music" folder.
std::shared_ptr<oo::Music> ResourceManager::ooMusicNamed(
    const std::string& fileName, const std::optional<std::string>& folderName) {
  if (fileName == "missing.ogg" ||
      folderName != std::optional<std::string>("Music"))
    return nullptr;
  return oo::Music::initWithContentsOfFile(
      fileName);  // the stand-in names the music by its path
}

namespace {

stdfs::path sRoot;

void SetUp() {
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-musiccontroller-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
}

std::vector<std::string> TakeLog() {
  std::vector<std::string> result;
  result.swap(gLog);
  return result;
}

std::optional<std::string> ModeDefault() {
  return oo::Defaults::standard().stringForKey("music mode");
}

MusicState* Current(oo::MusicController* controller) {
  return &gMusics.at(
      reinterpret_cast<const oo::Music*>(controller->soundSource()));
}

}  // namespace

// First: the shared controller reads its mode from the defaults when it is
// made.
TEST(OOMusicController, theSharedControllerReadsItsMode) {
  ::SetUp();
  oo::Defaults::standard().setObject("music mode",
                                     oo::PList(std::string("off")));
  oo::MusicController* controller = oo::MusicController::sharedController();
  EXPECT_TRUE(controller != nullptr &&
           oo::MusicController::sharedController() == controller);
  EXPECT_TRUE(controller->mode() == oo::MusicMode::kOff);
  EXPECT_TRUE(!controller->isPlaying() &&
           !controller->playingMusic().has_value() &&
           controller->soundSource() == nullptr);

  // Off: nothing plays.
  controller->playThemeMusic();
  controller->playMusicNamed("x.ogg", false);
  EXPECT_TRUE(TakeLog().empty() && !controller->isPlaying());
}

// The theme asked for while off is remembered: turning music on plays it.
TEST(OOMusicController, modeChangesAreKept) {
  ::SetUp();
  oo::MusicController* controller = oo::MusicController::sharedController();
  {
    controller->setMode(oo::MusicMode::kOn);
    EXPECT_TRUE(controller->mode() == oo::MusicMode::kOn &&
             ModeDefault() == std::optional<std::string>("on"));
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"play OoliteTheme.ogg looped"}));
    controller->setMode(oo::MusicMode::kITunes);  // not on this platform: refused
    EXPECT_TRUE(controller->mode() == oo::MusicMode::kOn &&
             ModeDefault() == std::optional<std::string>("on"));
    controller->setMode(oo::MusicMode::kOff);
    EXPECT_TRUE(controller->mode() == oo::MusicMode::kOff &&
             ModeDefault() == std::optional<std::string>("off"));
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop OoliteTheme.ogg"}));
    controller->setMode(
        oo::MusicMode::kOn);  // the stop forgot the theme: nothing plays
    EXPECT_TRUE(TakeLog().empty() && !controller->isPlaying());
  }
  EXPECT_TRUE(gLiveMusics == 0);
}

TEST(OOMusicController, playsMusicByName) {
  ::SetUp();
  oo::MusicController* controller = oo::MusicController::sharedController();
  {
    controller->playMusicNamed("a.ogg", true, 2.0f);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play a.ogg looped"}));
    EXPECT_TRUE(controller->isPlaying() &&
             controller->playingMusic() == std::optional<std::string>("a.ogg"));
    EXPECT_TRUE(Current(controller)->_gain == 1.0f);  // clamped

    controller->playMusicNamed("a.ogg", false);  // already playing it: nothing
    EXPECT_TRUE(TakeLog().empty());

    controller->playMusicNamed("missing.ogg",
                               false);  // none: the current one goes on
    EXPECT_TRUE(TakeLog().empty() &&
             controller->playingMusic() == std::optional<std::string>("a.ogg"));

    controller->playMusicNamed("b.ogg", false);
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"stop a.ogg", "play b.ogg"}));
    EXPECT_TRUE(Current(controller)->_gain == OO_DEFAULT_SOUNDSOURCE_GAIN);

    controller->stopMusicNamed("a.ogg");  // not the one playing
    EXPECT_TRUE(TakeLog().empty());
    controller->stopMusicNamed("b.ogg");
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop b.ogg"}));
    EXPECT_TRUE(!controller->isPlaying() && controller->soundSource() == nullptr);
  }
  EXPECT_TRUE(gLiveMusics == 0);
}

TEST(OOMusicController, specialMusic) {
  ::SetUp();
  oo::MusicController* controller = oo::MusicController::sharedController();
  {
    controller->playThemeMusic();
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"play OoliteTheme.ogg looped"}));
    controller->stopDockingMusic();  // not docking music: nothing
    controller->stopMissionMusic();
    EXPECT_TRUE(TakeLog().empty());
    controller->stopThemeMusic();  // the theme gives way to the docked music
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop OoliteTheme.ogg",
                                                    "play OoliteDocked.ogg"}));

    controller->playDockingMusic();
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"stop OoliteDocked.ogg",
                                               "play BlueDanube.ogg looped"}));
    controller->toggleDockingMusic();  // playing the docking music: stops it
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop BlueDanube.ogg"}));
    controller->toggleDockingMusic();  // nothing playing: starts it
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"play BlueDanube.ogg looped"}));
    controller->stopDockingMusic();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop BlueDanube.ogg"}));

    // The mission music: the theme until it is set; none when it is cleared.
    controller->playMissionMusic();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play OoliteTheme.ogg"}));
    controller->setMissionMusic(std::string("mission.ogg"));
    controller->playMissionMusic();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop OoliteTheme.ogg",
                                                    "play mission.ogg"}));
    controller->stopMissionMusic();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop mission.ogg"}));
    controller->setMissionMusic(std::nullopt);
    controller->playMissionMusic();
    EXPECT_TRUE(TakeLog().empty());

    // A change of mode stops the music, and on again starts the special one.
    controller->playDockedMusic();
    TakeLog();
    controller->setMode(oo::MusicMode::kOff);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop OoliteDocked.ogg"}));
    controller->toggleDockingMusic();  // only when on
    EXPECT_TRUE(TakeLog().empty());
    controller->playDockedMusic();  // off: remembered, not played
    EXPECT_TRUE(TakeLog().empty());
    controller->setMode(oo::MusicMode::kOn);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play OoliteDocked.ogg"}));

    controller->justStop();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop OoliteDocked.ogg"}));
    controller->setMode(oo::MusicMode::kOff);
    controller->setMode(oo::MusicMode::kOn);  // nothing special any more: just stops
    controller->stop();
    EXPECT_TRUE(TakeLog().empty());
  }
  EXPECT_TRUE(gLiveMusics == 0);
}

// The C++ API: the one controller.
TEST(OOMusicController, cxxApi) {
  ::SetUp();
  oo::MusicController* controller = oo::MusicController::sharedController();
  EXPECT_TRUE(controller != nullptr &&
           controller == oo::MusicController::sharedController());
  {
    EXPECT_TRUE(controller->mode() == oo::MusicMode::kOn);
    controller->setMissionMusic(std::string("c.ogg"));
    controller->playMissionMusic();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play c.ogg"}));
    EXPECT_TRUE(controller->isPlaying() &&
             controller->playingMusic() == std::optional<std::string>("c.ogg"));
    EXPECT_TRUE(oo::MusicController::sharedController()->isPlaying());
    controller->playMusicNamed("d.ogg", true, 0.5f);
    EXPECT_TRUE((TakeLog() ==
              std::vector<std::string>{"stop c.ogg", "play d.ogg looped"}));
    EXPECT_TRUE(Current(oo::MusicController::sharedController())->_gain == 0.5f);
    controller->stop();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop d.ogg"}) &&
             !controller->isPlaying());
  }
  EXPECT_TRUE(gLiveMusics == 0);
}
