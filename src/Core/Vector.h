/*

OOVector.h

Mathematical framework for Oolite.

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

#include "FastArithmetic.h"
#include "MathsBase.h"

#ifndef OOMATHS_EXTERNAL_VECTOR_TYPES

struct Vector {
  oo::Scalar x;
  oo::Scalar y;
  oo::Scalar z;
};

struct Vector2D {
  oo::Scalar x;
  oo::Scalar y;
};

#endif

namespace oo {

// The right, up and forward unit vectors of an orientation.
struct BasisVectors {
  Vector right;
  Vector up;
  Vector forward;
};

}  // namespace oo

extern const Vector kZeroVector, /* 0, 0, 0 */
    kBasisXVector,               /* 1, 0, 0 */
    kBasisYVector,               /* 0, 1, 0 */
    kBasisZVector;               /* 0, 0, 1 */

extern const Vector2D kZeroVector2D, /* 0, 0 */
    kBasisXVector2D,                 /* 1, 0 */
    kBasisYVector2D;                 /* 0, 1 */

/* Construct vector */
inline Vector make_vector(oo::Scalar vx, oo::Scalar vy,
                          oo::Scalar vz) INLINE_CONST_FUNC;
inline Vector2D MakeVector2D(oo::Scalar vx, oo::Scalar vy) INLINE_CONST_FUNC;

/* Generate random vectors. */
namespace oo {

Vector RandomUnitVector();
Vector VectorRandomSpatial(
    Scalar
        maxLength);  // Random vector uniformly distributed in radius-maxLength
                     // sphere. (Longer vectors are more common.)
Vector VectorRandomRadial(Scalar maxLength);

}  // namespace oo
   // Random vector with uniform distribution of
   // direction and radius in radius-maxLength sphere.
   // (Causes clustering at centre.)
// only needed in high-precision forms so far
// Vector OORandomPositionInCylinder(Vector centre1, OOScalar exclusion1, Vector
// centre2, OOScalar exclusion2, OOScalar radius); Vector
// OORandomPositionInShell(Vector centre, OOScalar inner, OOScalar outer);

/* Multiply vector by scalar (in place) */
inline void scale_vector(Vector* outVector,
                         oo::Scalar factor) ALWAYS_INLINE_FUNC NONNULL_FUNC;

/* Multiply vector by scalar */
inline Vector vector_multiply_scalar(Vector v, oo::Scalar s) INLINE_CONST_FUNC;

/* Addition and subtraction of vectors */
inline Vector vector_add(Vector a, Vector b) INLINE_CONST_FUNC;
inline Vector vector_subtract(Vector a, Vector b) INLINE_CONST_FUNC;
#define vector_between(a, b) vector_subtract(b, a)
inline Vector vector_flip(Vector v) INLINE_CONST_FUNC;

/* Vector linear interpolation */
namespace oo {

inline Vector VectorInterpolate(Vector a, Vector b,
                                Scalar where) INLINE_CONST_FUNC;
inline Vector VectorTowards(Vector a, Vector b, Scalar where) INLINE_CONST_FUNC;

}  // namespace oo

/* Comparison of vectors */
inline bool vector_equal(Vector a, Vector b) INLINE_CONST_FUNC;

/* Square of magnitude of vector */
inline oo::Scalar magnitude2(Vector vec) INLINE_CONST_FUNC;

/* Magnitude of vector */
inline oo::Scalar magnitude(Vector vec) INLINE_CONST_FUNC;

/* Normalize vector */
inline Vector vector_normal(Vector vec) INLINE_CONST_FUNC;

/* Normalize vector, returning fallback if zero vector. */
inline Vector vector_normal_or_fallback(Vector vec,
                                        Vector fallback) INLINE_CONST_FUNC;
inline Vector vector_normal_or_xbasis(Vector vec) INLINE_CONST_FUNC;
inline Vector vector_normal_or_ybasis(Vector vec) INLINE_CONST_FUNC;
inline Vector vector_normal_or_zbasis(Vector vec) INLINE_CONST_FUNC;

/* Square of distance between vectors */
inline oo::Scalar distance2(Vector v1, Vector v2) INLINE_CONST_FUNC;

/* Distance between vectors */
inline oo::Scalar distance(Vector v1, Vector v2) INLINE_CONST_FUNC;

/* Dot product */
inline oo::Scalar dot_product(Vector first, Vector second) INLINE_CONST_FUNC;

/* NORMALIZED cross product */
inline Vector cross_product(Vector first, Vector second) INLINE_CONST_FUNC;

/* General cross product */
inline Vector true_cross_product(Vector first, Vector second) CONST_FUNC;

/* Triple product */
inline oo::Scalar triple_product(Vector first, Vector second,
                                 Vector third) INLINE_CONST_FUNC;

/* Given three points on a surface, returns the normal to the surface. */
inline Vector normal_to_surface(Vector v1, Vector v2, Vector v3) CONST_FUNC;

#ifndef OO_PLAIN_CPP_UNIT
#include <string>
#include <vector>

#include "oofnd/PList.hpp"
std::string VectorDescription(Vector vector);  // "(x, y, z)"

/*	For storing vectors in property lists: a PList::Object node holds the
   box (OOJSShip.cpp makes them, OOPListGameTypes.cpp and the JavaScript engine
   read them). The box is the node's foreign object itself.
*/
namespace oo {

class NativeVector : public oo::PListForeign {
 public:
  explicit NativeVector(Vector vect);
  Vector getVector();

  // oo::PListForeign: the class name, and "%@" as <OONativeVector 0x...>.
  std::string className() const override;
  std::string description() const override;

 private:
  Vector v = {};
};

}  // namespace oo

#endif

/*	OpenGL conveniences. Need to be macros to work with OOMacroOpenGL. */
#define GLVertexOOVector(v)       \
  do {                            \
    Vector v_ = v;                \
    glVertex3f(v_.x, v_.y, v_.z); \
  } while (0)
#define GLTranslateOOVector(v)            \
  do {                                    \
    Vector v_ = v;                        \
    OOGL(glTranslatef(v_.x, v_.y, v_.z)); \
  } while (0)

/*** Only inline definitions beyond this point ***/

inline Vector make_vector(oo::Scalar vx, oo::Scalar vy, oo::Scalar vz) {
  Vector result;
  result.x = vx;
  result.y = vy;
  result.z = vz;
  return result;
}

inline Vector2D MakeVector2D(oo::Scalar vx, oo::Scalar vy) {
  Vector2D result;
  result.x = vx;
  result.y = vy;
  return result;
}

inline void scale_vector(Vector* vec, oo::Scalar factor) {
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

inline Vector vector_multiply_scalar(Vector v, oo::Scalar s) {
  /*
          Clang static analyzer: reports a garbage value here when called from
          OOMesh::rescaleByFactor(), apparently on baseless assumption that
          OOMesh._vertices points to only one vertex.
          -- Ahruman 2012-09-14
  */
  Vector r;
  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  return r;
}

inline Vector vector_add(Vector a, Vector b) {
  Vector r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  r.z = a.z + b.z;
  return r;
}

namespace oo {

inline Vector VectorInterpolate(Vector a, Vector b, Scalar where) {
  return make_vector(std::lerp(a.x, b.x, where), std::lerp(a.y, b.y, where),
                     std::lerp(a.z, b.z, where));
}

inline Vector VectorTowards(Vector a, Vector b, Scalar where) {
  return make_vector(a.x + b.x * where, a.y + b.y * where, a.z + b.z * where);
}

}  // namespace oo

inline Vector vector_subtract(Vector a, Vector b) {
  Vector r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  r.z = a.z - b.z;
  return r;
}

inline Vector vector_flip(Vector v) { return vector_subtract(kZeroVector, v); }

inline bool vector_equal(Vector a, Vector b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline oo::Scalar magnitude2(Vector vec) {
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline oo::Scalar magnitude(Vector vec) {
  return static_cast<oo::Scalar>(sqrt(magnitude2(vec)));
}

inline Vector vector_normal_or_fallback(Vector vec, Vector fallback) {
  oo::Scalar mag2 = magnitude2(vec);
  if (mag2 == 0.0f) [[unlikely]]
    return fallback;
  return vector_multiply_scalar(vec,
                                static_cast<oo::Scalar>(1.0f / sqrt(mag2)));
}

inline Vector vector_normal_or_xbasis(Vector vec) {
  return vector_normal_or_fallback(vec, kBasisXVector);
}

inline Vector vector_normal_or_ybasis(Vector vec) {
  return vector_normal_or_fallback(vec, kBasisYVector);
}

inline Vector vector_normal_or_zbasis(Vector vec) {
  return vector_normal_or_fallback(vec, kBasisZVector);
}

inline Vector vector_normal(Vector vec) {
  return vector_normal_or_fallback(vec, kZeroVector);
}

inline oo::Scalar distance2(Vector v1, Vector v2) {
  return magnitude2(vector_subtract(v1, v2));
}

inline oo::Scalar distance(Vector v1, Vector v2) {
  return magnitude(vector_subtract(v1, v2));
}

inline Vector true_cross_product(Vector first, Vector second) {
  Vector result;
  result.x = (first.y * second.z) - (first.z * second.y);
  result.y = (first.z * second.x) - (first.x * second.z);
  result.z = (first.x * second.y) - (first.y * second.x);
  return result;
}

inline Vector cross_product(Vector first, Vector second) {
  return vector_normal(true_cross_product(first, second));
}

inline oo::Scalar dot_product(Vector a, Vector b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline oo::Scalar triple_product(Vector first, Vector second, Vector third) {
  return dot_product(first, true_cross_product(second, third));
}

inline Vector normal_to_surface(Vector v1, Vector v2, Vector v3) {
  Vector d0, d1;
  d0 = vector_subtract(v2, v1);
  d1 = vector_subtract(v3, v2);
  return cross_product(d0, d1);
}
