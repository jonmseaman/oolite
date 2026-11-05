/*
OOJSWormhole.mm

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

#include "OOJSWormhole.h"

#include <cstdint>
#include <cstring>

#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSVector.h"
#include "OOJSWormhole.h"
#include "OOJavaScriptEngine.h"
#include "WormholeEntity.h"

/*
 * OOJS_NATIVE_ENTER/EXIT and OOJS_PROFILE_ENTER/EXIT are C++ try/catch and
 * scope guards (OOJSEngineNativeWrappers.h). The category on WormholeEntity is
 * free functions, whose methods are on the WormholeEntity facade in
 * WormholeEntity+ObjCBridge.mm.
 */

static JSClassID sWormholeClassID;

namespace {
static bool JSWormholeGetWormholeEntity(JSContext* context,
                                        JSValueConst wormholeObj,
                                        WormholeEntity** outEntity);
}  // namespace

namespace {
static JSValue WormholeGetProperty(OOJS_GETTER_ARGS);
}  // namespace

namespace {
static JSClassDef sWormholeClass = {
    .class_name = "Wormhole",
    .finalizer =
        OOJSObjectWrapperFinalize,  // the slot holds an OOJSEntityHolder
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kWormhole_arrivalTime,
  kWormhole_destination,
  kWormhole_expiryTime,
  kWormhole_origin

};

/*	All Wormhole properties are read-only, so the SpiderMonkey setProperty
   hook (which only reported a bad property selector) was unreachable for them
        and is not ported.
*/
namespace {
static const JSCFunctionListEntry sWormholeProperties[] = {
    // JS name, getter, setter, ID
    OOJS_PROPERTY_RO("arrivalTime", WormholeGetProperty, kWormhole_arrivalTime),
    OOJS_PROPERTY_RO("destination", WormholeGetProperty, kWormhole_destination),
    OOJS_PROPERTY_RO("expiryTime", WormholeGetProperty, kWormhole_expiryTime),
    OOJS_PROPERTY_RO("origin", WormholeGetProperty, kWormhole_origin),
};
}  // namespace

// No Wormhole methods.

void InitOOJSWormhole(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sWormholeClassID, &sWormholeClass);

  JSValue parentPrototype = JS_GetClassProto(context, JSEntityClassID());
  OOCAssert(JS_IsObject(parentPrototype),
            "Entity must be initialized before Wormhole.");
  OOJSInitClass(context, global, sWormholeClassID, parentPrototype,
                OOJSUnconstructableConstruct, 0, sWormholeProperties,
                OOJS_COUNT(sWormholeProperties), NULL, 0, NULL, 0);
  JS_FreeValue(context, parentPrototype);

  OOJSRegisterObjectConverter(sWormholeClassID, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(sWormholeClassID, JSEntityClassID());
}

JSClassID JSWormholeClassID(void) { return sWormholeClassID; }

namespace {
static bool JSWormholeGetWormholeEntity(JSContext* context,
                                        JSValueConst wormholeObj,
                                        WormholeEntity** outEntity) {
  OOJS_PROFILE_ENTER

  bool result;
  Entity* entity = nullptr;  // the slot holds the C++ entity

  if (outEntity == NULL) return false;
  *outEntity = nullptr;

  result = OOJSEntityGetEntity(context, wormholeObj, &entity);
  if (!result) return false;

  // The wormhole is C++ behind the root's facade.
  *outEntity = dynamic_cast<WormholeEntity*>(entity);
  return *outEntity != nullptr;

  OOJS_PROFILE_EXIT
}
}  // namespace

// The bodies of WormholeEntity (OOJavaScriptExtensions), whose methods are on
// the WormholeEntity facade, in WormholeEntity+ObjCBridge.mm.
std::optional<std::string> OOJSWormholeJSClassName(void) {
  return std::string("Wormhole");
}

bool OOJSWormholeIsVisibleToScripts(void) { return true; }

namespace {
static JSValue WormholeGetProperty(OOJS_GETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  WormholeEntity* entity = nullptr;

  if (!JSWormholeGetWormholeEntity(context, OOJS_THIS, &entity))
    return OOJSNativeFailure(context);
  if (entity == nullptr) {
    return JS_UNDEFINED;
  }

  switch (magic) {
    case kWormhole_arrivalTime:
      return JS_NewNumber(context, entity->arrivalTime());

    case kWormhole_destination:
      return JS_NewNumber(context, entity->getDestination());

    case kWormhole_expiryTime:
      return JS_NewNumber(context, entity->expiryTime());

    case kWormhole_origin:
      return JS_NewNumber(context, entity->getOrigin());

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic,
                                    sWormholeProperties,
                                    OOJS_COUNT(sWormholeProperties));
      return OOJSNativeFailure(context);
  }

  return JS_NULL;  // what OOJSValueFromNativeObject() gave for nil (no case
                   // reaches it)

  OOJS_NATIVE_EXIT
}
}  // namespace
