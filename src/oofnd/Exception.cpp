/*	oofnd/Exception.cpp
        OORaiseException(), the exception names and oo::CurrentOOException()
   (see oofnd/Exception.hpp).
*/

#include "oofnd/Exception.hpp"

#include <cstdarg>
#include <cstdio>
#include <string>

namespace oo {

const char* const GenericException = "NSGenericException";
const char* const InvalidArgumentException = "NSInvalidArgumentException";
const char* const RangeException = "NSRangeException";
const char* const InternalInconsistencyException =
    "NSInternalInconsistencyException";
const char* const MallocException = "NSMallocException";

}  // namespace oo

namespace {

// printf into a std::string; args is consumed.
std::string FormatReason(const char* format, va_list args) {
  std::string reason;
  va_list measure;
  va_copy(measure, args);
  int length = std::vsnprintf(nullptr, 0, format, measure);
  va_end(measure);
  if (length > 0) {
    reason.resize(static_cast<size_t>(length) + 1);
    std::vsnprintf(reason.data(), reason.size(), format, args);
    reason.resize(static_cast<size_t>(length));
  }
  return reason;
}

}  // namespace

namespace oo {

void RaiseException(const char* name, const char* format, ...) {
  std::string reason;
  if (format != nullptr) {
    va_list args;
    va_start(args, format);
    reason = FormatReason(format, args);
    va_end(args);
  }
  throw oo::Exception(name, reason.c_str());
}

}  // namespace oo

namespace oo {

std::optional<CaughtOOException> CurrentOOException() {
  try {
    throw;
  } catch (const Exception& exception) {
    return CaughtOOException(&exception);
  } catch (...) {
    return std::nullopt;  // not the game's exception: not what @catch
                          // (OOException *) caught
  }
}

}  // namespace oo
