/*

OOPlasmaBurstEntity.m


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

#include "PlasmaBurstEntity.h"

#include <memory>

#include "Universe.h"

#define kPlasmaBurstInitialSize 64.0f
#define kPlasmaBurstGrowthRate 64.0f
#define kPlasmaBurstDuration 2.0f

std::shared_ptr<oo::PlasmaBurstEntity> oo::PlasmaBurstEntity::burstWithPosition(
    HPVector inPosition) {
  const std::shared_ptr<oo::PlasmaBurstEntity> burst =
      oo::makeShared<oo::PlasmaBurstEntity>();
  burst->initWithPosition(inPosition);
  return burst;
}

void oo::PlasmaBurstEntity::initWithPosition(HPVector inPosition) {
  oo::LightParticleEntity::initWithDiameter(kPlasmaBurstInitialSize);
  // [super initWithDiameter:] could not fail.
  {
    setPosition(inPosition);
    setCollisionRadius(2.0);

    setColor(oo::Color::redColor().get());
  }
}

void oo::PlasmaBurstEntity::update(oo::TimeDelta delta_t) {
  oo::LightParticleEntity::update(delta_t);

  oo::TimeDelta lifeTime = timeElapsedSinceSpawn();
  float attenuation = oo::Clamp_0_1_f(
      static_cast<float>(1.0f - lifeTime / kPlasmaBurstDuration));

  _diameter = static_cast<float>(kPlasmaBurstInitialSize +
                                 lifeTime * kPlasmaBurstGrowthRate);

  _colorComponents[3] = attenuation;

  if (lifeTime > kPlasmaBurstDuration) {
    if (UNIVERSE != nullptr) UNIVERSE->removeEntity(this);
  }
}
