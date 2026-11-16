/*

OOMultiTextureMaterial.m


Copyright (C) 2010-2013 Jens Ayton

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

#include "MultiTextureMaterial.h"

#include <memory>
#include <string>
#include <vector>

#include "CombinedEmissionMapGenerator.h"
#include "MaterialSpecifier.h"
#include "OOMacroOpenGL.h"
#include "OOOpenGLExtensionManager.h"
#include "Texture.h"
#include "oofnd/objc/Assert.h"

#if OO_MULTITEXTURE

std::shared_ptr<oo::MultiTextureMaterial>
oo::MultiTextureMaterial::materialWithName(
    const std::optional<std::string>& name, const oo::PList& configuration) {
  std::shared_ptr<oo::MultiTextureMaterial> result =
      oo::makeShared<oo::MultiTextureMaterial>();
  if (!result->initWithName(name, configuration)) return nullptr;
  return result;
}

bool oo::MultiTextureMaterial::initWithName(
    const std::optional<std::string>& name, const oo::PList& configuration) {
  if (!OOOpenGLExtensionManager::sharedManager()->textureCombinersSupported()) {
    return false;  // [self release]; return nil: the factory drops the object
  }

  // The configuration mixes plist data with live objects (colours): an
  // oo::PList carries both exactly.
  const oo::PList& config = configuration;
  const oo::PList diffuseSpec = oo::MaterialDiffuseMapSpecifier(config, name);
  const oo::PList emissionSpec = oo::MaterialEmissionMapSpecifier(config);
  const oo::PList illuminationSpec =
      oo::MaterialIlluminationMapSpecifier(config);
  const oo::PList emissionAndIlluminationSpec =
      oo::MaterialEmissionAndIlluminationMapSpecifier(config);
  std::shared_ptr<OOColor> diffuseColor = oo::MaterialDiffuseColor(config);
  std::shared_ptr<OOColor> emissionColor;
  std::shared_ptr<OOColor> illuminationColor =
      oo::MaterialIlluminationModulateColor(config);

  // A copy of the configuration (an empty one for nil, as
  // +dictionaryWithDictionary: gave).
  oo::PList mutableConfiguration =
      config.isDict() ? config : oo::PList(oo::PList::Dict{});

  if (!emissionSpec.isNull() || !emissionAndIlluminationSpec.isNull()) {
    emissionColor = oo::MaterialEmissionModulateColor(config);

    /*	If an emission map and an emission colour are both specified, stop
            the superclass (OOBasicMaterial) from applying the emission colour.
    */
    mutableConfiguration.getIf<oo::PList::Dict>()->erase(
        oo::kMaterialEmissionColorName);
    mutableConfiguration.getIf<oo::PList::Dict>()->erase(
        oo::kMaterialEmissionColorLegacyName);
  }

  oo::BasicMaterial::initWithName(name, mutableConfiguration);
  {  // was if (self != nil): the superclass's initialiser cannot fail
    if (!diffuseSpec.isNull()) {
      _diffuseMap = oo::Texture::textureWithConfiguration(diffuseSpec);
      if (_diffuseMap.get() != nullptr) _unitsUsed++;
    }

    // Check for simplest cases, where we don't need to bake a derived emission
    // map.
    if (!emissionSpec.isNull() && illuminationSpec.isNull() &&
        emissionAndIlluminationSpec.isNull() && emissionColor == nullptr) {
      _emissionMap = oo::Texture::textureWithConfiguration(
          emissionSpec, oo::TextureFlags::kExtraShrink);
      if (_emissionMap.get() != nullptr) _unitsUsed++;
    } else {
      std::shared_ptr<oo::CombinedEmissionMapGenerator> generator;

      if (!emissionAndIlluminationSpec.isNull()) {
        generator = oo::CombinedEmissionMapGenerator::
            generatorWithEmissionAndIlluminationMapSpec(
                emissionAndIlluminationSpec, _diffuseMap.get(),
                diffuseColor.get(), emissionColor.get(),
                illuminationColor.get(), emissionAndIlluminationSpec);
      } else {
        const oo::PList optionsSpec =
            !emissionSpec.isNull() ? emissionSpec : illuminationSpec;
        generator =
            oo::CombinedEmissionMapGenerator::generatorWithEmissionMapSpec(
                emissionSpec, emissionColor.get(), _diffuseMap.get(),
                diffuseColor.get(), illuminationSpec, illuminationColor.get(),
                optionsSpec);
      }

      _emissionMap = oo::Texture::textureWithGenerator(generator.get());
      if (_emissionMap.get() != nullptr) _unitsUsed++;
    }
  }

  return true;
}

std::optional<std::string> oo::MultiTextureMaterial::descriptionComponents()
    const {
  std::vector<std::string> bits;
  if (_diffuseMap.get())
    bits.push_back("diffuse map: " + _diffuseMap->shortDescription());
  if (_emissionMap.get())
    bits.push_back("emission map: " + _emissionMap->shortDescription());

  std::optional<std::string> result =
      oo::BasicMaterial::descriptionComponents();
  if (!bits.empty()) {
    std::string joined;
    for (const std::string& bit : bits)
      joined += (joined.empty() ? "" : ",") + bit;
    result = result.value_or("") + " - " + joined;  // StdString(nil) was ""
  }
  return result;
}

std::size_t oo::MultiTextureMaterial::textureUnitCount() { return _unitsUsed; }

std::size_t oo::MultiTextureMaterial::countOfTextureUnitsWithBaseCoordinates() {
  return _unitsUsed;
}

void oo::MultiTextureMaterial::ensureFinishedLoading() {
  if (_diffuseMap != nullptr) _diffuseMap->ensureFinishedLoading();
  if (_emissionMap != nullptr) _emissionMap->ensureFinishedLoading();
}

void oo::MultiTextureMaterial::apply() {
  OO_ENTER_OPENGL();

  oo::BasicMaterial::apply();

  GLenum textureUnit = GL_TEXTURE0_ARB;

  if (_diffuseMap.get() != nullptr) {
    OOGL(glActiveTextureARB(textureUnit++));
    OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB));
    OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE));
    if (_diffuseMap != nullptr) _diffuseMap->apply();
  }

  if (_emissionMap.get() != nullptr) {
    OOGL(glActiveTextureARB(textureUnit++));
    OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB));
    OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD));
    if (_emissionMap != nullptr) _emissionMap->apply();
  }

  OOCAssert(textureUnit - GL_TEXTURE0_ARB == _unitsUsed,
            "OOMultiTextureMaterial texture unit count invalid (expected %zu, "
            "actually using %u)",
            _unitsUsed, textureUnit - GL_TEXTURE0_ARB);

  if (textureUnit > GL_TEXTURE1_ARB) {
    OOGL(glActiveTextureARB(GL_TEXTURE0_ARB));
  }
}

void oo::MultiTextureMaterial::unapplyWithNext(oo::Material* next) {
  OO_ENTER_OPENGL();

  oo::BasicMaterial::unapplyWithNext(next);

  // -isKindOfClass: (nil is not one)
  std::size_t i;
  oo::MultiTextureMaterial* nextMulti =
      dynamic_cast<oo::MultiTextureMaterial*>(next);
  i = (nextMulti != nullptr) ? nextMulti->textureUnitCount() : 0;
  for (; i != _unitsUsed; ++i) {
    OOGL(glActiveTextureARB(static_cast<GLenum>(GL_TEXTURE0_ARB + i)));
    oo::Texture::applyNone();
    OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));
  }
  OOGL(glActiveTextureARB(GL_TEXTURE0_ARB));
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>>
oo::MultiTextureMaterial::allTextures() {
  std::vector<std::shared_ptr<oo::Texture>> result;
  if (_diffuseMap.get() == nullptr) {
    result.emplace_back(_emissionMap);
    return result;
  }
  result.emplace_back(_diffuseMap);
  result.emplace_back(_emissionMap);
  return result;
}
#endif

#endif /* OO_MULTITEXTURE */
