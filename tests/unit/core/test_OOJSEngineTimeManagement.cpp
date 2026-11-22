/*	test_OOJSEngineTimeManagement.mm
	Unit tests for the script profiler (src/Core/Scripting/OOJSEngineTimeManagement.h/.mm).

	The file holds the debug-build profiler, whose results are OOTimeProfile and OOTimeProfileEntry
	objects. (The time limiter is the engine's, in OOJavaScriptEngine.mm, as #768 has it, on
	QuickJS-ng's interrupt handler; its cases are in test_OOJSTimeLimiter.mm.) The test links the
	game's own objects for the file and the stopwatch (OOProfilingStopwatch.mm). What the rest of
	the engine would provide is defined below as the smallest stand-in that does the same thing:
	the time limiter the profiler reads, pauses and extends, and the property-list converter
	(recording what it is given). With the function call hook (OOJS_QUICKJS_CALL_HOOK, upstream
	#580), the profiler also installs its hook on the main thread context's runtime, so the test
	makes a QuickJS context of its own for that, and stands in for the three engine functions the
	hook's tracing names functions with; no JavaScript function runs, so they are never called.
	The tests pin the profiler's entries (names, hit counts, the frame kind), the
	profile's and entries' descriptions and property lists and the comparisons.
	Run: tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "OOJSEngineTimeManagement.h"
#include "oofnd/PList.hpp"


#include <chrono>
#include <optional>
#include <string>
#include <thread>
#include <vector>


// MARK: What the rest of the engine provides ------------------------------------------------------

namespace {
std::vector<oo::PList> sConverted;		// what OOJSValueFromPList was given
} // namespace


JSValue OOJSValueFromPList(JSContext *context, const oo::PList &plist)
{
	sConverted.push_back(plist);
	return JS_NewInt32(context, static_cast<std::int32_t>(sConverted.size()));
}


#if OOJS_PROFILE
// The engine's time limiter, which the profiler reads, pauses and extends: a limiter that is never
// started.
oo::TimeDelta OOJSGetTimeLimiterLimit(void)  { return 0.0; }
void OOJSPauseTimeLimiter(void)  {}
void OOJSResumeTimeLimiter(void)  {}
void OOJSExcludeTimeFromLimiter(oo::TimeDelta)  {}
#endif


#if OOJS_QUICKJS_CALL_HOOK
// The engine's main thread context, which the profiler installs its call hook on: the test's own
// (MainThreadContext below).
JSContext *gOOJSMainThreadContext = nullptr;

// What the hook's tracing names a JavaScript function with. No JavaScript function runs here.
std::string OOJSDescribeValue(JSContext *, JSValueConst, bool)  { return "?"; }
std::optional<std::string> OOStringFromJSID(JSAtom)  { return std::nullopt; }
#endif


// MARK: Helpers -----------------------------------------------------------------------------------

#if OOJS_PROFILE
namespace {

void SleepMs(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


OOTimeProfileEntry *EntryNamed(OOTimeProfile *profile, const std::string &name)
{
	for (const auto &entry : profile->profileEntries())
	{
		if (entry->function().value_or("<none>") == name)  return entry.get();
	}
	return nullptr;
}


const oo::PList *Key(const oo::PList &dict, const char *key)
{
	return dict.get<oo::PList>(key);
}


// A QuickJS runtime and context of the test's own, as the engine's main thread context for as long
// as it lives.
class MainThreadContext
{
public:
	MainThreadContext()
	{
#if OOJS_QUICKJS_CALL_HOOK
		_runtime = JS_NewRuntime();
		gOOJSMainThreadContext = JS_NewContext(_runtime);
#endif
	}

	~MainThreadContext()
	{
#if OOJS_QUICKJS_CALL_HOOK
		JS_FreeContext(gOOJSMainThreadContext);
		gOOJSMainThreadContext = nullptr;
		JS_FreeRuntime(_runtime);
#endif
	}

	MainThreadContext(const MainThreadContext &) = delete;
	MainThreadContext &operator=(const MainThreadContext &) = delete;

private:
#if OOJS_QUICKJS_CALL_HOOK
	JSRuntime *_runtime = nullptr;
#endif
};

}	// namespace
#endif


// MARK: Tests -------------------------------------------------------------------------------------

#if OOJS_PROFILE
TEST(OOJSEngineTimeManagement, profiling)
{
	{
		const MainThreadContext mainThreadContext;
		EXPECT_TRUE(!OOJSIsProfiling());
		OOJSBeginProfiling(false);
		EXPECT_TRUE(OOJSIsProfiling());

		OOJSProfileStackFrame outer, inner, again, nameless;
		OOJSProfileEnter(&outer, "outer");
		SleepMs(20);
		OOJSProfileEnter(&inner, "inner");
		SleepMs(20);
		OOJSProfileExit(&inner);
		OOJSProfileExit(&outer);
		OOJSProfileEnter(&again, "inner");
		OOJSProfileExit(&again);
		OOJSProfileEnter(&nameless, NULL);
		OOJSProfileExit(&nameless);

		std::shared_ptr<OOTimeProfile> profileRef = OOJSEndProfiling();
		OOTimeProfile *profile = profileRef.get();
		EXPECT_TRUE(!OOJSIsProfiling());
		EXPECT_TRUE(profile != nullptr);
		EXPECT_EQ(profile->profileEntries().size(), 3u);

		OOTimeProfileEntry *outerEntry = EntryNamed(profile, "outer");
		OOTimeProfileEntry *innerEntry = EntryNamed(profile, "inner");
		EXPECT_TRUE(outerEntry != nullptr && innerEntry != nullptr);
		EXPECT_EQ(outerEntry->hitCount(), 1u);
		EXPECT_EQ(innerEntry->hitCount(), 2u);
		EXPECT_TRUE(!innerEntry->isJavaScriptFrame());
		EXPECT_TRUE(outerEntry->totalTimeSum() >= innerEntry->totalTimeSum());	// outer contains inner's first call
		EXPECT_TRUE(outerEntry->totalTimeSum() >= outerEntry->selfTimeSum());
		EXPECT_EQ(innerEntry->totalTimeAverage(), innerEntry->totalTimeSum() / 2);
		EXPECT_EQ(innerEntry->selfTimeMax(), innerEntry->totalTimeMax());	// nothing below it
		EXPECT_TRUE(profile->totalTime() >= outerEntry->totalTimeSum());
		EXPECT_EQ(profile->nonExtensionTime(), profile->totalTime() - profile->extensionTime());
#if OOJS_QUICKJS_CALL_HOOK
		// The call hook times JavaScript functions themselves (upstream #580), and none ran.
		EXPECT_EQ(profile->javaScriptTime(), 0.0);
#else
		EXPECT_EQ(profile->javaScriptTime(), profile->totalTime() - profile->nativeTime());
#endif
		EXPECT_TRUE(profile->nativeTime() >= outerEntry->selfTimeSum() + innerEntry->selfTimeSum());

		// Comparisons: "reverse" is longest first.
		EXPECT_EQ(outerEntry->compareBySelfTime(outerEntry), oo::ComparisonResult::kSame);
		EXPECT_EQ(oo::ToUnderlying(outerEntry->compareByTotalTime(innerEntry)), -oo::ToUnderlying(outerEntry->compareByTotalTimeReverse(innerEntry)));
		EXPECT_EQ(oo::ToUnderlying(innerEntry->compareBySelfTime(outerEntry)), -oo::ToUnderlying(innerEntry->compareBySelfTimeReverse(outerEntry)));
		if (outerEntry->totalTimeSum() > innerEntry->totalTimeSum())  EXPECT_EQ(outerEntry->compareByTotalTimeReverse(innerEntry), oo::ComparisonResult::kAscending);

		// The descriptions.
		const std::string profileText = profile->description().value_or("");
		EXPECT_TRUE(profileText.find("Total time: ") != std::string::npos);
		EXPECT_TRUE(profileText.find("NAME  T  COUNT    TOTAL     SELF  TOTAL%   SELF%  SELFMAX") != std::string::npos);
		EXPECT_TRUE(profileText.find("inner  N      2 ") != std::string::npos);
		EXPECT_TRUE(profileText.find("outer  N      1 ") != std::string::npos);
		EXPECT_TRUE(profileText.find("(null)  N      1 ") != std::string::npos);
		const std::string innerText = innerEntry->description().value_or("");
		EXPECT_TRUE(innerText.find("inner: 2 times, total ") != std::string::npos);
		const std::string outerText = outerEntry->description().value_or("");
		EXPECT_TRUE(outerText.find("outer: 1 time, ") != std::string::npos);
		EXPECT_TRUE(outerText.find("(self ") != std::string::npos);

		// The property lists, as JS gets them.
		sConverted.clear();
		innerEntry->oo_jsValueInContext(NULL);
		EXPECT_EQ(sConverted.size(), 1u);
		if (sConverted.size() == 1)
		{
			const oo::PList &plist = sConverted[0];
			EXPECT_TRUE(plist.isDict() && plist.count() == 9);
			EXPECT_TRUE(Key(plist, "name") != nullptr && *Key(plist, "name")->getIf<std::string>() == "inner");
			EXPECT_TRUE(Key(plist, "hitCount") != nullptr && Key(plist, "hitCount")->doubleValue() == 2);
			EXPECT_TRUE(Key(plist, "isJavaScriptFrame") != nullptr && Key(plist, "isJavaScriptFrame")->type() == oo::PList::Type::kBool && !Key(plist, "isJavaScriptFrame")->boolValue());
		}
		sConverted.clear();
		EntryNamed(profile, "<none>")->oo_jsValueInContext(NULL);
		EXPECT_TRUE(sConverted.size() == 1 && sConverted[0].isDict() && sConverted[0].count() == 0);	// a nameless entry

		sConverted.clear();
		profile->oo_jsValueInContext(NULL);
		EXPECT_EQ(sConverted.size(), 1u);
		if (sConverted.size() == 1)
		{
			const oo::PList &plist = sConverted[0];
			EXPECT_TRUE(plist.isDict() && plist.count() == 7);
			for (const char *key : { "totalTime", "javaScriptTime", "nativeTime", "extensionTime", "nonExtensionTime", "profilerOverhead" })  EXPECT_TRUE(Key(plist, key) != nullptr && Key(plist, key)->type() == oo::PList::Type::kReal);
			const oo::PList *profiles = Key(plist, "profiles");
			EXPECT_TRUE(profiles != nullptr && profiles->isArray() && profiles->count() == 3);
			if (profiles != nullptr && profiles->isArray())
			{
				// Each entry's own property list, as #768 builds "profiles".
				for (const oo::PList &entry : *profiles->getIf<oo::PList::Array>())  EXPECT_TRUE(entry.isDict() && (entry.count() == 9 || entry.count() == 0));
			}
		}
	}
}
#endif
