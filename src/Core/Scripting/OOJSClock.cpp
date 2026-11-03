/*

OOJSClock.cpp


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

#include "OOJSClock.h"

#include <cstring>

#include "OODebugStandards.h"
#include "OOJSPlayer.h"
#include "OOJavaScriptEngine.h"
#include "OOStringParsing.h"
#include "PlayerEntity.h"
#include "PlayerEntityScriptMethods.h"
#include "Universe.h"

namespace {
JSValue ClockGetProperty(OOJS_GETTER_ARGS);

// Methods
JSValue JSClockToString(OOJS_NATIVE_ARGS);
JSValue ClockClockStringForTime(OOJS_NATIVE_ARGS);
JSValue ClockAddSeconds(OOJS_NATIVE_ARGS);

JSClassID sClockClassID;
JSClassDef sClockClass = {
    .class_name = "Clock",
};

enum : std::uint8_t {
  // Property IDs
  kClock_absoluteSeconds,  // game real time clock, double, read-only
  kClock_seconds,          // game clock time, double, read-only
  kClock_minutes,  // game clock time minutes (rounded down), integer double,
                   // read-only
  kClock_hours,    // game clock time hours (rounded down), integer double,
                   // read-only
  kClock_days,     // game clock time days (rounded down), int, read-only
  kClock_secondsComponent,  // second component of game clock time, double,
                            // read-only
  kClock_minutesComponent,  // minute component of game clock time (rounded
                            // down), int, read-only
  kClock_hoursComponent,    // hour component of game clock time (rounded down),
                            // int, read-only
  kClock_daysComponent,     // day component of game clock time (rounded down),
                            // int, read-only
  kClock_clockString,  // game clock time as display string, string, read-only
  kClock_isAdjusting,  // clock is adjusting, boolean, read-only
  kClock_adjustedSeconds,    // game clock time, including adjustments, double,
                             // read-only
  kClock_legacy_scriptTimer  // legacy scriptTimer_number, double, read-only
};

const JSCFunctionListEntry sClockProperties[] = {
    // JS name, getter, ID
    OOJS_PROPERTY_RO("absoluteSeconds", ClockGetProperty,
                     kClock_absoluteSeconds),
    OOJS_PROPERTY_RO("seconds", ClockGetProperty, kClock_seconds),
    OOJS_PROPERTY_RO("minutes", ClockGetProperty, kClock_minutes),
    OOJS_PROPERTY_RO("hours", ClockGetProperty, kClock_hours),
    OOJS_PROPERTY_RO("days", ClockGetProperty, kClock_days),
    OOJS_PROPERTY_RO("secondsComponent", ClockGetProperty,
                     kClock_secondsComponent),
    OOJS_PROPERTY_RO("minutesComponent", ClockGetProperty,
                     kClock_minutesComponent),
    OOJS_PROPERTY_RO("hoursComponent", ClockGetProperty, kClock_hoursComponent),
    OOJS_PROPERTY_RO("daysComponent", ClockGetProperty, kClock_daysComponent),
    OOJS_PROPERTY_RO("clockString", ClockGetProperty, kClock_clockString),
    OOJS_PROPERTY_RO("isAdjusting", ClockGetProperty, kClock_isAdjusting),
    OOJS_PROPERTY_RO("adjustedSeconds", ClockGetProperty,
                     kClock_adjustedSeconds),
    OOJS_PROPERTY_RO("legacy_scriptTimer", ClockGetProperty,
                     kClock_legacy_scriptTimer),
};

const JSCFunctionListEntry sClockMethods[] = {
    // JS name, min args, function
    JS_CFUNC_DEF("toString", 0, JSClockToString),
    JS_CFUNC_DEF("clockStringForTime", 1, ClockClockStringForTime),
    JS_CFUNC_DEF("addSeconds", 1, ClockAddSeconds),
};
}  // namespace

void InitOOJSClock(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sClockClassID, &sClockClass);
  JSValueConst clockPrototype =
      OOJSInitClass(context, global, sClockClassID, JS_UNDEFINED,
                    OOJSUnconstructableConstruct, 0, sClockProperties,
                    OOJS_COUNT(sClockProperties), sClockMethods,
                    OOJS_COUNT(sClockMethods), NULL, 0);
  // JS_DefinePropertyValueStr() takes ownership of the clock object.
  JS_DefinePropertyValueStr(
      context, global, "clock",
      JS_NewObjectProtoClass(context, clockPrototype, sClockClassID),
      OOJS_PROP_READONLY);
}

namespace {
JSValue ClockGetProperty(OOJS_GETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  PlayerEntity* player = OOPlayerForScripting();
  double clockTime;

  clockTime = (player != nullptr ? player->PlayerEntity::clockTime()
                                 : 0.0);  // qualified: the final overrider, so
                                          // the binding test stands in for it

  // INT_TO_JSVAL() truncated its (double) argument to an integer, as the casts
  // to int32_t below do.
  switch (magic) {
    case kClock_absoluteSeconds:
      return JS_NewNumber(context,
                          (UNIVERSE != nullptr ? UNIVERSE->getTime() : 0.0));

    case kClock_seconds:
      return JS_NewNumber(context, clockTime);

    case kClock_minutes:
      return JS_NewNumber(context, floor(clockTime / 60.0));

    case kClock_hours:
      return JS_NewNumber(context, floor(clockTime / 3600.0));

    case kClock_secondsComponent:
      return JS_NewInt32(context, static_cast<int32_t>(fmod(clockTime, 60.0)));

    case kClock_minutesComponent:
      return JS_NewInt32(
          context, static_cast<int32_t>(fmod(floor(clockTime / 60.0), 60.0)));

    case kClock_hoursComponent:
      return JS_NewInt32(
          context, static_cast<int32_t>(fmod(floor(clockTime / 3600.0), 24.0)));

    case kClock_days:
    case kClock_daysComponent:
      return JS_NewInt32(context,
                         static_cast<int32_t>(floor(clockTime / 86400.0)));

    case kClock_clockString:
      OOJS_RETURN_STRING_OR_NULL(
          (player != nullptr ? player->dial_clock() : std::string()));

    case kClock_isAdjusting:
      return OOJSValueFromBOOL(
          (player != nullptr ? player->clockAdjusting() : false));

    case kClock_adjustedSeconds:
      return JS_NewNumber(
          context,
          (player != nullptr ? player->PlayerEntity::clockTimeAdjusted()
                             : 0.0));  // qualified: the final overrider, so
                                       // the binding test stands in for it

    case kClock_legacy_scriptTimer:
      cxx_OOStandardsDeprecated(
          "The legacy_scriptTimer property is deprecated");
      return JS_NewNumber(
          context, (player != nullptr ? player->scriptTimer() : OOTimeDelta{}));

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic, sClockProperties,
                                    OOJS_COUNT(sClockProperties));
      return JS_EXCEPTION;
  }

  OOJS_NATIVE_EXIT
}

// *** Methods ***

// toString() : String
JSValue JSClockToString(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  PlayerEntity* player = OOPlayerForScripting();
  OOJS_RETURN_STRING_OR_NULL(
      (player != nullptr ? player->dial_clock() : std::string()));

  OOJS_NATIVE_EXIT
}

// clockStringForTime(time : Number) : String
JSValue ClockClockStringForTime(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  double time;

  if (EXPECT_NOT(argc < 1 || JS_ToFloat64(context, &time, OOJS_ARGV[0]) < 0)) {
    if (JS_HasException(context))
      JS_FreeValue(context, JS_GetException(context));
    JSValueConst arg = JS_UNDEFINED;
    if (argc > 0) arg = OOJS_ARGV[0];
    cxx_OOJSReportBadArguments(context, "Clock", "clockStringForTime", 1, &arg,
                               std::nullopt, "number");
    return JS_EXCEPTION;
  }

  OOJS_RETURN_STRING_OR_NULL(cxx_ClockToString(time, false));

  OOJS_NATIVE_EXIT
}

// addSeconds(seconds : Number) : String
JSValue ClockAddSeconds(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  double time;
  const double kMaxTime = 30.0 * 24.0 * 3600.0;  // 30 days

  if (EXPECT_NOT(argc < 1 || JS_ToFloat64(context, &time, OOJS_ARGV[0]) < 0)) {
    if (JS_HasException(context))
      JS_FreeValue(context, JS_GetException(context));
    JSValueConst arg = JS_UNDEFINED;
    if (argc > 0) arg = OOJS_ARGV[0];
    cxx_OOJSReportBadArguments(context, "Clock", "addSeconds", 1, &arg,
                               std::nullopt, "number");
    return JS_EXCEPTION;
  }

  if (time > kMaxTime || time < 1.0 || !isfinite(time)) {
    OOJS_RETURN_BOOL(false);
  }

  if (PlayerEntity* player = OOPlayerForScripting())
    player->addToAdjustTime(time);

  OOJS_RETURN_BOOL(true);

  OOJS_NATIVE_EXIT
}
}  // namespace
