/*	test_OOAsyncWorkManager.mm
        Unit tests for OOAsyncWorkManager (src/Core/OOAsyncWorkManager.h).

        Pins what the manager does: one shared manager; nil
   is refused; a task is performed on a work thread and then completed on the
   thread that waits for it or collects pending tasks; a task without
        -completeAsyncTask is performed and never collected; waiting for a task
   that was never added returns at once; an exception thrown by
   -performAsyncTask is swallowed and the task still completes; every priority
   is accepted and performed. The tasks are C++ tasks implementing
   OOAsyncWorkTask. Run: tests/run.sh unit
*/

#include <atomic>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "AsyncWorkManager.h"
#include "oofnd/Exception.hpp"  // oo::Exception

class TestTask : public oo::AsyncWorkTask {
 public:
  TestTask() { callingThread = std::this_thread::get_id(); }

  void performAsyncTask() override {
    performedOnCallingThread = (std::this_thread::get_id() == callingThread);
    performed = true;
    if (shouldThrow)
      throw oo::Exception(oo::GenericException,
                          "");  // the managers swallow the game's exception
  }

  bool hasCompleteAsyncTask() const override { return true; }

  void completeAsyncTask() override {
    completedAfterPerform = performed;
    completedOnCallingThread = (std::this_thread::get_id() == callingThread);
    completions++;
  }

  std::atomic<bool> performed = false;
  std::atomic<bool> performedOnCallingThread = false;
  std::atomic<int> completions = 0;
  bool completedAfterPerform = false;
  bool completedOnCallingThread = false;
  bool shouldThrow = false;
  std::thread::id callingThread;
};

// A task with no completion (the method was optional).
class TestFireAndForgetTask : public oo::AsyncWorkTask {
 public:
  void performAsyncTask() override { performed = true; }

  std::atomic<bool> performed = false;
};

// OOLogging.mm would link the game's log file and resource manager in; the
// abstract base class's subclass-responsibility message it defines is never
// reached by these tests.
namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char*) {}

}  // namespace oo


namespace {

// Waits (up to five seconds) for a work thread to set flag.
bool WaitFor(const std::atomic<bool>& flag) {
  for (int i = 0; i < 500 && !flag; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return flag;
}

}  // namespace

TEST(OOAsyncWorkManager, oneSharedManager) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    EXPECT_TRUE(manager != nullptr);
    EXPECT_TRUE(oo::AsyncWorkManager::sharedAsyncWorkManager() == manager);
  }
}

TEST(OOAsyncWorkManager, nilIsRefused) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    EXPECT_TRUE(!manager->addTask(nullptr, oo::AsyncWorkPriority::kMedium));
    manager->waitForTaskToComplete(nullptr);  // returns at once
  }
}

TEST(OOAsyncWorkManager, waitPerformsThenCompletesOnTheWaitingThread) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    const std::shared_ptr<TestTask> task = oo::makeShared<TestTask>();
    EXPECT_TRUE(manager->addTask(task.get(), oo::AsyncWorkPriority::kMedium));
    manager->waitForTaskToComplete(task.get());
    EXPECT_TRUE(task->performed);
    EXPECT_TRUE(!task->performedOnCallingThread);  // on a work thread
    EXPECT_EQ(task->completions.load(), 1);
    EXPECT_TRUE(task->completedAfterPerform);
    EXPECT_TRUE(task->completedOnCallingThread);
    manager->completePendingTasks();
    EXPECT_EQ(task->completions.load(), 1);  // completed once only
  }
}

TEST(OOAsyncWorkManager, completePendingTasksCollectsReadyTasks) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    const std::shared_ptr<TestTask> task = oo::makeShared<TestTask>();
    EXPECT_EQ(task.use_count(), 1);
    EXPECT_TRUE(manager->addTask(task.get(), oo::AsyncWorkPriority::kHigh));
    EXPECT_TRUE(WaitFor(task->performed));
    // Performed, but completed only when collected: poll until it is ready.
    for (int i = 0; i < 500 && task->completions == 0; i++) {
      manager->completePendingTasks();
      if (task->completions == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(task->completions.load(), 1);
    EXPECT_TRUE(task->completedOnCallingThread);
    manager->waitForTaskToComplete(
        task.get());  // no longer pending: returns at once
    EXPECT_EQ(task->completions.load(), 1);
  }
}

TEST(OOAsyncWorkManager, waitForATaskNeverAddedReturns) {
  {
    const std::shared_ptr<TestTask> task = oo::makeShared<TestTask>();
    oo::AsyncWorkManager::sharedAsyncWorkManager()->waitForTaskToComplete(
        task.get());
    EXPECT_TRUE(!task->performed);
    EXPECT_EQ(task->completions.load(), 0);
  }
}

TEST(OOAsyncWorkManager, taskWithoutCompletionIsPerformed) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    const std::shared_ptr<TestFireAndForgetTask> task =
        oo::makeShared<TestFireAndForgetTask>();
    EXPECT_TRUE(manager->addTask(task.get(), oo::AsyncWorkPriority::kLow));
    EXPECT_TRUE(WaitFor(task->performed));
    manager
        ->completePendingTasks();  // nothing to collect for it; must not hang
  }
}

TEST(OOAsyncWorkManager, exceptionInPerformIsSwallowed) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    const std::shared_ptr<TestTask> task = oo::makeShared<TestTask>();
    task->shouldThrow = true;
    EXPECT_TRUE(manager->addTask(task.get(), oo::AsyncWorkPriority::kMedium));
    manager->waitForTaskToComplete(task.get());
    EXPECT_TRUE(task->performed);
    EXPECT_EQ(task->completions.load(), 1);
  }
}

TEST(OOAsyncWorkManager, everyPriorityIsPerformed) {
  {
    oo::AsyncWorkManager* manager = oo::AsyncWorkManager::sharedAsyncWorkManager();
    const std::shared_ptr<TestTask> low = oo::makeShared<TestTask>();
    const std::shared_ptr<TestTask> medium = oo::makeShared<TestTask>();
    const std::shared_ptr<TestTask> high = oo::makeShared<TestTask>();
    EXPECT_TRUE(manager->addTask(low.get(), oo::AsyncWorkPriority::kLow));
    EXPECT_TRUE(manager->addTask(medium.get(), oo::AsyncWorkPriority::kMedium));
    EXPECT_TRUE(manager->addTask(high.get(), oo::AsyncWorkPriority::kHigh));
    manager->waitForTaskToComplete(low.get());
    manager->waitForTaskToComplete(medium.get());
    manager->waitForTaskToComplete(high.get());
    EXPECT_TRUE(low->completions == 1 && medium->completions == 1 &&
             high->completions == 1);
  }
}
