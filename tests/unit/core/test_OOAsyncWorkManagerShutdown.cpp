/*	test_OOAsyncWorkManagerShutdown.cpp
        Unit tests for OOAsyncWorkManager::shutdown() (src/Core/OOAsyncWorkManager.h),
   the quit path's join of the work threads.

        Pins: shutdown() waits for a task a work thread has already started and
   returns once it has finished; it returns promptly when the workers are idle;
   a task added after shutdown is never performed (no worker is left); a second
   shutdown() is harmless. Its own executable, because shutdown() stops the one
   shared manager for the rest of the process. Run: tests/run.sh unit
*/

#include <atomic>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "AsyncWorkManager.h"

// OOLogging.mm would link the game's log file and resource manager in; the
// abstract base class's subclass-responsibility message it defines is never
// reached by these tests.
namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char*) {}

}  // namespace oo


namespace {

class SlowTask : public oo::AsyncWorkTask {
 public:
  void performAsyncTask() override {
    started = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    finished = true;
  }

  std::atomic<bool> started = false;
  std::atomic<bool> finished = false;
};

class FlagTask : public oo::AsyncWorkTask {
 public:
  void performAsyncTask() override { performed = true; }

  std::atomic<bool> performed = false;
};

// Waits (up to five seconds) for a work thread to set flag.
bool WaitFor(const std::atomic<bool>& flag) {
  for (int i = 0; i < 500 && !flag; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return flag;
}

}  // namespace

TEST(OOAsyncWorkManagerShutdown, shutdownWaitsForAStartedTaskThenStopsTheWorkers) {
  oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
  const std::shared_ptr<SlowTask> slow = oo::makeShared<SlowTask>();
  EXPECT_TRUE(manager->addTask(slow.get(), oo::AsyncWorkPriority::kMedium));
  EXPECT_TRUE(WaitFor(slow->started));

  const auto start = std::chrono::steady_clock::now();
  manager->shutdown();
  EXPECT_TRUE(slow->finished);  // the busy worker finished its task first
  EXPECT_TRUE(std::chrono::steady_clock::now() - start < std::chrono::seconds(5));

  // No worker is left to perform a task added now.
  const std::shared_ptr<FlagTask> late = oo::makeShared<FlagTask>();
  manager->addTask(late.get(), oo::AsyncWorkPriority::kHigh);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_TRUE(!late->performed);

  manager->shutdown();  // twice is harmless
}
