/*

OODrawable.m


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

#include "Drawable.h"

#include <format>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "Demangle.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/String.hpp"

void oo::Drawable::renderOpaqueParts() {}

void oo::Drawable::renderTranslucentParts() {}

bool oo::Drawable::hasOpaqueParts() { return false; }

bool oo::Drawable::hasTranslucentParts() { return false; }

GLfloat oo::Drawable::collisionRadius() { return 0.0f; }

GLfloat oo::Drawable::maxDrawDistance() { return 0.0f; }

BoundingBox oo::Drawable::boundingBox() { return kZeroBoundingBox; }

void oo::Drawable::setBindingTarget(Entity* /*target*/) {}

void oo::Drawable::dumpSelfState() {}

std::optional<std::string> oo::Drawable::descriptionComponents() const {
  return std::nullopt;
}

// The C++ class's name and the drawable's own address, with the description
// components if any.
std::string oo::Drawable::description() const {
  const std::string name = oo::DescriptionClassName(typeid(*this).name());
  std::string result =
      std::format("<{} {}>", name, oo::str::pointerDescription(this));
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::Drawable::allTextures() {
  return {};
}

// The object's own size.
size_t oo::Drawable::totalSize() { return objectSize(); }
#endif

/*	OODrawableAutorelease(): the drawable goes to the innermost
   oo::AutoreleaseScope, whose drain releases it where the keeper object that
   held it was released.
*/
namespace oo {

void DrawableAutorelease(std::shared_ptr<Drawable> drawable) {
  if (drawable == nullptr) return;
  oo::autorelease(std::move(drawable));
}

}  // namespace oo
