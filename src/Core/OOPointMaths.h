#pragma once

#include <tgmath.h>

#include "OOFunctionAttributes.h"
#include "oofnd/objc/OOFoundationTypes.h"  // NSPoint

// Utilities for working with NSPoints as 2D vectors.
inline NSPoint PtAdd(NSPoint a, NSPoint b) {
  return NSMakePoint(a.x + b.x, a.y + b.y);
}

inline NSPoint PtSub(NSPoint a, NSPoint b) {
  return NSMakePoint(a.x - b.x, a.y - b.y);
}

inline NSPoint PtScale(NSPoint p, double scale) {
  return NSMakePoint(p.x * scale, p.y * scale);
}

inline double PtDot(NSPoint a, NSPoint b) { return a.x * b.x + a.y * b.y; }

inline double PtCross(NSPoint a, NSPoint b) { return a.x * b.y - b.x * a.y; }

inline NSPoint PtRotCW(NSPoint p) {
  // Rotate 90 degrees clockwise.
  return NSMakePoint(p.y, -p.x);
}

inline NSPoint PtRotACW(NSPoint p) {
  // Rotate 90 degrees anticlockwise.
  return NSMakePoint(-p.y, p.x);
}

inline NSPoint PtNormal(NSPoint p) {
  return PtScale(p, 1.0 / sqrt(PtDot(p, p)));
}
