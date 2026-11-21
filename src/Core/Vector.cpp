/*

OOVector.m

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

#include "Maths.h"
#include "legacy_random.h"  // randf
#include "oofnd/String.hpp"

const Vector kZeroVector = {0.0f, 0.0f, 0.0f};
const Vector kBasisXVector = {1.0f, 0.0f, 0.0f};
const Vector kBasisYVector = {0.0f, 1.0f, 0.0f};
const Vector kBasisZVector = {0.0f, 0.0f, 1.0f};

const Vector2D kZeroVector2D = {0.0f, 0.0f};
const Vector2D kBasisXVector2D = {1.0f, 0.0f};
const Vector2D kBasisYVector2D = {0.0f, 1.0f};

std::string VectorDescription(Vector vector) {
  return std::format("({:g}, {:g}, {:g})", vector.x, vector.y, vector.z);
}

oo::NativeVector::NativeVector(Vector vect) { v = vect; }

Vector oo::NativeVector::getVector() { return v; }

std::string oo::NativeVector::className() const { return "OONativeVector"; }

std::string oo::NativeVector::description() const {
  return std::format("<OONativeVector {}>", oo::str::pointerDescription(this));
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

Vector RandomUnitVector() {
  Vector v;
  float m;

  do {
    // one draw per local, in the order Clang evaluates the arguments
    float x = randf() - 0.5f;
    float y = randf() - 0.5f;
    float z = randf() - 0.5f;
    v = make_vector(x, y, z);
    m = magnitude2(v);
    // We're confining to a sphere of radius 0.5 using the sqared magnitude; 0.5
    // squared is 0.25.
  } while (m > 0.25f || m == 0.0f);

  return vector_normal(v);
}

Vector VectorRandomSpatial(Scalar maxLength) {
  Vector v;
  float m;

  do {
    // one draw per local, in the order Clang evaluates the arguments
    float x = randf() - 0.5f;
    float y = randf() - 0.5f;
    float z = randf() - 0.5f;
    v = make_vector(x, y, z);
    m = magnitude2(v);
    // We're confining to a sphere of radius 0.5 using the sqared magnitude; 0.5
    // squared is 0.25.
  } while (m > 0.25f);

  // 2.0 is to compensate for the 0.5-radius sphere.
  return vector_multiply_scalar(v, maxLength * 2.0f);
}

Vector VectorRandomRadial(Scalar maxLength) {
  // one draw per local, in the order Clang evaluates the arguments
  Vector direction = RandomUnitVector();
  Scalar length = randf() * maxLength;
  return vector_multiply_scalar(direction, length);
}

Vector RandomPositionInBoundingBox(BoundingBox bb) {
  Vector result;
  result.x = bb.min.x + randf() * (bb.max.x - bb.min.x);
  result.y = bb.min.y + randf() * (bb.max.y - bb.min.y);
  result.z = bb.min.z + randf() * (bb.max.z - bb.min.z);
  return result;
}

}  // namespace oo

// only need high precision versions of these
/*Vector OORandomPositionInCylinder(Vector centre1, OOScalar exclusion1, Vector
centre2, OOScalar exclusion2, OOScalar radius)
{
        OOScalar exc12 = exclusion1*exclusion1;
        OOScalar exc22 = exclusion2*exclusion2;
        Vector result;
        do
        {
                result =
vector_add(OOVectorInterpolate(centre1,centre2,randf()),OOVectorRandomSpatial(radius));
        }
        while(distance2(result,centre1)<exc12 ||
distance2(result,centre2)<exc22); return result;
}

Vector OORandomPositionInShell(Vector centre, OOScalar inner, OOScalar outer)
{
        Vector result;
        OOScalar inner2 = inner*inner;
        do
        {
                result = vector_add(centre,OOVectorRandomSpatial(outer));
        } while(distance2(result,centre)<inner2);
        return result;
        }*/
