/*

OOEntityWithDrawable.m

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

#include "EntityWithDrawable.h"

#include <utility>
#include <vector>

#include "Drawable.h"
#include "Texture.h"  // allTextures() destroys std::shared_ptr<OOTexture>s
#include "Universe.h"

// The drawable is a member, released with the entity.

oo::Drawable* oo::EntityWithDrawable::getDrawable() { return drawable.get(); }

void oo::EntityWithDrawable::setDrawable(oo::Drawable* inDrawable) {
  if (inDrawable != drawable.get()) {
    // [drawable autorelease]: the old one lives until the pool drains
    oo::DrawableAutorelease(std::move(drawable));
    drawable = oo::share(inDrawable);
    // Messages to a nil drawable did nothing and answered 0.
    oo::Drawable* cxxDrawable = drawable.get();
    if (cxxDrawable != nullptr)
      cxxDrawable->setBindingTarget(this);  // the C++ entity

    collision_radius =
        cxxDrawable != nullptr ? cxxDrawable->collisionRadius() : 0.0f;
    no_draw_distance =
        cxxDrawable != nullptr ? cxxDrawable->maxDrawDistance() : 0.0f;
    boundingBox =
        cxxDrawable != nullptr ? cxxDrawable->boundingBox() : kZeroBoundingBox;
  }
}

double oo::EntityWithDrawable::findCollisionRadius() {
  oo::Drawable* cxxDrawable = drawable.get();
  return cxxDrawable != nullptr ? cxxDrawable->collisionRadius() : 0.0f;
}

void oo::EntityWithDrawable::drawImmediate(bool /*immediate*/,
                                           bool translucent) {
  if (no_draw_distance < cam_zero_distance) {
    // Don't draw.
    return;
  }

  if (no_draw_distance != INFINITY && !getIsImmuneToBreakPatternHide()) {
    // (always draw sky, always draw break patterns)
    if (!getIsSubEntity()) {
      GLfloat clipradius = collision_radius;
      if (getIsShip()) {
        clipradius = frustumRadius();  // [oo::ToShip(self) frustumRadius]
      } else if (getIsVisualEffect()) {
        // [(OOVisualEffectEntity *)self frustumRadius]
        clipradius = frustumRadius();
      }
      // don't bother with frustum culling within/near collision radius, as
      // potential for problems with floating point inaccuracy causing
      // unwanted disappearance maybe fix
      // http://aegidian.org/bb/viewtopic.php?f=3&t=13619 - CIM
      if (cam_zero_distance > (clipradius + 1000) * (clipradius + 1000)) {
        if (!(UNIVERSE != nullptr ? UNIVERSE->viewFrustumIntersectsSphereAt(
                                        cameraRelativePosition, clipradius)
                                  : false)) {
          return;
        }
      }
    } else {
      // is subentity
      // don't bother with frustum culling within 1km, as above - CIM
      if (cam_zero_distance >
          (collision_radius + 1000) * (collision_radius + 1000)) {
        // check correct sub-entity position
        if (!(UNIVERSE != nullptr
                  ? UNIVERSE->viewFrustumIntersectsSphereAt(
                        cameraRelativePosition, collisionRadius())
                  : false)) {
          return;
        }
      }
    }
  }

  if ((UNIVERSE != nullptr ? UNIVERSE->getWireframeGraphics() : false))
    oo::GLWireframeModeOn();

  oo::Drawable* cxxDrawable = drawable.get();
  if (cxxDrawable != nullptr) {
    if (translucent)
      cxxDrawable->renderTranslucentParts();
    else
      cxxDrawable->renderOpaqueParts();
  }

  if ((UNIVERSE != nullptr ? UNIVERSE->getWireframeGraphics() : false))
    oo::GLWireframeModeOff();
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>>
oo::EntityWithDrawable::allTextures() {
  oo::Drawable* cxxDrawable = getDrawable();
  return cxxDrawable != nullptr ? cxxDrawable->allTextures()
                                : std::vector<std::shared_ptr<oo::Texture>>();
}
#endif
