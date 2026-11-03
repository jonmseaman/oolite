/*
OOJSDock.m

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

#include "OOJSDock.h"

#include <cstring>

#include "DockEntity.h"
#include "GameController.h"
#include "OOJSEntity.h"
#include "OOJSPlayer.h"
#include "OOJSShip.h"
#include "OOJavaScriptEngine.h"

namespace {
static JSClassID sDockClassID;
}  // namespace

namespace {
static bool JSDockGetDockEntity(JSContext* context, JSValueConst stationObj,
                                DockEntity** outEntity);
}  // namespace
namespace {
static bool JSDockGetShipEntity(JSContext* context, JSValueConst shipObj,
                                ShipEntity** outEntity);
}  // namespace

namespace {
static JSValue DockIsQueued(OOJS_NATIVE_ARGS);
}  // namespace

namespace {
static JSValue DockGetProperty(OOJS_GETTER_ARGS);
}  // namespace
namespace {
static JSValue DockSetProperty(OOJS_SETTER_ARGS);
}  // namespace

namespace {
static JSClassDef sDockClass = {
    .class_name = "Dock",
    .finalizer = OOJSObjectWrapperFinalize,
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kDock_allowsDocking,
  kDock_disallowedDockingCollides,
  kDock_allowsLaunching,
  kDock_dockingQueueLength,
  kDock_launchingQueueLength
};

namespace {
static const JSCFunctionListEntry sDockProperties[] = {
    // JS name, getter, setter, ID
    OOJS_PROPERTY_RW("allowsDocking", DockGetProperty, DockSetProperty,
                     kDock_allowsDocking),
    OOJS_PROPERTY_RW("disallowedDockingCollides", DockGetProperty,
                     DockSetProperty, kDock_disallowedDockingCollides),
    OOJS_PROPERTY_RW("allowsLaunching", DockGetProperty, DockSetProperty,
                     kDock_allowsLaunching),
    OOJS_PROPERTY_RO("dockingQueueLength", DockGetProperty,
                     kDock_dockingQueueLength),
    OOJS_PROPERTY_RO("launchingQueueLength", DockGetProperty,
                     kDock_launchingQueueLength),
};
}  // namespace

namespace {
static const JSCFunctionListEntry sDockMethods[] = {
    // JS name, min args, function
    JS_CFUNC_DEF("isQueued", 1, DockIsQueued),
};
}  // namespace

void InitOOJSDock(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sDockClassID, &sDockClass);

  JSValue parentPrototype = JS_GetClassProto(context, JSShipClassID());
  OOCAssert(JS_IsObject(parentPrototype),
            "Ship must be initialized before Dock.");
  OOJSInitClass(context, global, sDockClassID, parentPrototype,
                OOJSUnconstructableConstruct, 0, sDockProperties,
                OOJS_COUNT(sDockProperties), sDockMethods,
                OOJS_COUNT(sDockMethods), nullptr, 0);
  JS_FreeValue(context, parentPrototype);

  OOJSRegisterObjectConverter(sDockClassID, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(sDockClassID, JSShipClassID());
}

JSClassID JSDockClassID(void) { return sDockClassID; }

namespace {
static bool JSDockGetDockEntity(JSContext* context, JSValueConst dockObj,
                                DockEntity** outEntity) {
  OOJS_PROFILE_ENTER

  bool result;
  Entity* entity = nullptr;  // the slot holds the C++ entity

  if (outEntity == NULL) return false;
  *outEntity = nullptr;

  result = OOJSEntityGetEntity(context, dockObj, &entity);
  if (!result) return false;

  // A dock's object is a ship's: its C++ part says it is a dock.
  DockEntity* dock = oo::ToDock(entity);
  if (dock == nullptr) return false;

  *outEntity = dock;
  return true;

  OOJS_PROFILE_EXIT
}
}  // namespace

namespace {
static bool JSDockGetShipEntity(JSContext* context, JSValueConst shipObj,
                                ShipEntity** outEntity) {
  OOJS_PROFILE_ENTER

  bool result;
  Entity* entity = nullptr;  // the slot holds the C++ entity

  if (outEntity == NULL) return false;
  *outEntity = nullptr;

  result = OOJSEntityGetEntity(context, shipObj, &entity);
  if (!result) return false;

  if (!(oo::ToShip(entity) != nullptr)) return false;

  *outEntity = oo::ToShip(entity);
  return true;

  OOJS_PROFILE_EXIT
}
}  // namespace

// The bodies of DockEntity's class questions, which DockEntity's overrides
// call.
std::optional<std::string> OOJSDockJSClassName(void) {
  return std::string("Dock");
}

namespace {
static JSValue DockGetProperty(OOJS_GETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  DockEntity* entity = nullptr;

  if (!JSDockGetDockEntity(context, OOJS_THIS, &entity))
    return OOJSNativeFailure(context);
  if (entity == nullptr) return JS_UNDEFINED;

  switch (magic) {
    case kDock_allowsDocking:
      return OOJSValueFromBOOL(entity->allowsDocking());

    case kDock_disallowedDockingCollides:
      return OOJSValueFromBOOL(entity->disallowedDockingCollides());

    case kDock_allowsLaunching:
      return OOJSValueFromBOOL(entity->allowsLaunching());

    case kDock_dockingQueueLength:
      return JS_NewNumber(context, entity->countOfShipsInDockingQueue());

    case kDock_launchingQueueLength:
      return JS_NewNumber(context, entity->countOfShipsInLaunchQueue());

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic, sDockProperties,
                                    OOJS_COUNT(sDockProperties));
      return OOJSNativeFailure(context);
  }

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static JSValue DockSetProperty(OOJS_SETTER_ARGS) {
  OOJS_NATIVE_ENTER(context)

  DockEntity* entity = nullptr;
  int bValue;

  if (!JSDockGetDockEntity(context, OOJS_THIS, &entity))
    return OOJSNativeFailure(context);
  if (entity == nullptr) return JS_UNDEFINED;

  switch (magic) {
    case kDock_allowsDocking:
      if ((bValue = JS_ToBool(context, value)) >= 0) {
        entity->setAllowsDocking(bValue != 0);
        return JS_UNDEFINED;
      }
      break;

    case kDock_allowsLaunching:
      if ((bValue = JS_ToBool(context, value)) >= 0) {
        entity->setAllowsLaunching(bValue != 0);
        return JS_UNDEFINED;
      }
      break;

    case kDock_disallowedDockingCollides:
      if ((bValue = JS_ToBool(context, value)) >= 0) {
        entity->setDisallowedDockingCollides(bValue != 0);
        return JS_UNDEFINED;
      }
      break;

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic, sDockProperties,
                                    OOJS_COUNT(sDockProperties));
      return OOJSNativeFailure(context);
  }

  OOJSReportBadPropertyValue(context, OOJS_THIS, magic, sDockProperties,
                             OOJS_COUNT(sDockProperties), value);
  return OOJSNativeFailure(context);

  OOJS_NATIVE_EXIT
}
}  // namespace

// *** Methods ***

namespace {
static JSValue DockIsQueued(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  bool result = false;
  DockEntity* dock = nullptr;

  JSDockGetDockEntity(context, OOJS_THIS, &dock);
  // SpiderMonkey tolerated returning normally with an error pending; QuickJS
  // does not.
  if (JS_HasException(context)) return JS_EXCEPTION;
  if (argc == 0) {
    cxx_OOJSReportBadArguments(context, "Dock", "isQueued", MIN(argc, 1),
                               OOJS_ARGV, std::nullopt, "ship");
    return OOJSNativeFailure(context);
  }
  ShipEntity* ship = nullptr;
  // The SpiderMonkey version did not check that argv[0] is an object; a
  // non-object is treated as no ship.
  if (JS_IsObject(OOJS_ARGV[0])) {
    JSDockGetShipEntity(context, OOJS_ARGV[0], &ship);
    if (JS_HasException(context)) return JS_EXCEPTION;
  }
  if (ship != nullptr) {
    result = (dock != nullptr ? dock->shipIsInDockingQueue(ship) : false);
  }

  OOJS_RETURN_BOOL(result);

  OOJS_NATIVE_EXIT
}
}  // namespace
