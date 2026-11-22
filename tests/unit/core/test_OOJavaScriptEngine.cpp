/*	test_OOJavaScriptEngine.mm
	Unit tests for the JavaScript engine (src/Core/Scripting/OOJavaScriptEngine.h/.mm).

	The engine is the singleton that owns the JS runtime and the main thread context: it makes the
	global object and every Oolite class on it, looks up the standard classes, calls JS functions,
	keeps the error-reporting flags, reports errors and warnings (to the log and to a debug
	monitor), describes values and dumps the stack. The same file holds the JS glue of OOObject,
	OONativeVector and OONull, OOJSValue, the generic object wrapper, the entity predicates and the
	JS -> property list converters. It needs the real game, so the test links every game object but
	main's (tests/unit/core/meson.build entry ['*'], as test_OOJSScript does) and points the user's
	directories at a scratch folder. The expectations pin what the scripts and the log see. Values
	follow the engine's ownership conventions (OOJSEngineCore.h): every owned value the test makes
	is freed. Run: meson test -C build/meson_test test_OOJavaScriptEngine
*/

#include <gtest/gtest.h>

#include "OOJavaScriptEngine.h"
#include "OOJSPropID.h"
#include "Entity.h"
#include "Vector.h"
#include "oofnd/PList.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "OOJSPrivateObject.h"
#include "oofnd/FileSystem.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/Notification.hpp"

#include <quickjs.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <process.h>
#include <string>
#include <vector>


extern JSContext *gOOJSMainThreadContext;	// OOJavaScriptEngine.mm


// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif


// (cxxObjectWrapper counts TestCxxNative's finalisation.)

// A C++ native object, as a JS object's private slot holds: counts its JS
// wrapper's finalisation and its own, and is a plist Object node's foreign
// object.
static int sCxxNativeDeallocs = 0;

class TestCxxNative : public oo::PListForeign, public OOJSPrivateObject
{
public:
	~TestCxxNative() override  { sCxxNativeDeallocs++; }
	std::string className() const override  { return "TestCxxNative"; }
	std::string description() const override  { return "<TestCxxNative>"; }
	JSValue jsValueInContext(JSContext *) override  { return JS_UNDEFINED; }
	void clearJSSelf(JSValueConst) override  { _clears++; }
	std::optional<std::string> jsDescription() override  { return std::string("[TestCxxNative x=1]"); }

	int		_clears = 0;
};


// An entity whose script-visible answers the test chooses (the predicates ask
// only these). It overrides the virtual members; its -isShip, -isSubEntity and
// -status answers are the C++ entity's own flags and status, which the
// predicates read for a C++ entity, set where the test set its ivars.
class TestScriptEntity : public Entity
{
public:
	bool isVisibleToScripts() override  { return _scriptVisible; }
	bool isPlanet() override  { return false; }
	JSValue jsValueInContext(JSContext *context) override  { return JS_NewInt32(context, 42); }

	bool _scriptVisible = false;
};


#if OOJSENGINE_MONITOR_SUPPORT
// A debug monitor that records what the engine sends it: a C++
// OOJavaScriptEngineMonitor.
namespace {

// The records.
struct TestMonitorRecord
{
	int							_errors = 0;
	int							_logs = 0;
	OOJavaScriptEngine			*_engine = nullptr;
	bool						_showLocation = false;
	std::string					_message;
	std::optional<std::string>	_messageClass;
};


class TestMonitor : public OOJavaScriptEngineMonitor, public TestMonitorRecord
{
public:
	void jsEngine(OOJavaScriptEngine *engine,
				  JSContext * /*context*/,
				  const std::string & /*severity*/,
				  const std::optional<std::string> & /*file*/,
				  unsigned /*line*/,
				  unsigned /*stackSkip*/,
				  bool showLocation,
				  const std::string &message) override
	{
		_errors++;
		_engine = engine;
		_showLocation = showLocation;
		_message = message;
	}


	void jsEngine(OOJavaScriptEngine *engine,
				  JSContext * /*context*/,
				  const std::string &message,
				  const std::optional<std::string> &messageClass) override
	{
		_logs++;
		_engine = engine;
		_message = message;
		_messageClass = messageClass;
	}
};

}	// namespace
#endif


namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;

std::vector<std::string> sLog;


void Capture(std::string_view line)
{
	sLog.emplace_back(line);
}


void WriteText(const stdfs::path &path, const std::string &text)
{
	stdfs::create_directories(path.parent_path());
	EXPECT_TRUE(oo::fs::writeFile(path, oo::Data(text.data(), text.size()), oo::fs::WriteMode::kDirect).has_value());
}


// The scratch home and game folder, made once, before the cache manager and the engine exist.
void SetUp()
{
	if (!sRoot.empty())  return;
	sRoot = stdfs::temp_directory_path() / ("oo-test-jsengine-" + std::to_string(static_cast<unsigned long>(::_getpid())));
	stdfs::remove_all(sRoot);
	stdfs::create_directories(sRoot);
	EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
	stdfs::current_path(sRoot);
	WriteText(sRoot / "Resources" / "Info-gnustep.plist", "{ CFBundleVersion = \"9.9.9-test\"; }");
	(void)OOJavaScriptEngine::sharedEngine();
}


void StartLog()
{
	oo::log::logger().setInitialized(true);
	oo::log::logger().setSink(&Capture);
	oo::log::logger().setDisplay("script.javaScript", true);
	oo::log::logger().setDisplay("script.javaScript.debugger", true);
	oo::log::logger().setDisplay("script.javaScript.stackTrace", true);
	oo::log::logger().setDisplay("script.javaScript.exception", true);
	oo::log::logger().setDisplay("script.javaScript.error", true);
	oo::log::logger().setDisplay("script.javaScript.warning", true);
	sLog.clear();
}


int LogLinesContaining(std::string_view text)
{
	int count = 0;
	for (const std::string &line : sLog)
	{
		if (line.find(text) != std::string::npos)  count++;
	}
	return count;
}


std::string LogText()
{
	std::string text;
	for (const std::string &line : sLog)  text += line + "\n";
	return text;
}


// An owned value, freed when it goes out of scope; it reads as the borrowed value.
class Owned
{
public:
	explicit Owned(JSValue value): _value(value)  {}
	~Owned()  { JS_FreeValue(OOJSAcquireContext(), _value); }
	Owned(const Owned &) = delete;
	Owned &operator=(const Owned &) = delete;
	operator JSValueConst() const  { return _value; }

private:
	JSValue _value;
};


// Evaluates source as a global script in file (line 1 first); the result, or JS_EXCEPTION with the
// exception pending. Owned by the caller.
JSValue EvalIn(JSContext *context, const std::string &source, const char *file)
{
	return JS_Eval(context, source.c_str(), source.size(), file, JS_EVAL_TYPE_GLOBAL);
}


// Evaluates source in the global object, which must not throw.
Owned Eval(JSContext *context, const std::string &source)
{
	JSValue result = EvalIn(context, source, "test.js");
	EXPECT_TRUE(!JS_IsException(result));
	return Owned(result);
}


std::string Describe(JSContext *context, const std::string &source, bool abbreviate)
{
	return OOJSDescribeValue(context, Eval(context, source), abbreviate);
}


// Whether value is the object other.
bool IsSameObject(JSValueConst value, JSValueConst other)
{
	return JS_IsObject(value) && JS_IsObject(other) && JS_VALUE_GET_PTR(value) == JS_VALUE_GET_PTR(other);
}


// object.toString(), as a string.
std::string CallToString(JSContext *context, JSValueConst object)
{
	Owned function(JS_GetPropertyStr(context, object, "toString"));
	Owned result(JS_Call(context, function, object, 0, nullptr));
	EXPECT_TRUE(!JS_IsException(result));
	return OOStringFromJSValue(context, result).value_or("<none>");
}


// A JS class whose objects hold a retained native object, finalised by the engine's wrapper hook.
JSClassID sTestWrapperClassID = 0;
const JSClassDef sTestWrapperClass =
{
	.class_name = "TestWrapper",
	.finalizer = OOJSObjectWrapperFinalize,
};


// Vault: a value held only natively, served by the class's get_property hook and cleared by its
// delete_property hook (missionVariables loaded from a save). SpiderMonkey ran delProperty on
// `delete` whether or not the object had an own property of that name, so a delete never preceded
// by a JS assignment must still reach the hook.
bool sVaultHasA = true;
int sVaultDels = 0;


bool AtomIs(JSAtom atom, const char *name)
{
	return OOStringFromJSID(atom).value_or("") == name;
}


JSValue VaultGet(JSContext *context, JSValueConst, JSAtom atom, JSValueConst)
{
	if (sVaultHasA && AtomIs(atom, "a"))  return JS_NewInt32(context, 11);
	return JS_UNDEFINED;
}


int VaultDelete(JSContext *, JSValueConst, JSAtom atom)
{
	++sVaultDels;
	if (AtomIs(atom, "a"))  sVaultHasA = false;
	return 1;
}


JSClassExoticMethods sVaultExoticMethods =
{
	.delete_property = VaultDelete,
	.get_property = VaultGet,
};
JSClassID sVaultClassID = 0;
const JSClassDef sVaultClass =
{
	.class_name = "Vault",
	.exotic = &sVaultExoticMethods,
};

}	// namespace


// MARK: The engine ---------------------------------------------------------------------------------

TEST(OOJavaScriptEngine, sharedEngine)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		EXPECT_TRUE(engine != nullptr);
		EXPECT_TRUE(OOJavaScriptEngine::sharedEngine() == engine);
		EXPECT_TRUE(gOOJSMainThreadContext != NULL);
		EXPECT_TRUE(JS_IsObject(engine->globalObject()));

		JSContext *context = OOJSAcquireContext();
		EXPECT_TRUE(IsSameObject(Owned(JS_GetGlobalObject(context)), engine->globalObject()));
		// The Oolite classes and the prefix script are set up on the global object.
		EXPECT_TRUE(JS_IsObject(Eval(context, "Vector3D")));
		EXPECT_TRUE(JS_IsObject(Eval(context, "missionVariables")));
		OOJSRelinquishContext(context);

		engine->runMissionCallback();	// no callback set: does nothing
	}
}


TEST(OOJavaScriptEngine, standardClasses)
{
	::SetUp();
	{
		// The engine looks the standard classes' IDs up when it starts.
		EXPECT_TRUE(gOOJSObjectClassID != 0);
		EXPECT_TRUE(gOOJSStringClassID != 0);
		EXPECT_TRUE(gOOJSArrayClassID != 0);
		EXPECT_TRUE(gOOJSNumberClassID != 0);
		EXPECT_TRUE(gOOJSBooleanClassID != 0);
		EXPECT_TRUE(gOOJSObjectClassID != gOOJSArrayClassID);
		EXPECT_TRUE(gOOJSStringClassID != gOOJSNumberClassID);
		EXPECT_TRUE(gOOJSNumberClassID != gOOJSBooleanClassID);

		JSContext *context = OOJSAcquireContext();
		EXPECT_TRUE(JS_GetClassID(Eval(context, "[1, 2]")) == gOOJSArrayClassID);
		EXPECT_TRUE(JS_GetClassID(Eval(context, "({ a: 1 })")) == gOOJSObjectClassID);
		EXPECT_TRUE(JS_GetClassID(Eval(context, "new String('s')")) == gOOJSStringClassID);
		EXPECT_TRUE(JS_GetClassID(Eval(context, "new Number(1)")) == gOOJSNumberClassID);
		EXPECT_TRUE(JS_GetClassID(Eval(context, "new Boolean(false)")) == gOOJSBooleanClassID);
		OOJSRelinquishContext(context);
	}
}


TEST(OOJavaScriptEngine, flags)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		const bool show = engine->showErrorLocations();
		engine->setShowErrorLocations(true);
		EXPECT_TRUE(engine->showErrorLocations());
		engine->setShowErrorLocations(false);
		EXPECT_TRUE(!engine->showErrorLocations());
		engine->setShowErrorLocations(show);

#ifndef NDEBUG
		// Off unless the preferences say otherwise (the scratch home has none).
		EXPECT_TRUE(!engine->dumpStackForErrors());
		EXPECT_TRUE(!engine->dumpStackForWarnings());
		engine->setDumpStackForErrors(true);
		EXPECT_TRUE(engine->dumpStackForErrors());
		EXPECT_TRUE(!engine->dumpStackForWarnings());
		engine->setDumpStackForWarnings(true);
		EXPECT_TRUE(engine->dumpStackForWarnings());
		engine->setDumpStackForErrors(false);
		engine->setDumpStackForWarnings(false);
		EXPECT_TRUE(!engine->dumpStackForErrors());
		EXPECT_TRUE(!engine->dumpStackForWarnings());
#endif
	}
}


TEST(OOJavaScriptEngine, callJSFunction)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		JSContext *context = OOJSAcquireContext();
		Owned function = Eval(context, "(function (a, b) { return this.base + a * b; })");
		Owned thisValue = Eval(context, "({ base: 100 })");
		OOJSRelinquishContext(context);

		JSValueConst argv[2] = { JS_NewInt32(context, 6), JS_NewInt32(context, 7) };
		JSValue result = JS_UNDEFINED;
		EXPECT_TRUE(engine->callJSFunction(function, thisValue, 2, argv, &result));
		EXPECT_TRUE(JS_VALUE_GET_TAG(result) == JS_TAG_INT && JS_VALUE_GET_INT(result) == 142);
		JS_FreeValue(context, result);
		result = JS_UNDEFINED;

		// A function that throws: NO, and the exception is reported (not left pending).
		StartLog();
		context = OOJSAcquireContext();
		Owned thrower = Eval(context, "(function () { throw new Error('call boom'); })");
		OOJSRelinquishContext(context);
		EXPECT_TRUE(!engine->callJSFunction(thrower, thisValue, 0, NULL, &result));
		EXPECT_TRUE(LogLinesContaining("call boom") >= 1);
		context = OOJSAcquireContext();
		EXPECT_TRUE(!JS_HasException(context));
		OOJSRelinquishContext(context);
	}
}


// What a holder of a JS value does in place of the old GC roots: it keeps its own reference
// (OOJSEngineCore.h, rule 4), and the collector leaves the values alone.
TEST(OOJavaScriptEngine, gcRoots)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		JSContext *context = OOJSAcquireContext();
		JSValue value = EvalIn(context, "({ kept: 'value' })", "test.js");
		JSValue object = EvalIn(context, "({ kept: 'object' })", "test.js");
		OOJSRelinquishContext(context);

		engine->garbageCollectionOpportunity(true);
		engine->garbageCollectionOpportunity(false);
		context = OOJSAcquireContext();
		EXPECT_TRUE(OOStringFromJSValue(context, Eval(context, "1, 'ok'")).value_or("") == "ok");
		OOJSRelinquishContext(context);

		JS_FreeValue(context, value);
		JS_FreeValue(context, object);
		engine->garbageCollectionOpportunity(true);
	}
}


// MARK: Errors and warnings ------------------------------------------------------------------------

TEST(OOJavaScriptEngine, errorReporting)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		const bool show = engine->showErrorLocations();
#if OOJSENGINE_MONITOR_SUPPORT
		TestMonitor monitorObject;	// set while the test runs, then cleared (the engine borrows it)
		TestMonitor *monitor = &monitorObject;
		engine->setMonitor(monitor);
#endif

		// An error reported by a native, with the caller's prefix.
		StartLog();
		engine->setShowErrorLocations(false);
		JSContext *context = OOJSAcquireContext();
		OOJSReportErrorForCaller(context, std::string("TestClass"), std::string("doThing"), "bad %s %d", "input", 7);
		OOJSReportPendingException(context);
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("***** JavaScript ") == 1);
		EXPECT_TRUE(LogLinesContaining("(<unidentified script>): ") == 1);
		EXPECT_TRUE(LogLinesContaining("TestClass.doThing: bad input 7") == 1);
#if OOJSENGINE_MONITOR_SUPPORT
		EXPECT_TRUE(monitor->_errors == 1);
		EXPECT_TRUE(monitor->_engine == engine);
		EXPECT_TRUE(!monitor->_showLocation);
		EXPECT_TRUE(monitor->_message.find("TestClass.doThing: bad input 7") != std::string::npos);
#endif

		// A bad-arguments report describes the arguments.
		StartLog();
		context = OOJSAcquireContext();
		{
			Owned three = Eval(context, "'three'");
			JSValueConst args[2] = { JS_NewInt32(context, 3), three };
			OOJSReportBadArguments(context, std::string("Ship"), std::string("fly"), 2, args, std::nullopt, std::string("number"));
		}
		OOJSReportPendingException(context);
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("Ship.fly: Invalid arguments (3, \"three\") -- expected number.") == 1);

		// An uncaught exception with locations shown: two lines.
		StartLog();
		engine->setShowErrorLocations(true);
		context = OOJSAcquireContext();
		const char *source = "\n\nthrow new Error('located boom');";
		JSValue result = EvalIn(context, source, "located.js");
		EXPECT_TRUE(JS_IsException(result));
		OOJSReportPendingException(context);
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("located boom") >= 1);
		EXPECT_TRUE(LogLinesContaining("located.js, line 3") == 1);
#if OOJSENGINE_MONITOR_SUPPORT
		EXPECT_TRUE(monitor->_showLocation);
#endif

		// No second line when the stack skip is set.
		StartLog();
		OOJSSetWarningOrErrorStackSkip(1);
		context = OOJSAcquireContext();
		result = EvalIn(context, source, "located.js");
		EXPECT_TRUE(JS_IsException(result));
		OOJSReportPendingException(context);
		OOJSRelinquishContext(context);
		OOJSSetWarningOrErrorStackSkip(0);
		EXPECT_TRUE(LogLinesContaining("located boom") >= 1);
		EXPECT_TRUE(LogLinesContaining("located.js, line 3") == 0);

		// A warning: dashes, and nothing pending.
		StartLog();
		engine->setShowErrorLocations(false);
		context = OOJSAcquireContext();
		OOJSReportWarningForCaller(context, std::nullopt, std::string("warnFunc"), "careful %s", "now");
		EXPECT_TRUE(!JS_HasException(context));
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("----- JavaScript warning") == 1);
		EXPECT_TRUE(LogLinesContaining("warnFunc: careful now") == 1);

#if OOJSENGINE_MONITOR_SUPPORT
		// Script log messages go to the monitor too.
		const int logs = monitor->_logs;
		context = OOJSAcquireContext();
		Eval(context, "log('monitored message')");
		OOJSRelinquishContext(context);
		EXPECT_TRUE(monitor->_logs == logs + 1);
		EXPECT_TRUE(monitor->_message == "monitored message");
		EXPECT_TRUE(!monitor->_messageClass.has_value());
		engine->setMonitor(nullptr);
#endif
		engine->setShowErrorLocations(show);
	}
}


#ifndef NDEBUG
// A native that dumps the JS stack it is called from.
JSValue DumpStackNative(OOJS_NATIVE_ARGS)
{
	(void)oojsThis;
	(void)argc;
	(void)oojsArgv;
	OOJSDumpStack(context);
	return JS_UNDEFINED;
}


TEST(OOJavaScriptEngine, stackDump)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		JSContext *context = OOJSAcquireContext();
		EXPECT_TRUE(JS_SetPropertyStr(context, engine->globalObject(), "testDumpStack", JS_NewCFunction(context, DumpStackNative, "testDumpStack", 0)) >= 0);

		// #768's dump is QuickJS's own stack text: each frame's function and location
		// (file:line:column), no variables, innermost first.
		StartLog();
		const char *source = "function dumpTarget(a) {\n\tvar b = a + 1;\n\ttestDumpStack();\n\treturn b;\n}\ndumpTarget(41);\n";
		JSValue result = EvalIn(context, source, "dumpstack.js");
		EXPECT_TRUE(!JS_IsException(result));
		JS_FreeValue(context, result);
		EXPECT_TRUE(LogLinesContaining("at dumpTarget (dumpstack.js:3:") == 1);
		EXPECT_TRUE(LogLinesContaining("at <eval> (dumpstack.js:6:") == 1);
		if (LogLinesContaining("dumpstack.js") == 0)  fprintf(stderr, "%s", LogText().c_str());
		OOJSRelinquishContext(context);

		// The stack skip is the error handler's (#768): a dump keeps every frame.
		StartLog();
		OOJSSetWarningOrErrorStackSkip(1);
		context = OOJSAcquireContext();
		result = EvalIn(context, source, "dumpstack.js");
		EXPECT_TRUE(!JS_IsException(result));
		JS_FreeValue(context, result);
		OOJSRelinquishContext(context);
		OOJSSetWarningOrErrorStackSkip(0);
		EXPECT_TRUE(LogLinesContaining("at dumpTarget (dumpstack.js:3:") == 1);
		EXPECT_TRUE(LogLinesContaining("at <eval> (dumpstack.js:6:") == 1);

		// This backend has no `debugger` hook: the statement does nothing.
		engine->enableDebuggerStatement();
		StartLog();
		context = OOJSAcquireContext();
		Eval(context, "(function () { debugger; return 1; })();");
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("debugger invoked") == 0);

		// With no JS on the stack, a dump writes nothing.
		StartLog();
		context = OOJSAcquireContext();
		OOJSDumpStack(context);
		OOJSRelinquishContext(context);
		EXPECT_TRUE(LogLinesContaining("script.javaScript.stackTrace") + LogLinesContaining("<Oolite native>") == 0);
	}
}
#endif


// MARK: Values -------------------------------------------------------------------------------------

TEST(OOJavaScriptEngine, describeValue)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();
		EXPECT_EQ(Describe(context, "'a\"b\\n'", false), "\"a\\\"b\\n\"");
		EXPECT_EQ(Describe(context, "new String('boxed')", false), "\"boxed\"");
		EXPECT_EQ(Describe(context, "new Array(251).join('x')", false), "\"" + std::string(200, 'x') + "...\"");
		EXPECT_EQ(Describe(context, "[1, 2, 3]", false), "[1, 2, 3]");
		EXPECT_EQ(Describe(context, "[1, 'two', 3, 4, 5, 6]", false), "[1, \"two\", 3, 4, ... <6 items total>]");
		EXPECT_EQ(Describe(context, "[[1, 2], {}]", false), "[[<2 items>], {...}]");
		EXPECT_EQ(Describe(context, "({ a: 1 })", true), "{...}");
		EXPECT_EQ(Describe(context, "({ a: 1 })", false), "[object Object]");
		EXPECT_EQ(Describe(context, "(function namedFn() {})", false), "function namedFn");
		EXPECT_EQ(Describe(context, "3.5", false), "3.5");
		EXPECT_EQ(Describe(context, "null", false), "null");
		EXPECT_EQ(Describe(context, "undefined", false), "undefined");
		EXPECT_EQ(Describe(context, "true", false), "true");

		Owned p = Eval(context, "'p'");
		JSValueConst params[2] = { JS_NewInt32(context, 1), p };
		EXPECT_EQ(OOJSStringWithJavaScriptParameters(params, 2, context).value_or("<none>"), "(1, \"p\")");
		OOJSRelinquishContext(context);
	}
}


// A getter that throws while the engine reads a value's name, constructor, stack or element (for a
// description, a class name or a report) leaves no exception pending afterwards.
TEST(OOJavaScriptEngine, throwingAccessorsLeaveNoPendingException)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();

		Owned noConstructor = Eval(context, "({ get constructor() { throw 1; } })");
		EXPECT_EQ(OOJSClassNameOfValue(context, noConstructor), "Object");
		EXPECT_TRUE(!JS_HasException(context));

		Owned noName = Eval(context, "({ constructor: Object.defineProperty(function () {}, 'name', { get: function () { throw 2; } }) })");
		EXPECT_EQ(OOJSClassNameOfValue(context, noName), "Object");
		EXPECT_TRUE(!JS_HasException(context));

		Describe(context, "Object.defineProperty(function f() {}, 'name', { get: function () { throw 3; } })", false);
		EXPECT_TRUE(!JS_HasException(context));

		EXPECT_EQ(Describe(context, "Object.defineProperty([1, 2], 0, { get: function () { throw 4; } })", false), "[?, 2]");
		EXPECT_TRUE(!JS_HasException(context));

		StartLog();
		JSValue thrown = EvalIn(context, "var e = new Error('m'); Object.defineProperty(e, 'name', { get: function () { throw 5; } }); "
		                        "Object.defineProperty(e, 'stack', { get: function () { throw 6; } }); throw e;", "test.js");
		EXPECT_TRUE(JS_IsException(thrown));
		OOJSReportPendingException(context);
		EXPECT_TRUE(!JS_HasException(context));

		OOJSRelinquishContext(context);
	}
}

TEST(OOJavaScriptEngine, idAndStringCaches)
{
	::SetUp();
	{
		JSAtom identifier = OOJSID("testIdentifier");
		EXPECT_EQ(OOStringFromJSID(identifier).value_or("<none>"), "testIdentifier");
		EXPECT_TRUE(OOJSID("testIdentifier") == identifier);

		JSContext *context = OOJSAcquireContext();
		Owned literal(OOJSSTR("test literal"));
		EXPECT_TRUE(JS_IsString(literal));
		EXPECT_EQ(OOStringFromJSValue(context, literal).value_or("<none>"), "test literal");
		OOJSRelinquishContext(context);
	}
}


TEST(OOJavaScriptEngine, nativeObjectValues)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();

		// nil and OONull are JS null; OONull is one object that copies as itself.
    // (nil is the null PList, through OOJSValueFromPList.)
		EXPECT_TRUE(JS_IsNull(Owned(OOJSValueFromPList(context, oo::PList()))));
		OONull *null = OONull::null();
		EXPECT_TRUE(null != nullptr && OONull::null() == null);
		EXPECT_TRUE(OOIsNullObjectNode(OONullObjectNode()) && OONullObjectNode() == OONullObjectNode());	// the one null, shared
		EXPECT_EQ(null->description(), "<null>");
		EXPECT_TRUE(JS_IsNull(Owned(OOJSValueFromPList(context, OONullObjectNode()))));
		EXPECT_TRUE(JS_IsNull(Owned(null->jsValueInContext(context))));

		// A native vector is a Vector3D.
		std::shared_ptr<oo::NativeVector> vector = oo::makeShared<oo::NativeVector>(make_vector(1.0f, 2.0f, 3.0f));
		Owned vectorValue(OOJSValueFromPList(context, oo::PList(oo::PList::Object(vector))));
		EXPECT_TRUE(JS_IsObject(vectorValue));
		const char *className = OOJSClassName(JS_GetClassID(vectorValue));
		EXPECT_TRUE(className != nullptr && std::strcmp(className, "Vector3D") == 0);
		EXPECT_EQ(OOStringFromJSValue(context, vectorValue).value_or("<none>"), "(1, 2, 3)");

		OOJSRelinquishContext(context);
	}
}


TEST(OOJavaScriptEngine, jsValueHolder)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();
		Owned object = Eval(context, "({ held: 'yes' })");
		std::shared_ptr<OOJSValue> holder = OOJSValue::valueWithJSValue(object, context);
		EXPECT_TRUE(holder != nullptr);
		EXPECT_TRUE(IsSameObject(Owned(holder->jsValueInContext(context)), object));
		EXPECT_TRUE(IsSameObject(Owned(OOJSValueFromPList(context, OOJSValueObjectNode(holder.get()))), object));
		OOJSRelinquishContext(context);

		// A nil context is acquired for the call.
		std::shared_ptr<OOJSValue> noContext = OOJSValue::valueWithJSValue(JS_NewInt32(context, 9), NULL);
		JSValue nine = noContext->jsValueInContext(NULL);
		EXPECT_TRUE(JS_VALUE_GET_TAG(nine) == JS_TAG_INT && JS_VALUE_GET_INT(nine) == 9);

		// Undefined holds undefined.
		std::shared_ptr<OOJSValue> undefinedHolder = OOJSValue::valueWithJSValue(JS_UNDEFINED, NULL);
		EXPECT_TRUE(JS_IsUndefined(undefinedHolder->jsValueInContext(NULL)));

		OOJavaScriptEngine::sharedEngine()->garbageCollectionOpportunity(true);	// the held object survives
		context = OOJSAcquireContext();
		EXPECT_EQ(OOStringFromJSValue(context, Owned(holder->jsValueInContext(context))).value_or("<none>"), "[object Object]");
		OOJSRelinquishContext(context);
	}
}


// The generic wrapper's hooks with the C++ object the slot holds: the finalizer
// tells it its wrapper is gone, releases it and empties the slot; toString
// names the JS class (only an object with a converter described itself).
TEST(OOJavaScriptEngine, cxxObjectWrapper)
{
	::SetUp();
	std::shared_ptr<TestCxxNative> native = oo::makeShared<TestCxxNative>();
	const int deallocs = sCxxNativeDeallocs;
	{
		JSContext *context = OOJSAcquireContext();
		OOJSRegisterClass(&sTestWrapperClassID, &sTestWrapperClass);
		Owned wrapper(JS_NewObjectClass(context, static_cast<int>(sTestWrapperClassID)));
		EXPECT_TRUE(JS_IsObject(wrapper));
		EXPECT_TRUE(OOJSSetCxxPrivate(wrapper, native));
		EXPECT_TRUE(JS_SetPropertyStr(context, wrapper, "toString", JS_NewCFunction(context, OOJSObjectWrapperToString, "toString", 0)) >= 0);

		// Unconverted, toString names the JS class.
		EXPECT_EQ(CallToString(context, wrapper), "[object TestWrapper]");

		// With the basic private-object converter the wrapper is an Object node of the C++ object,
		// and toString still names the JS class.
		OOJSRegisterObjectConverter(sTestWrapperClassID, OOJSBasicPrivateObjectConverter);
		const oo::PList node = OOJSPListFromJSObject(context, wrapper);
		EXPECT_TRUE(node.getIf<oo::PList::Object>() != nullptr && node.getIf<oo::PList::Object>()->get() == native.get());
		EXPECT_EQ(CallToString(context, wrapper), "[object TestWrapper]");

		// Finalising clears the native object's JS self, releases it and empties the slot.
		OOJSObjectWrapperFinalize(JS_GetRuntime(context), wrapper);
		EXPECT_TRUE(native->_clears == 1);
		JSClassID ignored;
		EXPECT_TRUE(JS_GetAnyOpaque(wrapper, &ignored) == NULL);
		EXPECT_TRUE(OOJSPListFromJSObject(context, wrapper).isNull());
		OOJSObjectWrapperFinalize(JS_GetRuntime(context), wrapper);	// nothing left: does nothing
		EXPECT_TRUE(native->_clears == 1);

		// With no native object, toString names the JS class.
		EXPECT_EQ(CallToString(context, wrapper), "[object TestWrapper]");

		OOJSRegisterObjectConverter(sTestWrapperClassID, NULL);
		OOJSRelinquishContext(context);
	}
	EXPECT_TRUE(sCxxNativeDeallocs == deallocs);	// the test's own reference
	native = nullptr;
	EXPECT_TRUE(sCxxNativeDeallocs == deallocs + 1);
}


// `delete` of a property the object does not have still reaches the class's delete hook, which
// clears the value it serves natively (moved from the facade backend's own test).
TEST(OOJavaScriptEngine, deleteReachesTheClassHookWithNoOwnProperty)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();
		OOJSRegisterClass(&sVaultClassID, &sVaultClass);
		Owned global(JS_GetGlobalObject(context));
		EXPECT_TRUE(JS_SetPropertyStr(context, global, "vault", JS_NewObjectClass(context, static_cast<int>(sVaultClassID))) >= 0);
		Owned result = Eval(context, "[vault.a, delete vault.a, vault.a === undefined].join('|')");
		EXPECT_EQ(OOStringFromJSValue(context, result).value_or("<none>"), "11|true|true");
		EXPECT_TRUE(sVaultDels == 1 && !sVaultHasA);
		EXPECT_EQ(OOStringFromJSValue(context, Eval(context, "delete global.vault")).value_or("<none>"), "true");
		OOJSRelinquishContext(context);
	}
}


TEST(OOJavaScriptEngine, entityPredicates)
{
	::SetUp();
	{
		EXPECT_TRUE(!JSEntityIsJavaScriptVisiblePredicate(nullptr, NULL));
		EXPECT_TRUE(!JSEntityIsJavaScriptSearchablePredicate(nullptr, NULL));
		EXPECT_TRUE(!JSEntityIsDemoShipPredicate(nullptr, NULL));

		TestScriptEntity *entity = static_cast<TestScriptEntity *>(oo::MadeEntity(oo::makeShared<TestScriptEntity>()));
		entity->setStatus(oo::EntityStatus::kInFlight);
		EXPECT_TRUE(!JSEntityIsJavaScriptVisiblePredicate(entity, NULL));
		EXPECT_TRUE(!JSEntityIsJavaScriptSearchablePredicate(entity, NULL));

		entity->_scriptVisible = true;
		EXPECT_TRUE(JSEntityIsJavaScriptVisiblePredicate(entity, NULL));
		EXPECT_TRUE(JSEntityIsJavaScriptSearchablePredicate(entity, NULL));	// neither ship nor planet
		EXPECT_TRUE(!JSEntityIsDemoShipPredicate(entity, NULL));

		entity->isShip = true;
		EXPECT_TRUE(JSEntityIsJavaScriptSearchablePredicate(entity, NULL));
		EXPECT_TRUE(!JSEntityIsDemoShipPredicate(entity, NULL));
		entity->isSubEntity = true;
		EXPECT_TRUE(!JSEntityIsJavaScriptSearchablePredicate(entity, NULL));
		entity->isSubEntity = false;
		entity->setStatus(oo::EntityStatus::kCockpitDisplay);
		EXPECT_TRUE(!JSEntityIsJavaScriptSearchablePredicate(entity, NULL));
		EXPECT_TRUE(JSEntityIsDemoShipPredicate(entity, NULL));
		entity->isSubEntity = true;
		EXPECT_TRUE(!JSEntityIsDemoShipPredicate(entity, NULL));

		// A JS function as a predicate: called with the entity's JS value.
		JSContext *context = OOJSAcquireContext();
		Owned isFortyTwo = Eval(context, "(function (e) { return e === 42; })");
		JSFunctionPredicateParameter param = { context, isFortyTwo, JS_UNDEFINED, false };
		EXPECT_TRUE(JSFunctionPredicate(entity, &param));
		EXPECT_TRUE(!param.errorFlag);
		Owned isNotFortyTwo = Eval(context, "(function (e) { return e !== 42; })");
		param.function = isNotFortyTwo;
		EXPECT_TRUE(!JSFunctionPredicate(entity, &param));
		EXPECT_TRUE(!param.errorFlag);

		StartLog();
		Owned thrower = Eval(context, "(function (e) { throw new Error('predicate boom'); })");
		param.function = thrower;
		EXPECT_TRUE(!JSFunctionPredicate(entity, &param));
		EXPECT_TRUE(param.errorFlag);
		EXPECT_TRUE(LogLinesContaining("predicate boom") >= 1);
		Owned always = Eval(context, "(function (e) { return true; })");
		param.function = always;
		EXPECT_TRUE(!JSFunctionPredicate(entity, &param));	// the error flag stops further filtering
		OOJSRelinquishContext(context);
	}
}


TEST(OOJavaScriptEngine, propertyListsFromJS)
{
	::SetUp();
	{
		JSContext *context = OOJSAcquireContext();
		oo::PList array = OOJSPListFromJSValue(context, Eval(context, "[1, null, 'a', [true]]"));
		EXPECT_TRUE(array.isArray());
		const oo::PList::Array *elements = array.getIf<oo::PList::Array>();
		EXPECT_TRUE(elements != nullptr && elements->size() == 4);
		if (elements != nullptr && elements->size() == 4)
		{
			EXPECT_TRUE((*elements)[0] == oo::PList::signedInteger(1));
			EXPECT_TRUE(OOIsNullObjectNode((*elements)[1]));	// a null element is OONull
			EXPECT_TRUE((*elements)[2] == oo::PList(std::string("a")));
			EXPECT_TRUE((*elements)[3].isArray());
		}
		EXPECT_TRUE(OOJSPListFromJSValue(context, Eval(context, "[]")).isArray());

		oo::PList dict = OOJSPListFromJSValue(context, Eval(context, "({ s: new String('x'), n: new Number(2.5), b: new Boolean(false) })"));
		EXPECT_TRUE(dict.isDict());
		EXPECT_TRUE(dict.find("s") != nullptr && *dict.find("s") == oo::PList(std::string("x")));
		EXPECT_TRUE(dict.find("n") != nullptr && *dict.find("n") == oo::PList(2.5));
		EXPECT_TRUE(dict.find("b") != nullptr && *dict.find("b") == oo::PList(static_cast<bool>(false)));	// OOCocoa.h defines false as 0

    // Round trip through OOJSValueFromPList.
		oo::PList back = OOJSPListFromJSValue(context, Owned(OOJSValueFromPList(context, oo::PList(oo::PList::Array{oo::PList(std::string("q")), oo::PList::signedInteger(5)}))));
		EXPECT_TRUE(back == oo::PList(oo::PList::Array{oo::PList(std::string("q")), oo::PList::signedInteger(5)}));
		OOJSRelinquishContext(context);
	}
}


// MARK: Reset (last: it rebuilds the context) -----------------------------------------------------

TEST(OOJavaScriptEngine, reset)
{
	::SetUp();
	{
		OOJavaScriptEngine *engine = OOJavaScriptEngine::sharedEngine();
		JSContext *context = OOJSAcquireContext();
		Eval(context, "this.survivesReset = 1;");
		OOJSRelinquishContext(context);

		static int willResets = 0, didResets = 0, otherSender = 0;
		static int marker;
		static bool contextDuringWill = false;
		oo::NotificationCenter::defaultCenter().addObserver(&marker, kOOJavaScriptEngineWillResetNotificationName, engine,
			[](const oo::Notification &) { willResets++; contextDuringWill = (gOOJSMainThreadContext != NULL); });
		oo::NotificationCenter::defaultCenter().addObserver(&marker, kOOJavaScriptEngineDidResetNotificationName, engine,
			[](const oo::Notification &) { didResets++; });
		oo::NotificationCenter::defaultCenter().addObserver(&marker, kOOJavaScriptEngineDidResetNotificationName, &otherSender,
			[](const oo::Notification &) { otherSender++; });

		EXPECT_TRUE(engine->reset());
		EXPECT_TRUE(willResets == 1 && didResets == 1 && otherSender == 0);
		EXPECT_TRUE(contextDuringWill);
		EXPECT_TRUE(OOJavaScriptEngine::sharedEngine() == engine);
		EXPECT_TRUE(gOOJSMainThreadContext != NULL);
		EXPECT_TRUE(JS_IsObject(engine->globalObject()));
		EXPECT_TRUE(gOOJSArrayClassID != 0);

		context = OOJSAcquireContext();
		EXPECT_EQ(OOStringFromJSValue(context, Eval(context, "typeof survivesReset")).value_or("<none>"), "undefined");
		EXPECT_TRUE(JS_IsObject(Eval(context, "Vector3D")));
		EXPECT_TRUE(JS_GetClassID(Eval(context, "[1]")) == gOOJSArrayClassID);
		// The standard converters are registered again.
		EXPECT_TRUE(OOJSPListFromJSValue(context, Eval(context, "[1]")).isArray());
		OOJSRelinquishContext(context);

		oo::NotificationCenter::defaultCenter().removeObserver(&marker);
	}
}


TEST(OOJavaScriptEngine, cleanUp)
{
	oo::log::logger().setSink(nullptr);
	stdfs::current_path(stdfs::temp_directory_path());
	std::error_code ignored;
	stdfs::remove_all(sRoot, ignored);
	EXPECT_TRUE(true);
}
