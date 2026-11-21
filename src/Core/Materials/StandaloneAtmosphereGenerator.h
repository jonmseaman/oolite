/*

OOStandaloneAtmosphereGenerator.h

Generator for planet atmospheres when the planet is using a
non-generated diffuse map.


Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Maths.h"
#include "TextureGenerator.h"
#include "oofnd/PList.hpp"

namespace oo {

struct StandaloneAtmosphereGeneratorInfo {
  RANROTSeed seed;

  unsigned width;
  unsigned height;

  // Atmosphere parameters.
  float cloudAlpha;
  float cloudFraction;
  FloatRGB airColor;
  FloatRGB cloudColor;
  FloatRGB paleCloudColor;

  // Noise generation stuff.
  std::vector<float> fbmBuffer;

  std::vector<uint16_t> permutations;

  unsigned planetAspectRatio;
  unsigned planetScaleOffset;
  bool perlin3d;
};

/*	A leaf of the texture generators, over OOTextureGenerator. Its one
   caller (OOPlanetEntity) calls the static members; the texture takes the C++
   generator.
*/
class StandaloneAtmosphereGenerator : public TextureGenerator {
 public:
  /*	planetInfo is the planet's material parameters, a mixed configuration:
     plist values with the colours as PList::Object nodes holding OOColors
     Was [[OOStandaloneAtmosphereGenerator alloc] initWithPlanetInfo:seed:];
     null where that answered nil.
  */
  static std::shared_ptr<StandaloneAtmosphereGenerator> generatorWithPlanetInfo(
      const oo::PList& planetInfo, RANROTSeed seed);
  bool initWithPlanetInfo(const oo::PList& planetInfo,
                          RANROTSeed seed);  // false where it answered nil

  // Were +planetTextureWithInfo:seed: and
  // +generateAtmosphereTexture:withInfo:seed:; the texture is answered
  // retained.
  static std::shared_ptr<Texture> planetTextureWithInfo(
      const oo::PList& planetInfo, RANROTSeed seed);
  static bool generateAtmosphereTexture(std::shared_ptr<Texture>* texture,
                                        const oo::PList& planetInfo,
                                        RANROTSeed seed);

  std::optional<std::string> descriptionComponents() const override;
  TextureFlags textureOptions() override;
  std::optional<std::string> cacheKey() override;

  /*	Was -getResult:format:width:height:, whose keywords are not the root's
          -getResult:format:originalWidth:originalHeight:, so it never overrode
     it and nothing sends it; kept, under a name that does not override
     getResult() either.
  */
  std::optional<Result> getResultFormatWidthHeight();

  void loadTexture() override;

 private:
  // defined where the .cpp's DEBUG_DUMP_RAW is set
  void dumpNoiseBuffer(float* noise);

  StandaloneAtmosphereGeneratorInfo _info = {};
  unsigned _planetScale = {};
};

}  // namespace oo
