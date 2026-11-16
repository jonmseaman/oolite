/*	test_OOMusic.mm
        Unit tests for OOMusic (src/Core/OOALMusic.h), a subclass of the root
   OOSound that overrides the root's class-cluster initialiser.

        A music wraps the sound the root's cluster loads for its path and plays
   it through one sound source that every music shares, made on first play and
   never released; only one music plays at a time. OpenAL runs on OpenAL Soft's
   null backend and the user's defaults are a scratch folder's, as in
   test_OOSound.mm. The sound source is the game's; the mixer and its channels
   under it are this file's stubs, which record what the source tells them, and
   so are the decoder and the two concrete sounds that OOALSound.mm names (C++
   stand-ins): "missing.ogg" has no decoder. The expectations run through the
   C++ API (OOMusic, a subclass of OOSound) and ask the C++ source. Run: bash
   meson test -C build/meson_test test_OOMusic
*/

#include <process.h>

#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "ALBufferedSound.h"
#include "ALMusic.h"
#include "ALSoundDecoder.h"
#include "ALSoundMixer.h"
#include "ALStreamedSound.h"
#include "oo_test.hpp"

namespace stdfs = std::filesystem;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
}

}  // namespace oo


/*	The decoder: "missing.ogg" (and no path) has none; any other is small.
   Counts the decoders alive. A C++ stand-in: the factory the cluster calls, a
   subclass answering the path, and the root's members (OOALSoundDecoder.mm is
   not linked), which its vtable names.
*/
static int gLiveDecoders = 0;

namespace {
class TestDecoder final : public oo::ALSoundDecoder {
 public:
  explicit TestDecoder(const std::string& path) : _path(path) {
    gLiveDecoders++;
  }
  ~TestDecoder() override { gLiveDecoders--; }

  size_t sizeAsBuffer() override { return 1000; }
  std::optional<std::string> name() override { return _path; }

 private:
  std::string _path;
};
}  // namespace

std::shared_ptr<oo::ALSoundDecoder> oo::ALSoundDecoder::initWithPath(
    const std::optional<std::string>& inPath) {
  if (!inPath.has_value() || *inPath == "missing.ogg") return nullptr;
  return oo::makeShared<TestDecoder>(*inPath);
}

bool oo::ALSoundDecoder::readCreatingBuffer(std::vector<char>&) { return false; }
size_t oo::ALSoundDecoder::streamToBuffer(char*) { return 0; }
size_t oo::ALSoundDecoder::sizeAsBuffer() { return 0; }
bool oo::ALSoundDecoder::isStereo() { return false; }
long oo::ALSoundDecoder::sampleRate() { return 0; }
void oo::ALSoundDecoder::reset() {}
std::optional<std::string> oo::ALSoundDecoder::name() { return std::string(); }
std::optional<std::string> oo::ALSoundDecoder::descriptionComponents() const {
  return std::nullopt;
}

static int gLiveSounds = 0;

// The buffered sound: a C++ stand-in (the members the cluster and the root
// call). Counts the live ones.
std::shared_ptr<oo::ALBufferedSound> oo::ALBufferedSound::initWithDecoder(
    oo::ALSoundDecoder* inDecoder) {
  std::shared_ptr<oo::ALBufferedSound> sound = std::shared_ptr<oo::ALBufferedSound>(new oo::ALBufferedSound(), oo::Dispose{});
  sound->_name = inDecoder->name();
  gLiveSounds++;
  return sound;
}

namespace oo {

ALBufferedSound::~ALBufferedSound() { gLiveSounds--; }

}  // namespace oo


std::optional<std::string> oo::ALBufferedSound::name() { return _name; }

ALuint oo::ALBufferedSound::soundBuffer() {
  return oo::Sound::soundBuffer();  // the stub did not override it
}

// The streamed sound: a C++ stand-in. It refuses every decoder.
std::shared_ptr<oo::ALStreamedSound> oo::ALStreamedSound::initWithDecoder(
    oo::ALSoundDecoder* inDecoder) {
  (void)inDecoder;
  return nullptr;
}
namespace oo {

ALStreamedSound::~ALStreamedSound() {}

}  // namespace oo

std::optional<std::string> oo::ALStreamedSound::name() { return _name; }
void oo::ALStreamedSound::rewind() {}
bool oo::ALStreamedSound::soundIncomplete() { return false; }
ALuint oo::ALStreamedSound::soundBuffer() { return 0; }

/*	The mixer and its channels, under the real sound source
   (OOSoundSource.mm): the mixer hands out channels that record what they are
   told, in order, and a stopped channel tells its delegate as the real one
   does. The channel is a C++ stand-in (the members the source calls),
   with its own delegate and sound.
*/
static std::vector<std::string> gChannelLog;

namespace oo {

SoundChannel::~SoundChannel() {}

}  // namespace oo

void oo::SoundChannel::setDelegate(oo::SoundChannelDelegate* delegate) {
  _delegate = delegate;
}
void oo::SoundChannel::setPosition(Vector position) { (void)position; }
void oo::SoundChannel::setGain(float gain) {
  gChannelLog.push_back("gain " + std::to_string(gain));
}

bool oo::SoundChannel::playSound(oo::Sound* sound, bool loop) {
  gChannelLog.push_back(
      "play " +
      ((sound != nullptr) ? sound->name() : std::nullopt).value_or("(none)") +
      (loop ? " looped" : ""));
  _sound = oo::share(sound);  // [sound retain]
  return true;
}

void oo::SoundChannel::stop() {
  gChannelLog.push_back("stop");
  std::shared_ptr<oo::Sound> sound = std::move(
      _sound);  // _sound = nil; [sound release] at the end of the scope
  if (_delegate != nullptr) _delegate->channel(this, sound.get());
}

static int gChannelsOut = 0;

// The mixer (a C++ stand-in): one, never released, handing out a new channel
// each time.
oo::SoundMixer* oo::SoundMixer::sharedMixer() {
  static oo::SoundMixer* mixer = (*new std::shared_ptr<oo::SoundMixer>(oo::makeShared<oo::SoundMixer>())).get();
  return mixer;
}

void oo::SoundMixer::update() {}

void oo::SoundMixer::shutdown() {}

::oo::SoundChannel* oo::SoundMixer::popChannel() {
  gChannelsOut++;
  return (*new std::shared_ptr<oo::SoundChannel>(oo::makeShared<oo::SoundChannel>())).get();  // leaked: a few per run
}

void oo::SoundMixer::pushChannel(::oo::SoundChannel* channel) {
  (void)channel;
  gChannelsOut--;
}

namespace {

stdfs::path sRoot;

void SetUp() {
  if (!sRoot.empty()) return;
  OO_CHECK(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-music-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  OO_CHECK(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  OO_CHECK(oo::Sound::setUp());
}

std::vector<std::string> TakeChannelLog() {
  std::vector<std::string> result;
  result.swap(gChannelLog);
  return result;
}

}  // namespace

// First: before any music has played there is no source; its gain is 0 and
// setting it does nothing.
OO_TEST(noSourceBeforeTheFirstPlay) {
  SetUp();
  {
    std::shared_ptr<oo::Music> music =
        oo::Music::initWithContentsOfFile(std::string("theme.ogg"));
    OO_CHECK(music != nullptr);
    OO_CHECK(music->musicSoundSource() == nullptr);
    OO_CHECK(music->musicGain() == 0.0f);
    music->setMusicGain(0.5f);
    OO_CHECK(!music->isPlaying());
    music->stop();
    OO_CHECK(TakeChannelLog().empty() && gChannelsOut == 0);
  }
}

// A music wraps the sound the root's cluster loads, and answers its name.
OO_TEST(wrapsTheClustersSound) {
  SetUp();
  {
    std::shared_ptr<oo::Music> music =
        oo::Music::initWithContentsOfFile(std::string("theme.ogg"));
    OO_CHECK(music.get() != nullptr);
    OO_CHECK(music->name() == std::optional<std::string>("theme.ogg"));
    OO_CHECK(gLiveSounds == 1 && gLiveDecoders == 0);
    OO_CHECK(!music->soundIncomplete());

    OO_CHECK(!oo::Music::initWithContentsOfFile(std::string("missing.ogg")));
    OO_CHECK(!oo::Music::initWithContentsOfFile(std::nullopt));
    OO_CHECK(gLiveSounds == 1);
  }
  OO_CHECK(gLiveSounds == 0);  // released with the music
}

OO_TEST(playsThroughTheSharedSource) {
  SetUp();
  TakeChannelLog();
  {
    std::shared_ptr<oo::Music> theme =
        oo::Music::initWithContentsOfFile(std::string("theme.ogg"));
    theme->playLooped(true);
    OO_CHECK(
        (TakeChannelLog() ==
         std::vector<std::string>{"gain 1.000000", "play theme.ogg looped"}));
    OO_CHECK(theme->isPlaying() && gChannelsOut == 1);
    oo::SoundSource* source = theme->musicSoundSource();
    OO_CHECK(source != nullptr && source->sound() != nullptr && source->loop());

    theme->playLooped(false);  // already playing: nothing
    OO_CHECK(TakeChannelLog().empty());

    theme->setMusicGain(0.25f);
    OO_CHECK(theme->musicGain() == 0.25f && source->gain() == 0.25f);
    OO_CHECK((TakeChannelLog() == std::vector<std::string>{"gain 0.250000"}));

    // Another music takes the source; the first is no longer playing.
    std::shared_ptr<oo::Music> docked =
        oo::Music::initWithContentsOfFile(std::string("docked.ogg"));
    OO_CHECK(!docked->isPlaying());
    docked->playLooped(false);
    OO_CHECK(docked->musicSoundSource() == source && gChannelsOut == 1);
    OO_CHECK(
        (TakeChannelLog() ==
         std::vector<std::string>{"stop", "gain 0.250000", "play docked.ogg"}));
    OO_CHECK(docked->isPlaying() && !theme->isPlaying() && !source->loop());

    theme->stop();  // not the playing one: nothing
    OO_CHECK(TakeChannelLog().empty() && docked->isPlaying());

    docked->stop();
    OO_CHECK((TakeChannelLog() == std::vector<std::string>{"stop"}));
    OO_CHECK(!docked->isPlaying() && source->sound() == nullptr &&
             gChannelsOut == 0);

    // Played again after it was stopped.
    docked->playLooped(true);
    OO_CHECK(docked->isPlaying() && source->sound() != nullptr);
    docked->stop();
    TakeChannelLog();
  }
}

// A playing music that is released stops first.
OO_TEST(releasedWhilePlaying) {
  SetUp();
  TakeChannelLog();
  oo::SoundSource* source = nullptr;
  {
    std::shared_ptr<oo::Music> music =
        oo::Music::initWithContentsOfFile(std::string("theme.ogg"));
    music->playLooped(true);
    source = music->musicSoundSource();
    TakeChannelLog();
    music = nullptr;
  }
  OO_CHECK((TakeChannelLog() == std::vector<std::string>{"stop"}));
  OO_CHECK(source != nullptr && source->sound() == nullptr &&
           !source->isPlaying());
  OO_CHECK(gLiveSounds == 0);
}

// The C++ API: the factory hides the root's and answers null where the
// initialiser answered nil.
OO_TEST(cxxApi) {
  SetUp();
  TakeChannelLog();
  {
    OO_CHECK(!oo::Music::initWithContentsOfFile(std::string("missing.ogg")));
    const std::shared_ptr<oo::Music> music =
        oo::Music::initWithContentsOfFile(std::string("theme.ogg"));
    OO_CHECK(music && music->name() == std::optional<std::string>("theme.ogg"));
    OO_CHECK(!music->isPlaying() &&
             music->soundBuffer() == 0);  // the root's default
    music->playLooped(false);
    OO_CHECK(music->isPlaying() && music->musicSoundSource() != nullptr);
    music->setMusicGain(0.5f);
    OO_CHECK(music->musicGain() == 0.5f);
    music->stop();
    OO_CHECK(!music->isPlaying());
    OO_CHECK((TakeChannelLog() ==
              std::vector<std::string>{"gain 0.250000", "play theme.ogg",
                                       "gain 0.500000", "stop"}));
  }
  OO_CHECK(gLiveSounds == 0);
}

OO_TEST_MAIN()
