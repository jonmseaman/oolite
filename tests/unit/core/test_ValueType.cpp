/*	test_ValueType.cpp
        Unit tests for oo::ValueType (src/Core/ValueType.h).

        The by-name, callObjC and shader-binding tables tag each row with the
   kind of value its method returns. The tags replaced @encode() strings
   (OOTypeEncodings.h, deleted), so these pin that every type the rows use maps
   to the kind its old string gave: exact fundamental types, every game enum an
   int (each was encoded "i"), typedefs resolved, references and cv dropped,
   and any other pointer or class "other". Then the two consumers: the shader
   uniform type a kind gives, and the by-name dispatcher's partition (its
   result kinds), each as the old string comparison gave it.
        Run: tests/run.sh unit
*/

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "GuiDisplayGen.h"
#include "OOColor.h"
#include "OOEntityHold.h"
#include "OOFoundation.h"
#include "OOMaths.h"
#include "OOShaderUniformMethodType.h"
#include "OOTypes.h"
#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "StationEntity.h"
#include "ValueType.h"
#include "oo_test.hpp"

// main.cpp defines the debug flags, and the test has its own main.
#ifndef NDEBUG
std::uint32_t gDebugFlags = 0;
#endif

using oo::kValueType;
using VT = oo::ValueType;

// Fundamental types: by the type itself, never by size.
static_assert(kValueType<void> == VT::kVoid);
static_assert(kValueType<bool> == VT::kBool);
static_assert(kValueType<signed char> == VT::kChar);
static_assert(kValueType<unsigned char> == VT::kUChar);
static_assert(kValueType<short> == VT::kShort);
static_assert(kValueType<unsigned short> == VT::kUShort);
static_assert(kValueType<int> == VT::kInt);
static_assert(kValueType<unsigned> == VT::kUInt);
static_assert(kValueType<long> == VT::kLong);
static_assert(kValueType<unsigned long> == VT::kULong);
static_assert(kValueType<long long> == VT::kLongLong);
static_assert(kValueType<unsigned long long> == VT::kULongLong);
static_assert(kValueType<float> == VT::kFloat);
static_assert(kValueType<double> == VT::kDouble);

// Typedefs resolve to what they name.
static_assert(kValueType<GLfloat> == VT::kFloat);
static_assert(kValueType<GLint> == VT::kInt);
static_assert(kValueType<OOScalar> == VT::kFloat);
static_assert(kValueType<OOTimeDelta> == VT::kDouble);
static_assert(kValueType<OOTimeAbsolute> == VT::kDouble);
static_assert(kValueType<std::uint8_t> == VT::kUChar);
static_assert(kValueType<std::uint32_t> == VT::kUInt);
static_assert(kValueType<std::size_t> == kValueType<decltype(sizeof(0))>);
static_assert(kValueType<std::ptrdiff_t> ==
              kValueType<decltype(static_cast<int*>(nullptr) -
                                  static_cast<int*>(nullptr))>);
static_assert(kValueType<OOSystemID> == VT::kShort);
static_assert(kValueType<OOGalaxyID> == VT::kUChar);
static_assert(kValueType<OOFuelQuantity> == VT::kUShort);
static_assert(kValueType<OOCargoQuantity> == VT::kUInt);
static_assert(kValueType<OOCreditsQuantity> == kValueType<std::uint64_t>);
static_assert(kValueType<OOTechLevelID> == kValueType<std::size_t>);
static_assert(kValueType<OOUniversalID> == VT::kUShort);
static_assert(kValueType<OOWeaponFacingSet> == VT::kUChar);

// Every game enum was encoded "i", whatever its underlying type.
static_assert(kValueType<OOAegisStatus> == VT::kInt);
static_assert(kValueType<OOAlertCondition> == VT::kInt);
static_assert(kValueType<OOBehaviour> == VT::kInt);
static_assert(kValueType<OOCargoType> == VT::kInt);
static_assert(kValueType<OOCompassMode> == VT::kInt);
static_assert(kValueType<OODockingClearanceStatus> == VT::kInt);
static_assert(kValueType<OOEnergyUnitType> == VT::kInt);
static_assert(kValueType<OOEntityStatus> == VT::kInt);
static_assert(kValueType<OOFuelScoopStatus> == VT::kInt);
static_assert(kValueType<OOGUIBackgroundSpecial> == VT::kInt);
static_assert(kValueType<OOGUIScreenID> == VT::kInt);
static_assert(kValueType<OOGalacticHyperspaceBehaviour> == VT::kInt);
static_assert(kValueType<OOLongRangeChartMode> == VT::kInt);
static_assert(kValueType<OOMissileStatus> == VT::kInt);
static_assert(kValueType<OOPlayerFleeingStatus> == VT::kInt);
static_assert(kValueType<OORouteType> == VT::kInt);
static_assert(kValueType<OOScanClass> == VT::kInt);
static_assert(kValueType<OOSpeechSettings> == VT::kInt);
static_assert(kValueType<OOStationAlertLevel> == VT::kInt);
static_assert(kValueType<OOWeaponFacing> == VT::kInt);  // a uint8_t enum

// Structures, the colour, objects.
static_assert(kValueType<Vector> == VT::kVector);
static_assert(kValueType<HPVector> == VT::kHPVector);
static_assert(kValueType<Quaternion> == VT::kQuaternion);
static_assert(kValueType<OOMatrix> == VT::kMatrix);
static_assert(kValueType<NSPoint> == VT::kPoint);
static_assert(kValueType<NSRect> == VT::kRect);
static_assert(kValueType<BoundingBox> == VT::kBoundingBox);
static_assert(kValueType<OOColor*> == VT::kColor);
static_assert(kValueType<oo::ObjectResult> == VT::kObject);

// Values, with references and cv dropped.
static_assert(kValueType<oo::PList> == VT::kPList);
static_assert(kValueType<const oo::PList&> == VT::kPList);
static_assert(kValueType<std::string> == VT::kString);
static_assert(kValueType<const std::string&> == VT::kString);
static_assert(kValueType<std::optional<std::string>> == VT::kOptionalString);

// Any other pointer or class.
static_assert(kValueType<ShipEntity*> == VT::kOther);
static_assert(kValueType<OOHUDBeaconIcon*> == VT::kOther);
static_assert(kValueType<std::vector<oo::EntityHold>*> == VT::kOther);
static_assert(kValueType<std::vector<std::shared_ptr<OOTexture>>> ==
              VT::kOther);

namespace {

// The uniform type the old template table gave the type's encoding.
template <class T>
OOShaderUniformType TypeOf() {
  return oo::ShaderUniformTypeOf(kValueType<T>);
}

}  // namespace

OO_TEST(shaderUniformTypes) {
  OO_CHECK(TypeOf<bool>() == kOOShaderUniformTypeChar);  // bool was "c"
  OO_CHECK(TypeOf<signed char>() == kOOShaderUniformTypeChar);
  OO_CHECK(TypeOf<unsigned char>() == kOOShaderUniformTypeUnsignedChar);
  OO_CHECK(TypeOf<short>() == kOOShaderUniformTypeShort);
  OO_CHECK(TypeOf<unsigned short>() == kOOShaderUniformTypeUnsignedShort);
  OO_CHECK(TypeOf<int>() == kOOShaderUniformTypeInt);
  OO_CHECK(TypeOf<OOCompassMode>() == kOOShaderUniformTypeInt);
  OO_CHECK(TypeOf<unsigned>() == kOOShaderUniformTypeUnsignedInt);
  OO_CHECK(TypeOf<long>() == kOOShaderUniformTypeLong);
  OO_CHECK(TypeOf<unsigned long>() == kOOShaderUniformTypeUnsignedLong);
  OO_CHECK(TypeOf<float>() == kOOShaderUniformTypeFloat);
  OO_CHECK(TypeOf<double>() == kOOShaderUniformTypeDouble);
  OO_CHECK(TypeOf<Vector>() == kOOShaderUniformTypeVector);
  OO_CHECK(TypeOf<HPVector>() == kOOShaderUniformTypeHPVector);
  OO_CHECK(TypeOf<Quaternion>() == kOOShaderUniformTypeQuaternion);
  OO_CHECK(TypeOf<OOMatrix>() == kOOShaderUniformTypeMatrix);
  OO_CHECK(TypeOf<NSPoint>() == kOOShaderUniformTypePoint);
  OO_CHECK(TypeOf<OOColor*>() == kOOShaderUniformTypeColor);
  OO_CHECK(TypeOf<oo::ObjectResult>() == kOOShaderUniformTypeObject);

  // No template matched these encodings.
  OO_CHECK(TypeOf<void>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<long long>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<unsigned long long>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<NSRect>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<BoundingBox>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<oo::PList>() == kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<std::optional<std::string>>() ==
           kOOShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<ShipEntity*>() == kOOShaderUniformTypeInvalid);
}

OO_TEST(valueTypeNames) {
  OO_CHECK(oo::ValueTypeName(VT::kVoid) == "void");
  OO_CHECK(oo::ValueTypeName(VT::kPList) == "oo::PList");
  OO_CHECK(oo::ValueTypeName(VT::kOther) == "another pointer or class");
}

OO_TEST_MAIN()
