/*

ValueType.h

The kind of value a table row's method returns, as a compile-time tag: the
by-name table (OOEntityCalledByNameTable.h), callObjC's
(OOJSCallEntityMethods.h), the shader bindings (EntityShaderBindings.cpp) and
the debug monitor's (OOJSConsole.cpp) tag each row with
oo::kValueType<ReturnType>. It replaced the @encode() strings the Objective-C
build gave, and maps each type to the kind its string did: exact fundamental
types (never by size), every game enum an int (each was encoded "i"),
typedefs resolved, references and cv dropped, the maths structures and the
colour by name, and any other pointer or class kOther. A type that is none of
these does not compile.

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

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

struct Vector;
struct HPVector;
struct Quaternion;

namespace oo {

struct Matrix;

}  // namespace oo

struct CGPoint;
struct CGRect;
struct BoundingBox;

namespace oo {

class Color;

}  // namespace oo

namespace oo {

class PList;

enum class ValueType : std::uint8_t {
  kVoid,
  kBool,
  kChar,
  kUChar,
  kShort,
  kUShort,
  kInt,
  kUInt,
  kLong,
  kULong,
  kLongLong,
  kULongLong,
  kFloat,
  kDouble,
  kVector,
  kHPVector,
  kQuaternion,
  kMatrix,
  kPoint,
  kRect,
  kBoundingBox,
  kColor,   // OOColor *
  kObject,  // an object result (oo::ObjectResult)
  kPList,
  kString,
  kOptionalString,
  kOther,  // any other pointer or class
};

// The tag a row uses for a method whose result is an object.
struct ObjectResult;

template <class T>
consteval ValueType ValueTypeOf() {
  using U = std::remove_cvref_t<T>;
  if constexpr (std::is_void_v<U>)
    return ValueType::kVoid;
  else if constexpr (std::is_same_v<U, bool>)
    return ValueType::kBool;
  else if constexpr (std::is_enum_v<U>)
    return ValueType::kInt;
  else if constexpr (std::is_same_v<U, char> || std::is_same_v<U, signed char>)
    return ValueType::kChar;
  else if constexpr (std::is_same_v<U, unsigned char>)
    return ValueType::kUChar;
  else if constexpr (std::is_same_v<U, short>)
    return ValueType::kShort;
  else if constexpr (std::is_same_v<U, unsigned short>)
    return ValueType::kUShort;
  else if constexpr (std::is_same_v<U, int>)
    return ValueType::kInt;
  else if constexpr (std::is_same_v<U, unsigned int>)
    return ValueType::kUInt;
  else if constexpr (std::is_same_v<U, long>)
    return ValueType::kLong;
  else if constexpr (std::is_same_v<U, unsigned long>)
    return ValueType::kULong;
  else if constexpr (std::is_same_v<U, long long>)
    return ValueType::kLongLong;
  else if constexpr (std::is_same_v<U, unsigned long long>)
    return ValueType::kULongLong;
  else if constexpr (std::is_same_v<U, float>)
    return ValueType::kFloat;
  else if constexpr (std::is_same_v<U, double>)
    return ValueType::kDouble;
  else if constexpr (std::is_same_v<U, Vector>)
    return ValueType::kVector;
  else if constexpr (std::is_same_v<U, HPVector>)
    return ValueType::kHPVector;
  else if constexpr (std::is_same_v<U, Quaternion>)
    return ValueType::kQuaternion;
  else if constexpr (std::is_same_v<U, oo::Matrix>)
    return ValueType::kMatrix;
  else if constexpr (std::is_same_v<U, CGPoint>)
    return ValueType::kPoint;
  else if constexpr (std::is_same_v<U, CGRect>)
    return ValueType::kRect;
  else if constexpr (std::is_same_v<U, BoundingBox>)
    return ValueType::kBoundingBox;
  else if constexpr (std::is_same_v<U, oo::Color*>)
    return ValueType::kColor;
  else if constexpr (std::is_same_v<U, ObjectResult>)
    return ValueType::kObject;
  else if constexpr (std::is_same_v<U, PList>)
    return ValueType::kPList;
  else if constexpr (std::is_same_v<U, std::string>)
    return ValueType::kString;
  else if constexpr (std::is_same_v<U, std::optional<std::string>>)
    return ValueType::kOptionalString;
  else if constexpr (std::is_pointer_v<U> || std::is_class_v<U>)
    return ValueType::kOther;
  else
    static_assert(false, "oo::ValueTypeOf: a type no table row has used");
}

template <class T>
inline constexpr ValueType kValueType = ValueTypeOf<T>();

// The kind's name, for log lines.
constexpr std::string_view ValueTypeName(ValueType type) {
  switch (type) {
    case ValueType::kVoid:
      return "void";
    case ValueType::kBool:
      return "bool";
    case ValueType::kChar:
      return "char";
    case ValueType::kUChar:
      return "unsigned char";
    case ValueType::kShort:
      return "short";
    case ValueType::kUShort:
      return "unsigned short";
    case ValueType::kInt:
      return "int";
    case ValueType::kUInt:
      return "unsigned int";
    case ValueType::kLong:
      return "long";
    case ValueType::kULong:
      return "unsigned long";
    case ValueType::kLongLong:
      return "long long";
    case ValueType::kULongLong:
      return "unsigned long long";
    case ValueType::kFloat:
      return "float";
    case ValueType::kDouble:
      return "double";
    case ValueType::kVector:
      return "Vector";
    case ValueType::kHPVector:
      return "HPVector";
    case ValueType::kQuaternion:
      return "Quaternion";
    case ValueType::kMatrix:
      return "OOMatrix";
    case ValueType::kPoint:
      return "NSPoint";
    case ValueType::kRect:
      return "NSRect";
    case ValueType::kBoundingBox:
      return "BoundingBox";
    case ValueType::kColor:
      return "OOColor *";
    case ValueType::kObject:
      return "an object";
    case ValueType::kPList:
      return "oo::PList";
    case ValueType::kString:
      return "std::string";
    case ValueType::kOptionalString:
      return "std::optional<std::string>";
    case ValueType::kOther:
      return "another pointer or class";
  }
  return "another pointer or class";
}

}  // namespace oo
