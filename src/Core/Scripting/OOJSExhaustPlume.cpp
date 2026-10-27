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

#include <cstdint>
#include <cstring>

#include "EntityOOJavaScriptExtensions.h"
#include "OOExhaustPlumeEntity.h"
#include "OOJSEntity.h"
#include "OOJSExhaustPlume.h"
#include "OOJSVector.h"
#include "OOJavaScriptEngine.h"
#include "ShipEntity.h"
#include "ooscript/JSEngine.hpp"

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

namespace {
static ooscript::Object sExhaustPlumePrototype;
}  // namespace

namespace {
static bool JSExhaustPlumeGetExhaustPlumeEntity(
    ooscript::Context context, ooscript::Object jsobj,
    OOExhaustPlumeEntity** outEntity);
}  // namespace

namespace {
static bool ExhaustPlumeGetProperty(Context cx, Object obj, PropertyId propID,
                                    Value* value);
}  // namespace
namespace {
static bool ExhaustPlumeSetProperty(Context cx, Object obj, PropertyId propID,
                                    bool strict, Value* value);
}  // namespace

namespace {
static bool ExhaustPlumeRemove(ooscript::Context cx,
                               ooscript::CallArgs& oojsArgs);
}  // namespace

namespace {
static ClassDef sExhaustPlumeClass = {
    "ExhaustPlume",
    ClassFlag::HasPrivate,

    nullptr,                  // addProperty (engine default: PropertyStub)
    nullptr,                  // delProperty (engine default: PropertyStub)
    ExhaustPlumeGetProperty,  // getProperty
    ExhaustPlumeSetProperty,  // setProperty
    nullptr,                  // enumerate (engine default: EnumerateStub)
    nullptr,  // newEnumerate (ooscript::ClassFlag::NewEnumerate not used)
    nullptr,  // resolve (engine default: ResolveStub)
    nullptr,  // convert (engine default: ConvertStub)
    OOJSCxxObjectWrapperFinalize,  // finalize
    nullptr,                       // call
    nullptr,                       // construct
    nullptr,  // backend: owned by the facade backend, must start null
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kExhaustPlume_size
};

// A mirror of sExhaustPlumeProperties with the read-only/read-write flags the
// two bad-property error reporters in OOJavaScriptEngine.mm
// (OOJSReportBadPropertySelector/Value) describe the properties by (see
// OOJSVector.mm's sVectorPropertiesRaw).
namespace {
static ooscript::PropertySpec sExhaustPlumePropertiesRaw[] = {
    // JS name							ID
    // flags
    {"size", kExhaustPlume_size, OOJS_PROP_READWRITE_CB},
    {0}};
}  // namespace

namespace {
static PropertySpec sExhaustPlumeProperties[] = {
    // JS name							ID
    // flags getter		setter
    {"size", kExhaustPlume_size,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {0}};
}  // namespace

namespace {
static FunctionSpec sExhaustPlumeMethods[] = {
    // JS name					Function
    // min args	flags
    {"remove", ExhaustPlumeRemove, 0, 0},

    {0}};
}  // namespace

void InitOOJSExhaustPlume(ooscript::Context context, ooscript::Object global) {
  Object proto = ooscript::initClass(
      (context), (global), (JSEntityPrototype()), &sExhaustPlumeClass,
      OOJSUnconstructableConstruct, 0, sExhaustPlumeProperties,
      sExhaustPlumeMethods, nullptr, nullptr);
  sExhaustPlumePrototype = (proto);
  OOJSRegisterObjectConverter(&sExhaustPlumeClass, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(&sExhaustPlumeClass, JSEntityClass());
}

namespace {
static bool JSExhaustPlumeGetExhaustPlumeEntity(
    ooscript::Context context, ooscript::Object jsobj,
    OOExhaustPlumeEntity** outEntity) {
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
void OOJSExhaustPlumeGetJSClass(ooscript::ClassDef** outClass,
                                ooscript::Object* outPrototype) {
  *outClass = &sExhaustPlumeClass;
  *outPrototype = sExhaustPlumePrototype;
}

std::optional<std::string> OOJSExhaustPlumeJSClassName(void) {
  return std::string("ExhaustPlume");
}

bool OOJSExhaustPlumeIsVisibleToScripts(void) { return true; }

namespace {
static bool ExhaustPlumeGetProperty(Context cx, Object obj, PropertyId propID,
                                    Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = (cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = (value);

  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* entity = nullptr;

  if (!JSExhaustPlumeGetExhaustPlumeEntity(context, thisObj, &entity))
    return false;
  if (entity == nullptr) {
    *value_raw = ooscript::undefinedValue();
    return true;
  }

  switch (ooscript::idToInt32(propID)) {
    case kExhaustPlume_size:
      return VectorToJSValue(context, entity->scale(), value_raw);

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sExhaustPlumePropertiesRaw);
      return false;
  }

  *value_raw = ooscript::nullValue();  // unreached: every case returns
  return true;

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static bool ExhaustPlumeSetProperty(Context cx, Object obj, PropertyId propID,
                                    bool /*strict*/, Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = (cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = (value);

  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* entity = nullptr;
  Vector vValue;

  if (!JSExhaustPlumeGetExhaustPlumeEntity(context, thisObj, &entity))
    return false;
  if (entity == nullptr) return true;

  switch (ooscript::idToInt32(propID)) {
    case kExhaustPlume_size:
      if (JSValueToVector(context, *value_raw, &vValue)) {
        entity->setScale(vValue);
        return true;
      }
      break;

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sExhaustPlumePropertiesRaw);
      return false;
  }

  OOJSReportBadPropertyValue(context, thisObj, (propID),
                             sExhaustPlumePropertiesRaw, *value_raw);
  return false;

  OOJS_NATIVE_EXIT
}
}  // namespace

// *** Methods ***

#define GET_THIS_EXHAUSTPLUME(THISENT)                                      \
  do {                                                                      \
    if (EXPECT_NOT(!JSExhaustPlumeGetExhaustPlumeEntity(context, OOJS_THIS, \
                                                        &(THISENT))))       \
      return false;                                 /* Exception */         \
    if (OOIsStaleEntity(THISENT)) OOJS_RETURN_VOID; /* the C++ plume's */   \
  } while (0)

namespace {
static bool ExhaustPlumeRemove(ooscript::Context context,
                               ooscript::CallArgs& oojsArgs) {
  OOJS_NATIVE_ENTER(context)

  OOExhaustPlumeEntity* thisEnt = nullptr;
  GET_THIS_EXHAUSTPLUME(thisEnt);

  ShipEntity* parent = oo::ToShip(thisEnt->owner());  // the C++ member
  if (parent != nullptr) parent->removeExhaust(thisEnt);

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}
}  // namespace
