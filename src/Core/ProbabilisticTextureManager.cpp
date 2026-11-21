/*

OOProbabilisticTextureManager.m


Copyright (C) 2007-2013 Jens Ayton

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

#include "ProbabilisticTextureManager.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "TextureFlags.h"
#include "Types.h"
#include "legacy_random.h"
#include "oofnd/Log.hpp"
#include "oofnd/Object.hpp"
#include "oofnd/PListDescription.hpp"
#include "oofnd/String.hpp"

namespace {

// oo_stringForKey: a string, a number's string value, or nil.
std::optional<std::string> OptionalStringForKey(const oo::PList& dict,
                                                std::string_view key) {
  const oo::PList* value = dict.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return dict.get<std::string>(key);
}

}  // namespace

std::shared_ptr<oo::ProbabilisticTextureManager>
oo::ProbabilisticTextureManager::createWithPListName(
    const std::string& plistName, oo::TextureFlags options, GLfloat anisotropy,
    GLfloat lodBias) {
  return createWithPListName(plistName, options, anisotropy, lodBias,
                             RANROTGetFullSeed());
}

// -initWithPListName:...seed: on a new object; null where it answered nil.
std::shared_ptr<oo::ProbabilisticTextureManager>
oo::ProbabilisticTextureManager::createWithPListName(
    const std::string& plistName, oo::TextureFlags options, GLfloat anisotropy,
    GLfloat lodBias, RANROTSeed seed) {
  std::shared_ptr<oo::ProbabilisticTextureManager> manager =
      std::shared_ptr<oo::ProbabilisticTextureManager>(
          new oo::ProbabilisticTextureManager(), oo::Dispose{});
  if (!manager->initWithPListName(plistName, options, anisotropy, lodBias,
                                  seed))
    return nullptr;
  return manager;
}

bool oo::ProbabilisticTextureManager::initWithPListName(
    const std::string& plistName, oo::TextureFlags options, GLfloat anisotropy,
    GLfloat lodBias, RANROTSeed seed) {
  bool OK = true;
  oo::PList config;
  std::size_t i, count, j;
  const oo::PList* entry = nullptr;
  std::optional<std::string> name;
  float probability = 0.0f;  // set wherever name is (GCC -Wmaybe-uninitialized)
  std::shared_ptr<oo::Texture> texture;
  int galID = -1;
  const oo::PList* object = nullptr;

  if (OK) {
    config = ResourceManager::arrayFromFilesNamed(plistName, "Config", true);
    if (!config) OK = false;
  }

  if (OK) {
    count = config.count();

    _textures.resize(count);
    _prob.resize(count);
    _galaxy.resize(count);
    _probMaxGal.assign(oo::kMaximumGalaxyID + 1, 0.0f);

    //  Go through list and load textures.
    for (i = 0; i != count; ++i) {
      entry = config.at(i);
      galID = -1;
      if (entry->isDict()) {
        name = OptionalStringForKey(*entry, "texture");
        probability = entry->get<float>("probability", 1.0f);
        object = entry->find("galaxy");
        if (object != nullptr && object->isString()) {
          galID = oo::str::intValue(*object->getIf<std::string>());
        } else if (object != nullptr) {
          OO_LOG("textures.load",
                 "***** ERROR: {} for texture {} is not a string.", "galaxy",
                 name.value_or("(null)"));
        }
      } else if (entry->isString()) {
        name = *entry->getIf<std::string>();
        probability = 1.0f;
      } else {
        name = std::nullopt;
      }

      if (name.has_value() && 0.0f < probability) {
        texture = oo::Texture::textureWithName(name, "Textures", options,
                                               anisotropy, lodBias);
        if (texture != nullptr) {
          _textures[_count] = texture;
          _prob[_count] =
              probability + (galID >= 0
                                 ? _probMaxGal[static_cast<std::size_t>(galID)]
                                 : _probMax);
          _galaxy[_count] = (galID >= 0 ? galID : -1);
          if (galID >= 0) {
            _probMaxGal[static_cast<std::size_t>(galID)] += probability;
          } else {
            for (j = 0; j <= oo::kMaximumGalaxyID; j++)
              _probMaxGal[j] += probability;
            _probMax += probability;
          }
          ++_count;
        }
      }
    }

    if (_count == 0) OK = false;
  }

  if (OK) _seed = seed;

  return OK;
}

namespace oo {

ProbabilisticTextureManager::~ProbabilisticTextureManager() = default;

}  // namespace oo

// OOObject's -description wrapped this as "<OOProbabilisticTextureManager
// 0x...>{...}", the text this class's own -description printed.
std::optional<std::string>
oo::ProbabilisticTextureManager::descriptionComponents() const {
  return std::format("{} textures, cumulative probability={:g}", _count,
                     _probMax);
}

oo::Texture* oo::ProbabilisticTextureManager::selectTexture() {
  float selection;
  unsigned i;
  int hold = -1;
  // qualified: the final overrider, so the
  // binding test stands in for it
  int galID = (PLAYER != nullptr ? PLAYER->PlayerEntity::currentGalaxyID() : 0);

  selection = randfWithSeed(&_seed);

  selection *= _probMaxGal[static_cast<std::size_t>(galID)];

  for (i = 0; i != _count; ++i) {
    if (_galaxy[i] == -1 || _galaxy[i] == galID) {
      // make a note of the index of the first texture that meets the galaxy
      // list criteria (but only if _galaxy is set)
      if (hold == -1 && _prob[i] > 0 && _galaxy[i] == galID)
        hold = static_cast<int>(i);
      if (selection <= _prob[i]) return _textures[i].get();
    }
  }

  // first catch point if loop above fails to return a texture
  // return first texture that meets the galaxy list criteria and has a
  // probability > 0
  if (hold >= 0) {
    OO_LOG("probabilisticTextureManager.internalWarning",
           "{}: overrun! Galaxy List requirements not met. Choosing first "
           "texture available for galaxy.",
           __PRETTY_FUNCTION__);
    return _textures[static_cast<std::size_t>(hold)].get();
  }

  OO_LOG("probabilisticTextureManager.internalFailure",
         "{}: overrun! Choosing last texture.", __PRETTY_FUNCTION__);
  return _textures[_count - 1].get();
}

unsigned oo::ProbabilisticTextureManager::textureCount() { return _count; }

void oo::ProbabilisticTextureManager::ensureTexturesLoaded() {
  unsigned i;

  for (i = 0; i != _count; ++i) {
    _textures[i]->ensureFinishedLoading();
  }
}

RANROTSeed oo::ProbabilisticTextureManager::seed() { return _seed; }

void oo::ProbabilisticTextureManager::setSeed(RANROTSeed seed) { _seed = seed; }
