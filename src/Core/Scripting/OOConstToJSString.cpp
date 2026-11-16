/*

OOConstToJSString.m


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

#define OO_PLAIN_CPP_UNIT \
  1  // a plain C++ unit: the shared headers' game-unit parts stay out of it
#include "OOConstToJSString.h"

#include <array>
#include <string_view>

#include "oofnd/objc/OOAssert.h"

/*
        Each type of constant has its own lookup table, which is a linear list
   of TableEntry structs plus a count.

        The list is statically initialized with the constant values and C
   strings. OOConstToJSStringInit() (through InitTable()) adds an interned
   JSAtom for each C string. Atoms are interned once per process (they belong to
   the runtime, which survives engine resets) and are never freed. Two atoms are
        equal if and only if their strings are equal, so lookups from JS strings
        compare atoms.

        ConstTables are globals, which are accessed as local externs in the
   inlines in the header. All the advantages of globals with most of the
   advantages of encapsulation, sorta.

        -- Ahruman 2011-01-15
*/

struct TableEntry {
  std::ptrdiff_t value;
  const char* cString;
  JSAtom atom;
};

struct ConstTable {
  std::size_t count;
  TableEntry* entries;
};

// A table's rows as entries: the names are literals, so NUL-terminated.
constexpr const char* NameCString(std::string_view literal) {
  return literal.data();
}

template <class E, std::size_t N>
constexpr std::array<TableEntry, N> EntriesFrom(
    const oo::EnumTable<E, N>& table) {
  std::array<TableEntry, N> entries{};
  for (std::size_t i = 0; i < N; ++i) {
    entries[i] = {.value = oo::ToUnderlying(table.entries[i].first),
                  .cString = NameCString(table.entries[i].second),
                  .atom = JS_ATOM_NULL};
  }
  return entries;
}

// Unconditional, as upstream's: OOCAssert reads it in every flavour.
static bool sInited = false;
static bool sAtomsInterned = false;

/*
        The interned string "UNDEFINED", returned by
   OOJSValueFromConstantPRIVATE() if passed a bogus constant value.
*/
static JSAtom sUndefinedAtom;

/*
        Initialize table contents (with C strings, see above) from each enum's
        name table (the headers in Tables).
*/

static std::array<TableEntry, oo::kCompassModeTable.entries.size()>
    sOOCompassModeTableEntries = EntriesFrom(oo::kCompassModeTable);

static std::array<TableEntry, oo::kEntityStatusTable.entries.size()>
    sOOEntityStatusTableEntries = EntriesFrom(oo::kEntityStatusTable);

static std::array<TableEntry,
                  oo::kGalacticHyperspaceBehaviourTable.entries.size()>
    sOOGalacticHyperspaceBehaviourTableEntries =
        EntriesFrom(oo::kGalacticHyperspaceBehaviourTable);

static std::array<TableEntry, oo::kGUIScreenIDTable.entries.size()>
    sOOGUIScreenIDTableEntries = EntriesFrom(oo::kGUIScreenIDTable);

static std::array<TableEntry, oo::kScanClassTable.entries.size()>
    sOOScanClassTableEntries = EntriesFrom(oo::kScanClassTable);

static std::array<TableEntry, oo::kViewIDTable.entries.size()>
    sOOViewIDTableEntries = EntriesFrom(oo::kViewIDTable);

static std::array<TableEntry, oo::kShipDamageTypeTable.entries.size()>
    sOOShipDamageTypeTableEntries = EntriesFrom(oo::kShipDamageTypeTable);

static std::array<TableEntry, oo::kLegalStatusReasonTable.entries.size()>
    sOOLegalStatusReasonTableEntries = EntriesFrom(oo::kLegalStatusReasonTable);

static std::array<TableEntry, oo::kLongRangeChartModeTable.entries.size()>
    sOOLongRangeChartModeTableEntries =
        EntriesFrom(oo::kLongRangeChartModeTable);

ConstTable gOOCompassModeConstTable = {sOOCompassModeTableEntries.size(),
                                       sOOCompassModeTableEntries.data()};
ConstTable gOOEntityStatusConstTable = {sOOEntityStatusTableEntries.size(),
                                        sOOEntityStatusTableEntries.data()};
ConstTable gOOGalacticHyperspaceBehaviourConstTable = {
    sOOGalacticHyperspaceBehaviourTableEntries.size(),
    sOOGalacticHyperspaceBehaviourTableEntries.data()};
ConstTable gOOGUIScreenIDConstTable = {sOOGUIScreenIDTableEntries.size(),
                                       sOOGUIScreenIDTableEntries.data()};
ConstTable gOOScanClassConstTable = {sOOScanClassTableEntries.size(),
                                     sOOScanClassTableEntries.data()};
ConstTable gOOViewIDConstTable = {sOOViewIDTableEntries.size(),
                                  sOOViewIDTableEntries.data()};
ConstTable gOOShipDamageTypeConstTable = {sOOShipDamageTypeTableEntries.size(),
                                          sOOShipDamageTypeTableEntries.data()};
ConstTable gOOLegalStatusReasonConstTable = {
    sOOLegalStatusReasonTableEntries.size(),
    sOOLegalStatusReasonTableEntries.data()};
ConstTable gOOLongRangeChartModeConstTable = {
    sOOLongRangeChartModeTableEntries.size(),
    sOOLongRangeChartModeTableEntries.data()};

static void InitTable(JSContext* context, ConstTable* table);

// MARK: Initialization

void OOConstToJSStringInit(JSContext* context) {
  OOCAssert(!sInited, "OOConstToJSStringInit() called while already inited.");
  OOCParameterAssert(context != nullptr);

  // Atoms survive engine resets, so they only need interning once.
  if (!sAtomsInterned) {
    sUndefinedAtom = JS_NewAtom(context, "UNDEFINED");

    InitTable(context, &gOOEntityStatusConstTable);
    InitTable(context, &gOOCompassModeConstTable);
    InitTable(context, &gOOGalacticHyperspaceBehaviourConstTable);
    InitTable(context, &gOOGUIScreenIDConstTable);
    InitTable(context, &gOOScanClassConstTable);
    InitTable(context, &gOOViewIDConstTable);
    InitTable(context, &gOOShipDamageTypeConstTable);
    InitTable(context, &gOOLegalStatusReasonConstTable);
    InitTable(context, &gOOLongRangeChartModeConstTable);

    sAtomsInterned = true;
  }

  sInited = true;
}

void OOConstToJSStringDestroy() {
  OOCAssert(sInited, "OOConstToJSStringDestroy() called while not inited.");
  sInited = false;
  // The atoms remain valid; they belong to the runtime.
}

static int CompareEntries(const void* a, const void* b) {
  const TableEntry* entA = static_cast<const TableEntry*>(a);
  const TableEntry* entB = static_cast<const TableEntry*>(b);

  if (entA->value < entB->value) return -1;
  if (entA->value > entB->value) return 1;
  return 0;
}

static void InitTable(JSContext* context, ConstTable* table) {
  OOCParameterAssert(context != nullptr && table != nullptr);

  std::size_t i;
  for (i = 0; i < table->count; i++) {
    table->entries[i].atom = JS_NewAtom(context, table->entries[i].cString);
  }

  qsort(table->entries, table->count, sizeof *table->entries, CompareEntries);
}

// MARK: Lookup

JSValue OOJSValueFromConstantPRIVATE(JSContext* context, std::ptrdiff_t value,
                                     struct ConstTable* table) {
  OOCAssert(sInited, "%s called before OOConstToJSStringInit().",
            __PRETTY_FUNCTION__);
  OOCParameterAssert(context != nullptr);
  OOCParameterAssert(table != nullptr && table->count > 0);

  // Binary search. (Signed bounds: the SpiderMonkey version underflowed for
  // values below the first entry.)
  std::ptrdiff_t min = 0, max = static_cast<std::ptrdiff_t>(table->count) - 1;
  std::ptrdiff_t current;
  while (min <= max) {
    std::ptrdiff_t mid = (min + max) / 2;
    current = table->entries[mid].value;
    if (current < value) {
      min = mid + 1;
    } else if (current > value) {
      max = mid - 1;
    } else {
      return JS_AtomToString(context, table->entries[mid].atom);
    }
  }

  return JS_AtomToString(context, sUndefinedAtom);
}

std::size_t OOConstantFromJSValuePRIVATE(JSContext* context, JSValueConst value,
                                         struct ConstTable* table,
                                         std::ptrdiff_t defaultValue) {
  OOCAssert(sInited, "%s called before OOConstToJSStringInit().",
            __PRETTY_FUNCTION__);
  OOCParameterAssert(context != nullptr && table != nullptr);

  if (!JS_IsString(value)) [[unlikely]]
    return static_cast<std::size_t>(defaultValue);

  // Interning a string yields the same atom as the table entry with the same
  // contents.
  JSAtom atom = JS_ValueToAtom(context, value);
  if (atom == JS_ATOM_NULL) [[unlikely]] {
    JS_FreeValue(context, JS_GetException(context));
    return static_cast<std::size_t>(defaultValue);
  }

  std::ptrdiff_t result = defaultValue;
  std::size_t i, count = table->count;
  for (i = 0; i < count; i++) {
    if (table->entries[i].atom == atom) {
      result = table->entries[i].value;
      break;
    }
  }

  JS_FreeAtom(context, atom);
  return static_cast<std::size_t>(result);
}
