// clang-format off
/*	oofnd/Thread.hpp
        oo::thread: the thing Oolite asked of NSThread that std::thread
        cannot do (name a thread), plus a test for the main thread.

            Foundation                                    here
            --------------------------------------------  -------------------------------------
            [NSThread isMainThread]                       oo::thread::isMainThread()
            [[NSThread currentThread] setName:n]          oo::thread::setCurrentName(utf8)
            detachNewThreadSelector:toTarget:withObject:  a std::jthread owned by its user
            [[NSThread currentThread] threadDictionary]   a C++ thread_local

        SEMANTICS:

          * The main thread is the thread that ran static initialisation, i.e. the one that calls
            main(): its id is captured then (kMainThreadId below), before any code in main()
            runs. isMainThread() is false only when asked during another TU's static
            initialisation before this header's inline variable is initialised; nothing in the
            game does that.
          * setCurrentName(): SetThreadDescription on Windows (what GNUstep's -setName: calls
            there, measured: GetThreadDescription reads the name back), pthread_setname_np
            elsewhere (Linux truncates to 15 bytes, as GNUstep's call did).

        Header-only, C++23.
*/
// clang-format on

#pragma once

#include <string>
#include <string_view>
#include <thread>

#if defined(_WIN32)
// The three kernel32 entry points, declared exactly as <processthreadsapi.h>
// declares them (HANDLE = void *, HRESULT = long, BOOL = int), so this header
// does not drag <windows.h> (and its BOOL, min/max and interface macros) into
// game code. A TU that also includes <windows.h> sees identical redeclarations.
extern "C" {
__declspec(dllimport) void* __stdcall GetCurrentThread();
__declspec(dllimport) long __stdcall SetThreadDescription(
    void* hThread, const wchar_t* lpThreadDescription);
__declspec(dllimport) int __stdcall MultiByteToWideChar(
    unsigned int CodePage, unsigned long dwFlags, const char* lpMultiByteStr,
    int cbMultiByte, wchar_t* lpWideCharStr, int cchWideChar);
}
#else
#include <pthread.h>
#endif

namespace oo::thread {

// The id of the thread that ran static initialisation (the main thread); see
// the banner.
inline const std::thread::id kMainThreadId = std::this_thread::get_id();

inline bool isMainThread() {
  return std::this_thread::get_id() == kMainThreadId;
}

inline bool setCurrentName(std::string_view utf8Name) {
#if defined(_WIN32)
  const std::string name(utf8Name);
  const unsigned int kCP_UTF8 = 65001;
  int units = MultiByteToWideChar(kCP_UTF8, 0, name.c_str(), -1, nullptr, 0);
  if (units <= 0) return false;
  std::wstring wide(static_cast<size_t>(units), L'\0');
  if (MultiByteToWideChar(kCP_UTF8, 0, name.c_str(), -1, wide.data(), units) !=
      units)
    return false;
  return SetThreadDescription(GetCurrentThread(), wide.c_str()) >= 0;
#elif defined(__APPLE__)
  const std::string name(utf8Name);
  return pthread_setname_np(name.c_str()) == 0;
#else
  std::string name(utf8Name);
  if (name.size() > 15)
    name.resize(15);  // Linux's limit; longer names fail with ERANGE
  return pthread_setname_np(pthread_self(), name.c_str()) == 0;
#endif
}

}  // namespace oo::thread
