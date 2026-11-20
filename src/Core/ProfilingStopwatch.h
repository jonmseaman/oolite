/*

OOProfilingStopwatch.h
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

#pragma once

#ifndef OOSTOPWATCH_STANDALONE
#include "Foundation.h"
#include "FunctionAttributes.h"
#include "Types.h"
#endif

#include <memory>

#include "oofnd/Object.hpp"

/*	Platform-specific high-resolution timer:

        OOHighResTimeValue is a time value. This could be a scalar, struct or
   pointer. OOHighResTimeValue OOGetHighResTime(void) returns the current time.
        OODisposeHighResTime() destroys an existing OOHighResTimeValue, if
   necessary. It must do nothing if the value passed is zeroed out.
        OOCopyHighResTime(x) returns a timer value equal to x.
        OOTimeDelta OOHighResTimeDeltaInSeconds(OOHighResTimeValue startTime,
   OOHighResTimeValue endTime) returns the difference between two time values,
   in seconds.
*/

#if OOLITE_WINDOWS

// Windows: if standalone, use timeGetTime...
// DWORD and the timer functions, from their own headers: <windows.h> here would
// bring in the old <winsock.h> ahead of a later <winsock2.h>.
#include <windef.h>

#if OOSTOPWATCH_STANDALONE
// windef.h must come first: timeapi.h's mmsyscom.h uses BYTE, DWORD and
// DECLARE_HANDLE without defining them.
#include <timeapi.h>

#define OO_PROFILING_STOPWATCH_WINDOWS 1

namespace oo {

using HighResTimeValue = DWORD;

}  // namespace oo
   // Rolls over once every 50 days, but we can
   // live with that.

// Note: timeGetTime returns time in milliseconds. This results in lower time
// resolution in Windows, but at this stage I don't think we need to do
// something about it. If we really need microseond precision, we might consider
// an implementation based on the Win32 API QueryPerformanceCounter function -
// Nikos 20100615.
#define OOGetHighResTime timeGetTime
#define OOCopyHighResTime(time) (static_cast<oo::HighResTimeValue>(time))

#else
/*	...otherwise use the performance counter, so the profiler can time short
        functions (timeGetTime() only counts whole milliseconds).
*/
#include <profileapi.h>

#define OO_PROFILING_STOPWATCH_PERFORMANCE_COUNTER 1

namespace oo {

using HighResTimeValue = int64_t;

}  // namespace oo

inline oo::HighResTimeValue OOGetHighResTime(void) {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return counter.QuadPart;
}

#define OOCopyHighResTime(time) (static_cast<oo::HighResTimeValue>(time))
#endif

#else

// Other platforms (presumed unixy): use gettimeofday().

#define OO_PROFILING_STOPWATCH_GETTIMEOFDAY 1
#include <sys/time.h>

namespace oo {

using HighResTimeValue = timeval;

}  // namespace oo

inline oo::HighResTimeValue OOGetHighResTime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv;
}

#define OOCopyHighResTime(time) (static_cast<oo::HighResTimeValue>(time))

#endif

// Neither kind of value owns anything, so there is nothing to free.
namespace oo {

inline void DisposeHighResTime(const HighResTimeValue& /*time*/) {}

TimeDelta HighResTimeDeltaInSeconds(HighResTimeValue startTime,
                                    HighResTimeValue endTime);

class ProfilingStopwatch : public oo::Object {
 public:
  ProfilingStopwatch();
  ~ProfilingStopwatch() override;

  static std::shared_ptr<ProfilingStopwatch>
  stopwatch();  // New stopwatch is initially started.

  void start();
  void stop();
  TimeDelta currentTime();  // Returns stop time - start time if stopped, or
                            // now - start time if running.

  /*	Resets timer to zero, returning the current value. This is drift-free,
     i.e. if it is called twice in a row while running the sum is an accurate
     time since the timer started.
  */
  TimeDelta reset();

 private:
  HighResTimeValue _start = {};
  HighResTimeValue _end = {};
  bool _running = {};
};

}  // namespace oo
