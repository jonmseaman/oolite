/*	test_OOSoundSource.mm
        Unit tests for OOSoundSource (src/Core/OOSoundSource.h).

        A sound source plays one sound at a time through a channel it pops from
   the shared mixer, as the channel's delegate: it gives the channel its
   position and gain, keeps itself alive while it plays, plays the sound again
   until its repeat count is spent, and then pushes the channel back. +stopAll
   stops every source that is playing. The mixer and its channels are this
   file's stubs, which record what they are told (the real ones would make
   OpenAL sources); the sound is an opaque object to a source, so it is a stub
   too, with only a name (a C++ subclass of the root, with the root's
   members OOALSound.mm would define). The playing channel is private, so the
   test reads it as a friend. The tests ask the C++ source (alloc/init as
   oo::makeShared, "%@" as descriptionComponents(), an autorelease pool as an
   oo::AutoreleaseScope); after them comes the C++ API. The mixer and the
   channel are C++ stand-ins, and the source is its channel's C++ delegate
   (OOSoundChannelDelegate). Run: tests/run.sh unit
   test_OOSoundSource
*/

#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "ALSoundMixer.h"
#include "SoundSource.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/String.hpp"

static int gLiveSounds = 0;

/*	The sound: OOALSound.mm is not linked, so the root's members the source
   and the sound's vtable name are defined here as the root answers them (C++),
   and a test sound has only a name, which "%@" prints between the braces.
*/
oo::Sound::Sound() {}
std::optional<std::string> oo::Sound::name() { return std::nullopt; }
ALuint oo::Sound::soundBuffer() { return 0; }
bool oo::Sound::soundIncomplete() { return false; }
void oo::Sound::rewind() {}
std::optional<std::string> oo::Sound::descriptionComponents() const {
  return std::nullopt;
}

std::string oo::Sound::description() const {
  std::string result = oo::str::format(
      "<OOSound %s>", oo::str::pointerDescription(this).c_str());
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}

class TestSound final : public oo::Sound {
 public:
  explicit TestSound(const std::string& name) : _name(name) { gLiveSounds++; }
  ~TestSound() override { gLiveSounds--; }

  std::optional<std::string> descriptionComponents() const override {
    return _name;
  }

  std::string _name;
};

/*	A channel records what it is told, in order; Finish() ends its sound as
   the real one does when OpenAL stops, and stop() as its stop() does: both tell
   the delegate. A C++ stand-in (the members a source calls): its delegate,
   sound and loop are the channel's own; the position and gain it is given are
   kept per channel here.
*/
static std::vector<std::string> gLog;

struct TestChannelState {
  Vector position = {};
  float gain = 0;
};
static std::map<oo::SoundChannel*, TestChannelState> gChannels;

struct OOSoundChannelTestAccess {
  static oo::SoundChannelDelegate* Delegate(oo::SoundChannel* channel) {
    return channel->_delegate;
  }

  // The stub's -finish.
  static void Finish(oo::SoundChannel* channel) {
    std::shared_ptr<oo::Sound> sound =
        std::move(channel->_sound);  // [sound release] at the end of the scope
    channel->_sound = nullptr;
    if (channel->_delegate != nullptr)
      channel->_delegate->channel(channel, sound.get());
  }
};

namespace oo {

SoundChannel::~SoundChannel() { gChannels.erase(this); }

}  // namespace oo


void oo::SoundChannel::setDelegate(oo::SoundChannelDelegate* delegate) {
  _delegate = delegate;
}

void oo::SoundChannel::setPosition(Vector position) {
  gChannels[this].position = position;
}

void oo::SoundChannel::setGain(float gain) { gChannels[this].gain = gain; }

bool oo::SoundChannel::playSound(oo::Sound* sound, bool loop) {
  gLog.push_back("play " + static_cast<TestSound*>(sound)->_name +
                 (loop ? " looped" : ""));
  _loop = loop;
  _sound = oo::share(sound);  // [sound retain]
  return true;
}

void oo::SoundChannel::stop() {
  gLog.push_back("stop");
  OOSoundChannelTestAccess::Finish(this);
}

static int gChannelsOut = 0;
static int gChannelsAvailable = 8;

// The mixer (a C++ stand-in): one, never released, handing out at most
// gChannelsAvailable channels.
oo::SoundMixer* oo::SoundMixer::sharedMixer() {
  static oo::SoundMixer* mixer =
      (*new std::shared_ptr<oo::SoundMixer>(oo::makeShared<oo::SoundMixer>())).get();
  return mixer;
}

::oo::SoundChannel* oo::SoundMixer::popChannel() {
  if (gChannelsOut == gChannelsAvailable) return nullptr;
  gChannelsOut++;
  return (*new std::shared_ptr<oo::SoundChannel>(oo::makeShared<oo::SoundChannel>()))
      .get();  // leaked: a few per run
}

void oo::SoundMixer::pushChannel(::oo::SoundChannel* channel) {
  (void)channel;
  gChannelsOut--;
  gLog.push_back("push");
}

namespace {

// [[[OOSound alloc] init] autorelease] with a name: it lives until the
// innermost scope ends.
oo::Sound* MakeSound(const char* name) {
  return oo::autorelease(oo::makeShared<TestSound>(name));
}

std::vector<std::string> TakeLog() {
  std::vector<std::string> result;
  result.swap(gLog);
  return result;
}

}  // namespace

struct OOSoundSourceTestAccess {
  static oo::SoundChannel* Channel(oo::SoundSource* source) {
    return source->_channel;
  }
};

namespace {

// The channel a playing source has (its private _channel).
oo::SoundChannel* ChannelOf(oo::SoundSource* source) {
  return OOSoundSourceTestAccess::Channel(source);
}

}  // namespace

TEST(OOSoundSource, defaults) {
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    const std::shared_ptr<oo::SoundSource> source = oo::makeShared<oo::SoundSource>();
    EXPECT_TRUE(source->sound() == nullptr);
    EXPECT_TRUE(!source->loop() && source->repeatCount() == 1 &&
             !source->isPlaying());
    EXPECT_TRUE(!source->positional() &&
             vector_equal(source->position(), kZeroVector));
    EXPECT_TRUE(source->gain() == OO_DEFAULT_SOUNDSOURCE_GAIN);

    source->play();  // no sound: nothing
    EXPECT_TRUE(!source->isPlaying() && TakeLog().empty());

    EXPECT_TRUE(source->descriptionComponents() ==
             std::optional<std::string>(
                 "sound=(null), loop=NO, repeatCount=1, not playing"));
  }
}

TEST(OOSoundSource, attributes) {
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    oo::Sound* sound = MakeSound("beep");
    const std::shared_ptr<oo::SoundSource> source = oo::SoundSource::sourceWithSound(sound);
    EXPECT_TRUE(source->sound() == sound);
    EXPECT_TRUE(dynamic_cast<oo::SoundSource*>(source.get()) != nullptr);

    source->setLoop(true);
    EXPECT_TRUE(source->loop());
    source->setRepeatCount(3);
    EXPECT_TRUE(source->repeatCount() == 3);
    source->setRepeatCount(0);
    EXPECT_TRUE(source->repeatCount() == 1);  // 0 reads as 1

    source->setPosition(make_vector(1, 2, 3));
    EXPECT_TRUE(source->positional() &&
             vector_equal(source->position(), make_vector(1, 2, 3)));
    source->setPositional(false);
    EXPECT_TRUE(!source->positional() &&
             vector_equal(source->position(), kZeroVector));
    source->setPositional(true);
    EXPECT_TRUE(source->positional());
    source->setPosition(kZeroVector);
    EXPECT_TRUE(source->positional());  // a zero position does not clear it

    source->setGain(0.5f);
    EXPECT_TRUE(source->gain() == 0.5f);

    // The advanced attributes are ignored.
    source->setVelocity(make_vector(1, 1, 1));
    source->setOrientation(make_vector(1, 1, 1));
    source->setConeAngle(1.0f);
    source->setGainInsideCone(1.0f, 0.5f);
    source->positionRelativeTo(nullptr);

    oo::Sound* other = MakeSound("boop");
    source->setSound(other);
    EXPECT_TRUE(source->sound() == other);
    EXPECT_TRUE(
        source->descriptionComponents() ==
        std::optional<std::string>("sound=" + other->description() +
                                   ", loop=YES, repeatCount=1, not playing"));
  }
}

// Playing takes a channel and hands it the source's attributes; the source
// keeps itself alive.
TEST(OOSoundSource, playsThroughAChannel) {
  TakeLog();
  oo::SoundSource* weak = nullptr;
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    std::shared_ptr<oo::SoundSource> source =
        oo::makeShared<oo::SoundSource>(MakeSound("beep"));
    source->setPosition(make_vector(4, 5, 6));
    source->setGain(0.25f);
    source->setLoop(true);
    source->play();
    EXPECT_TRUE(source->isPlaying() && gChannelsOut == 1);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play beep looped"}));

    oo::SoundChannel* channel = ChannelOf(source.get());
    EXPECT_TRUE(channel != nullptr &&
             OOSoundChannelTestAccess::Delegate(channel) == source.get());
    EXPECT_TRUE(vector_equal(gChannels[channel].position, make_vector(4, 5, 6)) &&
             gChannels[channel].gain == 0.25f);
    EXPECT_TRUE(source->descriptionComponents().value_or("").find(
                 "playing on channel <OOSoundChannel 0x") != std::string::npos);

    // Changes reach the playing channel.
    source->setGain(0.75f);
    source->setPosition(make_vector(7, 8, 9));
    EXPECT_TRUE(gChannels[channel].gain == 0.75f &&
             vector_equal(gChannels[channel].position, make_vector(7, 8, 9)));

    weak = source.get();
    source = nullptr;  // [source release]: playing, still alive
  }
  EXPECT_TRUE(weak->isPlaying());

  // The channel ends the sound: the source pushes the channel back and lets
  // itself go.
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    OOSoundChannelTestAccess::Finish(ChannelOf(weak));
  }
  EXPECT_TRUE((TakeLog() == std::vector<std::string>{"push"}));
  EXPECT_TRUE(gChannelsOut == 0);
}

// A repeat count plays the sound again (unlooped) until it is spent.
TEST(OOSoundSource, repeats) {
  TakeLog();
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    const std::shared_ptr<oo::SoundSource> source = oo::makeShared<oo::SoundSource>();
    source->playSound(MakeSound("beep"), 3);
    EXPECT_TRUE(source->repeatCount() == 3);
    oo::SoundChannel* channel = ChannelOf(source.get());
    OOSoundChannelTestAccess::Finish(channel);
    OOSoundChannelTestAccess::Finish(channel);
    EXPECT_TRUE(source->isPlaying());
    OOSoundChannelTestAccess::Finish(channel);
    EXPECT_TRUE(!source->isPlaying());
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play beep", "play beep",
                                                    "play beep", "push"}));

    // -playOrRepeat adds one to a playing source's count.
    source->setRepeatCount(1);
    source->play();
    source->playOrRepeat();
    channel = ChannelOf(source.get());
    OOSoundChannelTestAccess::Finish(channel);
    EXPECT_TRUE(source->isPlaying());
    OOSoundChannelTestAccess::Finish(channel);
    EXPECT_TRUE(!source->isPlaying());
    EXPECT_TRUE((TakeLog() ==
              std::vector<std::string>{"play beep", "play beep", "push"}));
  }
}

// -stop hands the channel to the class, which pushes it back; playing again
// stops first.
TEST(OOSoundSource, stops) {
  TakeLog();
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    oo::Sound* beep = MakeSound("beep");
    const std::shared_ptr<oo::SoundSource> source = oo::SoundSource::sourceWithSound(beep);
    source->play();
    source->play();
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play beep", "stop", "push",
                                                    "play beep"}));
    source->stop();
    EXPECT_TRUE(!source->isPlaying() && gChannelsOut == 0);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop", "push"}));
    source->stop();  // not playing: nothing
    EXPECT_TRUE(TakeLog().empty());

    // -playOrRepeatSound: plays a new sound, repeats the same one.
    source->playOrRepeatSound(beep);
    EXPECT_TRUE(source->isPlaying());
    source->playOrRepeatSound(beep);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play beep"}));
    oo::Sound* boop = MakeSound("boop");
    source->playOrRepeatSound(boop);
    EXPECT_TRUE(source->sound() == boop);
    EXPECT_TRUE(
        (TakeLog() == std::vector<std::string>{"stop", "push", "play boop"}));

    // Changing the sound stops the playing one.
    source->setSound(beep);
    EXPECT_TRUE(!source->isPlaying());
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"stop", "push"}));

    // -playOOSound: plays with the source's own repeat count.
    source->setRepeatCount(2);
    source->playOOSound(boop);
    EXPECT_TRUE(source->sound() == boop && source->repeatCount() == 2);
    source->stop();
    TakeLog();
  }
}

// +stopAll stops every playing source; with no channel to be had, a source does
// not play.
TEST(OOSoundSource, stopAllAndNoChannel) {
  TakeLog();
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    const std::shared_ptr<oo::SoundSource> a =
        oo::SoundSource::sourceWithSound(MakeSound("a"));
    const std::shared_ptr<oo::SoundSource> b =
        oo::SoundSource::sourceWithSound(MakeSound("b"));
    a->play();
    b->play();
    EXPECT_TRUE(gChannelsOut == 2);
    oo::SoundSource::stopAll();
    EXPECT_TRUE(!a->isPlaying() && !b->isPlaying() && gChannelsOut == 0);
    EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play a", "play b", "stop",
                                                    "push", "stop", "push"}));
    oo::SoundSource::stopAll();  // none playing
    EXPECT_TRUE(TakeLog().empty());

    gChannelsAvailable = 0;
    a->play();
    EXPECT_TRUE(!a->isPlaying() && TakeLog().empty());
    gChannelsAvailable = 8;

    // It was still recorded as playing (as before); +stopAll lets every source
    // go.
    oo::SoundSource::stopAll();
    EXPECT_TRUE(TakeLog().empty());
  }
  EXPECT_TRUE(gLiveSounds == 0);
}

// The C++ API, and a playing C++ source: it is the channel's delegate, and it
// keeps itself alive.
TEST(OOSoundSource, cxxApi) {
  TakeLog();
  oo::SoundSource* weak = nullptr;
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    const std::shared_ptr<oo::SoundSource> source =
        oo::SoundSource::sourceWithSound(MakeSound("beep"));
    EXPECT_TRUE(source->sound() != nullptr && source->repeatCount() == 1 &&
             source->gain() == OO_DEFAULT_SOUNDSOURCE_GAIN);
    EXPECT_TRUE(
        source->descriptionComponents() ==
        std::optional<std::string>("sound=" + source->sound()->description() +
                                   ", loop=NO, repeatCount=1, not playing"));
    source->setGain(0.5f);
    source->play();
    EXPECT_TRUE(source->isPlaying());
    oo::SoundChannel* channel = OOSoundSourceTestAccess::Channel(source.get());
    EXPECT_TRUE(OOSoundChannelTestAccess::Delegate(channel) == source.get() &&
             gChannels[channel].gain == 0.5f);
    weak = source.get();
  }
  // The Ref and the pool are gone; the playing source keeps itself.
  EXPECT_TRUE(weak->isPlaying());
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    OOSoundChannelTestAccess::Finish(OOSoundSourceTestAccess::Channel(weak));
  }
  EXPECT_TRUE((TakeLog() == std::vector<std::string>{"play beep", "push"}));
  {
    oo::AutoreleaseScope scope;  // was @autoreleasepool
    oo::SoundSource::stopAll();    // lets it go
  }
}
