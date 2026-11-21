/*

OOJSConsole.m (QuickJS-ng)


Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

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

#ifdef OO_DEBUG_MONITOR_ENABLED

#include "JSConsole.h"

#include <stdint.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "ConstToString.h"
#include "DebugFlags.h"
#include "DebugMonitor.h"
#include "Entity.h"                        // an entity's -inspect
#include "EntityOOJavaScriptExtensions.h"  // IWYU pragma: keep: used only in OO_DEBUG builds
#include "LogHeader.h"                     // OOPlatformDescription()
#include "LoggingExtended.h"
#include "OOJSCall.h"
#include "OOJSEngineTimeManagement.h"
#include "OOJSEntity.h"
#include "OOJSScript.h"
#include "OpenGLExtensionManager.h"
#include "ResourceManager.h"  // IWYU pragma: keep: used only in DEBUG builds
#include "oofnd/AutoreleaseScope.hpp"  // IWYU pragma: keep: used only with OOJS_PROFILE
#include "oofnd/PList.hpp"
#if OO_DEBUG
#include "ShaderUniformMethodType.h"  // callObjC()'s scalar results on the console
#include "ValueType.h"
#endif

#include "oofnd/String.hpp"

// inspectEntity() calls the entity's virtual inspect() (oo::InspectEntity()),
// which does nothing unless a subclass overrides it.

/*	Differences from the SpiderMonkey version:
        - pedanticMode is remembered but has no effect: SpiderMonkey's
          JSOPTION_STRICT extra warnings have no QuickJS equivalent.
        - dumpNamedRoots() does not exist (QuickJS has no named roots), and
          dumpHeap() writes QuickJS's memory usage summary.
        - profile(), getProfile() and trace() only profile natives, as in
          SpiderMonkey builds without MOZ_TRACE_JSCALLS.
        - ConsoleSettings properties are provided by the class's exotic methods
          rather than get/set/delete hooks. As before, they are not enumerable.
*/

static JSClassID sConsoleClassID;
static JSClassID sConsoleSettingsClassID;
static bool sPedanticMode = true;

namespace {
oo::PList ConsoleConverter(JSContext* context, JSValueConst object);
}  // namespace

static JSValue ConsoleGetProperty(OOJS_GETTER_ARGS);
static JSValue ConsoleSetProperty(OOJS_SETTER_ARGS);

// Methods
static JSValue ConsoleConsoleMessage(OOJS_NATIVE_ARGS);
static JSValue ConsoleClearConsole(OOJS_NATIVE_ARGS);
static JSValue ConsoleScriptStack(OOJS_NATIVE_ARGS);
static JSValue ConsoleInspectEntity(OOJS_NATIVE_ARGS);
#if OO_DEBUG
static JSValue ConsoleCallObjCMethod(OOJS_NATIVE_ARGS);
static JSValue ConsoleSetUpCallObjC(OOJS_NATIVE_ARGS);
#endif
static JSValue ConsoleIsExecutableJavaScript(OOJS_NATIVE_ARGS);
static JSValue ConsoleDisplayMessagesInClass(OOJS_NATIVE_ARGS);
static JSValue ConsoleSetDisplayMessagesInClass(OOJS_NATIVE_ARGS);
static JSValue ConsoleWriteLogMarker(OOJS_NATIVE_ARGS);
static JSValue ConsoleWriteMemoryStats(OOJS_NATIVE_ARGS);
static JSValue ConsoleWriteJSMemoryStats(OOJS_NATIVE_ARGS);
static JSValue ConsoleGarbageCollect(OOJS_NATIVE_ARGS);
#if DEBUG
static JSValue ConsoleDumpHeap(OOJS_NATIVE_ARGS);
#endif
#if OOJS_PROFILE
static JSValue ConsoleProfile(OOJS_NATIVE_ARGS);
static JSValue ConsoleGetProfile(OOJS_NATIVE_ARGS);
static JSValue ConsoleTrace(OOJS_NATIVE_ARGS);

namespace {
bool PerformProfiling(JSContext* context, const char* nominalFunction, int argc,
                      JSValueConst* argv, JSValue* outRval, bool trace,
                      std::shared_ptr<OOTimeProfile>* outProfile);
}  // namespace
#endif

static int ConsoleSettingsGetOwnProperty(JSContext* context,
                                         JSPropertyDescriptor* desc,
                                         JSValueConst object, JSAtom propID);
static int ConsoleSettingsDeleteProperty(JSContext* context,
                                         JSValueConst object, JSAtom propID);
static int ConsoleSettingsDefineOwnProperty(JSContext* context,
                                            JSValueConst object, JSAtom propID,
                                            JSValueConst value,
                                            JSValueConst getter,
                                            JSValueConst setter, int flags);
static JSValue ConsoleSettingsGetProperty(JSContext* context,
                                          JSValueConst object, JSAtom propID,
                                          JSValueConst receiver);
static int ConsoleSettingsSetProperty(JSContext* context, JSValueConst object,
                                      JSAtom propID, JSValueConst value,
                                      JSValueConst receiver, int flags);

/*	The console objects' private slot holds the C++ monitor, shared
   (OOJSSetCxxPrivate()), released by the finalizer, which tells the monitor
   its wrapper is gone (OODebugMonitor::clearJSSelf() does nothing). The monitor
   is never released, so the slot keeps nothing alive that would otherwise die.
*/
static JSClassDef sConsoleClass = {
    .class_name = "Console",
    .finalizer = OOJSCxxObjectWrapperFinalize,
};

namespace {

enum class ConsoleProperty : std::int16_t {
  // Property IDs
  kDebugFlags,   // debug flags, integer, read/write
  kDetailLevel,  // graphics detail level, symbolic string, read/write
  // maximum graphics detail level, symbolic string, read-only
  kMaximumDetailLevel,
  kDisplayFPS,  // display FPS (and related info), boolean, read/write
  // Information about system we're running on in unspecified format, string,
  // read-only
  kPlatformDescription,
  kIgnoreDroppedPackets,  // boolean (default false), read/write
  // JS pedantic mode (JS_STRICT flag, not the same as "use strict"), boolean
  // (default true), read/write
  kPedanticMode,
  // Show error/warning source locations, boolean (default true), read/write
  kShowErrorLocations,
  // Write stack dump when reporting error/exception, boolean (default false),
  // read/write
  kDumpStackForErrors,
  // Write stack dump when reporting warning, boolean (default false),
  // read/write
  kDumpStackForWarnings,

  kGlVendorString,    // OpenGL GL_VENDOR string, string, read-only
  kGlRendererString,  // OpenGL GL_RENDERER string, string, read-only
  // GL_MAX_TEXTURE_UNITS_ARB, integer, read-only
  kGlFixedFunctionTextureUnitCount,
  // GL_MAX_TEXTURE_IMAGE_UNITS_ARB, integer, read-only
  kGlFragmentShaderTextureUnitCount,

  // Symbolic constants for debug flags:
  kDebugLinkedLists,
  kDebugCollisions,
  kDebugDocking,
  kDebugOctreeLogging,
  kDebugBoundingBoxes,
  kDebugOctreeDraw,
  kDebugDrawNormals,
  kDebugNoDust,
  kDebugNoShaderFallback,
  kDebugShaderValidation,

  kDebugMisc
};

}  // namespace

#define OOJS_PROPERTY_HIDDEN_RW(name, getter, setter, magic)            \
  JS_CGETSET_MAGIC_DEF2(name, getter, setter, OOJSPropertyMagic(magic), \
                        OOJS_PROP_HIDDEN_READWRITE_CB)

// A flag's JS property name as the C string a property entry holds.
// OOEnumName's names are string literals, so the view's data is
// null-terminated.
static constexpr const char* DebugFlagPropertyName(oo::DebugFlags flag) {
  return OOEnumName(flag).data();
}

static const JSCFunctionListEntry sConsoleProperties[] = {
    // JS name								Getter
    // Setter				ID
    OOJS_PROPERTY_RW("debugFlags", ConsoleGetProperty, ConsoleSetProperty,
                     ConsoleProperty::kDebugFlags),
    OOJS_PROPERTY_RW("detailLevel", ConsoleGetProperty, ConsoleSetProperty,
                     ConsoleProperty::kDetailLevel),
    OOJS_PROPERTY_RO("maximumDetailLevel", ConsoleGetProperty,
                     ConsoleProperty::kMaximumDetailLevel),
    OOJS_PROPERTY_RW("displayFPS", ConsoleGetProperty, ConsoleSetProperty,
                     ConsoleProperty::kDisplayFPS),
    OOJS_PROPERTY_RO("platformDescription", ConsoleGetProperty,
                     ConsoleProperty::kPlatformDescription),
    OOJS_PROPERTY_RW("pedanticMode", ConsoleGetProperty, ConsoleSetProperty,
                     ConsoleProperty::kPedanticMode),
    OOJS_PROPERTY_RW("ignoreDroppedPackets", ConsoleGetProperty,
                     ConsoleSetProperty,
                     ConsoleProperty::kIgnoreDroppedPackets),
    OOJS_PROPERTY_HIDDEN_RW("__showErrorLocations", ConsoleGetProperty,
                            ConsoleSetProperty,
                            ConsoleProperty::kShowErrorLocations),
    OOJS_PROPERTY_HIDDEN_RW("__dumpStackForErrors", ConsoleGetProperty,
                            ConsoleSetProperty,
                            ConsoleProperty::kDumpStackForErrors),
    OOJS_PROPERTY_HIDDEN_RW("__dumpStackForWarnings", ConsoleGetProperty,
                            ConsoleSetProperty,
                            ConsoleProperty::kDumpStackForWarnings),
    OOJS_PROPERTY_RO("glVendorString", ConsoleGetProperty,
                     ConsoleProperty::kGlVendorString),
    OOJS_PROPERTY_RO("glRendererString", ConsoleGetProperty,
                     ConsoleProperty::kGlRendererString),
    OOJS_PROPERTY_RO("glFixedFunctionTextureUnitCount", ConsoleGetProperty,
                     ConsoleProperty::kGlFixedFunctionTextureUnitCount),
    OOJS_PROPERTY_RO("glFragmentShaderTextureUnitCount", ConsoleGetProperty,
                     ConsoleProperty::kGlFragmentShaderTextureUnitCount),

// Each flag's name is its OOEnumName, as console.DEBUG_* always was.
#define DEBUG_FLAG_DECL(flag, magic)                            \
  OOJS_PROPERTY_RO(DebugFlagPropertyName(oo::DebugFlags::flag), \
                   ConsoleGetProperty, magic)
    DEBUG_FLAG_DECL(kLinkedLists, ConsoleProperty::kDebugLinkedLists),
    DEBUG_FLAG_DECL(kCollisions, ConsoleProperty::kDebugCollisions),
    DEBUG_FLAG_DECL(kDocking, ConsoleProperty::kDebugDocking),
    DEBUG_FLAG_DECL(kOctreeLogging, ConsoleProperty::kDebugOctreeLogging),
    DEBUG_FLAG_DECL(kBoundingBoxes, ConsoleProperty::kDebugBoundingBoxes),
    DEBUG_FLAG_DECL(kOctreeDraw, ConsoleProperty::kDebugOctreeDraw),
    DEBUG_FLAG_DECL(kDrawNormals, ConsoleProperty::kDebugDrawNormals),
    DEBUG_FLAG_DECL(kNoDust, ConsoleProperty::kDebugNoDust),
    DEBUG_FLAG_DECL(kNoShaderFallback, ConsoleProperty::kDebugNoShaderFallback),
    DEBUG_FLAG_DECL(kShaderValidation, ConsoleProperty::kDebugShaderValidation),

    DEBUG_FLAG_DECL(kMisc, ConsoleProperty::kDebugMisc),
#undef DEBUG_FLAG_DECL
};

static const JSCFunctionListEntry sConsoleMethods[] = {
    // JS name							Function
    // min args
    JS_CFUNC_DEF("consoleMessage", 2, ConsoleConsoleMessage),
    JS_CFUNC_DEF("clearConsole", 0, ConsoleClearConsole),
    JS_CFUNC_DEF("scriptStack", 0, ConsoleScriptStack),
    JS_CFUNC_DEF("inspectEntity", 1, ConsoleInspectEntity),
#if OO_DEBUG
    JS_CFUNC_DEF("__setUpCallObjC", 1, ConsoleSetUpCallObjC),
#endif
    JS_CFUNC_DEF("isExecutableJavaScript", 2, ConsoleIsExecutableJavaScript),
    JS_CFUNC_DEF("displayMessagesInClass", 1, ConsoleDisplayMessagesInClass),
    JS_CFUNC_DEF("setDisplayMessagesInClass", 2,
                 ConsoleSetDisplayMessagesInClass),
    JS_CFUNC_DEF("writeLogMarker", 0, ConsoleWriteLogMarker),
    JS_CFUNC_DEF("writeMemoryStats", 0, ConsoleWriteMemoryStats),
    JS_CFUNC_DEF("writeJSMemoryStats", 0, ConsoleWriteJSMemoryStats),
    JS_CFUNC_DEF("garbageCollect", 0, ConsoleGarbageCollect),
#if DEBUG
    JS_CFUNC_DEF("dumpHeap", 0, ConsoleDumpHeap),
#endif
#if OOJS_PROFILE
    JS_CFUNC_DEF("profile", 1, ConsoleProfile),
    JS_CFUNC_DEF("getProfile", 1, ConsoleGetProfile),
    JS_CFUNC_DEF("trace", 1, ConsoleTrace),
#endif
};

static JSClassExoticMethods sConsoleSettingsExoticMethods = {
    .get_own_property = ConsoleSettingsGetOwnProperty,
    .delete_property = ConsoleSettingsDeleteProperty,
    .define_own_property = ConsoleSettingsDefineOwnProperty,
    .get_property = ConsoleSettingsGetProperty,
    .set_property = ConsoleSettingsSetProperty,
};

static JSClassDef sConsoleSettingsClass = {
    .class_name = "ConsoleSettings",
    .finalizer = OOJSCxxObjectWrapperFinalize,  // (same as Console)
    .exotic = &sConsoleSettingsExoticMethods,
};

static void InitOOJSConsole(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sConsoleClassID, &sConsoleClass);
  OOJSInitClass(context, global, sConsoleClassID, JS_UNDEFINED,
                OOJSUnconstructableConstruct, 0, sConsoleProperties,
                OOJS_COUNT(sConsoleProperties), sConsoleMethods,
                OOJS_COUNT(sConsoleMethods), nullptr, 0);
  OOJSRegisterObjectConverter(sConsoleClassID, ConsoleConverter);

  OOJSRegisterClass(&sConsoleSettingsClassID, &sConsoleSettingsClass);
  OOJSInitClass(context, global, sConsoleSettingsClassID, JS_UNDEFINED,
                OOJSUnconstructableConstruct, 0, nullptr, 0, nullptr, 0,
                nullptr, 0);
  OOJSRegisterObjectConverter(sConsoleSettingsClassID, ConsoleConverter);
}

namespace {

// The monitor of a Console or ConsoleSettings object; null for any other object
// (or a prototype).
oo::DebugMonitor* MonitorFromJSObject(JSValueConst object) {
  if (!JS_IsObject(object)) return nullptr;
  const JSClassID classID = JS_GetClassID(object);
  if (classID == 0 ||
      (classID != sConsoleClassID && classID != sConsoleSettingsClassID))
    return nullptr;
  return static_cast<oo::DebugMonitor*>(
      OOJSCxxPrivateFromOpaque(JS_GetOpaque(object, classID)));
}

/*	The natives' internal error for a `this` that is not a console object:
   the class of the object the engine converts it to ("(null)" for a plain
   object).
*/
void ReportNotTheMonitor(JSContext* context, JSValueConst object,
                         const char* function) {
  const oo::PList node = OOJSPListFromJSValue(context, object);
  // An entity's node is described as "Entity"; any other node as "(null)".
  const std::string className = (oo::EntityIn(node) != nullptr)
                                    ? std::string("Entity")
                                    : std::string("(null)");
  OOJSReportError(context, "Expected OODebugMonitor, got %s in %s. %s",
                  className.c_str(), function,
                  "This is an internal error, please report it.");
}

// The monitor the private slot holds; null, with the internal error reported,
// for any other object.
oo::DebugMonitor* MonitorForConsoleObject(JSContext* context,
                                          JSValueConst object,
                                          const char* function) {
  oo::DebugMonitor* monitor = MonitorFromJSObject(object);
  if (monitor == nullptr) ReportNotTheMonitor(context, object, function);
  return monitor;
}

// The console objects' converter: the monitor as an Object node
// (oo::PListForeign), whose JS value is the console object; null for none.
oo::PList ConsoleConverter(JSContext* /*context*/, JSValueConst object) {
  oo::DebugMonitor* monitor = MonitorFromJSObject(object);
  if (monitor == nullptr) return oo::PList();
  return oo::PList(oo::PList::Object(oo::share(monitor)));
}

}  // namespace

JSValue DebugMonitorToJSConsole(JSContext* context, oo::DebugMonitor* monitor) {
  OOJS_PROFILE_SCOPE();

  // The classes belong to the context, so are set up again after every reset.
  JSValue consolePrototype = (sConsoleClassID != 0)
                                 ? JS_GetClassProto(context, sConsoleClassID)
                                 : JS_NULL;
  if (!JS_IsObject(consolePrototype)) {
    JS_FreeValue(context, consolePrototype);
    InitOOJSConsole(context,
                    OOJavaScriptEngine::sharedEngine()->globalObject());
    consolePrototype = JS_GetClassProto(context, sConsoleClassID);
  }

  // Create Console object
  JSValue object =
      JS_NewObjectProtoClass(context, consolePrototype, sConsoleClassID);
  JS_FreeValue(context, consolePrototype);
  if (JS_IsException(object)) {
    OOJSReportPendingException(context);
    return JS_NULL;
  }
  if (!OOJSSetCxxPrivate(object, oo::share(monitor))) {
    JS_FreeValue(context, object);
    return JS_NULL;
  }

  // Create ConsoleSettings object
  JSValue settingsPrototype =
      JS_GetClassProto(context, sConsoleSettingsClassID);
  JSValue settingsObject = JS_NewObjectProtoClass(context, settingsPrototype,
                                                  sConsoleSettingsClassID);
  JS_FreeValue(context, settingsPrototype);
  if (JS_IsException(settingsObject)) {
    OOJSReportPendingException(context);
    JS_FreeValue(context, object);
    return JS_NULL;
  }
  if (!OOJSSetCxxPrivate(settingsObject, oo::share(monitor))) {
    JS_FreeValue(context, settingsObject);
    JS_FreeValue(context, object);
    return JS_NULL;
  }

  if (JS_SetPropertyStr(context, object, "settings", settingsObject) < 0) {
    OOJSReportPendingException(context);
    JS_FreeValue(context, object);
    return JS_NULL;
  }

  return object;
}

static JSValue ConsoleGetProperty(OOJS_GETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  switch (static_cast<ConsoleProperty>(magic)) {
    using enum ConsoleProperty;
#ifdef OO_DEBUG_MONITOR_ENABLED
    case kDebugFlags:
      return JS_NewUint32(context, static_cast<uint32_t>(gDebugFlags));
#endif

    case kDetailLevel:
      OOJS_RETURN_PLIST(oo::PList(oo::StringFromGraphicsDetail(
          (UNIVERSE != nullptr ? UNIVERSE->getDetailLevel()
                               : oo::GraphicsDetail{}))));

    case kMaximumDetailLevel:
      OOJS_RETURN_PLIST(oo::PList(oo::StringFromGraphicsDetail(
          oo::OpenGLExtensionManager::sharedManager()->maximumDetailLevel())));

    case kDisplayFPS:
      return OOJSValueFromBOOL(
          (UNIVERSE != nullptr ? UNIVERSE->getDisplayFPS() : false));

    case kPlatformDescription:
      OOJS_RETURN_PLIST(oo::PList(oo::PlatformDescription()));

    case kPedanticMode:
      return OOJSValueFromBOOL(sPedanticMode);

    case kIgnoreDroppedPackets:
      return OOJSValueFromBOOL(
          oo::DebugMonitor::sharedDebugMonitor()->TCPIgnoresDroppedPackets());

    case kShowErrorLocations:
      return OOJSValueFromBOOL(
          OOJavaScriptEngine::sharedEngine()->showErrorLocations());

    case kDumpStackForErrors:
      return OOJSValueFromBOOL(
          OOJavaScriptEngine::sharedEngine()->dumpStackForErrors());

    case kDumpStackForWarnings:
      return OOJSValueFromBOOL(
          OOJavaScriptEngine::sharedEngine()->dumpStackForWarnings());

    case kGlVendorString:
      OOJS_RETURN_STRING_OR_NULL(
          oo::OpenGLExtensionManager::sharedManager()->vendorString());

    case kGlRendererString:
      OOJS_RETURN_STRING_OR_NULL(
          oo::OpenGLExtensionManager::sharedManager()->rendererString());

    case kGlFixedFunctionTextureUnitCount:
      return JS_NewInt32(
          context,
          oo::OpenGLExtensionManager::sharedManager()->textureUnitCount());

    case kGlFragmentShaderTextureUnitCount:
      return JS_NewInt32(
          context,
          oo::OpenGLExtensionManager::sharedManager()->textureImageUnitCount());

#define DEBUG_FLAG_CASE(flag, magic)                  \
  case magic:                                         \
    return JS_NewInt32(context, static_cast<int32_t>( \
                                    oo::ToUnderlying(oo::DebugFlags::flag)));
      DEBUG_FLAG_CASE(kLinkedLists, kDebugLinkedLists);
      DEBUG_FLAG_CASE(kCollisions, kDebugCollisions);
      DEBUG_FLAG_CASE(kDocking, kDebugDocking);
      DEBUG_FLAG_CASE(kOctreeLogging, kDebugOctreeLogging);
      DEBUG_FLAG_CASE(kBoundingBoxes, kDebugBoundingBoxes);
      DEBUG_FLAG_CASE(kOctreeDraw, kDebugOctreeDraw);
      DEBUG_FLAG_CASE(kDrawNormals, kDebugDrawNormals);
      DEBUG_FLAG_CASE(kNoDust, kDebugNoDust);
      DEBUG_FLAG_CASE(kNoShaderFallback, kDebugNoShaderFallback);
      DEBUG_FLAG_CASE(kShaderValidation, kDebugShaderValidation);

      DEBUG_FLAG_CASE(kMisc, kDebugMisc);
#undef DEBUG_FLAG_CASE

    default:
      OOJSReportBadPropertySelector(context, thisObj, magic, sConsoleProperties,
                                    OOJS_COUNT(sConsoleProperties));
      return OOJSNativeFailure(context);
  }

  OOJS_NATIVE_EXIT
}

static JSValue ConsoleSetProperty(OOJS_SETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  int32_t iValue;
  std::optional<std::string> sValue;

  switch (static_cast<ConsoleProperty>(magic)) {
    using enum ConsoleProperty;
#ifdef OO_DEBUG_MONITOR_ENABLED
    case kDebugFlags:
      if (OOJSValueToInt32(context, value, &iValue)) {
        gDebugFlags = static_cast<std::uint32_t>(iValue);
      }
      break;
#endif
    case kDetailLevel:
      sValue = OOStringFromJSValue(context, value);
      OOJS_BEGIN_FULL_NATIVE(context)
      if (UNIVERSE != nullptr)
        UNIVERSE->setDetailLevel(
            oo::GraphicsDetailFromString(sValue.value_or("")));
      OOJS_END_FULL_NATIVE
      break;

    case kDisplayFPS:
      if (UNIVERSE != nullptr)
        UNIVERSE->setDisplayFPS(OOJSValueToBOOL(context, value));
      break;

    case kPedanticMode:
      sPedanticMode = OOJSValueToBOOL(context, value);
      break;

    case kIgnoreDroppedPackets:
      oo::DebugMonitor::sharedDebugMonitor()->setTCPIgnoresDroppedPackets(
          OOJSValueToBOOL(context, value));
      break;

    case kShowErrorLocations:
      OOJavaScriptEngine::sharedEngine()->setShowErrorLocations(
          OOJSValueToBOOL(context, value));
      break;

    case kDumpStackForErrors:
      OOJavaScriptEngine::sharedEngine()->setDumpStackForErrors(
          OOJSValueToBOOL(context, value));
      break;

    case kDumpStackForWarnings:
      OOJavaScriptEngine::sharedEngine()->setDumpStackForWarnings(
          OOJSValueToBOOL(context, value));
      break;

    default:
      OOJSReportBadPropertySelector(context, thisObj, magic, sConsoleProperties,
                                    OOJS_COUNT(sConsoleProperties));
      return OOJSNativeFailure(context);
  }

  return JS_UNDEFINED;

  OOJS_NATIVE_EXIT
}

namespace {

[[maybe_unused]] bool DoWeDefineAllDebugFlags(oo::DebugFlags flags);

bool DoWeDefineAllDebugFlags(oo::DebugFlags flags) {
  /*	This function doesn't do anything, but will generate a warning
          (Enumeration value 'kFoo' not handled in switch) if a debug flag
          is added without updating it. The point is that if you get such a
          warning, you should first add a JS symbolic constant for the flag,
          then add it to the switch to suppress the warning.
          NOTE: don't add a default: to this switch, or I will have to hurt you.
          -- Ahruman 2010-04-11
  */
  switch (flags) {
    case oo::DebugFlags::kLinkedLists:
    case oo::DebugFlags::kCollisions:
    case oo::DebugFlags::kDocking:
    case oo::DebugFlags::kOctreeLogging:
    case oo::DebugFlags::kBoundingBoxes:
    case oo::DebugFlags::kOctreeDraw:
    case oo::DebugFlags::kDrawNormals:
    case oo::DebugFlags::kNoDust:
    case oo::DebugFlags::kNoShaderFallback:
    case oo::DebugFlags::kShaderValidation:
    case oo::DebugFlags::kMisc:
      return true;
    case oo::DebugFlags::kNone:
    case oo::DebugFlags::kAll:
      return false;
  }

  return false;
}

}  // namespace

// *** Settings ***

/*	The ConsoleSettings prototype is itself an instance of the class (see
        OOJSInitClass()), but has no monitor. It has no settings: properties
        defined on it while the class is set up, such as constructor, are
        ignored, and reads fall through to Object.prototype.
*/
static bool IsSettingsPrototype(JSValueConst object) {
  return JS_GetOpaque(object, sConsoleSettingsClassID) == nullptr;
}

// Setting key for a string property ID, or nullopt for other IDs (symbols).
static std::optional<std::string> SettingKeyForPropertyID(JSContext* context,
                                                          JSAtom propID) {
  std::optional<std::string> result;
  JSValue idValue = JS_AtomToValue(context, propID);
  if (JS_IsString(idValue)) {
    result = OOStringFromJSValue(context, idValue);
  }
  JS_FreeValue(context, idValue);
  return result;
}

/*	SpiderMonkey's get hook was called for every read; settings that don't
        exist read as undefined, and settings are not reported as own properties
        (the enumerate hook was a stub), so for-in and Object.keys() see
   nothing.
*/
static int ConsoleSettingsGetOwnProperty(JSContext* /*context*/,
                                         JSPropertyDescriptor* /*desc*/,
                                         JSValueConst /*object*/,
                                         JSAtom /*propID*/) {
  return false;
}

static JSValue ConsoleSettingsGetProperty(JSContext* context,
                                          JSValueConst object, JSAtom propID,
                                          JSValueConst /*receiver*/) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> key = SettingKeyForPropertyID(context, propID);
  if (!key.has_value() || IsSettingsPrototype(object)) {
    // Symbols, such as Symbol.toPrimitive: look them up on the prototype.
    JSValue prototype = JS_GetPrototype(context, object);
    JSValue result = JS_GetProperty(context, prototype, propID);
    JS_FreeValue(context, prototype);
    return result;
  }

  oo::DebugMonitor* monitor =
      MonitorForConsoleObject(context, object, __PRETTY_FUNCTION__);
  if (monitor == nullptr) return OOJSNativeFailure(context);

  const oo::PList setting = monitor->configurationValueForKey(*key);
  if (setting) return OOJSValueFromPList(context, setting);
  return JS_UNDEFINED;

  OOJS_NATIVE_EXIT
}

static int SetSetting(JSContext* context, JSValueConst object, JSAtom propID,
                      JSValueConst value) {
  std::optional<std::string> key = SettingKeyForPropertyID(context, propID);
  if (!key.has_value() || IsSettingsPrototype(object)) return true;

  oo::DebugMonitor* monitor =
      MonitorForConsoleObject(context, object, __PRETTY_FUNCTION__);
  if (monitor == nullptr) return -1;

  // Not OOJS_BEGIN_FULL_NATIVE() - we use JSAPI while paused.
  OOJSPauseTimeLimiter();
  if (JS_IsNull(value) || JS_IsUndefined(value)) {
    monitor->setConfigurationValue(oo::PList(), *key);
  } else {
    const oo::PList settingValue = OOJSPListFromJSValue(context, value);
    if (!settingValue.isNull()) {
      monitor->setConfigurationValue(settingValue, *key);
    } else {
      OOJSReportWarning(
          context,
          "debugConsole.settings: could not convert %s to native object.",
          OOStringFromJSValue(context, value).value_or("(null)").c_str());
    }
  }
  OOJSResumeTimeLimiter();

  return true;
}

static int ConsoleSettingsSetProperty(JSContext* context, JSValueConst object,
                                      JSAtom propID, JSValueConst value,
                                      JSValueConst /*receiver*/,
                                      int /*flags*/) {
  try {
    return SetSetting(context, object, propID, value);
  } catch (...) {
    OOJSReportCurrentException(context);
    return -1;
  }
}

// Object.defineProperty() and similar: treated as assignment of the value.
static int ConsoleSettingsDefineOwnProperty(
    JSContext* context, JSValueConst object, JSAtom propID, JSValueConst value,
    JSValueConst /*getter*/, JSValueConst /*setter*/, int flags) {
  try {
    if (!(flags & JS_PROP_HAS_VALUE)) return true;
    return SetSetting(context, object, propID, value);
  } catch (...) {
    OOJSReportCurrentException(context);
    return -1;
  }
}

static int ConsoleSettingsDeleteProperty(JSContext* context,
                                         JSValueConst object, JSAtom propID) {
  try {
    std::optional<std::string> key = SettingKeyForPropertyID(context, propID);
    if (!key.has_value() || IsSettingsPrototype(object)) return false;

    oo::DebugMonitor* monitor =
        MonitorForConsoleObject(context, object, __PRETTY_FUNCTION__);
    if (monitor == nullptr) return -1;

    monitor->setConfigurationValue(oo::PList(), *key);
    return true;
  } catch (...) {
    OOJSReportCurrentException(context);
    return -1;
  }
}

// *** Methods ***

// function consoleMessage(colorCode : String, message : String [, emphasisStart
// : Number, emphasisLength : Number]) : void
static JSValue ConsoleConsoleMessage(OOJS_NATIVE_ARGS) {
  NSRange emphasisRange = {0, 0};

  OOJS_NATIVE_ENTER(context)

  oo::DebugMonitor* monitor = nullptr;  // the monitor the private slot holds
  std::optional<std::string> colorKey, message;
  double location, length;

  // Not OOJS_BEGIN_FULL_NATIVE() - we use JSAPI while paused.
  OOJSPauseTimeLimiter();
  monitor = MonitorFromJSObject(OOJS_THIS);
  if (monitor == nullptr) {
    // Whatever `this` was, it is described as "(null)".
    OOJSReportError(context, "Expected OODebugMonitor, got %s in %s. %s",
                    "(null)", __PRETTY_FUNCTION__,
                    "This is an internal error, please report it.");
    OOJSResumeTimeLimiter();
    return OOJSNativeFailure(context);
  }

  if (argc > 0) colorKey = OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (argc > 1) message = OOStringFromJSValue(context, OOJS_ARGV[1]);

  if (argc > 3) {
    // Attempt to get two numbers, specifying an emphasis range.
    if (JS_ToFloat64(context, &location, OOJS_ARGV[2]) == 0 &&
        JS_ToFloat64(context, &length, OOJS_ARGV[3]) == 0) {
      emphasisRange = NSRange{static_cast<std::size_t>(location),
                              static_cast<std::size_t>(length)};
    }
  }

  if (!message.has_value()) {
    if (!colorKey.has_value()) {
      OOJSReportWarning(context,
                        "Console.consoleMessage() called with no parameters.");
    } else {
      message = colorKey;
      colorKey = "command-result";
    }
  }

  if (message.has_value()) {
    monitor->appendJSConsoleLine(*message, colorKey, emphasisRange);
  }
  OOJSResumeTimeLimiter();

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

// function clearConsole() : void
static JSValue ConsoleClearConsole(JSContext* context, JSValueConst oojsThis,
                                   int /*argc*/, JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  oo::DebugMonitor* monitor =
      MonitorForConsoleObject(context, OOJS_THIS, __PRETTY_FUNCTION__);
  if (monitor == nullptr) return OOJSNativeFailure(context);

  monitor->clearJSConsole();
  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

// function scriptStack() : Array
static JSValue ConsoleScriptStack(JSContext* context, JSValueConst /*oojsThis*/,
                                  int /*argc*/, JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  // The scripts' nodes (a script that has gone, which a timer or definition
  // pushed weakly, is null).
  oo::PList::Array stack;
  for (const std::shared_ptr<OOJSScript>& script : OOJSScript::scriptStack())
    stack.push_back(OOScriptObjectNode(script.get()));
  OOJS_RETURN_PLIST(oo::PList(std::move(stack)));

  OOJS_NATIVE_EXIT
}

// function inspectEntity(entity : Entity) : void
static JSValue ConsoleInspectEntity(JSContext* context,
                                    JSValueConst /*oojsThis*/, int argc,
                                    JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  Entity* cxxEntity = nullptr;  // the C++ entity

  if (argc > 0 && JSValueToEntity(context, OOJS_ARGV[0], &cxxEntity)) {
    OOJS_BEGIN_FULL_NATIVE(context)
    // -inspect, if the entity has it (nothing for nil)
    oo::InspectEntity(cxxEntity);
    OOJS_END_FULL_NATIVE
  }

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

#if OO_DEBUG
namespace {

/*	callObjC()'s names on the console objects: the debug monitor's methods
   without arguments, and the two that take the joined string argument. Each row
   calls the C++ member of that name. A scalar row keeps the kind of
   the member's return type, which decides its result (or the "cannot be called"
   error), and a row that cannot be called from JavaScript (kNotCallable)
   answers that error. Every other name answers "does not respond".
*/
enum class MonitorMethodForm {
  kVoidVoid,
  kVoidString,
  kPListString,
  kScalar,
  kNotCallable
};

struct MonitorMethod {
  const char* name;
  MonitorMethodForm form;
  void (*voidVoid)(oo::DebugMonitor*);
  void (*voidString)(oo::DebugMonitor*, const std::string&);
  oo::PList (*pListString)(oo::DebugMonitor*, const std::string&);
  std::int64_t (*scalar)(oo::DebugMonitor*);
  oo::ValueType type;  // a kScalar row: the return type's kind, else kVoid
};

const MonitorMethod kMonitorMethods[] = {
    {"performJSConsoleCommand:", MonitorMethodForm::kVoidString, nullptr,
     [](oo::DebugMonitor* m, const std::string& s) {
       m->performJSConsoleCommand(s);
     },
     nullptr, nullptr, oo::ValueType::kVoid},
    {"configurationValueForKey:", MonitorMethodForm::kPListString, nullptr,
     nullptr,
     [](oo::DebugMonitor* m, const std::string& s) {
       return m->configurationValueForKey(s);
     },
     nullptr, oo::ValueType::kVoid},
    {"clearJSConsole", MonitorMethodForm::kVoidVoid,
     [](oo::DebugMonitor* m) { m->clearJSConsole(); }, nullptr, nullptr,
     nullptr, oo::ValueType::kVoid},
    {"showJSConsole", MonitorMethodForm::kVoidVoid,
     [](oo::DebugMonitor* m) { m->showJSConsole(); }, nullptr, nullptr, nullptr,
     oo::ValueType::kVoid},
    {"dumpMemoryStatistics", MonitorMethodForm::kVoidVoid,
     [](oo::DebugMonitor* m) { m->dumpMemoryStatistics(); }, nullptr, nullptr,
     nullptr, oo::ValueType::kVoid},
#if OOLITE_SDL
    {"applicationWillTerminate", MonitorMethodForm::kVoidVoid,
     [](oo::DebugMonitor* m) { m->applicationWillTerminate(); }, nullptr,
     nullptr, nullptr, oo::ValueType::kVoid},
#endif
    {"debuggerConnected", MonitorMethodForm::kScalar, nullptr, nullptr, nullptr,
     [](oo::DebugMonitor* m) -> std::int64_t { return m->debuggerConnected(); },
     oo::kValueType<bool>},
    {"TCPIgnoresDroppedPackets", MonitorMethodForm::kScalar, nullptr, nullptr,
     nullptr,
     [](oo::DebugMonitor* m) -> std::int64_t {
       return m->TCPIgnoresDroppedPackets();
     },
     oo::kValueType<bool>},
    {"usingPlugInController", MonitorMethodForm::kScalar, nullptr, nullptr,
     nullptr,
     [](oo::DebugMonitor* m) -> std::int64_t {
       return m->usingPlugInController();
     },
     oo::kValueType<bool>},
    {"configurationKeys", MonitorMethodForm::kNotCallable, nullptr, nullptr,
     nullptr, nullptr, oo::ValueType::kVoid},  // a std::vector result
    {"dumpJSMemoryStatistics", MonitorMethodForm::kScalar, nullptr, nullptr,
     nullptr,
     [](oo::DebugMonitor* m) -> std::int64_t {
       return static_cast<std::int64_t>(m->dumpJSMemoryStatistics());
     },
     oo::kValueType<size_t>},
};

/*	OOJSCallObjCObjectMethod()'s steps for the monitor's methods, on the
   table above. *outResult (initialised by the caller) is owned by the caller
   when set; on failure an error has been reported.
*/
bool CallMonitorMethod(JSContext* context, oo::DebugMonitor* monitor, int argc,
                       JSValueConst* argv, JSValue* outResult) {
  const std::string className;  // none
  if (argc == 0) {
    OOJSReportError(context, "%s.callObjC(): no selector specified.",
                    className.c_str());
    return false;
  }

  const std::optional<std::string> name = OOStringFromJSValue(context, argv[0]);
  std::optional<std::string> parameter;
  if (1 < argc && name.has_value() && oo::str::hasSuffix(*name, ":")) {
    std::string joined;
    for (int i = 1; i < argc; i++) {
      if (i > 1) joined += " ";
      joined += OOStringFromJSValueEvenIfNull(context, argv[i])
                    .value_or(std::string());
    }
    parameter = joined;
  }

  const MonitorMethod* method = nullptr;
  if (name.has_value()) {
    for (const MonitorMethod& row : kMonitorMethods) {
      if (*name == row.name) method = &row;
    }
  }
  const char* nameString = name.has_value() ? name->c_str() : "(null)";
  if (method == nullptr) {
    OOJSReportError(context, "%s.callObjC(): %s does not respond to method %s.",
                    className.c_str(), monitor->description().c_str(),
                    nameString);
    return false;
  }

  oo::PList result;
  switch (method->form) {
    case MonitorMethodForm::kVoidString:
    case MonitorMethodForm::kPListString:
      if (!parameter.has_value()) {
        OOJSReportError(context,
                        "%s.callObjC(): method %s requires a parameter.",
                        className.c_str(), nameString);
        return false;
      }
      if (method->form == MonitorMethodForm::kVoidString)
        method->voidString(monitor, *parameter);
      else
        result = method->pListString(monitor, *parameter);
      break;

    case MonitorMethodForm::kVoidVoid:
      method->voidVoid(monitor);
      break;

    case MonitorMethodForm::kScalar:
      switch (oo::ShaderUniformTypeOf(method->type)) {
        case oo::kShaderUniformTypeChar:
        case oo::kShaderUniformTypeUnsignedChar:
        case oo::kShaderUniformTypeShort:
        case oo::kShaderUniformTypeUnsignedShort:
        case oo::kShaderUniformTypeInt:
        case oo::kShaderUniformTypeUnsignedInt:
        case oo::kShaderUniformTypeLong:
          result = oo::PList::signedInteger(method->scalar(monitor));
          break;

        case oo::kShaderUniformTypeUnsignedLong:
          result = oo::PList::unsignedInteger(
              static_cast<std::uint64_t>(method->scalar(monitor)));
          break;

        default:
          // size_t on Windows (unsigned long long) matched no template.
          OOJSReportError(
              context,
              "%s.callObjC(): method %s cannot be called from JavaScript.",
              className.c_str(), nameString);
          return false;
      }
      break;

    case MonitorMethodForm::kNotCallable:
      OOJSReportError(
          context, "%s.callObjC(): method %s cannot be called from JavaScript.",
          className.c_str(), nameString);
      return false;
  }

  if (!result.isNull()) *outResult = OOJSValueFromPList(context, result);
  return true;
}

}  // namespace

// function callObjC(selector : String [, ...]) : Object
static JSValue ConsoleCallObjCMethod(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  JSValue result = JS_UNDEFINED;
  bool OK;

  // The console objects hold the C++ monitor: its table, not a selector.
  if (oo::DebugMonitor* monitor = MonitorFromJSObject(OOJS_THIS)) {
    OOJSPauseTimeLimiter();
    OK = CallMonitorMethod(context, monitor, argc, OOJS_ARGV, &result);
    OOJSResumeTimeLimiter();
    if (!OK) {
      JS_FreeValue(context, result);
      return OOJSNativeFailure(context);
    }
    return result;
  }

  // An entity's `this` converts to its entity node, whose C++ entity this takes
  // and calls by its name table. Any other `this` gets the error below, as a
  // plain JS object does.
  const oo::PList thisNode = OOJSPListFromJSValue(context, OOJS_THIS);
  Entity* entity = oo::EntityIn(thisNode);
  if (entity == nullptr) {
    OOJSReportError(
        context,
        "Attempt to call __callObjCMethod() for non-Objective-C object %s.",
        OOStringFromJSValueEvenIfNull(context, OOJS_THIS)
            .value_or("(null)")
            .c_str());
    return OOJSNativeFailure(context);
  }

  OOJSPauseTimeLimiter();
  // The class name of the error texts: an entity's is its C++ part's, the
  // answer its object's -oo_jsClassName forwarded to.
  std::optional<std::string> className = OOJSEntityJSClassName(entity);
  OK = OOJSCallEntityMethod(context, entity, className.value_or(std::string()),
                            argc, OOJS_ARGV, &result);
  OOJSResumeTimeLimiter();

  if (!OK) {
    JS_FreeValue(context, result);
    return OOJSNativeFailure(context);
  }
  return result;

  OOJS_NATIVE_EXIT
}

// function __setUpCallObjC(object) -- object is expected to be Object.prototye.
static JSValue ConsoleSetUpCallObjC(JSContext* context,
                                    JSValueConst /*oojsThis*/, int argc,
                                    JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  if (argc < 1 || !JS_IsObject(OOJS_ARGV[0])) [[unlikely]] {
    OOJSReportBadArguments(context, "Console", "__setUpCallObjC", argc,
                           OOJS_ARGV, std::nullopt, "Object.prototype");
    return OOJSNativeFailure(context);
  }

  // Read-only, permanent and not enumerable, as OOJS_METHOD_READONLY was.
  JSValue function =
      JS_NewCFunction(context, ConsoleCallObjCMethod, "callObjC", 1);
  JS_DefinePropertyValueStr(context, OOJS_ARGV[0], "callObjC", function, 0);
  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}
#endif

/*	Like SpiderMonkey's JS_BufferIsCompilableUnit(): true unless the code
   fails to compile only because it ends too soon, as when the console user is
        part way through typing a multi-line function. Code with other syntax
        errors is "executable" so that the error is reported when it's run.
        QuickJS reports running out of input as an "end of" error (input,
        string, comment, regexp) or an unexpected empty token, at the end of
        the code.
*/
static bool IsIncompleteSyntaxError(JSContext* context, JSValueConst exception,
                                    const std::string& code) {
  if (!JS_IsError(exception)) return false;

  JSValue nameVal = JS_GetPropertyStr(context, exception, "name");
  const std::optional<std::string> name = OOStringFromJSValue(context, nameVal);
  JS_FreeValue(context, nameVal);
  if (name != "SyntaxError") return false;

  JSValue messageVal = JS_GetPropertyStr(context, exception, "message");
  const std::string message =
      OOStringFromJSValue(context, messageVal).value_or(std::string());
  JS_FreeValue(context, messageVal);

  if (oo::str::lowercase(message).find("end of ") != std::string::npos)
    return true;
  if (oo::str::hasSuffix(message, "''")) return true;

  // Otherwise, incomplete if the error is at the very end of the code.
  JSValue stackVal = JS_GetPropertyStr(context, exception, "stack");
  const std::string stack =
      OOStringFromJSValue(context, stackVal).value_or(std::string());
  JS_FreeValue(context, stackVal);

  // Lines and columns as the NSString form counted them (lengths in UTF-16
  // units).
  const size_t lastNewline = code.rfind('\n');
  const size_t lineCount =
      static_cast<size_t>(std::count(code.begin(), code.end(), '\n')) + 1;
  const std::string lastLine =
      (lastNewline == std::string::npos) ? code : code.substr(lastNewline + 1);
  const std::string endLocation = std::format(
      "<console input>:{}:{}", static_cast<std::uint64_t>(lineCount),
      static_cast<std::uint64_t>(oo::utf8ToUtf16(lastLine).size()) + 1);
  const std::string firstFrame = stack.substr(0, stack.find('\n'));
  return firstFrame.find(endLocation) != std::string::npos;
}

// function isExecutableJavaScript(this : Object, string : String) : Boolean
static JSValue ConsoleIsExecutableJavaScript(JSContext* context,
                                             JSValueConst /*oojsThis*/,
                                             int argc, JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  bool result = true;

  if (argc < 2 || !JS_IsObject(OOJS_ARGV[0]) || !JS_IsString(OOJS_ARGV[1])) {
    OOJS_RETURN_BOOL(false);  // Fail silently
  }

  // Not OOJS_BEGIN_FULL_NATIVE() - we use JSAPI while paused.
  OOJSPauseTimeLimiter();

  // Its UTF-8 bytes; JS_Eval() requires a terminating NUL, which std::string
  // has.
  const std::string string =
      OOStringFromJSValue(context, OOJS_ARGV[1]).value_or(std::string());

  JSValue compiled =
      JS_Eval(context, string.c_str(), string.size(), "<console input>",
              JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);
  if (JS_IsException(compiled)) {
    JSValue exception = JS_GetException(context);
    result = !IsIncompleteSyntaxError(context, exception, string);
    JS_FreeValue(context, exception);
  } else {
    JS_FreeValue(context, compiled);
  }

  OOJSResumeTimeLimiter();

  OOJS_RETURN_BOOL(result);

  OOJS_NATIVE_EXIT
}

// function displayMessagesInClass(class : String) : Boolean
static JSValue ConsoleDisplayMessagesInClass(JSContext* context,
                                             JSValueConst /*oojsThis*/,
                                             int argc, JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> messageClass;

  if (argc > 0) messageClass = OOStringFromJSValue(context, OOJS_ARGV[0]);
  OOJS_RETURN_BOOL(messageClass.has_value() &&
                   oo::log::willDisplay(*messageClass));

  OOJS_NATIVE_EXIT
}

// function setDisplayMessagesInClass(class : String, flag : Boolean) : void
static JSValue ConsoleSetDisplayMessagesInClass(JSContext* context,
                                                JSValueConst /*oojsThis*/,
                                                int argc,
                                                JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> messageClass;

  if (argc > 0) messageClass = OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (messageClass.has_value() && argc > 1) {
    oo::log::logger().setDisplay(*messageClass,
                                 OOJSValueToBOOL(context, OOJS_ARGV[1]));
  }
  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

// function writeLogMarker() : void
static JSValue ConsoleWriteLogMarker(JSContext* context,
                                     JSValueConst /*oojsThis*/, int /*argc*/,
                                     JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  oo::LogInsertMarker();
  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

// function writeMemoryStats() : void
static JSValue ConsoleWriteMemoryStats(JSContext* context,
                                       JSValueConst /*oojsThis*/, int /*argc*/,
                                       JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  OOJS_BEGIN_FULL_NATIVE(context)
  oo::DebugMonitor::sharedDebugMonitor()->dumpMemoryStatistics();
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

// function writeJSMemoryStats() : void
static JSValue ConsoleWriteJSMemoryStats(JSContext* context,
                                         JSValueConst /*oojsThis*/,
                                         int /*argc*/,
                                         JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  OOJS_BEGIN_FULL_NATIVE(context)
  oo::DebugMonitor::sharedDebugMonitor()->dumpJSMemoryStatistics();
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}

static size_t JSHeapSize(JSRuntime* runtime) {
  JSMemoryUsage usage;
  JS_ComputeMemoryUsage(runtime, &usage);
  return static_cast<size_t>(usage.malloc_size);
}

// function garbageCollect() : string
static JSValue ConsoleGarbageCollect(JSContext* context,
                                     JSValueConst /*oojsThis*/, int /*argc*/,
                                     JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  JSRuntime* runtime = JS_GetRuntime(context);
  size_t bytesBefore = JSHeapSize(runtime);
  JS_RunGC(runtime);
  size_t bytesAfter = JSHeapSize(runtime);

  OOJS_RETURN_PLIST(oo::PList(std::format("Bytes before: {} Bytes after: {}",
                                          bytesBefore, bytesAfter)));

  OOJS_NATIVE_EXIT
}

#if DEBUG
static JSValue ConsoleDumpHeap(JSContext* context, JSValueConst /*oojsThis*/,
                               int /*argc*/, JSValueConst* /*oojsArgv*/) {
  OOJS_NATIVE_ENTER(context)

  bool OK = false;
  const std::optional<std::string> diagnosticDirectory =
      ResourceManager::diagnosticFileLocation();
  const std::string path = diagnosticDirectory.has_value()
                               ? oo::str::appendingPathComponent(
                                     *diagnosticDirectory, "js-heaps.txt")
                               : std::string();
  FILE* file = fopen(path.c_str(), "w");
  if (file != nullptr) {
    JSRuntime* runtime = JS_GetRuntime(context);
    JSMemoryUsage usage;
    JS_ComputeMemoryUsage(runtime, &usage);
    JS_DumpMemoryUsage(file, &usage, runtime);
    fclose(file);
    OK = true;
  }

  OOJS_RETURN_BOOL(OK);

  OOJS_NATIVE_EXIT
}
#endif

#if OOJS_PROFILE

// function profile(func : function [, Object this = debugConsole.script]) :
// String
static JSValue ConsoleProfile(JSContext* context, JSValueConst /*oojsThis*/,
                              int argc, JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  if (OOJSIsProfiling()) [[unlikely]] {
    OOJSReportError(
        context,
        "Profiling functions may not be called while already profiling.");
    return OOJSNativeFailure(context);
  }

  JSValue result = JS_UNDEFINED;
  {
    oo::AutoreleaseScope pool;  // was @autoreleasepool
    std::shared_ptr<OOTimeProfile> profile;

    if (PerformProfiling(context, "profile", argc, OOJS_ARGV, nullptr, false,
                         &profile)) {
      result = OOJSValueFromPList(
          context, profile
                       ? oo::PList(profile->description().value_or("(null)"))
                       : oo::PList());
    } else {
      result = OOJSNativeFailure(context);
    }
  }

  return result;

  OOJS_NATIVE_EXIT
}

// function getProfile(func : function [, Object this = debugConsole.script]) :
// Object { totalTime : Number, jsTime : Number, extensionTime : Number }
static JSValue ConsoleGetProfile(JSContext* context, JSValueConst /*oojsThis*/,
                                 int argc, JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  if (OOJSIsProfiling()) [[unlikely]] {
    OOJSReportError(
        context,
        "Profiling functions may not be called while already profiling.");
    return OOJSNativeFailure(context);
  }

  JSValue result = JS_UNDEFINED;
  {
    oo::AutoreleaseScope pool;  // was @autoreleasepool
    std::shared_ptr<OOTimeProfile> profile;

    if (PerformProfiling(context, "getProfile", argc, OOJS_ARGV, nullptr, false,
                         &profile)) {
      result = profile ? profile->oo_jsValueInContext(context) : JS_NULL;
    } else {
      result = OOJSNativeFailure(context);
    }
  }

  return result;

  OOJS_NATIVE_EXIT
}

// function trace(func : function [, Object this = debugConsole.script]) :
// [return type of func]
static JSValue ConsoleTrace(JSContext* context, JSValueConst /*oojsThis*/,
                            int argc, JSValueConst* oojsArgv) {
  OOJS_NATIVE_ENTER(context)

  if (OOJSIsProfiling()) [[unlikely]] {
    OOJSReportError(
        context,
        "Profiling functions may not be called while already profiling.");
    return OOJSNativeFailure(context);
  }

  JSValue rval = JS_UNDEFINED;
  JSValue result;
  {
    oo::AutoreleaseScope pool;  // was @autoreleasepool
    if (PerformProfiling(context, "trace", argc, OOJS_ARGV, &rval, true,
                         nullptr)) {
      result = rval;
    } else {
      result = OOJSNativeFailure(context);
    }
  }

  return result;

  OOJS_NATIVE_EXIT
}

/*	Calls argv[0] with argv[1] (or debugConsole.script) as this, profiling
   it. *outRval, if not NULL, is owned by the caller. On failure, any exception
        has been reported.
*/
namespace {

bool PerformProfiling(JSContext* context, const char* nominalFunction, int argc,
                      JSValueConst* argv, JSValue* outRval, bool trace,
                      std::shared_ptr<OOTimeProfile>* outProfile) {
  // Get function.
  if (argc < 1 || !OOJSValueIsFunction(context, argv[0])) {
    OOJSReportBadArguments(context, "Console", nominalFunction,
                           std::min(argc, 1), argv, std::nullopt, "function");
    return false;
  }
  JSValueConst function = argv[0];

  // Get "this" object.
  JSValue thisObj;
  if (argc > 1) {
    thisObj = JS_DupValue(context, argv[1]);
  } else {
    JSValue debugConsole =
        oo::DebugMonitor::sharedDebugMonitor()->jsValueInContext(context);
    thisObj = JS_GetPropertyStr(context, debugConsole, "script");
    JS_FreeValue(context, debugConsole);
  }

  // Fiddle with time limiter.
  // We want to save the current limit, reset the limiter, and set the time
  // limit to a long time.
  // A long time - 115.7 days - but, crucially, finite.
  constexpr double LONG_TIME = (1e7);

  oo::TimeDelta originalLimit = OOJSGetTimeLimiterLimit();
  OOJSSetTimeLimiterLimit(LONG_TIME);
  OOJSResetTimeLimiter();

  OOJSBeginProfiling(trace);

  // Call the function.
  JSValue rval = JS_Call(context, function, thisObj, 0, nullptr);

  // Get results.
  std::shared_ptr<OOTimeProfile> profile = OOJSEndProfiling();
  if (outProfile != nullptr) *outProfile = std::move(profile);

  // Restore original timer state.
  OOJSSetTimeLimiterLimit(originalLimit);
  OOJSResetTimeLimiter();

  JS_FreeValue(context, thisObj);

  if (JS_IsException(rval)) {
    OOJSReportPendingException(context);
    return false;
  }

  if (outRval != nullptr)
    *outRval = rval;
  else
    JS_FreeValue(context, rval);
  return true;
}

}  // namespace

#endif  // OOJS_PROFILE

#endif /* OO_DEBUG_MONITOR_ENABLED */
