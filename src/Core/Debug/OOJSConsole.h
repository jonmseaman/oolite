/*

OOJSConsole.h

JavaScript object representing the JavaScript console.


Oolite Debug Support

Copyright (C) 2007-2013 Jens Ayton

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

#include "OOFoundation.h"
class OODebugMonitor;

#include <quickjs.h>

#include <string>

// Create the JS console object for the debug monitor. The caller owns the
// result.
JSValue DebugMonitorToJSConsole(JSContext* context, OODebugMonitor* monitor);

// console.__displayProjection() (spec 008, contracts/settings.md "Debug read"):
// the game installs a function that describes the projection in use as a JSON
// object; the console parses and returns it. Kept as a hook so this file does
// not depend on Universe or the view.
using OODisplayProjectionProvider = std::string (*)();
void OOJSConsoleSetDisplayProjectionProvider(
    OODisplayProjectionProvider provider);
// Spec 008: sets one display preference and applies it at once, as the Screen
// Matching screen does, so a scenario can change a setting mid-run.
using OODisplayPreferenceSetter = void (*)(const std::string& key,
                                           const std::string& value);
void OOJSConsoleSetDisplayPreferenceSetter(OODisplayPreferenceSetter setter);
