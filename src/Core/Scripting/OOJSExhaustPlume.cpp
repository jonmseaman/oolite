/*
OOJSExhaustPlume.m

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

#include "OOJSExhaustPlume.h"

#include <cstdint>
#include <cstring>

#include "EntityOOJavaScriptExtensions.h"
#include "OOExhaustPlumeEntity.h"
#include "OOJSEntity.h"
#include "OOJSVector.h"
#include "OOJavaScriptEngine.h"
#include "ShipEntity.h"

namespace {
static JSClassID sExhaustPlumeClassID;
}  // namespace

namespace {
static bool JSExhaustPlumeGetExhaustPlumeEntity(
    JSContext* context, JSValueConst jsobj, OOExhaustPlumeEntity** outEntity);
}  // namespace

namespace {
static JSValue ExhaustPlumeGetProperty(OOJS_GETTER_ARGS);
}  // namespace
namespace {
static JSValue ExhaustPlumeSetProperty(OOJS_SETTER_ARGS);
}  // namespace

namespace {
static JSValue ExhaustPlumeRemove(OOJS_NATIVE_ARGS);
}  // namespace

namespace {
static JSClassDef sExhaustPlumeClass = {
    .class_name = "ExhaustPlume",
    .finalizer = OOJSObjectWrapperFinalize,
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kExhaustPlume_size
};

namespace {
static const JSCFunctionListEntry sExhaustPlumeProperties[] = {
    // JS name, getter, setter, ID
    OOJS_PROPERTY_RW("size", ExhaustPlumeGetProperty, ExhaustPlumeSetProperty,
                     kExhaustPlume_size),
};
}  // namespace

namespace {
static const JSCFunctionListEntry sExhaustPlumeMethods[] = {
    // JS name, min args, function
    JS_CFUNC_DEF("remove", 0, ExhaustPlumeRemove),
};
}  // namespace

void InitOOJSExhaustPlume(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sExhaustPlumeClassID, &sExhaustPlumeClass);

  JSValue parentPrototype = JS_GetClassProto(context, JSEntityClassID());
  OOCAssert(JS_IsObject(parentPrototype),
            "Entity must be initialized before ExhaustPlume.");
  OOJSInitClass(context, global, sExhaustPlumeClassID, parentPrototype,
                OOJSUnconstructableConstruct, 0, sExhaustPlumeProperties,
                OOJS_COUNT(sExhaustPlumeProperties), sExhaustPlumeMethods,
                OOJS_COUNT(sExhaustPlumeMethods), nullptr, 0);
  JS_FreeValue(context, parentPrototype);

  OOJSRegisterObjectConverter(sExhaustPlumeClassID, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(sExhaustPlumeClassID, JSEntityClassID());
}

JSClassID JSExhaustPlumeClassID(void) { return sExhaustPlumeClassID; }

namespace {
static bool JSExhaustPlumeGetExhaustPlumeEntity(
    JSContext* context, JSValueConst jsobj, OOExhaustPlumeEntity** outEntity) {
  OOJS_PROFILE_ENTER

  bool result;
  Entity* entity = nullptr;  // the slot holds the C++ entity

  if (outEntity == NULL) return false;
  *outEntity = nullptr;

  result = OOJSEntityGetEntity(context, jsobj, &entity);
  if (!result) return false;

  // A plume is the C++ OOExhaustPlumeEntity.
  OOExhaustPlumeEntity* exhaust = dynamic_cast<OOExhaustPlumeEntity*>(entity);
  if (exhaust == nullptr) return false;

  *outEntity = exhaust;
  return true;

  OOJS_PROFILE_EXIT
}
}  // namespace

// The bodies of OOExhaustPlumeEntity (OOJavaScriptExtensions), which the C++
// class's overrides of the root's JS members call.
std::optional<std::string> OOJSExhaustPlumeJSClassName(void) {
  return std::string("ExhaustPlume");
}

bool OOJSExhaustPlumeIsVisibleToScripts(void) { return true; }

namespace {
static JSValue ExhaustPlumeGetProperty(OOJS_GETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* entity = nullptr;

  if (!JSExhaustPlumeGetExhaustPlumeEntity(context, OOJS_THIS, &entity))
    return OOJSNativeFailure(context);
  if (entity == nullptr) return JS_UNDEFINED;

  switch (magic) {
    case kExhaustPlume_size:
      OOJS_RETURN_VECTOR(entity->scale());

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic,
                                    sExhaustPlumeProperties,
                                    OOJS_COUNT(sExhaustPlumeProperties));
      return OOJSNativeFailure(context);
  }

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static JSValue ExhaustPlumeSetProperty(OOJS_SETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* entity = nullptr;
  Vector vValue;

  if (!JSExhaustPlumeGetExhaustPlumeEntity(context, OOJS_THIS, &entity))
    return OOJSNativeFailure(context);
  if (entity == nullptr) return JS_UNDEFINED;

  switch (magic) {
    case kExhaustPlume_size:
      if (JSValueToVector(context, value, &vValue)) {
        entity->setScale(vValue);
        return JS_UNDEFINED;
      }
      break;

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic,
                                    sExhaustPlumeProperties,
                                    OOJS_COUNT(sExhaustPlumeProperties));
      return OOJSNativeFailure(context);
  }

  OOJSReportBadPropertyValue(context, OOJS_THIS, magic, sExhaustPlumeProperties,
                             OOJS_COUNT(sExhaustPlumeProperties), value);
  return OOJSNativeFailure(context);

  OOJS_NATIVE_EXIT
}
}  // namespace

// *** Methods ***

#define GET_THIS_EXHAUSTPLUME(THISENT)                                      \
  do {                                                                      \
    if (EXPECT_NOT(!JSExhaustPlumeGetExhaustPlumeEntity(context, OOJS_THIS, \
                                                        &(THISENT))))       \
      return OOJSNativeFailure(context);            /* Exception */         \
    if (OOIsStaleEntity(THISENT)) OOJS_RETURN_VOID; /* the C++ plume's */   \
  } while (0)

namespace {
static JSValue ExhaustPlumeRemove(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* thisEnt = nullptr;
  GET_THIS_EXHAUSTPLUME(thisEnt);

  ShipEntity* parent = oo::ToShip(thisEnt->owner());  // the C++ member
  if (parent != nullptr) parent->removeExhaust(thisEnt);

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}
}  // namespace
