/*

OOQuaternion.h

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

#include "MathsBase.h"

// Each maths header includes what it uses, so OOMaths.h may list them in any
// order.
#include "HPVector.h"
#include "Vector.h"

struct Quaternion {
  oo::Scalar w;
  oo::Scalar x;
  oo::Scalar y;
  oo::Scalar z;
};

extern const Quaternion kIdentityQuaternion;  // 1, 0, 0, 0
extern const Quaternion kZeroQuaternion;      // 0, 0, 0, 0

/* Construct quaternion */
inline Quaternion make_quaternion(oo::Scalar qw, oo::Scalar qx, oo::Scalar qy,
                                  oo::Scalar qz) INLINE_CONST_FUNC;

/* Comparison of quaternions */
inline bool quaternion_equal(Quaternion a, Quaternion b) INLINE_CONST_FUNC;

/* Multiply quaternions */
Quaternion quaternion_multiply(Quaternion q1, Quaternion q2) CONST_FUNC;

/* Negation, or additive inverse -- negate all components */
inline Quaternion quaternion_negate(Quaternion q) INLINE_CONST_FUNC;

/* Conjugate, or spacial inverse -- negate x, y, z components */
inline Quaternion quaternion_conjugate(Quaternion q) INLINE_CONST_FUNC;

/* Set quaternion to random unit quaternion */
void quaternion_set_random(Quaternion* quat) NONNULL_FUNC;

namespace oo {

inline Quaternion RandomQuaternion() ALWAYS_INLINE_FUNC;

}  // namespace oo

/* Build quaternion representing a rotation around a given axis */
inline void quaternion_set_rotate_about_axis(Quaternion* quat, Vector axis,
                                             oo::Scalar angle) NONNULL_FUNC;

/* Inner product of two quaternions */
inline oo::Scalar quaternion_dot_product(Quaternion q1,
                                         Quaternion q2) CONST_FUNC;

/* Create basis vectors from a quaternion. */
Vector vector_forward_from_quaternion(Quaternion quat) CONST_FUNC;
Vector vector_up_from_quaternion(Quaternion quat) CONST_FUNC;
Vector vector_right_from_quaternion(Quaternion quat) CONST_FUNC;

HPVector HPvector_forward_from_quaternion(Quaternion quat) CONST_FUNC;

oo::BasisVectors basis_vectors_from_quaternion(Quaternion quat);

/* produce a quaternion representing an angle between two vectors. Assumes the
 * vectors are normalized. */
Quaternion quaternion_rotation_between(Vector v0, Vector v1) CONST_FUNC;
Quaternion quaternion_rotation_betweenHP(HPVector v0, HPVector v1) CONST_FUNC;

/* produce a quaternion representing an angle between two vectors with a maximum
 * arc */
Quaternion quaternion_limited_rotation_between(Vector v0, Vector v1,
                                               float maxArc) CONST_FUNC;

/* Rotate a quaternion about a fixed axis. */
void quaternion_rotate_about_x(Quaternion* quat, oo::Scalar angle) NONNULL_FUNC;
void quaternion_rotate_about_y(Quaternion* quat, oo::Scalar angle) NONNULL_FUNC;
void quaternion_rotate_about_z(Quaternion* quat, oo::Scalar angle) NONNULL_FUNC;
void quaternion_rotate_about_axis(Quaternion* quat, Vector axis,
                                  oo::Scalar angle) NONNULL_FUNC;

/* Normalize quaternion */
inline void quaternion_normalize(Quaternion* quat)
    NONNULL_FUNC ALWAYS_INLINE_FUNC;

#include <string>
#include <vector>
std::string QuaternionDescription(
    Quaternion quaternion);  // "(w + xi + yj + zk)"

Vector quaternion_rotate_vector(Quaternion q, Vector vector) CONST_FUNC;
HPVector quaternion_rotate_HPvector(Quaternion q, HPVector vector) CONST_FUNC;

/*** Only inline definitions beyond this point ***/

inline Quaternion make_quaternion(oo::Scalar qw, oo::Scalar qx, oo::Scalar qy,
                                  oo::Scalar qz) {
  Quaternion result;
  result.w = qw;
  result.x = qx;
  result.y = qy;
  result.z = qz;
  return result;
}

inline bool quaternion_equal(Quaternion a, Quaternion b) {
  return a.w == b.w && a.x == b.x && a.y == b.y && a.z == b.z;
}

inline Quaternion quaternion_negate(Quaternion q) {
  return make_quaternion(-q.w, -q.x, -q.y, -q.z);
}

inline Quaternion quaternion_conjugate(Quaternion q) {
  return make_quaternion(q.w, -q.x, -q.y, -q.z);
}

inline void quaternion_set_rotate_about_axis(Quaternion* quat, Vector axis,
                                             oo::Scalar angle) {
  oo::Scalar a = angle * 0.5f;
  oo::Scalar scale = static_cast<oo::Scalar>(sin(a));

  quat->w = static_cast<oo::Scalar>(cos(a));
  quat->x = axis.x * scale;
  quat->y = axis.y * scale;
  quat->z = axis.z * scale;
}

inline oo::Scalar quaternion_dot_product(Quaternion q1, Quaternion q2) {
  return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

inline void quaternion_normalize(Quaternion* quat) {
  oo::Scalar w = quat->w;
  oo::Scalar x = quat->x;
  oo::Scalar y = quat->y;
  oo::Scalar z = quat->z;

  oo::Scalar lv =
      static_cast<oo::Scalar>(1.0f / sqrt(w * w + x * x + y * y + z * z));

  quat->w = lv * w;
  quat->x = lv * x;
  quat->y = lv * y;
  quat->z = lv * z;
}

namespace oo {

inline Quaternion RandomQuaternion() {
  Quaternion q;
  quaternion_set_random(&q);
  return q;
}

}  // namespace oo
