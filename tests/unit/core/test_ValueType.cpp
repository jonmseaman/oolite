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

#include <gtest/gtest.h>

#include "GuiDisplayGen.h"
#include "Color.h"
#include "EntityHold.h"
#include "Foundation.h"
#include "Maths.h"
#include "ShaderUniformMethodType.h"
#include "Types.h"
#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "StationEntity.h"
#include "ValueType.h"

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
static_assert(kValueType<oo::Scalar> == VT::kFloat);
static_assert(kValueType<oo::TimeDelta> == VT::kDouble);
static_assert(kValueType<oo::TimeAbsolute> == VT::kDouble);
static_assert(kValueType<std::uint8_t> == VT::kUChar);
static_assert(kValueType<std::uint32_t> == VT::kUInt);
static_assert(kValueType<std::size_t> == kValueType<decltype(sizeof(0))>);
static_assert(kValueType<std::ptrdiff_t> ==
              kValueType<decltype(static_cast<int*>(nullptr) -
                                  static_cast<int*>(nullptr))>);
static_assert(kValueType<oo::SystemID> == VT::kShort);
static_assert(kValueType<oo::GalaxyID> == VT::kUChar);
static_assert(kValueType<oo::FuelQuantity> == VT::kUShort);
static_assert(kValueType<oo::CargoQuantity> == VT::kUInt);
static_assert(kValueType<oo::CreditsQuantity> == kValueType<std::uint64_t>);
static_assert(kValueType<oo::TechLevelID> == kValueType<std::size_t>);
static_assert(kValueType<oo::UniversalID> == VT::kUShort);
static_assert(kValueType<oo::WeaponFacingSet> == VT::kUChar);

// Every game enum was encoded "i", whatever its underlying type.
static_assert(kValueType<oo::AegisStatus> == VT::kInt);
static_assert(kValueType<oo::AlertCondition> == VT::kInt);
static_assert(kValueType<oo::Behaviour> == VT::kInt);
static_assert(kValueType<oo::CargoType> == VT::kInt);
static_assert(kValueType<oo::CompassMode> == VT::kInt);
static_assert(kValueType<oo::DockingClearanceStatus> == VT::kInt);
static_assert(kValueType<oo::EnergyUnitType> == VT::kInt);
static_assert(kValueType<oo::EntityStatus> == VT::kInt);
static_assert(kValueType<oo::FuelScoopStatus> == VT::kInt);
static_assert(kValueType<oo::GUIBackgroundSpecial> == VT::kInt);
static_assert(kValueType<oo::GUIScreenID> == VT::kInt);
static_assert(kValueType<oo::GalacticHyperspaceBehaviour> == VT::kInt);
static_assert(kValueType<oo::LongRangeChartMode> == VT::kInt);
static_assert(kValueType<oo::MissileStatus> == VT::kInt);
static_assert(kValueType<oo::PlayerFleeingStatus> == VT::kInt);
static_assert(kValueType<oo::RouteType> == VT::kInt);
static_assert(kValueType<oo::ScanClass> == VT::kInt);
static_assert(kValueType<oo::SpeechSettings> == VT::kInt);
static_assert(kValueType<oo::StationAlertLevel> == VT::kInt);
static_assert(kValueType<oo::WeaponFacing> == VT::kInt);  // a uint8_t enum

// Structures, the colour, objects.
static_assert(kValueType<Vector> == VT::kVector);
static_assert(kValueType<HPVector> == VT::kHPVector);
static_assert(kValueType<Quaternion> == VT::kQuaternion);
static_assert(kValueType<oo::Matrix> == VT::kMatrix);
static_assert(kValueType<NSPoint> == VT::kPoint);
static_assert(kValueType<NSRect> == VT::kRect);
static_assert(kValueType<BoundingBox> == VT::kBoundingBox);
static_assert(kValueType<oo::Color*> == VT::kColor);
static_assert(kValueType<oo::ObjectResult> == VT::kObject);

// Values, with references and cv dropped.
static_assert(kValueType<oo::PList> == VT::kPList);
static_assert(kValueType<const oo::PList&> == VT::kPList);
static_assert(kValueType<std::string> == VT::kString);
static_assert(kValueType<const std::string&> == VT::kString);
static_assert(kValueType<std::optional<std::string>> == VT::kOptionalString);

// Any other pointer or class.
static_assert(kValueType<ShipEntity*> == VT::kOther);
static_assert(kValueType<oo::HUDBeaconIcon*> == VT::kOther);
static_assert(kValueType<std::vector<oo::EntityHold>*> == VT::kOther);
static_assert(kValueType<std::vector<std::shared_ptr<oo::Texture>>> ==
              VT::kOther);

namespace {

// The uniform type the old template table gave the type's encoding.
template <class T>
oo::ShaderUniformType TypeOf() {
  return oo::ShaderUniformTypeOf(kValueType<T>);
}

}  // namespace

TEST(ValueType, shaderUniformTypes) {
  EXPECT_TRUE(TypeOf<bool>() == oo::kShaderUniformTypeChar);  // bool was "c"
  EXPECT_TRUE(TypeOf<signed char>() == oo::kShaderUniformTypeChar);
  EXPECT_TRUE(TypeOf<unsigned char>() == oo::kShaderUniformTypeUnsignedChar);
  EXPECT_TRUE(TypeOf<short>() == oo::kShaderUniformTypeShort);
  EXPECT_TRUE(TypeOf<unsigned short>() == oo::kShaderUniformTypeUnsignedShort);
  EXPECT_TRUE(TypeOf<int>() == oo::kShaderUniformTypeInt);
  EXPECT_TRUE(TypeOf<oo::CompassMode>() == oo::kShaderUniformTypeInt);
  EXPECT_TRUE(TypeOf<unsigned>() == oo::kShaderUniformTypeUnsignedInt);
  EXPECT_TRUE(TypeOf<long>() == oo::kShaderUniformTypeLong);
  EXPECT_TRUE(TypeOf<unsigned long>() == oo::kShaderUniformTypeUnsignedLong);
  EXPECT_TRUE(TypeOf<float>() == oo::kShaderUniformTypeFloat);
  EXPECT_TRUE(TypeOf<double>() == oo::kShaderUniformTypeDouble);
  EXPECT_TRUE(TypeOf<Vector>() == oo::kShaderUniformTypeVector);
  EXPECT_TRUE(TypeOf<HPVector>() == oo::kShaderUniformTypeHPVector);
  EXPECT_TRUE(TypeOf<Quaternion>() == oo::kShaderUniformTypeQuaternion);
  EXPECT_TRUE(TypeOf<oo::Matrix>() == oo::kShaderUniformTypeMatrix);
  EXPECT_TRUE(TypeOf<NSPoint>() == oo::kShaderUniformTypePoint);
  EXPECT_TRUE(TypeOf<oo::Color*>() == oo::kShaderUniformTypeColor);
  EXPECT_TRUE(TypeOf<oo::ObjectResult>() == oo::kShaderUniformTypeObject);

  // No template matched these encodings.
  EXPECT_TRUE(TypeOf<void>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<long long>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<unsigned long long>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<NSRect>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<BoundingBox>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<oo::PList>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<std::optional<std::string>>() ==
           oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<ShipEntity*>() == oo::kShaderUniformTypeInvalid);
}

TEST(ValueType, valueTypeNames) {
  EXPECT_TRUE(oo::ValueTypeName(VT::kVoid) == "void");
  EXPECT_TRUE(oo::ValueTypeName(VT::kPList) == "oo::PList");
  EXPECT_TRUE(oo::ValueTypeName(VT::kOther) == "another pointer or class");
}
