/*	test_OOScriptTimer.mm
        Unit tests for OOScriptTimer (src/Core/Scripting/OOScriptTimer.h/.mm).

        OOScriptTimer is the abstract base of script timers: it keeps the fire
   time and interval, schedules itself in a priority queue ordered by next fire
   time (OOPriorityQueue), and +updateTimers fires the due ones in order,
   rescheduling the repeating ones. This pins the initialisers (and the one
   that answers null), the interval clamp, setNextTime's refusal while
   scheduled, the description, the firing order, one-shot and repeating timers,
   a timer scheduled while the timers are updating (deferred to the end of the
   update), a timer that stops itself when it fires, +noteGameReset, and the
   ordering selector the queue calls. Run: tests/run.sh unit
*/

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Logging.h"
#include "OOScriptTimer.h"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked. The functions of it that the class calls are defined here instead,
   and count.
*/
static int gSubclassResponsibilities = 0;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
  gSubclassResponsibilities++;
}

const char* const kLogException = "exception";

}  // namespace oo


/*	The universe, as far as OOScriptTimer sees it: the game clock
 * (-getTime), which the test sets.
 */
static double gNow = 100.0;

// The universe as the code under test sees it: a stand-in for the C++ class,
// with the header's names and signatures.
class Universe {
 public:
  oo::TimeAbsolute getTime();
};
typedef Universe FakeUniverse;

oo::TimeAbsolute Universe::getTime() { return gNow; }

class Universe;
Universe* gSharedUniverse = nullptr;

// A timer that records its firings in a shared log, and can be told to stop
// itself or to start another timer when it fires.
static std::vector<std::string> gFired;

class TestTimer final : public OOScriptTimer {
 public:
  static std::shared_ptr<TestTimer> timerWithNextTime(oo::TimeAbsolute nextTime,
                                              oo::TimeDelta interval) {
    std::shared_ptr<TestTimer> timer = std::shared_ptr<TestTimer>(new TestTimer(), oo::Dispose{});
    if (!timer->initWithNextTime(nextTime, interval)) return nullptr;
    return timer;
  }

  void timerFired() override {
    gFired.push_back(name);
    if (stopWhenFired) unscheduleTimer();
    if (startWhenFired != nullptr) startWhenFired->scheduleTimer();
  }

  std::string name;
  bool stopWhenFired = false;
  OOScriptTimer* startWhenFired = nullptr;
};

namespace {

void SetUp(double now) {
  if (gSharedUniverse == nullptr) gSharedUniverse = new FakeUniverse();
  gNow = now;
  gFired.clear();
}

std::shared_ptr<TestTimer> MakeTimer(const char* name, double nextTime,
                             double interval) {
  std::shared_ptr<TestTimer> timer = TestTimer::timerWithNextTime(nextTime, interval);
  if (timer != nullptr) timer->name = name;
  return timer;
}

std::string Fired() {
  std::string result;
  for (const std::string& name : gFired)
    result += (result.empty() ? "" : ",") + name;
  return result;
}

}  // namespace

TEST(OOScriptTimer, initialisers) {
  {
    ::SetUp(100.0);
    std::shared_ptr<OOScriptTimer> timer = OOScriptTimer::timerWithNextTime(110.0, 5.0);
    EXPECT_TRUE(timer != nullptr);
    EXPECT_EQ(timer->nextTime(), 110.0);
    EXPECT_EQ(timer->interval(), 5.0);
    EXPECT_TRUE(!timer->isScheduled());

    // A negative next time is "now + interval"; a non-positive interval is -1
    // (one-shot).
    timer = OOScriptTimer::timerWithNextTime(-1.0, 5.0);
    EXPECT_EQ(timer->nextTime(), 105.0);
    timer = OOScriptTimer::timerWithNextTime(120.0, 0.0);
    EXPECT_EQ(timer->interval(), -1.0);
    EXPECT_EQ(timer->nextTime(), 120.0);

    // A past (or negative) next time with no interval is meaningless: nil.
    EXPECT_TRUE(OOScriptTimer::timerWithNextTime(90.0, -1.0) == nullptr);
    EXPECT_TRUE(OOScriptTimer::timerWithNextTime(-1.0, 0.0) == nullptr);
    EXPECT_TRUE(OOScriptTimer::timerWithNextTime(100.0, -1.0) != nullptr);

    // A one-shot timer fires after the delay.
    timer = OOScriptTimer::oneShotTimerWithDelay(2.5);
    EXPECT_EQ(timer->nextTime(), 102.5);
    EXPECT_EQ(timer->interval(), -1.0);
  }
}

TEST(OOScriptTimer, accessors) {
  {
    ::SetUp(100.0);
    std::shared_ptr<OOScriptTimer> timer = OOScriptTimer::timerWithNextTime(110.0, 5.0);
    timer->setInterval(-3.0);
    EXPECT_EQ(timer->interval(), -1.0);
    timer->setInterval(7.0);
    EXPECT_EQ(timer->interval(), 7.0);
    EXPECT_TRUE(timer->setNextTime(130.0));
    EXPECT_EQ(timer->nextTime(), 130.0);

    // While scheduled the next time cannot change.
    EXPECT_TRUE(timer->scheduleTimer());
    EXPECT_TRUE(timer->isScheduled());
    EXPECT_TRUE(
        timer->scheduleTimer());  // already scheduled: YES, no second entry
    EXPECT_TRUE(!timer->setNextTime(140.0));
    EXPECT_EQ(timer->nextTime(), 130.0);
    timer->unscheduleTimer();
    EXPECT_TRUE(!timer->isScheduled());
    EXPECT_TRUE(timer->setNextTime(140.0));
  }
}

TEST(OOScriptTimer, description) {
  {
    ::SetUp(100.0);
    std::shared_ptr<OOScriptTimer> timer = OOScriptTimer::timerWithNextTime(110.0, 5.0);
    EXPECT_EQ(timer->descriptionComponents().value_or("<none>"),
                "nextTime: 110, interval: 5, not running");
    timer->scheduleTimer();
    EXPECT_EQ(timer->descriptionComponents().value_or("<none>"),
                "nextTime: 110, interval: 5, running");
    timer->unscheduleTimer();
    timer = OOScriptTimer::oneShotTimerWithDelay(1.5);
    EXPECT_EQ(timer->descriptionComponents().value_or("<none>"),
                "nextTime: 101.5, one-shot, not running");
    EXPECT_TRUE(timer->description().find("OOScriptTimer") != std::string::npos);
  }
}

TEST(OOScriptTimer, ordering) {
  {
    ::SetUp(100.0);
    std::shared_ptr<OOScriptTimer> early = OOScriptTimer::timerWithNextTime(110.0, 5.0);
    std::shared_ptr<OOScriptTimer> late = OOScriptTimer::timerWithNextTime(120.0, 5.0);
    EXPECT_EQ(early->compareByNextFireTime(late.get()), oo::ComparisonResult::kAscending);
    EXPECT_EQ(late->compareByNextFireTime(early.get()), oo::ComparisonResult::kDescending);
    EXPECT_EQ(early->compareByNextFireTime(early.get()), oo::ComparisonResult::kSame);
    EXPECT_EQ(early->compareByNextFireTime(nullptr),
                oo::ComparisonResult::kDescending);  // nil is -INFINITY
  }
}

TEST(OOScriptTimer, firing) {
  {
    ::SetUp(100.0);
    std::shared_ptr<TestTimer> b = MakeTimer("b", 120.0, -1.0);
    std::shared_ptr<TestTimer> a = MakeTimer("a", 110.0, -1.0);
    std::shared_ptr<TestTimer> r = MakeTimer("r", 105.0, 10.0);
    EXPECT_TRUE(b->scheduleTimer() && a->scheduleTimer() && r->scheduleTimer());

    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "");

    gNow = 112.0;
    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "r,a");
    EXPECT_TRUE(!a->isScheduled());  // one-shot: done
    EXPECT_TRUE(r->isScheduled());   // repeating: rescheduled at the next multiple
                                  // of its interval
    EXPECT_EQ(r->nextTime(), 115.0);

    gNow = 130.0;
    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "r,a,r,b");
    EXPECT_EQ(r->nextTime(), 135.0);
    EXPECT_TRUE(!b->isScheduled());

    // A one-shot that has run cannot be scheduled again once its time has
    // passed.
    EXPECT_TRUE(!a->scheduleTimer());
    EXPECT_TRUE(!a->isScheduled());

    r->unscheduleTimer();
    gNow = 200.0;
    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "r,a,r,b");
  }
}

TEST(OOScriptTimer, scheduleMovesPastTimeForward) {
  {
    ::SetUp(100.0);
    std::shared_ptr<TestTimer> r = MakeTimer("r", 101.0, 4.0);
    gNow = 110.0;
    EXPECT_TRUE(r->isValidForScheduling());
    EXPECT_EQ(r->nextTime(), 113.0);  // the next 101 + 4n after now
    gNow = 113.0;
    std::shared_ptr<TestTimer> s = MakeTimer("s", 105.0, 4.0);
    EXPECT_TRUE(s->isValidForScheduling());
    EXPECT_EQ(s->nextTime(),
                113.0);  // exactly now: not run yet, so it may fire now
  }
}

TEST(OOScriptTimer, stopsItselfAndDefers) {
  {
    ::SetUp(100.0);
    std::shared_ptr<TestTimer> stopper = MakeTimer("stopper", 101.0, 5.0);
    stopper->stopWhenFired = true;
    std::shared_ptr<TestTimer> later = MakeTimer("later", 102.0, -1.0);
    std::shared_ptr<TestTimer> starter = MakeTimer("starter", 101.5, -1.0);
    starter->startWhenFired = later.get();
    EXPECT_TRUE(stopper->scheduleTimer() && starter->scheduleTimer());

    gNow = 103.0;
    OOScriptTimer::updateTimers();
    // "later" was due, but it was scheduled during the update, so it waits for
    // the next one.
    EXPECT_EQ(Fired(), "stopper,starter");
    EXPECT_TRUE(!stopper->isScheduled());
    EXPECT_TRUE(later->isScheduled());

    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "stopper,starter,later");
    EXPECT_TRUE(!later->isScheduled());
  }
}

TEST(OOScriptTimer, gameReset) {
  {
    ::SetUp(100.0);
    std::shared_ptr<TestTimer> a = MakeTimer("a", 110.0, 5.0);
    std::shared_ptr<TestTimer> b = MakeTimer("b", 120.0, -1.0);
    EXPECT_TRUE(a->scheduleTimer() && b->scheduleTimer());
    OOScriptTimer::noteGameReset();
    EXPECT_TRUE(!a->isScheduled());
    EXPECT_TRUE(!b->isScheduled());
    gNow = 200.0;
    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "");      // the queue was emptied
    EXPECT_TRUE(a->scheduleTimer());  // and they can be scheduled again
    OOScriptTimer::updateTimers();
    EXPECT_EQ(Fired(), "a");
    a->unscheduleTimer();
  }
}

TEST(OOScriptTimer, baseTimerFired) {
  {
    ::SetUp(100.0);
    std::shared_ptr<OOScriptTimer> timer =
        OOScriptTimer::timerWithNextTime(101.0, -1.0);
    EXPECT_TRUE(timer->scheduleTimer());
    gSubclassResponsibilities = 0;
    gNow = 102.0;
    OOScriptTimer::updateTimers();
    EXPECT_EQ(gSubclassResponsibilities, 1);
    EXPECT_TRUE(!timer->isScheduled());
  }
}

TEST(OOScriptTimer, scheduledTimerIsKept) {
  // The timer subsystem retains a scheduled timer: one the caller has let go of
  // still fires.
  ::SetUp(100.0);
  {
    EXPECT_TRUE(MakeTimer("kept", 101.0, -1.0)->scheduleTimer());
  }
  gNow = 102.0;
  {
    OOScriptTimer::updateTimers();
  }
  EXPECT_EQ(Fired(), "kept");
}
