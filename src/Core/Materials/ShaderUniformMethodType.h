/*

OOShaderUniformMethodType.h

Type code declarations and OpenStep implementation agnostic method type
matching for uniform bindings.


Copyright (C) 2007-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <cstdint>

#include "OOOpenGLExtensionManager.h"

#if OO_SHADERS || defined(OO_DEBUG_MONITOR_ENABLED)

#include <cstddef>
#include <string>

#include "OOMaths.h"
#include "ValueType.h"

// A uniform's (and a by-name method's) type tag: stored as uint8_t, an index
// into the template table, and the start of OOJSCall's method-type numbers, so
// typed constants rather than an enumeration.
namespace oo {

using ShaderUniformType = int;
inline constexpr ShaderUniformType kShaderUniformTypeInvalid =
    0;  // Not valid for bindings or constants
inline constexpr ShaderUniformType kShaderUniformTypeChar = 1;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeUnsignedChar =
    2;                                                           // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeShort = 3;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeUnsignedShort =
    4;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeInt =
    5;  // Binding or constant
inline constexpr ShaderUniformType kShaderUniformTypeUnsignedInt =
    6;                                                          // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeLong = 7;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeUnsignedLong =
    8;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeLongLong =
    9;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeUnsignedLongLong =
    10;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeFloat =
    11;  // Binding or constant
inline constexpr ShaderUniformType kShaderUniformTypeDouble =
    12;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeVector =
    13;  // Binding or constant
inline constexpr ShaderUniformType kShaderUniformTypeHPVector =
    14;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeQuaternion =
    15;  // Binding or constant
inline constexpr ShaderUniformType kShaderUniformTypeMatrix =
    16;  // Binding or constant
inline constexpr ShaderUniformType kShaderUniformTypePoint =
    17;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeColor =
    18;  // Binding only: the C++ colour (OOColor *)
inline constexpr ShaderUniformType kShaderUniformTypeObject =
    19;  // Binding only
inline constexpr ShaderUniformType kShaderUniformTypeCount = 20;

}  // namespace oo
   // Not valid for bindings or constants

static_assert(static_cast<int>(oo::kShaderUniformTypeInvalid) == 0);
static_assert(static_cast<int>(oo::kShaderUniformTypeChar) == 1);
static_assert(static_cast<int>(oo::kShaderUniformTypeUnsignedChar) == 2);
static_assert(static_cast<int>(oo::kShaderUniformTypeShort) == 3);
static_assert(static_cast<int>(oo::kShaderUniformTypeUnsignedShort) == 4);
static_assert(static_cast<int>(oo::kShaderUniformTypeInt) == 5);
static_assert(static_cast<int>(oo::kShaderUniformTypeUnsignedInt) == 6);
static_assert(static_cast<int>(oo::kShaderUniformTypeLong) == 7);
static_assert(static_cast<int>(oo::kShaderUniformTypeUnsignedLong) == 8);
static_assert(static_cast<int>(oo::kShaderUniformTypeLongLong) == 9);
static_assert(static_cast<int>(oo::kShaderUniformTypeUnsignedLongLong) == 10);
static_assert(static_cast<int>(oo::kShaderUniformTypeFloat) == 11);
static_assert(static_cast<int>(oo::kShaderUniformTypeDouble) == 12);
static_assert(static_cast<int>(oo::kShaderUniformTypeVector) == 13);
static_assert(static_cast<int>(oo::kShaderUniformTypeHPVector) == 14);
static_assert(static_cast<int>(oo::kShaderUniformTypeQuaternion) == 15);
static_assert(static_cast<int>(oo::kShaderUniformTypeMatrix) == 16);
static_assert(static_cast<int>(oo::kShaderUniformTypePoint) == 17);
static_assert(static_cast<int>(oo::kShaderUniformTypeColor) == 18);
static_assert(static_cast<int>(oo::kShaderUniformTypeObject) == 19);

namespace oo {

/*	The uniform type of a row's value type (ValueType.h): the member tables
   give their rows' types this way, so a row's type is the method's it
   replaced. Each answer is the one the old template table gave the type's
   @encode() string: bool was "c" (Char), and long long, unsigned long long,
   the rectangle, the bounding box, values and other pointers matched no
   template (Invalid).
*/
constexpr oo::ShaderUniformType ShaderUniformTypeOf(ValueType type) {
  switch (type) {
    case ValueType::kBool:
    case ValueType::kChar:
      return oo::kShaderUniformTypeChar;
    case ValueType::kUChar:
      return oo::kShaderUniformTypeUnsignedChar;
    case ValueType::kShort:
      return oo::kShaderUniformTypeShort;
    case ValueType::kUShort:
      return oo::kShaderUniformTypeUnsignedShort;
    case ValueType::kInt:
      return oo::kShaderUniformTypeInt;
    case ValueType::kUInt:
      return oo::kShaderUniformTypeUnsignedInt;
    case ValueType::kLong:
      return oo::kShaderUniformTypeLong;
    case ValueType::kULong:
      return oo::kShaderUniformTypeUnsignedLong;
    case ValueType::kFloat:
      return oo::kShaderUniformTypeFloat;
    case ValueType::kDouble:
      return oo::kShaderUniformTypeDouble;
    case ValueType::kVector:
      return oo::kShaderUniformTypeVector;
    case ValueType::kHPVector:
      return oo::kShaderUniformTypeHPVector;
    case ValueType::kQuaternion:
      return oo::kShaderUniformTypeQuaternion;
    case ValueType::kMatrix:
      return oo::kShaderUniformTypeMatrix;
    case ValueType::kPoint:
      return oo::kShaderUniformTypePoint;
    case ValueType::kColor:
      return oo::kShaderUniformTypeColor;
    case ValueType::kObject:
      return oo::kShaderUniformTypeObject;
    case ValueType::kVoid:
    case ValueType::kLongLong:
    case ValueType::kULongLong:
    case ValueType::kRect:
    case ValueType::kBoundingBox:
    case ValueType::kPList:
    case ValueType::kString:
    case ValueType::kOptionalString:
    case ValueType::kOther:
      return oo::kShaderUniformTypeInvalid;
  }
  return oo::kShaderUniformTypeInvalid;
}

}  // namespace oo

/*	Binding to a target's C++ members: a uniform
        bound to a property name the target's class lists in its member table
   reads the member through the table's getter, not a method found by selector.
   The value is passed in the field its type uses: integers (and booleans and
   enums) in i, floats in f, the structures in theirs, a colour in color
   (borrowed).
*/
class OOColor;

namespace oo {

struct ShaderBindingValue {
  std::int64_t i;
  double f;
  Vector v;
  HPVector hpv;
  Quaternion q;
  OOMatrix m;
  NSPoint p;
  OOColor* color;
};

}  // namespace oo

/*	A row's getter reads the member of the target's C++ object (the rows
   receive the C++ entity). The same type as OOShaderCxxBindingGetter
   (OOShaderUniform.h), so the uniform reads a member binding as it reads a C++
   target's.
*/
namespace oo {
class Object;
}

namespace oo {

using ShaderMemberGetter = void (*)(oo::Object* target,
                                    ShaderBindingValue& outValue);

struct ShaderMemberBinding {
  ShaderUniformType type;
  ShaderMemberGetter get;
  oo::Object* target;  // the C++ object get reads (the entity's part),
                       // borrowed: the uniform holds it weakly
};

}  // namespace oo

/*	A material's binding target is the C++ entity. What the uniform and the
   material ask of the entity they ask through these,
   which the entities register (EntityShaderBindings.cpp), so the materials
   still link without them; with none registered an entity target binds nothing.
*/
class Entity;

namespace oo {

struct ShaderEntityHooks {
  // -superShaderBindingTarget: the entity a "bind to super" uniform binds to
  // next, or null.
  Entity* (*superTarget)(Entity* entity);
  // What the log lines print for the entity: its class's
  // name ("Entity"), and its description; "(null)" for an entity without one.
  std::string (*className)(Entity* entity);
  std::string (*description)(Entity* entity);
  // The row of the entities' member table for the name, where it binds this
  // entity (as the member lookup above).
  bool (*memberBinding)(Entity* entity, const char* property,
                        ShaderMemberBinding* outBinding);
  // -randomSeedForShaders, where the entity's object answers it.
  bool (*randomSeed)(Entity* entity, uint32_t* outSeed);
};

void SetShaderEntityHooks(const ShaderEntityHooks& hooks) noexcept;
const ShaderEntityHooks* ShaderEntityHooksIfSet() noexcept;

}  // namespace oo
   // null where none is registered

#endif  // OO_SHADERS
