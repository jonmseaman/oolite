/*

OOJSPropID.h (QuickJS-ng)

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

#include <quickjs.h>

#include <algorithm>
#include <cstddef>

#include "OOFunctionAttributes.h"

/*	Engine-neutral names for a property ID and a value (jsid and jsval with
        SpiderMonkey). Declared here, rather than in OOJavaScriptEngine.h, so
   game headers imported by OOJavaScriptEngine.h can use them.
*/
using OOJSPropID = JSAtom;
using OOJSVal = JSValue;

JSAtom OOJSInternAtomPRIVATE(const char* name);

namespace oo::js {

// A string literal as a template argument, so that each distinct name has its
// own cached atom.
template <std::size_t N>
struct AtomName {
  // Implicit on purpose: a string literal converts to it.
  consteval AtomName(const char (&name)[N]) { std::copy_n(name, N, chars); }

  char chars[N] = {};
};

// The interned atom for Name: made the first time it is asked for (a
// thread-safe static), then kept for the life of the process, as atoms are.
template <AtomName Name>
JSAtom Atom() {
  static const JSAtom atom = OOJSInternAtomPRIVATE(Name.chars);
  return atom;
}

}  // namespace oo::js

/*
        OOJSID(string literal)
        A property ID for a string literal: oo::js::Atom<literal>(). Kept as a
        macro because tests call it by this name.
*/
#define OOJSID(str) (oo::js::Atom<"" str>())
