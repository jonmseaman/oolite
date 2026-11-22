/*	test_OODebugSupport.mm
        Unit tests for the debug support's start-up
   (src/Core/Debug/OODebugSupport.h), a Debug module file with no class.

        OOInitDebugSupport() reads debugConfig.plist. When the debug OXP's
   beacon file is installed it makes the TCP console client for console-host (a
   string, or a number's string value) and console-port, and tells the monitor
   it is not using a plug-in controller; the console is activated when the
   client connected, or when always-load-debug-console is set, and then the
        monitor is given the debugger (nil if none) and the JavaScript engine
   enables the debugger statement. The stand-ins are the monitor's and client's
   C++ members that the debug support calls (the client is the C++ debugger
   itself).

        What it reaches would bring the game into the link, so this file stands
   in for it: the resource manager (the configuration and the beacon's path),
   the debug monitor, the TCP console client (which records where it was asked
   to connect, and can be made to fail) and the JavaScript engine. Run:
   tests/run.sh unit
*/

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "DebugMonitor.h"
#include "DebugSupport.h"
#include "DebugTCPConsoleClient.h"
#include "oofnd/PList.hpp"

// MARK: What the rest of the game provides
// ---------------------------------------------------------

namespace {

struct Record {
  oo::PList configuration;                // debugConfig.plist
  std::optional<std::string> beaconPath;  // DebugOXPLocatorBeacon.magic
  bool clientFails = false;
  std::vector<std::pair<std::optional<std::string>, uint16_t>> clientsMade;
  const void* lastClient = nullptr;  // the client made
  std::vector<bool> usingPlugInController;
  std::vector<const void*> debuggersSet;
  int debuggerStatementsEnabled = 0;
};

Record sRecord;

const bool kTrue = (1 == 1);  // a Bool node (OOCocoa.h defines true as 1)

}  // namespace

#include "ResourceManager.h"  // OOResourceMergeMode, as the stand-in declared it

// C++ definitions of the ResourceManager statics the code under test calls,
// with the stand-in's answers.
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& fileName, const std::optional<std::string>& folderName,
    oo::ResourceMergeMode mergeMode, bool useCache) {
  if (fileName == "debugConfig.plist" && folderName == "Config" &&
      mergeMode == oo::ResourceMergeMode::kBasic && !useCache)
    return sRecord.configuration;
  return oo::PList();
}

std::optional<std::string> ResourceManager::pathForFileNamed(
    const std::string& fileName, const std::optional<std::string>& folderName) {
  // The beacon is looked for in the folder named "nil" (sic).
  if (fileName == "DebugOXPLocatorBeacon.magic" && folderName == "nil")
    return sRecord.beaconPath;
  return std::nullopt;
}

// The engine (OOJavaScriptEngine.h), declared with the header's names and
// signatures but not its class; the stand-in's answers are kept.
class OOJavaScriptEngine {
 public:
  static OOJavaScriptEngine* sharedEngine();
  void enableDebuggerStatement();
};

OOJavaScriptEngine* OOJavaScriptEngine::sharedEngine() {
  static OOJavaScriptEngine* engine = new OOJavaScriptEngine;
  return engine;
}

void OOJavaScriptEngine::enableDebuggerStatement() {
  sRecord.debuggerStatementsEnabled++;
}

// The C++ monitor's and TCP client's members that the debug support calls; the
// client's debugger interface members are never reached (the monitor is a
// stand-in). The engine-monitor members, which give the C++ monitor its vtable
// (they are virtual); never reached here.
void oo::DebugMonitor::jsEngine(::OOJavaScriptEngine*, JSContext*,
                              const std::string&,
                              const std::optional<std::string>&, unsigned,
                              unsigned, bool, const std::string&) {}
void oo::DebugMonitor::jsEngine(::OOJavaScriptEngine*, JSContext*,
                              const std::string&,
                              const std::optional<std::string>&) {}
// The monitor's JS glue and description, also virtual; never reached here.
JSValue oo::DebugMonitor::jsValueInContext(JSContext*) { return JS_UNDEFINED; }
void oo::DebugMonitor::clearJSSelf(JSValueConst) {}
std::string oo::DebugMonitor::className() const { return "OODebugMonitor"; }
std::string oo::DebugMonitor::description() const { return "<OODebugMonitor>"; }

oo::DebugMonitor* oo::DebugMonitor::sharedDebugMonitor() {
  static oo::DebugMonitor* monitor = nullptr;
  if (monitor == nullptr) monitor = oo::test::NewRetained<oo::DebugMonitor>();
  return monitor;
}

bool oo::DebugMonitor::setDebugger(oo::DebuggerInterface* debugger) {
  sRecord.debuggersSet.push_back(debugger);
  return debugger != nullptr;
}

void oo::DebugMonitor::setUsingPlugInController(bool flag) {
  sRecord.usingPlugInController.push_back(flag);
}

std::shared_ptr<oo::DebugTCPConsoleClient> oo::DebugTCPConsoleClient::clientWithAddress(
    const std::optional<std::string>& address, uint16_t port) {
  sRecord.clientsMade.emplace_back(address, port);
  if (sRecord.clientFails) return std::shared_ptr<oo::DebugTCPConsoleClient>();
  std::shared_ptr<oo::DebugTCPConsoleClient> client =
      std::shared_ptr<oo::DebugTCPConsoleClient>(new oo::DebugTCPConsoleClient(), oo::Dispose{});
  sRecord.lastClient = client.get();
  return client;
}

namespace oo {

DebugTCPConsoleClient::~DebugTCPConsoleClient() {}

}  // namespace oo


bool oo::DebugTCPConsoleClient::connectDebugMonitor(oo::DebugMonitor*,
                                                  std::optional<std::string>*) {
  return true;
}
void oo::DebugTCPConsoleClient::disconnectDebugMonitor(
    oo::DebugMonitor*, const std::optional<std::string>&) {}
void oo::DebugTCPConsoleClient::debugMonitor(oo::DebugMonitor*, const std::string&,
                                           const std::optional<std::string>&,
                                           NSRange) {}
void oo::DebugTCPConsoleClient::debugMonitorClearConsole(oo::DebugMonitor*) {}
void oo::DebugTCPConsoleClient::debugMonitorShowConsole(oo::DebugMonitor*) {}
void oo::DebugTCPConsoleClient::debugMonitor(oo::DebugMonitor*, const oo::PList&) {}
void oo::DebugTCPConsoleClient::debugMonitor(oo::DebugMonitor*, const oo::PList&,
                                           const std::string&) {}
std::string oo::DebugTCPConsoleClient::description() const {
  return "<OODebugTCPConsoleClient>";
}

namespace {

void Reset(oo::PList::Dict configuration, bool beacon) {
  sRecord = Record();
  sRecord.configuration = oo::PList(std::move(configuration));
  if (beacon)
    sRecord.beaconPath = "AddOns/Debug.oxp/DebugOXPLocatorBeacon.magic";
}

}  // namespace

// MARK: Tests
// -------------------------------------------------------------------------------------

// No debug OXP and no always-load: nothing is set up.
TEST(OODebugSupport, nothingWithoutTheDebugOXP) {
  {
    Reset({}, false);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.empty());
    EXPECT_TRUE(sRecord.usingPlugInController.empty());
    EXPECT_TRUE(sRecord.debuggersSet.empty());
    EXPECT_TRUE(sRecord.debuggerStatementsEnabled == 0);
  }
}

// always-load-debug-console without the OXP: the console with no debugger.
TEST(OODebugSupport, alwaysLoadWithoutTheDebugOXP) {
  {
    Reset({{"always-load-debug-console", oo::PList(kTrue)}}, false);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.empty());
    EXPECT_TRUE(sRecord.debuggersSet.size() == 1 &&
             sRecord.debuggersSet[0] == nullptr);
    EXPECT_TRUE(sRecord.debuggerStatementsEnabled == 1);
  }
}

// The debug OXP: a TCP client for the configured host and port becomes the
// monitor's debugger.
TEST(OODebugSupport, theTCPConsoleClient) {
  {
    Reset({{"console-host", oo::PList(std::string("console.example"))},
           {"console-port", oo::PList(8564)}},
          true);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.size() == 1);
    if (sRecord.clientsMade.size() == 1) {
      EXPECT_TRUE(sRecord.clientsMade[0].first ==
               std::optional<std::string>("console.example"));
      EXPECT_TRUE(sRecord.clientsMade[0].second == 8564);
    }
    EXPECT_TRUE(sRecord.usingPlugInController == std::vector<bool>{false});
    EXPECT_TRUE(sRecord.lastClient != nullptr);
    EXPECT_TRUE(sRecord.debuggersSet.size() == 1 &&
             sRecord.debuggersSet[0] == sRecord.lastClient);
    EXPECT_TRUE(sRecord.debuggerStatementsEnabled == 1);
  }
}

// No host: the client's default (nullopt), and port 0; a number is read as its
// string value; anything else is no host.
TEST(OODebugSupport, theHostSetting) {
  {
    Reset({}, true);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.size() == 1 &&
             !sRecord.clientsMade[0].first.has_value() &&
             sRecord.clientsMade[0].second == 0);

    Reset({{"console-host", oo::PList(42)}}, true);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.size() == 1 &&
             sRecord.clientsMade[0].first == std::optional<std::string>("42"));

    Reset({{"console-host",
            oo::PList(oo::PList::Array{oo::PList(std::string("x"))})}},
          true);
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.size() == 1 &&
             !sRecord.clientsMade[0].first.has_value());
  }
}

// The client cannot connect: no console, unless always-load is set, and then no
// debugger.
TEST(OODebugSupport, theClientFails) {
  {
    Reset({{"console-host", oo::PList(std::string("127.0.0.1"))}}, true);
    sRecord.clientFails = true;
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.clientsMade.size() == 1);
    EXPECT_TRUE(sRecord.usingPlugInController == std::vector<bool>{false});
    EXPECT_TRUE(sRecord.debuggersSet.empty());
    EXPECT_TRUE(sRecord.debuggerStatementsEnabled == 0);

    Reset({{"always-load-debug-console", oo::PList(kTrue)}}, true);
    sRecord.clientFails = true;
    OOInitDebugSupport();
    EXPECT_TRUE(sRecord.debuggersSet.size() == 1 &&
             sRecord.debuggersSet[0] == nullptr);
    EXPECT_TRUE(sRecord.debuggerStatementsEnabled == 1);
  }
}
