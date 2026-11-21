/*

OOBasicMaterial.m


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

#include "BasicMaterial.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <typeinfo>
#include <vector>

#include "MaterialSpecifier.h"
#include "Texture.h"
#include "Universe.h"
#include "oofnd/PList.hpp"

constexpr auto FACE = GL_FRONT_AND_BACK;

namespace {

// Made on first use and never released.
oo::BasicMaterial* sDefaultMaterial = nullptr;

}  // namespace

std::shared_ptr<oo::BasicMaterial> oo::BasicMaterial::materialWithName(
    const std::optional<std::string>& name) {
  std::shared_ptr<oo::BasicMaterial> result =
      oo::makeShared<oo::BasicMaterial>();
  result->initWithName(name);
  return result;
}

std::shared_ptr<oo::BasicMaterial> oo::BasicMaterial::materialWithName(
    const std::optional<std::string>& name, const oo::PList& configuration) {
  std::shared_ptr<oo::BasicMaterial> result =
      oo::makeShared<oo::BasicMaterial>();
  result->initWithName(name, configuration);
  return result;
}

void oo::BasicMaterial::initWithName(const std::optional<std::string>& name) {
  materialName = name;

  setDiffuseRed(1.0f, 1.0f, 1.0f, 1.0f);
  setAmbientRed(1.0f, 1.0f, 1.0f, 1.0f);
  specular[3] = 1.0;
  emission[3] = 1.0;
}

void oo::BasicMaterial::initWithName(const std::optional<std::string>& name,
                                     const oo::PList& configuration) {
  std::shared_ptr<oo::Color> colorDesc;
  int specularExponent;

  initWithName(name);

  // An empty dictionary, not nil: the specifier defaults (a specular exponent
  // of 10) apply. The configuration mixes plist data with live objects
  // (colours): an oo::PList carries both exactly.
  const oo::PList config =
      !configuration.isNull() ? configuration : oo::PList(oo::PList::Dict{});

  // The specifier answers the colour; OOColor::colorWithDescription() of its
  // Object node is the colour itself.
  colorDesc = oo::MaterialDiffuseColor(config);
  if (colorDesc != nullptr) setDiffuseColor(colorDesc.get());

  colorDesc = oo::MaterialAmbientColor(config);
  if (colorDesc != nullptr)
    setAmbientColor(colorDesc.get());
  else
    setAmbientColor(diffuseColor().get());

  colorDesc = oo::MaterialEmissionColor(config);
  if (colorDesc != nullptr) setEmissionColor(colorDesc.get());

  specularExponent = oo::MaterialSpecularExponent(config);
  if (specularExponent != 0 && permitSpecular()) {
    colorDesc = oo::MaterialSpecularColor(config);
    setShininess(static_cast<uint8_t>(specularExponent));
    if (colorDesc != nullptr) setSpecularColor(colorDesc.get());
  }
}

std::optional<std::string> oo::BasicMaterial::name() { return materialName; }

bool oo::BasicMaterial::doApply() {
  OOGL(glMaterialfv(FACE, GL_DIFFUSE, diffuse));
  OOGL(glMaterialfv(FACE, GL_SPECULAR, specular));
  OOGL(glMaterialfv(FACE, GL_AMBIENT, ambient));
  OOGL(glMaterialfv(FACE, GL_EMISSION, emission));
  OOGL(glMateriali(FACE, GL_SHININESS, _shininess));
  // Exactly this class, not a subclass
  if (typeid(*this) == typeid(oo::BasicMaterial)) {
    oo::Texture::applyNone();
  }

  return true;
}

void oo::BasicMaterial::unapplyWithNext(oo::Material* next) {
  // This class or a subclass (null is neither)
  if (dynamic_cast<oo::BasicMaterial*>(next) == nullptr) {
    if (sDefaultMaterial == nullptr) [[unlikely]] {
      // Never released: a heap std::shared_ptr that is never deleted.
      // never deleted
      static std::shared_ptr<oo::BasicMaterial>* sDefaultMaterialHolder =
          nullptr;
      sDefaultMaterialHolder = new std::shared_ptr<oo::BasicMaterial>(
          materialWithName(std::string("<default material>")));
      sDefaultMaterial = sDefaultMaterialHolder->get();
    }
    sDefaultMaterial->doApply();
  }
}

std::shared_ptr<oo::Color> oo::BasicMaterial::diffuseColor() {
  return oo::Color::colorWithRed(diffuse[0], diffuse[1], diffuse[2],
                                 diffuse[3]);
}

void oo::BasicMaterial::setDiffuseColor(oo::Color* color) {
  if (color != nullptr) {
    setDiffuseRed(color->redComponent(), color->greenComponent(),
                  color->blueComponent(), color->alphaComponent());
  }
}

void oo::BasicMaterial::setAmbientAndDiffuseColor(oo::Color* color) {
  setAmbientColor(color);
  setDiffuseColor(color);
}

std::shared_ptr<oo::Color> oo::BasicMaterial::specularColor() {
  return oo::Color::colorWithRed(specular[0], specular[1], specular[2],
                                 specular[3]);
}

void oo::BasicMaterial::setSpecularColor(oo::Color* color) {
  if (color != nullptr) {
    setSpecularRed(color->redComponent(), color->greenComponent(),
                   color->blueComponent(), color->alphaComponent());
  }
}

std::shared_ptr<oo::Color> oo::BasicMaterial::ambientColor() {
  return oo::Color::colorWithRed(ambient[0], ambient[1], ambient[2],
                                 ambient[3]);
}

void oo::BasicMaterial::setAmbientColor(oo::Color* color) {
  if (color != nullptr) {
    setAmbientRed(color->redComponent(), color->greenComponent(),
                  color->blueComponent(), color->alphaComponent());
  }
}

std::shared_ptr<oo::Color> oo::BasicMaterial::emmisionColor() {
  return oo::Color::colorWithRed(emission[0], emission[1], emission[2],
                                 emission[3]);
}

void oo::BasicMaterial::setEmissionColor(oo::Color* color) {
  if (color != nullptr) {
    setEmissionRed(color->redComponent(), color->greenComponent(),
                   color->blueComponent(), color->alphaComponent());
  }
}

namespace {

// A colour's four components, from one array to another.
void CopyComponents(const GLfloat from[4], GLfloat to[4]) {
  std::copy_n(from, 4, to);
}

}  // namespace

void oo::BasicMaterial::getDiffuseComponents(GLfloat outComponents[4]) {
  CopyComponents(diffuse, outComponents);
}

void oo::BasicMaterial::setDiffuseComponents(const GLfloat components[4]) {
  CopyComponents(components, diffuse);
}

void oo::BasicMaterial::setAmbientAndDiffuseComponents(
    const GLfloat components[4]) {
  setAmbientComponents(components);
  setDiffuseComponents(components);
}

void oo::BasicMaterial::getSpecularComponents(GLfloat outComponents[4]) {
  CopyComponents(specular, outComponents);
}

void oo::BasicMaterial::setSpecularComponents(const GLfloat components[4]) {
  CopyComponents(components, specular);
}

void oo::BasicMaterial::getAmbientComponents(GLfloat outComponents[4]) {
  CopyComponents(ambient, outComponents);
}

void oo::BasicMaterial::setAmbientComponents(const GLfloat components[4]) {
  CopyComponents(components, ambient);
}

void oo::BasicMaterial::getEmissionComponents(GLfloat outComponents[4]) {
  CopyComponents(emission, outComponents);
}

void oo::BasicMaterial::setEmissionComponents(const GLfloat components[4]) {
  CopyComponents(components, emission);
}

void oo::BasicMaterial::setDiffuseRed(GLfloat r, GLfloat g, GLfloat b,
                                      GLfloat a) {
  diffuse[0] = r;
  diffuse[1] = g;
  diffuse[2] = b;
  diffuse[3] = a;
}

void oo::BasicMaterial::setAmbientAndDiffuseRed(GLfloat r, GLfloat g, GLfloat b,
                                                GLfloat a) {
  setAmbientRed(r, g, b, a);
  setDiffuseRed(r, g, b, a);
}

void oo::BasicMaterial::setSpecularRed(GLfloat r, GLfloat g, GLfloat b,
                                       GLfloat a) {
  specular[0] = r;
  specular[1] = g;
  specular[2] = b;
  specular[3] = a;
}

void oo::BasicMaterial::setAmbientRed(GLfloat r, GLfloat g, GLfloat b,
                                      GLfloat a) {
  ambient[0] = r;
  ambient[1] = g;
  ambient[2] = b;
  ambient[3] = a;
}

void oo::BasicMaterial::setEmissionRed(GLfloat r, GLfloat g, GLfloat b,
                                       GLfloat a) {
  emission[0] = r;
  emission[1] = g;
  emission[2] = b;
  emission[3] = a;
}

uint8_t oo::BasicMaterial::shininess() { return _shininess; }

void oo::BasicMaterial::setShininess(uint8_t value) {
  _shininess = std::min<uint8_t>(value, 128);
}

bool oo::BasicMaterial::permitSpecular() {
  return !(UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false);
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::BasicMaterial::allTextures() {
  return {};
}
#endif
