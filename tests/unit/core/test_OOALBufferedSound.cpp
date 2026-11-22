/*	test_OOALBufferedSound.mm
        Unit tests for OOALBufferedSound (src/Core/OOALBufferedSound.h): a
   sound of the Audio module, a subclass of the root OOSound.

        A buffered sound decodes the whole of its decoder's sound when it is
   made, and each -soundBuffer is a new OpenAL buffer holding all of it. OpenAL
   runs on OpenAL Soft's null backend (ALSOFT_DRIVERS=null) and the user's
   defaults are a scratch folder's (HOMEPATH), as in test_OOSound.mm. The
   decoder is the game's own, on the game's Resources/Sounds/boop.ogg; the
        streamed sound and the mixer, which OOALSound.mm also names, are this
   file's stubs. The tests ask the C++ class (initWithDecoder(), the class
   checks as dynamic_cast); the description is the root's description() and the
   decoder and the cluster are C++. After them comes the C++ API
   (OOALBufferedSound, a subclass of OOSound). Run: bash
   meson test -C build/meson_test test_OOALBufferedSound
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

// OOALSound.mm makes a streamed sound for more than 1 MB of decoded data; never
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

namespace {

stdfs::path sRoot;

const std::string& BoopPath() {
  static const std::string path =
      stdfs::absolute(stdfs::path(__FILE__).parent_path() / ".." / ".." / ".." /
                      "Resources" / "Sounds" / "boop.ogg")
          .lexically_normal()
          .generic_string();
  return path;
}

// The null OpenAL backend and a scratch home, before anything asks for either.
void SetUp() {
  if (!sRoot.empty()) return;
  (void)BoopPath();
  EXPECT_TRUE(::_putenv_s("ALSOFT_DRIVERS", "null") == 0);
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-bufferedsound-" +
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

// boop.ogg, decoded: mono, 11025 Hz, 7682 bytes.
constexpr ALint kBoopRate = 11025;
constexpr ALint kBoopBytes = 7682;

}  // namespace

TEST(OOALBufferedSound, madeFromADecoder) {
  ::SetUp();
  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  EXPECT_TRUE(decoder != nullptr);
  const std::shared_ptr<oo::ALBufferedSound> sound =
      oo::ALBufferedSound::initWithDecoder(decoder.get());
  EXPECT_TRUE(sound != nullptr);
  EXPECT_TRUE(dynamic_cast<oo::ALBufferedSound*>(sound.get()) != nullptr &&
           dynamic_cast<oo::Sound*>(sound.get()) != nullptr);
  EXPECT_TRUE(sound->name() == std::optional<std::string>("boop.ogg"));
  EXPECT_TRUE(!sound->soundIncomplete());
  sound->rewind();
  EXPECT_TRUE(!sound->soundIncomplete());
  EXPECT_TRUE(sound->description().starts_with("<OOALBufferedSound 0x"));

  EXPECT_TRUE(!oo::ALBufferedSound::initWithDecoder(nullptr));
}

// Each buffer is a new OpenAL buffer with the whole sound.
TEST(OOALBufferedSound, eachBufferHoldsTheWholeSound) {
  ::SetUp();
  {
    const std::shared_ptr<oo::ALBufferedSound> sound = oo::ALBufferedSound::initWithDecoder(
        oo::ALSoundDecoder::codecWithPath(BoopPath()).get());
    const ALuint first = sound->soundBuffer();
    EXPECT_TRUE(first != 0 && alIsBuffer(first));
    EXPECT_TRUE(BufferInt(first, AL_SIZE) == kBoopBytes);
    EXPECT_TRUE(BufferInt(first, AL_FREQUENCY) == kBoopRate);
    EXPECT_TRUE(BufferInt(first, AL_CHANNELS) == 1);
    EXPECT_TRUE(BufferInt(first, AL_BITS) == 16);

    const ALuint second = sound->soundBuffer();
    EXPECT_TRUE(second != 0 && second != first);
    EXPECT_TRUE(BufferInt(second, AL_SIZE) == kBoopBytes);
    alDeleteBuffers(1, &first);
    alDeleteBuffers(1, &second);
  }
}

// The root's class cluster answers a buffered sound for a small file.
TEST(OOALBufferedSound, theClusterAnswersOne) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Sound> sound = oo::Sound::initWithContentsOfFile(BoopPath());
    EXPECT_TRUE(dynamic_cast<oo::ALBufferedSound*>(sound.get()) != nullptr);
    EXPECT_TRUE(sound->name() == std::optional<std::string>("boop.ogg"));
    const ALuint buffer = sound->soundBuffer();
    EXPECT_TRUE(BufferInt(buffer, AL_SIZE) == kBoopBytes);
    alDeleteBuffers(1, &buffer);
  }
}

// The C++ API: the factory answers null where the initialiser answered nil.
TEST(OOALBufferedSound, cxxApi) {
  ::SetUp();
  {
    EXPECT_TRUE(!oo::ALBufferedSound::initWithDecoder(nullptr));
    const std::shared_ptr<oo::ALBufferedSound> sound = oo::ALBufferedSound::initWithDecoder(
        oo::ALSoundDecoder::codecWithPath(BoopPath()).get());
    EXPECT_TRUE(sound && sound->name() == std::optional<std::string>("boop.ogg"));
    EXPECT_TRUE(!sound->soundIncomplete());
    ALuint buffer = sound->soundBuffer();
    EXPECT_TRUE(BufferInt(buffer, AL_SIZE) == kBoopBytes &&
             BufferInt(buffer, AL_FREQUENCY) == kBoopRate);
    alDeleteBuffers(1, &buffer);
  }
}
