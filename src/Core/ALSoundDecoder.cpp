/*

OOALSoundDecoder.m


OOALSound - OpenAL sound implementation for Oolite.
Copyright (C) 2005-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "ALSoundDecoder.h"

#include <vorbis/vorbisfile.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Logging.h"
#include "oofnd/String.hpp"
#include "unzip.h"

namespace oo {

static size_t ReadOXZVorbis(void* ptr, size_t size, size_t nmemb,
                            void* datasource);
static int CloseOXZVorbis(void* datasource);

}  // namespace oo

// not practical to implement these
// static int OOSeekOXZVorbis  (void *datasource, ogg_int64_t offset, int
// whence); static long OOTellOXZVorbis (void *datasource);

namespace {

// The class cluster's one concrete decoder, private to this file.
class ALSoundVorbisCodec final : public oo::ALSoundDecoder {
 public:
  // Was [[OOALSoundVorbisCodec alloc] initWithPath:]: null where it
  // answered nil.
  static std::shared_ptr<oo::ALSoundDecoder> createWithPath(
      const std::optional<std::string>& path);

  ~ALSoundVorbisCodec() override;

  std::optional<std::map<std::string, std::string>> comments();

  bool readCreatingBuffer(std::vector<char>& out) override;
  size_t streamToBuffer(char* buffer) override;
  size_t sizeAsBuffer() override;
  bool isStereo() override;
  std::optional<std::string> descriptionComponents() const override;
  long sampleRate() override;
  void reset() override;
  std::optional<std::string> name() override;

 private:
  ALSoundVorbisCodec() = default;
  bool initWithPath(const std::optional<std::string>& path);

  OggVorbis_File _vf = {};
  std::optional<std::string> _name;  // nil when the path was nil
  bool _readStarted = {};
  bool _seekableStream = {};
  unzFile uf = {};  // @public before, for the OXZ read callback, now its friend

  friend size_t(::oo::ReadOXZVorbis)(void* ptr, size_t size, size_t nmemb,
                                     void* datasource);
};

}  // namespace

std::shared_ptr<oo::ALSoundDecoder> oo::ALSoundDecoder::initWithPath(
    const std::optional<std::string>& inPath) {
  std::shared_ptr<oo::ALSoundDecoder> self;

  if (oo::str::pathExtension(inPath.value_or(std::string())) == "ogg") {
    self = ALSoundVorbisCodec::createWithPath(inPath);
  }

  return self;
}

std::shared_ptr<oo::ALSoundDecoder> oo::ALSoundDecoder::codecWithPath(
    const std::string& inPath) {
  if (oo::str::pathExtension(inPath) == "ogg") {
    return ALSoundVorbisCodec::createWithPath(inPath);
  }
  return nullptr;
}

size_t oo::ALSoundDecoder::streamToBuffer(char* /*ioBuffer*/) { return 0; }

bool oo::ALSoundDecoder::readCreatingBuffer(std::vector<char>& out) {
  out.clear();
  return false;
}

size_t oo::ALSoundDecoder::sizeAsBuffer() { return 0; }

bool oo::ALSoundDecoder::isStereo() { return false; }

long oo::ALSoundDecoder::sampleRate() { return 0; }

void oo::ALSoundDecoder::reset() {
  // nothing
}

std::optional<std::string> oo::ALSoundDecoder::name() { return std::string(); }

std::optional<std::string> oo::ALSoundDecoder::descriptionComponents() const {
  return std::nullopt;
}

std::shared_ptr<oo::ALSoundDecoder> ALSoundVorbisCodec::createWithPath(
    const std::optional<std::string>& path) {
  std::shared_ptr<ALSoundVorbisCodec> codec =
      std::shared_ptr<ALSoundVorbisCodec>(new ALSoundVorbisCodec(),
                                          oo::Dispose{});
  if (!codec->initWithPath(path)) return nullptr;
  return codec;
}

/*	The body of -initWithPath:. [super init] could not fail, so its
   block stays as it was; each
        "[self release]; self = nil;" is return false (the factory drops the
   object, as the release did).
*/
bool ALSoundVorbisCodec::initWithPath(const std::optional<std::string>& path) {
  {
    bool OK = false;

    const std::string pathString = path.value_or(std::string());
    if (path.has_value()) _name = oo::str::lastPathComponent(pathString);

    std::size_t i, cl;
    const std::vector<std::string> components =
        oo::str::pathComponents(pathString);
    cl = components.size();
    for (i = 0; i < cl; i++) {
      const std::string& component = components[i];
      if (oo::str::lowercase(oo::str::pathExtension(component)) == "oxz") {
        break;
      }
    }
    // if i == cl then the path is entirely uncompressed
    if (i == cl) {
      /* Get vorbis data from a standard file stream */
      int err;
      FILE* file;

      _seekableStream = true;

      if (path.has_value()) {
        file = fopen(path->c_str(), "rb");
        if (nullptr != file) {
          err = ov_open_callbacks(file, &_vf, nullptr, 0, OV_CALLBACKS_DEFAULT);
          if (0 == err) {
            OK = true;
          }
        }
      }

      if (!OK) {
        return false;
      }
    } else {
      _seekableStream = false;

      const std::string zipFile =
          oo::str::pathWithComponents(std::vector<std::string>(
              components.begin(),
              components.begin() + static_cast<std::ptrdiff_t>(i) + 1));
      const std::string containedFile =
          oo::str::pathWithComponents(std::vector<std::string>(
              components.begin() + static_cast<std::ptrdiff_t>(i) + 1,
              components.end()));

      uf = unzOpen64(zipFile.c_str());
      if (uf == nullptr) {
        OO_LOG(oo::kLogFileNotFound, "Could not unzip OXZ at {}", zipFile);
        return false;
      } else {
        // unzLocateFile(*, *, 1) = case-sensitive extract
        if (unzLocateFile(uf, containedFile.c_str(), 1) != UNZ_OK) {
          unzClose(uf);
          return false;
        } else {
          int err = UNZ_OK;
          unz_file_info64 file_info = {};
          err = unzGetCurrentFileInfo64(uf, &file_info, nullptr, 0, nullptr, 0,
                                        nullptr, 0);
          if (err != UNZ_OK) {
            unzClose(uf);
            OO_LOG(oo::kLogFileNotFound,
                   "Could not get properties of {} within OXZ at {}",
                   containedFile, zipFile);
            return false;
          } else {
            err = unzOpenCurrentFile(uf);
            if (err != UNZ_OK) {
              unzClose(uf);
              OO_LOG(oo::kLogFileNotFound, "Could not read {} within OXZ at {}",
                     containedFile, zipFile);
              return false;
            } else {
              ov_callbacks _callbacks = {
                  oo::ReadOXZVorbis,   // read sequentially
                  nullptr,             // no seek
                  oo::CloseOXZVorbis,  // close file
                  nullptr,             // no tell
              };
              err = ov_open_callbacks(this, &_vf, nullptr, 0, _callbacks);
              if (0 == err) {
                OK = true;
                _readStarted = false;
              }
              if (!OK) {
                unzClose(uf);
                return false;
              }
            }
          }
        }
      }
    }
  }
#ifdef OOLITE_DEBUG_SOUND_FILE_OPENING
  {
    OO_LOG("sound.retain", "{}", _name.value_or("(null)"));
  }
#endif
  return true;
}

ALSoundVorbisCodec::~ALSoundVorbisCodec() {
#ifdef OOLITE_DEBUG_SOUND_FILE_OPENING
  {
    OO_LOG("sound.release", "{}", _name.value_or("(null)"));
  }
#endif

  ov_clear(&_vf);
  unzClose(uf);
}

std::optional<std::map<std::string, std::string>>
ALSoundVorbisCodec::comments() {
  vorbis_comment* comments;
  unsigned i, count;
  std::optional<std::map<std::string, std::string>> result;

  comments = ov_comment(&_vf, -1);
  if (nullptr != comments) {
    count = static_cast<unsigned int>(comments->comments);
    if (0 != count) {
      result.emplace();
      for (i = 0; i != count; ++i) {
        const std::string comment(
            comments->user_comments[i],
            static_cast<std::size_t>(comments->comment_lengths[i]));
        const std::size_t equals = comment.find('=');
        if (equals != std::string::npos) {
          (*result)[comment.substr(0, equals)] = comment.substr(equals + 1);
        } else {
          (*result)[comment] = "";
        }
      }
    }
  }

  return result;
}

bool ALSoundVorbisCodec::readCreatingBuffer(std::vector<char>& out) {
  char* dst;
  size_t sizeInFrames = 0;
  int remaining;
  std::int64_t framesRead;
  // 16-bit samples, either two or one track
  int frameSize = isStereo() ? 4 : 2;
  ogg_int64_t totalSizeInFrames;

  out.clear();

  totalSizeInFrames = ov_pcm_total(&_vf, -1);
  assert(
      static_cast<uint64_t>(totalSizeInFrames) <
      static_cast<uint64_t>(SIZE_MAX));  // Should have been checked by caller
  sizeInFrames = static_cast<size_t>(totalSizeInFrames);

  std::vector<char> buffer(static_cast<size_t>(frameSize) * sizeInFrames);

  if (sizeInFrames) {
    remaining =
        static_cast<int>(std::min(static_cast<size_t>(frameSize) * sizeInFrames,
                                  static_cast<size_t>(INT_MAX)));
    dst = buffer.data();

    std::array<char, 4096> pcmout{};

    do {
      int toRead = static_cast<int>(pcmout.size());
      if (remaining < toRead) {
        toRead = remaining;
      }
      framesRead = ov_read(&_vf, pcmout.data(), toRead, 0, 2, 1, nullptr);
      if (framesRead <= 0) {
        if (OV_HOLE == framesRead) continue;
        // else:
        break;
      }

      memcpy(dst, pcmout.data(),
             sizeof(char) * static_cast<std::size_t>(static_cast<std::size_t>(
                                static_cast<std::size_t>(framesRead))));

      remaining -= framesRead;
      dst += framesRead;
    } while (0 < remaining);

    sizeInFrames -=
        static_cast<size_t>(remaining);  // In case we stopped at an error
  }

  buffer.resize(
      std::min(buffer.size(), sizeInFrames * static_cast<size_t>(frameSize)));
  out = std::move(buffer);
  return true;
}

size_t ALSoundVorbisCodec::streamToBuffer(char* buffer) {
  int remaining = oo::AL_STREAM_CHUNK_SIZE;
  size_t streamed = 0;
  std::int64_t framesRead;

  char* dst = buffer;
  std::array<char, 4096> pcmout{};
  _readStarted = true;
  do {
    int toRead = static_cast<int>(pcmout.size());
    if (remaining < toRead) {
      toRead = remaining;
    }
    framesRead = ov_read(&_vf, pcmout.data(), toRead, 0, 2, 1, nullptr);
    if (framesRead <= 0) {
      if (OV_HOLE == framesRead) continue;
      // else:
      break;
    }
    memcpy(dst, pcmout.data(),
           sizeof(char) * static_cast<std::size_t>(static_cast<std::size_t>(
                              static_cast<std::size_t>(framesRead))));
    remaining -=
        sizeof(char) * static_cast<std::size_t>(static_cast<std::size_t>(
                           static_cast<std::size_t>(framesRead)));
    dst += sizeof(char) * static_cast<std::size_t>(static_cast<std::size_t>(
                              static_cast<std::size_t>(framesRead)));
    streamed +=
        sizeof(char) * static_cast<std::size_t>(static_cast<std::size_t>(
                           static_cast<std::size_t>(framesRead)));
  } while (0 < remaining);

  return streamed;
}

size_t ALSoundVorbisCodec::sizeAsBuffer() {
  ogg_int64_t size;
  size = ov_pcm_total(&_vf, -1);
  size *= sizeof(char) * (isStereo() ? 4 : 2);
  if (static_cast<uint64_t>(SIZE_MAX) < static_cast<uint64_t>(size))
    size = static_cast<ogg_int64_t>(SIZE_MAX);
  return static_cast<size_t>(size);
}

bool ALSoundVorbisCodec::isStereo() { return 1 < ov_info(&_vf, -1)->channels; }

// The description wraps this as "<OOALSoundVorbisCodec 0x...>{...}".
// comments() reads the
// file through a non-const pointer, so it is reached through const_cast.
std::optional<std::string> ALSoundVorbisCodec::descriptionComponents() const {
  oo::PList commentList;
  if (const auto comments = const_cast<ALSoundVorbisCodec*>(this)->comments()) {
    oo::PList::Dict dict;
    for (const auto& [key, value] : *comments)
      dict.emplace(key, oo::PList(value));
    commentList = oo::PList(std::move(dict));
  }
  return std::format("\"{}\", comments={}", _name.value_or("(null)"),
                     oo::DescriptionOf(commentList));
}

long ALSoundVorbisCodec::sampleRate() { return ov_info(&_vf, -1)->rate; }

void ALSoundVorbisCodec::reset() {
  if (!_readStarted) {
    return;  // don't need to do anything
  }
  if (_seekableStream) {
    ov_pcm_seek(&_vf, 0);
    return;
  }
  // reset current file pointer in OXZ
  unzOpenCurrentFile(uf);
  // reopen OGG streamer
  ov_clear(&_vf);
  ov_callbacks _callbacks = {
      oo::ReadOXZVorbis,   // read sequentially
      nullptr,             // no seek
      oo::CloseOXZVorbis,  // close file
      nullptr,             // no tell
  };
  ov_open_callbacks(this, &_vf, nullptr, 0, _callbacks);
  _readStarted = false;
}

std::optional<std::string> ALSoundVorbisCodec::name() { return _name; }

namespace oo {

static size_t ReadOXZVorbis(void* ptr, size_t size, size_t nmemb,
                            void* datasource) {
  ALSoundVorbisCodec* src = static_cast<ALSoundVorbisCodec*>(datasource);
  size_t toRead = size * nmemb;
  // Read straight into the decoder's buffer. This used to read into a malloc'd
  // copy that was never freed: a leak on every read of a Vorbis file in an OXZ.
  int err = UNZ_OK;
  err = unzReadCurrentFile(src->uf, ptr, static_cast<unsigned int>(toRead));
  //	OO_LOG("sound.replay", "Read {} blocks, got {}",
  // static_cast<int>(toRead), static_cast<int>(err));
  if (err < 0) {
    return static_cast<size_t>(OV_EREAD);
  }
  return static_cast<size_t>(err);
}

int CloseOXZVorbis(void* /*datasource*/) {
  //  doing this prevents replaying
  //	OOALSoundVorbisCodec *src = (OOALSoundVorbisCodec *)datasource;
  //	unzClose(src->uf);
  return 0;
}

}  // namespace oo
