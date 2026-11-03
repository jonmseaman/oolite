/*

OOJSEngineDebuggerHelpers.cpp (QuickJS-ng)

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

/*
        These functions exist to help debugging JavaScript code. They can be
   called directly from gdb, for example:

                call (char *)JSValueToStrDbg(someValue)

        The functions are:

                const char *JSValueToStrDbg(JSValue)
                const char *JSObjectToStrDbg(JSValue)
                const char *JSStringToStrDbg(JSValue)
                Converts any JS value to a string, using the complete process
   and potentially invoking JS toString() methods. This might mess up QuickJS
   internal state in some cases (for instance, if called while an exception is
   pending).

                const char *JSValueToStrSafeDbg(JSValue)
                const char *JSObjectToStrSafeDbg(JSValue)
                const char *JSStringToStrSafeDbg(JSValue)
                As above, but without calling any JavaScript code. Primitives
                (including strings) are converted; for objects only the class
   name is provided.

                const char *JSIDToStrSafeDbg(JSAtom)
                Like JSValueToStrSafeDbg() for property IDs (atoms).

                const char *JSValueTypeDbg(JSValue)
                Returns the type of the value, or the class name if it's an
   object.

        The text a function returns stays valid until the next call of one of
   them.

        Changes from the SpiderMonkey version: QuickJS has no JSObject* or
        JSString* handle types, so the Object and String variants take a
   JSValue and are equivalent to the Value variants. Strings can always be read
   without running JS code, so the Safe variants show string contents rather
   than "string [n chars]". JS_ARRAY_HOLE and the other SpiderMonkey "magic"
   values do not exist; QuickJS's internal tags are reported instead.

         Another useful function is OOJSDumpStack (results are found in the
   log):

                call OOJSDumpStack(context)

        QuickJS has no stack frame API, so the dump is the stack property of a
        new Error: function names and locations only.

        In almost all Oolite functions that deal with JavaScript, there is a
   single JSContext called "context". In QuickJS functions, it's called "ctx".

        The public functions keep external linkage on purpose: they are called
   by name from gdb and Xcode data formatters, never through a declaration in
   a header that clang-tidy's misc-use-internal-linkage could see.
*/

#ifdef OO_DEBUG_MONITOR_ENABLED

#include "OOJavaScriptEngine.h"
#include "oofnd/StdLib.hpp"
#include "oofnd/String.hpp"

// Exported for use from a debugger; not declared in a header.
const char* JSValueToStrDbg(JSValue val);
const char* JSObjectToStrDbg(JSValue obj);
const char* JSStringToStrDbg(JSValue str);
const char* JSValueTypeDbg(JSValue val);
const char* JSValueToStrSafeDbg(JSValue val);
const char* JSObjectToStrSafeDbg(JSValue obj);
const char* JSStringToStrSafeDbg(JSValue str);
const char* JSIDToStrSafeDbg(JSAtom anID);

namespace {
// The text a *Dbg function returns stays valid until the next such call (it
// was an autoreleased string's UTF-8 buffer, valid until the pool drained); the
// debugger prints it straight away.
const char* DebuggerCString(std::string text) {
  static std::string sText;
  sText = std::move(text);
  return sText.c_str();
}
}  // namespace

const char* JSValueToStrDbg(
    JSValue val)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  JSContext* context = OOJSAcquireContext();
  const std::optional<std::string> text =
      cxx_OOStringFromJSValueEvenIfNull(context, val);
  const char* result = text.has_value() ? DebuggerCString(*text) : NULL;
  OOJSRelinquishContext(context);

  return result;
}

const char* JSObjectToStrDbg(
    JSValue obj)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  return JSValueToStrDbg(obj);
}

const char* JSStringToStrDbg(
    JSValue str)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  return JSValueToStrDbg(str);
}

const char* JSValueTypeDbg(
    JSValue val)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  switch (JS_VALUE_GET_TAG(val)) {
    case JS_TAG_INT:
      return "integer";
    case JS_TAG_FLOAT64:
      return "double";
    case JS_TAG_STRING:
    case JS_TAG_STRING_ROPE:
      return "string";
    case JS_TAG_BOOL:
      return "boolean";
    case JS_TAG_NULL:
      return "null";
    case JS_TAG_UNDEFINED:
      return "void";
    case JS_TAG_SYMBOL:
      return "symbol";
    case JS_TAG_BIG_INT:
    case JS_TAG_SHORT_BIG_INT:
      return "bigint";
    case JS_TAG_UNINITIALIZED:
      return "magic (uninitialized)";
    case JS_TAG_CATCH_OFFSET:
      return "magic (catch offset)";
    case JS_TAG_EXCEPTION:
      return "magic (exception)";
    case JS_TAG_MODULE:
      return "magic (module)";
    case JS_TAG_FUNCTION_BYTECODE:
      return "magic (function bytecode)";
    case JS_TAG_OBJECT: {
      const char* className = OOJSClassName(JS_GetClassID(val));
      return (className != NULL) ? className : "object";
    }
  }
  return "unknown";
}

// Doesn't run JavaScript code.
const char* JSValueToStrSafeDbg(
    JSValue val)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  std::string formatted;

  switch (JS_VALUE_GET_TAG(val)) {
    case JS_TAG_INT:
      formatted = oo::str::format("%i", JS_VALUE_GET_INT(val));
      break;

    case JS_TAG_FLOAT64:
      formatted = oo::str::format("%g", JS_VALUE_GET_FLOAT64(val));
      break;

    case JS_TAG_BOOL:
      formatted = JS_VALUE_GET_BOOL(val) ? "true" : "false";
      break;

    case JS_TAG_STRING:
    case JS_TAG_STRING_ROPE: {
      // Converting a string primitive doesn't call into JS code.
      JSContext* context = OOJSAcquireContext();
      size_t length;
      const char* chars = JS_ToCStringLen(context, &length, val);
      if (chars != NULL) {
        formatted = std::string(chars, length);
        JS_FreeCString(context, chars);
      } else {
        JS_FreeValue(context, JS_GetException(context));
        formatted = "string [unreadable]";
      }
      break;
    }

    case JS_TAG_UNDEFINED:
      return "undefined";

    default:
      return JSValueTypeDbg(val);
  }

  return DebuggerCString(std::move(formatted));
}

const char* JSObjectToStrSafeDbg(
    JSValue obj)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  return JSValueToStrSafeDbg(obj);
}

const char* JSStringToStrSafeDbg(
    JSValue str)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  return JSValueToStrSafeDbg(str);
}

const char* JSIDToStrSafeDbg(
    JSAtom anID)  // NOLINT(misc-use-internal-linkage): called by name
                  // from gdb, see file header.
{
  if (anID == JS_ATOM_NULL) return "void";

  JSContext* context = OOJSAcquireContext();
  std::string formatted;
  const char* chars = JS_AtomToCString(context, anID);
  if (chars != NULL) {
    formatted = chars;
    JS_FreeCString(context, chars);
  } else {
    JS_FreeValue(context, JS_GetException(context));
    formatted = oo::str::format("unknown <0x%X>", static_cast<unsigned>(anID));
  }

  return DebuggerCString(std::move(formatted));
}
#endif
