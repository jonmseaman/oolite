// Stub for tests/unit/expander: only what
// src/Core/OOStringExpander.mm uses.
#pragma once
// The C++ forms the expander calls, over the harness's credits formatter
// (test_string_expander.cpp; Foundation-free, the same text).
#include "Foundation.h"
#include "Types.h"
#ifdef __cplusplus
#include <string>
std::string OOHarnessCredits(oo::CreditsQuantity tenths, bool decimal);
namespace oo {

static inline std::string Credits(CreditsQuantity tenthsOfCredits) {
  return OOHarnessCredits(tenthsOfCredits, true);
}
static inline std::string IntCredits(CreditsQuantity integerCredits) {
  return OOHarnessCredits(integerCredits * 10, false);
}

}  // namespace oo

#endif
