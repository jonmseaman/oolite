/*

OOJSPropID.h


JavaScript support for Oolite
Copyright (C) 2007-2013 David Taylor and Jens Ayton.

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

#include "OOFunctionAttributes.h"
#include "ooscript/JSEngine.hpp"

/*
        OOJSID(const char * [literal])
        Macro to create a string-based ooscript::PropertyId. The string is
   interned and converted into a string by a helper the first time the macro is
   hit, then cached.
*/

#if defined(__GNUC__)
#define OOJSID(str)                               \
  ({                                              \
    static ooscript::PropertyId idCache;          \
    static bool inited;                           \
    if (EXPECT_NOT(!inited)) {                    \
      OOJSInitJSIDCachePRIVATE("" str, &idCache); \
      inited = true;                              \
    }                                             \
    idCache;                                      \
  })
#else
// cl.exe has no statement expressions. Each lambda expression is its own type,
// so each use still has its own cache.
#define OOJSID(str)                               \
  ([]() -> ooscript::PropertyId {                 \
    static ooscript::PropertyId idCache;          \
    static bool inited;                           \
    if (EXPECT_NOT(!inited)) {                    \
      OOJSInitJSIDCachePRIVATE("" str, &idCache); \
      inited = true;                              \
    }                                             \
    return idCache;                               \
  }())
#endif
void OOJSInitJSIDCachePRIVATE(const char* name, ooscript::PropertyId* idCache);
