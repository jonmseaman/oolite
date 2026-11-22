/*	test_thread.cpp
        Unit tests for oofnd/Thread.hpp: the std::thread
   replacements for NSThread. On Windows this TU includes <windows.h> AFTER the
   header, which also proves the header's kernel32 declarations are identical
   redeclarations.
*/

#include <gtest/gtest.h>

#include "oofnd/Thread.hpp"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>

namespace {

// Runs `body` on a detached std::thread and waits for it to finish. The wait
// is UNBOUNDED: a 10 s bound flaked under a loaded ASan build,
// and a timed-out wait returned while the detached lambda still held references
// to this frame's m/cv/done - a use-after-return in the test itself. A hung
// thread is bounded by the run's own timeout (check-oofnd, ctest, accept.sh's
// budget) instead. Returns true once `body` has run, so callers still check it.
template <typename F>
bool RunDetachedAndWait(F body) {
  std::mutex m;
  std::condition_variable cv;
  bool done = false;
  std::thread([&] {
    body();
    std::lock_guard<std::mutex> lock(m);
    done = true;
    cv.notify_one();
  }).detach();
  std::unique_lock<std::mutex> lock(m);
  cv.wait(lock, [&] { return done; });
  return done;
}

}  // namespace

TEST(thread, mainThreadIsTheStaticInitThread) {
  EXPECT_TRUE(oo::thread::isMainThread());
  EXPECT_TRUE(oo::thread::kMainThreadId == std::this_thread::get_id());
}

TEST(thread, detachedThreadRunsAndIsNotMain) {
  std::atomic<int> ran{0};
  std::atomic<bool> wasMain{true};
  EXPECT_TRUE(RunDetachedAndWait([&] {
    wasMain = oo::thread::isMainThread();
    ++ran;
  }));
  EXPECT_EQ(ran.load(), 1);
  EXPECT_TRUE(!wasMain.load());
}

TEST(thread, detachMovesItsCallable) {
  std::string seen;
  std::string payload = "moved into the thread";
  EXPECT_TRUE(RunDetachedAndWait([&seen, p = std::move(payload)] { seen = p; }));
  EXPECT_EQ(seen, std::string("moved into the thread"));
}

TEST(thread, setCurrentNameIsReadBack) {
  bool ok = false;
  std::string readBack;
  EXPECT_TRUE(RunDetachedAndWait([&] {
    ok = oo::thread::setCurrentName("OOAsyncWorkManager thread 1");
#if defined(_WIN32)
    PWSTR desc = nullptr;
    if (SUCCEEDED(GetThreadDescription(GetCurrentThread(), &desc)) &&
        desc != nullptr) {
      for (const wchar_t* c = desc; *c != L'\0'; ++c)
        readBack.push_back(static_cast<char>(*c));
      LocalFree(desc);
    }
#else
    char buf[64] = {};
    if (pthread_getname_np(pthread_self(), buf, sizeof buf) == 0)
      readBack = buf;
#endif
  }));
  EXPECT_TRUE(ok);
#if defined(_WIN32)
  EXPECT_EQ(readBack, std::string("OOAsyncWorkManager thread 1"));
#else
  EXPECT_EQ(readBack,
              std::string("OOAsyncWorkManager thread 1").substr(0, 15));
#endif
}
