/*

OOPListSchemaVerifier.h

Utility class to verify the structure of a property list based on a schema
(which is itself a property list).

The delegate is the C++ interface OOPListSchemaVerifierDelegate.

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

#include "OXPVerifier.h"

#if OO_OXP_VERIFIER_ENABLED

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "OOEnumFlags.h"
#include "OOFoundation.h"
#include "OOFunctionAttributes.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/objc/Object.h"

/*	A schema verifier error: what the
   Foundation error object carried. domain is kOOPListSchemaVerifierErrorDomain,
   code an OOPListSchemaVerifierErrorCode, failureReason the old
   -localizedFailureReason, and userInfo a Dict of the error keys below
        (kPListKeyPathErrorKey and the error's own; a nested error is held as a
   Dict of these four fields). Where the old error could be nil, it is a
   std::optional.
*/
namespace oo {

struct PListSchemaVerifierError {
  std::string domain;
  int code = 0;
  std::optional<std::string> failureReason;
  oo::PList userInfo;
};

}  // namespace oo


/*	The key path the verification core hands down, one link per level.
   Defined in OOPListSchemaVerifier.cpp, the only file that uses it; the
   verification core's members below take it.
*/
struct BackLinkChain;

namespace oo {

class PListSchemaVerifier;

/*	The verifier's delegate: was the informal OOObject
   (OOPListSchemaVerifierDelegate) category, any object answering the two
   selectors below. The verifier holds it unretained. Both selectors begin with
   verifier:, which a verifier stage could not use as a member name (it would
   hide the stage's verifier()), so each member adds the selector's
   distinguishing keyword.
*/
class PListSchemaVerifierDelegate {
 public:
  // Handle "delegated types". Return true for valid, false for invalid.
  // name: a string; keyPath: an array of strings and numbers; typeKey: a
  // string.
  virtual bool verifierTestProperty(
      PListSchemaVerifier* verifier, const oo::PList& rootPList,
      const std::string& name, const oo::PList& subPList,
      const oo::PList& keyPath, const oo::PList& typeKey,
      std::optional<PListSchemaVerifierError>* outError) = 0;

  /*	Method notifying of verification failure.
          Return true to continue verifying, false to stop.
  */
  // name: a string; localSchema: the schema type specifier (a string or a
  // dictionary).
  virtual bool verifierFailedForProperty(
      PListSchemaVerifier* verifier, const oo::PList& rootPList,
      const std::string& name, const oo::PList& subPList,
      const PListSchemaVerifierError& error,
      const oo::PList& localSchema) = 0;

 protected:
  ~PListSchemaVerifierDelegate() = default;
};

class PListSchemaVerifier : public oo::Object {
 public:
  static std::shared_ptr<PListSchemaVerifier> verifierWithSchema(
      const oo::PList& schema);  // null for a null schema
  static std::shared_ptr<PListSchemaVerifier> initWithSchema(
      const oo::PList& schema);  // -initWithSchema:; null for a null schema

  void setDelegate(PListSchemaVerifierDelegate* delegate);  // not retained
  PListSchemaVerifierDelegate* delegate();

  bool verifyPropertyList(const oo::PList& plist, const std::string& name);

  /*	Convert a key path (such as provided to the delegate method
          -verifier:withPropertyList:failedForProperty:atPath:expectedType:) to
     a human-readable string. Strings are separated by dots and numbers are give
          brackets. For instance, the key path ( "adder-player", "custom_views",
     0, "view_description" ) is transfomed to
          "adder-player.custom_views[0].view_description".
  */
  static std::optional<std::string> descriptionForKeyPath(
      const oo::PList&
          keyPath);  // a null or empty path is "root"; nullopt for a component
                     // that is neither string nor number

  // Internal (the OOPrivate category): the verification core and the delegate
  // calls, which the type verifiers in OOPListSchemaVerifier.cpp (file-static
  // functions) call directly.
  bool delegateVerifierWithPropertyList(
      const oo::PList& rootPList, const std::string& name,
      const oo::PList& subPList, BackLinkChain keyPath,
      const oo::PList& typeKey,
      std::optional<PListSchemaVerifierError>* outError);
  bool delegateVerifierWithPropertyList(const oo::PList& rootPList,
                                        const std::string& name,
                                        const oo::PList& subPList,
                                        const PListSchemaVerifierError& error,
                                        const oo::PList& localSchema);
  bool verifyPList(const oo::PList& rootPList, const std::string& name,
                   const oo::PList& subProperty, const oo::PList& subSchema,
                   BackLinkChain keyPath, bool tentative,
                   std::optional<PListSchemaVerifierError>* outError,
                   bool* outStop);
  oo::PList resolveSchemaType(
      const oo::PList& specifier, BackLinkChain keyPath,
      std::optional<PListSchemaVerifierError>*
          outError);  // null: not resolved (*outError says why)

 private:
  explicit PListSchemaVerifier(
      const oo::PList& schema);  // -initWithSchema:, less its failure

  oo::PList _schema;
  oo::PList _definitions;  // the schema's $definitions (null if none)

  PListSchemaVerifierDelegate* _delegate = {};  // Not retained.
  uint32_t _badDelegateWarning : 1 = 0;
};

// Error domain and codes used to report schema verifier errors (UTF-8; the
// error's domain and userInfo keys are these texts).
extern const char* const kPListSchemaVerifierErrorDomain;

}  // namespace oo


extern const char* const
    kPListKeyPathErrorKey;  // Array specifying key path in plist.
extern const char* const
    kSchemaKeyPathErrorKey;  // Array specifying key path in schema.

extern const char* const
    kExpectedClassErrorKey;  // Expected class. Nil for vector and quaternion.
extern const char* const
    kExpectedClassNameErrorKey;  // String describing expected class. May be
                                 // more specific (for instance, "boolean" or
                                 // "positive integer" for a number).
extern const char* const
    kUnknownKeyErrorKey;  // Unallowed key found in dictionary.
extern const char* const
    kMissingRequiredKeysErrorKey;  // Array of the required keys not present in
                                   // dictionary, sorted
extern const char* const
    kMissingSubStringErrorKey;  // String or array of strings not found for
                                // OOPListSchemaVerifierErrorCode::kStringPrefixMissing/OOPListSchemaVerifierErrorCode::kStringSuffixMissing/OOPListSchemaVerifierErrorCode::kStringSubstringMissing.
extern const char* const
    kUnnownFilterErrorKey;  // Unrecognized filter specifier for
                            // OOPListSchemaVerifierErrorCode::kSchemaUnknownFilter.
                            // Not specified if filter is not a string.
extern const char* const kErrorsByOptionErrorKey;
extern const char* const
    kUnderlyingErrorErrorKey;  // The delegate's own error for
                               // OOPListSchemaVerifierErrorCode::kDelegatedTypeError.
                               // // Dictionary of errors for oneOf types.

extern const char* const
    kUnknownTypeErrorKey;  // Set for
                           // OOPListSchemaVerifierErrorCode::kSchemaUnknownType.
extern const char* const
    kUndefinedMacroErrorKey;  // Set for
                              // OOPListSchemaVerifierErrorCode::kSchemaUndefiniedMacroReference.

// All plist verifier errors have a short error description in their
// -localizedFailureReason.

namespace oo {

enum class PListSchemaVerifierErrorCode : std::int32_t {
  kNone,
  kInternal,  // PList verifier did something dumb.

  // Verification errors -- property list doesn't match schema.
  kTypeMismatch,  // Basic type mismatch -- array instead of number,
                  // for instance.

  kMinimumConstraintNotMet,  // minimum/minCount/minLength constraint
                             // violated
  kMaximumConstraintNotMet,  // maximum/maxCount/maxLength constraint
                             // violated
  kNumberIsNegative,         // Negative number in positiveFloat.

  kStringPrefixMissing,     // String does not match requiredPrefix rule.
                            // kMissingSubStringErrorKey is set.
  kStringSuffixMissing,     // String does not match requiredSuffix rule.
                            // kMissingSubStringErrorKey is set.
  kStringSubstringMissing,  // String does not match requiredSuffix
                            // rule. kMissingSubStringErrorKey is set.

  kDictionaryUnknownKey,           // Unknown key for dictionary with
                                   // allowOthers = NO.
  kDictionaryMissingRequiredKeys,  // requiredKeys rule is not
                                   // fulfilled. The missing keys are
                                   // listed in
                                   // kMissingRequiredKeysErrorKey.

  kEnumerationBadValue,  // Enumeration type contains string that
                         // isn't in permitted set.

  kOneOfNoMatch,  // No match for oneOf type. kErrorsByOptionErrorKey
                  // is set to a dictionary of type specifiers to
                  // errors. Note that the keys in this dictionary can
                  // be either strings or dictionaries.

  kDelegatedTypeError,  // Delegate's verification method failed. If it
                        // returned an error, this will be in
                        // kUnderlyingErrorErrorKey.

  // Schema errors -- schema is broken.
  kStartOfSchemaErrors = 100,

  kSchemaBadTypeSpecifier,          // Bad type specifier - specifier is not a
                                    // string or a dictionary, or is a
                                    // dictionary with no type key.
                                    // kUndefinedMacroErrorKey is set.
  kSchemaUndefiniedMacroReference,  // Reference to $macro not found
                                    // in $definitions.
  kSchemaUnknownType,               // Unknown type specified in type specifier.
                                    // kUnknownTypeErrorKey is set.
  kSchemaNoOneOfOptions,            // OneOf clause has no options array.
  kSchemaNoEnumerationValues,       // Enumeration clause has no values
                                    // array.
  kSchemaUnknownFilter,             // Bad value for string/enumeration filter
                                    // specifier.
  kSchemaBadComparator,             // String comparision requirement value
                                    // (requiredPrefix etc.) is not a string.

  kLastErrorCode,
};

}  // namespace oo

static_assert(std::to_underlying(oo::PListSchemaVerifierErrorCode::kNone) == 0);
static_assert(std::to_underlying(oo::PListSchemaVerifierErrorCode::kInternal) ==
              1);
static_assert(
    std::to_underlying(oo::PListSchemaVerifierErrorCode::kTypeMismatch) == 2);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kMinimumConstraintNotMet) ==
              3);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kMaximumConstraintNotMet) ==
              4);
static_assert(
    std::to_underlying(oo::PListSchemaVerifierErrorCode::kNumberIsNegative) == 5);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kStringPrefixMissing) == 6);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kStringSuffixMissing) == 7);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kStringSubstringMissing) ==
              8);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kDictionaryUnknownKey) == 9);
static_assert(
    std::to_underlying(
        oo::PListSchemaVerifierErrorCode::kDictionaryMissingRequiredKeys) == 10);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kEnumerationBadValue) == 11);
static_assert(
    std::to_underlying(oo::PListSchemaVerifierErrorCode::kOneOfNoMatch) == 12);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kDelegatedTypeError) == 13);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kStartOfSchemaErrors) == 100);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kSchemaBadTypeSpecifier) ==
              101);
static_assert(
    std::to_underlying(
        oo::PListSchemaVerifierErrorCode::kSchemaUndefiniedMacroReference) ==
    102);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kSchemaUnknownType) == 103);
static_assert(
    std::to_underlying(oo::PListSchemaVerifierErrorCode::kSchemaNoOneOfOptions) ==
    104);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kSchemaNoEnumerationValues) ==
              105);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kSchemaUnknownFilter) == 106);
static_assert(std::to_underlying(
                  oo::PListSchemaVerifierErrorCode::kSchemaBadComparator) == 107);
static_assert(
    std::to_underlying(oo::PListSchemaVerifierErrorCode::kLastErrorCode) == 108);

namespace oo {

inline bool PlistErrorIsSchemaError(PListSchemaVerifierErrorCode error) {
  return PListSchemaVerifierErrorCode::kStartOfSchemaErrors < error &&
         error < PListSchemaVerifierErrorCode::kLastErrorCode;
}

}  // namespace oo


#endif  // OO_OXP_VERIFIER_ENABLED
