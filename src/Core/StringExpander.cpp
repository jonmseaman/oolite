/*

OOStringExpander.m

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the impllied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#include "StringExpander.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

#include "CallByName.h"
#include "Foundation.h"
#include "OOJavaScriptEngine.h"
#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "StringParsing.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PrintfFormat.h"
#include "oofnd/String.hpp"
#include "oofnd/objc/Assert.h"
#include "oofnd/objc/Exception.h"

/*	The expansion engine works on UTF-16 code units, held in std::u16string;
   an empty optional stands for nil. Text enters and leaves as UTF-8 (WTF-8,
   lossless: oo::utf8ToUtf16 / oo::utf16ToUtf8). The unmigrated callees
   (Universe, PlayerEntity, ResourceManager) and the log and JavaScript reports
   are converted at the call.
*/
namespace oo {

using Units = std::u16string;
using MaybeUnits = std::optional<std::u16string>;

}  // namespace oo

namespace {

oo::Units UnitsWithCharacters(const char16_t* characters, std::size_t length);
std::size_t FindUnits(const oo::Units& string, std::u16string_view target,
                      std::size_t from);
bool HasPrefix(const oo::Units& string, std::u16string_view prefix);
bool HasSuffix(const oo::Units& string, std::u16string_view suffix);

}  // namespace

// Syntax warnings are for OXP developers, so include them wherever the debug
// console is.
#if OOLITE_DEBUG || defined(OO_DEBUG_MONITOR_ENABLED)
#define WARNINGS 1
#else
#define WARNINGS 0
#endif

#define OO_EXPANDER_RANDOM                 \
  (context->useGoodRNG ? (Ranrot() & 0xFF) \
                       : static_cast<unsigned>(gen_rnd_number()))

/*
        Total stack limit for strings being parsed (in UTF-16 code elements,
        i.e. units of 2 bytes), used recursively - for instance, if the root
        string takes 10,000 characters, any string it recurses into gets
        kStackAllocationLimit - 10,000. If the limit would be exceeded,
        the unexpanded string is returned instead.

        The limit is expected to be much higher than necessary for any
   practical string, and exists only to catch pathological behaviour without
   crashing.
*/
constexpr int kStackAllocationLimit = UINT16_MAX;

/*
        Recursion limit, for much the same purpose. Without it, we crash about
        22,000 stack frames deep when trying to expand a = "[a]" on a Mac.
*/
constexpr int kRecursionLimit = 100;

/*	OOStringExpansionContext

        Struct used to store context and caches for the entire string expansion
        operation, including recursive calls (so it can't contain anything
   pertaining to the specific string being expanded).
*/
namespace {

struct StringExpansionContext {
  Random_Seed seed;
  oo::MaybeUnits systemName;
  oo::PList overrides;     // a dictionary of mixed values; null: none (was nil)
  oo::PList legacyLocals;  // a dictionary of mixed values; null: none (was nil)
  bool isJavaScript;
  bool convertBackslashN;
  bool hasPercentR =
      false;  // Set to indicate we need an ExpandPercentR() pass.
  bool useGoodRNG;
  bool disallowPercentI = false;

  oo::MaybeUnits systemNameWithIan{};  // Cache for %I
  oo::MaybeUnits randomNameN{};        // Cache for %N
  oo::MaybeUnits randomNameR{};        // Cache for %R
  oo::PList systemDescriptions{};  // Cache for system_description (an array),
                                   // used for numbered keys; null: not loaded.
  std::size_t sysDescCount = 0;    // Count of systemDescriptions, valid after
                                   // GetSystemDescriptions() called.
};

}  // namespace

namespace {

/*	Accessors for lazily-instantiated caches in context.
 */
oo::MaybeUnits GetSystemName(StringExpansionContext* context);     // %H
oo::MaybeUnits GetSystemNameIan(StringExpansionContext* context);  // %I
oo::MaybeUnits GetRandomNameN(StringExpansionContext* context);    // %N
oo::MaybeUnits GetRandomNameR(StringExpansionContext* context);    // %R
const oo::PList& GetSystemDescriptions(StringExpansionContext* context);

void AppendCharacters(oo::MaybeUnits* result, const char16_t* characters,
                      std::size_t start, std::size_t end);

oo::Units NewRandomDigrams(StringExpansionContext* context);
oo::Units OldRandomDigrams();

// Various bits of expansion logic, each with a comment of its very own at the
// implementation.
oo::Units Expand(StringExpansionContext* context, const oo::Units& string,
                 std::size_t sizeLimit, std::size_t recursionLimit);

oo::MaybeUnits ExpandKey(StringExpansionContext* context,
                         const char16_t* characters, std::size_t size,
                         std::size_t idx, std::size_t* replaceLength,
                         std::size_t sizeLimit, std::size_t recursionLimit);
oo::MaybeUnits ExpandDigitKey(StringExpansionContext* context,
                              const char16_t* characters, std::size_t keyStart,
                              std::size_t keyLength, std::size_t sizeLimit,
                              std::size_t recursionLimit);
oo::MaybeUnits ExpandStringKey(StringExpansionContext* context,
                               const oo::Units& key, std::size_t sizeLimit,
                               std::size_t recursionLimit);
oo::MaybeUnits ExpandStringKeyOverride(StringExpansionContext* context,
                                       const std::string& key);
oo::MaybeUnits ExpandStringKeySpecial(StringExpansionContext* context,
                                      const std::string& key);
oo::MaybeUnits ExpandStringKeyKeyboardBinding(StringExpansionContext* context,
                                              const oo::Units& key);
/*	Key -> method name tables, keyed by the key's UTF-8 (string equality).
   Never iterated. The value is the name the method is called by
   (oo::CallEntityByName).
*/
using SelectorTable = std::map<std::string, std::string, std::less<>>;

const std::string* LookUpSelector(const SelectorTable* table,
                                  const std::string& key) {
  if (table == nullptr) return nullptr;
  auto found = table->find(key);
  return (found != table->end()) ? &found->second : nullptr;
}

// A lookup's text, entering the UTF-16 engine (nullopt stays nullopt).
oo::MaybeUnits UnitsFromOptional(const std::optional<std::string>& string);

const SelectorTable* SpecialSubstitutionSelectors();
oo::MaybeUnits ExpandStringKeyFromDescriptions(StringExpansionContext* context,
                                               const std::string& key,
                                               std::size_t sizeLimit,
                                               std::size_t recursionLimit);
oo::MaybeUnits ExpandStringKeyMissionVariable(StringExpansionContext* context,
                                              const oo::Units& key,
                                              const std::string& keyString);
oo::MaybeUnits ExpandStringKeyLegacyLocalVariable(
    StringExpansionContext* context, const std::string& key);
oo::Units ValueText(const oo::PList& value);
oo::MaybeUnits ExpandLegacyScriptSelectorKey(StringExpansionContext* context,
                                             const std::string& key);
const std::string* LookUpLegacySelector(const std::string& key);

oo::MaybeUnits ExpandPercentEscape(StringExpansionContext* context,
                                   const char16_t* characters, std::size_t size,
                                   std::size_t idx, std::size_t* replaceLength);
oo::MaybeUnits ExpandSystemNameForGalaxyEscape(StringExpansionContext* context,
                                               const char16_t* characters,
                                               std::size_t size,
                                               std::size_t idx,
                                               std::size_t* replaceLength);
oo::MaybeUnits ExpandSystemNameEscape(StringExpansionContext* context,
                                      const char16_t* characters,
                                      std::size_t size, std::size_t idx,
                                      std::size_t* replaceLength);
oo::MaybeUnits ExpandPercentR(StringExpansionContext* context,
                              const oo::MaybeUnits& input);
#if WARNINGS
void ReportWarningForUnknownKey(StringExpansionContext* context,
                                const oo::Units& key,
                                const std::string& keyString);
#endif

oo::MaybeUnits ApplyOperators(oo::MaybeUnits string,
                              const oo::Units& operatorsString);
oo::MaybeUnits ApplyOneOperator(const oo::MaybeUnits& string,
                                const oo::Units& op,
                                const oo::MaybeUnits& param);

/*	SyntaxWarning(context, logMessageClass, format, ...)
        SyntaxError(context, logMessageClass, format, ...)

        Report warning or error for expansion syntax, including unknown keys.

        Warnings are reported as JS warnings or log messages (depending on the
        context->isJavaScript flag) if the relevant log message class is
   enabled. Warnings are completely disabled in Deployment builds.

        Errors are reported as JS warnings (not exceptions) or log messages
   (again depending on context->isJavaScript) in all configurations. Exceptions
   are not used to avoid breaking code that worked with the old expander, even
   if it was questionable.

        Errors that are not syntax or invalid keys are reported with
   OO_LOG_ERR().
*/
void SyntaxIssue(StringExpansionContext* context,
                 const std::source_location& where, const char* logMessageClass,
                 const char* prefix, const char* format, ...)
    OO_PRINTF_FORMAT(5, 6);
void ReportJavaScriptWarning(JSContext* jsc, const char* format, ...)
    __attribute__((format(printf, 2, 3)));
}  // namespace

// OOLOG_WARNING_PREFIX's text, as a C string.
#define kSyntaxIssuePrefix "----- WARNING: "

#define SyntaxError(CONTEXT, CLASS, FORMAT, ...)                 \
  SyntaxIssue(CONTEXT, ::std::source_location::current(), CLASS, \
              kSyntaxIssuePrefix, FORMAT __VA_OPT__(, ) __VA_ARGS__)

#if WARNINGS
#define SyntaxWarning(CONTEXT, CLASS, FORMAT, ...)               \
  SyntaxIssue(CONTEXT, ::std::source_location::current(), CLASS, \
              kSyntaxIssuePrefix, FORMAT __VA_OPT__(, ) __VA_ARGS__)
#else
#define SyntaxWarning(...) \
  do {                     \
  } while (0)
#endif

// MARK: -
// MARK: Public functions

namespace oo {

std::optional<std::string> ExpandDescriptionString(
    Random_Seed seed, const std::string& string, const oo::PList& overrides,
    const oo::PList& legacyLocals, const std::optional<std::string>& systemName,
    ExpandOptions options) {
  StringExpansionContext context = {
      .seed = seed,
      .systemName = systemName.has_value()
                        ? MaybeUnits(oo::utf8ToUtf16(*systemName))
                        : std::nullopt,
      .overrides = overrides,
      .legacyLocals = legacyLocals,
      .isJavaScript = oo::Any(options & ExpandOptions::kForJavaScript),
      .convertBackslashN = oo::Any(options & ExpandOptions::kBackslashN),
      .useGoodRNG = oo::Any(options & ExpandOptions::kGoodRNG)};

  // Avoid recursive %I expansion by pre-seeding cache with literal %I.
  if (oo::Any(options & ExpandOptions::kDisallowPercentI)) {
    context.systemNameWithIan = Units(u"%I");
  }

  RandomState savedRandomState;
  if (oo::Any(options & ExpandOptions::kReseedRNG)) {
    savedRandomState = SaveRandomState();
    SetReallyRandomRANROTAndRndSeeds();
  }

  std::optional<std::string> result;
  // Was an autorelease-pool block, kept as the C++ pool: what the expander
  // calls by name may defer a release (OOCallByName).
  {
    oo::AutoreleaseScope pool;
    // TODO: profile caching the results. Would need to keep track of whether
    // we've done something nondeterministic (array selection, %R etc). (The
    // context's values are C++ now: nothing is left to release if an exception
    // unwinds.)
    MaybeUnits intermediate;
    const Units units = oo::utf8ToUtf16(string);
    if (oo::Any(options & ExpandOptions::kKey)) {
      intermediate = ExpandStringKey(&context, units, kStackAllocationLimit,
                                     kRecursionLimit);
    } else {
      intermediate =
          Expand(&context, units, kStackAllocationLimit, kRecursionLimit);
    }
    if (context.hasPercentR) {
      intermediate = ExpandPercentR(&context, intermediate);
    }
    if (intermediate.has_value()) result = oo::utf16ToUtf8(*intermediate);

    if (oo::Any(options & ExpandOptions::kReseedRNG)) {
      RestoreRandomState(savedRandomState);
    }
  }
  return result;
}

std::optional<std::string> GenerateSystemDescription(
    Random_Seed seed, const std::optional<std::string>& name) {
  seed_RNG_only_for_planet_description(seed);
  return ExpandDescriptionString(seed, "system-description-string", oo::PList(),
                                 oo::PList(), name, ExpandOptions::kKey);
}

Random_Seed StringExpanderDefaultRandomSeed() {
  auto* manager =
      (UNIVERSE != nullptr
           ? UNIVERSE->getSystemManager()
           : nullptr);  // null: a zero seed, as a message to nil (auto: the
                        // expander harness stubs the class)
  return (manager != nullptr) ? manager->getRandomSeedForCurrentSystem()
                              : Random_Seed{};
}

}  // namespace oo

// MARK: -
// MARK: Guts

namespace {

/*	Expand(context, string, sizeLimit, recursionLimit)

        Top-level expander. Expands all types of substitution in a string.

        <sizeLimit> is the remaining budget for stack allocation of read
   buffers. (Expand() is the only function that creates such buffers.)
   <recursionLimit> limits the number of recursive calls of Expand() that are
   permitted. If one of the limits would be exceeded, Expand() returns the input
   string unmodified.
*/
oo::Units Expand(StringExpansionContext* context, const oo::Units& string,
                 std::size_t sizeLimit, std::size_t recursionLimit) {
  OOCParameterAssert(context != nullptr && sizeLimit <= kStackAllocationLimit);

  const std::size_t size = string.size();

  // Avoid stack overflow.
  if (size > sizeLimit || recursionLimit == 0) [[unlikely]]
    return string;
  sizeLimit -= size;
  recursionLimit--;

  // Nothing to expand in an empty string, and the size-1 thing below would be
  // trouble.
  if (size == 0) return string;

  const char16_t* characters = string.data();

  /*	Beginning of current range of non-special characters. If we encounter
          a substitution, we'll be copying from here forward.
  */
  std::size_t copyRangeStart = 0;

  // Result, if we perform any substitutions.
  oo::MaybeUnits result;

  /*	The iteration limit is size - 1 because every valid substitution is at
          least 2 characters long. This way, characters[idx + 1] is always
     valid.
  */
  for (std::size_t idx = 0; idx < size - 1; idx++) {
    /*	Main parsing loop. If, at the end of the loop, replacement != nil,
            we copy the characters from copyRangeStart to idx into the result,
            the insert replacement, and skip replaceLength characters forward
            (minus one, because idx is incremented by the loop.)
    */
    oo::MaybeUnits replacement;
    std::size_t replaceLength = 0;
    uint16_t thisChar = characters[idx];

    if (thisChar == '[') {
      replacement = ExpandKey(context, characters, size, idx, &replaceLength,
                              sizeLimit, recursionLimit);
    } else if (thisChar == '%') {
      replacement =
          ExpandPercentEscape(context, characters, size, idx, &replaceLength);
    } else if (thisChar == ']') {
      SyntaxWarning(context, "strings.expand.warning.unbalancedClosingBracket",
                    "%s", "Unbalanced ] in string.");
    } else if (thisChar == '\\' && context->convertBackslashN) {
      if (characters[idx + 1] == 'n') {
        replaceLength = 2;
        replacement = oo::Units(u"\n");
      }
    } else {
      // No token start character, so we definitely have no replacement.
      continue;
    }

    if (replacement.has_value()) {
      /*	If replacement string is "\x7F", eat the following character.
              This is used in system_description for the one empty string
              in [22]. At the end of the string there is no following
              character, and nothing is eaten (upstream tested
              replaceLength < size, which let idx + replaceLength pass size
              there and assert below).
      */
      if (*replacement == u"\x7F" && replaceLength < size) {
        if (idx + replaceLength < size) replaceLength++;
        replacement = oo::Units();
      }

      // Avoid copying if we're replacing the entire input string.
      if (copyRangeStart == 0 && replaceLength == size) {
        return *replacement;
      }

      // Write the pending literal segment to result. This also allocates result
      // if needed.
      AppendCharacters(&result, characters, copyRangeStart, idx);

      *result += *replacement;

      // Skip over replaced part and start a new literal segment.
      idx += replaceLength - 1;
      copyRangeStart = idx + 1;
    }
  }

  if (result.has_value()) {
    // Append any trailing literal segment.
    AppendCharacters(&result, characters, copyRangeStart, size);

    return *result;
  } else {
    // No substitutions, return original string.
    return string;
  }
}

/*	ExpandKey(context, characters, size, idx, replaceLength, sizeLimit,
   recursionLimit)

        Expand a substitution key, i.e. a section surrounded by square brackets.
        On entry, <idx> is the offset to an opening bracket. ExpandKey()
   searches for the balancing closing bracket, and if it is found dispatches to
   either ExpandDigitKey() (for a key consisting only of digits) or
   ExpandStringKey() (for anything else).

        The key may be terminated by a vertical bar |, followed by an operator.
   An operator is an identifier, optionally followed by a colon and additional
        text, and may be terminated with another bar and operator.
*/
oo::MaybeUnits ExpandKey(StringExpansionContext* context,
                         const char16_t* characters, std::size_t size,
                         std::size_t idx, std::size_t* replaceLength,
                         std::size_t sizeLimit, std::size_t recursionLimit) {
  OOCParameterAssert(context != nullptr && characters != nullptr &&
                     replaceLength != nullptr);
  OOCParameterAssert(characters[idx] == '[');

  // Find the balancing close bracket.
  std::size_t end, balanceCount = 1, firstBar = 0;
  bool allDigits = true;

  for (end = idx + 1; end < size && balanceCount > 0; end++) {
    if (characters[end] == ']') {
      balanceCount--;
    } else {
      if (characters[end] == '[')
        balanceCount++;
      else if (characters[end] == '|' && firstBar == 0)
        firstBar = end;
      if (!isdigit(characters[end]) && firstBar == 0) allDigits = false;
    }
  }

  // Fail if no balancing bracket.
  if (balanceCount != 0) [[unlikely]] {
    SyntaxWarning(context, "strings.expand.warning.unbalancedOpeningBracket",
                  "%s", "Unbalanced [ in string.");
    return std::nullopt;
  }

  std::size_t totalLength = end - idx;
  *replaceLength = totalLength;
  std::size_t keyStart = idx + 1, keyLength = totalLength - 2;
  if (firstBar != 0) keyLength = firstBar - idx - 1;

  if (keyLength == 0) [[unlikely]] {
    SyntaxWarning(context, "strings.expand.warning.emptyKey", "%s",
                  "Invalid expansion code [] string. (To avoid this message, "
                  "use %%[%%].)");
    return std::nullopt;
  }

  oo::MaybeUnits expanded;
  if (allDigits) {
    expanded = ExpandDigitKey(context, characters, keyStart, keyLength,
                              sizeLimit, recursionLimit);
  } else {
    oo::Units key = UnitsWithCharacters(characters + keyStart, keyLength);
    expanded = ExpandStringKey(context, key, sizeLimit, recursionLimit);
  }

  if (firstBar != 0) {
    oo::Units operators =
        UnitsWithCharacters(characters + firstBar + 1, end - firstBar - 2);
    expanded = ApplyOperators(expanded, operators);
  }

  return expanded;
}

/*	ApplyOperators(string, operatorsString)

        Given a string and a series of formatting operators separated by
   vertical bars (or a single formatting operator), apply the operators, in
   sequence, to the string.
 */
oo::MaybeUnits ApplyOperators(oo::MaybeUnits string,
                              const oo::Units& operatorsString) {
  // Each part of operatorsString between '|' separators
  // (-componentsSeparatedByString:), in order.
  std::size_t partStart = 0;
  for (;;) {
    const std::size_t bar = FindUnits(operatorsString, u"|", partStart);
    const std::size_t partEnd =
        (bar == oo::kNotFound) ? operatorsString.size() : bar;
    oo::Units op = operatorsString.substr(partStart, partEnd - partStart);

    oo::MaybeUnits param;
    const std::size_t colon = FindUnits(op, u":", 0);
    if (colon != oo::kNotFound) {
      param = op.substr(colon + 1);
      op.resize(colon);
    }
    string = ApplyOneOperator(string, op, param);

    if (bar == oo::kNotFound) break;
    partStart = bar + 1;
  }

  return string;
}

// -doubleValue / -longLongValue / -intValue, answering 0 for nil as messaging
// nil did.
double DoubleValue(const oo::MaybeUnits& string) {
  return string.has_value() ? oo::plist_get::doubleValue(*string) : 0.0;
}

std::int64_t LongLongValue(const oo::MaybeUnits& string) {
  return string.has_value() ? oo::plist_get::longLongValue(*string) : 0;
}

int IntValue(const oo::MaybeUnits& string) {
  return string.has_value() ? oo::plist_get::intValue(*string) : 0;
}

oo::MaybeUnits UnitsFromUTF8(const std::string& string) {
  return oo::utf8ToUtf16(string);
}

oo::MaybeUnits Operator_cr(const oo::MaybeUnits& string,
                           const oo::MaybeUnits& /* param */) {
  return oo::utf8ToUtf16(
      oo::Credits(static_cast<oo::CreditsQuantity>(DoubleValue(string) * 10)));
}

oo::MaybeUnits Operator_dcr(const oo::MaybeUnits& string,
                            const oo::MaybeUnits& /* param */) {
  return oo::utf8ToUtf16(
      oo::Credits(static_cast<oo::CreditsQuantity>(LongLongValue(string))));
}

oo::MaybeUnits Operator_icr(const oo::MaybeUnits& string,
                            const oo::MaybeUnits& /* param */) {
  return oo::utf8ToUtf16(
      oo::IntCredits(static_cast<oo::CreditsQuantity>(LongLongValue(string))));
}

oo::MaybeUnits Operator_idcr(const oo::MaybeUnits& string,
                             const oo::MaybeUnits& /* param */) {
  return oo::utf8ToUtf16(oo::IntCredits(
      static_cast<oo::CreditsQuantity>(round(DoubleValue(string) / 10.0))));
}

oo::MaybeUnits Operator_precision(const oo::MaybeUnits& string,
                                  const oo::MaybeUnits& param) {
  return UnitsFromUTF8(
      std::format("{:.{}f}", DoubleValue(string), IntValue(param)));
}

oo::MaybeUnits Operator_multiply(const oo::MaybeUnits& string,
                                 const oo::MaybeUnits& param) {
  return UnitsFromUTF8(
      std::format("{:g}", DoubleValue(string) * DoubleValue(param)));
}

oo::MaybeUnits Operator_add(const oo::MaybeUnits& string,
                            const oo::MaybeUnits& param) {
  return UnitsFromUTF8(
      std::format("{:g}", DoubleValue(string) + DoubleValue(param)));
}

/*	ApplyOneOperator(string, op, param)

        Apply a single formatting operator to a string.

        For example, the expansion expression "[distance|precision:1]" will be
        expanded by a call to ApplyOneOperator(@"distance", @"precision", @"1").

        <param> may be nil, indicating an operator with no parameter (no colon).
 */
oo::MaybeUnits ApplyOneOperator(const oo::MaybeUnits& string,
                                const oo::Units& op,
                                const oo::MaybeUnits& param) {
  static const struct {
    std::u16string_view name;
    oo::MaybeUnits (*function)(const oo::MaybeUnits& string,
                               const oo::MaybeUnits& param);
  } operators[] = {

#define OPERATOR(name) {u## #name, Operator_##name}
      OPERATOR(dcr),       OPERATOR(cr),       OPERATOR(icr), OPERATOR(idcr),
      OPERATOR(precision), OPERATOR(multiply), OPERATOR(add),
#undef OPERATOR
  };

  for (const auto& entry : operators) {
    if (op == entry.name) return entry.function(string, param);
  }

  OO_LOG_ERR("strings.expand.invalidOperator",
             "Unknown string expansion operator {}", oo::utf16ToUtf8(op));
  return string;
}

/*	ExpandDigitKey(context, characters, keyStart, keyLength, sizeLimit,
   recursionLimit)

        Expand a key (as per ExpandKey()) consisting entirely of digits.
   <keyStart> and <keyLength> specify the range of characters containing the
   key.

        Digit-only keys are looked up in the system_description array in
        descriptions.plist, which is expected to contain only arrays of strings
   (no loose strings). When an array is retrieved, a string is selected from it
        at random and the result is expanded recursively by calling Expand().
*/
oo::MaybeUnits ExpandDigitKey(StringExpansionContext* context,
                              const char16_t* characters, std::size_t keyStart,
                              std::size_t keyLength, std::size_t sizeLimit,
                              std::size_t recursionLimit) {
  OOCParameterAssert(context != nullptr && characters != nullptr);

  std::size_t keyValue = 0, idx;
  for (idx = keyStart; idx < (keyStart + keyLength); idx++) {
    OOCAssert(
        isdigit(characters[idx]), "%s called with non-numeric key [%s].",
        __FUNCTION__,
        oo::utf16ToUtf8(UnitsWithCharacters(characters + keyStart, keyLength))
            .c_str());

    keyValue = keyValue * 10 + characters[idx] - '0';
  }

  // Retrieve selected system_description entry (it must be an array, as
  // PListView required).
  const oo::PList& sysDescs = GetSystemDescriptions(context);
  const oo::PList* entry = sysDescs.at(keyValue);
  if (entry != nullptr && !entry->isArray()) entry = nullptr;

  if (entry == nullptr) [[unlikely]] {
    if (keyValue >= context->sysDescCount) {
      SyntaxWarning(
          context, "strings.expand.warning.outOfRangeKey",
          "Out-of-range system description expansion key [%s] in string.",
          oo::utf16ToUtf8(UnitsWithCharacters(characters + keyStart, keyLength))
              .c_str());
    } else {
      // This is out of the scope of whatever triggered it, so shouldn't be a JS
      // warning.
      OO_LOG_ERR("strings.expand.invalidData", "{}",
                 "descriptions.plist entry system_description must be an array "
                 "of arrays of strings.");
    }
    return std::nullopt;
  }

  // Select a random sub-entry.
  std::size_t selection, count = entry->count();
  std::size_t rnd = static_cast<std::size_t>(OO_EXPANDER_RANDOM);
  if (count == 5 && !context->useGoodRNG) {
    // Time-honoured Elite-compatible way for five items.
    if (rnd >= 0xCC)
      selection = 4;
    else if (rnd >= 0x99)
      selection = 3;
    else if (rnd >= 0x66)
      selection = 2;
    else if (rnd >= 0x33)
      selection = 1;
    else
      selection = 0;
  } else {
    // General way.
    selection = (rnd * count) / 256;
  }

  // Look up and recursively expand string.
  // PListView's string rule: a string, or a number's -stringValue; anything
  // else is none.
  oo::MaybeUnits string;
  const oo::PList* choice = entry->at(selection);
  if (choice != nullptr && (choice->isString() || choice->isNumber()))
    string = oo::utf8ToUtf16(entry->at<std::string>(selection));
  OOCParameterAssert(string.has_value());
  if (!string.has_value()) return std::nullopt;
  return Expand(context, *string, sizeLimit, recursionLimit);
}

/*	ExpandStringKey(context, key, sizeLimit, recursionLimit)

        Expand a key (as per ExpandKey()) which doesn't consist entirely of
   digits. Looks for the key in a number of different places in prioritized
   order.
*/
oo::MaybeUnits ExpandStringKey(StringExpansionContext* context,
                               const oo::Units& key, std::size_t sizeLimit,
                               std::size_t recursionLimit) {
  OOCParameterAssert(context != nullptr);

  // The lookups are keyed by the key's UTF-8 (WTF-8: lossless, as the string
  // object the engine built).
  const std::string keyString = oo::utf16ToUtf8(key);

  // Overrides have top priority.
  oo::MaybeUnits result = ExpandStringKeyOverride(context, keyString);

  // Specials override descriptions.plist.
  if (!result.has_value()) result = ExpandStringKeySpecial(context, keyString);

  // Now try descriptions.plist.
  if (!result.has_value())
    result = ExpandStringKeyFromDescriptions(context, keyString, sizeLimit,
                                             recursionLimit);

  // For efficiency, descriptions.plist overrides keybindings.
  // OXPers should therefore avoid oolite_key_ description keys
  if (!result.has_value())
    result = ExpandStringKeyKeyboardBinding(context, key);

  // Try mission variables.
  if (!result.has_value())
    result = ExpandStringKeyMissionVariable(context, key, keyString);

  // Try legacy local variables.
  if (!result.has_value())
    result = ExpandStringKeyLegacyLocalVariable(context, keyString);

  // Try legacy script methods. (Upstream calls it for its side effects and
  // discards the result.)
  if (!result.has_value()) ExpandLegacyScriptSelectorKey(context, keyString);

#if WARNINGS
  // None of that worked, so moan a bit.
  if (!result.has_value()) ReportWarningForUnknownKey(context, key, keyString);
#endif

  return result;
}

/*	ExpandStringKeyOverride(context, key)

        Attempt to expand a key by retriving it from the overrides dictionary of
        the context (ultimately from OOExpandDescriptionString()). Overrides are
        used to provide context-specific expansions, such as "[self:name]" in
        comms messages, and can also be used from JavaScript.

        The main difference between overrides and legacy locals is priority.
*/
oo::MaybeUnits ExpandStringKeyOverride(StringExpansionContext* context,
                                       const std::string& key) {
  OOCParameterAssert(context != nullptr);

  const oo::PList* value = context->overrides.find(key);
  if (value != nullptr) {
#if WARNINGS
    if (!value->isString() && !value->isNumber()) {
      SyntaxWarning(context, "strings.expand.warning.invalidOverride",
                    "String expansion override value %s for [%s] is not a "
                    "string or number.",
                    oo::DescriptionOf(*value).c_str(),
                    key.c_str());  // the value (was <ClassName 0x...> of its
                                   // Foundation object)
    }
#endif
    return ValueText(*value);
  }

  return std::nullopt;
}

/*	ExpandStringKeySpecial(context, key)

        Attempt to expand a key by matching a set of special expansion codes
   that call PlayerEntity methods but aren't legacy script methods. Also unlike
        legacy script methods, all these methods return strings.
*/
oo::MaybeUnits ExpandStringKeySpecial(StringExpansionContext* context,
                                      const std::string& key) {
  OOCParameterAssert(context != nullptr);

  const std::string* selector =
      LookUpSelector(SpecialSubstitutionSelectors(), key);
  if (selector != nullptr) {
    OOCAssert(
        oo::EntityRespondsToName(PLAYER, *selector),
        "Special string expansion selector %s for [%s] is not implemented.",
        selector->c_str(), key.c_str());

    // Called by name on the player's C++ part: a string
    // result, as a PList.
    const oo::PList result = oo::CallEntityByName(PLAYER, *selector);
    if (!result.isNull()) {
      const std::string* string = result.getIf<std::string>();
      OOCAssert(string != nullptr,
                "Special string expansion [%s] expanded to %s, but expected a "
                "string.",
                key.c_str(), oo::DescriptionOf(result).c_str());
      if (string != nullptr)
        return UnitsFromOptional(std::optional<std::string>(*string));
    }
  }

  return std::nullopt;
}

/*	ExpandStringKeyKeyboardBinding(context, key)

        Attempt to expand a key by matching it against the keybindings
*/
oo::MaybeUnits ExpandStringKeyKeyboardBinding(StringExpansionContext* context,
                                              const oo::Units& key) {
  OOCParameterAssert(context != nullptr);
  if (HasPrefix(key, u"oolite_key_")) {
    const std::string binding = oo::utf16ToUtf8(key.substr(7));
    return UnitsFromOptional((PLAYER != nullptr
                                  ? PLAYER->keyBindingDescription2(binding)
                                  : std::optional<std::string>()));
  }
  return std::nullopt;
}

/*	SpecialSubstitutionSelectors()

        Retrieve the mapping of special keys for ExpandStringKeySpecial() to
        selectors.
*/
const SelectorTable* SpecialSubstitutionSelectors() {
  static SelectorTable* specials = nullptr;
  if (specials != nullptr) return specials;

  struct {
    std::string_view key;
    std::string_view selector;
  } selectors[] = {
      {"commander_name", "commanderName_string"},
      {"commander_shipname", "commanderShip_string"},
      {"commander_shipdisplayname", "commanderShipDisplayName_string"},
      {"commander_rank", "commanderRank_string"},
      {"commander_kills", "commanderKillsAsString"},
      {"commander_legal_status", "commanderLegalStatus_string"},
      {"commander_bounty", "commanderBountyAsString"},
      {"credits_number", "creditsFormattedForSubstitution"},
      {"_oo_legacy_credits_number", "creditsFormattedForLegacySubstitution"}};

  unsigned i, count = std::size(selectors);

  specials = new SelectorTable;
  for (i = 0; i < count; i++) {
    specials->emplace(selectors[i].key, selectors[i].selector);
  }

  return specials;
}

/*	ExpandStringKeyFromDescriptions(context, key, sizeLimit, recursionLimit)

        Attempt to expand a key by looking it up in descriptions.plist. Matches
        may be single strings or arrays of strings. For arrays, one of the
   strings is selected at random.

        Matched strings are expanded recursively by calling Expand().
*/
oo::MaybeUnits ExpandStringKeyFromDescriptions(StringExpansionContext* context,
                                               const std::string& key,
                                               std::size_t sizeLimit,
                                               std::size_t recursionLimit) {
  // The error log names the value's class as the Foundation object built from
  // it did.
  const oo::PList* found = UNIVERSE->descriptions()->find(key);
  oo::PList value = (found != nullptr) ? *found : oo::PList();
  if (!value.isNull()) {
    if (value.isArray() && value.count() > 0) {
      std::size_t rnd =
          static_cast<std::size_t>(OO_EXPANDER_RANDOM % value.count());
      const oo::PList chosen = *value.at(rnd);
      value = chosen;
    }

    const std::string* text = value.getIf<std::string>();
    if (text == nullptr) {
      // This is out of the scope of whatever triggered it, so shouldn't be a JS
      // warning.
      OO_LOG_ERR("strings.expand.invalidData",
                 "String expansion value {} for [{}] from descriptions.plist "
                 "is not a string or number.",
                 oo::DescriptionOf(value),
                 key);  // the value (was <ClassName 0x...> of its Foundation
                        // object)
      return std::nullopt;
    }

    // Expand recursively.
    return Expand(context, oo::utf8ToUtf16(*text), sizeLimit, recursionLimit);
  }

  return std::nullopt;
}

/*	ExpandStringKeyMissionVariable(context, key)

        Attempt to expand a key by matching it to a mission variable.
*/
oo::MaybeUnits ExpandStringKeyMissionVariable(
    StringExpansionContext* /* context */, const oo::Units& key,
    const std::string& keyString) {
  if (HasPrefix(key, u"mission_")) {
    // A string variable expands to its text; an unset one to nothing. A
    // variable that is not a string (an array, from the legacy set-list action)
    // expands as unset.
    const oo::PList value =
        (PLAYER != nullptr ? PLAYER->missionVariableForKey(keyString)
                           : oo::PList());
    const std::string* text = value.getIf<std::string>();
    return UnitsFromOptional(
        (text != nullptr) ? std::optional<std::string>(*text) : std::nullopt);
  }

  return std::nullopt;
}

/*	ExpandStringKeyMissionVariable(context, key)

        Attempt to expand a key by matching it to a legacy local variable.

        The main difference between overrides and legacy locals is priority.
*/
oo::MaybeUnits ExpandStringKeyLegacyLocalVariable(
    StringExpansionContext* context, const std::string& key) {
  const oo::PList* value = context->legacyLocals.find(key);
  if (value == nullptr) return std::nullopt;
  return ValueText(*value);
}

oo::MaybeUnits UnitsFromOptional(const std::optional<std::string>& string) {
  if (!string.has_value()) return std::nullopt;
  return oo::utf8ToUtf16(*string);
}

/*	A mixed value's description, as the lookups above inserted it: a string
   is itself, a number its string value (oo::plist_get::numberStringValue),
   anything else its description.
*/
oo::Units ValueText(const oo::PList& value) {
  if (const std::string* text = value.getIf<std::string>())
    return oo::utf8ToUtf16(*text);
  if (value.isNumber())
    return oo::utf8ToUtf16(oo::plist_get::numberStringValue(value));
  return oo::utf8ToUtf16(oo::DescriptionOf(value));
}

/*	ExpandLegacyScriptSelectorKey(context, key)

        Attempt to expand a key by treating it as a legacy script query method
   and invoking it. Only whitelisted methods are permitted, and aliases are
        respected.
*/
oo::MaybeUnits ExpandLegacyScriptSelectorKey(StringExpansionContext* context,
                                             const std::string& key) {
  OOCParameterAssert(context != nullptr);

  const std::string* selector = LookUpLegacySelector(key);

  if (selector != nullptr) {
    // Called by name on the player's C++ part: "%@" of the
    // result, as [[result description]] gave it (nil stays nil).
    const oo::PList result = oo::CallEntityByName(PLAYER, *selector);
    return UnitsFromOptional(
        !result.isNull() ? std::optional<std::string>(oo::DescriptionOf(result))
                         : std::nullopt);
  } else {
    return std::nullopt;
  }
}

/*	LookUpLegacySelector(key)

        If <key> is a whitelisted legacy script query method, or aliases to one,
        return the corresponding selector.
*/
const std::string* LookUpLegacySelector(const std::string& key) {
  const std::string* selector = nullptr;
  static SelectorTable* selectorCache = nullptr;

  // Try cache lookup.
  selector = LookUpSelector(selectorCache, key);

  if (selector == nullptr) {
    // whitelist.plist's query methods (its string entries) and aliases (a
    // string, or a number's -stringValue, as PListView's string rule gave),
    // built once. ResourceManager is not migrated here: its dictionary converts
    // once, with oo::PListFrom.
    static std::map<std::string, std::string, std::less<>>* aliases = nullptr;
    static std::set<std::string, std::less<>>* whitelist = nullptr;
    if (whitelist == nullptr) {
      const oo::PList whitelistDict = ResourceManager::whitelistDictionary();
      whitelist = new std::set<std::string, std::less<>>;
      aliases = new std::map<std::string, std::string, std::less<>>;
      const oo::PList* methods = whitelistDict.find("query_methods");
      if (methods != nullptr && methods->isArray()) {
        for (const oo::PList& method : *methods->getIf<oo::PList::Array>()) {
          if (const std::string* name = method.getIf<std::string>())
            whitelist->insert(*name);
        }
      }
      const oo::PList* aliasDict = whitelistDict.find("query_method_aliases");
      if (aliasDict != nullptr && aliasDict->isDict()) {
        for (const auto& alias : *aliasDict->getIf<oo::PList::Dict>()) {
          if (alias.second.isString() || alias.second.isNumber())
            aliases->emplace(alias.first,
                             aliasDict->get<std::string>(alias.first));
        }
      }
    }

    auto alias = aliases->find(key);
    const std::string& selectorName =
        (alias != aliases->end()) ? alias->second : key;

    if (whitelist->count(selectorName) != 0) {
      /*	This is an assertion, not a warning, because whitelist.plist is
              part of the game and cannot be overriden by OXPs. If there is an
              invalid selector in the whitelist, it's a game bug.
      */
      OOCAssert(oo::EntityRespondsToName(PLAYER, selectorName),
                "Player does not respond to whitelisted query selector %s.",
                key.c_str());

      // Add it to cache (the name; a map's element does not move).
      if (selectorCache == nullptr) {
        selectorCache = new SelectorTable;
      }
      selector = &selectorCache->emplace(key, selectorName).first->second;
    }
  }

  return selector;
}

#if WARNINGS
/*	ReportWarningForUnknownKey(context, key)

        Called when we fall through all the various ways of expanding string
   keys above. If the key looks like a legacy script query method, assume it is
        and report a bad selector. Otherwise, report it as an unknown key.
*/
void ReportWarningForUnknownKey(StringExpansionContext* context,
                                const oo::Units& keyUnits,
                                const std::string& key) {
  if (HasSuffix(keyUnits, u"_string") || HasSuffix(keyUnits, u"_number") ||
      HasSuffix(keyUnits, u"_bool")) {
    SyntaxError(context, "strings.expand.invalidSelector",
                "Unpermitted legacy script method [%s] in string.",
                key.c_str());
  } else {
    SyntaxWarning(context, "strings.expand.warning.unknownExpansion",
                  "Unknown expansion key [%s] in string.", key.c_str());
  }
}
#endif

/*	ExpandKey(context, characters, size, idx, replaceLength)

        Expand an escape code. <idx> is the index of the % sign introducing the
        escape code. Supported escape codes are:
                %H
                %I
                %N
                %R
                %J###, where ### are three digits
                %G######, where ### are six digits
                %%
                %[
                %]

        In addition, the codes %@, %d and %. are ignored, because they're used
        with the string formatter (%@ / %d) on strings that have
        already been expanded.

        Any other code results in a warning.
*/
oo::MaybeUnits ExpandPercentEscape(StringExpansionContext* context,
                                   const char16_t* characters, std::size_t size,
                                   std::size_t idx,
                                   std::size_t* replaceLength) {
  OOCParameterAssert(context != nullptr && characters != nullptr &&
                     replaceLength != nullptr);
  OOCParameterAssert(characters[idx] == '%');

  // All %-escapes except %J and %G are 2 characters.
  *replaceLength = 2;
  uint16_t selector = characters[idx + 1];

  switch (selector) {
    case 'H':
      return GetSystemName(context);

    case 'I':
      return GetSystemNameIan(context);

    case 'N':
      return GetRandomNameN(context);

    case 'R':
      // to keep planet description generation consistent with earlier versions
      // this must be done after all other substitutions in a second pass.
      context->hasPercentR = true;
      return oo::Units(u"%R");

    case 'G':
      return ExpandSystemNameForGalaxyEscape(context, characters, size, idx,
                                             replaceLength);

    case 'J':
      return ExpandSystemNameEscape(context, characters, size, idx,
                                    replaceLength);

    case '%':
      return oo::Units(u"%");

    case '[':
      return oo::Units(u"[");

    case ']':
      return oo::Units(u"]");

      /*	These are string formatting specifiers that occur in
              descriptions.plist. The '.' is for floating-point (g and f)
              specifiers that have field widths specified. No unadorned
              %f or %g is found in vanilla Oolite descriptions.plist.

              Ideally, these would be replaced with the caller formatting
              the value and passing it as an override - it would be safer
              and make descriptions.plist clearer - but it would be a big
              job and uglify the callers.
              -- Ahruman 2012-10-05
      */
    case '@':
    case 'd':
    case '.':
      return std::nullopt;

    default:
      // Yay, percent signs!
      /*	The old GNUstep formatter's %lc of a unichar kept only its low
         byte: the high byte of the character it produced was undefined (probed
         on gnustep-base 1.31.1: U+0041 gave U+DB41, U+0301 gave U+D301 or
         U+DB01 from run to run; tests/unit/expander masks all but the low
         byte). The low byte is printed, as the character U+00xx (%c for ASCII,
              so that U+0000 survives; UTF-8 through %s above it).
      */
      const unsigned char lowByte = static_cast<unsigned char>(selector & 0xFF);
      if (lowByte < 0x80) {
        SyntaxWarning(context, "strings.expand.warning.unknownPercentEscape",
                      "Unknown escape code in string: %%%c. (To encode a %% "
                      "sign without this warning, use %%%% - but prefer "
                      "\"percent\" in prose writing.)",
                      lowByte);
      } else {
        SyntaxWarning(context, "strings.expand.warning.unknownPercentEscape",
                      "Unknown escape code in string: %%%s. (To encode a %% "
                      "sign without this warning, use %%%% - but prefer "
                      "\"percent\" in prose writing.)",
                      oo::utf16ToUtf8(oo::Units(1, lowByte)).c_str());
      }

      return std::nullopt;
  }
}

/* ExpandPercentR(context, string)
         Replaces all %R in string with its expansion.
         Separate to allow this to be delayed to the end of the string expansion
         for compatibility with 1.76 expansion of %R in planet descriptions
*/
oo::MaybeUnits ExpandPercentR(StringExpansionContext* context,
                              const oo::MaybeUnits& input) {
  // [input rangeOfString:@"%R"] (a nil input answered location 0, i.e. found).
  if (input.has_value() && FindUnits(*input, u"%R", 0) == oo::kNotFound) {
    return input;  // no %Rs to replace
  }
  const oo::MaybeUnits percentR = GetRandomNameR(context);
  if (!input.has_value())
    return std::nullopt;  // Unreachable: %R is only seen in an expanded string.
  oo::Units output = *input;

  /* This loop should be completely unnecessary, but for some reason
   * replaceOccurrencesOfString sometimes only replaces the first
   * instance of %R if percentR contains the non-ASCII
   * digrams-apostrophe character.  (I guess
   * http://lists.gnu.org/archive/html/gnustep-dev/2011-10/msg00048.html
   * this bug in GNUstep's implementation here, which is in 1.22) So
   * to cover that case, if there are still %R in the string after
   * replacement, try again. Affects things like thargoid curses, and
   * particularly %Rful expansions of [nom]. Probably this can be
   * tidied up once GNUstep 1.22 is ancient history, but that'll be a
   * few years yet. - CIM 15/1/2013 */

  do {
    // Every "%R" replaced, unit for unit (a literal search).
    oo::Units replaced;
    replaced.reserve(output.size());
    for (std::size_t i = 0; i < output.size();) {
      if (i + 1 < output.size() && output[i] == u'%' && output[i + 1] == u'R') {
        replaced += *percentR;
        i += 2;
      } else {
        replaced += output[i++];
      }
    }
    output = replaced;
  } while (FindUnits(output, u"%R", 0) != oo::kNotFound);

  return output;
}

/*	ExpandSystemNameForGalaxyEscape(context, characters, size, idx,
   replaceLength)

        Expand a %G###### code by looking up the corresponding system name in
   any cgalaxy.
*/
oo::MaybeUnits ExpandSystemNameForGalaxyEscape(StringExpansionContext* context,
                                               const char16_t* characters,
                                               std::size_t size,
                                               std::size_t idx,
                                               std::size_t* replaceLength) {
  OOCParameterAssert(context != nullptr && characters != nullptr &&
                     replaceLength != nullptr);
  OOCParameterAssert(characters[idx + 1] == 'G');

  // A valid %G escape is always eight characters including the six digits.
  *replaceLength = 8;

#define kInvalidGEscapeMessage \
  "String escape code %G must be followed by six integers."
  if (size - idx < 8) [[unlikely]] {
    // Too close to end of string to actually have six characters, let alone six
    // digits.
    SyntaxError(context, "strings.expand.invalidJEscape", "%s",
                kInvalidGEscapeMessage);
    return std::nullopt;
  }

  char hundreds = static_cast<char>(characters[idx + 2]);
  char tens = static_cast<char>(characters[idx + 3]);
  char units = static_cast<char>(characters[idx + 4]);
  char galHundreds = static_cast<char>(characters[idx + 5]);
  char galTens = static_cast<char>(characters[idx + 6]);
  char galUnits = static_cast<char>(characters[idx + 7]);

  if (!(isdigit(hundreds) && isdigit(tens) && isdigit(units) &&
        isdigit(galHundreds) && isdigit(galTens) && isdigit(galUnits))) {
    SyntaxError(context, "strings.expand.invalidJEscape", "%s",
                kInvalidGEscapeMessage);
    return std::nullopt;
  }

  oo::SystemID sysID =
      (hundreds - '0') * 100 + (tens - '0') * 10 + (units - '0');
  if (sysID > oo::kMaximumSystemID) {
    SyntaxError(context, "strings.expand.invalidJEscape.range",
                "String escape code %%G%3u for system is out of range (must be "
                "less than %u).",
                sysID, oo::kMaximumSystemID + 1);
    return std::nullopt;
  }

  oo::GalaxyID galID = static_cast<oo::GalaxyID>(
      (galHundreds - '0') * 100 + (galTens - '0') * 10 + (galUnits - '0'));
  if (galID > oo::kMaximumGalaxyID) {
    SyntaxError(context, "strings.expand.invalidJEscape.range",
                "String escape code %%G%3u for galaxy is out of range (must be "
                "less than %u).",
                galID, oo::kMaximumGalaxyID + 1);
    return std::nullopt;
  }

  return UnitsFromOptional((UNIVERSE != nullptr
                                ? UNIVERSE->getSystemName(sysID, galID)
                                : std::optional<std::string>()));
}

/*	ExpandSystemNameEscape(context, characters, size, idx, replaceLength)

        Expand a %J### code by looking up the corresponding system name in the
        current galaxy.
*/
oo::MaybeUnits ExpandSystemNameEscape(StringExpansionContext* context,
                                      const char16_t* characters,
                                      std::size_t size, std::size_t idx,
                                      std::size_t* replaceLength) {
  OOCParameterAssert(context != nullptr && characters != nullptr &&
                     replaceLength != nullptr);
  OOCParameterAssert(characters[idx + 1] == 'J');

  // A valid %J escape is always five characters including the three digits.
  *replaceLength = 5;

#define kInvalidJEscapeMessage \
  "String escape code %J must be followed by three integers."
  if (size - idx < 5) [[unlikely]] {
    // Too close to end of string to actually have three characters, let alone
    // three digits.
    SyntaxError(context, "strings.expand.invalidJEscape", "%s",
                kInvalidJEscapeMessage);
    return std::nullopt;
  }

  char hundreds = static_cast<char>(characters[idx + 2]);
  char tens = static_cast<char>(characters[idx + 3]);
  char units = static_cast<char>(characters[idx + 4]);

  if (!(isdigit(hundreds) && isdigit(tens) && isdigit(units))) {
    SyntaxError(context, "strings.expand.invalidJEscape", "%s",
                kInvalidJEscapeMessage);
    return std::nullopt;
  }

  oo::SystemID sysID =
      (hundreds - '0') * 100 + (tens - '0') * 10 + (units - '0');
  if (sysID > oo::kMaximumSystemID) {
    SyntaxError(
        context, "strings.expand.invalidJEscape.range",
        "String escape code %%J%3u is out of range (must be less than %u).",
        sysID, oo::kMaximumSystemID + 1);
    return std::nullopt;
  }

  return UnitsFromOptional((UNIVERSE != nullptr
                                ? UNIVERSE->getSystemName(sysID)
                                : std::optional<std::string>()));
}

void AppendCharacters(oo::MaybeUnits* result, const char16_t* characters,
                      std::size_t start, std::size_t end) {
  OOCParameterAssert(result != nullptr && characters != nullptr &&
                     start <= end);

  if (!result->has_value()) {
    // Ensure there is a string. We want this even if the range is empty.
    result->emplace();
  }

  if (start == end) return;

  // The segment goes through UnitsWithCharacters()'s byte-order-mark handling.
  **result += UnitsWithCharacters(characters + start, end - start);
}

oo::MaybeUnits GetSystemName(StringExpansionContext* context) {
  OOCParameterAssert(context != nullptr);
  if (!context->systemName.has_value()) {
    context->systemName = UnitsFromOptional(
        (UNIVERSE != nullptr ? UNIVERSE->getSystemName(
                                   (PLAYER != nullptr ? PLAYER->systemID() : 0))
                             : std::optional<std::string>()));
  }

  return context->systemName;
}

oo::MaybeUnits GetSystemNameIan(StringExpansionContext* context) {
  OOCParameterAssert(context != nullptr);

  if (!context->systemNameWithIan.has_value()) {
    const std::optional<std::string> name = oo::ExpandDescriptionString(
        context->seed, "planetname-possessive", oo::PList(), oo::PList(),
        std::nullopt,
        oo::ExpandOptions::kDisallowPercentI | oo::ExpandOptions::kGoodRNG |
            oo::ExpandOptions::kKey);
    if (name.has_value()) context->systemNameWithIan = oo::utf8ToUtf16(*name);
  }

  return context->systemNameWithIan;
}

oo::MaybeUnits GetRandomNameN(StringExpansionContext* context) {
  OOCParameterAssert(context != nullptr);

  if (!context->randomNameN.has_value()) {
    context->randomNameN = NewRandomDigrams(context);
  }

  return context->randomNameN;
}

oo::MaybeUnits GetRandomNameR(StringExpansionContext* context) {
  OOCParameterAssert(context != nullptr);

  if (!context->randomNameR.has_value()) {
    context->randomNameR = OldRandomDigrams();
  }

  return context->randomNameR;
}

const oo::PList& GetSystemDescriptions(StringExpansionContext* context) {
  OOCParameterAssert(context != nullptr);

  if (context->systemDescriptions.isNull()) {
    // Looked up once per context (PListView's rule: an array, else none).
    const oo::PList* found =
        UNIVERSE->descriptions()->find("system_description");
    const oo::PList value = (found != nullptr) ? *found : oo::PList();
    if (value.isArray()) context->systemDescriptions = value;
    context->sysDescCount = context->systemDescriptions.count();
  }

  return context->systemDescriptions;
}

/*	The digram at <location> of descriptions.plist's "digrams" string (two
   units from <location>; it raises past the end).
*/
oo::Units Digram(const std::optional<std::string>& digrams,
                 const oo::Units& units, std::size_t location) {
  if (location + 2 > units.size()) {
    // Raises OORangeException with GNUstep's -substringWithRange: text, as
    // messaging the string did; answers nothing (nothing appended) for no
    // <digrams> (a message to nil).
    if (digrams.has_value()) {
      oo::RaiseException(
          oo::RangeException,
          "in substringWithRange:, range { %zu, 2 } extends beyond size (%zu)",
          static_cast<std::size_t>(location),
          static_cast<std::size_t>(units.size()));
    }
    return oo::Units();
  }
  return units.substr(location, 2);
}

// -capitalizedString.
oo::Units Capitalized(const oo::Units& name) {
  return oo::utf8ToUtf16(oo::str::capitalized(oo::utf16ToUtf8(name)));
}

/*	Generates pseudo-random digram string using gen_rnd_number()
        (world-generation consistent PRNG), but misses some possibilities. Used
        for "%R" description string for backwards compatibility.
*/
oo::Units OldRandomDigrams() {
  /* The only point of using %R is for world generation, so there's
   * no point in checking the context */
  unsigned len = gen_rnd_number() & 3;
  const oo::PList* digramsEntry = UNIVERSE->descriptions()->find("digrams");
  const std::string* digramsText =
      (digramsEntry != nullptr) ? digramsEntry->getIf<std::string>() : nullptr;
  const std::optional<std::string> digrams =
      (digramsText != nullptr) ? std::optional<std::string>(*digramsText)
                               : std::nullopt;
  const oo::Units digramUnits =
      UnitsFromOptional(digrams).value_or(oo::Units());
  oo::Units name;

  for (unsigned i = 0; i <= len; i++) {
    unsigned x = gen_rnd_number() & 0x3e;
    name += Digram(digrams, digramUnits, x);
  }

  return Capitalized(name);
}

/*	Generates pseudo-random digram string. Used for "%N" description string.
 */
oo::Units NewRandomDigrams(StringExpansionContext* context) {
  unsigned length = (static_cast<unsigned int>(OO_EXPANDER_RANDOM % 4)) + 1;
  if ((static_cast<unsigned int>(OO_EXPANDER_RANDOM % 5)) <
      ((length == 1) ? 3 : 1))
    ++length;  // Make two-letter names rarer and 10-letter names happen
               // sometimes
  const oo::PList* digramsEntry = UNIVERSE->descriptions()->find("digrams");
  const std::string* digramsText =
      (digramsEntry != nullptr) ? digramsEntry->getIf<std::string>() : nullptr;
  const std::optional<std::string> digrams =
      (digramsText != nullptr) ? std::optional<std::string>(*digramsText)
                               : std::nullopt;
  const oo::Units digramUnits =
      UnitsFromOptional(digrams).value_or(oo::Units());
  std::size_t count = digramUnits.size() / 2;
  oo::Units name;

  for (unsigned i = 0; i != length; ++i) {
    name += Digram(digrams, digramUnits,
                   (static_cast<std::size_t>(OO_EXPANDER_RANDOM % count)) * 2);
  }

  return Capitalized(name);
}

// MARK: -
// MARK: UTF-16 units

/*	+stringWithCharacters:length: / -initWithCharacters:length:, which is
   how the engine used to cut keys, operators and literal segments out of a
   string: GNUstep drops a leading U+FEFF, or drops a leading U+FFFE and
   byte-swaps the rest, and does so twice (probed on gnustep-base 1.31.1).
*/
oo::Units UnitsWithCharacters(const char16_t* characters, std::size_t length) {
  oo::Units units(characters, length);
  for (int pass = 0; pass < 2 && !units.empty(); pass++) {
    if (units[0] == 0xFEFF) {
      units.erase(0, 1);
    } else if (units[0] == 0xFFFE) {
      units.erase(0, 1);
      for (char16_t& c : units) c = static_cast<char16_t>((c << 8) | (c >> 8));
    }
  }
  return units;
}

/*	The location of <target> in the range <from to end>, oo::kNotFound if
   absent: literal units, except that an occurrence a sequence-extending unit
   follows is inside a longer composed sequence and does not count, and that a
   search starting inside a composed sequence (not at the start of the string)
   cannot match at the first unit after it (oo::str::Scanner has the same rule,
   probed on gnustep-base 1.31.1). <target> is ASCII.
*/
std::size_t FindUnits(const oo::Units& string, std::u16string_view target,
                      std::size_t from) {
  std::size_t blind = oo::kNotFound;
  if (from > 0 && from < string.size() &&
      oo::str::detail::extendsSequence(string[from])) {
    blind = from;
    while (blind < string.size() &&
           oo::str::detail::extendsSequence(string[blind]))
      blind++;
  }
  for (std::size_t at = from; at + target.size() <= string.size(); at++) {
    if (at == blind ||
        std::u16string_view(string).substr(at, target.size()) != target)
      continue;
    const std::size_t end = at + target.size();
    if (end < string.size() && oo::str::detail::extendsSequence(string[end]))
      continue;
    return at;
  }
  return oo::kNotFound;
}

// -hasPrefix: / -hasSuffix: (literal) with a non-empty ASCII argument.
bool HasPrefix(const oo::Units& string, std::u16string_view prefix) {
  return string.size() >= prefix.size() &&
         std::u16string_view(string).substr(0, prefix.size()) == prefix;
}

bool HasSuffix(const oo::Units& string, std::u16string_view suffix) {
  return string.size() >= suffix.size() &&
         std::u16string_view(string).substr(string.size() - suffix.size()) ==
             suffix;
}

void SyntaxIssue(StringExpansionContext* context,
                 const std::source_location& where, const char* logMessageClass,
                 const char* prefix, const char* format, ...) {
  OOCParameterAssert(context != nullptr);

  va_list args;
  va_start(args, format);
  const std::string message = oo::str::vformat(format, args);
  va_end(args);

  // Logging and the JavaScript engine call sites are not migrated here: the
  // formatted text is handed on.
  if (oo::log::willDisplay(logMessageClass)) {
    if (context->isJavaScript) {
      /*	NOTE: syntax errors are reported as warnings when called from JS
              because we don't want to start throwing exceptions when the old
              expander didn't.
      */
      JSContext* jsc = OOJSAcquireContext();
      ReportJavaScriptWarning(jsc, "%s", message.c_str());
      OOJSRelinquishContext(jsc);
    } else {
      oo::log::logger().write(logMessageClass, where.function_name(),
                              where.file_name(), where.line(),
                              std::string(prefix) + message);
    }
  }
}

// OOJSReportWarningWithArguments with a printf format and its arguments
// (the engine's va_list entry).
void ReportJavaScriptWarning(JSContext* jsc, const char* format, ...) {
  va_list args;
  va_start(args, format);
  OOJSReportWarningWithArguments(jsc, format, args);
  va_end(args);
}

}  // namespace
