/*

OOHPVector.m

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

#include <format>
#include <string>
#include <vector>

#include "Maths.h"
#include "legacy_random.h"  // randf
#include "oofnd/Log.hpp"

const HPVector kZeroHPVector = {0.0, 0.0, 0.0};
const HPVector kBasisXHPVector = {1.0, 0.0, 0.0};
const HPVector kBasisYHPVector = {0.0, 1.0, 0.0};
const HPVector kBasisZHPVector = {0.0, 0.0, 1.0};

const HPVector2D kZeroHPVector2D = {0.0, 0.0};
const HPVector2D kBasisXHPVector2D = {1.0, 0.0};
const HPVector2D kBasisYHPVector2D = {0.0, 1.0};

std::string HPVectorDescription(HPVector vector) {
  return std::format("({:g}, {:g}, {:g})", vector.x, vector.y, vector.z);
}

std::vector<double> ArrayFromHPVector(HPVector vec) {
  return {vec.x, vec.y, vec.z};
}

/*	This generates random vectors distrubuted evenly over the surface of the
        unit sphere. It does this the simple way, by generating vectors in the
        half-unit cube and rejecting those outside the half-unit sphere (and the
        zero vector), then normalizing the result. (Half-unit measures are used
        to avoid unnecessary multiplications of randf() values.)

        In principle, using three normally-distributed co-ordinates (and again
        normalizing the result) would provide the right result without looping,
   but I don't trust bellf() so I'll go with the simple approach for now.
*/
namespace oo {

HPVector RandomUnitHPVector() {
  HPVector v;
  HPScalar m;

  do {
    // one draw per local, in the order Clang evaluates the arguments
    float x = randf() - 0.5f;
    float y = randf() - 0.5f;
    float z = randf() - 0.5f;
    v = make_HPvector(x, y, z);
    m = HPmagnitude2(v);
    // We're confining to a sphere of radius 0.5 using the sqared magnitude; 0.5
    // squared is 0.25.
  } while (m > 0.25 || m == 0.0);

  return HPvector_normal(v);
}

HPVector HPVectorRandomSpatial(HPScalar maxLength) {
  HPVector v;
  HPScalar m;

  do {
    // one draw per local, in the order Clang evaluates the arguments
    float x = randf() - 0.5f;
    float y = randf() - 0.5f;
    float z = randf() - 0.5f;
    v = make_HPvector(x, y, z);
    m = HPmagnitude2(v);
    // We're confining to a sphere of radius 0.5 using the sqared magnitude; 0.5
    // squared is 0.25.
  } while (m > 0.25);

  // 2.0 is to compensate for the 0.5-radius sphere.
  return HPvector_multiply_scalar(v, maxLength * 2.0);
}

HPVector HPVectorRandomRadial(HPScalar maxLength) {
  // one draw per local, in the order Clang evaluates the arguments
  HPVector direction = RandomUnitHPVector();
  HPScalar length = randf() * maxLength;
  return HPvector_multiply_scalar(direction, length);
}

HPVector HPRandomPositionInBoundingBox(BoundingBox bb) {
  HPVector result;
  result.x = static_cast<HPScalar>(bb.min.x + randf() * (bb.max.x - bb.min.x));
  result.y = static_cast<HPScalar>(bb.min.y + randf() * (bb.max.y - bb.min.y));
  result.z = static_cast<HPScalar>(bb.min.z + randf() * (bb.max.z - bb.min.z));
  return result;
}

HPVector RandomPositionInCylinder(HPVector centre1, HPScalar exclusion1,
                                  HPVector centre2, HPScalar exclusion2,
                                  HPScalar radius) {
  HPScalar exc12 = exclusion1 * exclusion1;
  HPScalar exc22 = exclusion2 * exclusion2;
  if (HPdistance(centre1, centre2) < (exclusion1 + exclusion2) * 1.2) {
    OO_LOG("position.cylinder.error",
           "Trying to generate cylinder position in range {:f} long with "
           "exclusions {:f} and {:f}",
           HPdistance(centre1, centre2), exclusion1, exclusion2);
  }
  HPVector result;
  do {
    // one draw per local, in the order Clang evaluates the arguments
    HPVector along = HPVectorInterpolate(centre1, centre2, randf());
    HPVector offset = HPVectorRandomSpatial(radius);
    result = HPvector_add(along, offset);
  } while (HPdistance2(result, centre1) < exc12 ||
           HPdistance2(result, centre2) < exc22);
  return result;
}

HPVector RandomPositionInShell(HPVector centre, HPScalar inner,
                               HPScalar outer) {
  HPVector result;
  HPScalar inner2 = inner * inner;
  do {
    result = HPvector_add(centre, HPVectorRandomSpatial(outer));
  } while (HPdistance2(result, centre) < inner2);
  return result;
}

HPVector ProjectHPVectorToPlane(HPVector point, HPVector plane,
                                HPVector normal) {
  return HPvector_subtract(
      point,
      HPvector_multiply_scalar(
          normal, HPdot_product(HPvector_subtract(point, plane), normal)));
}

}  // namespace oo
