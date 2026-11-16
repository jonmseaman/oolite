/*

OOJSFrameCallbacks.h

Support for JavaScript callbacks to be invoked on every frame.


Copyright (C) 2011-2013 Jens Ayton

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

#include <cstdint>
#include <optional>

#include "OOJavaScriptEngine.h"

void InitOOJSFrameCallbacks(JSContext* context, JSValueConst global);

// The first frame-callback id comes from the clock unless a seed is set here
// (a headless game sets its seed before the engine starts), so ids repeat.
void OOJSFrameCallbacksSetIDSeed(std::optional<std::uint32_t> seed);

void OOJSFrameCallbacksInvoke(oo::TimeDelta delta);

void OOJSFrameCallbacksRemoveAll();
