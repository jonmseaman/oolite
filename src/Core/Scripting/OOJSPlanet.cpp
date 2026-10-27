/*

OOJSPlanet.mm


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

#include "OOJSEntity.h"
#include "OOJSPlanet.h"
#include "OOJSQuaternion.h"
#include "OOJSVector.h"
#include "OOJavaScriptEngine.h"
#include "OOPlanetEntity.h"
#include "ooscript/JSEngine.hpp"

namespace ooscript {}
using ooscript::CallArgs;
using ooscript::ClassDef;
using ooscript::ClassFlag;
using ooscript::Context;
using ooscript::Object;
using ooscript::PropertyFlag;
using ooscript::PropertyId;
using ooscript::PropertySpec;
using ooscript::Value;

// Byte-identical facade <-> jsapi views, local to this call site (see
// OOJSVector.mm).

namespace {
static ooscript::Object sPlanetPrototype;
}  // namespace

namespace {

namespace {

// A colour's components as its -normalizedArray gave them to JavaScript:
// floats, null for no colour.
oo::PList NormalizedColorComponents(OOColor* color) {
  if (color == nullptr) return oo::PList();
  oo::PList::Array components;
  for (float component : color->normalizedArray())
    components.push_back(oo::PList::singleReal(component));
  return oo::PList(std::move(components));
}

}  // namespace

static bool PlanetGetProperty(Context cx, Object obj, PropertyId propID,
                              Value* value);
}  // namespace
namespace {
static bool PlanetSetProperty(Context cx, Object obj, PropertyId propID,
                              bool strict, Value* value);
}  // namespace

namespace {
static ClassDef sPlanetClass = {
    "Planet",
    ClassFlag::HasPrivate,

    nullptr,            // addProperty
    nullptr,            // delProperty
    PlanetGetProperty,  // getProperty
    PlanetSetProperty,  // setProperty
    nullptr,            // enumerate
    nullptr,  // newEnumerate (ooscript::ClassFlag::NewEnumerate not used)
    nullptr,  // resolve
    nullptr,  // convert
    OOJSCxxObjectWrapperFinalize,  // finalize
    nullptr,                       // call
    nullptr,                       // construct
    nullptr,  // backend: owned by the facade backend, must start null
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kPlanet_airColor,          // air color, read/write
  kPlanet_airColorMixRatio,  // air color mix ratio, float, read/write
  kPlanet_airDensity,        // air density, float, read/write
  kPlanet_hasAtmosphere,
  kPlanet_illuminationColor,   // illumination color, read/write
  kPlanet_isMainPlanet,        // Is [UNIVERSE planet], boolean, read-only
  kPlanet_name,                // Name of planet, string, read/write
  kPlanet_radius,              // Radius of planet in metres, read-only
  kPlanet_texture,             // Planet texture read / write
  kPlanet_orientation,         // orientation, quaternion, read/write
  kPlanet_rotationalVelocity,  // read/write
  kPlanet_terminatorThresholdVector,
};

namespace {
static PropertySpec sPlanetProperties[] = {
    // JS name						ID
    // flags
    // getter	setter
    {"airColor", kPlanet_airColor,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"airColorMixRatio", kPlanet_airColorMixRatio,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"airDensity", kPlanet_airDensity,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"hasAtmosphere", kPlanet_hasAtmosphere, OOJS_PROP_READONLY_CB, nullptr,
     nullptr},
    {"illuminationColor", kPlanet_illuminationColor,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"isMainPlanet", kPlanet_isMainPlanet, OOJS_PROP_READONLY_CB, nullptr,
     nullptr},
    {"name", kPlanet_name,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"radius", kPlanet_radius, OOJS_PROP_READONLY_CB, nullptr, nullptr},
    {"rotationalVelocity", kPlanet_rotationalVelocity,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"texture", kPlanet_texture,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {"orientation", kPlanet_orientation,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},  // Not documented since it's inherited from Entity
    {"terminatorThresholdVector", kPlanet_terminatorThresholdVector,
     PropertyFlag::Permanent | PropertyFlag::Enumerate | PropertyFlag::Shared,
     nullptr, nullptr},
    {0}};
}  // namespace

// Raw jsapi mirror of sPlanetProperties for the shared error reporters that
// still take a ooscript::PropertySpec* (see OOJSVector.mm's
// sVectorPropertiesRaw).
namespace {
static ooscript::PropertySpec sPlanetPropertiesRaw[] = {
    // JS name						ID
    // flags
    {"airColor", kPlanet_airColor, OOJS_PROP_READWRITE_CB},
    {"airColorMixRatio", kPlanet_airColorMixRatio, OOJS_PROP_READWRITE_CB},
    {"airDensity", kPlanet_airDensity, OOJS_PROP_READWRITE_CB},
    {"hasAtmosphere", kPlanet_hasAtmosphere, OOJS_PROP_READONLY_CB},
    {"illuminationColor", kPlanet_illuminationColor, OOJS_PROP_READWRITE_CB},
    {"isMainPlanet", kPlanet_isMainPlanet, OOJS_PROP_READONLY_CB},
    {"name", kPlanet_name, OOJS_PROP_READWRITE_CB},
    {"radius", kPlanet_radius, OOJS_PROP_READONLY_CB},
    {"rotationalVelocity", kPlanet_rotationalVelocity, OOJS_PROP_READWRITE_CB},
    {"texture", kPlanet_texture, OOJS_PROP_READWRITE_CB},
    {"orientation", kPlanet_orientation,
     OOJS_PROP_READWRITE_CB},  // Not documented since it's inherited from
                               // Entity
    {"terminatorThresholdVector", kPlanet_terminatorThresholdVector,
     OOJS_PROP_READWRITE_CB},
    {0}};
}  // namespace

namespace {
// The getter checks the JS class and answers the C++ planet (the slot holds the
// C++ entity, read by OOJSEntityGetEntityOfClass for the Planet class); null
// for a stale entity.
bool JSPlanetGetPlanetEntity(ooscript::Context context,
                             ooscript::Object inObject,
                             OOPlanetEntity** outObject) {
  Entity* entity = nullptr;
  if (!OOJSEntityGetEntityOfClass(context, inObject, &sPlanetClass, &entity))
    return false;
  *outObject = dynamic_cast<OOPlanetEntity*>(entity);
  return true;
}
}  // namespace

void InitOOJSPlanet(ooscript::Context context, ooscript::Object global) {
  Object proto =
      ooscript::initClass((context), (global), (JSEntityPrototype()),
                          &sPlanetClass, OOJSUnconstructableConstruct, 0,
                          sPlanetProperties, nullptr, nullptr, nullptr);
  sPlanetPrototype = (proto);
  OOJSRegisterObjectConverter(&sPlanetClass, OOJSEntityObjectConverter);
  OOJSRegisterSubclass(&sPlanetClass, JSEntityClass());
}

// The bodies of OOPlanetEntity (OOJavaScriptExtensions), called by the C++
// planet's overrides.
bool OOJSPlanetIsVisibleToScripts(OOPlanetEntity* planet) {
  OOStellarBodyType type =
      (planet != nullptr ? planet->planetType() : (OOStellarBodyType)0);
  return type == STELLAR_TYPE_NORMAL_PLANET || type == STELLAR_TYPE_MOON;
}

void OOJSPlanetGetJSClass(ooscript::ClassDef** outClass,
                          ooscript::Object* outPrototype) {
  *outClass = &sPlanetClass;
  *outPrototype = sPlanetPrototype;
}

std::optional<std::string> OOJSPlanetJSClassName(OOPlanetEntity* planet) {
  switch ((planet != nullptr ? planet->planetType() : (OOStellarBodyType)0)) {
    case STELLAR_TYPE_NORMAL_PLANET:
      return std::string("Planet");
    case STELLAR_TYPE_MOON:
      return std::string("Moon");
    default:
      return std::string("Unknown");
  }
}

namespace {
static bool PlanetGetProperty(Context cx, Object obj, PropertyId propID,
                              Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = (cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = (value);

  OOJS_NATIVE_ENTER(context)

  OOPlanetEntity* planet = nil;
  if (!JSPlanetGetPlanetEntity(context, thisObj, &planet)) return false;

  switch (ooscript::idToInt32(propID)) {
    case kPlanet_airColor:
      *value_raw = OOJSValueFromPList(
          context,
          NormalizedColorComponents(
              (planet != nullptr ? planet->airColor() : (OOColor*)nullptr)));
      return true;

    case kPlanet_airColorMixRatio:
      return ooscript::newNumberValue(
          cx, (planet != nullptr ? planet->airColorMixRatio() : 0.0f), value);

    case kPlanet_airDensity:
      return ooscript::newNumberValue(
          cx, (planet != nullptr ? planet->airDensity() : 0.0f), value);

    case kPlanet_illuminationColor:
      *value_raw = OOJSValueFromPList(
          context, NormalizedColorComponents((planet != nullptr
                                                  ? planet->illuminationColor()
                                                  : (OOColor*)nullptr)));
      return true;

    case kPlanet_isMainPlanet:
      *value_raw = OOJSValueFromBOOL(
          planet == (UNIVERSE != nullptr ? UNIVERSE->planet() : nullptr));
      return true;

    case kPlanet_radius:
      return ooscript::newNumberValue(
          cx, (planet != nullptr ? planet->radius() : 0.0), value);

    case kPlanet_hasAtmosphere:
      *value_raw = OOJSValueFromBOOL(
          (planet != nullptr ? planet->hasAtmosphere() : false));
      return true;

    case kPlanet_texture: {
      const std::optional<std::string> textureName =
          (planet != nullptr ? planet->textureFileName()
                             : std::optional<std::string>());
      *value_raw = OOJSValueFromPList(context, textureName.has_value()
                                                   ? oo::PList(*textureName)
                                                   : oo::PList());
    }
      return true;

    case kPlanet_name: {
      const std::optional<std::string> name =
          (planet != nullptr ? planet->name() : std::optional<std::string>());
      *value_raw = OOJSValueFromPList(
          context, name.has_value() ? oo::PList(*name) : oo::PList());
    }
      return true;

    case kPlanet_orientation:
      // The C++ member, qualified: the final overrider for a planet
      // (OOPlanetEntity has no subclass and does not override it), so the
      // binding test stands in for it; zero for none.
      return QuaternionToJSValue(context,
                                 planet != nullptr
                                     ? planet->Entity::normalOrientation()
                                     : Quaternion{},
                                 value_raw);

    case kPlanet_rotationalVelocity:
      return ooscript::newNumberValue(
          cx, (planet != nullptr ? planet->rotationalVelocity() : 0.0), value);

    case kPlanet_terminatorThresholdVector:
      return VectorToJSValue(
          context,
          (planet != nullptr ? planet->terminatorThresholdVector() : Vector{}),
          value_raw);

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sPlanetPropertiesRaw);
      return false;
  }

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static bool PlanetSetProperty(Context cx, Object obj, PropertyId propID,
                              bool /*strict*/, Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = (cx);
  ooscript::Object thisObj = (obj);
  ooscript::Value* value_raw = (value);

  OOJS_NATIVE_ENTER(context)

  OOPlanetEntity* planet = nil;
  std::optional<std::string> sValue;
  Quaternion qValue;
  Vector vValue;
  double dValue;
  oo::Ref<OOColor> colorForScript;

  if (!JSPlanetGetPlanetEntity(context, thisObj, &planet)) return false;

  switch (ooscript::idToInt32(propID)) {
    case kPlanet_airColor:
      colorForScript = OOColor::colorWithDescription(
          cxx_OOJSPListFromJSValue(context, *value_raw));
      if (colorForScript != nullptr || ooscript::isNull(*value_raw)) {
        if (planet != nullptr) planet->setAirColor(colorForScript.get());
        return true;
      }
      break;

    case kPlanet_airColorMixRatio:
      if (ooscript::valueToNumber(cx, *value, &dValue)) {
        if (planet != nullptr) planet->setAirColorMixRatio(dValue);
        return true;
      }
      break;

    case kPlanet_airDensity:
      if (ooscript::valueToNumber(cx, *value, &dValue)) {
        if (planet != nullptr) planet->setAirDensity(dValue);
        return true;
      }
      break;

    case kPlanet_illuminationColor:
      colorForScript = OOColor::colorWithDescription(
          cxx_OOJSPListFromJSValue(context, *value_raw));
      if (colorForScript != nullptr || ooscript::isNull(*value_raw)) {
        if (planet != nullptr)
          planet->setIlluminationColor(colorForScript.get());
        return true;
      }
      break;

    case kPlanet_name:
      sValue = cxx_OOStringFromJSValue(context, *value_raw);
      if (planet != nullptr) planet->setName(sValue);
      return true;

    case kPlanet_texture: {
      bool OK = false;
      sValue = cxx_OOStringFromJSValue(context, *value_raw);

      OOJSPauseTimeLimiter();

      if (planet != nullptr)  // was -isKindOfClass: of the facade class: false
                              // only for nil
      {
        if (!sValue.has_value()) {
          cxx_OOJSReportWarning(context,
                                "Expected texture string. Value not set.");
        } else {
          OK = true;
        }
      }

      if (OK) {
        OK = (planet != nullptr ? planet->setUpPlanetFromTexture(sValue)
                                : false);  // has a value here
        if (!OK)
          cxx_OOJSReportWarning(context,
                                "Cannot find texture \"%s\". Value not set.",
                                sValue->c_str());
      }

      OOJSResumeTimeLimiter();

      return true;  // Even if !OK, no exception was raised.
    }

    case kPlanet_orientation:
      if (JSValueToQuaternion(context, *value_raw, &qValue)) {
        quaternion_normalize(&qValue);
        // The C++ member, qualified: the final overrider (OOPlanetEntity has no
        // subclass), so the binding test stands in for it; nothing for none.
        if (planet != nullptr) planet->OOPlanetEntity::setOrientation(qValue);
        return true;
      }
      break;

    case kPlanet_rotationalVelocity:
      if (ooscript::valueToNumber(cx, *value, &dValue)) {
        if (planet != nullptr) planet->setRotationalVelocity(dValue);
        return true;
      }
      break;

    case kPlanet_terminatorThresholdVector:
      if (JSValueToVector(context, *value_raw, &vValue)) {
        if (planet != nullptr) planet->setTerminatorThresholdVector(vValue);
        return true;
      }
      break;

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sPlanetPropertiesRaw);
      return false;
  }

  OOJSReportBadPropertyValue(context, thisObj, (propID), sPlanetPropertiesRaw,
                             *value_raw);
  return false;

  OOJS_NATIVE_EXIT
}
}  // namespace
