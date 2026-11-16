/*

OOCombinedEmissionMapGenerator.h

Generator which renders a single RGB emission map from some combination of
emission_map, emission, illumination_map, illumination_color and
emission_and_illumination_map parameters.


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

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Color.h"
#include "TextureGenerator.h"
#include "oofnd/PList.hpp"

/*	A leaf of the texture generators, over OOTextureGenerator. Its caller
   (OOMultiTextureMaterial) makes it with the factories below, and hands the C++
   generator to the texture.
*/
namespace oo {

class CombinedEmissionMapGenerator : public TextureGenerator {
 public:
  ~CombinedEmissionMapGenerator() override;  // was -dealloc

  /*	Were
     -initWithEmissionMapSpec:emissionColor:diffuseMap:diffuseColor:illuminationMapSpec:
          illuminationColor:optionsSpecifier: and
     -initWithEmissionAndIlluminationMapSpec:
          diffuseMap:diffuseColor:emissionColor:illuminationColor:optionsSpecifier:;
     null where they answered nil.
  */
  static std::shared_ptr<CombinedEmissionMapGenerator>
  generatorWithEmissionMapSpec(const oo::PList& emissionMapSpec,
                               oo::Color* emissionColor, Texture* diffuseMap,
                               oo::Color* diffuseColor,
                               const oo::PList& illuminationMapSpec,
                               oo::Color* illuminationColor,
                               const oo::PList& spec);
  static std::shared_ptr<CombinedEmissionMapGenerator>
  generatorWithEmissionAndIlluminationMapSpec(
      const oo::PList& emissionAndIlluminationMapSpec, Texture* diffuseMap,
      oo::Color* diffuseColor, oo::Color* emissionColor,
      oo::Color* illuminationColor, const oo::PList& spec);

#ifndef NDEBUG
  std::optional<std::string> descriptionComponents() const override;
#endif

  TextureFlags textureOptions() override;
  GLfloat anisotropy() override;
  GLfloat lodBias() override;
  std::optional<std::string> cacheKey() override;

  void loadTexture() override;

 private:
  bool initWithEmissionMapSpec(const oo::PList& emissionMapSpec,
                               oo::Color* emissionColor, Texture* diffuseMap,
                               oo::Color* diffuseColor,
                               const oo::PList& illuminationMapSpec,
                               oo::Color* illuminationColor, bool isCombinedMap,
                               const oo::PList& spec);

  std::string constructCacheKey();

  std::string _cacheKey;

  oo::PList _emissionSpec;
  oo::PList _illuminationSpec;
  std::shared_ptr<Texture> _diffuseMap;

  PixMap _emissionPx = {};
  PixMap _diffusePx = {};
  PixMap _illuminationPx = {};
  std::shared_ptr<oo::Color> _emissionColor;
  std::shared_ptr<oo::Color> _illuminationColor;
  bool _isCombinedMap = false;

  TextureFlags _textureOptions = TextureFlags::kNone;
  GLfloat _anisotropy = 0;
  GLfloat _lodBias = 0;

#ifndef NDEBUG
  std::string _emissionDesc;
  std::string _illuminationDesc;
  std::string _diffuseDesc;
#endif
};

}  // namespace oo
