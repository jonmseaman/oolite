/*

PlayerEntityLegacyScriptEngine.h

Various utility methods used for scripting.

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

#include "PlayerEntity.h"

/* The category PlayerEntity (Scripting) is members of PlayerEntity, declared
 * in PlayerEntity.h. This header stays for the files that include it.
 */

#include <string>

#include "OOEnumFlags.h"
#include "oofnd/PList.hpp"

class OOScript;

// A sanitized legacy condition stores both as plist unsigned integers
// (OOLegacyScriptWhitelist.h), so their numbers are pinned.
namespace oo {

enum class ComparisonType : unsigned {
  kEqual,
  kNotEqual,
  kLessThan,
  kGreaterThan,
  kOneOf,
  kUndefined
};

}  // namespace oo

static_assert(oo::ToUnderlying(oo::ComparisonType::kEqual) == 0);
static_assert(oo::ToUnderlying(oo::ComparisonType::kNotEqual) == 1);
static_assert(oo::ToUnderlying(oo::ComparisonType::kLessThan) == 2);
static_assert(oo::ToUnderlying(oo::ComparisonType::kGreaterThan) == 3);
static_assert(oo::ToUnderlying(oo::ComparisonType::kOneOf) == 4);
static_assert(oo::ToUnderlying(oo::ComparisonType::kUndefined) == 5);

namespace oo {

enum class OperationType : unsigned {
  kString,
  kNumber,
  kBool,
  kMissionVar,
  kLocalVar,
  kFalse,

  kInvalid  // Must be last.
};

}  // namespace oo

static_assert(oo::ToUnderlying(oo::OperationType::kString) == 0);
static_assert(oo::ToUnderlying(oo::OperationType::kNumber) == 1);
static_assert(oo::ToUnderlying(oo::OperationType::kBool) == 2);
static_assert(oo::ToUnderlying(oo::OperationType::kMissionVar) == 3);
static_assert(oo::ToUnderlying(oo::OperationType::kLocalVar) == 4);
static_assert(oo::ToUnderlying(oo::OperationType::kFalse) == 5);
static_assert(oo::ToUnderlying(oo::OperationType::kInvalid) == 6);

namespace oo {

std::string ComparisonTypeToString(ComparisonType type);

}  // namespace oo
