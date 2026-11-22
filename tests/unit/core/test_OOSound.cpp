/*	test_OOSound.mm
        Unit tests for OOSound (src/Core/OOALSound.h).

        OOSound is the root of the sounds: OOALBufferedSound, OOALStreamedSound
   and OOMusic derive from it in their own files.
   The root keeps the sound system's global state (set up, sound OK, the master
   volume, which it keeps in the "volume_control" default) and is the class
   cluster that initWithContentsOfFile() turns into a buffered or a streamed
   sound by the decoded size. The tests ask the C++ class (alloc/init as
   oo::makeShared, the cluster as the factory, the class checks as dynamic_cast,
   description()). After them come the C++ API and a C++ subclass, as
   test_OODrawable.mm does.

        OpenAL runs on OpenAL Soft's null backend (ALSOFT_DRIVERS=null), so the
   machine's sound hardware cannot change the answers; the user's defaults are a
   scratch folder's (HOMEPATH), so the machine's own volume cannot either. The
   decoder, the two concrete sounds and the mixer are this file's stubs: the
   real ones would bring Vorbis and the whole mixer into the link. All are C++
   stand-ins. Run: tests/run.sh unit
*/

#include <process.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "ALBufferedSound.h"
#include "ALSound.h"
#include "ALSoundDecoder.h"
#include "ALSoundMixer.h"
#include "ALStreamedSound.h"
#include "Logging.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/Log.hpp"

namespace stdfs = std::filesystem;

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked. The one function of it that the root calls is defined here instead,
   and counts.
*/
static int gSubclassResponsibilities = 0;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
  gSubclassResponsibilities++;
}

}  // namespace oo


static unsigned gMixerUpdates = 0;

// The mixer (a C++ stand-in): one, never released, counting the updates the
// root sends it.
oo::SoundMixer* oo::SoundMixer::sharedMixer() {
  static oo::SoundMixer* mixer = (*new std::shared_ptr<oo::SoundMixer>(oo::makeShared<oo::SoundMixer>())).get();
  return mixer;
}

void oo::SoundMixer::update() { gMixerUpdates++; }

void oo::SoundMixer::shutdown() {}

/*	The decoder: a path names its decoded size. "missing.ogg" (and no path)
   has no decoder, "big.ogg" is one byte over the 1 MB a buffered sound may
   hold, "edge.ogg" exactly that, and anything else is small. Counts the
   decoders alive, so the test sees each released. A C++ stand-in: the factory
   the cluster calls, a subclass answering the path, and the root's members
   (OOALSoundDecoder.mm is not linked), which its vtable names.
*/
static int gLiveDecoders = 0;

namespace {
class TestDecoder final : public oo::ALSoundDecoder {
 public:
  explicit TestDecoder(const std::string& path) : _path(path) {
    gLiveDecoders++;
  }
  ~TestDecoder() override { gLiveDecoders--; }

  size_t sizeAsBuffer() override {
    if (_path == "big.ogg") return (1 << 20) + 1;
    if (_path == "edge.ogg") return 1 << 20;
    return 1000;
  }

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

// The two concrete sounds. "bad.ogg" fails to load. The buffered sound is a C++
// stand-in (the members the cluster and the root call).
std::shared_ptr<oo::ALBufferedSound> oo::ALBufferedSound::initWithDecoder(
    oo::ALSoundDecoder* inDecoder) {
  if (inDecoder->name() == "bad.ogg") return nullptr;
  std::shared_ptr<oo::ALBufferedSound> sound = std::shared_ptr<oo::ALBufferedSound>(new oo::ALBufferedSound(), oo::Dispose{});
  sound->_name = inDecoder->name();
  return sound;
}

namespace oo {

ALBufferedSound::~ALBufferedSound() {}

}  // namespace oo


std::optional<std::string> oo::ALBufferedSound::name() { return _name; }

ALuint oo::ALBufferedSound::soundBuffer() { return 42; }

// The streamed sound: a C++ stand-in. Its rewinds are counted per sound, and a
// rewind reaches the root's.
static std::map<const oo::ALStreamedSound*, int> gStreamedRewinds;

std::shared_ptr<oo::ALStreamedSound> oo::ALStreamedSound::initWithDecoder(
    oo::ALSoundDecoder* inDecoder) {
  std::shared_ptr<oo::ALStreamedSound> sound = std::shared_ptr<oo::ALStreamedSound>(new oo::ALStreamedSound(), oo::Dispose{});
  sound->_name = inDecoder->name();
  return sound;
}

namespace oo {

ALStreamedSound::~ALStreamedSound() { gStreamedRewinds.erase(this); }

}  // namespace oo


std::optional<std::string> oo::ALStreamedSound::name() { return _name; }

void oo::ALStreamedSound::rewind() {
  gStreamedRewinds[this]++;
  oo::Sound::rewind();
}

bool oo::ALStreamedSound::soundIncomplete() { return true; }

ALuint oo::ALStreamedSound::soundBuffer() { return 7; }

// The rewinds of a streamed sound.
static int Rewinds(oo::Sound* sound) {
  const auto found =
      gStreamedRewinds.find(dynamic_cast<oo::ALStreamedSound*>(sound));
  return found != gStreamedRewinds.end() ? found->second : 0;
}

// A subclass that overrides the designated initialiser and wraps a sound, as
// OOMusic does (a C++ subclass).
class TestMusic final : public oo::Sound {
 public:
  static std::shared_ptr<TestMusic> initWithContentsOfFile(
      const std::optional<std::string>& inPath) {
    std::shared_ptr<TestMusic> self = std::shared_ptr<TestMusic>(new TestMusic(), oo::Dispose{});
    self->_sound = oo::Sound::initWithContentsOfFile(inPath);
    if (nullptr == self->_sound) self = nullptr;
    return self;
  }

  std::optional<std::string> name() override { return _sound->name(); }

 private:
  TestMusic() = default;

  std::shared_ptr<oo::Sound> _sound;
};

namespace {

stdfs::path sRoot;

// The null OpenAL backend and a scratch home, before anything asks for either.
void SetUp() {
  if (!sRoot.empty()) return;
  EXPECT_TRUE(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-sound-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
}

bool Near(float a, float b) { return std::fabs(a - b) < 1e-6f; }

float ListenerGain() {
  ALfloat gain = -1.0f;
  alGetListenerf(AL_GAIN, &gain);
  return gain;
}

float VolumeDefault() {
  return oo::Defaults::standard().floatForKey("volume_control");
}

// The log, as the sound writes it.
std::vector<std::string> gLog;

void Capture(std::string_view line) { gLog.emplace_back(line); }

void StartLog() {
  oo::log::logger().setInitialized(true);
  oo::log::logger().setSink(&Capture);
  gLog.clear();
}

int LogLinesContaining(std::string_view text) {
  int count = 0;
  for (const std::string& line : gLog) {
    if (line.find(text) != std::string::npos) count++;
  }
  return count;
}

}  // namespace

// First: making a sound sets the sound system up, at half volume when the user
// has set none.
TEST(OOSound, initSetsUpAtHalfVolume) {
  ::SetUp();
  EXPECT_TRUE(!oo::Sound::isSoundOK());
  EXPECT_TRUE(oo::Defaults::standard().object("volume_control").isNull());

  const std::shared_ptr<oo::Sound> sound = oo::makeShared<oo::Sound>();
  EXPECT_TRUE(sound != nullptr);
  EXPECT_TRUE(oo::Sound::isSoundOK());
  EXPECT_TRUE(oo::Sound::setUp());  // once only, and remembered
  EXPECT_TRUE(Near(oo::Sound::masterVolume(), 0.5f));
  EXPECT_TRUE(Near(ListenerGain(), 0.5f));
  EXPECT_TRUE(Near(VolumeDefault(), 0.5f));
}

TEST(OOSound, masterVolumeIsClampedAndKept) {
  ::SetUp();
  oo::Sound::setMasterVolume(0.25f);
  EXPECT_TRUE(Near(oo::Sound::masterVolume(), 0.25f));
  EXPECT_TRUE(Near(ListenerGain(), 0.25f));
  EXPECT_TRUE(Near(VolumeDefault(), 0.25f));

  oo::Sound::setMasterVolume(2.0f);
  EXPECT_TRUE(Near(oo::Sound::masterVolume(), 1.0f));
  EXPECT_TRUE(Near(VolumeDefault(), 1.0f));

  oo::Sound::setMasterVolume(-1.0f);
  EXPECT_TRUE(Near(oo::Sound::masterVolume(), 0.0f));
  EXPECT_TRUE(Near(ListenerGain(), 0.0f));

  // The default is written only when the volume changes.
  oo::Defaults::standard().setFloat("volume_control", 0.75f);
  oo::Sound::setMasterVolume(0.0f);
  EXPECT_TRUE(Near(VolumeDefault(), 0.75f));
  oo::Sound::setMasterVolume(0.5f);
  EXPECT_TRUE(Near(VolumeDefault(), 0.5f));
}

TEST(OOSound, updateAsksTheMixer) {
  ::SetUp();
  const unsigned before = gMixerUpdates;
  oo::Sound::update();
  oo::Sound::update();
  EXPECT_TRUE(gMixerUpdates == before + 2);
}

TEST(OOSound, rootDefaults) {
  ::SetUp();
  const std::shared_ptr<oo::Sound> sound = oo::makeShared<oo::Sound>();
  const int before = gSubclassResponsibilities;
  EXPECT_TRUE(sound->name() == std::optional<std::string>(std::string()));
  EXPECT_TRUE(gSubclassResponsibilities == before + 1);
  EXPECT_TRUE(sound->soundBuffer() == 0);
  EXPECT_TRUE(gSubclassResponsibilities == before + 2);
  EXPECT_TRUE(!sound->soundIncomplete());
  sound->rewind();
  EXPECT_TRUE(gSubclassResponsibilities == before + 2);
}

// The class cluster: the decoded size picks the concrete sound; the decoder is
// released.
TEST(OOSound, loadingPicksBufferedOrStreamed) {
  ::SetUp();
  StartLog();
  {
    const std::shared_ptr<oo::Sound> small =
        oo::Sound::initWithContentsOfFile(std::string("small.ogg"));
    EXPECT_TRUE(dynamic_cast<oo::ALBufferedSound*>(small.get()) != nullptr);
    EXPECT_TRUE(small->name() == std::optional<std::string>("small.ogg"));
    EXPECT_TRUE(small->soundBuffer() == 42 && !small->soundIncomplete());

    const std::shared_ptr<oo::Sound> edge =
        oo::Sound::initWithContentsOfFile(std::string("edge.ogg"));
    EXPECT_TRUE(dynamic_cast<oo::ALBufferedSound*>(edge.get()) != nullptr);

    const std::shared_ptr<oo::Sound> big =
        oo::Sound::initWithContentsOfFile(std::string("big.ogg"));
    EXPECT_TRUE(dynamic_cast<oo::ALStreamedSound*>(big.get()) != nullptr);
    EXPECT_TRUE(big->name() == std::optional<std::string>("big.ogg"));
    EXPECT_TRUE(big->soundBuffer() == 7 && big->soundIncomplete());
    big->rewind();
    EXPECT_TRUE(Rewinds(big.get()) == 1);

    EXPECT_TRUE(gLiveDecoders == 0);
  }
#ifndef NDEBUG
  EXPECT_TRUE(LogLinesContaining("Loaded sound small.ogg") == 1);
  EXPECT_TRUE(LogLinesContaining("Loaded sound big.ogg") == 1);
#endif
  EXPECT_TRUE(LogLinesContaining("Failed to load sound") == 0);
}

TEST(OOSound, loadingFailures) {
  ::SetUp();
  StartLog();
  // No decoder: null, and nothing logged.
  EXPECT_TRUE(oo::Sound::initWithContentsOfFile(std::nullopt) == nullptr);
  EXPECT_TRUE(oo::Sound::initWithContentsOfFile(std::string("missing.ogg")) ==
           nullptr);
  EXPECT_TRUE(gLog.empty());

  // The concrete sound refuses: null, logged.
  EXPECT_TRUE(oo::Sound::initWithContentsOfFile(std::string("bad.ogg")) == nullptr);
  EXPECT_TRUE(LogLinesContaining("Failed to load sound \"bad.ogg\"") == 1);
  EXPECT_TRUE(gLiveDecoders == 0);
}

TEST(OOSound, subclassOverridingTheInitialiser) {
  ::SetUp();
  const std::shared_ptr<TestMusic> music =
      TestMusic::initWithContentsOfFile(std::string("small.ogg"));
  EXPECT_TRUE(dynamic_cast<TestMusic*>(static_cast<oo::Sound*>(music.get())) !=
           nullptr);
  EXPECT_TRUE(music->name() == std::optional<std::string>("small.ogg"));
  EXPECT_TRUE(!music->soundIncomplete());
  EXPECT_TRUE(TestMusic::initWithContentsOfFile(std::string("missing.ogg")) ==
           nullptr);
}

// A converted sound: a C++ subclass. Global, as a game class is, so that its
// description names it as the game's would.
class TestCxxSound : public oo::Sound {
 public:
  std::optional<std::string> name() override { return "cxx.ogg"; }
  ALuint soundBuffer() override { return 99; }
  void rewind() override { rewinds++; }
  std::optional<std::string> descriptionComponents() const override {
    return "test";
  }

  int rewinds = 0;
};

TEST(OOSound, cxxApi) {
  ::SetUp();
  EXPECT_TRUE(oo::Sound::isSoundOK() && oo::Sound::setUp());
  oo::Sound::setMasterVolume(0.125f);
  EXPECT_TRUE(Near(oo::Sound::masterVolume(), 0.125f));
  const unsigned updates = gMixerUpdates;
  oo::Sound::update();
  EXPECT_TRUE(gMixerUpdates == updates + 1);

  {
    // The factory answers the concrete sound.
    const std::shared_ptr<oo::Sound> small =
        oo::Sound::initWithContentsOfFile(std::string("small.ogg"));
    EXPECT_TRUE(dynamic_cast<oo::ALBufferedSound*>(small.get()) != nullptr);
    EXPECT_TRUE(small->name() == std::optional<std::string>("small.ogg"));
    const std::shared_ptr<oo::Sound> big =
        oo::Sound::initWithContentsOfFile(std::string("big.ogg"));
    EXPECT_TRUE(dynamic_cast<oo::ALStreamedSound*>(big.get()) != nullptr);
    EXPECT_TRUE(!oo::Sound::initWithContentsOfFile(std::nullopt));
    EXPECT_TRUE(!oo::Sound::initWithContentsOfFile(std::string("bad.ogg")));
    EXPECT_TRUE(gLiveDecoders == 0);
  }

  const std::shared_ptr<oo::Sound> root = oo::makeShared<oo::Sound>();
  const int before = gSubclassResponsibilities;
  EXPECT_TRUE(root->name() == std::optional<std::string>(std::string()) &&
           root->soundBuffer() == 0);
  EXPECT_TRUE(gSubclassResponsibilities == before + 2);
  EXPECT_TRUE(!root->soundIncomplete());
  root->rewind();
  EXPECT_TRUE(!root->descriptionComponents().has_value());
}

// A C++ subclass's overrides answer, and the root's defaults; "%@" names the
// class.
TEST(OOSound, cxxSoundBehindTheFacade) {
  ::SetUp();
  const std::shared_ptr<TestCxxSound> sound = oo::makeShared<TestCxxSound>();
  oo::Sound* root = sound.get();

  EXPECT_TRUE(root->name() == std::optional<std::string>("cxx.ogg"));
  EXPECT_TRUE(root->soundBuffer() == 99 && !root->soundIncomplete());
  root->rewind();
  EXPECT_TRUE(sound->rewinds == 1);

  const std::string text = root->description();
  EXPECT_TRUE(text.starts_with("<TestCxxSound 0x"));
  EXPECT_TRUE(text.ends_with(">{test}"));
}

// Virtual calls reach the concrete sounds' overrides, and the root's where they
// have none.
TEST(OOSound, objCSoundBehindACxxPointer) {
  ::SetUp();
  const std::shared_ptr<oo::Sound> part =
      oo::Sound::initWithContentsOfFile(std::string("big.ogg"));
  EXPECT_TRUE(part != nullptr);

  // The streamed sound is C++: its rewind reaches the root's.
  EXPECT_TRUE(part->name() == std::optional<std::string>("big.ogg"));
  EXPECT_TRUE(part->soundBuffer() == 7 && part->soundIncomplete());
  part->rewind();
  EXPECT_TRUE(Rewinds(part.get()) == 1);
  EXPECT_TRUE(!part->descriptionComponents().has_value());

  // A subclass that overrides only some: the root answers the rest.
  const std::shared_ptr<TestMusic> music =
      TestMusic::initWithContentsOfFile(std::string("small.ogg"));
  EXPECT_TRUE(music->name() == std::optional<std::string>("small.ogg"));
  EXPECT_TRUE(!music->soundIncomplete());
}
