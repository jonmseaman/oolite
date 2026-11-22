/*	test_OOSoundChannel.mm
        Unit tests for OOSoundChannel (src/Core/OOALSoundChannel.h).

        A channel owns one OpenAL source. It plays a sound by queueing the
   sound's buffers on it (one for a buffered sound, one at a time for a streamed
   one, two in flight), and when the source stops it deletes the buffers, drops
   the sound and tells its delegate. The free list of the mixer is kept through
   -next/-setNext:. OpenAL runs on OpenAL Soft's null backend
        (ALSOFT_DRIVERS=null), which plays in real time, so the tests wait for a
   short sound to end. The sounds are this file's C++ subclasses of the root
   OOSound, which make real OpenAL buffers of silence; the decoder, the two
   concrete sounds and the mixer that OOALSound.mm names are stubs. The
   channel's source is private, so the test reads it as a friend. The tests ask
   the C++ channel; what its destructor tells the delegate is pinned by
   releasedWhilePlayingTellsTheDelegate. The delegate is a C++
   OOSoundChannelDelegate, and the sounds are made with oo::makeShared and
   released by their std::shared_ptr.
   Run: meson test -C build/meson_test test_OOSoundChannel
*/

#include <process.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "ALBufferedSound.h"
#include "ALSound.h"
#include "ALSoundChannel.h"
#include "ALSoundDecoder.h"
#include "ALSoundMixer.h"
#include "ALStreamedSound.h"

namespace stdfs = std::filesystem;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
}

}  // namespace oo


// The mixer, as the root's update() and the OpenAL controller's shutdown()
// reach it: there is none (a C++ stand-in).
oo::SoundMixer* oo::SoundMixer::sharedMixer() { return nullptr; }
void oo::SoundMixer::update() {}
void oo::SoundMixer::shutdown() {}

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

/*	A sound of `chunks` buffers of `frames` frames of mono silence at 22050
   Hz: incomplete until the last has been made. Counts its buffers and rewinds.
*/
static int gLiveSounds = 0;

// A C++ subclass of the root.
class TestSound final : public oo::Sound {
 public:
  TestSound() { gLiveSounds++; }
  ~TestSound() override { gLiveSounds--; }

  std::optional<std::string> name() override { return std::string("test"); }

  ALuint soundBuffer() override {
    std::vector<short> silence(static_cast<size_t>(_frames), 0);
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, silence.data(),
                 static_cast<ALsizei>(silence.size() * sizeof(short)), 22050);
    _made++;
    return buffer;
  }

  bool soundIncomplete() override { return _made < _chunks; }

  void rewind() override {
    _rewinds++;
    _made = 0;
  }

  int _chunks = 1;
  int _made = 0;
  int _frames = 64;
  int _rewinds = 0;
};

// The delegate: records each sound it is told has finished, and on which
// channel.
class TestDelegate final : public oo::SoundChannelDelegate {
 public:
  void channel(oo::SoundChannel* inChannel, oo::Sound* inSound) override {
    _lastChannel = inChannel;
    _finished.push_back(inSound);
  }

  std::vector<oo::Sound*> _finished;  // compared by identity only
  oo::SoundChannel* _lastChannel = nullptr;
};

namespace {

stdfs::path sRoot;

void SetUp() {
  if (!sRoot.empty()) return;
  EXPECT_TRUE(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-soundchannel-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  EXPECT_TRUE(oo::Sound::setUp());
}

}  // namespace

struct OOSoundChannelTestAccess {
  static ALuint Source(oo::SoundChannel* channel) { return channel->_source; }
};

namespace {

// The channel's OpenAL source (its private _source).
ALuint SourceOf(oo::SoundChannel* channel) {
  return OOSoundChannelTestAccess::Source(channel);
}

// What [[OOSoundChannel alloc] init] made: a new channel, null when OpenAL
// would not make its source.
std::shared_ptr<oo::SoundChannel> NewChannel() {
  std::shared_ptr<oo::SoundChannel> channel = oo::makeShared<oo::SoundChannel>();
  if (!channel->init()) channel = nullptr;
  return channel;
}

ALint SourceInt(ALuint source, ALenum what) {
  ALint value = -1;
  alGetSourcei(source, what, &value);
  return value;
}

// Updates the channel until its sound is gone (at most two seconds).
bool UpdateUntilFinished(oo::SoundChannel* channel) {
  for (int i = 0; i < 400 && channel->sound() != nullptr; i++) {
    channel->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return channel->sound() == nullptr;
}

}  // namespace

TEST(OOSoundChannel, aNewChannelHasARelativeSource) {
  ::SetUp();
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    EXPECT_TRUE(channel != nullptr);
    const ALuint source = SourceOf(channel.get());
    EXPECT_TRUE(source != 0 && alIsSource(source));
    EXPECT_TRUE(SourceInt(source, AL_SOURCE_RELATIVE) == AL_TRUE);
    EXPECT_TRUE(channel->sound() == nullptr && channel->next() == nullptr);
    channel->update();  // nothing playing: nothing
    channel->stop();
    EXPECT_TRUE(channel->sound() == nullptr);
  }
}

TEST(OOSoundChannel, positionGainAndTheFreeList) {
  ::SetUp();
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    const std::shared_ptr<oo::SoundChannel> other = NewChannel();
    const ALuint source = SourceOf(channel.get());

    channel->setPosition(make_vector(1.0f, 2.0f, 3.0f));
    ALfloat x = 0, y = 0, z = 0;
    alGetSource3f(source, AL_POSITION, &x, &y, &z);
    EXPECT_TRUE(x == 1.0f && y == 2.0f && z == 3.0f);

    channel->setGain(0.5f);
    ALfloat gain = 0;
    alGetSourcef(source, AL_GAIN, &gain);
    EXPECT_TRUE(gain == 0.5f);

    channel->setNext(other.get());
    EXPECT_TRUE(channel->next() == other.get() && other->next() == nullptr);
    channel->setNext(nullptr);
    EXPECT_TRUE(channel->next() == nullptr);
  }
}

// A sound plays, is retained while it plays, and when it ends the delegate
// hears of it.
TEST(OOSoundChannel, playsASoundToTheEnd) {
  ::SetUp();
  TestDelegate delegate;
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    channel->setDelegate(&delegate);

    EXPECT_TRUE(!channel->playSound(nullptr, false));

    std::shared_ptr<TestSound> made = oo::makeShared<TestSound>();
    TestSound* sound = made.get();
    EXPECT_TRUE(channel->playSound(sound, false));
    EXPECT_TRUE(sound->_rewinds == 1 && sound->_made == 1);
    EXPECT_TRUE(channel->sound() == sound);
    const ALuint source = SourceOf(channel.get());
    EXPECT_TRUE(SourceInt(source, AL_BUFFERS_QUEUED) == 1);
    made = nullptr;              // [sound release]
    EXPECT_TRUE(gLiveSounds == 1);  // the channel keeps it

    EXPECT_TRUE(UpdateUntilFinished(channel.get()));
    EXPECT_TRUE(delegate._finished.size() == 1 && delegate._finished[0] == sound &&
             delegate._lastChannel == channel.get());
    EXPECT_TRUE(SourceInt(source, AL_BUFFERS_QUEUED) == 0);
    channel->setDelegate(nullptr);  // unretained
  }
  EXPECT_TRUE(gLiveSounds == 0);
}

// stop() ends the sound at once, and the delegate hears of it; a new sound
// stops the old one.
TEST(OOSoundChannel, stopAndReplace) {
  ::SetUp();
  TestDelegate delegate;
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    channel->setDelegate(&delegate);

    const std::shared_ptr<TestSound> first = oo::makeShared<TestSound>();
    first->_frames = 22050;  // a second: still playing when stopped
    const std::shared_ptr<TestSound> second = oo::makeShared<TestSound>();
    second->_frames = 22050;
    EXPECT_TRUE(channel->playSound(first.get(), false));
    EXPECT_TRUE(channel->playSound(second.get(), false));
    EXPECT_TRUE(delegate._finished.size() == 1 &&
             delegate._finished[0] == first.get());
    EXPECT_TRUE(channel->sound() == second.get());

    channel->stop();
    EXPECT_TRUE(delegate._finished.size() == 2 &&
             delegate._finished[1] == second.get());
    EXPECT_TRUE(channel->sound() == nullptr);
    EXPECT_TRUE(SourceInt(SourceOf(channel.get()), AL_SOURCE_STATE) == AL_STOPPED);
    EXPECT_TRUE(SourceInt(SourceOf(channel.get()), AL_BUFFERS_QUEUED) == 0);
    channel->setDelegate(nullptr);
  }
  EXPECT_TRUE(gLiveSounds == 0);
}

// A streamed sound: the next buffer is made while it plays, until the sound is
// complete.
TEST(OOSoundChannel, streamsAnIncompleteSound) {
  ::SetUp();
  TestDelegate delegate;
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    channel->setDelegate(&delegate);

    const std::shared_ptr<TestSound> sound = oo::makeShared<TestSound>();
    sound->_chunks = 4;
    sound->_frames = 2205;  // a tenth of a second each
    EXPECT_TRUE(channel->playSound(sound.get(), false));
    EXPECT_TRUE(UpdateUntilFinished(channel.get()));
    EXPECT_TRUE(sound->_made == 4);
    EXPECT_TRUE(delegate._finished.size() == 1);
    channel->setDelegate(nullptr);
  }
}

// A looped sound is rewound and played again when it is complete.
TEST(OOSoundChannel, loopsASound) {
  ::SetUp();
  {
    const std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    const std::shared_ptr<TestSound> sound = oo::makeShared<TestSound>();
    sound->_frames = 2205;
    EXPECT_TRUE(channel->playSound(sound.get(), true));
    for (int i = 0; i < 400 && sound->_rewinds < 3; i++) {
      channel->update();
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_TRUE(sound->_rewinds >= 3);
    EXPECT_TRUE(channel->sound() == sound.get());
    channel->stop();
    EXPECT_TRUE(channel->sound() == nullptr);
  }
}

// The C++ API: -init's failure is init()'s false; the free list holds C++
// channels.
TEST(OOSoundChannel, cxxApi) {
  ::SetUp();
  TestDelegate delegate;
  {
    const std::shared_ptr<oo::SoundChannel> channel = oo::makeShared<oo::SoundChannel>();
    EXPECT_TRUE(channel->init());
    const ALuint source = OOSoundChannelTestAccess::Source(channel.get());
    EXPECT_TRUE(source != 0 && alIsSource(source) &&
             SourceInt(source, AL_SOURCE_RELATIVE) == AL_TRUE);

    const std::shared_ptr<oo::SoundChannel> other = oo::makeShared<oo::SoundChannel>();
    EXPECT_TRUE(other->init());
    channel->setNext(other.get());
    EXPECT_TRUE(channel->next() == other.get() && other->next() == nullptr);

    channel->setDelegate(&delegate);
    EXPECT_TRUE(!channel->playSound(nullptr, false));
    const std::shared_ptr<TestSound> sound = oo::makeShared<TestSound>();
    sound->_frames = 22050;
    EXPECT_TRUE(channel->playSound(sound.get(), false) &&
             channel->sound() == sound.get());
    channel->stop();
    EXPECT_TRUE(channel->sound() == nullptr);
    // The delegate is told of the channel.
    EXPECT_TRUE(delegate._finished.size() == 1 &&
             delegate._lastChannel == channel.get());
    channel->setDelegate(nullptr);
  }
}

// A channel released while it plays tells the delegate of itself (the
// destructor does it).
TEST(OOSoundChannel, releasedWhilePlayingTellsTheDelegate) {
  ::SetUp();
  TestDelegate delegate;
  oo::SoundChannel* released = nullptr;
  TestSound* sound = nullptr;
  {
    const std::shared_ptr<TestSound> made =
        oo::makeShared<TestSound>();  // was autoreleased in this pool
    std::shared_ptr<oo::SoundChannel> channel = NewChannel();
    sound = made.get();
    sound->_frames = 22050;
    channel->setDelegate(&delegate);
    EXPECT_TRUE(channel->playSound(sound, false));
    released = channel.get();
    channel = nullptr;  // the last reference
    EXPECT_TRUE(delegate._finished.size() == 1 && delegate._finished[0] == sound);
  }
  EXPECT_TRUE(delegate._lastChannel ==
           released);  // the channel itself, while it was destroyed
  EXPECT_TRUE(gLiveSounds == 0);
}
