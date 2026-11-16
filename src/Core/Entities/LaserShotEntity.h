/*

OOLaserShotEntity.h

Entity subclass implementing GIANT SPACE LAZORS.


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
#include <string>

#include "Maths.h"
#include "ShipEntity.h"
#include "Texture.h"
#include "Types.h"

struct OOLaserShotEntityTestAccess;

namespace oo {

class LaserShotEntity : public Entity {
 public:
  // A new shot, initialised.
  static std::shared_ptr<LaserShotEntity> laserFromShip(
      ::ShipEntity* ship, oo::WeaponFacing direction, Vector offset);

  std::optional<std::string> descriptionComponents() const override;

  void setColor(oo::Color* color);

  void setRange(GLfloat range);

  void update(oo::TimeDelta delta_t) override;
  void drawImmediate(bool immediate, bool translucent) override;
  bool isEffect() override;
  bool canCollide() override;

  oo::Texture* texture1();
  oo::Texture* texture2();

  static void setUpTexture();
  static oo::Texture* innerTexture();
  static oo::Texture* outerTexture();
  // The graphics reset client is a file-local C++ one (OOLaserShotEntity.cpp).
  static void resetGraphicsState();

 private:
  friend struct ::OOLaserShotEntityTestAccess;

  // -initLaserFromShip:direction:offset:'s body, run once right after
  // construction.
  void initLaserFromShip(::ShipEntity* srcEntity, oo::WeaponFacing direction,
                         Vector offset);

  GLfloat _color[4] = {};
  oo::TimeDelta _lifetime = {};
  GLfloat _range = {};
  Vector _offset = {};
  Quaternion _relOrientation = {};
};

}  // namespace oo
