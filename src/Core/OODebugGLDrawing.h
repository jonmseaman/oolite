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

#include "OOColor.h"
#include "OOMaths.h"

#if !defined(OODEBUGLDRAWING_DISABLE) && !defined(OO_DEBUG_MONITOR_ENABLED)
#define OODEBUGLDRAWING_DISABLE 1
#endif

#ifndef OODEBUGLDRAWING_DISABLE

class OOMaterial;

using OODebugWFState = struct {
  std::shared_ptr<OOMaterial> material;  // from OODebugBeginWireframe(),
                                         // given up by OODebugEndWireframe()
};

OODebugWFState OODebugBeginWireframe(
    bool ignoreZ);  // bool, not bool: the .cpp's bool is windows.h's int
void OODebugEndWireframe(OODebugWFState state);

inline void OODebugDrawBoundingBox(BoundingBox box);
inline void OODebugDrawBoundingBoxBetween(Vector min, Vector max);
inline void OODebugDrawColoredBoundingBox(BoundingBox box, OOColor* color);
void OODebugDrawColoredBoundingBoxBetween(Vector min, Vector max,
                                          OOColor* color);

// Normals are drawn as cyan lines
inline void OODebugDrawNormal(Vector position, Vector normal, GLfloat scale);
inline void OODebugDrawNormalAtOrigin(Vector normal, GLfloat scale);

// Other vectors are drawn as magenta lines by default
inline void OODebugDrawVector(Vector position, Vector v);
inline void OODebugDrawColoredVector(Vector position, Vector v, OOColor* color);
inline void OODebugDrawVectorAtOrigin(Vector v);
inline void OODebugDrawColoredVectorAtOrigin(Vector v, OOColor* color);

// Lines are drawn white by default
inline void OODebugDrawLine(Vector start, Vector end);
void OODebugDrawColoredLine(Vector start, Vector end, OOColor* color);

// Bases are drawn as one red, one green and one blue vector, representing x, y
// and z axes in the current coordinate frame.
void OODebugDrawBasis(Vector position, GLfloat scale);
inline void OODebugDrawBasisAtOrigin(GLfloat scale);

void OODebugDrawPoint(Vector position, OOColor* color);

/*** Only inline definitions beyond this point ***/

inline void OODebugDrawBoundingBoxBetween(Vector min, Vector max) {
  OODebugDrawColoredBoundingBoxBetween(min, max, OOColor::blueColor().get());
}

inline void OODebugDrawBoundingBox(BoundingBox box) {
  OODebugDrawBoundingBoxBetween(box.min, box.max);
}

inline void OODebugDrawColoredBoundingBox(BoundingBox box, OOColor* color) {
  OODebugDrawColoredBoundingBoxBetween(box.min, box.max, color);
}

inline void OODebugDrawNormal(Vector position, Vector normal, GLfloat scale) {
  OODebugDrawColoredVector(
      position, vector_add(position, vector_multiply_scalar(normal, scale)),
      OOColor::cyanColor().get());
}

inline void OODebugDrawNormalAtOrigin(Vector normal, GLfloat scale) {
  OODebugDrawNormal(kZeroVector, normal, scale);
}

inline void OODebugDrawColoredVector(Vector position, Vector v,
                                     OOColor* color) {
  OODebugDrawColoredLine(position, vector_add(position, v), color);
}

inline void OODebugDrawVector(Vector position, Vector v) {
  OODebugDrawColoredVector(position, v, OOColor::magentaColor().get());
}

inline void OODebugDrawVectorAtOrigin(Vector v) {
  OODebugDrawVector(kZeroVector, v);
}

inline void OODebugDrawColoredVectorAtOrigin(Vector v, OOColor* color) {
  OODebugDrawColoredVector(kZeroVector, v, color);
}

inline void OODebugDrawLine(Vector start, Vector end) {
  OODebugDrawColoredLine(start, end, OOColor::whiteColor().get());
}

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
