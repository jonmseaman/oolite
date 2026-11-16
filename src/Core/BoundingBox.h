/*

OOBoundingBox.h

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

struct BoundingBox {
  Vector min;
  Vector max;
};

inline constexpr BoundingBox kZeroBoundingBox = {{0.0f, 0.0f, 0.0f},
                                                 {0.0f, 0.0f, 0.0f}};

/* Extend bounding box to contain specified point. */
inline void bounding_box_add_vector(BoundingBox* box,
                                    Vector vec) ALWAYS_INLINE_FUNC NONNULL_FUNC;
inline void bounding_box_add_xyz(BoundingBox* box, GLfloat x, GLfloat y,
                                 GLfloat z) ALWAYS_INLINE_FUNC NONNULL_FUNC;

/* Reset bounding box to kZeroBoundingBox. */
inline void bounding_box_reset(BoundingBox* box) NONNULL_FUNC;

/* Reset bounding box to a zero-sized box surrounding specified vector. */
inline void bounding_box_reset_to_vector(BoundingBox* box, Vector vec)
    ALWAYS_INLINE_FUNC NONNULL_FUNC;

inline void bounding_box_get_dimensions(BoundingBox bb, GLfloat* xSize,
                                        GLfloat* ySize,
                                        GLfloat* zSize) ALWAYS_INLINE_FUNC;

namespace oo {

inline Vector BoundingBoxCenter(BoundingBox bb) INLINE_CONST_FUNC;

Vector RandomPositionInBoundingBox(BoundingBox bb);
HPVector HPRandomPositionInBoundingBox(BoundingBox bb);

}  // namespace oo

/*** Only inline definitions beyond this point ***/

inline void bounding_box_add_vector(BoundingBox* box, Vector vec) {
  box->min.x = static_cast<oo::Scalar>(fmin(box->min.x, vec.x));
  box->max.x = static_cast<oo::Scalar>(fmax(box->max.x, vec.x));
  box->min.y = static_cast<oo::Scalar>(fmin(box->min.y, vec.y));
  box->max.y = static_cast<oo::Scalar>(fmax(box->max.y, vec.y));
  box->min.z = static_cast<oo::Scalar>(fmin(box->min.z, vec.z));
  box->max.z = static_cast<oo::Scalar>(fmax(box->max.z, vec.z));
}

inline void bounding_box_add_xyz(BoundingBox* box, GLfloat x, GLfloat y,
                                 GLfloat z) {
  box->min.x = static_cast<oo::Scalar>(fmin(box->min.x, x));
  box->max.x = static_cast<oo::Scalar>(fmax(box->max.x, x));
  box->min.y = static_cast<oo::Scalar>(fmin(box->min.y, y));
  box->max.y = static_cast<oo::Scalar>(fmax(box->max.y, y));
  box->min.z = static_cast<oo::Scalar>(fmin(box->min.z, z));
  box->max.z = static_cast<oo::Scalar>(fmax(box->max.z, z));
}

inline void bounding_box_reset(BoundingBox* box) { *box = kZeroBoundingBox; }

inline void bounding_box_reset_to_vector(BoundingBox* box, Vector vec) {
  box->min = vec;
  box->max = vec;
}

inline void bounding_box_get_dimensions(BoundingBox bb, GLfloat* xSize,
                                        GLfloat* ySize, GLfloat* zSize) {
  if (xSize != nullptr) *xSize = bb.max.x - bb.min.x;
  if (ySize != nullptr) *ySize = bb.max.y - bb.min.y;
  if (zSize != nullptr) *zSize = bb.max.z - bb.min.z;
}

namespace oo {

inline Vector BoundingBoxCenter(BoundingBox bb) {
  return vector_multiply_scalar(vector_add(bb.min, bb.max), 0.5f);
}

}  // namespace oo
