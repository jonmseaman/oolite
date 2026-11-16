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

#include "ProfilingStopwatch.h"
#include "oo_test.hpp"

namespace {

void Sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

}  // namespace

OO_TEST(deltaInSeconds) {
  oo::HighResTimeValue start = OOGetHighResTime();
  oo::HighResTimeValue end = OOCopyHighResTime(start);
  OO_CHECK_EQ(oo::HighResTimeDeltaInSeconds(start, end), 0.0);
  Sleep(20);
  end = OOGetHighResTime();
  oo::TimeDelta delta = oo::HighResTimeDeltaInSeconds(start, end);
  OO_CHECK(delta >= 0.015 && delta < 5.0);
  OO_CHECK(oo::HighResTimeDeltaInSeconds(end, start) == -delta);
#if OO_PROFILING_STOPWATCH_MICROSECONDS
  OO_CHECK_EQ(oo::HighResTimeDeltaInSeconds(0, 1500000), 1.5);  // microseconds
#endif
}

OO_TEST(newStopwatchIsRunning) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  OO_CHECK(watch != nullptr);
  oo::TimeDelta first = watch->currentTime();
  OO_CHECK(first >= 0.0 && first < 5.0);
  Sleep(20);
  oo::TimeDelta second = watch->currentTime();
  OO_CHECK(second >= first + 0.015);
}

OO_TEST(stopFreezesTheTime) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::makeShared<oo::ProfilingStopwatch>();
  Sleep(20);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  OO_CHECK(stopped >= 0.015 && stopped < 5.0);
  Sleep(20);
  OO_CHECK_EQ(watch->currentTime(), stopped);
}

OO_TEST(resetWhileStopped) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(20);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  OO_CHECK_EQ(watch->reset(), stopped);    // the value it had
  OO_CHECK_EQ(watch->currentTime(), 0.0);  // and zero after
  Sleep(10);
  OO_CHECK_EQ(watch->currentTime(), 0.0);  // still stopped
}

OO_TEST(resetWhileRunningIsDriftFree) {
  oo::HighResTimeValue before = OOGetHighResTime();
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(20);
  oo::TimeDelta firstLap = watch->reset();
  OO_CHECK(firstLap >= 0.015 && firstLap < 5.0);
  Sleep(20);
  oo::TimeDelta secondLap = watch->reset();
  OO_CHECK(secondLap >= 0.015 && secondLap < 5.0);
  oo::HighResTimeValue after = OOGetHighResTime();
  // The laps add up to no more than the time around both (the restart is the
  // reading).
  OO_CHECK(firstLap + secondLap <=
           oo::HighResTimeDeltaInSeconds(before, after) + 1e-6);
  OO_CHECK(watch->currentTime() <
           secondLap);  // running again from the second reset
}

OO_TEST(startRestartsAStoppedWatch) {
  std::shared_ptr<oo::ProfilingStopwatch> watch = oo::ProfilingStopwatch::stopwatch();
  Sleep(30);
  watch->stop();
  oo::TimeDelta stopped = watch->currentTime();
  watch->start();
  oo::TimeDelta restarted = watch->currentTime();
  OO_CHECK(restarted < stopped);  // measured from the new start
  Sleep(20);
  OO_CHECK(watch->currentTime() >= restarted + 0.015);  // and running
}

OO_TEST_MAIN()
