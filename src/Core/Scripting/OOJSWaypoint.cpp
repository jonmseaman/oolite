/*
OOJSWaypoint.m

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

#include <cstdint>
#include <cstring>

#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSQuaternion.h"
#include "OOJSVector.h"
#include "OOJSWaypoint.h"
#include "OOJavaScriptEngine.h"
#include "OOWaypointEntity.h"
#include "ooscript/JSEngine.hpp"

/*
        Retargeted onto the ooscript facade (JSEngine.hpp): the class dispatch
   table is a static ooscript::ClassDef (the stub hooks are nullptr), InitClass
   is ooscript::initClass, and the two directly spelled numeric-conversion
   calls are ooscript::newNumberValue/valueToNumber. `this` is spelled
   `thisObj`.

        Waypoint is registered as an Entity subclass and object converter with
   &sWaypointClass, the same ooscript::ClassDef that ooscript::getClass()
   reports for its instances.

        OOJS_NATIVE_ENTER/EXIT and OOJS_PROFILE_ENTER/EXIT are C++ try/catch
   and scope guards (OOJSEngineNativeWrappers.h). The category on
   OOWaypointEntity is three free functions, whose methods and interface are on
   the OOWaypointEntity facade in OOWaypointEntity+ObjCBridge.mm. Messages to
   classes that are still Objective-C (OOWaypointEntity, Entity, Universe,
   PlayerEntity) go through their Objective-C interfaces.
*/

namespace ooscript {}
using ooscript::CallArgs;
using ooscript::ClassDef;
using ooscript::ClassFlag;
using ooscript::Context;
using ooscript::FunctionSpec;
using ooscript::Object;
using ooscript::PropertyFlag;
using ooscript::PropertyId;
using ooscript::PropertySpec;
using ooscript::Value;

// Byte-identical façade <-> jsapi views, local to this call site (see
// OOJSVector.mm).

namespace {
static ooscript::Object sWaypointPrototype;
}  // namespace

namespace {
static bool JSWaypointGetWaypointEntity(ooscript::Context context,
                                        ooscript::Object stationObj,
                                        OOWaypointEntity** outEntity);
}  // namespace

namespace {
static bool WaypointGetProperty(Context cx, Object obj, PropertyId propID,
                                Value* value);
}  // namespace
namespace {
static bool WaypointSetProperty(Context cx, Object obj, PropertyId propID,
                                bool strict, Value* value);
}  // namespace

namespace {
static ClassDef sWaypointClass = {
    "Waypoint",
    ClassFlag::HasPrivate,

    nullptr,              // addProperty (engine default: PropertyStub)
    nullptr,              // delProperty (engine default: PropertyStub)
    WaypointGetProperty,  // getProperty
    WaypointSetProperty,  // setProperty
    nullptr,              // enumerate (engine default: EnumerateStub)
    nullptr,  // newEnumerate (ooscript::ClassFlag::NewEnumerate not used)
    nullptr,  // resolve (engine default: ResolveStub)
    nullptr,  // convert (engine default: ConvertStub)
    OOJSCxxObjectWrapperFinalize,  // finalize
    nullptr,                       // call
    nullptr,                       // construct
    nullptr,  // backend: owned by the façade backend, must start null
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kWaypoint_beaconCode,
  kWaypoint_beaconLabel,
  kWaypoint_orientation,  // overrides entity as waypoints can be unoriented
  kWaypoint_size
};

namespace {
static PropertySpec sWaypointProperties[] = {
    // JS name							ID
    // flags								getter
    // setter
    {"beaconCode", kWaypoint_beaconCode,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"beaconLabel", kWaypoint_beaconLabel,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"orientation", kWaypoint_orientation,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"size", kWaypoint_size,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {0}};
}  // namespace

// A raw jsapi mirror of sWaypointProperties, used only for the two bad-property
// error reporters in OOJavaScriptEngine.m
// (OOJSReportBadPropertySelector/Value): those helpers are shared across every
// binding file and still take a ooscript::PropertySpec*.
namespace {
static ooscript::PropertySpec sWaypointPropertiesRaw[] = {
    // JS name							ID
    // flags
    {"beaconCode", kWaypoint_beaconCode, OOJS_PROP_READWRITE_CB},
    {"beaconLabel", kWaypoint_beaconLabel, OOJS_PROP_READWRITE_CB},
    {"orientation", kWaypoint_orientation, OOJS_PROP_READWRITE_CB},
    {"size", kWaypoint_size, OOJS_PROP_READWRITE_CB},
    {0}};
}  // namespace

namespace {
static FunctionSpec sWaypointMethods[] = {
    // JS name					Function
    // min args	flags
    //	{ "",     WaypointDoStuff,    0,	0 },
    {0}};
}  // namespace

void InitOOJSWaypoint(ooscript::Context context, ooscript::Object global) {
  Object proto = ooscript::initClass(
      (context), (global), (JSEntityPrototype()), &sWaypointClass,
      OOJSUnconstructableConstruct, 0, sWaypointProperties, sWaypointMethods,
      nullptr, nullptr);
  sWaypointPrototype = (proto);
  OOJSRegisterObjectConverter(&sWaypointClass, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(&sWaypointClass, JSEntityClass());
}

namespace {
static bool JSWaypointGetWaypointEntity(ooscript::Context context,
                                        ooscript::Object wormholeObj,
                                        OOWaypointEntity** outEntity) {
  OOJS_PROFILE_ENTER

  bool result;
  Entity* entity = nullptr;  // the slot holds the C++ entity

  if (outEntity == NULL) return false;
  *outEntity = nullptr;

  result = OOJSEntityGetEntity(context, wormholeObj, &entity);
  if (!result) return false;

  // A waypoint is the C++ OOWaypointEntity (a nil or other entity is not). The
  // natives call its members.
  OOWaypointEntity* waypoint = dynamic_cast<OOWaypointEntity*>(entity);
  if (waypoint == nullptr) return false;

  *outEntity = waypoint;
  return true;

  OOJS_PROFILE_EXIT
}
}  // namespace

// The bodies of OOWaypointEntity (OOJavaScriptExtensions), which the C++
// class's overrides of the root's JS members call.
void OOJSWaypointGetJSClass(ooscript::ClassDef** outClass,
                            ooscript::Object* outPrototype) {
  *outClass = &sWaypointClass;
  *outPrototype = sWaypointPrototype;
}

std::optional<std::string> OOJSWaypointJSClassName(void) {
  return std::string("Waypoint");
}

bool OOJSWaypointIsVisibleToScripts(void) { return true; }

namespace {
static bool WaypointGetProperty(Context cx, Object obj, PropertyId propID,
                                Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = reinterpret_cast<ooscript::Context>(cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = reinterpret_cast<ooscript::Value*>(value);

  OOJS_NATIVE_ENTER(context)

  OOWaypointEntity* waypoint = nullptr;
  oo::PList result;  // null: nil
  std::optional<std::string> text;
  Quaternion q = kIdentityQuaternion;

  if (!JSWaypointGetWaypointEntity(context, thisObj, &waypoint))
    return false;  // not null after this

  switch (ooscript::idToInt32(propID)) {
    case kWaypoint_beaconCode:
      text = waypoint->beaconCode();
      if (text.has_value()) result = oo::PList(*text);
      break;

    case kWaypoint_beaconLabel:
      text = waypoint->beaconLabel();
      if (text.has_value()) result = oo::PList(*text);
      break;

    case kWaypoint_orientation:
      q = waypoint
              ->getOrientation();  // what the object's -orientation answered
      if (!waypoint->getOriented()) {
        q = kZeroQuaternion;
      }
      return QuaternionToJSValue(context, q, value_raw);

    case kWaypoint_size:
      return ooscript::newNumberValue(cx, waypoint->size(), value);

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sWaypointPropertiesRaw);
      return false;
  }

  *value_raw = OOJSValueFromPList(context, result);
  return true;

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static bool WaypointSetProperty(Context cx, Object obj, PropertyId propID,
                                bool /*strict*/, Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = reinterpret_cast<ooscript::Context>(cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = reinterpret_cast<ooscript::Value*>(value);

  OOJS_NATIVE_ENTER(context)

  OOWaypointEntity* waypoint = nullptr;
  double fValue;
  std::optional<std::string> sValue;
  Quaternion qValue;

  if (!JSWaypointGetWaypointEntity(context, thisObj, &waypoint))
    return false;  // not null after this

  switch (ooscript::idToInt32(propID)) {
    case kWaypoint_beaconCode:
      sValue = cxx_OOStringFromJSValue(context, *value_raw);
      if (!sValue.has_value() || sValue->empty()) {
        if (waypoint->isBeacon()) {
          if (UNIVERSE != nullptr) UNIVERSE->clearBeacon(waypoint);
          if ((PLAYER != nullptr ? PLAYER->nextBeaconEntity() : nullptr) ==
              waypoint)  // not virtual, so the binding test stands in for it
          {
            if (PLAYER != nullptr)
              PLAYER->PlayerEntity::setCompassMode(
                  COMPASS_MODE_PLANET);  // qualified: the final overrider, so
                                         // the binding test stands in for it
          }
        }
      } else {
        if (waypoint->isBeacon()) {
          waypoint->setBeaconCode(sValue);
        } else  // Universe needs to update beacon lists in this case only
        {
          waypoint->setBeaconCode(sValue);
          if (UNIVERSE != nullptr) UNIVERSE->setNextBeacon(waypoint);
        }
      }
      return true;
      break;

    case kWaypoint_beaconLabel:
      sValue = cxx_OOStringFromJSValue(context, *value_raw);
      if (sValue.has_value()) {
        waypoint->setBeaconLabel(sValue);
        return true;
      }
      break;

    case kWaypoint_orientation:
      if (JSValueToQuaternion(context, *value_raw, &qValue)) {
        waypoint->Entity::setNormalOrientation(
            qValue);  // what the object's selector called; qualified: the final
                      // overrider for a waypoint (no override, no subclass), so
                      // the binding test stands in for it
        return true;
      }
      break;

    case kWaypoint_size:
      if (ooscript::valueToNumber(cx, *value, &fValue)) {
        if (fValue > 0.0) {
          waypoint->setSize(fValue);
          return true;
        }
      }
      break;

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sWaypointPropertiesRaw);
      return false;
  }

  OOJSReportBadPropertyValue(context, thisObj, (propID), sWaypointPropertiesRaw,
                             *value_raw);
  return false;

  OOJS_NATIVE_EXIT
}
}  // namespace

// *** Methods ***
