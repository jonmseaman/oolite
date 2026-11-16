/*

OOLightParticleEntity.m


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

#include "LightParticleEntity.h"

#include <memory>
#include <vector>

#include "Color.h"
#include "GraphicsResetManager.h"
#include "MacroOpenGL.h"
#include "MyOpenGLView.h"
#include "PlayerEntity.h"
#include "Texture.h"
#include "Universe.h"

constexpr double PARTICLE_DISTANCE_SCALE_LOW = 12.0;
constexpr double PARTICLE_DISTANCE_SCALE_HIGH = 36.0;

// The texture, made on first use and given up by a graphics reset. The holder
// is never deleted, so exit(0) frees no texture after the GL context has gone.
static std::shared_ptr<oo::Texture>& BlobTexture() {
  static std::shared_ptr<oo::Texture>& texture =
      *new std::shared_ptr<oo::Texture>();
  return texture;
}

namespace {

// The texture's graphics reset client (as OOLaserShotEntity's): registered
// once, with the texture, and never destroyed.
class BlobTextureResetClient : public oo::GraphicsResetClient {
 public:
  void resetGraphicsState() override {
    oo::LightParticleEntity::resetGraphicsState();
  }
};

}  // namespace

void oo::LightParticleEntity::initWithDiameter(float diameter) {
  // [super init] could not fail: the constructor ran Entity's -init body.
  {
    _diameter = diameter;
    no_draw_distance =
        static_cast<GLfloat>(pow(diameter / 2.0, M_SQRT2) *
                             NO_DRAW_DISTANCE_FACTOR * NO_DRAW_DISTANCE_FACTOR);
    no_draw_distance *= static_cast<GLfloat>(
        (UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false)
            ? PARTICLE_DISTANCE_SCALE_LOW
            : PARTICLE_DISTANCE_SCALE_HIGH);

    _colorComponents[0] = 1.0f;
    _colorComponents[1] = 1.0f;
    _colorComponents[2] = 1.0f;
    _colorComponents[3] = 1.0f;

    setScanClass(oo::ScanClass::kNoDraw);
    setStatus(oo::EntityStatus::kEffect);
  }
}

float oo::LightParticleEntity::diameter() { return _diameter; }

void oo::LightParticleEntity::setDiameter(float diameter) {
  _diameter = diameter;
}

void oo::LightParticleEntity::setColor(oo::Color* color) {
  // A message to a nil colour did nothing.
  if (color != nullptr)
    color->getRed(&_colorComponents[0], &_colorComponents[1],
                  &_colorComponents[2], &_colorComponents[3]);
}

void oo::LightParticleEntity::setColor(oo::Color* color, GLfloat alpha) {
  setColor(color);
  _colorComponents[3] = alpha;
}

void oo::LightParticleEntity::drawSubEntityImmediate(bool immediate,
                                                     bool translucent) {
  if (!translucent) return;

  /*	TODO: someone will inevitably build a ship so big that individual
          zero_distances are necessary for flashers, if they haven't already.
          -- Ahruman 2009-09-20
  */
  Entity* ownerPart = oo::LiveOwner(this);  // -owner's C++ entity
  cam_zero_distance = ownerPart != nullptr
                          ? static_cast<GLfloat>(ownerPart->camZeroDistance())
                          : 0;  // a message to nil answered 0
  if (no_draw_distance <= cam_zero_distance) return;

  Entity* father = ownerPart;
  Entity* last = nullptr;
  HPVector abspos = position;

  while (father != nullptr && father != last) {
    // && father != (Entity *)NO_TARGET, which is nil:
    // NO_TARGET is 0
    oo::Matrix rM = father->drawRotationMatrix();
    abspos = HPvector_add(oo::HPVectorMultiplyMatrix(abspos, rM),
                          father->getPosition());
    last = father;

    if (!oo::EntityIsSubEntity(father)) break;
    father = oo::LiveOwner(father);  // [father owner]
  }

  oo::Matrix temp_matrix = oo::GLPopModelView();
  oo::GLPushModelView();

  /* Flashers are drawn using the absolute view matrix */
  oo::GLLoadModelView(
      (UNIVERSE != nullptr ? UNIVERSE->getViewMatrix() : oo::Matrix{}));
  /* ...modified by the aggregate translation calculated above */
  oo::GLTranslateModelView(HPVectorToVector(HPvector_subtract(
      abspos,
      (PLAYER != nullptr ? PLAYER->viewpointPosition()
                         : HPVector{}))));  // move to camera-relative position
  drawImmediate(immediate, translucent);

  oo::GLLoadModelView(temp_matrix);
}

void oo::LightParticleEntity::drawImmediate(bool /*immediate*/,
                                            bool translucent) {
  if (!translucent) return;
  if ((UNIVERSE != nullptr ? UNIVERSE->breakPatternHide() : false) &&
      !getIsImmuneToBreakPatternHide()) {
    Entity* father = oo::LiveOwner(this);  // -owner's C++ entity
    while (father != nullptr) {
      // && father != (Entity *)NO_TARGET, which is
      // nil: NO_TARGET is 0
      if (!oo::EntityIsSubEntity(father)) break;
      father = oo::LiveOwner(father);  // [father owner]
    }
    if (!(father != nullptr && father->getIsImmuneToBreakPatternHide())) {
      // nil answered NO
      return;
    }
  }
  if (no_draw_distance <= cam_zero_distance) return;

  OO_ENTER_OPENGL();
  OOSetOpenGLState(oo::OpenGLStateID::kAdditiveBlending);

  OOGL(glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT));
  OOGL(glEnable(GL_BLEND));
  OOGL(glBlendFunc(GL_SRC_ALPHA, GL_ONE));

  OOGL(glEnable(GL_TEXTURE_2D));
  OOGL(glDepthMask(GL_FALSE));

  GLfloat distanceAttenuation = cam_zero_distance / no_draw_distance;
  distanceAttenuation = static_cast<GLfloat>(1.0 - distanceAttenuation);
  GLfloat components[4] = {_colorComponents[0], _colorComponents[1],
                           _colorComponents[2],
                           _colorComponents[3] * distanceAttenuation};
  OOGL(glColor4fv(components));

  OOGL(glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, components));
  OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND));

  oo::ViewID viewDir =
      (UNIVERSE != nullptr ? UNIVERSE->getViewDirection() : oo::ViewID{});
  if (viewDir != oo::ViewID::kGUIDisplay)
    oo::GLMultModelView(
        (PLAYER != nullptr ? PLAYER->drawRotationMatrix() : oo::Matrix{}));
  if (oo::Texture* blob = texture())
    blob->apply();  // a message to nil did nothing

  /*	NOTE: nominal diameter is actual radius, because of the black border
          in the texture. However, the offset along the view axis is not
          affected by the border and needs to be scaled.
          -- Ahruman 2009-12-20
  */
  float viewOffset = _diameter * 0.5f;

  OOGLBEGIN(GL_QUADS);
  switch (viewDir) {
    case oo::ViewID::kForward:
    case oo::ViewID::kGUIDisplay:
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-_diameter, -_diameter, -viewOffset);

      glTexCoord2f(1.0, 1.0);
      glVertex3f(_diameter, -_diameter, -viewOffset);

      glTexCoord2f(1.0, 0.0);
      glVertex3f(_diameter, _diameter, -viewOffset);

      glTexCoord2f(0.0, 0.0);
      glVertex3f(-_diameter, _diameter, -viewOffset);
      break;

    case oo::ViewID::kAft:
      glTexCoord2f(0.0, 1.0);
      glVertex3f(_diameter, -_diameter, viewOffset);

      glTexCoord2f(1.0, 1.0);
      glVertex3f(-_diameter, -_diameter, viewOffset);

      glTexCoord2f(1.0, 0.0);
      glVertex3f(-_diameter, _diameter, viewOffset);

      glTexCoord2f(0.0, 0.0);
      glVertex3f(_diameter, _diameter, viewOffset);
      break;

    case oo::ViewID::kStarboard:
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-viewOffset, -_diameter, _diameter);

      glTexCoord2f(1.0, 1.0);
      glVertex3f(-viewOffset, -_diameter, -_diameter);

      glTexCoord2f(1.0, 0.0);
      glVertex3f(-viewOffset, _diameter, -_diameter);

      glTexCoord2f(0.0, 0.0);
      glVertex3f(-viewOffset, _diameter, _diameter);
      break;

    case oo::ViewID::kPort:
      glTexCoord2f(0.0, 1.0);
      glVertex3f(viewOffset, -_diameter, -_diameter);

      glTexCoord2f(1.0, 1.0);
      glVertex3f(viewOffset, -_diameter, _diameter);

      glTexCoord2f(1.0, 0.0);
      glVertex3f(viewOffset, _diameter, _diameter);

      glTexCoord2f(0.0, 0.0);
      glVertex3f(viewOffset, _diameter, -_diameter);
      break;

    case oo::ViewID::kCustom: {
      ::PlayerEntity* player = PLAYER;
      Vector vi =
          (player != nullptr ? player->getCustomViewRightVector() : Vector{});
      vi.x *= _diameter;
      vi.y *= _diameter;
      vi.z *= _diameter;
      Vector vj =
          (player != nullptr ? player->getCustomViewUpVector() : Vector{});
      vj.x *= _diameter;
      vj.y *= _diameter;
      vj.z *= _diameter;
      Vector vk =
          (player != nullptr ? player->getCustomViewForwardVector() : Vector{});
      vk.x *= viewOffset;
      vk.y *= viewOffset;
      vk.z *= viewOffset;
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-vi.x - vj.x - vk.x, -vi.y - vj.y - vk.y, -vi.z - vj.z - vk.z);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(+vi.x - vj.x - vk.x, +vi.y - vj.y - vk.y, +vi.z - vj.z - vk.z);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(+vi.x + vj.x - vk.x, +vi.y + vj.y - vk.y, +vi.z + vj.z - vk.z);
      glTexCoord2f(0.0, 0.0);
      glVertex3f(-vi.x + vj.x - vk.x, -vi.y + vj.y - vk.y, -vi.z + vj.z - vk.z);
    } break;

    default:
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-_diameter, -_diameter, -_diameter);

      glTexCoord2f(1.0, 1.0);
      glVertex3f(_diameter, -_diameter, -_diameter);

      glTexCoord2f(1.0, 0.0);
      glVertex3f(_diameter, _diameter, -_diameter);

      glTexCoord2f(0.0, 0.0);
      glVertex3f(-_diameter, _diameter, -_diameter);
      break;
  }
  OOGLEND();

  OOGL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));
  OOGL(glPopAttrib());

  OOVerifyOpenGLState();
}

oo::Texture* oo::LightParticleEntity::texture() {
  return oo::LightParticleEntity::defaultParticleTexture();
}

void oo::LightParticleEntity::setUpTexture() {
  if (BlobTexture() == nullptr) {
    BlobTexture() = (oo::Texture::textureWithName(
        "oolite-particle-blur.png", "Textures",
        oo::TextureFlags::kMinFilterMipMap |
            oo::TextureFlags::kMagFilterLinear | oo::TextureFlags::kAlphaMask,
        kOOTextureDefaultAnisotropy / 2.0, 0.0));
    oo::GraphicsResetManager::sharedManager()->registerCxxClient(
        new BlobTextureResetClient);
  }
}

oo::Texture* oo::LightParticleEntity::defaultParticleTexture() {
  if (BlobTexture() == nullptr) setUpTexture();
  return BlobTexture().get();
}

void oo::LightParticleEntity::resetGraphicsState() { BlobTexture().reset(); }

bool oo::LightParticleEntity::isEffect() { return true; }

bool oo::LightParticleEntity::canCollide() { return false; }

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>>
oo::LightParticleEntity::allTextures() {
  std::vector<std::shared_ptr<oo::Texture>> result;
  result.emplace_back(oo::share(texture()));
  return result;
}
#endif
