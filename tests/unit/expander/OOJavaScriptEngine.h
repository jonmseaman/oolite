// Stub for tests/unit/expander: only what
// src/Core/OOStringExpander.mm uses.
#pragma once
#include <cstdarg>

#include "OOFoundation.h"
namespace ooscript {
struct Context {
  void* p;
};
}  // namespace ooscript
ooscript::Context OOJSAcquireContext(void);
void OOJSRelinquishContext(ooscript::Context context);
// The C++ form the expander calls (printf format).
void cxx_OOJSReportWarningWithArguments(ooscript::Context context,
                                        const char* format, va_list args);
