/*	test_OORegExpMatcher.mm
        Unit tests for OORegExpMatcher (src/Core/OORegExpMatcher.h).

        The matcher compiles a JavaScript function, "return
   regexp.test(string);", and runs it on a RegExp object it caches by pattern
   and flags. The test links the game's own OORegExpMatcher and the QuickJS-ng
   engine, so every match below is the real engine's answer. The glue the
   matcher uses but whose files reach the whole game (the engine singleton,
   OOJSFunction, OOJSValue, the two value converters and the exception reporter;
   OOJavaScriptEngine.mm links everything) is replaced by the minimal
   definitions below (OOJSFunction's C++ members), which count what the matcher
   asks of them. What matches, the flags, the empty pattern, the cache (one
   compiled RegExp per pattern and flags), and the pseudo-singleton (one matcher
   per autorelease pool, a new one after the pool drains; nil, and asked again,
   when the tester does not compile). The cases ask the C++ matcher: see the
   note above the tests. Run: tests/run.sh unit
*/

#include <quickjs.h>

#include <string>

#include <gtest/gtest.h>

#include "OOJSEngineCore.h"  // OOJSValue
#include "OOJSFunction.h"
#include "RegExpMatcher.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PList.hpp"

// --- Stand-ins for the engine glue (see the banner)
// ---------------------------------------------

JSContext* gOOJSMainThreadContext = nullptr;

namespace {

JSValue sGlobal = JS_UNDEFINED;  // owned, for the life of the process
unsigned sEngineCalls = 0;       // +OOJavaScriptEngine::sharedEngine()
unsigned sFunctionsMade = 0;     // OOJSFunction inits: one per matcher
unsigned sRegExpsMade =
    0;  // OOJSValue inits: one per RegExp the matcher compiled
unsigned sEvaluations = 0;  // OOJSFunction evaluations: one per match
bool sFailCompile =
    false;  // make the next OOJSFunction init fail, as a compile error does

}  // namespace

// The engine (OOJavaScriptEngine.h; a C++ class), declared with the header's
// names and signatures but not its class; the stand-in's answers are kept.
class OOJavaScriptEngine {
 public:
  static OOJavaScriptEngine* sharedEngine();
};

OOJavaScriptEngine* OOJavaScriptEngine::sharedEngine() {
  sEngineCalls++;
  if (gOOJSMainThreadContext == nullptr) {
    JSRuntime* runtime = JS_NewRuntime();
    JS_SetMemoryLimit(runtime, 64L * 1024L * 1024L);
    gOOJSMainThreadContext = JS_NewContext(runtime);
    sGlobal = JS_GetGlobalObject(gOOJSMainThreadContext);
  }
  return nullptr;
}

// OOJSFunction (src/Core/Scripting/OOJSFunction.h): the members the matcher
// calls, defined here in place of OOJSFunction.mm. The function is compiled
// as a function expression in the global scope, as OOJSFunction.mm does.
std::shared_ptr<OOJSFunction> OOJSFunction::initWithName(
    const std::optional<std::string>& name, JSValueConst scope,
    const std::optional<std::string>& code, std::size_t argCount,
    const char** argNames, const std::optional<std::string>& fileName,
    std::size_t lineNumber, JSContext* context) {
  (void)scope;
  (void)lineNumber;
  sFunctionsMade++;
  if (sFailCompile) return nullptr;
  std::string source = "(function " + name.value_or(std::string()) + "(";
  for (std::size_t i = 0; i < argCount; i++)
    source += std::string(i != 0 ? "," : "") + argNames[i];
  source += ") {\n" + code.value_or(std::string()) + "\n})";
  JSValue function =
      JS_Eval(context, source.c_str(), source.size(),
              fileName ? fileName->c_str() : "<test>", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(function)) {
    JS_FreeValue(context, JS_GetException(context));
    return nullptr;
  }
  std::shared_ptr<OOJSFunction> result = std::shared_ptr<OOJSFunction>(new OOJSFunction(function, context), oo::Dispose{});
  JS_FreeValue(context, function);
  return result;
}

OOJSFunction::OOJSFunction(JSValueConst function, JSContext* context) {
  _function = JS_DupValue(context, function);  // its own reference
}

OOJSFunction::~OOJSFunction() {
  JS_FreeValue(gOOJSMainThreadContext, _function);
}

bool OOJSFunction::evaluateWithContext(JSContext* context, JSValueConst jsThis,
                                       int argc, JSValueConst* argv,
                                       JSValue* result) {
  sEvaluations++;
  JSValue rval = JS_Call(context, _function,
                         JS_IsObject(jsThis) ? jsThis : sGlobal, argc, argv);
  if (JS_IsException(rval)) {
    *result = JS_UNDEFINED;
    return false;  // the exception is left pending
  }
  *result = rval;
  return true;
}

// The held value (OOJSEngineCore.h; a C++ class): the members the matcher
// calls, defined here with the stand-in's answers.
std::shared_ptr<OOJSValue> OOJSValue::valueWithJSValue(JSValueConst value,
                                               JSContext* context) {
  std::shared_ptr<OOJSValue> result = oo::makeShared<OOJSValue>();
  sRegExpsMade++;
  result->_val = JS_DupValue(context, value);  // owned
  return result;
}

OOJSValue::~OOJSValue() { JS_FreeValue(gOOJSMainThreadContext, _val); }

// Owned, as the engine's.
JSValue OOJSValue::jsValueInContext(JSContext* context) {
  return JS_DupValue(context, _val);
}
std::string OOJSValue::className() const { return "OOJSValue"; }
std::string OOJSValue::description() const { return "<OOJSValue>"; }

// Owned, as the engine's.
JSValue OOJSValueFromPList(JSContext* context, const oo::PList& plist) {
  const std::string* string = plist.getIf<std::string>();
  if (string == nullptr) return JS_NULL;
  return JS_NewStringLen(context, string->data(), string->size());
}

// The engine's reporter: the stand-in clears the exception.
void OOJSReportPendingException(JSContext* context) {
  JS_FreeValue(context, JS_GetException(context));
}

// --- The tests
// ------------------------------------------------------------------------------------
// The cases ask the C++ matcher. The C++ lifetime is
// cxxOneMatcherWhileHeld's; matchersHeldAcrossMatches pins what the pool gave a
// caller that holds its matcher.

namespace {

bool Matches(const std::string& string, const std::string& regExp,
             oo::RegExpFlags flags = {}) {
  std::shared_ptr<oo::RegExpMatcher> matcher = oo::RegExpMatcher::regExpMatcher();
  return matcher != nullptr && matcher->string(string, regExp, flags);
}

}  // namespace

TEST(OORegExpMatcher, matches) {
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(Matches("GeForce GTX 1080", "GeForce"));
    EXPECT_TRUE(Matches("GeForce GTX 1080", "^GeForce GTX [0-9]+$"));
    EXPECT_TRUE(!Matches("GeForce GTX 1080", "^GTX"));
    EXPECT_TRUE(!Matches("geforce", "GeForce"));
    EXPECT_TRUE(Matches("geforce", "GeForce", oo::RegExpFlags::kCaseInsensitive));
    EXPECT_TRUE(!Matches("one\ntwo", "^two"));
    EXPECT_TRUE(Matches("one\ntwo", "^two", oo::RegExpFlags::kMultiLine));
    EXPECT_TRUE(Matches("", "^$"));
    EXPECT_TRUE(Matches("caf\xC3\xA9",
                     "\xC3\xA9$"));  // UTF-8 in, and UTF-8 to the engine
    EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher()->string("abc", "b"));  // flags 0
  }
}

TEST(OORegExpMatcher, emptyPatternMatchesNothing) {
  unsigned evaluations = sEvaluations;
  EXPECT_TRUE(!Matches("abc", ""));
  EXPECT_TRUE(!Matches("", ""));
  EXPECT_TRUE(sEvaluations == evaluations);  // returned before running anything
}

TEST(OORegExpMatcher, regExpIsCachedByPatternAndFlags) {
  std::shared_ptr<oo::RegExpMatcher> matcher = oo::RegExpMatcher::regExpMatcher();
  unsigned made = sRegExpsMade;
  EXPECT_TRUE(matcher->string("abc", "c$", oo::RegExpFlags::kNone));
  EXPECT_TRUE(sRegExpsMade == made + 1);
  EXPECT_TRUE(!matcher->string("abd", "c$", oo::RegExpFlags::kNone));
  EXPECT_TRUE(sRegExpsMade == made + 1);  // same pattern and flags: cached
  EXPECT_TRUE(matcher->string("ABC", "c$", oo::RegExpFlags::kCaseInsensitive));
  EXPECT_TRUE(sRegExpsMade == made + 2);  // new flags
  EXPECT_TRUE(matcher->string("ABD", "d$", oo::RegExpFlags::kCaseInsensitive));
  EXPECT_TRUE(sRegExpsMade == made + 3);  // new pattern
  EXPECT_TRUE(matcher->string("abd", "d$", oo::RegExpFlags::kNone));
  EXPECT_TRUE(sRegExpsMade == made + 4);
}

TEST(OORegExpMatcher, matchersHeldAcrossMatches) {
  unsigned made = sFunctionsMade;
  unsigned engineCalls = sEngineCalls;
  {
    std::shared_ptr<oo::RegExpMatcher> first = oo::RegExpMatcher::regExpMatcher();
    EXPECT_TRUE(first != nullptr);
    EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() == first);
    EXPECT_TRUE(Matches("x", "x"));
    EXPECT_TRUE(sFunctionsMade ==
             made + 1);  // one tester compiled for the held matcher
    EXPECT_TRUE(sEngineCalls ==
             engineCalls + 1);  // the engine summoned once, by init()
  }
  {
    EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() != nullptr);
    EXPECT_TRUE(sFunctionsMade ==
             made + 2);  // the first matcher went with its last Ref
  }
}

TEST(OORegExpMatcher, failedInitGivesNilAndRetries) {
  sFailCompile = true;
  EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() == nullptr);
  sFailCompile = false;
  EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() !=
           nullptr);  // nothing was kept: asked again
  EXPECT_TRUE(Matches("abc", "b"));
}

TEST(OORegExpMatcher, cxxMatches) {
  std::shared_ptr<oo::RegExpMatcher> matcher = oo::RegExpMatcher::regExpMatcher();
  EXPECT_TRUE(matcher != nullptr);
  EXPECT_TRUE(matcher->string("GeForce GTX 1080", "^GeForce GTX [0-9]+$"));
  EXPECT_TRUE(!matcher->string("geforce", "GeForce"));
  EXPECT_TRUE(matcher->string("geforce", "GeForce", oo::RegExpFlags::kCaseInsensitive));
  EXPECT_TRUE(matcher->string("one\ntwo", "^two", oo::RegExpFlags::kMultiLine));
  EXPECT_TRUE(!matcher->string("abc", ""));
}

TEST(OORegExpMatcher, cxxOneMatcherWhileHeld) {
  unsigned made = sFunctionsMade;
  {
    std::shared_ptr<oo::RegExpMatcher> first = oo::RegExpMatcher::regExpMatcher();
    EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() == first);
    EXPECT_TRUE(sFunctionsMade == made + 1);
  }
  EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() !=
           nullptr);  // the first was freed with its last Ref
  EXPECT_TRUE(sFunctionsMade == made + 2);

  sFailCompile = true;
  EXPECT_TRUE(oo::RegExpMatcher::regExpMatcher() == nullptr);
  sFailCompile = false;
}
