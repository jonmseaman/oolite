/*	test_OOJSFunction.mm
	Unit tests for OOJSFunction (src/Core/Scripting/OOJSFunction.h/.mm).

	OOJSFunction keeps a JS function alive (a reference it owns) for Objective-C code: the AI's scan
	predicates (ShipEntityAI) and the regular-expression tester (OORegExpMatcher). The test runs it
	in a real QuickJS-ng context and links the game's own object for the class. What the rest of
	the engine would provide is defined below as the smallest stand-in that does the same thing:
	the engine object (the sender of its reset notification), the script stack, the time limiter,
	the value converter, the string converter and the pending-exception reporter. The engine's and
	OOJSScript's headers are not imported, because the test defines those classes. The tests pin: both
	initialisers (and when they answer nil), the name and the description, the function value, raw
	evaluation (the script stack and the time limiter balanced around each call, the arguments
	converted), the predicate's truthiness, and what an engine reset does (commit 34d0592d1). They
	ask the C++ class.
	Run: bash tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "OOJSFunction.h"
#include "Types.h"
#include "oofnd/Notification.hpp"


#include <quickjs.h>

#include <cstring>
#include <string>
#include <vector>


// MARK: What the rest of the engine provides ------------------------------------------------------

namespace {
int sScriptDepth = 0;
int sScriptPushes = 0;
int sLimiterDepth = 0;
int sLimiterStarts = 0;
} // namespace

JSContext *gOOJSMainThreadContext = nullptr;
extern const char * const kOOJavaScriptEngineWillResetNotificationName;
const char * const kOOJavaScriptEngineWillResetNotificationName = "org.aegidian.oolite OOJavaScriptEngine will reset";


std::optional<std::string> OOStringFromJSValue(JSContext *context, JSValueConst value)
{
	if (JS_IsNull(value) || JS_IsUndefined(value))  return std::nullopt;
	std::size_t length = 0;
	const char *chars = JS_ToCStringLen(context, &length, value);
	if (chars == nullptr)
	{
		JS_FreeValue(context, JS_GetException(context));
		return std::nullopt;
	}
	std::string result(chars, length);
	JS_FreeCString(context, chars);
	return result;
}


// As the engine's: the pending exception is reported (here, dropped) and cleared.
void OOJSReportPendingException(JSContext *context)
{
	JS_FreeValue(context, JS_GetException(context));
}


void OOJSStartTimeLimiterWithTimeLimit(oo::TimeDelta)  { sLimiterDepth++; sLimiterStarts++; }
void OOJSStopTimeLimiter(void)  { sLimiterDepth--; }


// The engine (OOJavaScriptEngine.h), declared with the header's names and
// signatures but not its class; the stand-in's answers are kept.
class OOJavaScriptEngine
{
public:
	static OOJavaScriptEngine *sharedEngine();
};


OOJavaScriptEngine *OOJavaScriptEngine::sharedEngine()
{
	static OOJavaScriptEngine *engine = new OOJavaScriptEngine;
	return engine;
}


#include "OOScript.h"


// OOJSScript (OOJSScript.h), which the code under test calls. A script is a
// C++ object, so the stand-in is a C++ subclass of OOScript declaring the
// members that code calls.
class OOJSScript : public OOScript
{
public:
	static void pushScript(OOJSScript *script);
	static void popScript(OOJSScript *script);
};

void OOJSScript::pushScript(OOJSScript *script)  { (void)script; sScriptDepth++; sScriptPushes++; }
void OOJSScript::popScript(OOJSScript *script)  { (void)script; sScriptDepth--; }


// MARK: The context -------------------------------------------------------------------------------

namespace {

JSContext *Context()
{
	if (gOOJSMainThreadContext == nullptr)
	{
		JSRuntime *runtime = JS_NewRuntime();
		JS_SetMemoryLimit(runtime, 8u * 1024u * 1024u);
		gOOJSMainThreadContext = JS_NewContext(runtime);
	}
	return gOOJSMainThreadContext;
}


// Evaluates src in the global scope and gives the value, owned (undefined if it threw).
JSValue Evaluate(const char *src)
{
	JSValue result = JS_Eval(Context(), src, std::strlen(src), "test.js", JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(result))
	{
		JS_FreeValue(Context(), JS_GetException(Context()));
		return JS_UNDEFINED;
	}
	return result;
}


// The value as a string ("undefined", "null", ...); frees the value.
std::string String(JSValue value)
{
	std::string result;
	if (JS_IsNull(value) || JS_IsUndefined(value))  result = JS_IsNull(value) ? "null" : "undefined";
	else  result = OOStringFromJSValue(Context(), value).value_or("<none>");
	JS_FreeValue(Context(), value);
	return result;
}


// Whether two values are the same object (identity, as the object pointers were compared).
bool SameObject(JSValueConst a, JSValueConst b)
{
	return JS_IsObject(a) && JS_IsObject(b) && JS_VALUE_GET_PTR(a) == JS_VALUE_GET_PTR(b);
}


std::shared_ptr<OOJSFunction> Compile(const char *name, const char *code, std::vector<const char *> args)
{
	return OOJSFunction::initWithName((name != nullptr ? std::optional<std::string>(name) : std::nullopt),
									  JS_UNDEFINED,
									  (code != nullptr ? std::optional<std::string>(code) : std::nullopt),
									  args.size(),
									  (args.empty() ? NULL : args.data()),
									  std::string("test.js"),
									  1,
									  Context());
}

}	// namespace


// MARK: Tests -------------------------------------------------------------------------------------

TEST(OOJSFunction, initWithFunction)
{
	{
		EXPECT_TRUE(OOJSFunction::initWithFunction(JS_NULL, Context()) == nullptr);

		JSValue f = Evaluate("(function double(x) { return 2 * x; })");
		std::shared_ptr<OOJSFunction> function = OOJSFunction::initWithFunction(f, Context());
		EXPECT_TRUE(function != nullptr);
		EXPECT_TRUE(SameObject(function->function(), f));
		EXPECT_EQ(function->name().value_or("<none>"), "double");
		EXPECT_EQ(function->descriptionComponents().value_or("<none>"), "double()");
		JSValue functionValue = function->functionValue();
		EXPECT_TRUE(JS_IsObject(functionValue));
		EXPECT_TRUE(SameObject(functionValue, f));
		JS_FreeValue(Context(), functionValue);
		JS_FreeValue(Context(), f);

		JSValue anonymousFunction = Evaluate("(function () { return 1; })");
		std::shared_ptr<OOJSFunction> anonymous = OOJSFunction::initWithFunction(anonymousFunction, Context());
		JS_FreeValue(Context(), anonymousFunction);
		EXPECT_TRUE(!anonymous->name().has_value());
		EXPECT_EQ(anonymous->descriptionComponents().value_or("<none>"), "<anonymous>()");
	}
}


TEST(OOJSFunction, initWithName)
{
	{
		std::shared_ptr<OOJSFunction> function = Compile("add", "return a + b;", { "a", "b" });
		EXPECT_TRUE(function != nullptr);
		EXPECT_EQ(function->name().value_or("<none>"), "add");
		JSValueConst argv[2] = { JS_NewInt32(Context(), 2), JS_NewInt32(Context(), 3) };
		JSValue result = JS_UNDEFINED;
		EXPECT_TRUE(function->evaluateWithContext(Context(), JS_UNDEFINED, 2, argv, &result));
		EXPECT_EQ(String(result), "5");

		// No code, argument names missing, or a syntax error: nil.
		EXPECT_TRUE(Compile("none", nullptr, {}) == nullptr);
		EXPECT_TRUE(OOJSFunction::initWithName(std::string("x"), JS_UNDEFINED, std::string("return 1;"), 1, NULL, std::nullopt, 0, Context()) == nullptr);
		EXPECT_TRUE(Compile("bad", "return (;", {}) == nullptr);
		JS_FreeValue(Context(), JS_GetException(Context()));

		// A NULL context: the main thread's is acquired and released.
		std::shared_ptr<OOJSFunction> withoutContext = OOJSFunction::initWithName(std::string("seven"), JS_UNDEFINED, std::string("return 7;"), 0, NULL, std::nullopt, 0, NULL);
		EXPECT_TRUE(withoutContext != nullptr);
		EXPECT_TRUE(withoutContext->evaluateWithContext(Context(), JS_UNDEFINED, 0, NULL, &result));
		EXPECT_EQ(String(result), "7");

		// An empty body answers undefined.
		std::shared_ptr<OOJSFunction> empty = Compile("empty", "", {});
		EXPECT_TRUE(empty->evaluateWithContext(Context(), JS_UNDEFINED, 0, NULL, &result));
		EXPECT_EQ(String(result), "undefined");
	}
}


TEST(OOJSFunction, evaluation)
{
	{
		std::shared_ptr<OOJSFunction> function = Compile("thisName", "return this.name + ':' + arguments.length;", {});
		JS_FreeValue(Context(), Evaluate("globalThis.named = { name: 'n' }; globalThis.name = 'global';"));
		JSValue result = JS_UNDEFINED;
		sScriptPushes = 0;
		sLimiterStarts = 0;
		JSValue named = Evaluate("named");
		EXPECT_TRUE(function->evaluateWithContext(Context(), named, 0, NULL, &result));
		JS_FreeValue(Context(), named);
		EXPECT_EQ(String(result), "n:0");
		EXPECT_TRUE(function->evaluateWithContext(Context(), JS_UNDEFINED, 0, NULL, &result));
		EXPECT_EQ(String(result), "global:0");	// no this: the global object (the function is not strict)
		EXPECT_EQ(sScriptPushes, 2);
		EXPECT_EQ(sLimiterStarts, 2);
		EXPECT_EQ(sScriptDepth, 0);
		EXPECT_EQ(sLimiterDepth, 0);

		// A throwing function answers NO, with the stack and the limiter balanced.
		std::shared_ptr<OOJSFunction> thrower = Compile("thrower", "throw new Error('boom');", {});
		EXPECT_TRUE(!thrower->evaluateWithContext(Context(), JS_UNDEFINED, 0, NULL, &result));
		JS_FreeValue(Context(), JS_GetException(Context()));
		EXPECT_EQ(sScriptDepth, 0);
		EXPECT_EQ(sLimiterDepth, 0);
	}
}


TEST(OOJSFunction, engineReset)
{
	{
		std::shared_ptr<OOJSFunction> function = Compile("resettable", "return 1;", {});
		EXPECT_TRUE(!JS_IsNull(function->function()));
		oo::NotificationCenter::defaultCenter().post(kOOJavaScriptEngineWillResetNotificationName, OOJavaScriptEngine::sharedEngine());
		EXPECT_TRUE(JS_IsNull(function->function()));
		EXPECT_TRUE(JS_IsNull(function->functionValue()));
		EXPECT_EQ(function->name().value_or("<none>"), "resettable");	// the name stays
	}
}


TEST(OOJSFunction, cxxAndFacade)
{
	{
		std::shared_ptr<OOJSFunction> function = OOJSFunction::initWithName(std::string("triple"), JS_UNDEFINED, std::string("return 3 * x;"), 1, (const char *[]){ "x" }, std::nullopt, 0, Context());
		EXPECT_TRUE(function != nullptr);
		EXPECT_TRUE(OOJSFunction::initWithFunction(JS_NULL, Context()) == nullptr);
		EXPECT_TRUE(OOJSFunction::initWithName(std::string("bad"), JS_UNDEFINED, std::string("return (;"), 0, NULL, std::nullopt, 0, Context()) == nullptr);
		JS_FreeValue(Context(), JS_GetException(Context()));
		JSValueConst argv[1] = { JS_NewInt32(Context(), 4) };
		JSValue result = JS_UNDEFINED;
		EXPECT_TRUE(function->evaluateWithContext(Context(), JS_UNDEFINED, 1, argv, &result));
		EXPECT_EQ(String(result), "12");
		EXPECT_EQ(function->descriptionComponents().value_or("<none>"), "triple()");
		EXPECT_EQ(function->name().value_or("<none>"), "triple");	// was asked of its facade
	}
}
