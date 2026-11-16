/*	test_OOSoundMixer.mm
        Unit tests for OOSoundMixer (src/Core/OOALSoundMixer.h), a singleton.

        The mixer is made on first use, after setting sound up, with
   kMixerGeneralChannels channels on a free list that the sound sources pop and
   push; -update updates every channel, and -shutdown (at exit) releases them.
   OpenAL runs on OpenAL Soft's null backend and the user's defaults are a
   scratch folder's, as in test_OOSound.mm. The channels are this file's stubs
   (C++), which count what they are told (the real ones
   would make OpenAL sources); so are the decoder and the two concrete sounds
   that OOALSound.mm names. The root's +update is the
   game's, so it reaches this mixer. The tests ask the C++ mixer and the C++
   root, and the decoder is a C++ stand-in. Before the shutdown, which is last,
   comes the C++ API. Run: bash
   meson test -C build/meson_test test_OOSoundMixer
*/

#include <process.h>

#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <map>
#include <set>
#include <string>

#include "ALBufferedSound.h"
#include "ALSound.h"
#include "ALSoundChannel.h"
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


// The decoder the root's cluster asks for: none, for every path (a C++
// stand-in).
std::shared_ptr<oo::ALSoundDecoder> oo::ALSoundDecoder::initWithPath(
    const std::optional<std::string>& inPath) {
  (void)inPath;
  return nullptr;
}

// The buffered sound the root's cluster names: a C++ stand-in. The decoder
// above refuses every path, so none is made.
std::shared_ptr<oo::ALBufferedSound> oo::ALBufferedSound::initWithDecoder(
    oo::ALSoundDecoder* inDecoder) {
  (void)inDecoder;
  return nullptr;
}
namespace oo {

ALBufferedSound::~ALBufferedSound() {}

}  // namespace oo

std::optional<std::string> oo::ALBufferedSound::name() { return _name; }
ALuint oo::ALBufferedSound::soundBuffer() { return 0; }

// The streamed sound the root's cluster names: a C++ stand-in. None is made
// here.
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

/*	The channels: a C++ stand-in (the members the mixer calls). They
   count the live ones and the updates each is sent; the free list's link is the
   channel's own _next.
*/
static int gLiveChannels = 0;
static int gChannelUpdates = 0;
static std::map<oo::SoundChannel*, int>
    gUpdates;  // per channel: the stub's _updates

bool oo::SoundChannel::init() {
  gLiveChannels++;
  return true;
}

namespace oo {

SoundChannel::~SoundChannel() {
  gLiveChannels--;
  gUpdates.erase(this);
}

}  // namespace oo


void oo::SoundChannel::update() {
  gUpdates[this]++;
  gChannelUpdates++;
}

oo::SoundChannel* oo::SoundChannel::next() { return _next; }

void oo::SoundChannel::setNext(oo::SoundChannel* next) { _next = next; }

namespace {

stdfs::path sRoot;

void SetUp() {
  if (!sRoot.empty()) return;
  OO_CHECK(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-soundmixer-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  OO_CHECK(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
}

}  // namespace

// First: the shared mixer sets sound up and makes its channels, once.
OO_TEST(theSharedMixerMakesItsChannels) {
  SetUp();
  OO_CHECK(!oo::Sound::isSoundOK());
  oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer();
  OO_CHECK(mixer != nullptr);
  OO_CHECK(oo::Sound::isSoundOK());
  OO_CHECK(gLiveChannels == kMixerGeneralChannels);
  OO_CHECK(oo::SoundMixer::sharedMixer() == mixer);
  OO_CHECK(gLiveChannels == kMixerGeneralChannels);
}

// The free list: last pushed, first popped; each channel once; nil when empty.
OO_TEST(popAndPushTheFreeList) {
  SetUp();
  oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer();
  std::set<oo::SoundChannel*> popped;
  oo::SoundChannel* first = nullptr;
  for (int i = 0; i < kMixerGeneralChannels; i++) {
    oo::SoundChannel* channel = mixer->popChannel();
    OO_CHECK(channel != nullptr && channel->next() == nullptr);
    if (first == nullptr) first = channel;
    popped.insert(channel);
  }
  OO_CHECK(popped.size() == kMixerGeneralChannels);
  OO_CHECK(mixer->popChannel() == nullptr);
  OO_CHECK(mixer->popChannel() == nullptr);

  oo::SoundChannel* a = *popped.begin();
  oo::SoundChannel* b = *popped.rbegin();
  mixer->pushChannel(a);
  mixer->pushChannel(b);
  OO_CHECK(b->next() == a);
  OO_CHECK(mixer->popChannel() == b);
  OO_CHECK(mixer->popChannel() == a);
  OO_CHECK(mixer->popChannel() == nullptr);

  for (oo::SoundChannel* channel : popped) mixer->pushChannel(channel);
  OO_CHECK(gLiveChannels == kMixerGeneralChannels);
  (void)first;
}

// -update, and the root's +update, update every channel once.
OO_TEST(updateUpdatesEveryChannel) {
  SetUp();
  oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer();
  const int before = gChannelUpdates;
  mixer->update();
  OO_CHECK(gChannelUpdates == before + kMixerGeneralChannels);
  oo::Sound::update();
  OO_CHECK(gChannelUpdates == before + 2 * kMixerGeneralChannels);

  oo::SoundChannel* channel = mixer->popChannel();
  OO_CHECK(gUpdates[channel] == 2);
  mixer->pushChannel(channel);
}

// The C++ API: the one mixer, the same free list and the same channels on every
// call.
OO_TEST(cxxApi) {
  SetUp();
  oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer();
  OO_CHECK(mixer != nullptr && mixer == oo::SoundMixer::sharedMixer());

  oo::SoundChannel* channel = mixer->popChannel();
  OO_CHECK(channel != nullptr && channel->next() == nullptr);
  mixer->pushChannel(channel);
  OO_CHECK(oo::SoundMixer::sharedMixer()->popChannel() == channel);
  oo::SoundMixer::sharedMixer()->pushChannel(channel);

  const int before = gChannelUpdates;
  mixer->update();
  OO_CHECK(gChannelUpdates == before + kMixerGeneralChannels);
}

// Last: -shutdown releases the channels; -update then has nothing to update.
OO_TEST(shutdownReleasesTheChannels) {
  SetUp();
  oo::SoundMixer* mixer = oo::SoundMixer::sharedMixer();
  mixer->shutdown();
  OO_CHECK(gLiveChannels == 0);
  const int before = gChannelUpdates;
  mixer->update();
  OO_CHECK(gChannelUpdates == before);
  OO_CHECK(oo::SoundMixer::sharedMixer() == mixer);
}

OO_TEST_MAIN()
