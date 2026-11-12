/*
oofnd/CxxAbi.h

<cxxabi.h> where the C++ runtime has it (libstdc++, libc++). The game uses
abi::__cxa_demangle to print a class's name; the Microsoft runtime has no
<cxxabi.h>, but its type_info::name() is already readable ("class OOSound"), so
there the stand-in returns that name without its class-key, in malloc()ed
memory as __cxa_demangle's is.
*/

#pragma once

#if defined(_MSC_VER) && !defined(__GLIBCXX__) && !defined(_LIBCPP_VERSION)
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

namespace abi {
inline char* __cxa_demangle(const char* name, char*, std::size_t*,
                            int* status) {
  for (const char* key : {"class ", "struct ", "union ", "enum "}) {
    const std::size_t length = std::strlen(key);
    if (std::strncmp(name, key, length) == 0) {
      name += length;
      break;
    }
  }
  const std::size_t size = std::strlen(name) + 1;
  char* copy = static_cast<char*>(std::malloc(size));
  if (copy != nullptr) std::memcpy(copy, name, size);
  if (status != nullptr) *status = (copy != nullptr) ? 0 : -1;
  return copy;
}
}  // namespace abi
#else
#include <cxxabi.h>
#endif
