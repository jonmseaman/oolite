/*

OOMathsBase.h

The scalar types, constants and ABS the maths headers share. Each
maths header includes it, so every one of them compiles on its own; OOMaths.h
includes them all.

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

#ifndef OO_PLAIN_CPP_UNIT
#include "Foundation.h"
#endif

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <tgmath.h>

#include "FunctionAttributes.h"

#ifndef OO_PLAIN_CPP_UNIT
#include "OpenGL.h"
#else
/*	A plain C or C++ translation unit (OOVoxel.c, for one) needs only the GL
   types. OOOpenGL.h imports OOFoundation.h, i.e. Foundation, which does not
   compile in a plain C++ unit.
*/
#include "OpenGLOnly.h"
#endif

namespace oo {

using Scalar = GLfloat;

using HPScalar = double;

}  // namespace oo

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154 /* 1/pi */
#endif
#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308 /* 2/pi */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */
#endif
