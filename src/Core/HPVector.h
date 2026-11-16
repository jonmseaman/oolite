/*

OOHPVector.h

Mathematical framework for Oolite.
High-precision vectors for world-space coordinates

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

#include <cmath>

#include "MathsBase.h"

// Each maths header includes what it uses, so OOMaths.h may list them in any
// order.
#include "FastArithmetic.h"
#include "Vector.h"

#ifndef OOMATHS_EXTERNAL_VECTOR_TYPES

struct HPVector {
  oo::HPScalar x;
  oo::HPScalar y;
  oo::HPScalar z;
};

struct HPVector2D {
  oo::HPScalar x;
  oo::HPScalar y;
};

#endif

extern const HPVector kZeroHPVector, /* 0, 0, 0 */
    kBasisXHPVector,                 /* 1, 0, 0 */
    kBasisYHPVector,                 /* 0, 1, 0 */
    kBasisZHPVector;                 /* 0, 0, 1 */

extern const HPVector2D kZeroHPVector2D, /* 0, 0 */
    kBasisXHPVector2D,                   /* 1, 0 */
    kBasisYHPVector2D;                   /* 0, 1 */

/* Construct vector */
inline HPVector make_HPvector(oo::HPScalar vx, oo::HPScalar vy,
                              oo::HPScalar vz) INLINE_CONST_FUNC;
inline HPVector2D MakeHPVector2D(oo::HPScalar vx,
                                 oo::HPScalar vy) INLINE_CONST_FUNC;

inline HPVector vectorToHPVector(Vector v) INLINE_CONST_FUNC;
inline Vector HPVectorToVector(HPVector v) INLINE_CONST_FUNC;

/* Generate random vectors. */
namespace oo {

HPVector RandomUnitHPVector();
HPVector HPVectorRandomSpatial(
    HPScalar
        maxLength);  // Random vector uniformly distributed in radius-maxLength
                     // sphere. (Longer vectors are more common.)
HPVector HPVectorRandomRadial(
    HPScalar maxLength);  // Random vector with uniform distribution of
                          // direction and radius in radius-maxLength sphere.
                          // (Causes clustering at centre.)
HPVector RandomPositionInCylinder(HPVector centre1, HPScalar exclusion1,
                                  HPVector centre2, HPScalar exclusion2,
                                  HPScalar radius);
HPVector RandomPositionInShell(HPVector centre, HPScalar inner, HPScalar outer);
/* returns the projection of 'point' to the plane defined by the point
         'plane' and the normal vector 'normal' */
HPVector ProjectHPVectorToPlane(HPVector point, HPVector plane,
                                HPVector normal);

}  // namespace oo

/* Multiply vector by scalar (in place) */
inline void HPscale_vector(HPVector* outHPVector,
                           oo::HPScalar factor) ALWAYS_INLINE_FUNC NONNULL_FUNC;

/* Multiply vector by scalar */
inline HPVector HPvector_multiply_scalar(HPVector v,
                                         oo::HPScalar s) INLINE_CONST_FUNC;

/* Addition and subtraction of vectors */
inline HPVector HPvector_add(HPVector a, HPVector b) INLINE_CONST_FUNC;
inline HPVector HPvector_subtract(HPVector a, HPVector b) INLINE_CONST_FUNC;
#define HPvector_between(a, b) HPvector_subtract(b, a)
inline HPVector HPvector_flip(HPVector v) INLINE_CONST_FUNC;

/* HPVector linear interpolation */
namespace oo {

inline HPVector HPVectorInterpolate(HPVector a, HPVector b,
                                    HPScalar where) INLINE_CONST_FUNC;
inline HPVector HPVectorTowards(HPVector a, HPVector b,
                                HPScalar where) INLINE_CONST_FUNC;

}  // namespace oo

/* Comparison of vectors */
inline bool HPvector_equal(HPVector a, HPVector b) INLINE_CONST_FUNC;

/* Square of magnitude of vector */
inline oo::HPScalar HPmagnitude2(HPVector vec) INLINE_CONST_FUNC;

/* Magnitude of vector */
inline oo::HPScalar HPmagnitude(HPVector vec) INLINE_CONST_FUNC;

/* Normalize vector */
inline HPVector HPvector_normal(HPVector vec) INLINE_CONST_FUNC;

/* Normalize vector, returning fallback if zero vector. */
inline HPVector HPvector_normal_or_fallback(HPVector vec, HPVector fallback)
    INLINE_CONST_FUNC;
inline HPVector HPvector_normal_or_xbasis(HPVector vec) INLINE_CONST_FUNC;
inline HPVector HPvector_normal_or_ybasis(HPVector vec) INLINE_CONST_FUNC;
inline HPVector HPvector_normal_or_zbasis(HPVector vec) INLINE_CONST_FUNC;

/* Square of distance between vectors */
inline oo::HPScalar HPdistance2(HPVector v1, HPVector v2) INLINE_CONST_FUNC;

/* Distance between vectors */
inline oo::HPScalar HPdistance(HPVector v1, HPVector v2) INLINE_CONST_FUNC;

/* Dot product */
inline oo::HPScalar HPdot_product(HPVector first,
                                  HPVector second) INLINE_CONST_FUNC;

/* NORMALIZED cross product */
inline HPVector HPcross_product(HPVector first,
                                HPVector second) INLINE_CONST_FUNC;

/* General cross product */
inline HPVector HPtrue_cross_product(HPVector first,
                                     HPVector second) CONST_FUNC;

/* Triple product */
inline oo::HPScalar HPtriple_product(HPVector first, HPVector second,
                                     HPVector third) INLINE_CONST_FUNC;

/* Given three points on a surface, returns the normal to the surface. */
inline HPVector HPnormal_to_surface(HPVector v1, HPVector v2,
                                    HPVector v3) CONST_FUNC;

#ifndef OO_PLAIN_CPP_UNIT
#include <string>
#include <vector>
std::string HPVectorDescription(HPVector vector);        // "(x, y, z)"
std::vector<double> ArrayFromHPVector(HPVector vector);  // { x, y, z }

#endif

/*	OpenGL conveniences. Need to be macros to work with OOMacroOpenGL. */
#define GLVertexOOHPVector(v)     \
  do {                            \
    HPVector v_ = v;              \
    glVertex3f(v_.x, v_.y, v_.z); \
  } while (0)
#define GLTranslateOOHPVector(v)          \
  do {                                    \
    HPVector v_ = v;                      \
    OOGL(glTranslatef(v_.x, v_.y, v_.z)); \
  } while (0)

/*** Only inline definitions beyond this point ***/

inline HPVector make_HPvector(oo::HPScalar vx, oo::HPScalar vy,
                              oo::HPScalar vz) {
  HPVector result;
  result.x = vx;
  result.y = vy;
  result.z = vz;
  return result;
}

inline HPVector2D MakeHPVector2D(oo::HPScalar vx, oo::HPScalar vy) {
  HPVector2D result;
  result.x = vx;
  result.y = vy;
  return result;
}

inline HPVector vectorToHPVector(Vector v) {
  HPVector result;
  result.x = static_cast<oo::HPScalar>(v.x);
  result.y = static_cast<oo::HPScalar>(v.y);
  result.z = static_cast<oo::HPScalar>(v.z);
  return result;
}

inline Vector HPVectorToVector(HPVector v) {
  Vector result;
  result.x = static_cast<oo::Scalar>(v.x);
  result.y = static_cast<oo::Scalar>(v.y);
  result.z = static_cast<oo::Scalar>(v.z);
  return result;
}

inline void HPscale_vector(HPVector* vec, oo::HPScalar factor) {
  /*
          Clang static analyzer: reports an unintialized value here when called
          from the HUD's rescaling. This is blatantly wrong, as
          the array the vector comes from is fully initialized in the range
     being looped over.
          -- Ahruman 2012-09-14
  */
  vec->x *= factor;
  vec->y *= factor;
  vec->z *= factor;
}

inline HPVector HPvector_multiply_scalar(HPVector v, oo::HPScalar s) {
  /*
          Clang static analyzer: reports a garbage value here when called from
          OOMesh::rescaleByFactor(), apparently on baseless assumption that
          OOMesh._vertices points to only one vertex.
          -- Ahruman 2012-09-14
  */
  HPVector r;
  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  return r;
}

inline HPVector HPvector_add(HPVector a, HPVector b) {
  HPVector r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  r.z = a.z + b.z;
  return r;
}

namespace oo {

inline HPVector HPVectorInterpolate(HPVector a, HPVector b, HPScalar where) {
  return make_HPvector(std::lerp(a.x, b.x, where), std::lerp(a.y, b.y, where),
                       std::lerp(a.z, b.z, where));
}

inline HPVector HPVectorTowards(HPVector a, HPVector b, HPScalar where) {
  return make_HPvector(a.x + b.x * where, a.y + b.y * where, a.z + b.z * where);
}

}  // namespace oo

inline HPVector HPvector_subtract(HPVector a, HPVector b) {
  HPVector r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  r.z = a.z - b.z;
  return r;
}

inline HPVector HPvector_flip(HPVector v) {
  return HPvector_subtract(kZeroHPVector, v);
}

inline bool HPvector_equal(HPVector a, HPVector b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline oo::HPScalar HPmagnitude2(HPVector vec) {
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline oo::HPScalar HPmagnitude(HPVector vec) {
  return sqrt(HPmagnitude2(vec));
}

inline HPVector HPvector_normal_or_fallback(HPVector vec, HPVector fallback) {
  oo::HPScalar mag2 = HPmagnitude2(vec);
  if (mag2 == 0.0) [[unlikely]]
    return fallback;
  return HPvector_multiply_scalar(vec, 1.0 / sqrt(mag2));
}

inline HPVector HPvector_normal_or_xbasis(HPVector vec) {
  return HPvector_normal_or_fallback(vec, kBasisXHPVector);
}

inline HPVector HPvector_normal_or_ybasis(HPVector vec) {
  return HPvector_normal_or_fallback(vec, kBasisYHPVector);
}

inline HPVector HPvector_normal_or_zbasis(HPVector vec) {
  return HPvector_normal_or_fallback(vec, kBasisZHPVector);
}

inline HPVector HPvector_normal(HPVector vec) {
  return HPvector_normal_or_fallback(vec, kZeroHPVector);
}

inline oo::HPScalar HPdistance2(HPVector v1, HPVector v2) {
  return HPmagnitude2(HPvector_subtract(v1, v2));
}

inline oo::HPScalar HPdistance(HPVector v1, HPVector v2) {
  return HPmagnitude(HPvector_subtract(v1, v2));
}

inline HPVector HPtrue_cross_product(HPVector first, HPVector second) {
  HPVector result;
  result.x = (first.y * second.z) - (first.z * second.y);
  result.y = (first.z * second.x) - (first.x * second.z);
  result.z = (first.x * second.y) - (first.y * second.x);
  return result;
}

inline HPVector HPcross_product(HPVector first, HPVector second) {
  return HPvector_normal(HPtrue_cross_product(first, second));
}

inline oo::HPScalar HPdot_product(HPVector a, HPVector b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline oo::HPScalar HPtriple_product(HPVector first, HPVector second,
                                     HPVector third) {
  return HPdot_product(first, HPtrue_cross_product(second, third));
}

inline HPVector HPnormal_to_surface(HPVector v1, HPVector v2, HPVector v3) {
  HPVector d0, d1;
  d0 = HPvector_subtract(v2, v1);
  d1 = HPvector_subtract(v3, v2);
  return HPcross_product(d0, d1);
}
