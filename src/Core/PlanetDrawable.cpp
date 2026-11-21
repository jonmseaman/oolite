/*

 OOPlanetDrawable.m

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

#include "PlanetDrawable.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "MacroOpenGL.h"
#include "MyOpenGLView.h"
#include "OpenGL.h"
#include "PlanetData.h"
#include "SingleTextureMaterial.h"
#include "StellarBody.h"
#include "Texture.h"  // allTextures() destroys std::shared_ptr<OOTexture>s
#include "Universe.h"
#include "oofnd/Log.hpp"

#ifdef OO_DEBUG_MONITOR_ENABLED
#include "DebugFlags.h"
#include "DebugGLDrawing.h"
#include "Entity.h"
#endif

constexpr auto LOD_GRANULARITY = (static_cast<float>(kOOPlanetDataLevels - 1));
constexpr auto LOD_FACTOR = (1.0 / 4.0);

std::shared_ptr<oo::PlanetDrawable> oo::PlanetDrawable::planetWithTextureName(
    const std::string& textureName, float radius) {
  std::shared_ptr<oo::PlanetDrawable> result =
      oo::makeShared<oo::PlanetDrawable>();
  result->setTextureName(textureName);
  result->setRadius(radius);

  return result;
}

std::shared_ptr<oo::PlanetDrawable> oo::PlanetDrawable::atmosphereWithRadius(
    float radius) {
  std::shared_ptr<oo::PlanetDrawable> result = initAsAtmosphere();
  result->setRadius(radius);

  return result;
}

oo::PlanetDrawable::PlanetDrawable() {
  {  // [super init] could not fail.
    _radius = 1.0f;
    recalculateTransform();
    setLevelOfDetail(0.5f);
  }
}

std::shared_ptr<oo::PlanetDrawable> oo::PlanetDrawable::initAsAtmosphere() {
  std::shared_ptr<oo::PlanetDrawable> self =
      oo::makeShared<oo::PlanetDrawable>();
  {  // -init could not fail.
    self->_isAtmosphere = true;
  }

  return self;
}

// -dealloc's DESTROY(_material) is the member's destructor.

std::shared_ptr<oo::PlanetDrawable> oo::PlanetDrawable::copy() {
  // [[[self class] allocWithZone:zone] init]: the class has no subclass
  std::shared_ptr<oo::PlanetDrawable> copy =
      oo::makeShared<oo::PlanetDrawable>();
  copy->setMaterial(material());
  copy->_isAtmosphere = _isAtmosphere;
  copy->_radius = _radius;
  copy->_transform = _transform;
  copy->_lod = _lod;

  return copy;
}

oo::Material* oo::PlanetDrawable::material() { return _material.get(); }

void oo::PlanetDrawable::setMaterial(oo::Material* material) {
  oo::MaterialAutorelease(std::move(_material));  // [_material autorelease]
  _material = oo::share(material);
}

std::optional<std::string> oo::PlanetDrawable::textureName() {
  oo::Material* material = _material.get();
  return material != nullptr ? material->name() : std::nullopt;
}

void oo::PlanetDrawable::setTextureName(const std::string& textureName) {
  if (this->textureName() != textureName) {
    // {diffuse_map={repeat_s=yes;cube_map=yes};}, as the old-style property
    // list parsed.
    const oo::PList spec(oo::PList::Dict{
        {"diffuse_map",
         oo::PList(oo::PList::Dict{{"repeat_s", oo::PList("yes")},
                                   {"cube_map", oo::PList("yes")}})}});
    // [_material release], then [[OOSingleTextureMaterial alloc]
    // initWithName:configuration:]: the factory's material (null for none).
    _material = oo::SingleTextureMaterial::materialWithName(textureName, spec);
  }
}

float oo::PlanetDrawable::radius() { return _radius; }

void oo::PlanetDrawable::setRadius(float radius) {
  _radius = fabsf(radius);
  recalculateTransform();
}

float oo::PlanetDrawable::levelOfDetail() {
  return static_cast<float>(_lod) / LOD_GRANULARITY;
}

void oo::PlanetDrawable::setLevelOfDetail(float lod) {
  _lod =
      static_cast<unsigned int>(roundf(oo::Clamp_0_1_f(lod) * LOD_GRANULARITY));
}

void oo::PlanetDrawable::calculateLevelOfDetailForViewDistance(float distance) {
  bool simple = (UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false);
  float drawFactor = static_cast<float>(
      (UNIVERSE != nullptr && UNIVERSE->getGameView() != nullptr
           ? UNIVERSE->getGameView()->getViewSize()
           : NSSize{})
          .width /
      (simple ? 100.0 : 40.0));
  // proportional to size on screen in pixels
  float drawRatio2 = drawFactor * _radius / sqrtf(distance);

  float lod = sqrtf(static_cast<float>(drawRatio2 * LOD_FACTOR));
  if (simple) {
    lod -= 0.5f / LOD_GRANULARITY;  // Make LOD transitions earlier.
    // Don't use highest LOD.
    lod = oo::Clamp_0_max_f(lod, (LOD_GRANULARITY - 1) / LOD_GRANULARITY);
  }
  setLevelOfDetail(lod);
}

void oo::PlanetDrawable::renderOpaqueParts() {
  assert(_lod < kOOPlanetDataLevels);

  OOSetOpenGLState(oo::OpenGLStateID::kOpaque);

  renderCommonParts();

  OOVerifyOpenGLState();
}

void oo::PlanetDrawable::renderTranslucentParts() {
  assert(_lod < kOOPlanetDataLevels);

  // yes, opaque - necessary changes made later
  OOSetOpenGLState(oo::OpenGLStateID::kOpaque);

  renderCommonParts();

  OOVerifyOpenGLState();
}

void oo::PlanetDrawable::renderTranslucentPartsOnOpaquePass() {
  assert(_lod < kOOPlanetDataLevels);

  OO_ENTER_OPENGL();

  // yes, opaque - necessary changes made later
  OOSetOpenGLState(oo::OpenGLStateID::kOpaque);

  OOGL(glDisable(GL_DEPTH_TEST));
  renderCommonParts();
  OOGL(glEnable(GL_DEPTH_TEST));

  OOVerifyOpenGLState();
}

void oo::PlanetDrawable::renderCommonParts() {
  const oo::PlanetDataLevel* data = &kPlanetData[_lod];
  // messages to nil did nothing and answered NO
  oo::Material* material = _material.get();

  OO_ENTER_OPENGL();

  OOGL(glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT));
  OOGL(glShadeModel(GL_SMOOTH));

  if (_isAtmosphere) {
    OOGL(glEnable(GL_BLEND));
    OOGL(glDepthMask(GL_FALSE));
  } else {
    OOGL(glDisable(GL_BLEND));
  }

  // Scale the ball.
  oo::GLPushModelView();
  oo::GLMultModelView(_transform);

  if (material != nullptr) material->apply();

  OOGL(glEnable(GL_LIGHTING));
  OOGL(glEnable(GL_TEXTURE_2D));

#if OO_TEXTURE_CUBE_MAP
  if (material != nullptr && material->wantsNormalsAsTextureCoordinates()) {
    OOGL(glDisable(GL_TEXTURE_2D));
    OOGL(glEnable(GL_TEXTURE_CUBE_MAP));
  }
#endif

  OOGL(glDisableClientState(GL_COLOR_ARRAY));

  OOGL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));

  OOGL(glVertexPointer(3, GL_FLOAT, 0, oo::kPlanetVertices));
  if (material != nullptr && material->wantsNormalsAsTextureCoordinates()) {
    OOGL(glTexCoordPointer(3, GL_FLOAT, 0, oo::kPlanetVertices));
  } else {
    OOGL(glTexCoordPointer(2, GL_FLOAT, 0, oo::kPlanetTexCoords));
  }

  // FIXME: instead of GL_RESCALE_NORMAL, consider copying and transforming the
  // vertex array for each planet.
  OOGL(glEnable(GL_RESCALE_NORMAL));
  OOGL(glNormalPointer(GL_FLOAT, 0, oo::kPlanetVertices));

  OOGL(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(data->faceCount * 3),
                      data->type, data->indices));

#ifdef OO_DEBUG_MONITOR_ENABLED
  if ((UNIVERSE != nullptr ? UNIVERSE->getWireframeGraphics() : false)) {
    OODebugDrawBasisAtOrigin(1.5);
  }
#endif

#if OO_TEXTURE_CUBE_MAP
  if (material != nullptr && material->wantsNormalsAsTextureCoordinates()) {
    OOGL(glEnable(GL_TEXTURE_2D));
    OOGL(glDisable(GL_TEXTURE_CUBE_MAP));
  }
#endif

  oo::GLPopModelView();
#ifdef OO_DEBUG_MONITOR_ENABLED
  if (oo::DebugFlagIsOn(oo::DebugFlags::kDrawNormals)) debugDrawNormals();
#endif

  oo::Material::applyNone();
  OOGL(glPopAttrib());

  OOGL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
}

bool oo::PlanetDrawable::hasOpaqueParts() { return !_isAtmosphere; }

bool oo::PlanetDrawable::hasTranslucentParts() { return _isAtmosphere; }

GLfloat oo::PlanetDrawable::collisionRadius() { return _radius; }

GLfloat oo::PlanetDrawable::maxDrawDistance() {
  // FIXME
  return INFINITY;
}

BoundingBox oo::PlanetDrawable::boundingBox() {
  return BoundingBox{{-_radius, -_radius, -_radius},
                     {_radius, _radius, _radius}};
}

void oo::PlanetDrawable::setBindingTarget(Entity* target) {
  if (oo::Material* material = _material.get())
    material->setBindingTarget(target);
}

void oo::PlanetDrawable::dumpSelfState() {
  oo::Drawable::dumpSelfState();
  OO_LOG("dumpState.planetDrawable", "radius: {:g}", radius());
  OO_LOG("dumpState.planetDrawable", "LOD: {:g}", levelOfDetail());
}

void oo::PlanetDrawable::recalculateTransform() {
  _transform = oo::MatrixForScaleUniform(_radius);
}

#ifdef OO_DEBUG_MONITOR_ENABLED

void oo::PlanetDrawable::debugDrawNormals() {
  oo::DebugWFState state;

  OO_ENTER_OPENGL();

  state = oo::DebugBeginWireframe(false);

  const oo::PlanetDataLevel* data = &kPlanetData[_lod];
  unsigned i;

  OOGLBEGIN(GL_LINES);
  for (i = 0; i < data->vertexCount; i++) {
    /*	Fun sphere facts: the normalized coordinates of a point on a sphere at
       the origin is equal to the object-space normal of the surface at that
       point. Furthermore, we can construct the binormal (a vector pointing
       westward along the surface) as the cross product of the normal with the Y
       axis. (This produces singularities at the pole, but there have to be
       singularities according to the Hairy Ball Theorem.) The tangent (a vector
       north along the surface) is then the inverse of the cross product of the
       normal and binormal.

            (This comment courtesy of the in-development planet shader.)
    */
    Vector v =
        make_vector(oo::kPlanetVertices[i * 3], oo::kPlanetVertices[i * 3 + 1],
                    oo::kPlanetVertices[i * 3 + 2]);
    Vector n = v;
    v = oo::VectorMultiplyMatrix(v, _transform);

    glColor3f(0.0f, 1.0f, 1.0f);
    GLVertexOOVector(v);
    GLVertexOOVector(vector_add(
        v, vector_multiply_scalar(n, static_cast<oo::Scalar>(_radius * 0.05))));

    Vector b = cross_product(n, kBasisYVector);
    Vector t = vector_flip(true_cross_product(n, b));

    glColor3f(1.0f, 1.0f, 0.0f);
    GLVertexOOVector(v);
    GLVertexOOVector(vector_add(
        v, vector_multiply_scalar(t, static_cast<oo::Scalar>(_radius * 0.03))));

    glColor3f(0.0f, 1.0f, 0.0f);
    GLVertexOOVector(v);
    GLVertexOOVector(vector_add(
        v, vector_multiply_scalar(b, static_cast<oo::Scalar>(_radius * 0.03))));
  }
  OOGLEND();

  oo::DebugEndWireframe(state);
}
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::PlanetDrawable::allTextures() {
  oo::Material* material = this->material();
  return material != nullptr ? material->allTextures()
                             : std::vector<std::shared_ptr<oo::Texture>>();
}

#endif
