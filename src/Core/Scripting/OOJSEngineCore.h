/*

OOJSEngineCore.h

The plain C++ part of the JavaScript engine API (upstream #768's
OOJavaScriptEngine.h, QuickJS-ng): context acquisition, error reporters,
argument helpers, the value/PList/string converters, class registration
(OOJSClasses.cpp), converter registration, the time limiter, the native
wrappers and the OOJS_* property flags and return macros. Nothing here declares
an Objective-C class, so a binding translation unit that is plain C++ includes
this header instead of OOJavaScriptEngine.h, which includes it back.

Where #768 passes NSString, this API passes std::string (std::optional where
#768 allows nil) and the cxx_ prefix marks the function as the C++ form; where
#768 passes an id or NSDictionary from a JS value, this API passes an oo::PList.

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

/*	REFERENCE OWNERSHIP CONVENTIONS (as #768's)

        SpiderMonkey values were traced by the garbage collector and never freed
        explicitly. QuickJS values are reference counted, so every JSValue is
        either owned (the holder must eventually JS_FreeValue() it, or hand it
        on) or borrowed (JSValueConst; the holder must not free it).

        1. Parameters typed JSValueConst are borrowed. Native callbacks never
   free their this value or arguments.
        2. JSValues returned from functions, and JSValues stored through out
           parameters (e.g. VectorToJSValue(context, v, &outValue)), are owned
   by the caller. This includes OOJSPrivateObject::jsValueInContext(),
           OOJSValueFromPList() and OOJSSTR(). On failure the out parameter is
           left as JS_UNDEFINED and need not be freed.
        3. OOJS_RETURN(v) and friends transfer ownership of v to QuickJS.
        4. A native object that keeps a JSValue (the old root pattern) owns a
           reference and must free it when done, and on
           kOOJavaScriptEngineWillResetNotificationName. Back references from JS
           to native objects use weak references, as before.
        5. Errors: a native that fails throws (cxx_OOJSReport*Error) and returns
           JS_EXCEPTION. Code calling into JS from C++ reports a pending
           exception with OOJSReportPendingException().
        6. Property IDs (JSAtom) from OOJSID() and cxx_OOJSIDFromString() are
           interned for the life of the process and are never freed.

        There is one runtime for the life of the process. Resetting the engine
        replaces the context (and so the global object and all prototypes), but
        class IDs and atoms survive.
*/

#pragma once

#include <quickjs.h>

#include <cstdarg>
#include <optional>
#include <string>
#include <string_view>

#include "OOFunctionAttributes.h"
#include "OOJSPropID.h"
#include "OOTypes.h"
#include "oofnd/PList.hpp"
#include "oofnd/Ref.hpp"
#include "oofnd/StdLib.hpp"
#include "oofnd/objc/OOAssert.h"
#include "oofnd/objc/OOFoundationTypes.h"

// Number of entries in a JSCFunctionListEntry (or other) array.
#define OOJS_COUNT(array) (sizeof(array) / sizeof *(array))

// There is only one context, created on the main thread.
OOINLINE JSContext* OOJSAcquireContext(void) {
  extern JSContext* gOOJSMainThreadContext;
  OOCAssert(gOOJSMainThreadContext != NULL,
            "Attempt to use JavaScript context before JavaScript engine is "
            "initialized.");
  return gOOJSMainThreadContext;
}

// QuickJS has no requests; kept so acquire/relinquish pairs stay balanced.
OOINLINE void OOJSRelinquishContext(JSContext* /*context*/) {}

/*	Notifications sent when JavaScript engine is reset, on
   oo::NotificationCenter (oofnd/Notification.hpp), posted with
   the engine as the object: will-reset before the context is destroyed,
   did-reset after it is recreated.
*/
extern const char* const kOOJavaScriptEngineWillResetNotificationName;
extern const char* const kOOJavaScriptEngineDidResetNotificationName;

/*	Engine-neutral names (#780), so code outside src/Core/Scripting does not
   name the engine's own calls for these common jobs.
*/
#define OOJSFreeValue(context, value) JS_FreeValue((context), (value))
#define OOJSUndefinedValue() JS_UNDEFINED
#define OOJSNullValue() JS_NULL
OOINLINE bool OOJSValueIsUndefined(JSValueConst value) {
  return JS_IsUndefined(value);
}
OOINLINE JSValue OOJSValueFromDouble(JSContext* context, double value) {
  return JS_NewNumber(context, value);
}
// JavaScript truthiness (like JS_ValueToBoolean()); false if conversion throws.
bool OOJSValueToBOOL(JSContext* context, JSValueConst value);

/*	OOJSValueToInt32(): replacement for SpiderMonkey's JS_ValueToInt32(),
        which converted to a number and rounded to nearest, failing with an
   error if the result was NaN or out of range. (QuickJS's JS_ToInt32() is ECMA
        ToInt32, which truncates and wraps silently, like
   JS_ValueToECMAInt32().) On failure an exception is pending.
*/
bool OOJSValueToInt32(JSContext* context, JSValueConst value,
                      int32_t* outValue);

/*	Script source wrapping.

        SpiderMonkey ran each script with its script object at the head of the
        scope chain, so a bare identifier resolved to a property of the script
        object before the global object (many OXPs rely on this). QuickJS has no
        API for that, but a sloppy-mode with statement provides the same scope
        chain, and strict-mode code nested inside it remains legal. The wrapper
        stays on the first line so line numbers are unchanged.

        Top-level var declarations become local to the script instead of being
        added to the global object (JSOPTION_VAROBJFIX). No known OXP depends
        on that, and it prevents scripts clobbering each other's globals.

        Anything prepended to a script (such as the debug-build "use strict"
        directive) must be added before wrapping: a with statement is a syntax
        error in strict code.
*/
std::string OOJSWrapScriptSource(const std::string& source);

// Report and clear a pending exception, as SpiderMonkey's error reporter did.
void OOJSReportPendingException(JSContext* context);

/*	Error and warning reporters.

        The error reporters throw an Error. The caller must return JS_EXCEPTION
        (or false from a bool helper whose caller returns JS_EXCEPTION).
        Warnings are logged with the current script location and do not throw.
        The cxx_ forms take printf formats (no %@: pass an object's text as %s
   with oo::DescriptionOf), formatted by oo::str::vformat. A nullopt function
   means no caller prefix; a nullopt scriptClass prefixes "function: ".
*/
void cxx_OOJSReportError(JSContext* context, const char* format, ...)
    OO_PRINTF_FORMAT(2, 3);
void cxx_OOJSReportErrorWithArguments(JSContext* context, const char* format,
                                      va_list args) OO_PRINTF_FORMAT(2, 0);
void cxx_OOJSReportErrorForCaller(JSContext* context,
                                  const std::optional<std::string>& scriptClass,
                                  const std::optional<std::string>& function,
                                  const char* format, ...)
    OO_PRINTF_FORMAT(4, 5);

void cxx_OOJSReportWarning(JSContext* context, const char* format, ...)
    OO_PRINTF_FORMAT(2, 3);
void cxx_OOJSReportWarningWithArguments(JSContext* context, const char* format,
                                        va_list args) OO_PRINTF_FORMAT(2, 0);
void cxx_OOJSReportWarningForCaller(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, const char* format, ...)
    OO_PRINTF_FORMAT(4, 5);

// Name of the property with the given magic (tinyid) in a property table, for
// error messages; the magic as a number if no getter/setter entry has it.
std::string OOStringFromJSPropertyMagic(int magic,
                                        const JSCFunctionListEntry* list,
                                        size_t count);

void OOJSReportBadPropertySelector(JSContext* context, JSValueConst thisObj,
                                   int magic, const JSCFunctionListEntry* list,
                                   size_t count);
void OOJSReportBadPropertyValue(JSContext* context, JSValueConst thisObj,
                                int magic, const JSCFunctionListEntry* list,
                                size_t count, JSValueConst value);
// A nullopt message is "Invalid arguments"; a nullopt expectedArgsDescription
// adds no " -- expected ..." suffix.
void cxx_OOJSReportBadArguments(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, int argc, JSValueConst* argv,
    const std::optional<std::string>& message,
    const std::optional<std::string>& expectedArgsDescription);

/*	OOJSNativeFailure()

        SpiderMonkey natives could return false without reporting an error,
        which silently stopped the running script. A QuickJS native must never
        return JS_EXCEPTION without a pending exception: scripts would catch a
        bogus "[uninitialized]" exception. Natives therefore fail with
                return OOJSNativeFailure(context);
        which returns the pending exception if there is one, and otherwise
        throws an uncatchable error that is not logged, stopping the script as
        before. (Directly after cxx_OOJSReportError(), return JS_EXCEPTION is
        fine.)
*/
JSValue OOJSNativeFailure(JSContext* context);

/*	OOJSSetWarningOrErrorStackSkip()

        Indicate that the direct call site is not relevant for error handler.
        Currently, if non-zero, no call site information is provided.
*/
void OOJSSetWarningOrErrorStackSkip(unsigned skip);

/*	cxx_OOJSArgumentListGetNumber()

        Get a single number from an argument list. NaN is treated as failure,
   as before. The optional outConsumed argument can be used to find out how
   many parameters were used (currently, this will be 0 on failure, otherwise
   1).

        On failure, it will return false and raise an error. If the caller is a
   JS callback, it must return JS_EXCEPTION.
*/
bool cxx_OOJSArgumentListGetNumber(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, int argc, JSValueConst* argv,
    double* outNumber, int* outConsumed);

/*	OOJSArgumentListGetNumberNoError()

        Like cxx_OOJSArgumentListGetNumber(), but leaves no pending exception
   on failure.
*/
bool OOJSArgumentListGetNumberNoError(JSContext* context, int argc,
                                      JSValueConst* argv, double* outNumber,
                                      int* outConsumed);

// Typed as int rather than bool to work with more general expressions such as
// bitfield tests.
OOINLINE JSValue OOJSValueFromBOOL(int b) { return JS_NewBool(NULL, b != 0); }

/*	OOJSValueFromPList()
        Return the JavaScript value representation of a property list, owned
   by the caller, exactly as OOJSValueFromNativeObject(context,
   oo::ObjectFromPList(plist)) gave it: null for a null PList; a bool as the
   number 1 or 0; an integer as an int32 when it fits, else a number; a real as
   a number (a single-precision one as the double of its float); a string as a
   string; data and dates undefined; an array as an Array and a dictionary as
   an Object (null elements and values dropped, empty keys skipped, key order);
   a PList::Object node as its C++ object's JS glue.
*/
JSValue OOJSValueFromPList(JSContext* context, const oo::PList& plist);

/**** String utilities ****/

/*	OOJSSTR(const char * [literal])

        An owned string value for a string literal. The string is interned the
        first time the macro is hit.
*/
#define OOJSSTR(str) JS_AtomToString(OOJSAcquireContext(), OOJSID(str))

/*	Convert an arbitrary JS value to a UTF-8 string, calling its toString
   method if needed. cxx_OOStringFromJSValue() returns nullopt if value is null
   or undefined, cxx_OOStringFromJSValueEvenIfNull() returns "null" or
   "undefined". Both return nullopt, with no exception left pending, if the
   conversion throws.
*/
std::optional<std::string> cxx_OOStringFromJSValue(JSContext* context,
                                                   JSValueConst value);
std::optional<std::string> cxx_OOStringFromJSValueEvenIfNull(
    JSContext* context, JSValueConst value);

/*	Describe a value for debugging or error reporting. Strings are quoted,
        escaped and limited in length. Functions are described as "function foo"
        (or just "function" if they're anonymous). Up to four elements of arrays
        are included, followed by total count of there are more than four.
        If abbreviateObjects, the description "[object Object]" is replaced with
        "{...}", which may or may not be clearer depending on context. Never
   empty of meaning: the last fallback is "?".
*/
std::string cxx_OOJSDescribeValue(JSContext* context, JSValueConst value,
                                  bool abbreviateObjects);

// Name of the value's JS class, e.g. "Vector3D", for error messages.
std::string OOJSClassNameOfValue(JSContext* context, JSValueConst value);

// Convert a property ID to a UTF-8 string (nullopt if it has no string value).
std::optional<std::string> cxx_OOStringFromJSID(JSAtom propID);

// Convert a UTF-8 string to a property ID (interned for the life of the
// process).
JSAtom cxx_OOJSIDFromString(const std::string& string);

/*	The string helpers of the retired string category
   (OOJavaScriptExtensions). Each std::optional result is
   nullopt where the category method returned nil.
*/

// For diagnostic messages; produces things like "(42, true, "a string", an
// object description)". nullopt if params is NULL and count is not zero.
std::optional<std::string> cxx_OOJSStringWithJavaScriptParameters(
    JSValueConst* params, int count, JSContext* context);

// Concatenate sequence of arbitrary JS objects into string (nullopt if count <
// 1 or values is NULL).
std::optional<std::string> cxx_OOJSConcatenationOfStringsFromJavaScriptValues(
    JSValueConst* values, size_t count, const std::string& separator,
    JSContext* context);

// Add escape codes for string so that it's a valid JavaScript literal (if you
// put "" or '' around it).
std::string cxx_OOJSEscapedForJavaScriptLiteral(std::string_view string);

/*	cxx_OOJSPListFromJSValue(context, value) /
   cxx_OOJSPListFromJSObject(context, object): the oo::PList form of #768's
   OOJSNativeObjectFromJSValue() family: null for null, undefined or an
   unconvertible value; int32 -> signed integer, other number -> real, boolean
   -> bool, string -> string; a JS Array -> array (a null or undefined element
   -> a PList::Object holding [OONull null]); a plain Object -> dictionary (see
   cxx_OOJSDictionaryFromJSObject(); the global object converts to null); an
   object of a class with a registered converter -> what the converter returns
   (a private object -> a PList::Object node holding it).
*/
oo::PList cxx_OOJSPListFromJSValue(JSContext* context, JSValueConst value);
oo::PList cxx_OOJSPListFromJSObject(JSContext* context, JSValueConst object);

// Class IDs of the standard classes, looked up when the engine starts.
extern JSClassID gOOJSObjectClassID, gOOJSStringClassID, gOOJSArrayClassID,
    gOOJSNumberClassID, gOOJSBooleanClassID;

OOINLINE bool OOJSValueIsFunction(JSContext* context, JSValueConst value) {
  return JS_IsFunction(context, value);
}

OOINLINE bool OOJSValueIsArray(JSContext* /*context*/, JSValueConst value) {
  return JS_IsArray(value);
}

/*	OOJSDictionaryFromJSValue(context, value)
        cxx_OOJSDictionaryFromJSObject(context, object)

        Converts a JavaScript value to a dictionary by calling
        cxx_OOJSPListFromJSValue() on each of its values (a live object such as
        an entity or OONull stays a PList::Object node); a value that converts
   to null is left out.

        Only enumerable own (i.e., not inherited) properties are included; an
        integer-like property is keyed by its decimal text, as JS itself names
   it. A value that is not an object or cannot be enumerated gives a null
   PList. This is also how the native-object family converts a plain JS Object.
*/
oo::PList OOJSDictionaryFromJSValue(JSContext* context, JSValueConst value);
oo::PList cxx_OOJSDictionaryFromJSObject(JSContext* context,
                                         JSValueConst object);

/*	OOJSDictionaryFromStringTable(context, value)

        Treat an arbitrary JavaScript object as a dictionary mapping strings to
        strings, and convert to a corresponding dictionary. The values are
        converted to strings. A null PList if the value is null, undefined or
   cannot be enumerated.

        Only enumerable own (i.e., not inherited) properties with string keys
   are included.
*/
oo::PList OOJSDictionaryFromStringTable(JSContext* context, JSValueConst value);

/*	Classes (OOJSClasses.cpp)

        OOJSRegisterClass() allocates a class ID (once per process; *ioClassID
        must start as 0) and registers the class with the runtime. Prototypes
        are created per context by each class's Init function.

        Oolite reflects part of its class hierarchy as related JS classes whose
        prototypes inherit each other. For instance, JS Entity methods work on
   JS Ships. OOJSRegisterSubclass() records such a relationship, and
        OOJSIsSubclass() tests whether putativeSubclass is equal to superclass
        or a registered subclass of it, recursively.
*/
enum { kOOJSMaxClassID = 1 << 16 };  // QuickJS's limit.

JSClassID OOJSRegisterClass(JSClassID* ioClassID, const JSClassDef* classDef);

// Object.prototype for the current context (owned).
JSValue OOJSObjectPrototype(JSContext* context);
// The class_name of a class registered with OOJSRegisterClass(), else NULL.
const char* OOJSClassName(JSClassID classID);

void OOJSRegisterSubclass(JSClassID subclass, JSClassID superclass);
bool OOJSIsSubclass(JSClassID putativeSubclass, JSClassID superclass);
OOINLINE bool OOJSIsMemberOfSubclass(JSContext* /*context*/,
                                     JSValueConst object,
                                     JSClassID superclass) {
  return JS_IsObject(object) &&
         OOJSIsSubclass(JS_GetClassID(object), superclass);
}

/*	Create the prototype for a class whose instances wrap native objects. As
        with SpiderMonkey's JS_InitClass(), the prototype is itself an instance
        of the class (with no native object), inheriting from parentPrototype
        (Object.prototype if JS_UNDEFINED). If constructor is non-NULL, a global
        constructor function named after the class is also defined.

        The prototype is set as the class prototype for the context; the
        return value is borrowed from there.

        SpiderMonkey reported the properties of a JS_InitClass() prototype
        (JSPROP_SHARED | JSPROP_PERMANENT) as own properties of every instance,
        so Object.keys(), hasOwnProperty() and JSON.stringify() saw them; e.g.
        JSON.stringify(new Vector3D(1, 2, 3)) is {"x":1,"y":2,"z":3}. Classes
        registered with OOJSRegisterClass() and no exotic methods of their own
        reproduce this for the getter/setter properties passed to
   OOJSInitClass(). As in SpiderMonkey, only the object's direct prototype
   counts: a ship's Ship properties are own properties, its Entity properties
   are not. Classes with their own exotic methods can call the functions below
   from theirs to get the same behaviour.
*/
int OOJSGetSharedPermanentOwnProperty(JSContext* context,
                                      JSPropertyDescriptor* desc,
                                      JSValueConst object, JSAtom prop);
int OOJSGetSharedPermanentOwnPropertyNames(JSContext* context,
                                           JSPropertyEnum** ptab,
                                           uint32_t* plen, JSValueConst object);

JSValueConst OOJSInitClass(JSContext* context, JSValueConst global,
                           JSClassID classID, JSValueConst parentPrototype,
                           JSCFunction* constructor, int constructorArgCount,
                           const JSCFunctionListEntry* properties,
                           size_t propertyCount,
                           const JSCFunctionListEntry* methods,
                           size_t methodCount,
                           const JSCFunctionListEntry* staticMethods,
                           size_t staticMethodCount);

/*	Support for the cxx_OOJSPListFromJSValue() family

        OOJSClassConverterCallback specifies the prototype for a callback
   function which converts a JavaScript object to its property-list form: plist
   data, or a PList::Object node holding a native object.

        OOJSBasicPrivateObjectConverter() is a OOJSClassConverterCallback which
        returns the JS object's private storage value as an Object node (a null
        PList for none). The slot holds a C++ object (OOJSPrivateObject.h): a
   node is made of it when it is a PList::Object's foreign object, else none.

        OOJSRegisterObjectConverter() registers a callback for a specific JS
   class. It is not automatically propagated to subclasses.
*/
typedef oo::PList (*OOJSClassConverterCallback)(JSContext* context,
                                                JSValueConst object);
oo::PList OOJSBasicPrivateObjectConverter(JSContext* context,
                                          JSValueConst object);

void OOJSRegisterObjectConverter(JSClassID classID,
                                 OOJSClassConverterCallback converter);

/*	OOJSValue: an object whose purpose in life is to hold a JavaScript
   value. This is somewhat useful for putting JavaScript objects in property
   lists, for instance to pass as properties to script loaders. The value is
   held (with its own reference) until the OOJSValue is destroyed or the engine
   is reset. A property list holds it as an Object node: it is its own node's
   foreign object (OOJSValueObjectNode()).
*/
class OOJSValue : public oo::PListForeign {
 public:
  // context may be NULL (the main thread context is used).
  static oo::Ref<OOJSValue> valueWithJSValue(JSValueConst value,
                                             JSContext* context);

  ~OOJSValue();

  // The held value, owned by the caller (a new reference).
  JSValue jsValueInContext(JSContext* context);

  // oo::PListForeign: its class, and "%@" as <OOJSValue 0x...>.
  std::string className() const override;
  std::string description() const override;

 private:
  // The initialiser after [super init], which could not fail: run by the
  // factory.
  void initWithJSValue(JSValueConst value, JSContext* context);

  void deleteJSValue();

  JSValue _val = JS_UNDEFINED;
  const void* _resetSender = {};  // the engine, the sender of its reset
                                  // notifications, while _val is held
};

// A held value as a PList::Object node (what oo::PListObject() did for the
// facade); null -> null PList.
inline oo::PList OOJSValueObjectNode(OOJSValue* value) {
  if (value == nullptr) return oo::PList();
  return oo::PList(oo::PList::Object(oo::Ref<oo::PListForeign>(value)));
}

#include "OOJSEngineNativeWrappers.h"

/*	Time limiter (OOJavaScriptEngine.cpp)

        The time limiter stops scripts from running an arbitrarily long time.

        The time limiter must be started before calling into JavaScript code.
        Calls to OOJSStartTimeLimiter() and OOJSStopTimeLimiter() must be
   balanced, and may be nested. OOJSStartTimeLimiterWithTimeLimit() is like
   OOJSStartTimeLimiter(), but specifies a custom time limit. This limit is only
   used if the limiter is actually stopped.

        The time limiter can be paused and resumed for native operations that
   are known to be slow. OOJSPauseTimeLimiter() and OOJSResumeTimeLimiter() must
        be balanced and can be nested, but the nest count may be negative - it
   is valid to call OOJSResumeTimeLimiter() followed by OOJSPauseTimeLimiter().

        QuickJS polls an interrupt handler while running JS code, so no
   watchdog thread or operation callback is needed.
*/
#define kOOJSLongTimeLimit (5.0)
void OOJSStartTimeLimiterWithTimeLimit(OOTimeDelta limit);
#define OOJSStartTimeLimiter() OOJSStartTimeLimiterWithTimeLimit(0.0)
void OOJSStopTimeLimiter(void);
void OOJSPauseTimeLimiter(void);
void OOJSResumeTimeLimiter(void);

#ifndef NDEBUG
// Debug console support for the time limiter.
void OOJSResetTimeLimiter(void);
OOTimeDelta OOJSGetTimeLimiterLimit(void);
void OOJSSetTimeLimiterLimit(OOTimeDelta limit);
#endif

/*	OOJSDumpStack()
        Write the current JavaScript stack to the log. QuickJS provides only
        function names and locations, not local variables.
*/
#if OOLITE_DEBUG || defined(OO_DEBUG_MONITOR_ENABLED)
void OOJSDumpStack(JSContext* context);
#else
#define OOJSDumpStack(cx) \
  do {                    \
  } while (0)
#endif

/***** Native function and property callback conveniences *****/

/*	Native functions are declared as
                static JSValue Foo(OOJS_NATIVE_ARGS);
        and use OOJS_THIS, OOJS_ARGV and argc. Property callbacks are declared
   as
                static JSValue FooGetProperty(OOJS_GETTER_ARGS);
                static JSValue FooSetProperty(OOJS_SETTER_ARGS);
        and switch on magic (the old tinyid); they too read their receiver as
   OOJS_THIS. A setter receives its value as value and returns JS_UNDEFINED on
   success. (#768 names the receiver of a property callback this, which C++
   reserves.)
*/
#define OOJS_NATIVE_ARGS \
  JSContext *context, JSValueConst oojsThis, int argc, JSValueConst *oojsArgv
#define OOJS_GETTER_ARGS JSContext *context, JSValueConst oojsThis, int magic
#define OOJS_SETTER_ARGS \
  JSContext *context, JSValueConst oojsThis, JSValueConst value, int magic
#define OOJS_THIS oojsThis
#define OOJS_ARGV oojsArgv

/*	Replacement for SpiderMonkey's JS_IsConstructing(), valid only in
        constructors created by OOJSInitClass() (JS_CFUNC_constructor_or_func):
        QuickJS passes new.target as this when called with new, and undefined
        when called as a function.
*/
#define OOJS_IS_CONSTRUCTING (!JS_IsUndefined(OOJS_THIS))

/***** Reusable JS callbacks ****/

/*	OOJSUnconstructableConstruct

        Constructor callback for pseudo-classes which can't be constructed.
*/
JSValue OOJSUnconstructableConstruct(OOJS_NATIVE_ARGS);

/*	OOJSObjectWrapperFinalize

        Finalizer for JS classes whose private storage is a retained C++
   object: OOJSCxxObjectWrapperFinalize() (OOJSPrivateObject.h).
*/
void OOJSObjectWrapperFinalize(JSRuntime* runtime, JSValueConst thisObj);

/*	OOJSObjectWrapperToString

        Implementation of toString() for JS classes whose private storage is an
        object reference: "[object <JS class name>]" ("[object]" for no class).
        A class whose slot holds a C++ object that describes itself uses
   OOJSCxxObjectWrapperToString() (OOJSPrivateObject.h) instead.
*/
JSValue OOJSObjectWrapperToString(OOJS_NATIVE_ARGS);

#define OOJS_RETURN(v) \
  do {                 \
    return (v);        \
  } while (0)
#define OOJS_RETURN_VOID OOJS_RETURN(JS_UNDEFINED)
#define OOJS_RETURN_NULL OOJS_RETURN(JS_NULL)
#define OOJS_RETURN_BOOL(v) OOJS_RETURN(OOJSValueFromBOOL(v))
#define OOJS_RETURN_INT(v) OOJS_RETURN(JS_NewInt32(context, (v)))
#define OOJS_RETURN_DOUBLE(v) OOJS_RETURN(JS_NewFloat64(context, (v)))

/*	PList return slots. A native returns C++
   values as oo::PList, converted by OOJSValueFromPList() (the same JS value
        OOJSValueFromNativeObject() gave for the Foundation object it replaces):

        OOJS_RETURN_PLIST(plist)            the value of <plist> (null for a
   null PList) OOJS_RETURN_STRING_OR_NULL(opt)     a std::optional<std::string>:
   the string, or null for nullopt (what nil gave)
*/
#define OOJS_RETURN_PLIST(plist) \
  OOJS_RETURN(OOJSValueFromPList(context, (plist)))
#define OOJS_RETURN_STRING_OR_NULL(opt)                               \
  do {                                                                \
    const std::optional<std::string>& oojsOptionalString_ = (opt);    \
    if (!oojsOptionalString_.has_value()) OOJS_RETURN_NULL;           \
    OOJS_RETURN(JS_NewStringLen(context, oojsOptionalString_->data(), \
                                oojsOptionalString_->size()));        \
  } while (0)

#define OOJS_RETURN_WITH_HELPER(helper, value) \
  do {                                         \
    JSValue oojsRVal;                          \
    if (!helper(context, (value), &oojsRVal))  \
      return OOJSNativeFailure(context);       \
    return oojsRVal;                           \
  } while (0)

#define OOJS_RETURN_VECTOR(value) \
  OOJS_RETURN_WITH_HELPER(VectorToJSValue, value)
#define OOJS_RETURN_HPVECTOR(value) \
  OOJS_RETURN_WITH_HELPER(HPVectorToJSValue, value)
#define OOJS_RETURN_QUATERNION(value) \
  OOJS_RETURN_WITH_HELPER(QuaternionToJSValue, value)

/***** Property attributes, equivalent to the SpiderMonkey OOJS_PROP_* flags
 * *****/

// Callback-based (JS_CGETSET_MAGIC_DEF2). Read-only properties are defined
// without a setter.
#define OOJS_PROP_READWRITE_CB JS_PROP_ENUMERABLE
#define OOJS_PROP_READONLY_CB JS_PROP_ENUMERABLE
#define OOJS_PROP_HIDDEN_READWRITE_CB 0
#define OOJS_PROP_HIDDEN_READONLY_CB 0

// Slot-based (JS_DefinePropertyValue*).
#define OOJS_PROP_READWRITE (JS_PROP_ENUMERABLE | JS_PROP_WRITABLE)
#define OOJS_PROP_READONLY JS_PROP_ENUMERABLE
#define OOJS_PROP_HIDDEN_READWRITE JS_PROP_WRITABLE
#define OOJS_PROP_HIDDEN_READONLY 0
#define OOJS_METHOD_READONLY OOJS_PROP_HIDDEN_READONLY

// Property definition shorthands for the common cases.
#define OOJS_PROPERTY_RW(name, getter, setter, magic) \
  JS_CGETSET_MAGIC_DEF2(name, getter, setter, magic, OOJS_PROP_READWRITE_CB)
#define OOJS_PROPERTY_RO(name, getter, magic) \
  JS_CGETSET_MAGIC_DEF2(name, getter, NULL, magic, OOJS_PROP_READONLY_CB)
