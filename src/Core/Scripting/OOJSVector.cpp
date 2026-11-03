/*

OOJSVector.cpp

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

#include "OOJSVector.h"

#include <cstdlib>
#include <cstring>

#include "OOConstToString.h"
#include "OOJSEntity.h"
#include "OOJSQuaternion.h"
#include "OOJavaScriptEngine.h"
#include "oofnd/String.hpp"

/*
 * The Vector3D class, in the design of upstream #768's OOJSVector.m: the
 * private slot (QuickJS's opaque pointer) holds a malloced HPVector, freed by
 * the finalizer; the prototype is an instance with no slot, which converts to
 * the zero vector. The statistics reporting that was a category on
 * PlayerEntity is two free functions, which the C++ PlayerEntity's members
 * call.
 */

namespace {
JSClassID sVectorClassID;

bool GetThisVector(JSContext* context, JSValueConst vectorObj,
                   HPVector* outVector, const std::string& method) NONNULL_FUNC;

JSValue VectorGetProperty(OOJS_GETTER_ARGS);
JSValue VectorSetProperty(OOJS_SETTER_ARGS);
void VectorFinalize(JSRuntime* runtime, JSValueConst thisObj);
JSValue VectorConstruct(OOJS_NATIVE_ARGS);

// Methods
JSValue VectorToString(OOJS_NATIVE_ARGS);
JSValue VectorToSource(OOJS_NATIVE_ARGS);
JSValue VectorAdd(OOJS_NATIVE_ARGS);
JSValue VectorSubtract(OOJS_NATIVE_ARGS);
JSValue VectorDistanceTo(OOJS_NATIVE_ARGS);
JSValue VectorSquaredDistanceTo(OOJS_NATIVE_ARGS);
JSValue VectorMultiply(OOJS_NATIVE_ARGS);
JSValue VectorDot(OOJS_NATIVE_ARGS);
JSValue VectorAngleTo(OOJS_NATIVE_ARGS);
JSValue VectorFromCoordinateSystem(OOJS_NATIVE_ARGS);
JSValue VectorToCoordinateSystem(OOJS_NATIVE_ARGS);
JSValue VectorCross(OOJS_NATIVE_ARGS);
JSValue VectorTripleProduct(OOJS_NATIVE_ARGS);
JSValue VectorDirection(OOJS_NATIVE_ARGS);
JSValue VectorMagnitude(OOJS_NATIVE_ARGS);
JSValue VectorSquaredMagnitude(OOJS_NATIVE_ARGS);
JSValue VectorRotationTo(OOJS_NATIVE_ARGS);
JSValue VectorRotateBy(OOJS_NATIVE_ARGS);
JSValue VectorToArray(OOJS_NATIVE_ARGS);

// Static methods
JSValue VectorStaticInterpolate(OOJS_NATIVE_ARGS);
JSValue VectorStaticRandom(OOJS_NATIVE_ARGS);
JSValue VectorStaticRandomDirection(OOJS_NATIVE_ARGS);
JSValue VectorStaticRandomDirectionAndLength(OOJS_NATIVE_ARGS);

JSClassDef sVectorClass = {
    .class_name = "Vector3D",
    .finalizer = VectorFinalize,
};

enum : std::uint8_t {
  // Property IDs
  kVector_x,
  kVector_y,
  kVector_z
};

const JSCFunctionListEntry sVectorProperties[] = {
    // JS name, getter, setter, ID
    OOJS_PROPERTY_RW("x", VectorGetProperty, VectorSetProperty, kVector_x),
    OOJS_PROPERTY_RW("y", VectorGetProperty, VectorSetProperty, kVector_y),
    OOJS_PROPERTY_RW("z", VectorGetProperty, VectorSetProperty, kVector_z),
};

const JSCFunctionListEntry sVectorMethods[] = {
    // JS name, min args, function
    JS_CFUNC_DEF("toSource", 0, VectorToSource),
    JS_CFUNC_DEF("toString", 0, VectorToString),
    JS_CFUNC_DEF("add", 1, VectorAdd),
    JS_CFUNC_DEF("angleTo", 1, VectorAngleTo),
    JS_CFUNC_DEF("cross", 1, VectorCross),
    JS_CFUNC_DEF("direction", 0, VectorDirection),
    JS_CFUNC_DEF("distanceTo", 1, VectorDistanceTo),
    JS_CFUNC_DEF("dot", 1, VectorDot),
    JS_CFUNC_DEF("fromCoordinateSystem", 1, VectorFromCoordinateSystem),
    JS_CFUNC_DEF("magnitude", 0, VectorMagnitude),
    JS_CFUNC_DEF("multiply", 1, VectorMultiply),
    JS_CFUNC_DEF("rotateBy", 1, VectorRotateBy),
    JS_CFUNC_DEF("rotationTo", 1, VectorRotationTo),
    JS_CFUNC_DEF("squaredDistanceTo", 1, VectorSquaredDistanceTo),
    JS_CFUNC_DEF("squaredMagnitude", 0, VectorSquaredMagnitude),
    JS_CFUNC_DEF("subtract", 1, VectorSubtract),
    JS_CFUNC_DEF("toArray", 0, VectorToArray),
    JS_CFUNC_DEF("toCoordinateSystem", 1, VectorToCoordinateSystem),
    JS_CFUNC_DEF("tripleProduct", 2, VectorTripleProduct),
};

const JSCFunctionListEntry sVectorStaticMethods[] = {
    // JS name, min args, function
    JS_CFUNC_DEF("interpolate", 3, VectorStaticInterpolate),
    JS_CFUNC_DEF("random", 0, VectorStaticRandom),
    JS_CFUNC_DEF("randomDirection", 0, VectorStaticRandomDirection),
    JS_CFUNC_DEF("randomDirectionAndLength", 0,
                 VectorStaticRandomDirectionAndLength),
};
}  // namespace

// *** Public ***

void InitOOJSVector(JSContext* context, JSValueConst global) {
  OOJSRegisterClass(&sVectorClassID, &sVectorClass);

  // As in SpiderMonkey, the prototype is itself an instance with no private
  // data, treated as zero.
  OOJSInitClass(context, global, sVectorClassID, JS_UNDEFINED, VectorConstruct,
                0, sVectorProperties, OOJS_COUNT(sVectorProperties),
                sVectorMethods, OOJS_COUNT(sVectorMethods),
                sVectorStaticMethods, OOJS_COUNT(sVectorStaticMethods));
}

JSValue JSVectorWithVector(JSContext* context, Vector vector) {
  return JSVectorWithHPVector(context, vectorToHPVector(vector));
}

bool VectorToJSValue(JSContext* context, Vector vector, JSValue* outValue) {
  assert(outValue != NULL);

  return HPVectorToJSValue(context, vectorToHPVector(vector), outValue);
}

JSValue JSVectorWithHPVector(JSContext* context, HPVector vector) {
  OOJS_PROFILE_ENTER

  HPVector* priv = static_cast<HPVector*>(malloc(sizeof *priv));
  if (EXPECT_NOT(priv == NULL)) return JS_ThrowOutOfMemory(context);

  *priv = vector;

  JSValue result = JS_NewObjectClass(context, static_cast<int>(sVectorClassID));
  if (EXPECT_NOT(JS_IsException(result))) {
    free(priv);
    return result;
  }
  JS_SetOpaque(result, priv);

  return result;

  OOJS_PROFILE_EXIT_JSVAL
}

bool HPVectorToJSValue(JSContext* context, HPVector vector, JSValue* outValue) {
  OOJS_PROFILE_ENTER

  assert(outValue != NULL);

  JSValue object = JSVectorWithHPVector(context, vector);
  if (EXPECT_NOT(JS_IsException(object))) {
    *outValue = JS_UNDEFINED;
    return false;
  }

  *outValue = object;
  return true;

  OOJS_PROFILE_EXIT
}

bool NSPointToVectorJSValue(JSContext* context, NSPoint point,
                            JSValue* outValue) {
  return VectorToJSValue(context, make_vector(point.x, point.y, 0), outValue);
}

bool JSValueToHPVector(JSContext* context, JSValueConst value,
                       HPVector* outVector) {
  if (EXPECT_NOT(!JS_IsObject(value))) return false;

  return JSObjectGetVector(context, value, outVector);
}

bool JSValueToVector(JSContext* context, JSValueConst value,
                     Vector* outVector) {
  if (EXPECT_NOT(!JS_IsObject(value))) return false;
  HPVector tmp = kZeroHPVector;
  bool result = JSObjectGetVector(context, value, &tmp);
  *outVector = HPVectorToVector(tmp);
  return result;
}

#if OO_DEBUG

typedef struct {
  NSUInteger vectorCount;
  NSUInteger entityCount;
  NSUInteger arrayCount;
  NSUInteger protoCount;
  NSUInteger nullCount;
  NSUInteger failCount;
} VectorStatistics;
namespace {
VectorStatistics sVectorConversionStats;
}  // namespace

// The bodies of the C++ PlayerEntity's statistics members.
oo::PList reportJSVectorStatistics(void) {
  VectorStatistics* stats = &sVectorConversionStats;

  NSUInteger sum = stats->vectorCount + stats->entityCount + stats->arrayCount +
                   stats->protoCount;
  double convFac = 100.0 / sum;
  if (sum == 0) convFac = 0;

  return oo::PList(oo::str::format(
      " vector-to-vector conversions: %zu (%g %%)\n"
      " entity-to-vector conversions: %zu (%g %%)\n"
      "  array-to-vector conversions: %zu (%g %%)\n"
      "prototype-to-zero conversions: %zu (%g %%)\n"
      "             null conversions: %zu (%g %%)\n"
      "           failed conversions: %zu (%g %%)\n"
      "                        total: %zu",
      (size_t)stats->vectorCount, stats->vectorCount * convFac,
      (size_t)stats->entityCount, stats->entityCount * convFac,
      (size_t)stats->arrayCount, stats->arrayCount * convFac,
      (size_t)stats->protoCount, stats->protoCount * convFac,
      (size_t)stats->nullCount, stats->nullCount * convFac,
      (size_t)stats->failCount, stats->failCount * convFac, (size_t)sum));
}

void clearJSVectorStatistics(void) {
  memset(&sVectorConversionStats, 0, sizeof sVectorConversionStats);
}

#define COUNT(FIELD)                \
  do {                              \
    sVectorConversionStats.FIELD++; \
  } while (0)

#else

#define COUNT(FIELD) \
  do {               \
  } while (0)

#endif

namespace {
// Get array element as a number. Like JS_ValueToNumber(), non-numbers give NaN
// rather than failing.
bool GetArrayElementNumber(JSContext* context, JSValueConst array,
                           uint32_t index, double* outNumber) {
  JSValue element = JS_GetPropertyUint32(context, array, index);
  int status =
      JS_IsException(element) ? -1 : JS_ToFloat64(context, outNumber, element);
  JS_FreeValue(context, element);
  return status == 0;
}
}  // namespace

bool JSObjectGetVector(JSContext* context, JSValueConst vectorObj,
                       HPVector* outVector) {
  OOJS_PROFILE_ENTER

  assert(outVector != NULL);

  HPVector* priv = NULL;
  int64_t arrayLength;
  double x, y, z;

  // vectorObj can legitimately be null or a primitive.
  if (EXPECT_NOT(!JS_IsObject(vectorObj))) {
    COUNT(nullCount);
    return false;
  }

  // If this is a (JS) Vector...
  priv = static_cast<HPVector*>(JS_GetOpaque(vectorObj, sVectorClassID));
  if (EXPECT(priv != NULL)) {
    COUNT(vectorCount);
    *outVector = *priv;
    return true;
  }

  // If it's an array...
  if (EXPECT(JS_IsArray(vectorObj))) {
    // ...and it has exactly three elements...
    if (JS_GetLength(context, vectorObj, &arrayLength) == 0 &&
        arrayLength == 3) {
      // ...use the three numbers as [x, y, z]
      if (GetArrayElementNumber(context, vectorObj, 0, &x) &&
          GetArrayElementNumber(context, vectorObj, 1, &y) &&
          GetArrayElementNumber(context, vectorObj, 2, &z)) {
        COUNT(arrayCount);
        *outVector = make_HPvector(x, y, z);
        return true;
      }
    }
  }

  // If it's an entity, use its position (zero for a stale one).
  if (OOJSIsMemberOfSubclass(context, vectorObj, JSEntityClassID())) {
    COUNT(entityCount);
    Entity* entity = nullptr;
    if (!OOJSEntityGetEntity(context, vectorObj, &entity)) entity = nullptr;
    *outVector = (entity != nullptr) ? entity->getPosition() : HPVector{};
    return true;
  }

  /*
          If it's actually a Vector3D but with no private field (this happens
     for Vector3D.prototype)...

          NOTE: it would be prettier to do this at the top when we handle normal
          Vector3Ds, but it's a rare case which should be kept off the fast
     path.
  */
  if (JS_GetClassID(vectorObj) == sVectorClassID) {
    COUNT(protoCount);
    *outVector = kZeroHPVector;
    return true;
  }

  COUNT(failCount);
  return false;

  OOJS_PROFILE_EXIT
}

namespace {
bool GetThisVector(JSContext* context, JSValueConst vectorObj,
                   HPVector* outVector, const std::string& method) {
  if (EXPECT(JSObjectGetVector(context, vectorObj, outVector))) return true;

  JSValueConst arg = vectorObj;
  cxx_OOJSReportBadArguments(context, "Vector3D", method, 1, &arg,
                             "Invalid target object", "Vector3D");
  return false;
}
}  // namespace

bool JSVectorSetVector(JSContext* context, JSValueConst vectorObj,
                       Vector vector) {
  return JSVectorSetHPVector(context, vectorObj, vectorToHPVector(vector));
}

bool JSVectorSetHPVector(JSContext* /*context*/, JSValueConst vectorObj,
                         HPVector vector) {
  OOJS_PROFILE_ENTER

  HPVector* priv = NULL;

  if (EXPECT_NOT(!JS_IsObject(vectorObj))) return false;

  priv = static_cast<HPVector*>(JS_GetOpaque(vectorObj, sVectorClassID));
  if (priv != NULL)  // If this is a (JS) Vector...
  {
    *priv = vector;
    return true;
  }

  if (JS_GetClassID(vectorObj) == sVectorClassID) {
    // Silently fail for the prototype.
    return true;
  }

  return false;

  OOJS_PROFILE_EXIT
}

namespace {
bool VectorFromArgumentListNoErrorInternal(JSContext* context, int argc,
                                           JSValueConst* argv,
                                           HPVector* outVector,
                                           int* outConsumed,
                                           bool permitNumberList) {
  OOJS_PROFILE_ENTER

  double x, y, z;

  if (EXPECT_NOT(argc == 0)) return false;
  assert(argv != NULL && outVector != NULL);

  if (outConsumed != NULL) *outConsumed = 0;

  // Is first object a vector, array or entity?
  if (JS_IsObject(argv[0])) {
    if (JSObjectGetVector(context, argv[0], outVector)) {
      if (outConsumed != NULL) *outConsumed = 1;
      return true;
    }
  }

  if (!permitNumberList) return false;

  // As a special case for VectorConstruct(), look for three numbers.
  if (argc < 3) return false;

  // Given a string, JS_ToFloat64() succeeds but provides a NaN number.
  if (EXPECT_NOT(JS_ToFloat64(context, &x, argv[0]) < 0 || isnan(x)))
    return false;
  if (EXPECT_NOT(JS_ToFloat64(context, &y, argv[1]) < 0 || isnan(y)))
    return false;
  if (EXPECT_NOT(JS_ToFloat64(context, &z, argv[2]) < 0 || isnan(z)))
    return false;

  // We got our three numbers.
  *outVector = make_HPvector(x, y, z);
  if (outConsumed != NULL) *outConsumed = 3;

  return true;

  OOJS_PROFILE_EXIT
}
}  // namespace

// EMMSTRAN: remove outConsumed, since it can only be 1 except in failure
// (constructor is an exception, but it uses
// VectorFromArgumentListNoErrorInternal() directly).
bool VectorFromArgumentList(JSContext* context, const std::string& scriptClass,
                            const std::string& function, int argc,
                            JSValueConst* argv, HPVector* outVector,
                            int* outConsumed) {
  if (VectorFromArgumentListNoErrorInternal(context, argc, argv, outVector,
                                            outConsumed, false))
    return true;
  else {
    cxx_OOJSReportBadArguments(context, scriptClass, function, argc, argv,
                               "Could not construct vector from parameters",
                               "Vector, Entity or array of three numbers");
    return false;
  }
}

bool VectorFromArgumentListNoError(JSContext* context, int argc,
                                   JSValueConst* argv, HPVector* outVector,
                                   int* outConsumed) {
  return VectorFromArgumentListNoErrorInternal(context, argc, argv, outVector,
                                               outConsumed, false);
}

// *** Implementation stuff ***

namespace {
JSValue VectorGetProperty(OOJS_GETTER_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector vector;
  OOHPScalar fValue;

  if (EXPECT_NOT(!JSObjectGetVector(context, OOJS_THIS, &vector)))
    return JS_UNDEFINED;

  switch (magic) {
    case kVector_x:
      fValue = vector.x;
      break;

    case kVector_y:
      fValue = vector.y;
      break;

    case kVector_z:
      fValue = vector.z;
      break;

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic,
                                    sVectorProperties,
                                    OOJS_COUNT(sVectorProperties));
      return JS_EXCEPTION;
  }

  return JS_NewFloat64(context, fValue);

  OOJS_PROFILE_EXIT_JSVAL
}

JSValue VectorSetProperty(OOJS_SETTER_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector vector;
  double dval;

  if (EXPECT_NOT(!JSObjectGetVector(context, OOJS_THIS, &vector)))
    return JS_UNDEFINED;
  if (EXPECT_NOT(JS_ToFloat64(context, &dval, value) < 0)) {
    JS_FreeValue(context, JS_GetException(context));
    OOJSReportBadPropertyValue(context, OOJS_THIS, magic, sVectorProperties,
                               OOJS_COUNT(sVectorProperties), value);
    return JS_EXCEPTION;
  }

  switch (magic) {
    case kVector_x:
      vector.x = dval;
      break;

    case kVector_y:
      vector.y = dval;
      break;

    case kVector_z:
      vector.z = dval;
      break;

    default:
      OOJSReportBadPropertySelector(context, OOJS_THIS, magic,
                                    sVectorProperties,
                                    OOJS_COUNT(sVectorProperties));
      return JS_EXCEPTION;
  }

  JSVectorSetHPVector(context, OOJS_THIS, vector);
  return JS_UNDEFINED;

  OOJS_PROFILE_EXIT_JSVAL
}

void VectorFinalize(JSRuntime* /*runtime*/, JSValueConst thisObj) {
  OOJS_PROFILE_ENTER

  free(JS_GetOpaque(thisObj, sVectorClassID));

  OOJS_PROFILE_EXIT_VOID
}

JSValue VectorConstruct(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector vector = kZeroHPVector;

  if (argc != 0) {
    if (EXPECT_NOT(!VectorFromArgumentListNoErrorInternal(
            context, argc, OOJS_ARGV, &vector, NULL, true))) {
      cxx_OOJSReportBadArguments(context, std::nullopt, std::nullopt, argc,
                                 OOJS_ARGV,
                                 "Could not construct vector from parameters",
                                 "Vector, Entity or array of three numbers");
      return JS_EXCEPTION;
    }
  }

  return JSVectorWithHPVector(context, vector);

  OOJS_PROFILE_EXIT_JSVAL
}

// *** Methods ***

// toString() : String
JSValue VectorToString(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  HPVector thisv;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "toString")))
    return JS_EXCEPTION;

  OOJS_RETURN_STRING_OR_NULL(cxx_HPVectorDescription(thisv));

  OOJS_NATIVE_EXIT
}

// toSource() : String
JSValue VectorToSource(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  HPVector thisv;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "toSource")))
    return JS_EXCEPTION;

  OOJS_RETURN_STRING_OR_NULL(
      oo::str::format("Vector3D(%g, %g, %g)", thisv.x, thisv.y, thisv.z));

  OOJS_NATIVE_EXIT
}

// add(v : vectorExpression) : Vector3D
JSValue VectorAdd(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv, result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "add")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "add", argc,
                                         OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPvector_add(thisv, thatv);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// subtract(v : vectorExpression) : Vector3D
JSValue VectorSubtract(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv, result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "subtract")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "subtract", argc,
                                         OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPvector_subtract(thisv, thatv);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// distanceTo(v : vectorExpression) : Number
JSValue VectorDistanceTo(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv;
  double result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "distanceTo")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "distanceTo",
                                         argc, OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPdistance(thisv, thatv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// squaredDistanceTo(v : vectorExpression) : Number
JSValue VectorSquaredDistanceTo(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv;
  double result;

  if (EXPECT_NOT(
          !GetThisVector(context, OOJS_THIS, &thisv, "squaredDistanceTo")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D",
                                         "squaredDistanceTo", argc, OOJS_ARGV,
                                         &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPdistance2(thisv, thatv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// multiply(n : Number) : Vector3D
JSValue VectorMultiply(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, result;
  double scalar;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "multiply")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!cxx_OOJSArgumentListGetNumber(
          context, "Vector3D", "multiply", argc, OOJS_ARGV, &scalar, NULL)))
    return JS_EXCEPTION;

  result = HPvector_multiply_scalar(thisv, scalar);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// dot(v : vectorExpression) : Number
JSValue VectorDot(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv;
  double result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "dot")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "dot", argc,
                                         OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPdot_product(thisv, thatv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// angleTo(v : vectorExpression) : Number
JSValue VectorAngleTo(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv;
  double result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "angleTo")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "angleTo", argc,
                                         OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPdot_product(HPvector_normal(thisv), HPvector_normal(thatv));
  if (result > 1.0) result = 1.0;
  if (result < -1.0) result = -1.0;
  // for identical vectors the dot_product sometimes returns a value > 1.0
  // because of rounding errors, resulting in an undefined result for the acos.
  result = acos(result);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// cross(v : vectorExpression) : Vector3D
JSValue VectorCross(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv, result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "cross")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "cross", argc,
                                         OOJS_ARGV, &thatv, NULL)))
    return JS_EXCEPTION;

  result = HPtrue_cross_product(thisv, thatv);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// tripleProduct(v : vectorExpression, u : vectorExpression) : Number
JSValue VectorTripleProduct(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv, theotherv;
  double result;
  int consumed;
  JSValueConst* argv = OOJS_ARGV;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "tripleProduct")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "tripleProduct",
                                         argc, argv, &thatv, &consumed)))
    return JS_EXCEPTION;
  argc -= consumed;
  argv += consumed;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "tripleProduct",
                                         argc, argv, &theotherv, NULL)))
    return JS_EXCEPTION;

  result = HPtriple_product(thisv, thatv, theotherv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// direction() : Vector3D
JSValue VectorDirection(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "direction")))
    return JS_EXCEPTION;

  result = HPvector_normal(thisv);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// magnitude() : Number
JSValue VectorMagnitude(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv;
  double result;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "magnitude")))
    return JS_EXCEPTION;

  result = HPmagnitude(thisv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// squaredMagnitude() : Number
JSValue VectorSquaredMagnitude(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv;
  double result;

  if (EXPECT_NOT(
          !GetThisVector(context, OOJS_THIS, &thisv, "squaredMagnitude")))
    return JS_EXCEPTION;

  result = HPmagnitude2(thisv);

  OOJS_RETURN_DOUBLE(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// rotationTo(v : vectorExpression [, limit : Number]) : Quaternion
JSValue VectorRotationTo(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, thatv;
  double limit = 0;
  bool gotLimit;
  Quaternion result;
  int consumed;
  JSValueConst* argv = OOJS_ARGV;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "rotationTo")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "rotationTo",
                                         argc, OOJS_ARGV, &thatv, &consumed)))
    return JS_EXCEPTION;

  argc -= consumed;
  argv += consumed;
  if (argc != 0)  // limit parameter is optional.
  {
    if (EXPECT_NOT(!cxx_OOJSArgumentListGetNumber(
            context, "Vector3D", "rotationTo", argc, argv, &limit, NULL)))
      return JS_EXCEPTION;
    gotLimit = true;
  } else
    gotLimit = false;

  if (gotLimit)
    result = quaternion_limited_rotation_between(
        HPVectorToVector(thisv), HPVectorToVector(thatv), limit);
  else
    result = quaternion_rotation_between(HPVectorToVector(thisv),
                                         HPVectorToVector(thatv));

  OOJS_RETURN_QUATERNION(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// rotateBy(q : quaternionExpression) : Vector3D
JSValue VectorRotateBy(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv, result;
  Quaternion q;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "rotateBy")))
    return JS_EXCEPTION;
  if (EXPECT_NOT(!QuaternionFromArgumentList(context, "Vector3D", "rotateBy",
                                             argc, OOJS_ARGV, &q, NULL)))
    return JS_EXCEPTION;

  result = quaternion_rotate_HPvector(q, thisv);

  OOJS_RETURN_HPVECTOR(result);

  OOJS_PROFILE_EXIT_JSVAL
}

// toArray() : Array
JSValue VectorToArray(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector thisv;

  if (EXPECT_NOT(!GetThisVector(context, OOJS_THIS, &thisv, "toArray")))
    return JS_EXCEPTION;

  JSValue result = JS_NewArray(context);
  if (EXPECT_NOT(JS_IsException(result))) return result;

  // JS_SetPropertyUint32() takes ownership of the element values.
  if (JS_SetPropertyUint32(context, result, 0,
                           JS_NewFloat64(context, thisv.x)) < 0 ||
      JS_SetPropertyUint32(context, result, 1,
                           JS_NewFloat64(context, thisv.y)) < 0 ||
      JS_SetPropertyUint32(context, result, 2,
                           JS_NewFloat64(context, thisv.z)) < 0) {
    JS_FreeValue(context, result);
    return JS_EXCEPTION;
  }

  return result;

  OOJS_PROFILE_EXIT_JSVAL
}

// toCoordinateSystem(coordScheme : String)
JSValue VectorToCoordinateSystem(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  HPVector thisv;
  std::optional<std::string> coordScheme;
  HPVector result;

  if (EXPECT_NOT(
          !GetThisVector(context, OOJS_THIS, &thisv, "toCoordinateSystem")))
    return JS_EXCEPTION;

  if (argc >= 1) coordScheme = cxx_OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (EXPECT_NOT(argc < 1 || !coordScheme.has_value())) {
    cxx_OOJSReportBadArguments(context, "Vector3D", "toCoordinateSystem",
                               MIN(argc, 1), OOJS_ARGV, std::nullopt,
                               "coordinate system");
    return JS_EXCEPTION;
  }

  OOJS_BEGIN_FULL_NATIVE(context)
  result =
      (UNIVERSE != nullptr ? UNIVERSE->legacyPositionFrom(thisv, *coordScheme)
                           : HPVector{});
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_HPVECTOR(result);

  OOJS_NATIVE_EXIT
}

// fromCoordinateSystem(coordScheme : String)
JSValue VectorFromCoordinateSystem(OOJS_NATIVE_ARGS) {
  OOJS_NATIVE_ENTER(context)

  HPVector thisv;
  std::optional<std::string> coordScheme;
  HPVector result;

  if (EXPECT_NOT(
          !GetThisVector(context, OOJS_THIS, &thisv, "fromCoordinateSystem")))
    return JS_EXCEPTION;

  if (argc >= 1) coordScheme = cxx_OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (EXPECT_NOT(argc < 1 || !coordScheme.has_value())) {
    cxx_OOJSReportBadArguments(context, "Vector3D", "fromCoordinateSystem",
                               MIN(argc, 1), OOJS_ARGV, std::nullopt,
                               "coordinate system");
    return JS_EXCEPTION;
  }

  OOJS_BEGIN_FULL_NATIVE(context)
  std::string arg = oo::str::format("%s %f %f %f", coordScheme->c_str(),
                                    thisv.x, thisv.y, thisv.z);
  result = (UNIVERSE != nullptr
                ? UNIVERSE->coordinatesFromCoordinateSystemString(arg)
                : HPVector{});
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_HPVECTOR(result);

  OOJS_NATIVE_EXIT
}

// *** Static methods ***

// interpolate(v : Vector3D, u : Vector3D, alpha : Number) : Vector3D
JSValue VectorStaticInterpolate(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  HPVector av, bv;
  double interp;
  HPVector result;
  int consumed;
  int inArgc = argc;
  JSValueConst* argv = OOJS_ARGV;
  JSValueConst* inArgv = argv;

  if (EXPECT_NOT(argc < 3)) goto INSUFFICIENT_ARGUMENTS;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "interpolate",
                                         argc, argv, &av, &consumed)))
    return JS_EXCEPTION;
  argc -= consumed;
  argv += consumed;
  if (EXPECT_NOT(argc < 2)) goto INSUFFICIENT_ARGUMENTS;
  if (EXPECT_NOT(!VectorFromArgumentList(context, "Vector3D", "interpolate",
                                         argc, argv, &bv, &consumed)))
    return JS_EXCEPTION;
  argc -= consumed;
  argv += consumed;
  if (EXPECT_NOT(argc < 1)) goto INSUFFICIENT_ARGUMENTS;
  if (EXPECT_NOT(!cxx_OOJSArgumentListGetNumber(
          context, "Vector3D", "interpolate", argc, argv, &interp, NULL)))
    return JS_EXCEPTION;

  result = OOHPVectorInterpolate(av, bv, interp);

  OOJS_RETURN_HPVECTOR(result);

INSUFFICIENT_ARGUMENTS:
  cxx_OOJSReportBadArguments(context, "Vector3D", "interpolate", inArgc, inArgv,
                             "Insufficient parameters",
                             "vector expression, vector expression and number");
  return JS_EXCEPTION;

  OOJS_PROFILE_EXIT_JSVAL
}

// random([maxLength : Number]) : Vector3D
JSValue VectorStaticRandom(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  double maxLength;

  if (argc == 0 || !OOJSArgumentListGetNumberNoError(context, argc, OOJS_ARGV,
                                                     &maxLength, NULL))
    maxLength = 1.0;

  OOJS_RETURN_HPVECTOR(OOHPVectorRandomSpatial(maxLength));

  OOJS_PROFILE_EXIT_JSVAL
}

// randomDirection([scale : Number]) : Vector3D
JSValue VectorStaticRandomDirection(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  double scale;

  if (argc == 0 ||
      !OOJSArgumentListGetNumberNoError(context, argc, OOJS_ARGV, &scale, NULL))
    scale = 1.0;

  OOJS_RETURN_HPVECTOR(HPvector_multiply_scalar(OORandomUnitHPVector(), scale));

  OOJS_PROFILE_EXIT_JSVAL
}

// randomDirectionAndLength([maxLength : Number]) : Vector3D
JSValue VectorStaticRandomDirectionAndLength(OOJS_NATIVE_ARGS) {
  OOJS_PROFILE_ENTER

  double maxLength;

  if (argc == 0 || !OOJSArgumentListGetNumberNoError(context, argc, OOJS_ARGV,
                                                     &maxLength, NULL))
    maxLength = 1.0;

  OOJS_RETURN_HPVECTOR(OOHPVectorRandomRadial(maxLength));

  OOJS_PROFILE_EXIT_JSVAL
}
}  // namespace
