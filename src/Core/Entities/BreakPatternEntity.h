/*

OOBreakPatternEntity.h

Entity implementing tunnel effect for hyperspace and stations.

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

#include "Entity.h"

namespace oo {

inline constexpr int kBreakPatternMaxSides = 128;
inline constexpr int kBreakPatternMaxVertices = (kBreakPatternMaxSides + 1) * 2;

}  // namespace oo

inline constexpr double BREAK_PATTERN_RING_SPACING = 50.0;
inline constexpr double BREAK_PATTERN_RING_SPEED = 200.0;

struct OOBreakPatternEntityTestAccess;

namespace oo {

class BreakPatternEntity : public Entity {
 public:
  // +breakPatternWithPolygonSides:startAngle:aspectRatio:: a new ring,
  // initialised.
  static std::shared_ptr<BreakPatternEntity> breakPatternWithPolygonSides(
      std::size_t sides, float startAngleDegrees, float aspectRatio);

  void setInnerColor(oo::Color* color1, oo::Color* color2);

  void setLifetime(double lifetime);

  void update(oo::TimeDelta delta_t) override;
  void drawImmediate(bool immediate, bool translucent) override;
  bool canCollide() override;

 private:
  friend struct ::OOBreakPatternEntityTestAccess;

  // -initWithPolygonSides:startAngle:aspectRatio:'s body, run once right after
  // construction.
  void initWithPolygonSides(std::size_t sides, float startAngleDegrees,
                            float aspectRatio);

  void setInnerColorComponents(GLfloat color1[4], GLfloat color2[4]);

  Vector _vertexPosition[kBreakPatternMaxVertices] = {};
  GLfloat _vertexColor[kBreakPatternMaxVertices][4] = {};
  std::size_t _vertexCount = {};
  double _lifetime = {};
};

}  // namespace oo
