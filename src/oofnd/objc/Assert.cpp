/*	oofnd/objc/OOAssert.cpp
        Foundation-free assertions: the failure path (see OOAssert.h).
*/

#include "oofnd/objc/Assert.h"

#include <cstdarg>
#include <cstdio>
#include <string>

#include "oofnd/Log.hpp"

namespace {

std::string FormatV(const char* format, va_list args) {
  std::string text;
  if (format == nullptr) return text;
  va_list measure;
  va_copy(measure, args);
  const int length = std::vsnprintf(nullptr, 0, format, measure);
  va_end(measure);
  if (length > 0) {
    text.resize(static_cast<std::size_t>(length) + 1);
    std::vsnprintf(text.data(), text.size(), format, args);
    text.resize(static_cast<std::size_t>(length));
  }
  return text;
}

[[noreturn]] void Fail(const std::string& reason) {
  // gnustep-base NSLogv()ed the same text first; its NSLog hook logged it in
  // class "gnustep".
  if (oo::log::willDisplay("gnustep"))
    oo::log::logger().write("gnustep", nullptr, nullptr, 0, reason);
  oo::RaiseException(oo::InternalInconsistencyException, "%s", reason.c_str());
  __builtin_unreachable();
}

}  // namespace

namespace oo {

void AssertFailedInFunction(const std::source_location& where,
                              const char* format, ...) {
  va_list args;
  va_start(args, format);
  const std::string message = FormatV(format, args);
  va_end(args);

  std::string reason = std::string(where.file_name()) + ":" +
                       std::to_string(where.line()) + "  Assertion failed in " +
                       where.function_name() + ".  " + message;
  Fail(reason);
}

}  // namespace oo

