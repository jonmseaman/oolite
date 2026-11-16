/*

OOMaterial.m


Copyright (C) 2007-2013 Jens Ayton and contributors

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

#include "Material.h"

#include <format>
#include <memory>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "Demangle.h"
#include "Logging.h"
#include "MaterialState.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/String.hpp"

namespace {

/*	The current material, retained. Never destroyed, as the static pointer
   was not.
*/
std::shared_ptr<oo::Material>& ActiveMaterial() {
  static auto* active = new std::shared_ptr<oo::Material>;
  return *active;
}

}  // namespace

void oo::Material::setUp() {
  // I thought we'd need this, but the stuff I needed it for turned out to be
  // problematic. Maybe in future. -- Ahruman
}

// name() is a subclass's override, so it cannot be const.
std::optional<std::string> oo::Material::descriptionComponents() const {
  return "\"" + const_cast<oo::Material*>(this)->name().value_or("(null)") +
         "\"";  // "%@" of the name, quoted
}

std::string oo::Material::description() const {
  const std::string name = oo::DescriptionClassName(typeid(*this).name());
  std::string result =
      std::format("<{} {}>", name, oo::str::pointerDescription(this));
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}

std::optional<std::string> oo::Material::name() {
  OOLogGenericParameterError();
  return std::nullopt;
}

// Make this the current GL shader program.
void oo::Material::apply() {
  if (oo::Material* active = ActiveMaterial().get())
    active->unapplyWithNext(this);
  ActiveMaterial() = nullptr;

  if (doApply()) {
    ActiveMaterial() = oo::share(this);
  }
}

void oo::Material::applyNone() {
  if (oo::Material* active = ActiveMaterial().get())
    active->unapplyWithNext(nullptr);
  ActiveMaterial() = nullptr;
}

std::shared_ptr<oo::Material> oo::Material::current() {
  return ActiveMaterial();
}

// OOMaterialState.h: the slot for plain C++ translation units.
namespace oo {

std::shared_ptr<Material> MaterialRetainCurrent() {
  return Material::current();
}

void MaterialApplyNone() { Material::applyNone(); }

void MaterialApplyAndRelease(std::shared_ptr<Material> material) {
  if (material != nullptr) material->apply();  // a message to nil did nothing
}

}  // namespace oo

void oo::Material::ensureFinishedLoading() {}

bool oo::Material::isFinishedLoading() { return true; }

void oo::Material::setBindingTarget(Entity* /*target*/) {}

bool oo::Material::wantsNormalsAsTextureCoordinates() { return false; }

#if OO_MULTITEXTURE
std::size_t oo::Material::countOfTextureUnitsWithBaseCoordinates() { return 1; }
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::Material::allTextures() {
  return {};
}
#endif

bool oo::Material::doApply() {
  OOLogGenericSubclassResponsibility();
  return false;
}

void oo::Material::unapplyWithNext(oo::Material* /*next*/) {
  // Do nothing.
}

void oo::Material::willDealloc() {
  if (ActiveMaterial().get() == this) [[unlikely]] {
    OO_LOG("shader.dealloc.imbalance", "{}",
           "***** Material deallocated while active, indicating a "
           "retain/release imbalance.");
    unapplyWithNext(nullptr);
    // = nil, not released: it is being deallocated. The reference is moved
    // into a holder that is never deleted.
    (void)new std::shared_ptr<oo::Material>(std::move(ActiveMaterial()));
  }
}

/*	OOMaterialAutorelease(): the material goes to the innermost
   oo::AutoreleaseScope, whose drain releases it where the keeper object that
   held it was released.
*/
namespace oo {

void MaterialAutorelease(std::shared_ptr<Material> material) {
  if (material == nullptr) return;
  oo::autorelease(std::move(material));
}

}  // namespace oo
