/*

OOProfilingStopwatch.m
Oolite

Testing utility to monitor elapsed times at high precision.


Copyright (C) 2010-2013 Jens Ayton and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "ProfilingStopwatch.h"

#include <memory>

oo::ProfilingStopwatch::ProfilingStopwatch() {
  _start = OOGetHighResTime();
  _end = OOCopyHighResTime(_start);
  _running = true;
}

std::shared_ptr<oo::ProfilingStopwatch> oo::ProfilingStopwatch::stopwatch() {
  return oo::makeShared<oo::ProfilingStopwatch>();
}

namespace oo {

ProfilingStopwatch::~ProfilingStopwatch() {
  DisposeHighResTime(_start);
  DisposeHighResTime(_end);
}

}  // namespace oo

void oo::ProfilingStopwatch::start() {
  oo::HighResTimeValue temp = _start;
  _start = OOGetHighResTime();
  oo::DisposeHighResTime(temp);
  _running = true;
}

void oo::ProfilingStopwatch::stop() {
  oo::HighResTimeValue temp = _start;
  _end = OOGetHighResTime();
  oo::DisposeHighResTime(temp);
  _running = false;
}

oo::TimeDelta oo::ProfilingStopwatch::currentTime() {
  if (_running) {
    oo::HighResTimeValue temp = _end;
    _end = OOGetHighResTime();
    oo::DisposeHighResTime(temp);
  }
  return oo::HighResTimeDeltaInSeconds(_start, _end);
}

oo::TimeDelta oo::ProfilingStopwatch::reset() {
  oo::TimeDelta result;
  if (_running) {
    oo::HighResTimeValue now = OOGetHighResTime();
    result = oo::HighResTimeDeltaInSeconds(_start, now);
    oo::DisposeHighResTime(_start);
    _start = now;
  } else {
    result = oo::HighResTimeDeltaInSeconds(_start, _end);
    oo::DisposeHighResTime(_end);
    _end = OOCopyHighResTime(_start);
  }
  return result;
}

namespace oo {

TimeDelta HighResTimeDeltaInSeconds(HighResTimeValue startTime,
                                    HighResTimeValue endTime) {
#if OO_PROFILING_STOPWATCH_MACH_ABSOLUTE_TIME
  uint64_t diff = endTime - startTime;
  static double conversion = 0.0;

  if (conversion == 0.0) [[unlikely]] {
    mach_timebase_info_data_t info;
    kern_return_t err = mach_timebase_info(&info);

    if (err == 0) {
      conversion = 1e-9 * static_cast<double>(info.numer) /
                   static_cast<double>(info.denom);
    }
  }

  return conversion * static_cast<double>(diff);
#elif OO_PROFILING_STOPWATCH_WINDOWS
  // Signed, so a reversed pair gives a negative delta (and a rollover between
  // the two still gives the right answer); the unsigned DWORD difference
  // wrapped to about 49.7 days.
  return 1e-3 * static_cast<double>(static_cast<int32_t>(endTime - startTime));
#elif OO_PROFILING_STOPWATCH_PERFORMANCE_COUNTER
  static double conversion = 0.0;

  if (conversion == 0.0) [[unlikely]] {
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency) && frequency.QuadPart != 0) {
      conversion = 1.0 / static_cast<double>(frequency.QuadPart);
    }
  }

  return conversion * static_cast<double>(endTime - startTime);
#elif OO_PROFILING_STOPWATCH_GETTIMEOFDAY
  int32_t deltaS = static_cast<int32_t>(endTime.tv_sec) -
                   static_cast<int32_t>(startTime.tv_sec);
  int32_t deltaU = static_cast<int32_t>(endTime.tv_usec) -
                   static_cast<int32_t>(startTime.tv_usec);
  double result = deltaU;
  result = (result * 1e-6) + deltaS;
  return result;
#endif
}

}  // namespace oo
