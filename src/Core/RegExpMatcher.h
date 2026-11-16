/*

OORegExpMatcher.h

Regular expression utility built on top of JavaScript regexp objects in lieu
of native regexp support. Not thread-safe.

If we had a performance-critical need for regexps, I'd want a real library,
but this will do for light usage.

A caller that wants one matcher across several matches holds the Ref.

Copyright (C) 2010-2013 Jens Ayton

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

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include "EnumFlags.h"
#include "Foundation.h"
#include "OOJSEngineCore.h"  // OOJSValue, which the matcher holds
#include "OOJSFunction.h"
#include "oofnd/Object.hpp"

// Same values as SpiderMonkey's JSREG_FOLD and JSREG_MULTILINE.
namespace oo {

enum class RegExpFlags : std::size_t {
  kNone = 0,
  kCaseInsensitive = 0x01,
  kMultiLine = 0x04
};

constexpr std::true_type OOEnableFlagOperators(RegExpFlags);

class RegExpMatcher : public oo::Object {
 public:
  /*	Pseudo-singleton: a single instance exists at a given time, but can be
     released. While one is alive (held by a Ref) this returns it; otherwise a
     new one. Null if the tester function could not be compiled.
  */
  static std::shared_ptr<RegExpMatcher> regExpMatcher();

  // Strings are UTF-8.
  bool string(const std::string& string, const std::string& regExp);
  bool string(const std::string& string, const std::string& regExp,
              RegExpFlags flags);

  ~RegExpMatcher() override;

 private:
  // The old -init, which could fail: regExpMatcher()
  // calls it right after making the object, and drops the object when it
  // returns false.
  bool init();

  std::shared_ptr<OOJSFunction> _tester = {};
  std::optional<std::string> _cachedRegExpString =
      {};  // UTF-8; nullopt: nothing cached
  std::shared_ptr<OOJSValue> _cachedRegExpObject = {};
  RegExpFlags _cachedFlags = {};
};

}  // namespace oo
