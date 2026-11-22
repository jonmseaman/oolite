/*	test_OOJSScript.mm
        Unit tests for OOJSScript (src/Core/Scripting/OOJSScript.h/.mm).

        OOJSScript is a JavaScript world or ship script: it compiles a file into
   a JS object of class Script, sets that object's default properties from the
   OXP's manifest and the caller's properties, runs the file, and then answers
   the name, version and description the script set (or a name made from the
   path), reads and sets the object's properties, calls its methods with itself
   on the stack of running scripts, and goes invalid when the engine resets. It
   needs the real engine and the cache, so the test links every game object but
   main's (tests/unit/core/meson.build entry ['*']), defines gDebugFlags and
   points the user's directories at a scratch folder. The scripts are written to
   that folder. The script is held as
   std::shared_ptr<OOJSScript>, every case asks the C++ class, and the crossing case
   pins the script's identity (the running stack holds the C++ script, the
   Script object's private slot refers to it weakly). Run: bash
   meson test -C build/meson_test test_OOJSScript
*/

#include <process.h>

#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Description.h"
#include "OOJSPropID.h"
#include "OOJSScript.h"
#include "OOJavaScriptEngine.h"
#include "oofnd/FileSystem.hpp"
#include "oofnd/Notification.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;

void WriteText(const stdfs::path& path, const std::string& text) {
  stdfs::create_directories(path.parent_path());
  EXPECT_TRUE(oo::fs::writeFile(path, oo::Data(text.data(), text.size()),
                             oo::fs::WriteMode::kDirect)
               .has_value());
}

// The scratch home and game folder, made once, before the cache manager and the
// engine exist.
void SetUp() {
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-jsscript-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot);
  EXPECT_TRUE(::_putenv_s("HOMEPATH", sRoot.string().c_str()) == 0);
  stdfs::current_path(sRoot);
  WriteText(sRoot / "Resources" / "Info-gnustep.plist",
            "{ CFBundleVersion = \"9.9.9-test\"; }");
  (void)OOJavaScriptEngine::sharedEngine();
}

// A script file under the scratch root, as the game spells paths (forward
// slashes).
std::string Script(const std::string& relativePath, const std::string& source) {
  const stdfs::path path = sRoot / relativePath;
  WriteText(path, source);
  return path.generic_string();
}

// The script.
OOJSScript* JS(const std::shared_ptr<OOJSScript>& script) { return script.get(); }

std::string Text(const oo::PList& value) {
  if (const std::string* string = value.getIf<std::string>()) return *string;
  return value.isNull() ? "<null>" : oo::DescriptionOf(value);
}

const char* const kNamedScript =
    "\"use strict\";\n"
    "this.name = \"  test-script_ \";\n"
    "this.version = \"1.0\";\n"
    "this.description = \"A test\";\n"
    "this.counter = 0;\n"
    "this.bump = function (n) { this.counter += n; return this.counter; };\n";

}  // namespace

TEST(OOJSScript, load) {
  ::SetUp();
  {
    const std::shared_ptr<OOJSScript> script = OOJSScript::scriptWithPath(
        Script("named/named.js", kNamedScript), oo::PList());
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;
    EXPECT_TRUE(dynamic_cast<OOScript*>(script.get()) != nullptr);
    EXPECT_EQ(script->name().value_or("<none>"),
                "test-script");  // spaces and underscores stripped
    EXPECT_EQ(script->version().value_or("<none>"), "1.0");
    EXPECT_EQ(script->scriptDescription().value_or("<none>"), "A test");
    EXPECT_EQ(script->displayName().value_or("<none>"), "test-script 1.0");
    EXPECT_TRUE(!script->requiresTickle());
    script->runWithTarget(nullptr);  // does nothing
    EXPECT_TRUE(script->description().find("\"test-script\" version 1.0") !=
             std::string::npos);
    EXPECT_EQ(Text(JS(script)->propertyNamed("name")),
                "  test-script_ ");  // the property keeps what the script set
    EXPECT_TRUE(!OOJSScript::currentlyRunningScript());
    EXPECT_TRUE(OOJSScript::scriptStack().empty());

    // No file: nil. (A file that does not compile is not loaded here: its error
    // report goes through the game's JS error reporter, which needs the running
    // game.)
    EXPECT_TRUE(OOJSScript::scriptWithPath((sRoot / "missing.js").generic_string(),
                                        oo::PList()) == nullptr);
  }
}

TEST(OOJSScript, defaultNames) {
  ::SetUp();
  {
    // A script that names itself nothing is named after its file, or its OXP
    // for script.js.
    const std::shared_ptr<OOJSScript> byFile = OOJSScript::scriptWithPath(
        Script("loose/unnamed.js", "\"use strict\";\n"), oo::PList());
    EXPECT_EQ(byFile->name().value_or("<none>"), "unnamed.js.anon-script");
    EXPECT_TRUE(!byFile->version().has_value());
    EXPECT_EQ(byFile->displayName().value_or("<none>"),
                "unnamed.js.anon-script");
    EXPECT_EQ(Text(JS(byFile)->propertyNamed("name")),
                "unnamed.js.anon-script");

    const std::shared_ptr<OOJSScript> byOXP = OOJSScript::scriptWithPath(
        Script("Foo.oxp/Config/script.js", "\"use strict\";\n"), oo::PList());
    EXPECT_EQ(byOXP->name().value_or("<none>"), "Foo.anon-script");

    const std::shared_ptr<OOJSScript> byFolder = OOJSScript::scriptWithPath(
        Script("Bar/Scripts/script.js", "\"use strict\";\n"), oo::PList());
    EXPECT_EQ(byFolder->name().value_or("<none>"), "Scripts.anon-script");
  }
}

TEST(OOJSScript, manifestAndProperties) {
  ::SetUp();
  {
    WriteText(sRoot / "Baz.oxp" / "manifest.plist",
              "{ identifier = \"org.test.baz\"; version = \"2.5\"; author = "
              "\"Tester\"; license = \"CC0\"; }");
    oo::PList::Dict given;
    given["mission"] = oo::PList(std::string("given"));
    const std::shared_ptr<OOJSScript> script = OOJSScript::scriptWithPath(
        Script("Baz.oxp/Config/script.js",
               "\"use strict\";\nthis.name = \"baz\";\n"),
        oo::PList(std::move(given)));
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;
    EXPECT_EQ(script->version().value_or("<none>"),
                "2.5");  // from the manifest
    EXPECT_EQ(Text(JS(script)->propertyNamed("author")), "Tester");
    EXPECT_EQ(Text(JS(script)->propertyNamed("license")), "CC0");
    EXPECT_EQ(Text(JS(script)->propertyNamed(kLocalManifestProperty)),
                "org.test.baz");
    EXPECT_EQ(Text(JS(script)->propertyNamed("mission")), "given");

    // Set and define from outside; nothing to set is refused.
    EXPECT_TRUE(JS(script)->setProperty(oo::PList(3), "three"));
    EXPECT_EQ(Text(JS(script)->propertyNamed("three")), "3");
    EXPECT_TRUE(JS(script)->defineProperty(oo::PList(std::string("fixed")),
                                        "constant"));
    EXPECT_EQ(Text(JS(script)->propertyNamed("constant")), "fixed");
    EXPECT_TRUE(!JS(script)->setProperty(oo::PList(), "nothing"));
    EXPECT_TRUE(!JS(script)->defineProperty(oo::PList(), "nothing"));
    EXPECT_TRUE(JS(script)->propertyNamed("neverSet").isNull());

    // A temporary manifest identifier sets nothing.
    WriteText(sRoot / "Tmp.oxp" / "manifest.plist",
              "{ identifier = \"__oolite.tmp.1\"; version = \"9\"; }");
    const std::shared_ptr<OOJSScript> temporary = OOJSScript::scriptWithPath(
        Script("Tmp.oxp/Config/script.js",
               "\"use strict\";\nthis.name = \"tmp\";\n"),
        oo::PList());
    EXPECT_TRUE(!temporary->version().has_value());
    EXPECT_TRUE(JS(temporary)->propertyNamed(kLocalManifestProperty).isNull());
  }
}

TEST(OOJSScript, callMethod) {
  ::SetUp();
  {
    const std::shared_ptr<OOJSScript> script = OOJSScript::scriptWithPath(
        Script("call/call.js", kNamedScript), oo::PList());
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;

    JSContext* context = OOJSAcquireContext();
    JSValue argv[1] = {JS_NewInt32(context, 5)};
    JSValue result = JS_UNDEFINED;
    EXPECT_TRUE(script->callMethod(OOJSID("bump"), context, argv, 1, &result));
    double number = 0;
    EXPECT_TRUE(JS_ToFloat64(context, &number, result) == 0 && number == 5);
    JS_FreeValue(context, result);
    EXPECT_TRUE(script->callMethod(OOJSID("bump"), context, argv, 1, NULL));
    EXPECT_EQ(Text(JS(script)->propertyWithID(OOJSID("counter"), context)),
                "10");
    EXPECT_TRUE(
        !script->callMethod(OOJSID("noSuchMethod"), context, NULL, 0, NULL));
    JSValue self = script->jsValueInContext(context);
    EXPECT_TRUE(!JS_IsUndefined(self));
    JS_FreeValue(context, self);

    // A script that is not JavaScript answers NO (OOScript (JavaScriptEvents)).
    const std::shared_ptr<OOScript> plain = oo::makeShared<OOScript>();
    EXPECT_TRUE(!plain->callMethod(OOJSID("bump"), context, argv, 1, NULL));
    OOJSRelinquishContext(context);

    // After the call the stack of running scripts is empty again.
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == nullptr);
  }
}

TEST(OOJSScript, runningStack) {
  ::SetUp();
  {
    const std::shared_ptr<OOJSScript> first = OOJSScript::scriptWithPath(
        Script("stack/first.js", "\"use strict\";\nthis.name = \"first\";\n"),
        oo::PList());
    const std::shared_ptr<OOJSScript> second = OOJSScript::scriptWithPath(
        Script("stack/second.js", "\"use strict\";\nthis.name = \"second\";\n"),
        oo::PList());
    EXPECT_TRUE(first != nullptr && second != nullptr);

    OOJSScript::pushScript(first.get());
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == first.get());
    OOJSScript::pushScript(second.get());
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == second.get());
    const std::vector<std::shared_ptr<OOJSScript>> stack = OOJSScript::scriptStack();
    EXPECT_TRUE(stack.size() == 2 && stack[0].get() == first.get() &&
             stack[1].get() == second.get());  // outermost first
    OOJSScript::popScript(second.get());
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == first.get());
    OOJSScript::popScript(first.get());
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == nullptr);
    EXPECT_TRUE(OOJSScript::scriptStack().empty());

    // A script-less push is allowed; the stack cannot be listed while it is on
    // it.
    OOJSScript::pushScript(nullptr);
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == nullptr);
    OOJSScript::popScript(nullptr);
  }
}

// The crossing: the script's identity is the C++ object. The
// running stack holds it, weakly, and the Script object's private slot refers
// to it weakly.
TEST(OOJSScript, crossing) {
  ::SetUp();
  {
    std::shared_ptr<OOJSScript> script = OOJSScript::scriptWithPath(
        Script("crossing/crossing.js", kNamedScript), oo::PList());
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;

    // The C++ members answer, and the root's members reach the overrides.
    OOScript* root = script.get();
    EXPECT_EQ(script->name().value_or("<none>"), "test-script");
    EXPECT_EQ(root->name().value_or("<none>"), "test-script");
    EXPECT_EQ(root->displayName().value_or("<none>"), "test-script 1.0");
    EXPECT_EQ(Text(script->propertyNamed("counter")), "0");
    EXPECT_TRUE(script->setProperty(oo::PList(7), "counter"));
    EXPECT_EQ(Text(script->propertyNamed("counter")), "7");

    // The running stack holds the script.
    OOJSScript::pushScript(script.get());
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == script.get());
    OOJSScript::popScript(script.get());
    EXPECT_TRUE(OOJSScript::scriptStack().empty());

    // A weak push answers the script while it lives (a timer's or
    // definition's).
    const std::weak_ptr<OOJSScript> weak = script;
    OOJSScript::pushScript(weak);
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == script.get());
    OOJSScript::popScript(weak.lock().get());

    // The Script object converts back to the script (a plist Object node), and
    // its JS glue answers; any other script answers undefined.
    JSContext* context = OOJSAcquireContext();
    JSValue value = script->jsValueInContext(context);  // owned: kept alive
    EXPECT_TRUE(!JS_IsUndefined(value));
    EXPECT_TRUE(OOScriptInObjectNode(OOJSPListFromJSValue(context, value)) ==
             script.get());
    EXPECT_TRUE(script->jsClassName() == std::optional<std::string>("Script"));
    EXPECT_TRUE(
        script->jsDescription() ==
        std::optional<std::string>("[Script \"test-script\" version 1.0]"));
    const std::shared_ptr<OOScript> plain = oo::makeShared<OOScript>();
    JSValue plainValue = plain->jsValueInContext(context);
    EXPECT_TRUE(JS_IsUndefined(plainValue));
    JS_FreeValue(context, plainValue);

    // The slot does not keep the script: once it goes, the object converts to
    // nothing, and a weak push answers no script.
    script = nullptr;
    EXPECT_TRUE(weak.lock().get() == nullptr);
    EXPECT_TRUE(OOJSPListFromJSValue(context, value).isNull());
    OOJSScript::pushScript(weak);
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == nullptr);
    EXPECT_TRUE(OOJSScript::scriptStack().size() == 1 &&
             OOJSScript::scriptStack()[0] == nullptr);
    OOJSScript::popScript(nullptr);
    JS_FreeValue(context, value);
    OOJSRelinquishContext(context);
  }
}

TEST(OOJSScript, engineReset) {
  ::SetUp();
  {
    const std::shared_ptr<OOJSScript> script = OOJSScript::scriptWithPath(
        Script("reset/reset.js", kNamedScript), oo::PList());
    EXPECT_TRUE(script != nullptr);
    if (script == nullptr) return;
    oo::NotificationCenter::defaultCenter().post(
        kOOJavaScriptEngineWillResetNotificationName,
        OOJavaScriptEngine::sharedEngine());

    // Invalid from now on: no properties, no calls, and so described.
    EXPECT_TRUE(script->description().find("invalid script") != std::string::npos);
    EXPECT_TRUE(JS(script)->propertyNamed("counter").isNull());
    EXPECT_TRUE(!JS(script)->setProperty(oo::PList(1), "counter"));
    JSContext* context = OOJSAcquireContext();
    EXPECT_TRUE(!script->callMethod(OOJSID("bump"), context, NULL, 0, NULL));
    JSValue self = script->jsValueInContext(context);
    EXPECT_TRUE(JS_IsUndefined(self));
    JS_FreeValue(context, self);
    OOJSRelinquishContext(context);
    EXPECT_EQ(script->name().value_or("<none>"),
                "test-script");  // the name was kept
  }
}

// A file that does not compile is not loaded (nil), its error goes
// through the engine's error reporter, and the failure path leaves nothing
// running and nothing dangling: the next script loads and runs as before.
TEST(OOJSScript, broken) {
  ::SetUp();
  {
    EXPECT_TRUE(OOJSScript::scriptWithPath(
                 Script("broken/broken.js", "\"use strict\";\nthis.name = ;\n"),
                 oo::PList()) == nullptr);
    EXPECT_TRUE(OOJSScript::currentlyRunningScript() == nullptr);
    EXPECT_TRUE(OOJSScript::scriptStack().empty());
  }
  {
    const std::shared_ptr<OOJSScript> after = OOJSScript::scriptWithPath(
        Script("broken/after.js", kNamedScript), oo::PList());
    EXPECT_TRUE(after != nullptr);
    EXPECT_EQ(after->name().value_or("<none>"), "test-script");
  }
}

TEST(OOJSScript, cleanUp) {
  stdfs::current_path(stdfs::temp_directory_path());
  std::error_code ignored;
  stdfs::remove_all(sRoot, ignored);
  EXPECT_TRUE(true);
}
