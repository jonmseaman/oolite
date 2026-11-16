/*

OOMultiTextureMaterial.h

A material that uses multitexturing and texture combiners.


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

#include <memory>
#include <string>
#include <vector>

#include "OOOpenGLExtensionManager.h"

#if OO_MULTITEXTURE

#include "BasicMaterial.h"

namespace oo {

class Texture;

class MultiTextureMaterial : public BasicMaterial {
 public:
  /*	A new material, initialised by initWithName() (below); null where it
     failed (it answered nil: no texture combiners). It was
     [[OOMultiTextureMaterial alloc] initWithName:configuration:].
  */
  static std::shared_ptr<MultiTextureMaterial> materialWithName(
      const std::optional<std::string>& name, const oo::PList& configuration);

  // Runs once, right after construction; false on failure.
  bool initWithName(
      const std::optional<std::string>& name,
      const oo::PList& configuration);  // shared with OOBasicMaterial

  std::size_t textureUnitCount();

  std::optional<std::string> descriptionComponents() const override;
  std::size_t countOfTextureUnitsWithBaseCoordinates() override;
  void ensureFinishedLoading() override;
  void apply() override;
  void unapplyWithNext(Material* next) override;

  size_t objectSize() const override { return sizeof *this; }
#ifdef OO_DEBUG_MONITOR_ENABLED
  std::vector<std::shared_ptr<Texture>> allTextures() override;
#endif

 private:
  std::shared_ptr<Texture> _diffuseMap = {};
  std::shared_ptr<Texture> _emissionMap = {};

  std::size_t _unitsUsed = {};
};

}  // namespace oo

#endif /* OO_MULTITEXTURE */
