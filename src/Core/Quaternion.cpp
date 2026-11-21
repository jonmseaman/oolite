/*

OOQuaternion.m

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

const Quaternion kIdentityQuaternion = {1.0f, 0.0f, 0.0f, 0.0f};
const Quaternion kZeroQuaternion = {0.0f, 0.0f, 0.0f, 0.0f};

Quaternion quaternion_multiply(Quaternion q1, Quaternion q2) {
  Quaternion result;
  result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  result.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
  result.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
  return result;
}

// NOTE: this is broken - its distribution is weighted towards corners of the
// hypercube. Probably doesn't matter, though.
void quaternion_set_random(Quaternion* quat) {
  // -511.5 to +511.5
  quat->w = static_cast<oo::Scalar>(Ranrot() % 1024) - 511.5f;
  // -511.5 to +511.5
  quat->x = static_cast<oo::Scalar>(Ranrot() % 1024) - 511.5f;
  // -511.5 to +511.5
  quat->y = static_cast<oo::Scalar>(Ranrot() % 1024) - 511.5f;
  // -511.5 to +511.5
  quat->z = static_cast<oo::Scalar>(Ranrot() % 1024) - 511.5f;
  quaternion_normalize(quat);
}

Vector vector_forward_from_quaternion(Quaternion quat) {
  oo::Scalar w, wy, wx;
  oo::Scalar x, xz, xx;
  oo::Scalar y, yz, yy;
  oo::Scalar z, zz;
  Vector res;

  w = quat.w;
  z = quat.z;
  y = quat.y;
  x = quat.x;

  xx = 2.0f * x;
  yy = 2.0f * y;
  zz = 2.0f * z;
  wx = w * xx;
  wy = w * yy;
  xx = x * xx;
  xz = x * zz;
  yy = y * yy;
  yz = y * zz;

  res.x = xz - wy;
  res.y = yz + wx;
  res.z = 1.0f - xx - yy;

  if (static_cast<bool>(res.x) || static_cast<bool>(res.y) ||
      static_cast<bool>(res.z))
    return vector_normal(res);
  else
    return make_vector(0.0f, 0.0f, 1.0f);
}

HPVector HPvector_forward_from_quaternion(Quaternion quat) {
  // HPVect: profile this later
  return vectorToHPVector(vector_forward_from_quaternion(quat));
}

Vector vector_up_from_quaternion(Quaternion quat) {
  oo::Scalar w, wz, wx;
  oo::Scalar x, xy, xx;
  oo::Scalar y, yz, yy;
  oo::Scalar z, zz;
  Vector res;

  w = quat.w;
  z = quat.z;
  y = quat.y;
  x = quat.x;

  xx = 2.0f * x;
  yy = 2.0f * y;
  zz = 2.0f * z;
  wx = w * xx;
  wz = w * zz;
  xx = x * xx;
  xy = x * yy;
  yz = y * zz;
  zz = z * zz;

  res.x = xy + wz;
  res.y = 1.0f - xx - zz;
  res.z = yz - wx;

  if (static_cast<bool>(res.x) || static_cast<bool>(res.y) ||
      static_cast<bool>(res.z))
    return vector_normal(res);
  else
    return make_vector(0.0f, 1.0f, 0.0f);
}

Vector vector_right_from_quaternion(Quaternion quat) {
  oo::Scalar w, wz, wy;
  oo::Scalar x, xz, xy;
  oo::Scalar y, yy;
  oo::Scalar z, zz;
  Vector res;

  w = quat.w;
  z = quat.z;
  y = quat.y;
  x = quat.x;

  yy = 2.0f * y;
  zz = 2.0f * z;
  wy = w * yy;
  wz = w * zz;
  xy = x * yy;
  xz = x * zz;
  yy = y * yy;
  zz = z * zz;

  res.x = 1.0f - yy - zz;
  res.y = xy - wz;
  res.z = xz + wy;

  if (static_cast<bool>(res.x) || static_cast<bool>(res.y) ||
      static_cast<bool>(res.z))
    return vector_normal(res);
  else
    return make_vector(1.0f, 0.0f, 0.0f);
}

oo::BasisVectors basis_vectors_from_quaternion(Quaternion quat) {
  oo::BasisVectors basis = {};
  oo::Scalar w, wz, wy, wx;
  oo::Scalar x, xz, xy, xx;
  oo::Scalar y, yz, yy;
  oo::Scalar z, zz;

  w = quat.w;
  z = quat.z;
  y = quat.y;
  x = quat.x;

  xx = 2.0f * x;
  yy = 2.0f * y;
  zz = 2.0f * z;
  wx = w * xx;
  wy = w * yy;
  wz = w * zz;
  xx = x * xx;
  xy = x * yy;
  xz = x * zz;
  yy = y * yy;
  yz = y * zz;
  zz = z * zz;

  {
    basis.right.x = 1.0f - yy - zz;
    basis.right.y = xy - wz;
    basis.right.z = xz + wy;

    if (static_cast<bool>(basis.right.x) || static_cast<bool>(basis.right.y) ||
        static_cast<bool>(basis.right.z))
      basis.right = vector_normal(basis.right);
    else
      basis.right = make_vector(1.0f, 0.0f, 0.0f);
  }

  {
    basis.up.x = xy + wz;
    basis.up.y = 1.0f - xx - zz;
    basis.up.z = yz - wx;

    if (static_cast<bool>(basis.up.x) || static_cast<bool>(basis.up.y) ||
        static_cast<bool>(basis.up.z))
      basis.up = vector_normal(basis.up);
    else
      basis.up = make_vector(0.0f, 1.0f, 0.0f);
  }

  {
    basis.forward.x = xz - wy;
    basis.forward.y = yz + wx;
    basis.forward.z = 1.0f - xx - yy;

    if (static_cast<bool>(basis.forward.x) ||
        static_cast<bool>(basis.forward.y) ||
        static_cast<bool>(basis.forward.z))
      basis.forward = vector_normal(basis.forward);
    else
      basis.forward = make_vector(0.0f, 0.0f, 1.0f);
  }
  return basis;
}

Quaternion quaternion_rotation_between(Vector v0, Vector v1) {
  Quaternion q;
  oo::Scalar s = sqrt((1.0f + v0.x * v1.x + v0.y * v1.y + v0.z * v1.z) * 2.0f);
  if (s > 0.0f) [[likely]] {
    oo::Scalar is = 1.0f / s;
    q.x = (v0.y * v1.z - v0.z * v1.y) * is;
    q.y = (v0.z * v1.x - v0.x * v1.z) * is;
    q.z = (v0.x * v1.y - v0.y * v1.x) * is;
    q.w = s * 0.5f;
  } else {
    // Is this actually a problem?
    if (vector_equal(v1, kBasisZVector) || vector_equal(v0, kBasisZVector)) {
      q = make_quaternion(0, 1, 0, 0);
    } else {
      q = kIdentityQuaternion;
    }
    // We arrive here for antiparallel vectors. Rotation axis is then undefined,
    // but not rotating is wrong. Probably the calling function should exclude
    // this situation. For current in-game use of this function we return
    // (0,1,0,0), but generally that is also wrong.
  }
  return q;
}

Quaternion quaternion_rotation_betweenHP(HPVector v0, HPVector v1) {
  Quaternion q;
  oo::HPScalar s = sqrt((1.0 + v0.x * v1.x + v0.y * v1.y + v0.z * v1.z) * 2.0);
  if (s > 0.0) [[likely]] {
    oo::HPScalar is = 1.0 / s;
    q.x = static_cast<oo::Scalar>((v0.y * v1.z - v0.z * v1.y) * is);
    q.y = static_cast<oo::Scalar>((v0.z * v1.x - v0.x * v1.z) * is);
    q.z = static_cast<oo::Scalar>((v0.x * v1.y - v0.y * v1.x) * is);
    q.w = static_cast<oo::Scalar>(s * 0.5);
  } else {
    // Is this actually a problem?
    if (HPvector_equal(v1, kBasisZHPVector) ||
        HPvector_equal(v0, kBasisZHPVector)) {
      q = make_quaternion(0, 1, 0, 0);
    } else {
      q = kIdentityQuaternion;
    }
    // We arrive here for antiparallel vectors. Rotation axis is then undefined,
    // but not rotating is wrong. Probably the calling function should exclude
    // this situation. For current in-game use of this function we return
    // (0,1,0,0), but generally that is also wrong.
  }
  return q;
}

Quaternion quaternion_limited_rotation_between(Vector v0, Vector v1,
                                               float maxArc) {
  // vectors both normalised
  Quaternion q;
  oo::Scalar min_s = 2.0f * cosf(0.5f * maxArc);
  oo::Scalar s = sqrtf((1.0f + v0.x * v1.x + v0.y * v1.y + v0.z * v1.z) * 2.0f);
  // for some antiparallel vectors, s returns a NaN instead of 0. Testing s > 0
  // catches both.
  if (s > 0.0f) [[likely]] {
    if (s < min_s) {
      // larger angle => smaller cos
      oo::Scalar a = maxArc * 0.5f;
      oo::Scalar w = cosf(a);
      oo::Scalar scale = sinf(a);
      q.x = (v0.y * v1.z - v0.z * v1.y) * scale;
      q.y = (v0.z * v1.x - v0.x * v1.z) * scale;
      q.z = (v0.x * v1.y - v0.y * v1.x) * scale;
      q.w = w;
    } else {
      oo::Scalar is = 1.0f / s;
      q.x = (v0.y * v1.z - v0.z * v1.y) * is;
      q.y = (v0.z * v1.x - v0.x * v1.z) * is;
      q.z = (v0.x * v1.y - v0.y * v1.x) * is;
      q.w = s * 0.5f;
    }
  } else {
    // Is this actually a problem?
    q = kIdentityQuaternion;
  }
  return q;
}

void quaternion_rotate_about_x(Quaternion* quat, oo::Scalar angle) {
  Quaternion result;
  oo::Scalar a = angle * 0.5f;
  oo::Scalar w = cosf(a);
  oo::Scalar scale = sinf(a);

  result.w = quat->w * w - quat->x * scale;
  result.x = quat->w * scale + quat->x * w;
  result.y = quat->y * w + quat->z * scale;
  result.z = quat->z * w - quat->y * scale;

  quat->w = result.w;
  quat->x = result.x;
  quat->y = result.y;
  quat->z = result.z;
}

void quaternion_rotate_about_y(Quaternion* quat, oo::Scalar angle) {
  Quaternion result;
  oo::Scalar a = angle * 0.5f;
  oo::Scalar w = cosf(a);
  oo::Scalar scale = sinf(a);

  result.w = quat->w * w - quat->y * scale;
  result.x = quat->x * w - quat->z * scale;
  result.y = quat->w * scale + quat->y * w;
  result.z = quat->z * w + quat->x * scale;

  quat->w = result.w;
  quat->x = result.x;
  quat->y = result.y;
  quat->z = result.z;
}

void quaternion_rotate_about_z(Quaternion* quat, oo::Scalar angle) {
  Quaternion result;
  oo::Scalar a = angle * 0.5f;
  oo::Scalar w = cosf(a);
  oo::Scalar scale = sinf(a);

  result.w = quat->w * w - quat->z * scale;
  result.x = quat->x * w + quat->y * scale;
  result.y = quat->y * w - quat->x * scale;
  result.z = quat->w * scale + quat->z * w;

  quat->w = result.w;
  quat->x = result.x;
  quat->y = result.y;
  quat->z = result.z;
}

void quaternion_rotate_about_axis(Quaternion* quat, Vector axis,
                                  oo::Scalar angle) {
  Quaternion q2 /*, result */;
  oo::Scalar a = angle * 0.5f;
  oo::Scalar w = cosf(a);
  oo::Scalar scale = sinf(a);

  q2.w = w;
  q2.x = axis.x * scale;
  q2.y = axis.y * scale;
  q2.z = axis.z * scale;

  *quat = quaternion_multiply(*quat, q2);
}

std::string QuaternionDescription(Quaternion quaternion) {
  float x, y, z;
  char xs, ys, zs;

  x = fabsf(quaternion.x);
  y = fabsf(quaternion.y);
  z = fabsf(quaternion.z);

  xs = (quaternion.x >= 0.0f) ? '+' : '-';
  ys = (quaternion.y >= 0.0f) ? '+' : '-';
  zs = (quaternion.z >= 0.0f) ? '+' : '-';

  return std::format("({:g} {} {:g}i {} {:g}j {} {:g}k)", quaternion.w, xs, x,
                     ys, y, zs, z);
}

Vector quaternion_rotate_vector(Quaternion q, Vector v) {
  Quaternion qv;

  qv.w = 0.0f - q.x * v.x - q.y * v.y - q.z * v.z;
  qv.x = -q.w * v.x + q.y * v.z - q.z * v.y;
  qv.y = -q.w * v.y + q.z * v.x - q.x * v.z;
  qv.z = -q.w * v.z + q.x * v.y - q.y * v.x;

  v.x = qv.w * -q.x + qv.x * -q.w + qv.y * -q.z - qv.z * -q.y;
  v.y = qv.w * -q.y + qv.y * -q.w + qv.z * -q.x - qv.x * -q.z;
  v.z = qv.w * -q.z + qv.z * -q.w + qv.x * -q.y - qv.y * -q.x;

  return v;
}

HPVector quaternion_rotate_HPvector(Quaternion q, HPVector v) {
  oo::HPScalar qvw, qvx, qvy, qvz;

  qvw = 0.0 - q.x * v.x - q.y * v.y - q.z * v.z;
  qvx = -q.w * v.x + q.y * v.z - q.z * v.y;
  qvy = -q.w * v.y + q.z * v.x - q.x * v.z;
  qvz = -q.w * v.z + q.x * v.y - q.y * v.x;

  v.x = qvw * -q.x + qvx * -q.w + qvy * -q.z - qvz * -q.y;
  v.y = qvw * -q.y + qvy * -q.w + qvz * -q.x - qvx * -q.z;
  v.z = qvw * -q.z + qvz * -q.w + qvx * -q.y - qvy * -q.x;

  return v;
}
