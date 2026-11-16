/*

OOConstToJSString.h

Convert various sets of integer constants to JavaScript strings and back again.
See also: OOConstToString.h.


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

/*	Plain includes: this header and OOConstToJSString's translation unit are
   C++. QuickJS-ng, std::ptrdiff_t/std::size_t and the
   constant enumerations are all the inlines below need.
*/
#pragma once

#include <quickjs.h>

#include "EntityEnums.h"
#include "FunctionAttributes.h"
#include "Types.h"

void OOConstToJSStringInit(JSContext* context);
void OOConstToJSStringDestroy();

struct ConstTable;

/*	QuickJS has no JSString type, so the SpiderMonkey OOJSStringFromFoo()
   and OOFooFromJSString() variants are gone; OOJSValueFromFoo() returns an
   owned string value and OOFooFromJSValue() accepts any value (non-strings give
        the default).
*/

// Private functions, don't use directly.
JSValue OOJSValueFromConstantPRIVATE(JSContext* context, std::ptrdiff_t value,
                                     struct ConstTable* table);
std::size_t OOConstantFromJSValuePRIVATE(JSContext* context, JSValueConst value,
                                         struct ConstTable* table,
                                         std::ptrdiff_t defaultValue);

/*	JSValue OOJSValueFromEntityStatus(JSContext *, OOEntityStatus)
        OOEntityStatus OOEntityStatusFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOEntityStatus.
*/
inline JSValue OOJSValueFromEntityStatus(JSContext* context,
                                         oo::EntityStatus value) {
  extern struct ConstTable gOOEntityStatusConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOEntityStatusConstTable);
}

inline oo::EntityStatus OOEntityStatusFromJSValue(JSContext* context,
                                                  JSValueConst value) {
  extern struct ConstTable gOOEntityStatusConstTable;
  return static_cast<oo::EntityStatus>(
      OOConstantFromJSValuePRIVATE(context, value, &gOOEntityStatusConstTable,
                                   oo::ToUnderlying(oo::kEntityStatusDefault)));
}

/*	JSValue OOJSValueFromScanClass(JSContext *, OOScanClass)
        OOScanClass OOScanClassFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOScanClass.
*/
inline JSValue OOJSValueFromScanClass(JSContext* context, oo::ScanClass value) {
  extern struct ConstTable gOOScanClassConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOScanClassConstTable);
}

inline oo::ScanClass OOScanClassFromJSValue(JSContext* context,
                                            JSValueConst value) {
  extern struct ConstTable gOOScanClassConstTable;
  return static_cast<oo::ScanClass>(
      OOConstantFromJSValuePRIVATE(context, value, &gOOScanClassConstTable,
                                   oo::ToUnderlying(oo::kScanClassDefault)));
}

/*	JSValue OOJSValueFromCompassMode(JSContext *, OOCompassMode)
        OOCompassMode OOCompassModeFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOCompassMode.
*/
inline JSValue OOJSValueFromCompassMode(JSContext* context,
                                        oo::CompassMode value) {
  extern struct ConstTable gOOCompassModeConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOCompassModeConstTable);
}

inline oo::CompassMode OOCompassModeFromJSValue(JSContext* context,
                                                JSValueConst value) {
  extern struct ConstTable gOOCompassModeConstTable;
  return static_cast<oo::CompassMode>(
      OOConstantFromJSValuePRIVATE(context, value, &gOOCompassModeConstTable,
                                   oo::ToUnderlying(oo::kCompassModeDefault)));
}

/*	JSValue OOJSValueFromGUIScreenID(JSContext *, OOGUIScreenID)
        OOGUIScreenID OOGUIScreenIDFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOGUIScreenID.
*/
inline JSValue OOJSValueFromGUIScreenID(JSContext* context,
                                        oo::GUIScreenID value) {
  extern struct ConstTable gOOGUIScreenIDConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOGUIScreenIDConstTable);
}

inline oo::GUIScreenID OOGUIScreenIDFromJSValue(JSContext* context,
                                                JSValueConst value) {
  extern struct ConstTable gOOGUIScreenIDConstTable;
  return static_cast<oo::GUIScreenID>(
      OOConstantFromJSValuePRIVATE(context, value, &gOOGUIScreenIDConstTable,
                                   oo::ToUnderlying(oo::kGUIScreenIDDefault)));
}

/*	JSValue OOJSValueFromGalacticHyperspaceBehaviour(JSContext *,
   OOGalacticHyperspaceBehaviour) OOGalacticHyperspaceBehaviour
   OOGalacticHyperspaceBehaviourFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOGalacticHyperspaceBehaviour.
*/
inline JSValue OOJSValueFromGalacticHyperspaceBehaviour(
    JSContext* context, oo::GalacticHyperspaceBehaviour value) {
  extern struct ConstTable gOOGalacticHyperspaceBehaviourConstTable;
  return OOJSValueFromConstantPRIVATE(
      context, oo::ToUnderlying(value),
      &gOOGalacticHyperspaceBehaviourConstTable);
}

inline oo::GalacticHyperspaceBehaviour OOGalacticHyperspaceBehaviourFromJSValue(
    JSContext* context, JSValueConst value) {
  extern struct ConstTable gOOGalacticHyperspaceBehaviourConstTable;
  return static_cast<oo::GalacticHyperspaceBehaviour>(
      OOConstantFromJSValuePRIVATE(
          context, value, &gOOGalacticHyperspaceBehaviourConstTable,
          oo::ToUnderlying(oo::kGalacticHyperspaceBehaviourDefault)));
}

/*	JSValue OOJSValueFromViewID(JSContext *, OOViewID)
        OOViewID OOViewIDFromJSValue(JSContext *, JSValueConst)

        Convert between JavaScript strings and OOViewID.
*/
inline JSValue OOJSValueFromViewID(JSContext* context, oo::ViewID value) {
  extern struct ConstTable gOOViewIDConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOViewIDConstTable);
}

inline oo::ViewID OOViewIDFromJSValue(JSContext* context, JSValueConst value) {
  extern struct ConstTable gOOViewIDConstTable;
  return static_cast<oo::ViewID>(
      OOConstantFromJSValuePRIVATE(context, value, &gOOViewIDConstTable,
                                   oo::ToUnderlying(oo::kViewIDDefault)));
}

/*	JSValue OOJSValueFromShipDamageType(JSContext *, OOShipDamageType)

        Convert OOShipDamageType to JavaScript strings.
*/
inline JSValue OOJSValueFromShipDamageType(JSContext* context,
                                           oo::ShipDamageType value) {
  extern struct ConstTable gOOShipDamageTypeConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOShipDamageTypeConstTable);
}

/*	JSValue OOJSValueFromLegalStatusReason(JSContext *, OOLegalStatusReason)

        Convert OOLegalStatusReason to JavaScript strings.
*/
inline JSValue OOJSValueFromLegalStatusReason(JSContext* context,
                                              oo::LegalStatusReason value) {
  extern struct ConstTable gOOLegalStatusReasonConstTable;
  return OOJSValueFromConstantPRIVATE(context, oo::ToUnderlying(value),
                                      &gOOLegalStatusReasonConstTable);
}
