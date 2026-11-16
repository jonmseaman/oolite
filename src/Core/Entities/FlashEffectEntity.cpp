/*

OOFlashEffectEntity.m


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

#include "FlashEffectEntity.h"

#include <memory>

#include "Color.h"
#include "GraphicsResetManager.h"
#include "PlayerEntity.h"
#include "Texture.h"
#include "Universe.h"

#define kLaserFlashDuration 0.3f
#define kExplosionFlashDuration 0.4f
#define kGrowthRateFactor \
  150.0f  // if average flashSize is 80 then this is 12000
#define kMinExplosionGrowth 600.0f
#define kLaserFlashInitialSize 1.0f
#define kExplosionFlashAlpha 0.5f

// The texture, made on first use and given up by a graphics reset. The holder
// is never deleted, so exit(0) frees no texture after the GL context has gone.
static std::shared_ptr<oo::Texture>& FlashTexture() {
  static std::shared_ptr<oo::Texture>& texture =
      *new std::shared_ptr<oo::Texture>();
  return texture;
}

namespace {

// The texture's graphics reset client (as OOLaserShotEntity's): registered
// once, with the texture, and never destroyed.
class FlashTextureResetClient : public oo::GraphicsResetClient {
 public:
  void resetGraphicsState() override {
    oo::FlashEffectEntity::resetGraphicsState();
  }
};

}  // namespace

void oo::FlashEffectEntity::initExplosionFlashWithPosition(HPVector pos,
                                                           Vector vel,
                                                           float size) {
  initWithPosition(pos, size, oo::Color::whiteColor().get(),
                   kExplosionFlashDuration);
  // [self initWithPosition:...] could not fail.
  {
    _growthRate = fmax(_growthRate, kMinExplosionGrowth);
    _alpha = kExplosionFlashAlpha;
    setVelocity(vel);
  }
}

void oo::FlashEffectEntity::initLaserFlashWithPosition(HPVector pos, Vector vel,
                                                       oo::Color* color) {
  initWithPosition(pos, kLaserFlashInitialSize, color, kLaserFlashDuration);
  // [self initWithPosition:...] could not fail.
  {
    setVelocity(vel);
    _alpha = 1.0f;
  }
}

// The C++ entity: the
// members -position, -velocity and -collisionRadius forwarded to; none answers
// zero, as nil did.
std::shared_ptr<oo::FlashEffectEntity>
oo::FlashEffectEntity::explosionFlashFromEntity(Entity* entity) {
  const std::shared_ptr<oo::FlashEffectEntity> flash =
      oo::makeShared<oo::FlashEffectEntity>();
  flash->initExplosionFlashWithPosition(
      entity != nullptr ? entity->getPosition() : kZeroHPVector,
      entity != nullptr ? entity->getVelocity() : kZeroVector,
      entity != nullptr ? entity->collisionRadius() : 0.0f);
  return flash;
}

std::shared_ptr<oo::FlashEffectEntity>
oo::FlashEffectEntity::laserFlashWithPosition(HPVector pos, Vector vel,
                                              oo::Color* color) {
  const std::shared_ptr<oo::FlashEffectEntity> flash =
      oo::makeShared<oo::FlashEffectEntity>();
  flash->initLaserFlashWithPosition(pos, vel, color);
  return flash;
}

void oo::FlashEffectEntity::initWithPosition(HPVector pos, float size,
                                             oo::Color* color, float duration) {
  oo::LightParticleEntity::initWithDiameter(size);
  // [super initWithDiameter:] could not fail.
  {
    setPosition(pos);
    _duration = duration;
    _growthRate = kGrowthRateFactor * size;
    setColor(color, 1.0f);
    assert(collisionRadius() == 0 && getEnergy() == 0 &&
           magnitude(getVelocity()) == 0);
  }
}

void oo::FlashEffectEntity::update(oo::TimeDelta delta_t) {
  oo::LightParticleEntity::update(delta_t);

  float tf = _duration * 0.667f;
  float tf1 = _duration - tf;

  // Scale up.
  _diameter += static_cast<float>(delta_t * _growthRate);

  // Fade in and out.
  oo::TimeDelta lifeTime = timeElapsedSinceSpawn();
  _colorComponents[3] = static_cast<GLfloat>(
      _alpha *
      ((lifeTime < tf) ? (lifeTime / tf) : (_duration - lifeTime) / tf1));

  // Disappear as necessary.
  if (lifeTime > _duration) {
    if (UNIVERSE != nullptr) UNIVERSE->removeEntity(this);
  }
}

oo::Texture* oo::FlashEffectEntity::texture() {
  if (FlashTexture() == nullptr) oo::FlashEffectEntity::setUpTexture();
  return FlashTexture().get();
}

void oo::FlashEffectEntity::setUpTexture() {
  if (FlashTexture() == nullptr) {
    FlashTexture() = (oo::Texture::textureWithName(
        "oolite-particle-flash.png", "Textures",
        oo::TextureFlags::kMinFilterMipMap |
            oo::TextureFlags::kMagFilterLinear | oo::TextureFlags::kAlphaMask,
        kOOTextureDefaultAnisotropy, 0.0));
    oo::GraphicsResetManager::sharedManager()->registerCxxClient(
        new FlashTextureResetClient);
  }
}

void oo::FlashEffectEntity::resetGraphicsState() { FlashTexture().reset(); }
