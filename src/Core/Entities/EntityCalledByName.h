/*

OOEntityCalledByName.h

The entities' methods called by name, as a C++ table: the rows
(OOEntityCalledByNameTable.h), one per method. The callers use
oo::CallEntityByName and oo::EntityRespondsToName (OOCallByName.h); the rows are
here for them.

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
#include <span>
#include <string>

#include "EnumFlags.h"
#include "Maths.h"
#include "ValueType.h"
#include "oofnd/PList.hpp"

class Entity;

namespace oo {

class Color;

}  // namespace oo

// The name sets: a row applies to an entity whose C++ part is
// of a class one of its flags names (a proxy's flag with the player's: the
// proxy's dials).
namespace oo {

enum class ByName : unsigned {
  kNone = 0,
  kShip = 1u << 0,
  kPlayer = 1u << 1,
  kProxy = 1u << 2,
  kStation = 1u << 3,
  kDock = 1u << 4,
  kEffect = 1u << 5,
};

constexpr std::true_type OOEnableFlagOperators(ByName);

enum class EntityByNameArgument : std::uint8_t {
  kNone,    // T name()
  kString,  // T name(const std::string &)
  kPList    // T name(const oo::PList &)
};

// A typed call's result, kept by its type: an integer, enum or BOOL result
// as a long long (as OOCallIntegerMethod() converted it), a float or double as
// a double, a vector, a quaternion, an entity, a colour, a property list.
// callObjC() reads the member its row's type names. (An object result, which no
// row answers, is not kept.)
struct EntityByNameValue {
  std::int64_t integer = 0;
  double real = 0;
  Vector vector{};
  Quaternion quaternion{};
  Entity* entity = nullptr;    // an entity result, borrowed: the C++ entity
  oo::Color* color = nullptr;  // a colour result, borrowed (fogUniform)
  oo::PList plist;
};

struct EntityByNameRow {
  const char* name;  // the selector's name
  ByName flags;
  oo::ValueType result;  // the declared result type's kind (ValueType.h)
  EntityByNameArgument argument;
  // The method's body on the entity, given the argument its kind takes; a PList
  // result, else null.
  oo::PList (*call)(Entity* entity, const std::string* string,
                    const oo::PList* plist);
  // A method with no argument: its body on the entity, its result kept by type;
  // else null.
  void (*typed)(Entity* entity, EntityByNameValue& value);
};

/*	The root entity's own names that callObjC() lists
   (OOJSCallEntityMethods.h) and the by-name table does not: a row per name, its
        answer (null: every entity; the planet's and the beacon's names for
   the parts that answer them) and, for a name callObjC() can call, the
   method's body on the C++ entity, its result kept by type.
*/
struct EntityFacadeRow {
  const char* name;
  bool (*answers)(Entity* entity);
  void (*typed)(Entity* entity, EntityByNameValue& value);
};

/*	Every name a C++ entity answers outside the by-name table: its own
   methods, categories' and root classes' (OOEntityObjectNamesTable.h).
   oo::EntityRespondsToName answers each for the entities that answer it: a
   root-entity row's method with no argument that OOCallByName could call is the
   row's body; any other is logged (the method's type encoding), and a
   root-class or lifetime method that the call could call (init, dealloc,
   retain, release,
   ...) is answered but never called.
*/
enum class EntityObjectNameKind : std::uint8_t {
  kSignature,  // no signature a by-name call can use: logged
  // a root-entity row's method with no argument: the row's body, called
  kCalled,
  kLifetime  // a root-class or lifetime method: answered, logged, never called
};

struct EntityObjectName {
  const char* name;
  bool (*answers)(Entity* entity);  // null: every entity
  EntityObjectNameKind kind;
  const char* encoding;  // the object's method's type encoding
};

std::span<const oo::EntityObjectName> EntityObjectNames();
// null if the object did not answer it
const oo::EntityObjectName* EntityObjectNameNamed(const std::string& name);

std::span<const oo::EntityFacadeRow> EntityFacadeRows();
// null if not a root-entity row
const oo::EntityFacadeRow* EntityFacadeRowNamed(const std::string& name);

std::span<const oo::EntityByNameRow> EntityByNameRows();
// null if not tabled
const oo::EntityByNameRow* EntityByNameRowNamed(const std::string& name);
bool EntityByNameRowApplies(const oo::EntityByNameRow& row, Entity* entity);

// The tabled method with no argument that applies to the entity, called, its
// result kept by type in value, or the root-entity row's; false (nothing
// called) for any other name.
bool CallEntityByNameTyped(Entity* entity, const std::string& name,
                           oo::EntityByNameValue& value);

}  // namespace oo
