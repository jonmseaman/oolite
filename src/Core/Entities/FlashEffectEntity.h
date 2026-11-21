/*

OOFlashEffectEntity.h

Flashes during explosions and laser hits - not to be confused with flashers.


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

#include "LightParticleEntity.h"

/*	The ship and the universe call the factories.
 */
namespace oo {

class FlashEffectEntity : public LightParticleEntity {
 public:
  // +explosionFlashFromEntity: and +laserFlashWithPosition:velocity:color:: a
  // new flash, initialised.
  // the C++ entity
  static std::shared_ptr<FlashEffectEntity> explosionFlashFromEntity(
      Entity* entity);
  static std::shared_ptr<FlashEffectEntity> laserFlashWithPosition(
      HPVector position, Vector vel, oo::Color* color);

  static void setUpTexture();
  // Called by the texture's file-local graphics reset client.
  static void resetGraphicsState();

  void update(oo::TimeDelta delta_t) override;
  oo::Texture* texture() override;

 private:
  // The initialisers' bodies, run once right after construction.
  void initExplosionFlashWithPosition(HPVector pos, Vector vel, float size);
  void initLaserFlashWithPosition(HPVector pos, Vector vel, oo::Color* color);
  // Designated initializer.
  void initWithPosition(HPVector pos, float size, oo::Color* color,
                        float duration);

  float _duration = {};
  float _growthRate = {};
  float _alpha = {};
};

}  // namespace oo
