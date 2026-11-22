/*	test_OOALStreamedSound.mm
        Unit tests for OOALStreamedSound (src/Core/OOALStreamedSound.h): a
   sound of the Audio module, a subclass of the root OOSound.

        A streamed sound keeps its decoder and decodes OOAL_STREAM_CHUNK_SIZE
   bytes into each -soundBuffer, a new OpenAL buffer, until a chunk comes back
   short; -rewind starts the decoder again. OpenAL runs on OpenAL Soft's null
   backend (ALSOFT_DRIVERS=null) and the user's defaults are a scratch folder's
   (HOMEPATH), as in test_OOSound.mm. The decoder is the game's own, on the
   game's Resources: boop.ogg fits in one chunk, OoliteTheme.ogg (the music)
   needs many and is the one the root's class cluster streams. The buffered
   sound and the mixer, which OOALSound.mm also names, are this file's stubs
   as well. The tests ask the C++ class (initWithDecoder(), the class checks as
   dynamic_cast); the description is the root's description(), the buffer helper
   calls the C++ sound and the decoder and the cluster are C++. After them comes
   the C++ API (OOALStreamedSound, a subclass of OOSound). Run:
   meson test -C build/meson_test test_OOALStreamedSound
*/

#include <process.h>

#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <string>

#include <gtest/gtest.h>

#include "ALBufferedSound.h"
#include "ALSoundDecoder.h"
#include "ALSoundMixer.h"
#include "ALStreamedSound.h"

namespace stdfs = std::filesystem;

// Link stubs for OOLogging.mm, which reaches the resource manager.
namespace oo {

const char* const kLogFileNotFound = "files.notFound";

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
}

}  // namespace oo


// The mixer, as the root's update() and the OpenAL controller's shutdown()
// reach it: there is none.
oo::SoundMixer* oo::SoundMixer::sharedMixer() { return nullptr; }
void oo::SoundMixer::update() {}
void oo::SoundMixer::shutdown() {}

// OOALSound.mm makes a buffered sound for up to 1 MB of decoded data; never
// here.
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

namespace {

stdfs::path sRoot;

std::string ResourcePath(const char* folder, const char* file) {
  return stdfs::absolute(stdfs::path(__FILE__).parent_path() / ".." / ".." /
                         ".." / "Resources" / folder / file)
      .lexically_normal()
      .generic_string();
}

const std::string& BoopPath() {
  static const std::string path = ResourcePath("Sounds", "boop.ogg");
  return path;
}

const std::string& ThemePath() {
  static const std::string path = ResourcePath("Music", "OoliteTheme.ogg");
  return path;
}

// The null OpenAL backend and a scratch home, before anything asks for either.
void SetUp() {
  if (!sRoot.empty()) return;
  (void)BoopPath();
  (void)ThemePath();
  EXPECT_TRUE(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-streamedsound-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  EXPECT_TRUE(oo::Sound::setUp());
}

ALint BufferInt(ALuint buffer, ALenum what) {
  ALint value = -1;
  alGetBufferi(buffer, what, &value);
  return value;
}

// The next buffer's size; the buffer is deleted.
ALint NextBufferSize(oo::Sound* sound) {
  ALuint buffer = sound->soundBuffer();
  const ALint size = BufferInt(buffer, AL_SIZE);
  alDeleteBuffers(1, &buffer);
  return size;
}

// boop.ogg, decoded: mono, 11025 Hz, 7682 bytes.
constexpr ALint kBoopRate = 11025;
constexpr ALint kBoopBytes = 7682;

}  // namespace

TEST(OOALStreamedSound, madeFromADecoder) {
  ::SetUp();
  {
    const std::shared_ptr<oo::ALSoundDecoder> decoder =
        oo::ALSoundDecoder::codecWithPath(BoopPath());
    EXPECT_TRUE(decoder != nullptr);
    const std::shared_ptr<oo::ALStreamedSound> sound =
        oo::ALStreamedSound::initWithDecoder(decoder.get());
    EXPECT_TRUE(sound != nullptr);
    EXPECT_TRUE(dynamic_cast<oo::ALStreamedSound*>(sound.get()) != nullptr &&
             dynamic_cast<oo::Sound*>(sound.get()) != nullptr);
    EXPECT_TRUE(sound->name() == std::optional<std::string>("boop.ogg"));
    EXPECT_TRUE(sound->soundIncomplete());  // nothing streamed yet
    EXPECT_TRUE(sound->description().starts_with("<OOALStreamedSound 0x"));

    EXPECT_TRUE(!oo::ALStreamedSound::initWithDecoder(nullptr));
  }
}

// A sound shorter than a chunk: one short buffer, then complete; a rewind
// starts it again.
TEST(OOALStreamedSound, aShortSoundIsOneChunk) {
  ::SetUp();
  {
    const std::shared_ptr<oo::ALStreamedSound> sound = oo::ALStreamedSound::initWithDecoder(
        oo::ALSoundDecoder::codecWithPath(BoopPath()).get());
    ALuint buffer = sound->soundBuffer();
    EXPECT_TRUE(buffer != 0 && alIsBuffer(buffer));
    EXPECT_TRUE(BufferInt(buffer, AL_SIZE) == kBoopBytes);
    EXPECT_TRUE(BufferInt(buffer, AL_FREQUENCY) == kBoopRate);
    EXPECT_TRUE(BufferInt(buffer, AL_CHANNELS) == 1);
    alDeleteBuffers(1, &buffer);
    EXPECT_TRUE(!sound->soundIncomplete());

    EXPECT_TRUE(NextBufferSize(sound.get()) ==
             0);  // past the end: an empty buffer

    sound->rewind();
    EXPECT_TRUE(sound->soundIncomplete());
    EXPECT_TRUE(NextBufferSize(sound.get()) == kBoopBytes);
    EXPECT_TRUE(!sound->soundIncomplete());
  }
}

// The music is streamed by the root's class cluster, a chunk at a time, in
// stereo.
TEST(OOALStreamedSound, theClusterStreamsTheMusic) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Sound> sound = oo::Sound::initWithContentsOfFile(ThemePath());
    EXPECT_TRUE(dynamic_cast<oo::ALStreamedSound*>(sound.get()) != nullptr);
    EXPECT_TRUE(sound->name() == std::optional<std::string>("OoliteTheme.ogg"));

    ALuint buffer = sound->soundBuffer();
    EXPECT_TRUE(BufferInt(buffer, AL_SIZE) ==
             static_cast<ALint>(oo::AL_STREAM_CHUNK_SIZE));
    EXPECT_TRUE(BufferInt(buffer, AL_CHANNELS) == 2);
    alDeleteBuffers(1, &buffer);
    EXPECT_TRUE(sound->soundIncomplete());
    EXPECT_TRUE(NextBufferSize(sound.get()) ==
             static_cast<ALint>(oo::AL_STREAM_CHUNK_SIZE));

    sound->rewind();
    EXPECT_TRUE(sound->soundIncomplete());
    EXPECT_TRUE(NextBufferSize(sound.get()) ==
             static_cast<ALint>(oo::AL_STREAM_CHUNK_SIZE));
  }
}

// The C++ API: the factory answers null where the initialiser answered nil.
TEST(OOALStreamedSound, cxxApi) {
  ::SetUp();
  {
    EXPECT_TRUE(!oo::ALStreamedSound::initWithDecoder(nullptr));
    const std::shared_ptr<oo::ALStreamedSound> sound = oo::ALStreamedSound::initWithDecoder(
        oo::ALSoundDecoder::codecWithPath(BoopPath()).get());
    EXPECT_TRUE(sound && sound->name() == std::optional<std::string>("boop.ogg"));
    EXPECT_TRUE(sound->soundIncomplete());
    ALuint buffer = sound->soundBuffer();
    EXPECT_TRUE(BufferInt(buffer, AL_SIZE) == kBoopBytes &&
             BufferInt(buffer, AL_FREQUENCY) == kBoopRate);
    alDeleteBuffers(1, &buffer);
    EXPECT_TRUE(!sound->soundIncomplete());
    sound->rewind();
    EXPECT_TRUE(sound->soundIncomplete());
  }
}
