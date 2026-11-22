/*	test_OOALSoundDecoder.mm
        Unit tests for OOALSoundDecoder (src/Core/OOALSoundDecoder.h).

        The decoder is a class cluster: -initWithPath: and +codecWithPath:
   answer the private Vorbis codec for a path ending in "ogg" (a file, or a file
   inside an OXZ) and nil otherwise; the public class's own answers are a
   decoder of nothing. The codec reads the whole sound into a buffer, or streams
   it in chunks of OOAL_STREAM_CHUNK_SIZE and starts again on -reset. The sound
   decoded here is the game's own Resources/Sounds/boop.ogg, found from this
   file's path, so the answers are the file's: Vorbis decodes the same samples
   on every machine. The tests ask the C++ class. After them comes the C++
   API's own case. Run: meson test -C build/meson_test test_OOALSoundDecoder
*/

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "ALSoundDecoder.h"
#include "Logging.h"
#include "oofnd/Log.hpp"

namespace stdfs = std::filesystem;

// The game's OOLogging.mm reaches the resource manager, so it is not linked;
// the decoder's one message class from it is defined here.
namespace oo {

const char* const kLogFileNotFound = "files.notFound";

}  // namespace oo


namespace {

// Resources/Sounds/boop.ogg, from this file's path (tests/unit/core/), made
// absolute before anything changes the current directory.
const std::string& BoopPath() {
  static const std::string path =
      stdfs::absolute(stdfs::path(__FILE__).parent_path() / ".." / ".." / ".." /
                      "Resources" / "Sounds" / "boop.ogg")
          .lexically_normal()
          .generic_string();
  return path;
}

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

// boop.ogg, as Vorbis decodes it: mono, 11025 Hz, 3841 frames of 16 bits.
constexpr long kBoopRate = 11025;
constexpr size_t kBoopBytes = 3841 * 2;

}  // namespace

TEST(OOALSoundDecoder, clusterPicksTheCodecByExtension) {
  StartLog();
  EXPECT_TRUE(stdfs::exists(BoopPath()));

  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::initWithPath(BoopPath());
  EXPECT_TRUE(decoder != nullptr);
  EXPECT_TRUE(dynamic_cast<oo::ALSoundDecoder*>(decoder.get()) != nullptr);
  EXPECT_TRUE(decoder->name() == std::optional<std::string>("boop.ogg"));

  EXPECT_TRUE(oo::ALSoundDecoder::initWithPath(std::nullopt) == nullptr);
  EXPECT_TRUE(oo::ALSoundDecoder::initWithPath(std::string("boop.wav")) == nullptr);
  EXPECT_TRUE(oo::ALSoundDecoder::initWithPath(std::string("no-such-file.ogg")) ==
           nullptr);
  EXPECT_TRUE(gLog.empty());

  EXPECT_TRUE(oo::ALSoundDecoder::codecWithPath(BoopPath()) != nullptr);
  EXPECT_TRUE(oo::ALSoundDecoder::codecWithPath(BoopPath())->name() ==
           std::optional<std::string>("boop.ogg"));
  EXPECT_TRUE(oo::ALSoundDecoder::codecWithPath("boop.wav") == nullptr);
  EXPECT_TRUE(oo::ALSoundDecoder::codecWithPath("no-such-file.ogg") == nullptr);

  // A path through an OXZ that is not there: null, logged.
  EXPECT_TRUE(oo::ALSoundDecoder::initWithPath(
               std::string("no-such.oxz/Sounds/x.ogg")) == nullptr);
  EXPECT_TRUE(LogLinesContaining("Could not unzip OXZ at no-such.oxz") == 1);
}

TEST(OOALSoundDecoder, theSoundsProperties) {
  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  EXPECT_TRUE(!decoder->isStereo());
  EXPECT_TRUE(decoder->sampleRate() == kBoopRate);
  EXPECT_TRUE(decoder->sizeAsBuffer() == kBoopBytes);

  // What the description printed between its braces (the facade's
  // "<OOALSoundVorbisCodec 0x..." went with it).
  EXPECT_TRUE(decoder->descriptionComponents().value_or("").starts_with(
      "\"boop.ogg\", comments="));
}

TEST(OOALSoundDecoder, readsTheWholeSound) {
  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  std::vector<char> buffer;
  EXPECT_TRUE(decoder->readCreatingBuffer(buffer));
  size_t size = buffer.size();
  EXPECT_TRUE(!buffer.empty() && size == kBoopBytes);
  bool silent = true;
  for (size_t i = 0; i < size; i++) silent = silent && buffer[i] == 0;
  EXPECT_TRUE(!silent);

  // No out-parameters: refused (the out-parameter is a reference now).
  static_assert(
      !std::is_invocable_v<decltype(&oo::ALSoundDecoder::readCreatingBuffer),
                           oo::ALSoundDecoder*, std::nullptr_t>);
}

TEST(OOALSoundDecoder, streamsAndStartsAgain) {
  const std::shared_ptr<oo::ALSoundDecoder> whole =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  std::vector<char> expected;
  EXPECT_TRUE(whole->readCreatingBuffer(expected));

  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  std::vector<char> chunk(oo::AL_STREAM_CHUNK_SIZE);
  EXPECT_TRUE(decoder->streamToBuffer(chunk.data()) == kBoopBytes);
  EXPECT_TRUE(!expected.empty() &&
           std::memcmp(chunk.data(), expected.data(), kBoopBytes) == 0);
  EXPECT_TRUE(decoder->streamToBuffer(chunk.data()) == 0);  // the end

  decoder->reset();
  std::fill(chunk.begin(), chunk.end(), 0);
  EXPECT_TRUE(decoder->streamToBuffer(chunk.data()) == kBoopBytes);
  EXPECT_TRUE(std::memcmp(chunk.data(), expected.data(), kBoopBytes) == 0);
}

// The public class's own answers: a decoder of nothing.
TEST(OOALSoundDecoder, theRootDecodesNothing) {
  const std::shared_ptr<oo::ALSoundDecoder> none = oo::makeShared<oo::ALSoundDecoder>();
  EXPECT_TRUE(none != nullptr);
  std::vector<char> buffer(1);
  EXPECT_TRUE(!none->readCreatingBuffer(buffer));
  EXPECT_TRUE(buffer.empty());
  char chunk[4] = {};
  EXPECT_TRUE(none->streamToBuffer(chunk) == 0);
  EXPECT_TRUE(none->sizeAsBuffer() == 0 && !none->isStereo() &&
           none->sampleRate() == 0);
  none->reset();
  EXPECT_TRUE(none->name() == std::optional<std::string>(std::string()));
}

// The C++ API: the same answers, null where the facade answered nil.
TEST(OOALSoundDecoder, cxxApi) {
  EXPECT_TRUE(!oo::ALSoundDecoder::initWithPath(std::nullopt));
  EXPECT_TRUE(!oo::ALSoundDecoder::initWithPath(std::string("boop.wav")));
  EXPECT_TRUE(!oo::ALSoundDecoder::codecWithPath("no-such-file.ogg"));

  const std::shared_ptr<oo::ALSoundDecoder> decoder =
      oo::ALSoundDecoder::initWithPath(BoopPath());
  EXPECT_TRUE(decoder &&
           decoder->name() == std::optional<std::string>("boop.ogg"));
  EXPECT_TRUE(!decoder->isStereo() && decoder->sampleRate() == kBoopRate &&
           decoder->sizeAsBuffer() == kBoopBytes);
  EXPECT_TRUE(decoder->descriptionComponents().value_or("").starts_with(
      "\"boop.ogg\", comments="));

  std::vector<char> buffer;
  EXPECT_TRUE(decoder->readCreatingBuffer(buffer) && buffer.size() == kBoopBytes);

  const std::shared_ptr<oo::ALSoundDecoder> streaming =
      oo::ALSoundDecoder::codecWithPath(BoopPath());
  std::vector<char> chunk(oo::AL_STREAM_CHUNK_SIZE);
  EXPECT_TRUE(streaming->streamToBuffer(chunk.data()) == kBoopBytes);
  EXPECT_TRUE(streaming->streamToBuffer(chunk.data()) == 0);
  streaming->reset();
  EXPECT_TRUE(streaming->streamToBuffer(chunk.data()) == kBoopBytes);

  const std::shared_ptr<oo::ALSoundDecoder> none = oo::makeShared<oo::ALSoundDecoder>();
  EXPECT_TRUE(none->sizeAsBuffer() == 0 &&
           none->name() == std::optional<std::string>(std::string()));
  EXPECT_TRUE(!none->descriptionComponents().has_value());
}
