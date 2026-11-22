/*	test_OOProfilingStopwatch.mm
        Unit tests for OOProfilingStopwatch (src/Core/OOProfilingStopwatch.h):.

        Pins: a new stopwatch is running; while running, currentTime() is now -
   start and grows; stop() freezes it; reset() returns the current value and
   restarts from zero (drift-free while running); start() restarts a stopped
   watch; and OOHighResTimeDeltaInSeconds() converts this build's microsecond
   readings. Timings sleep and compare against generous bounds, so a loaded
   machine cannot fail them. Run: tests/run.sh unit
*/

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "ProfilingStopwatch.h"

namespace {

void Sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

}  // namespace

TEST(OOProfilingStopwatch, deltaInSeconds) {
  oo::HighResTimeValue start = OOGetHighResTime();
  oo::HighResTimeValue end = OOCopyHighResTime(start);
  EXPECT_EQ(oo::HighResTimeDeltaInSeconds(start, end), 0.0);
  Sleep(20);
  end = OOGetHighResTime();
  oo::TimeDelta delta = oo::HighResTimeDeltaInSeconds(start, end);
  EXPECT_TRUE(delta >= 0.015 && delta < 5.0);
  EXPECT_TRUE(oo::HighResTimeDeltaInSeconds(end, start) == -delta);
#if OO_PROFILING_STOPWATCH_MICROSECONDS
  EXPECT_EQ(oo::HighResTimeDeltaInSeconds(0, 1500000), 1.5);  // microseconds
#endif
}

TEST(OOProfilingStopwatch, newStopwatchIsRunning) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  EXPECT_TRUE(watch != nullptr);
  oo::TimeDelta first = watch->currentTime();
  EXPECT_TRUE(first >= 0.0 && first < 5.0);
  Sleep(20);
  oo::TimeDelta second = watch->currentTime();
  EXPECT_TRUE(second >= first + 0.015);
}

TEST(OOProfilingStopwatch, stopFreezesTheTime) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::makeShared<oo::ProfilingStopwatch>();
  Sleep(20);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  EXPECT_TRUE(stopped >= 0.015 && stopped < 5.0);
  Sleep(20);
  EXPECT_EQ(watch->currentTime(), stopped);
}

TEST(OOProfilingStopwatch, resetWhileStopped) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(20);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  EXPECT_EQ(watch->reset(), stopped);    // the value it had
  EXPECT_EQ(watch->currentTime(), 0.0);  // and zero after
  Sleep(10);
  EXPECT_EQ(watch->currentTime(), 0.0);  // still stopped
}

TEST(OOProfilingStopwatch, resetWhileRunningIsDriftFree) {
  oo::HighResTimeValue before = OOGetHighResTime();
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(20);
  oo::TimeDelta firstLap = watch->reset();
  EXPECT_TRUE(firstLap >= 0.015 && firstLap < 5.0);
  Sleep(20);
  oo::TimeDelta secondLap = watch->reset();
  EXPECT_TRUE(secondLap >= 0.015 && secondLap < 5.0);
  oo::HighResTimeValue after = OOGetHighResTime();
  // The laps add up to no more than the time around both (the restart is the
  // reading).
  EXPECT_TRUE(firstLap + secondLap <=
           oo::HighResTimeDeltaInSeconds(before, after) + 1e-6);
  EXPECT_TRUE(watch->currentTime() <
           secondLap);  // running again from the second reset
}

TEST(OOProfilingStopwatch, startRestartsAStoppedWatch) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(30);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  watch->start();
  oo::TimeDelta restarted = watch->currentTime();
  EXPECT_TRUE(restarted < stopped);  // measured from the new start
  Sleep(20);
  EXPECT_TRUE(watch->currentTime() >= restarted + 0.015);  // and running
}
