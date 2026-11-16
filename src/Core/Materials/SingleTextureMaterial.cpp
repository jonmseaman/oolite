/*

OOSingleTextureMaterial.h


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

#include "SingleTextureMaterial.h"

#include <memory>
#include <string>
#include <vector>

#include "Texture.h"

std::shared_ptr<oo::SingleTextureMaterial>
oo::SingleTextureMaterial::materialWithName(
    const std::optional<std::string>& name, const oo::PList& configuration) {
  std::shared_ptr<oo::SingleTextureMaterial> result =
      oo::makeShared<oo::SingleTextureMaterial>();
  if (!result->initWithName(name, configuration)) return nullptr;
  return result;
}

std::shared_ptr<oo::SingleTextureMaterial>
oo::SingleTextureMaterial::materialWithName(
    const std::optional<std::string>& name, oo::Texture* texture,
    const oo::PList& configuration) {
  std::shared_ptr<oo::SingleTextureMaterial> result =
      oo::makeShared<oo::SingleTextureMaterial>();
  if (!result->initWithName(name, texture, configuration)) return nullptr;
  return result;
}

bool oo::SingleTextureMaterial::initWithName(
    const std::optional<std::string>& name, const oo::PList& configuration) {
  oo::PList texSpec;

  if (!configuration.isNull()) {
    // -oo_textureSpecifierForKey:@"diffuse_map" defaultName:name.
    const oo::PList* diffuseMap = configuration.find("diffuse_map");
    texSpec = oo::TextureSpecFromObject(
        (diffuseMap != nullptr) ? *diffuseMap : oo::PList(), name);
  } else if (name.has_value()) {
    texSpec = oo::PList(*name);
  }

  return initWithName(name,
                      oo::Texture::textureWithConfiguration(texSpec).get(),
                      configuration);
}

bool oo::SingleTextureMaterial::initWithName(
    const std::optional<std::string>& name, oo::Texture* texture,
    const oo::PList& configuration) {
  if (name.has_value() && texture != nullptr) {
    oo::BasicMaterial::initWithName(name, configuration);
    _texture = oo::share(texture);
  } else {
    return false;  // DESTROY(self): the factory drops the object
  }

  return true;
}

std::optional<std::string> oo::SingleTextureMaterial::descriptionComponents()
    const {
  // The texture's "%@" (-description; nil answered nullopt).
  if (_texture == nullptr) return std::nullopt;
  return _texture->description();
}

bool oo::SingleTextureMaterial::doApply() {
  if (!oo::BasicMaterial::doApply()) [[unlikely]]
    return false;

  if (_texture != nullptr) _texture->apply();  // a message to nil did nothing
  return true;
}

void oo::SingleTextureMaterial::unapplyWithNext(oo::Material* next) {
  // -isKindOfClass: (nil is not one)
  if (dynamic_cast<oo::SingleTextureMaterial*>(next) == nullptr)
    oo::Texture::applyNone();
  oo::BasicMaterial::unapplyWithNext(next);
}

void oo::SingleTextureMaterial::ensureFinishedLoading() {
  if (_texture != nullptr) _texture->ensureFinishedLoading();
}

bool oo::SingleTextureMaterial::isFinishedLoading() {
  return _texture != nullptr && _texture->isFinishedLoading();
}

bool oo::SingleTextureMaterial::wantsNormalsAsTextureCoordinates() {
  return _texture != nullptr && _texture->isCubeMap();
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>>
oo::SingleTextureMaterial::allTextures() {
  std::vector<std::shared_ptr<oo::Texture>> result;
  result.emplace_back(_texture);
  return result;
}
#endif
