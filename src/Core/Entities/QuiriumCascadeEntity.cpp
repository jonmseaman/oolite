/*

OOQuiriumCascadeEntity.m


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

#include "QuiriumCascadeEntity.h"

#include <format>
#include <memory>
#include <string>
#include <vector>

#include "MacroOpenGL.h"
#include "PlayerEntity.h"
#include "Universe.h"

#define kQuiriumCascadeDuration (20.0)  // seconds
// seconds before we start colliding with and damaging things.
#define kQuiriumCollisionDelay (0.05)

bool oo::QuiriumCascadeEntity::initQuiriumCascadeFromShip(::ShipEntity* ship) {
  if (ship == nullptr) {
    return false;  // [self release]; return nil
  }

  // [super init] could not fail: the constructor ran Entity's -init body.
  {
    setPosition((ship != nullptr ? ship->getPosition() : HPVector{}));

    setStatus(oo::EntityStatus::kEffect);
    scanClass = oo::ScanClass::kMine;

    setOwner(ship != nullptr ? oo::LiveOwner(ship)
                             : nullptr);  // [ship owner]'s C++ entity

    // Red and green channels are animated.
    _color[2] = 1.0f;
    _color[3] = 0.5f;
  }

  return true;
}

std::shared_ptr<oo::QuiriumCascadeEntity>
oo::QuiriumCascadeEntity::quiriumCascadeFromShip(::ShipEntity* ship) {
  std::shared_ptr<oo::QuiriumCascadeEntity> cascade =
      oo::makeShared<oo::QuiriumCascadeEntity>();
  if (!cascade->initQuiriumCascadeFromShip(ship)) return nullptr;
  return cascade;
}

std::optional<std::string> oo::QuiriumCascadeEntity::descriptionComponents()
    const {
  return std::format("{:f} seconds passed of {:f}", _timePassed,
                     kQuiriumCascadeDuration);
}

void oo::QuiriumCascadeEntity::update(oo::TimeDelta delta_t) {
  Entity::update(delta_t);
  _timePassed += delta_t;

  rotMatrix = oo::MatrixForBillboard(
      position, (PLAYER != nullptr ? PLAYER->getPosition() : HPVector{}));

  GLfloat tf = static_cast<GLfloat>(_timePassed / kQuiriumCascadeDuration);
  GLfloat stf = tf * tf;
  GLfloat expansionSpeed = 0.0;
  if (_timePassed > 0) {
    // Avoid divide by 0
    expansionSpeed = fmin(240.0f + 10.0f / (tf * tf), 1000.0f);
  }

  collision_radius += static_cast<GLfloat>(delta_t * expansionSpeed);  // expand
  // adjusted to take into account delta_t
  energy = static_cast<GLfloat>(delta_t * (100000 - 90000 * tf));

  _color[3] = oo::Clamp_0_1_f(0.5f * ((0.025f / tf) + 1.0f - stf));

  _color[0] = _color[1] = fmin(1.0f - 5.0f * tf, 1.0f);
  if (_color[0] < 0.0f) {
    _color[0] = 0.25f * tf * randf();
    _color[1] = 0.0f;
  }

  // manage collisions
  Entity* owner = ownerEntity();
  // a snapshot (enumerating the live array while it changed raised)
  const std::vector<oo::EntityHold> colliding = collidingEntities;
  for (const oo::EntityHold& collidingEntity : colliding) {
    Entity* e = collidingEntity.get();
    // we're going to force the weapon id to be the qbomb key here, because at
    // this point the cascade entity isn't a ship any more and there's no link
    // back to the original.
    if (e != nullptr) e->takeEnergyDamage(energy, this, owner, "EQ_QC_MINE");
  }

  // expire after ttl
  if (_timePassed > kQuiriumCascadeDuration) {
    if (UNIVERSE != nullptr) UNIVERSE->removeEntity(this);
  }
}

void oo::QuiriumCascadeEntity::drawImmediate(bool /*immediate*/,
                                             bool translucent) {
  if (!translucent ||
      (UNIVERSE != nullptr ? UNIVERSE->breakPatternHide() : false))
    return;

  OO_ENTER_OPENGL();
  OOSetOpenGLState(oo::OpenGLStateID::kAdditiveBlending);

  OOGL(glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT));

  OOGL(glDisable(GL_CULL_FACE));
  OOGL(glDisable(GL_TEXTURE_2D));
  OOGL(glEnable(GL_BLEND));
  OOGL(glBlendFunc(GL_SRC_ALPHA, GL_ONE));

  OOGL(glColor4fv(_color));
  OOGLBEGIN(GL_TRIANGLE_FAN);
  GLDrawBallBillboard(collision_radius, 4, sqrt(cam_zero_distance));
  OOGLEND();

  OOGL(glPopAttrib());

  OOVerifyOpenGLState();
  oo::CheckOpenGLErrors([&]() -> std::string {
    return "OOQuiriumCascadeEntity after drawing " +
           oo::EntityDescription(this);
  });
}

bool oo::QuiriumCascadeEntity::isEffect() { return true; }

bool oo::QuiriumCascadeEntity::isCascadeWeapon() { return true; }

bool oo::QuiriumCascadeEntity::canCollide() {
  return _timePassed > kQuiriumCollisionDelay;
}

bool oo::QuiriumCascadeEntity::checkCloseCollisionWith(Entity* /*other*/) {
  return true;
}
