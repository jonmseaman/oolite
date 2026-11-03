/*

OOJavaScriptEngine.cpp (QuickJS-ng)

JavaScript support for Oolite: upstream #768's OOJavaScriptEngine.m, in C++.
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

#include "OOJavaScriptEngine.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>

#include "EntityOOJavaScriptExtensions.h"
#include "OOConstToJSString.h"
#include "OOFoundation.h"  // OOLITE_DEBUG
#include "OOJSClock.h"
#include "OOJSDock.h"
#include "OOJSEngineTimeManagement.h"
#include "OOJSEntity.h"
#include "OOJSEquipmentInfo.h"
#include "OOJSExhaustPlume.h"
#include "OOJSFlasher.h"
#include "OOJSFont.h"
#include "OOJSFrameCallbacks.h"
#include "OOJSGlobal.h"
#include "OOJSManifest.h"
#include "OOJSMission.h"
#include "OOJSMissionVariables.h"
#include "OOJSOolite.h"
#include "OOJSPlanet.h"
#include "OOJSPlayer.h"
#include "OOJSPlayerShip.h"
#include "OOJSPrivateObject.h"
#include "OOJSQuaternion.h"
#include "OOJSScript.h"
#include "OOJSShip.h"
#include "OOJSShipGroup.h"
#include "OOJSSound.h"
#include "OOJSSoundSource.h"
#include "OOJSSpecialFunctions.h"
#include "OOJSStation.h"
#include "OOJSSun.h"
#include "OOJSSystem.h"
#include "OOJSSystemInfo.h"
#include "OOJSTimer.h"
#include "OOJSVector.h"
#include "OOJSVisualEffect.h"
#include "OOJSWaypoint.h"
#include "OOJSWorldScripts.h"
#include "OOJSWormhole.h"
#include "OOLoggingExtended.h"
#include "OOObjCPList.h"  // Object nodes (OOJSValueFromPList)
#include "OOPlanetEntity.h"
#include "OOProfilingStopwatch.h"
#include "OOVisualEffectEntity.h"
#include "OOWaypointEntity.h"
#include "ResourceManager.h"
#include "Universe.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/Encoding.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/Notification.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/PListGet.hpp"
#include "oofnd/StdLib.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/OOAssert.h"
#include "oofnd/objc/OOException.h"

#define OOJS_RUNTIME_SIZE_MiB 256
#define OOJS_STACK_SIZE (1024 * 1024)

#if OO_DEBUG
#define OOJS_TIME_LIMIT (0.2)  // seconds
#else
#define OOJS_TIME_LIMIT (1)  // seconds
#endif

namespace {
OOJavaScriptEngine* sSharedEngine = nullptr;  // the one +1 is never released
unsigned sErrorHandlerStackSkip = 0;

int InterruptHandler(JSRuntime* runtime, void* opaque);
void RegisterStandardObjectConverters(JSContext* context);
void LoadCompatibilityScript(JSContext* context);
}  // namespace

JSContext* gOOJSMainThreadContext = NULL;

const char* const kOOJavaScriptEngineWillResetNotificationName =
    "org.aegidian.oolite OOJavaScriptEngine will reset";
const char* const kOOJavaScriptEngineDidResetNotificationName =
    "org.aegidian.oolite OOJavaScriptEngine did reset";

//===========================================================================
// JavaScript engine initialisation and shutdown
//===========================================================================

OOJavaScriptEngine* OOJavaScriptEngine::sharedEngine() {
  if (sSharedEngine == nullptr) {
    // The one +1, never released. Recorded before init() runs, as -init
    // recorded self before it made the context, so a re-entrant sharedEngine()
    // answers the engine being set up.
    OOJavaScriptEngine* engine = oo::makeRef<OOJavaScriptEngine>().leakRef();
    sSharedEngine = engine;
    engine->init();
  }

  return sSharedEngine;
}

void OOJavaScriptEngine::runMissionCallback() { MissionRunCallback(); }

void OOJavaScriptEngine::init() {
  // -init asserted that no engine existed yet; sharedEngine() makes only this
  // one.
  OOCAssert(sSharedEngine == this,
            "Attempt to create multiple OOJavaScriptEngines.");
  _showErrorLocations = true;

  oo::Defaults& defaults = oo::Defaults::standard();
#ifdef OO_DEBUG_MONITOR_ENABLED
  /*	Set stack trace preferences from preferences. These will be overriden
          by the debug OXP script if installed, but being able to enable traces
          without setting up the debug console could be useful for debugging
          users' problems.
  */
  setDumpStackForErrors(defaults.boolForKey("dump-stack-for-errors"));
  setDumpStackForWarnings(defaults.boolForKey("dump-stack-for-warnings"));
#endif

  _runtime = JS_NewRuntime();
  if (_runtime == NULL) {
    OO_LOG("script.javaScript.init.error", "{}",
           "***** FATAL ERROR: failed to create JavaScript runtime.");
    exit(1);
  }

  const oo::PList jsRuntimeSize = defaults.object("jsruntime-size-mib");
  uint32_t jsRuntimeInMiB = static_cast<uint32_t>(
      oo::PListGet<int>::from(jsRuntimeSize.isNull() ? nullptr : &jsRuntimeSize,
                              OOJS_RUNTIME_SIZE_MiB));
  JS_SetMemoryLimit(_runtime, jsRuntimeInMiB * 1024L * 1024L);
  JS_SetMaxStackSize(_runtime, OOJS_STACK_SIZE);
  JS_SetInterruptHandler(_runtime, InterruptHandler, NULL);

  createMainThreadContext();
}

void OOJavaScriptEngine::createMainThreadContext() {
  OOCAssert(gOOJSMainThreadContext == NULL,
            "-[OOJavaScriptEngine createMainThreadContext] called while the "
            "main thread context exists.");

  gOOJSMainThreadContext = JS_NewContext(_runtime);
  if (gOOJSMainThreadContext == NULL) {
    OO_LOG("script.javaScript.init.error", "{}",
           "***** FATAL ERROR: failed to create JavaScript context.");
    exit(1);
  }
  _context = gOOJSMainThreadContext;

  // Create the global object.
  _globalObject = JS_GetGlobalObject(_context);
  JS_DefinePropertyValueStr(_context, _globalObject, "global",
                            JS_DupValue(_context, _globalObject),
                            OOJS_PROP_READONLY);

  RegisterStandardObjectConverters(_context);
  LoadCompatibilityScript(_context);

  SetUpOOJSGlobal(_context, _globalObject);
  OOConstToJSStringInit(_context);

  // Initialize Oolite classes.
  InitOOJSMissionVariables(_context, _globalObject);
  InitOOJSMission(_context, _globalObject);
  InitOOJSOolite(_context, _globalObject);
  InitOOJSVector(_context, _globalObject);
  InitOOJSQuaternion(_context, _globalObject);
  InitOOJSSystem(_context, _globalObject);
  InitOOJSEntity(_context, _globalObject);
  InitOOJSShip(_context, _globalObject);
  InitOOJSStation(_context, _globalObject);
  InitOOJSDock(_context, _globalObject);
  InitOOJSVisualEffect(_context, _globalObject);
  InitOOJSExhaustPlume(_context, _globalObject);
  InitOOJSFlasher(_context, _globalObject);
  InitOOJSWormhole(_context, _globalObject);
  InitOOJSWaypoint(_context, _globalObject);
  InitOOJSPlayer(_context, _globalObject);
  InitOOJSPlayerShip(_context, _globalObject);
  InitOOJSManifest(_context, _globalObject);
  InitOOJSSun(_context, _globalObject);
  InitOOJSPlanet(_context, _globalObject);
  InitOOJSScript(_context, _globalObject);
  InitOOJSTimer(_context, _globalObject);
  InitOOJSClock(_context, _globalObject);
  InitOOJSWorldScripts(_context, _globalObject);
  InitOOJSSound(_context, _globalObject);
  InitOOJSSoundSource(_context, _globalObject);
  InitOOJSSpecialFunctions(_context, _globalObject);
  InitOOJSSystemInfo(_context, _globalObject);
  InitOOJSEquipmentInfo(_context, _globalObject);
  InitOOJSShipGroup(_context, _globalObject);
  InitOOJSFrameCallbacks(_context, _globalObject);
  InitOOJSFont(_context, _globalObject);

  // Run prefix scripts.
  JSValue special = JSSpecialFunctionsObjectWrapper(_context);
  oo::Ref<OOJSValue> specialWrapper =
      OOJSValue::valueWithJSValue(special, _context);
  JS_FreeValue(_context, special);
  OOScriptAutorelease(OOScript::jsScriptFromFileNamed(
      "oolite-global-prefix.js",  // (nothing keeps it)
      oo::PList(oo::PList::Dict{
          {"special", OOJSValueObjectNode(specialWrapper.get())}})));

  OO_LOG("script.javaScript.init.success", "{}", "Set up JavaScript context.");
}

void OOJavaScriptEngine::destroyMainThreadContext() {
  if (gOOJSMainThreadContext != NULL) {
    OOConstToJSStringDestroy();

    JS_FreeValue(_context, _globalObject);
    _globalObject = JS_UNDEFINED;

    JS_FreeContext(_context);
    _context = NULL;
    gOOJSMainThreadContext = NULL;

    JS_RunGC(_runtime);

#ifndef NDEBUG
    /*	Every native object holding a JSValue should have let go of it on
            kOOJavaScriptEngineWillResetNotificationName, so nothing should be
            left. Anything that is indicates a reference counting error.
    */
    JSMemoryUsage usage;
    JS_ComputeMemoryUsage(_runtime, &usage);
    if (usage.obj_count != 0) {
      OO_LOG_WARN("script.javaScript.reset.liveObjects",
                  "{} JavaScript objects are still alive after the context "
                  "was destroyed; something is holding a JSValue it does not "
                  "own, or has not freed one it does.",
                  static_cast<long long>(usage.obj_count));
    } else {
      OO_LOG("script.javaScript.reset.liveObjects", "{}",
             "No JavaScript objects left alive after the context was "
             "destroyed.");
    }
#endif
  }
}

bool OOJavaScriptEngine::reset() {
  OOCAssert(gOOJSMainThreadContext != NULL,
            "JavaScript engine not active. Can't reset.");

  OOJSFrameCallbacksRemoveAll();

  oo::NotificationCenter::defaultCenter().post(
      kOOJavaScriptEngineWillResetNotificationName,
      this);  // the engine: what observers filter on

  destroyMainThreadContext();
  createMainThreadContext();

  oo::NotificationCenter::defaultCenter().post(
      kOOJavaScriptEngineDidResetNotificationName, this);

  garbageCollectionOpportunity(true);
  OO_LOG("script.javascript.init", "{}", "JavaScript reset successful.");
  return true;
}

namespace {
// true if JavaScript code is on the stack (e.g. quitGame() called from a
// script).
bool IsRunningJavaScript(JSContext* context) {
  if (context == NULL) return false;
  for (int level = 0; level < 16; level++) {
    JSAtom name = JS_GetScriptOrModuleName(context, level);
    if (name != JS_ATOM_NULL) {
      JS_FreeAtom(context, name);
      return true;
    }
  }
  return false;
}
}  // namespace

NSUInteger OOJavaScriptEngine::shutDown() {
  if (_runtime == NULL) return 0;

  /*	The game can quit from inside a script (quitGame()). The frames on the
          stack then legitimately hold JS values, and a function keeps its whole
          context alive, so the leak check would be meaningless, and freeing the
          context under running code is unsafe.
  */
  if (IsRunningJavaScript(_context)) {
    OO_LOG("script.javaScript.shutDown", "{}",
           "Not shutting down the JavaScript runtime (and skipping its leak "
           "check): quitting from inside a script.");
    return 0;
  }

  OOJSFrameCallbacksRemoveAll();
  oo::NotificationCenter::defaultCenter().post(
      kOOJavaScriptEngineWillResetNotificationName, this);
  destroyMainThreadContext();

  JSMemoryUsage usage;
  JS_ComputeMemoryUsage(_runtime, &usage);
  if (usage.obj_count != 0) {
    /*	JS_FreeRuntime() would assert. Dump what is left and abandon the
            runtime instead, so the leak can be reported.
    */
    OO_LOG("script.javaScript.shutDown.leak",
           "***** JavaScript runtime shut down with {} objects still alive; "
           "something is holding a JSValue it does not own, or has not freed "
           "one it does.",
           static_cast<long long>(usage.obj_count));
    _runtime = NULL;
    return static_cast<NSUInteger>(usage.obj_count);
  }

  JS_FreeRuntime(_runtime);
  _runtime = NULL;
  OO_LOG("script.javaScript.shutDown", "{}",
         "JavaScript runtime shut down with no leaked objects.");
  return 0;
}

OOJavaScriptEngine::~OOJavaScriptEngine() {
  shutDown();
  if (sSharedEngine == this) sSharedEngine = nullptr;
}

JSContext* OOJavaScriptEngine::context() { return _context; }

JSValueConst OOJavaScriptEngine::globalObject() { return _globalObject; }

bool OOJavaScriptEngine::callJSFunction(JSValueConst function,
                                        JSValueConst jsThis, int argc,
                                        JSValueConst* argv,
                                        JSValue* outResult) {
  OOCParameterAssert(JS_IsFunction(_context, function));

  OOJSStartTimeLimiter();
  JSValue result = JS_Call(_context, function, jsThis, argc, argv);
  OOJSStopTimeLimiter();

  if (JS_IsException(result)) {
    OOJSReportPendingException(_context);
    if (outResult != NULL) *outResult = JS_UNDEFINED;
    return false;
  }

  if (outResult != NULL)
    *outResult = result;
  else
    JS_FreeValue(_context, result);
  return true;
}

void OOJavaScriptEngine::garbageCollectionOpportunity(bool force) {
  /*	Reference counting frees most objects immediately, and QuickJS runs its
          cycle collector by allocation threshold, so only forced collections
          need doing here.
  */
  if (force) JS_RunGC(_runtime);
}

bool OOJavaScriptEngine::showErrorLocations() { return _showErrorLocations; }

void OOJavaScriptEngine::setShowErrorLocations(bool value) {
  _showErrorLocations = !!value;
}

#ifdef OO_DEBUG_MONITOR_ENABLED

bool OOJavaScriptEngine::dumpStackForErrors() { return _dumpStackForErrors; }

void OOJavaScriptEngine::setDumpStackForErrors(bool value) {
  _dumpStackForErrors = !!value;
}

bool OOJavaScriptEngine::dumpStackForWarnings() {
  return _dumpStackForWarnings;
}

void OOJavaScriptEngine::setDumpStackForWarnings(bool value) {
  _dumpStackForWarnings = !!value;
}

void OOJavaScriptEngine::enableDebuggerStatement() {
  // QuickJS treats the debugger statement as a no-op and has no hook for it.
}
#endif

#if OOJSENGINE_MONITOR_SUPPORT

void OOJavaScriptEngine::setMonitor(OOJavaScriptEngineMonitor* inMonitor) {
  _monitor = inMonitor;
}

void OOJavaScriptEngine::sendMonitorProblem(
    const std::string& severity, const std::optional<std::string>& file,
    unsigned line, const std::string& message, JSContext* theContext) {
  if (_monitor != nullptr) {
    _monitor->jsEngine(this, theContext, severity, file, line,
                       sErrorHandlerStackSkip, showErrorLocations(), message);
  }
}

void OOJavaScriptEngine::sendMonitorLogMessage(
    const std::optional<std::string>& message,
    const std::optional<std::string>& messageClass, JSContext* theContext) {
  if (_monitor != nullptr) {
    _monitor->jsEngine(this, theContext, message.value_or(""), messageClass);
  }
}

#endif

namespace {

// A file's text as -stringWithContentsOfUnicodeFile: read it (it read through
// .oxz archives); nullopt if it cannot be read.
std::optional<std::string> ContentsOfUnicodeFile(const std::string& path) {
  if (const std::optional<oo::Data> bytes = OODataFromOXZFile(path))
    return oo::str::decodeUnicodeText(bytes->stringView());
  return std::nullopt;
}

/*	Define the SpiderMonkey extensions OXPs rely on (toSource(),
   uneval()...) before any other script runs. The script is run as plain global
   code, not wrapped like normal scripts.
*/
void LoadCompatibilityScript(JSContext* context) {
  const std::optional<std::string> path =
      ResourceManager::pathForFileNamed("oolite-quickjs-compat.js", "Scripts");
  const std::optional<std::string> source =
      path.has_value() ? ContentsOfUnicodeFile(*path) : std::nullopt;
  if (!source.has_value()) {
    OO_LOG_ERR("script.javaScript.init.error", "{}",
               "Could not load oolite-quickjs-compat.js; SpiderMonkey "
               "compatibility functions such as toSource() will be missing.");
    return;
  }

  // JS_Eval requires a terminating NUL, which std::string's c_str() has.
  JSValue result = JS_Eval(context, source->c_str(), source->size(),
                           path->c_str(), JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(result)) OOJSReportPendingException(context);
  JS_FreeValue(context, result);

  /*	The bundled QuickJS-ng is patched (subprojects/packagefiles/quickjs-ng)
          for SpiderMonkey behaviour OXPs rely on. Warn if this one isn't, e.g.
          a system library.
  */
  static bool checked = false;
  if (!checked) {
    checked = true;
    const char* test =
        "(function () { 'use strict'; try { 'x'.oolite = 1; return true; } "
        "catch (e) { return false; } })()";
    JSValue patched = JS_Eval(context, test, strlen(test),
                              "<compatibility check>", JS_EVAL_TYPE_GLOBAL);
    if (!JS_IsException(patched) && !OOJSValueToBOOL(context, patched)) {
      OO_LOG_WARN("script.javaScript.init.unpatchedEngine", "{}",
                  "This QuickJS-ng library lacks Oolite's compatibility "
                  "patches; some OXPs that worked with SpiderMonkey may fail. "
                  "Build with the bundled QuickJS-ng subproject.");
    }
    if (JS_IsException(patched))
      JS_FreeValue(context, JS_GetException(context));
    JS_FreeValue(context, patched);
  }
}

}  // namespace

std::string OOJSWrapScriptSource(const std::string& source) {
  // A newline before the closing part is needed if the last line is a //
  // comment.
  const char* lineEnd = oo::str::hasSuffix(source, "\n") ? "" : "\n";
  return "with(this){(function(){" + source + lineEnd + "}).call(this)}";
}

bool OOJSValueToBOOL(JSContext* context, JSValueConst value) {
  int result = JS_ToBool(context, value);
  if (result < 0) {
    JS_FreeValue(context, JS_GetException(context));
    return false;
  }
  return result != 0;
}

bool OOJSValueToInt32(JSContext* context, JSValueConst value,
                      int32_t* outValue) {
  double number;
  if (JS_ToFloat64(context, &number, value) < 0) return false;

  number = floor(number + 0.5);
  if (std::isnan(number) || number < INT32_MIN || number > INT32_MAX) {
    cxx_OOJSReportError(context, "can't convert %s to an integer",
                        cxx_OOStringFromJSValueEvenIfNull(context, value)
                            .value_or("(null)")
                            .c_str());
    return false;
  }

  *outValue = static_cast<int32_t>(number);
  return true;
}

//===========================================================================
// Time limiter
//===========================================================================

namespace {
unsigned sLimiterStartDepth;
int sLimiterPauseDepth;
OOHighResTimeValue sLimiterStart;
OOHighResTimeValue sLimiterPauseStart;
double sLimiterTimeLimit;
}  // namespace

void OOJSStartTimeLimiterWithTimeLimit(OOTimeDelta limit) {
  if (sLimiterStartDepth++ == 0) {
    if (limit <= 0.0) limit = OOJS_TIME_LIMIT;
    sLimiterTimeLimit = limit;
    sLimiterPauseDepth = 0;

    OODisposeHighResTime(sLimiterStart);
    sLimiterStart = OOGetHighResTime();
  }
}

void OOJSStopTimeLimiter(void) {
  if (sLimiterStartDepth == 0) {
    OO_LOG("bug.javaScript.limiterDepth", "{}",
           "Attempt to stop JavaScript time limiter while it is already fully "
           "stopped. This is an internal bug, please report it.");
    return;
  }

  if (--sLimiterStartDepth == 0) sLimiterTimeLimit = 0.0;
}

void OOJSPauseTimeLimiter(void) {
  if (sLimiterPauseDepth++ == 0) {
    OODisposeHighResTime(sLimiterPauseStart);
    sLimiterPauseStart = OOGetHighResTime();
  }
}

void OOJSResumeTimeLimiter(void) {
  if (--sLimiterPauseDepth == 0) {
    OOHighResTimeValue now = OOGetHighResTime();
    sLimiterTimeLimit += OOHighResTimeDeltaInSeconds(sLimiterPauseStart, now);
    OODisposeHighResTime(now);
  }
}

#ifndef NDEBUG
void OOJSResetTimeLimiter(void) {
  OODisposeHighResTime(sLimiterStart);
  sLimiterStart = OOGetHighResTime();
}

OOTimeDelta OOJSGetTimeLimiterLimit(void) { return sLimiterTimeLimit; }

void OOJSSetTimeLimiterLimit(OOTimeDelta limit) { sLimiterTimeLimit = limit; }
#endif

#if OOJS_PROFILE
void OOJSExcludeTimeFromLimiter(OOTimeDelta time) {
  /*	Equivalent of pausing/resuming time limiter, except that it guarantees
          excluded time will match profiler overhead if there are no other
          pauses happening.
  */
  if (sLimiterPauseDepth == 0) sLimiterTimeLimit += time;
}
#endif

namespace {
/*	QuickJS polls this regularly while running JS code, so unlike the
        SpiderMonkey operation callback no watchdog thread is needed. Returning
        non-zero throws an uncatchable error, which unwinds all JS frames.
*/
int InterruptHandler(JSRuntime* /*runtime*/, void* /*opaque*/) {
  if (EXPECT(sLimiterStartDepth == 0 || sLimiterPauseDepth > 0)) return 0;

  OOHighResTimeValue now = OOGetHighResTime();
  OOTimeDelta elapsed = OOHighResTimeDeltaInSeconds(sLimiterStart, now);
  OODisposeHighResTime(now);
  if (elapsed <= sLimiterTimeLimit) return 0;

  OOJSScript* runningScript = OOJSScript::currentlyRunningScript();
  OO_LOG_ERR("script.javaScript.timeLimit",
             "Script \"{}\" ran for {:g} seconds and has been terminated.",
             ((runningScript != nullptr) ? runningScript->name() : std::nullopt)
                 .value_or("(null)"),
             elapsed);
#ifdef OO_DEBUG_MONITOR_ENABLED
  OOJSDumpStack(gOOJSMainThreadContext);
#endif
  return 1;
}
}  // namespace

//===========================================================================
// Error reporting
//===========================================================================

namespace {

std::string_view TrimWhitespace(std::string_view text) {
  static constexpr std::string_view kWhitespace = " \t";
  const std::size_t first = text.find_first_not_of(kWhitespace);
  if (first == std::string_view::npos) return std::string_view();
  const std::size_t last = text.find_last_not_of(kWhitespace);
  return text.substr(first, last - first + 1);
}

/*	QuickJS records locations only in the stack property, as lines of the
   form "    at name (file:line:column)" or "    at file:line:column". Extract
   the file and line of the innermost frame with a location.
*/
bool GetLocationFromStack(const std::optional<std::string>& stack,
                          std::optional<std::string>* outFile,
                          unsigned* outLine) {
  if (!stack.has_value()) return false;

  std::string_view rest(*stack);
  while (!rest.empty()) {
    const std::size_t newline = rest.find('\n');
    std::string_view frame = TrimWhitespace(rest.substr(0, newline));
    rest = (newline == std::string_view::npos) ? std::string_view()
                                               : rest.substr(newline + 1);

    if (frame.substr(0, 3) != "at ") continue;
    frame.remove_prefix(3);

    const std::size_t open = frame.rfind('(');
    if (open != std::string_view::npos && !frame.empty() &&
        frame.back() == ')') {
      frame = frame.substr(open + 1, frame.size() - open - 2);
    }

    // file:line:column; the file may itself contain colons.
    const std::size_t lastColon = frame.rfind(':');
    if (lastColon == std::string_view::npos || lastColon == 0) continue;
    const std::size_t lineColon = frame.rfind(':', lastColon - 1);
    if (lineColon == std::string_view::npos)
      continue;  // Native frame, e.g. "at log (native)".

    const std::string lineText(
        frame.substr(lineColon + 1, lastColon - lineColon - 1));
    *outFile = std::string(frame.substr(0, lineColon));
    *outLine = static_cast<unsigned>(atoi(lineText.c_str()));
    return true;
  }
  return false;
}

std::optional<std::string> StackOfValue(JSContext* context,
                                        JSValueConst value) {
  if (!JS_IsObject(value)) return std::nullopt;
  JSValue stack = JS_GetPropertyStr(context, value, "stack");
  if (JS_IsException(stack)) {
    JS_FreeValue(context, JS_GetException(context));
    return std::nullopt;
  }
  std::optional<std::string> result = cxx_OOStringFromJSValue(context, stack);
  JS_FreeValue(context, stack);
  return result;
}

std::optional<std::string> CurrentStack(JSContext* context) {
  // A fresh Error object captures the current JS stack.
  JSValue error = JS_NewError(context);
  std::optional<std::string> result = StackOfValue(context, error);
  JS_FreeValue(context, error);
  return result;
}

void LogProblem(JSContext* context, const std::string& severity,
                const std::string& errorName, const std::string& message,
                const std::optional<std::string>& stack) {
  const std::string messageClass =
      "script.javaScript." + severity + "." + errorName;
  const char* highlight = (severity == "warning") ? "-----" : "*****";
  OOJavaScriptEngine* jsEng = OOJavaScriptEngine::sharedEngine();

  // Skip the rest if this is a warning being ignored.
  if (!oo::log::willDisplay(messageClass)) return;

  // avoid windows DEP exceptions!
  // The running script (null when none runs, or once a script a timer or
  // definition pushed weakly has gone: a message to nil).
  OOJSScript* thisScript = OOJSScript::currentlyRunningScript();
  const std::string activeScript =
      ((thisScript != nullptr) ? thisScript->displayName() : std::nullopt)
          .value_or("<unidentified script>");
  OO_LOG(messageClass, "{} JavaScript {} ({}): {}", highlight, severity,
         activeScript, message);

  std::optional<std::string> file;
  unsigned line = 0;
  if (jsEng->showErrorLocations() && sErrorHandlerStackSkip == 0 &&
      GetLocationFromStack(stack, &file, &line)) {
    OO_LOG(messageClass, "      {}, line {}.", *file, line);
  }

#ifdef OO_DEBUG_MONITOR_ENABLED
  bool dump = (severity == "warning") ? jsEng->dumpStackForWarnings()
                                      : jsEng->dumpStackForErrors();
  if (dump && stack.has_value())
    OO_LOG("script.javaScript.stackTrace", "{}", *stack);
#endif

#if OOJSENGINE_MONITOR_SUPPORT
  if (!file.has_value()) GetLocationFromStack(stack, &file, &line);
  jsEng->sendMonitorProblem(severity, file, line, message, context);
#endif
}

}  // namespace

void OOJSReportPendingException(JSContext* context) {
  JSValue exception = JS_GetException(context);

  // The time limiter has already logged a termination.
  if (JS_IsUncatchableError(exception)) {
    JS_FreeValue(context, exception);
    return;
  }

  OOJSPauseTimeLimiter();

  std::string errorName = "exception";
  if (JS_IsError(exception)) {
    JSValue name = JS_GetPropertyStr(context, exception, "name");
    if (JS_IsException(name)) {
      JS_FreeValue(context, JS_GetException(context));
    } else {
      errorName = cxx_OOStringFromJSValue(context, name).value_or(errorName);
    }
    JS_FreeValue(context, name);
  }

  std::string message =
      cxx_OOStringFromJSValueEvenIfNull(context, exception).value_or("?");
  if (!JS_IsError(exception)) {
    message = "uncaught exception: " + message;
  }

  LogProblem(context, "exception", errorName, message,
             StackOfValue(context, exception));

  JS_FreeValue(context, exception);
  OOJSResumeTimeLimiter();
}

namespace {
std::string CallerPrefix(const std::optional<std::string>& scriptClass,
                         const std::optional<std::string>& function) {
  if (!function) return std::string();
  if (!scriptClass) return *function + ": ";
  return *scriptClass + "." + *function + ": ";
}
}  // namespace

void cxx_OOJSReportError(JSContext* context, const char* format, ...) {
  va_list args;

  va_start(args, format);
  cxx_OOJSReportErrorWithArguments(context, format, args);
  va_end(args);
}

void cxx_OOJSReportErrorForCaller(JSContext* context,
                                  const std::optional<std::string>& scriptClass,
                                  const std::optional<std::string>& function,
                                  const char* format, ...) {
  va_list args;

  try {
    va_start(args, format);
    std::string msg = oo::str::vformat(format, args);
    va_end(args);

    cxx_OOJSReportError(context, "%s%s",
                        CallerPrefix(scriptClass, function).c_str(),
                        msg.c_str());
  } catch (...) {
    // Squash any secondary errors during error handling: nothing more to do.
    return;
  }
}

void cxx_OOJSReportErrorWithArguments(JSContext* context, const char* format,
                                      va_list args) {
  try {
    std::string msg = oo::str::vformat(format, args);
    JS_ThrowPlainError(context, "%s", msg.c_str());
  } catch (...) {
    // Squash any secondary errors during error handling: nothing more to do.
    return;
  }
}

// OOJSReportCurrentException() is in OOJSEngineNativeWrappers.cpp.

JSValue OOJSNativeFailure(JSContext* context) {
  if (JS_HasException(context)) return JS_EXCEPTION;

  // Uncatchable errors are not reported by OOJSReportPendingException().
  JSValue error = JS_NewError(context);
  JS_SetUncatchableError(context, error);
  return JS_Throw(context, error);
}

#ifndef NDEBUG

void OOJSUnreachable(const char* function, const char* file, unsigned line) {
  OO_LOG(
      "fatal.unreachable",
      "Supposedly unreachable statement reached in {} ({}:{}) -- terminating.",
      function, oo::log::abbreviatedFileName(file),
      static_cast<unsigned>(line));
  abort();
}

#endif

void cxx_OOJSReportWarning(JSContext* context, const char* format, ...) {
  va_list args;

  va_start(args, format);
  cxx_OOJSReportWarningWithArguments(context, format, args);
  va_end(args);
}

void cxx_OOJSReportWarningForCaller(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, const char* format, ...) {
  va_list args;

  try {
    va_start(args, format);
    std::string msg = oo::str::vformat(format, args);
    va_end(args);

    cxx_OOJSReportWarning(context, "%s%s",
                          CallerPrefix(scriptClass, function).c_str(),
                          msg.c_str());
  } catch (...) {
    // Squash any secondary errors during error handling: nothing more to do.
    return;
  }
}

void cxx_OOJSReportWarningWithArguments(JSContext* context, const char* format,
                                        va_list args) {
  try {
    std::string msg = oo::str::vformat(format, args);
    OOJSPauseTimeLimiter();
    // SpiderMonkey reported Oolite's own warnings as error number 206,
    // "ooliteDefined" in javascript-errors.plist.
    LogProblem(context, "warning", "ooliteDefined", msg, CurrentStack(context));
    OOJSResumeTimeLimiter();
  } catch (...) {
    // Squash any secondary errors during error handling: nothing more to do.
    return;
  }
}

std::string OOStringFromJSPropertyMagic(int magic,
                                        const JSCFunctionListEntry* list,
                                        size_t count) {
  for (size_t i = 0; i < count; i++) {
    if (list[i].def_type == JS_DEF_CGETSET_MAGIC && list[i].magic == magic)
      return std::string(list[i].name);
  }
  return std::to_string(magic);
}

void OOJSReportBadPropertySelector(JSContext* context, JSValueConst thisObj,
                                   int magic, const JSCFunctionListEntry* list,
                                   size_t count) {
  const std::string propName = OOStringFromJSPropertyMagic(magic, list, count);
  const std::string className = OOJSClassNameOfValue(context, thisObj);

  cxx_OOJSReportError(context,
                      "Invalid property identifier %s for instance of %s.",
                      propName.c_str(), className.c_str());
}

void OOJSReportBadPropertyValue(JSContext* context, JSValueConst thisObj,
                                int magic, const JSCFunctionListEntry* list,
                                size_t count, JSValueConst value) {
  const std::string propName = OOStringFromJSPropertyMagic(magic, list, count);
  const std::string className = OOJSClassNameOfValue(context, thisObj);
  const std::string valueDesc = cxx_OOJSDescribeValue(context, value, true);

  cxx_OOJSReportError(
      context, "Cannot set property %s of instance of %s to invalid value %s.",
      propName.c_str(), className.c_str(), valueDesc.c_str());
}

void cxx_OOJSReportBadArguments(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, int argc, JSValueConst* argv,
    const std::optional<std::string>& message,
    const std::optional<std::string>& expectedArgsDescription) {
  try {
    std::string text = message ? *message : std::string("Invalid arguments");
    std::optional<std::string> parameters =
        cxx_OOJSStringWithJavaScriptParameters(argv, argc, context);
    text += " " + (parameters ? *parameters : std::string("(null)"));
    if (expectedArgsDescription)
      text += " -- expected " + *expectedArgsDescription;

    cxx_OOJSReportErrorForCaller(context, scriptClass, function, "%s.",
                                 text.c_str());
  } catch (...) {
    // Squash any secondary errors during error handling: nothing more to do.
    return;
  }
}

void OOJSSetWarningOrErrorStackSkip(unsigned skip) {
  sErrorHandlerStackSkip = skip;
}

#if OOLITE_DEBUG || defined(OO_DEBUG_MONITOR_ENABLED)
void OOJSDumpStack(JSContext* context) {
  const std::optional<std::string> stack = CurrentStack(context);
  OO_LOG("script.javaScript.stackTrace", "{}",
         stack.value_or("<no JavaScript stack>"));
}
#endif

bool cxx_OOJSArgumentListGetNumber(
    JSContext* context, const std::optional<std::string>& scriptClass,
    const std::optional<std::string>& function, int argc, JSValueConst* argv,
    double* outNumber, int* outConsumed) {
  if (OOJSArgumentListGetNumberNoError(context, argc, argv, outNumber,
                                       outConsumed)) {
    return true;
  } else {
    cxx_OOJSReportBadArguments(context, scriptClass, function, argc, argv,
                               std::string("Expected number, got"),
                               std::nullopt);
    return false;
  }
}

bool OOJSArgumentListGetNumberNoError(JSContext* context, int argc,
                                      JSValueConst* argv, double* outNumber,
                                      int* outConsumed) {
  OOJS_PROFILE_ENTER

  double value;

  OOCParameterAssert(context != NULL && (argv != NULL || argc == 0) &&
                     outNumber != NULL);

  // Get value, if possible.
  if (EXPECT_NOT(argc < 1 || JS_ToFloat64(context, &value, argv[0]) < 0 ||
                 std::isnan(value))) {
    // ToNumber can throw (e.g. for a Symbol); this variant must not leave an
    // exception pending.
    if (JS_HasException(context))
      JS_FreeValue(context, JS_GetException(context));
    if (outConsumed != NULL) *outConsumed = 0;
    return false;
  }

  // Success.
  *outNumber = value;
  if (outConsumed != NULL) *outConsumed = 1;
  return true;

  OOJS_PROFILE_EXIT
}

//===========================================================================
// Property IDs
//===========================================================================

JSAtom OOJSInternAtomPRIVATE(const char* name) {
  OOCParameterAssert(name != NULL && name[0] != '\0');

  JSAtom atom = JS_NewAtom(OOJSAcquireContext(), name);
  if (EXPECT_NOT(atom == JS_ATOM_NULL)) {
    OORaiseException(OOGenericException,
                     "Failed to initialize JS ID cache for \"%s\".", name);
  }
  return atom;
}

JSAtom cxx_OOJSIDFromString(const std::string& string) {
  // Atoms are interned for the life of the process; the cache keeps its one
  // reference to each.
  static std::unordered_map<std::string, JSAtom>* sAtomCache = nullptr;

  if (sAtomCache != nullptr) {
    auto cached = sAtomCache->find(string);
    if (cached != sAtomCache->end()) return cached->second;
  }

  JSAtom atom =
      JS_NewAtomLen(OOJSAcquireContext(), string.data(), string.size());
  if (EXPECT(atom != JS_ATOM_NULL)) {
    if (sAtomCache == nullptr)
      sAtomCache = new std::unordered_map<std::string, JSAtom>;
    sAtomCache->emplace(string, atom);
  }
  return atom;
}

std::optional<std::string> cxx_OOStringFromJSID(JSAtom propID) {
  JSContext* context = OOJSAcquireContext();
  JSValue value = JS_AtomToString(context, propID);
  std::optional<std::string> result = cxx_OOStringFromJSValue(context, value);
  JS_FreeValue(context, value);
  return result;
}

//===========================================================================
// Native to JS conversion
//===========================================================================

namespace {
// The Foundation array's glue (JSArrayFromNSArray) over ObjectFromPList's
// array: null elements dropped. Owned; undefined on failure.
JSValue JSArrayValueFromPList(JSContext* context,
                              const oo::PList::Array& array) {
  OOJS_PROFILE_ENTER

  JSValue result = JS_NewArray(context);
  if (JS_IsException(result)) {
    JS_FreeValue(context, JS_GetException(context));
    return JS_UNDEFINED;
  }

  uint32_t i = 0;
  for (const oo::PList& element : array) {
    if (element.isNull()) continue;
    // JS_SetPropertyUint32() takes ownership of the value.
    JSValue elementValue = OOJSValueFromPList(context, element);
    if (EXPECT_NOT(JS_SetPropertyUint32(context, result, i, elementValue) <
                   0)) {
      JS_FreeValue(context, JS_GetException(context));
      JS_FreeValue(context, result);
      return JS_UNDEFINED;
    }
    ++i;
  }

  return result;

  OOJS_PROFILE_EXIT_JSVAL
}

// The Foundation dictionary's glue (JSObjectFromNSDictionary) over
// ObjectFromPList's dictionary: null and undefined values dropped, empty keys
// skipped, key order. Owned; undefined on failure.
JSValue JSObjectValueFromPList(JSContext* context,
                               const oo::PList::Dict& dict) {
  OOJS_PROFILE_ENTER

  JSValue result = JS_NewObject(context);
  if (JS_IsException(result)) {
    JS_FreeValue(context, JS_GetException(context));
    return JS_UNDEFINED;
  }

  for (const auto& [key, element] : dict) {
    if (element.isNull() || key.empty()) continue;
    JSValue elementValue = OOJSValueFromPList(context, element);
    if (JS_IsUndefined(elementValue)) continue;
    // JS_SetProperty() takes ownership of the value.
    if (EXPECT_NOT(JS_SetProperty(context, result, cxx_OOJSIDFromString(key),
                                  elementValue) < 0)) {
      JS_FreeValue(context, JS_GetException(context));
      JS_FreeValue(context, result);
      return JS_UNDEFINED;
    }
  }

  return result;

  OOJS_PROFILE_EXIT_JSVAL
}
}  // namespace

JSValue OOJSValueFromPList(JSContext* context, const oo::PList& plist) {
  OOJS_PROFILE_ENTER

  switch (plist.type()) {
    case oo::PList::Type::Null:
      return JS_NULL;

    case oo::PList::Type::Bool:
      // +numberWithBool: is not a float type: an int32.
      return JS_NewInt32(context, *plist.getIf<bool>() ? 1 : 0);

    case oo::PList::Type::Integer: {
      const oo::PList::Integer& integer = *plist.getIf<oo::PList::Integer>();
      if (integer.isUnsigned) {
        const unsigned long long u = integer.unsignedValue();
        if (u <= static_cast<unsigned long long>(INT32_MAX))
          return JS_NewInt32(context, static_cast<int32_t>(u));
        return JS_NewNumber(context, static_cast<double>(u));
      }
      const long long v = integer.value;
      if (static_cast<long long>(INT32_MIN) <= v &&
          v <= static_cast<long long>(INT32_MAX))
        return JS_NewInt32(context, static_cast<int32_t>(v));
      return JS_NewNumber(context, static_cast<double>(v));
    }

    case oo::PList::Type::Real: {
      double d = *plist.getIf<double>();
      if (plist.isSinglePrecision())
        d = static_cast<double>(
            static_cast<float>(d));  // +numberWithFloat: -doubleValue
      return JS_NewNumber(context, d);
    }

    case oo::PList::Type::String: {
      const std::string& string = *plist.getIf<std::string>();
      return JS_NewStringLen(context, string.data(), string.size());
    }

    case oo::PList::Type::Data:
    case oo::PList::Type::Date:
      return JS_UNDEFINED;  // Foundation data and dates: the root class's glue

    case oo::PList::Type::Array:
      return JSArrayValueFromPList(context, *plist.getIf<oo::PList::Array>());

    case oo::PList::Type::Dict:
      return JSObjectValueFromPList(context, *plist.getIf<oo::PList::Dict>());

    case oo::PList::Type::Object: {
      // A C++ object that is its own JS glue (OOJSPrivateObject.h), else an
      // Objective-C object.
      if (const oo::PList::Object* node = plist.getIf<oo::PList::Object>()) {
        if (OOJSPrivateObject* glue =
                dynamic_cast<OOJSPrivateObject*>(node->get()))
          return OOJSValueFromCxxObject(context, glue);
        if (OONativeVector* box = dynamic_cast<OONativeVector*>(node->get()))
          return OONativeVectorJSValueInContext(box, context);
        // An entity's node: the entity's JS value.
        if (const auto* held =
                dynamic_cast<const oo::EntityPListForeign*>(node->get()))
          return OOJSEntityJSValue(held->entity(), context);
        // An entity held weakly: the entity's value, undefined once it has
        // gone.
        if (const auto* weak =
                dynamic_cast<const oo::EntityWeakPListForeign*>(node->get())) {
          Entity* entity = weak->entity();
          return entity != nullptr ? OOJSEntityJSValue(entity, context)
                                   : JS_UNDEFINED;
        }
        // The one null and a held value: their own JS glue.
        if (OONull* null = dynamic_cast<OONull*>(node->get()))
          return null->jsValueInContext(context);
        if (OOJSValue* held = dynamic_cast<OOJSValue*>(node->get()))
          return held->jsValueInContext(context);
      }
      // A node that holds no Objective-C object (a C++ one): no JS glue.
      return JS_UNDEFINED;
    }
  }
  return JS_UNDEFINED;

  OOJS_PROFILE_EXIT_JSVAL
}

oo::Ref<OOJSValue> OOJSValue::valueWithJSValue(JSValueConst value,
                                               JSContext* context) {
  OOJS_PROFILE_ENTER

  oo::Ref<OOJSValue> result = oo::makeRef<OOJSValue>();
  result->initWithJSValue(value, context);
  return result;

  OOJS_PROFILE_EXIT_VAL(nullptr)
}

void OOJSValue::initWithJSValue(JSValueConst value, JSContext* context) {
  OOJS_PROFILE_ENTER

  if (context == NULL) context = OOJSAcquireContext();

  _val = JS_DupValue(context, value);
  if (!JS_IsUndefined(_val)) {
    // The engine: the sender of its reset notifications. Kept for removing
    // the observer, so the destructor does not ask for the engine (which
    // could make it).
    _resetSender = OOJavaScriptEngine::sharedEngine();
    oo::NotificationCenter::defaultCenter().addObserver(
        this, kOOJavaScriptEngineWillResetNotificationName, _resetSender,
        [this](const oo::Notification&) { deleteJSValue(); });
  }

  OOJS_PROFILE_EXIT_VOID
}

void OOJSValue::deleteJSValue() {
  if (!JS_IsUndefined(_val)) {
    JS_FreeValue(OOJSAcquireContext(), _val);
    _val = JS_UNDEFINED;

    oo::NotificationCenter::defaultCenter().removeObserver(
        this, kOOJavaScriptEngineWillResetNotificationName, _resetSender);
  }
}

OOJSValue::~OOJSValue() { deleteJSValue(); }

JSValue OOJSValue::jsValueInContext(JSContext* context) {
  return JS_DupValue(context, _val);
}

std::string OOJSValue::className() const { return "OOJSValue"; }

std::string OOJSValue::description() const {
  return oo::str::format("<OOJSValue %s>",
                         oo::str::pointerDescription(this).c_str());
}

//===========================================================================
// String utilities
//===========================================================================

std::optional<std::string> cxx_OOStringFromJSValueEvenIfNull(
    JSContext* context, JSValueConst value) {
  OOJS_PROFILE_ENTER

  size_t length;
  // Calls the value's toString method if needed.
  const char* chars = JS_ToCStringLen(context, &length, value);
  if (chars == NULL) {
    JS_FreeValue(context, JS_GetException(context));
    return std::nullopt;
  }

  std::string result(chars, length);
  JS_FreeCString(context, chars);
  return result;

  OOJS_PROFILE_EXIT_VAL(std::nullopt)
}

std::optional<std::string> cxx_OOStringFromJSValue(JSContext* context,
                                                   JSValueConst value) {
  OOJS_PROFILE_ENTER

  if (EXPECT(!JS_IsNull(value) && !JS_IsUndefined(value))) {
    return cxx_OOStringFromJSValueEvenIfNull(context, value);
  }
  return std::nullopt;

  OOJS_PROFILE_EXIT_VAL(std::nullopt)
}

std::string OOJSClassNameOfValue(JSContext* context, JSValueConst value) {
  if (!JS_IsObject(value)) return "primitive";

  const char* registered = OOJSClassName(JS_GetClassID(value));
  if (registered != NULL) return registered;

  JSValue ctor = JS_GetPropertyStr(context, value, "constructor");
  if (JS_IsException(ctor)) {
    JS_FreeValue(context, JS_GetException(context));
    ctor = JS_UNDEFINED;
  }
  JSValue name = JS_IsObject(ctor) ? JS_GetPropertyStr(context, ctor, "name")
                                   : JS_UNDEFINED;
  if (JS_IsException(name)) {
    JS_FreeValue(context, JS_GetException(context));
    name = JS_UNDEFINED;
  }
  std::optional<std::string> result = cxx_OOStringFromJSValue(context, name);
  JS_FreeValue(context, name);
  JS_FreeValue(context, ctor);
  return result.value_or("Object");
}

namespace {
std::string DescribeValue(JSContext* context, JSValueConst value,
                          bool abbreviateObjects, bool recursing) {
  OOJS_PROFILE_ENTER

  if (JS_IsFunction(context, value)) {
    JSValue nameVal = JS_GetPropertyStr(context, value, "name");
    if (JS_IsException(nameVal)) {
      JS_FreeValue(context, JS_GetException(context));
      nameVal = JS_UNDEFINED;
    }
    std::optional<std::string> name = cxx_OOStringFromJSValue(context, nameVal);
    JS_FreeValue(context, nameVal);
    if (name.has_value() && !name->empty())
      return "function " + *name;
    else
      return "function";
  }

  std::optional<std::string> result;
  JSClassID classID = JS_GetClassID(value);

  if (JS_IsString(value) ||
      (classID != JS_INVALID_CLASS_ID && classID == gOOJSStringClassID)) {
    enum : std::uint8_t { kMaxLength = 200 };

    // Truncated to kMaxLength UTF-16 units, as the NSString was cut.
    const std::u16string units = oo::utf8ToUtf16(
        cxx_OOStringFromJSValueEvenIfNull(context, value).value_or(""));
    const std::string truncated = oo::utf16ToUtf8(std::u16string_view(
        units.data(), MIN(units.size(), (size_t)kMaxLength)));
    result = "\"" + cxx_OOJSEscapedForJavaScriptLiteral(truncated) +
             ((units.size() > kMaxLength) ? "..." : "") + "\"";
  } else if (JS_IsArray(value)) {
    // Descibe up to four elements of an array.
    int64_t count;
    if (JS_GetLength(context, value, &count) == 0) {
      if (!recursing) {
        std::string arrayDesc = "[";
        int64_t i, effectiveCount = MIN(count, (int64_t)4);
        for (i = 0; i < effectiveCount; i++) {
          JSValue item =
              JS_GetPropertyUint32(context, value, static_cast<uint32_t>(i));
          std::string itemDesc = "?";
          if (!JS_IsException(item)) {
            itemDesc = DescribeValue(
                context, item, true /* always abbreviate objects in arrays */,
                true);
          } else {
            JS_FreeValue(context, JS_GetException(context));
          }
          JS_FreeValue(context, item);
          if (i != 0) arrayDesc += ", ";
          arrayDesc += itemDesc;
        }
        if (effectiveCount != count) {
          arrayDesc += oo::str::format(", ... <%lli items total>]",
                                       static_cast<long long>(count));
        } else {
          arrayDesc += "]";
        }

        result = arrayDesc;
      } else {
        result =
            oo::str::format("[<%lli items>]", static_cast<long long>(count));
      }
    } else {
      JS_FreeValue(context, JS_GetException(context));
      result = "[...]";
    }
  }

  if (!result) {
    result = cxx_OOStringFromJSValueEvenIfNull(context, value);

    if (abbreviateObjects && classID == gOOJSObjectClassID && result &&
        *result == "[object Object]") {
      result = "{...}";
    }

    if (!result) result = "?";
  }

  return *result;

  OOJS_PROFILE_EXIT_VAL(std::string())
}
}  // namespace

std::string cxx_OOJSDescribeValue(JSContext* context, JSValueConst value,
                                  bool abbreviateObjects) {
  return DescribeValue(context, value, abbreviateObjects, false);
}

std::optional<std::string> cxx_OOJSStringWithJavaScriptParameters(
    JSValueConst* params, int count, JSContext* context) {
  OOJS_PROFILE_ENTER

  if (params == NULL && count != 0) return std::nullopt;

  std::string result = "(";

  for (int i = 0; i < count; ++i) {
    if (i != 0) result += ", ";
    result += cxx_OOJSDescribeValue(context, params[i], false);
  }

  result += ")";
  return result;

  OOJS_PROFILE_EXIT_VAL(std::nullopt)
}

std::optional<std::string> cxx_OOJSConcatenationOfStringsFromJavaScriptValues(
    JSValueConst* values, size_t count, const std::string& separator,
    JSContext* context) {
  OOJS_PROFILE_ENTER

  size_t i;
  std::optional<std::string> result;

  if (count < 1) return std::nullopt;
  if (values == NULL) return std::nullopt;

  for (i = 0; i != count; ++i) {
    std::optional<std::string> element =
        cxx_OOStringFromJSValueEvenIfNull(context, values[i]);
    if (!result)
      result = element;  // a nil element left the result nil, as -mutableCopy
                         // of nil did
    else {
      *result += separator;
      if (element) *result += *element;
    }
  }

  return result;

  OOJS_PROFILE_EXIT_VAL(std::nullopt)
}

std::string cxx_OOJSEscapedForJavaScriptLiteral(std::string_view string) {
  // Byte-wise over UTF-8: every escaped character is ASCII, so multi-byte
  // sequences pass through unchanged, as the UTF-16 units they encode did.
  std::string result;
  result.reserve(string.size());

  for (char c : string) {
    switch (c) {
      case '\\':
        result += "\\\\";
        break;

      case '\b':
        result += "\\b";
        break;

      case '\f':
        result += "\\f";
        break;

      case '\n':
        result += "\\n";
        break;

      case '\r':
        result += "\\r";
        break;

      case '\t':
        result += "\\t";
        break;

      case '\v':
        result += "\\v";
        break;

      case '\'':
        result += "\\\'";
        break;

      case '\"':
        result += "\\\"";
        break;

      default:
        result += c;
    }
  }
  return result;
}

// A vector box's JS value, a Vector3D.
JSValue OONativeVectorJSValueInContext(OONativeVector* vector,
                                       JSContext* context) {
  JSValue value = JS_UNDEFINED;
  VectorToJSValue(context, vector->getVector(), &value);
  return value;
}

OONull* OONull::null() {
  static OONull* sNull = nullptr;  // the one +1 is never released
  if (sNull == nullptr) sNull = oo::makeRef<OONull>().leakRef();
  return sNull;
}

// (-copyWithZone:, which answered the object itself, was the facade's: the one
// null is shared by reference.)

std::string OONull::className() const { return "OONull"; }

std::string OONull::description() const { return "<null>"; }

JSValue OONull::jsValueInContext(JSContext* /*context*/) { return JS_NULL; }

//===========================================================================
// Reusable callbacks
//===========================================================================

JSValue OOJSUnconstructableConstruct(OOJS_NATIVE_ARGS) {
  // For constructors, this is new.target, i.e. the constructor function
  // itself.
  JSValue nameVal = JS_GetPropertyStr(context, OOJS_THIS, "name");
  if (JS_IsException(nameVal)) {
    JS_FreeValue(context, JS_GetException(context));
    nameVal = JS_UNDEFINED;
  }
  std::optional<std::string> name = cxx_OOStringFromJSValue(context, nameVal);
  JS_FreeValue(context, nameVal);

  // %@ of a nil name printed "(null)".
  cxx_OOJSReportError(context, "%s cannot be used as a constructor.",
                      name ? name->c_str() : "(null)");
  return JS_EXCEPTION;
}

// The slot holds a C++ object: the C++ finalizer, which tells it its wrapper is
// gone (clearJSSelf), releases the slot's retain and empties the slot.
void OOJSObjectWrapperFinalize(JSRuntime* runtime, JSValueConst thisObj) {
  OOJSCxxObjectWrapperFinalize(runtime, thisObj);
}

// No slot is read here (not every class's slot holds a C++ object that
// describes itself), so the JS class names the object.
JSValue OOJSObjectWrapperToString(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> description;
  const char* className = OOJSClassName(JS_GetClassID(OOJS_THIS));
  if (className != NULL) {
    description = oo::str::format("[object %s]", className);
  }
  if (!description.has_value()) description = std::string("[object]");

  OOJS_RETURN_STRING_OR_NULL(description);

  OOJS_NATIVE_EXIT
}

//===========================================================================
// Entity predicates
//===========================================================================

bool JSFunctionPredicate(Entity* entity, void* parameter) {
  OOJS_PROFILE_ENTER

  JSFunctionPredicateParameter* param =
      static_cast<JSFunctionPredicateParameter*>(parameter);
  bool result = false;

  OOCParameterAssert(entity != nullptr && param != NULL);
  OOCParameterAssert(param->context != NULL);
  OOCParameterAssert(OOJSValueIsFunction(param->context, param->function));

  if (EXPECT_NOT(param->errorFlag)) return false;

  // the C++ entity's glue; entity is required to be non-nil (asserted above).
  JSValue arg = OOJSEntityJSValue(entity, param->context);

  OOJSStartTimeLimiter();
  OOJSResumeTimeLimiter();
  JSValue rval =
      JS_Call(param->context, param->function, param->jsThis, 1, &arg);
  OOJSPauseTimeLimiter();
  OOJSStopTimeLimiter();

  JS_FreeValue(param->context, arg);

  if (!JS_IsException(rval)) {
    result = OOJSValueToBOOL(param->context, rval);
    JS_FreeValue(param->context, rval);
  } else {
    OOJSReportPendingException(param->context);
    param->errorFlag = true;
  }

  return result;

  OOJS_PROFILE_EXIT
}

namespace {

/*	What the entity predicates ask an entity. */
bool OOJavaScriptEngineIsVisibleToScripts(Entity* entity) {
  return OOJSEntityIsVisibleToScripts(entity);
}

// The entity predicates -isShip, -isSubEntity, -isPlanet and -status.
bool OOJavaScriptEngineIsShip(Entity* entity) {
  return entity != nullptr && entity->getIsShip();
}

bool OOJavaScriptEngineIsSubEntity(Entity* entity) {
  return entity != nullptr && entity->getIsSubEntity();
}

bool OOJavaScriptEngineIsPlanet(Entity* entity) {
  return entity != nullptr && entity->isPlanet();
}

OOEntityStatus OOJavaScriptEngineStatus(Entity* entity) {
  return entity != nullptr ? entity->status() : OOEntityStatus{};
}

}  // namespace

bool JSEntityIsJavaScriptVisiblePredicate(Entity* entity, void* /*parameter*/) {
  OOJS_PROFILE_ENTER

  return OOJavaScriptEngineIsVisibleToScripts(entity);

  OOJS_PROFILE_EXIT
}

bool JSEntityIsJavaScriptSearchablePredicate(Entity* entity,
                                             void* /*parameter*/) {
  OOJS_PROFILE_ENTER

  if (!OOJavaScriptEngineIsVisibleToScripts(entity)) return false;
  if (OOJavaScriptEngineIsShip(entity)) {
    if (OOJavaScriptEngineIsSubEntity(entity)) return false;
    if (OOJavaScriptEngineStatus(entity) == STATUS_COCKPIT_DISPLAY)
      return false;  // Demo ship
    return true;
  } else if (OOJavaScriptEngineIsPlanet(entity)) {
    switch (static_cast<OOPlanetEntity*>(entity)
                ->planetType())  // a planet: not nil
    {
      case STELLAR_TYPE_MOON:
      case STELLAR_TYPE_NORMAL_PLANET:
      case STELLAR_TYPE_SUN:
        return true;

      case STELLAR_TYPE_MINIATURE:
        return false;
    }
  }

  return true;  // would happen if we added a new script-visible class

  OOJS_PROFILE_EXIT
}

bool JSEntityIsDemoShipPredicate(Entity* entity, void* /*parameter*/) {
  return (OOJavaScriptEngineIsVisibleToScripts(entity) &&
          OOJavaScriptEngineIsShip(entity) &&
          OOJavaScriptEngineStatus(entity) == STATUS_COCKPIT_DISPLAY &&
          !OOJavaScriptEngineIsSubEntity(entity));
}

//===========================================================================
// Classes
//===========================================================================

// Class registration lives in OOJSClasses.cpp; the C++ private-object getter in
// OOJSPrivateObject.cpp.

//===========================================================================
// JS to native conversion
//===========================================================================

oo::PList OOJSDictionaryFromJSValue(JSContext* context, JSValueConst value) {
  OOJS_PROFILE_ENTER

  if (EXPECT_NOT(!JS_IsObject(value))) {
    return oo::PList();
  }
  return cxx_OOJSDictionaryFromJSObject(context, value);

  OOJS_PROFILE_EXIT_VAL(oo::PList())
}

oo::PList cxx_OOJSDictionaryFromJSObject(JSContext* context,
                                         JSValueConst object) {
  OOJS_PROFILE_ENTER

  JSPropertyEnum* properties = NULL;
  uint32_t i, count;

  if (JS_GetOwnPropertyNames(context, &properties, &count, object,
                             JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0) {
    JS_FreeValue(context, JS_GetException(context));
    return oo::PList();
  }

  oo::PList::Dict result;
  for (i = 0; i != count; ++i) {
    /*	An integer-like property is keyed by its decimal text, the name
       JS itself gives it, so native code can look it up by that key.
    */
    std::optional<std::string> key = cxx_OOStringFromJSID(properties[i].atom);
    JSValue value = JS_GetProperty(context, object, properties[i].atom);
    if (JS_IsException(value)) {
      JS_FreeValue(context, JS_GetException(context));
      continue;
    }

    if (key.has_value() && !JS_IsUndefined(value)) {
      oo::PList element = cxx_OOJSPListFromJSValue(context, value);
      if (!element.isNull()) result.insert_or_assign(*key, std::move(element));
    }
    JS_FreeValue(context, value);
  }

  JS_FreePropertyEnum(context, properties, count);
  return oo::PList(std::move(result));

  OOJS_PROFILE_EXIT_VAL(oo::PList())
}

oo::PList OOJSDictionaryFromStringTable(JSContext* context,
                                        JSValueConst tableValue) {
  OOJS_PROFILE_ENTER

  JSPropertyEnum* properties = NULL;
  uint32_t i, count;

  if (EXPECT_NOT(JS_IsNull(tableValue) || JS_IsUndefined(tableValue))) {
    return oo::PList();
  }

  JSValue tableObject = JS_ToObject(context, tableValue);
  if (JS_IsException(tableObject)) {
    JS_FreeValue(context, JS_GetException(context));
    return oo::PList();
  }

  if (JS_GetOwnPropertyNames(context, &properties, &count, tableObject,
                             JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0) {
    JS_FreeValue(context, JS_GetException(context));
    JS_FreeValue(context, tableObject);
    return oo::PList();
  }

  oo::PList::Dict result;
  for (i = 0; i != count; ++i) {
    std::optional<std::string> key = cxx_OOStringFromJSID(properties[i].atom);
    JSValue value = JS_GetProperty(context, tableObject, properties[i].atom);
    if (JS_IsException(value)) {
      JS_FreeValue(context, JS_GetException(context));
      continue;
    }

    if (key && !JS_IsUndefined(value)) {
      std::optional<std::string> string =
          cxx_OOStringFromJSValueEvenIfNull(context, value);
      if (string) {
        result.insert_or_assign(*key, oo::PList(std::move(*string)));
      }
    }
    JS_FreeValue(context, value);
  }

  JS_FreePropertyEnum(context, properties, count);
  JS_FreeValue(context, tableObject);
  return oo::PList(std::move(result));

  OOJS_PROFILE_EXIT_VAL(oo::PList())
}

namespace {
// Indexed by class ID; process lifetime.
OOJSClassConverterCallback sObjectConverters[kOOJSMaxClassID];
}  // namespace

JSClassID gOOJSObjectClassID, gOOJSStringClassID, gOOJSArrayClassID,
    gOOJSNumberClassID, gOOJSBooleanClassID;

oo::PList cxx_OOJSPListFromJSValue(JSContext* context, JSValueConst value) {
  OOJS_PROFILE_ENTER

  if (JS_IsNull(value) || JS_IsUndefined(value)) return oo::PList();

  if (JS_VALUE_GET_TAG(value) == JS_TAG_INT) {
    return oo::PList::signedInteger(JS_VALUE_GET_INT(value));
  }
  if (JS_IsNumber(value)) {
    double doubleValue = 0.0;
    if (JS_ToFloat64(context, &doubleValue, value) < 0) {
      JS_FreeValue(context, JS_GetException(context));
      return oo::PList();
    }
    return oo::PList(doubleValue);
  }
  if (JS_IsBool(value)) {
    return oo::PList(static_cast<bool>(JS_VALUE_GET_BOOL(value)));
  }
  if (JS_IsString(value)) {
    std::optional<std::string> string = cxx_OOStringFromJSValue(context, value);
    return string.has_value() ? oo::PList(std::move(*string)) : oo::PList();
  }
  if (JS_IsObject(value)) {
    return cxx_OOJSPListFromJSObject(context, value);
  }
  return oo::PList();

  OOJS_PROFILE_EXIT_VAL(oo::PList())
}

oo::PList cxx_OOJSPListFromJSObject(JSContext* context, JSValueConst object) {
  OOJS_PROFILE_ENTER

  if (!JS_IsObject(object)) return oo::PList();

  JSClassID classID = JS_GetClassID(object);
  OOJSClassConverterCallback converter =
      (classID < kOOJSMaxClassID) ? sObjectConverters[classID] : NULL;
  if (converter == NULL) return oo::PList();

  // The engine's own converters build the PList; a private-object converter
  // gives an Object node.
  return converter(context, object);

  OOJS_PROFILE_EXIT_VAL(oo::PList())
}

// The slot holds a C++ object: a node of it when it is a node's foreign
// object, else none; an empty slot is a null PList.
oo::PList OOJSBasicPrivateObjectConverter(JSContext* /*context*/,
                                          JSValueConst object) {
  JSClassID ignored;
  oo::RefCounted* held =
      static_cast<oo::RefCounted*>(JS_GetAnyOpaque(object, &ignored));
  if (oo::PListForeign* foreign = dynamic_cast<oo::PListForeign*>(held))
    return oo::PList(oo::PList::Object(foreign));
  return oo::PList();
}

void OOJSRegisterObjectConverter(JSClassID classID,
                                 OOJSClassConverterCallback converter) {
  if (classID == 0 || classID >= kOOJSMaxClassID) return;
  sObjectConverters[classID] = converter;
}

namespace {
oo::PList JSArrayConverter(JSContext* context, JSValueConst array) {
  int64_t i, count;

  // Convert a JS array to a native array by calling cxx_OOJSPListFromJSValue()
  // on all its elements.
  if (!JS_IsArray(array)) return oo::PList();
  if (JS_GetLength(context, array, &count) < 0) {
    JS_FreeValue(context, JS_GetException(context));
    return oo::PList();
  }

  oo::PList::Array values;
  values.reserve(static_cast<std::size_t>(count));
  for (i = 0; i != count; ++i) {
    JSValue value =
        JS_GetPropertyUint32(context, array, static_cast<uint32_t>(i));
    if (JS_IsException(value)) {
      JS_FreeValue(context, JS_GetException(context));
      value = JS_UNDEFINED;
    }

    oo::PList element = cxx_OOJSPListFromJSValue(context, value);
    JS_FreeValue(context, value);
    if (element.isNull()) {
      element = OONullObjectNode();  // [OONull null]
    }
    values.push_back(std::move(element));
  }

  return oo::PList(std::move(values));
}

oo::PList JSStringConverter(JSContext* context, JSValueConst object) {
  std::optional<std::string> string = cxx_OOStringFromJSValue(context, object);
  return string.has_value() ? oo::PList(std::move(*string)) : oo::PList();
}

oo::PList JSNumberConverter(JSContext* context, JSValueConst object) {
  double value;
  if (JS_ToFloat64(context, &value, object) == 0) {
    return oo::PList(value);
  }
  JS_FreeValue(context, JS_GetException(context));
  return oo::PList();
}

oo::PList JSBooleanConverter(JSContext* context, JSValueConst object) {
  /*	Fun With JavaScript: Boolean(false) is a truthy value, since it's a
          non-null object. However, Boolean objects are transformed to numbers
          sanely, so this works.
  */
  double value;
  if (JS_ToFloat64(context, &value, object) == 0) {
    return oo::PList(value != 0);
  }
  JS_FreeValue(context, JS_GetException(context));
  return oo::PList();
}

// A plain JS Object: cxx_OOJSDictionaryFromJSObject(); an integer-like key is a
// string.
oo::PList JSObjectConverter(JSContext* context, JSValueConst object) {
  /*	SpiderMonkey's global object had a class of its own, with no converter,
          so it converted to nil. QuickJS's is a plain object, and converting it
          as one recurses until the stack overflows, since globalThis refers to
          itself; a Timer whose this is the global object did that on firing.
  */
  JSValue global = JS_GetGlobalObject(context);
  bool isGlobal = JS_IsStrictEqual(context, global, object);
  JS_FreeValue(context, global);
  if (isGlobal) return oo::PList();

  return cxx_OOJSDictionaryFromJSObject(context, object);
}

void RegisterStandardObjectConverters(JSContext* context) {
  if (gOOJSObjectClassID == 0) {
    // The standard class IDs are not part of the QuickJS API, so look them up
    // from instances.
    const char* source =
        "[{}, new String(''), [], new Number(0), new Boolean(false)]";
    JSValue templates =
        JS_Eval(context, source, strlen(source), "<init>", JS_EVAL_TYPE_GLOBAL);
    JSClassID* ids[] = {&gOOJSObjectClassID, &gOOJSStringClassID,
                        &gOOJSArrayClassID, &gOOJSNumberClassID,
                        &gOOJSBooleanClassID};
    for (uint32_t i = 0; i < OOJS_COUNT(ids); i++) {
      JSValue item = JS_GetPropertyUint32(context, templates, i);
      *ids[i] = JS_GetClassID(item);
      JS_FreeValue(context, item);
    }
    JS_FreeValue(context, templates);
  }

  OOJSRegisterObjectConverter(gOOJSObjectClassID, JSObjectConverter);
  OOJSRegisterObjectConverter(gOOJSStringClassID, JSStringConverter);
  OOJSRegisterObjectConverter(gOOJSArrayClassID, JSArrayConverter);
  OOJSRegisterObjectConverter(gOOJSNumberClassID, JSNumberConverter);
  OOJSRegisterObjectConverter(gOOJSBooleanClassID, JSBooleanConverter);
}
}  // namespace
