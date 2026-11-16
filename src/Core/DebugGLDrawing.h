/*

OODebugDrawing.h

A set of functions for drawing debug stuff like bounding boxes. These are
drawn in wireframe without Z buffer reads or writes.


Copyright (C) 2007-2013 Jens Ayton and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <memory>

#include "Color.h"
#include "Maths.h"

#if !defined(OODEBUGLDRAWING_DISABLE) && !defined(OO_DEBUG_MONITOR_ENABLED)
#define OODEBUGLDRAWING_DISABLE 1
#endif

#ifndef OODEBUGLDRAWING_DISABLE

namespace oo {

class Material;

}  // namespace oo

namespace oo {

struct DebugWFState {
  std::shared_ptr<oo::Material> material;  // from OODebugBeginWireframe(),
                                           // given up by OODebugEndWireframe()
};

DebugWFState DebugBeginWireframe(
    bool ignoreZ);  // bool, not bool: the .cpp's bool is windows.h's int
void DebugEndWireframe(DebugWFState state);

}  // namespace oo

inline void OODebugDrawBoundingBox(BoundingBox box);
inline void OODebugDrawBoundingBoxBetween(Vector min, Vector max);

namespace oo {

inline void DebugDrawColoredBoundingBox(BoundingBox box, Color* color);
void DebugDrawColoredBoundingBoxBetween(Vector min, Vector max, Color* color);

}  // namespace oo

// Normals are drawn as cyan lines
inline void OODebugDrawNormal(Vector position, Vector normal, GLfloat scale);
inline void OODebugDrawNormalAtOrigin(Vector normal, GLfloat scale);

// Other vectors are drawn as magenta lines by default
inline void OODebugDrawVector(Vector position, Vector v);
inline void OODebugDrawColoredVector(Vector position, Vector v,
                                     oo::Color* color);
inline void OODebugDrawVectorAtOrigin(Vector v);
inline void OODebugDrawColoredVectorAtOrigin(Vector v, oo::Color* color);

// Lines are drawn white by default
namespace oo {

inline void DebugDrawLine(Vector start, Vector end);
void DebugDrawColoredLine(Vector start, Vector end, Color* color);

}  // namespace oo

// Bases are drawn as one red, one green and one blue vector, representing x, y
// and z axes in the current coordinate frame.
void OODebugDrawBasis(Vector position, GLfloat scale);
inline void OODebugDrawBasisAtOrigin(GLfloat scale);

namespace oo {

void DebugDrawPoint(Vector position, Color* color);

}  // namespace oo

/*** Only inline definitions beyond this point ***/

inline void OODebugDrawBoundingBoxBetween(Vector min, Vector max) {
  oo::DebugDrawColoredBoundingBoxBetween(min, max,
                                         oo::Color::blueColor().get());
}

inline void OODebugDrawBoundingBox(BoundingBox box) {
  OODebugDrawBoundingBoxBetween(box.min, box.max);
}

namespace oo {

inline void DebugDrawColoredBoundingBox(BoundingBox box, Color* color) {
  DebugDrawColoredBoundingBoxBetween(box.min, box.max, color);
}

}  // namespace oo

inline void OODebugDrawNormal(Vector position, Vector normal, GLfloat scale) {
  OODebugDrawColoredVector(
      position, vector_add(position, vector_multiply_scalar(normal, scale)),
      oo::Color::cyanColor().get());
}

inline void OODebugDrawNormalAtOrigin(Vector normal, GLfloat scale) {
  OODebugDrawNormal(kZeroVector, normal, scale);
}

inline void OODebugDrawColoredVector(Vector position, Vector v,
                                     oo::Color* color) {
  oo::DebugDrawColoredLine(position, vector_add(position, v), color);
}

inline void OODebugDrawVector(Vector position, Vector v) {
  OODebugDrawColoredVector(position, v, oo::Color::magentaColor().get());
}

inline void OODebugDrawVectorAtOrigin(Vector v) {
  OODebugDrawVector(kZeroVector, v);
}

inline void OODebugDrawColoredVectorAtOrigin(Vector v, oo::Color* color) {
  OODebugDrawColoredVector(kZeroVector, v, color);
}

namespace oo {

inline void DebugDrawLine(Vector start, Vector end) {
  DebugDrawColoredLine(start, end, Color::whiteColor().get());
}

}  // namespace oo

inline void OODebugDrawBasisAtOrigin(GLfloat scale) {
  OODebugDrawBasis(kZeroVector, scale);
}

#else  // OODEBUGLDRAWING_DISABLE

#define OODRAW_NOOP \
  do {              \
  } while (0)

#define OODebugDrawBoundingBox(box) OODRAW_NOOP
#define OODebugDrawBoundingBoxBetween(min, max) OODRAW_NOOP
#define OODebugDrawNormal(position, normal, scale) OODRAW_NOOP
#define OODebugDrawNormalAtOrigin(normal, scale) OODRAW_NOOP
#define OODebugDrawVector(position, v) OODRAW_NOOP
#define OODebugDrawColoredVector(position, v, color) OODRAW_NOOP
#define OODebugDrawVectorAtOrigin(v) OODRAW_NOOP
#define OODebugDrawColoredVectorAtOrigin(v, color) OODRAW_NOOP
#define OODebugDrawBasis(position, scale) OODRAW_NOOP
#define OODebugDrawBasisAtOrigin(scale) OODRAW_NOOP

#endif  // OODEBUGLDRAWING_DISABLE
