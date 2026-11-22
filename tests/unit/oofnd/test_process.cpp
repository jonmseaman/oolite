/*	test_process.cpp
        Unit tests for oofnd/Process.hpp: the NSProcessInfo
   replacements. On Windows this TU includes <windows.h> AFTER the header, which
   also proves the header's kernel32 declarations are identical redeclarations.
*/

#include <gtest/gtest.h>

#include "oofnd/Process.hpp"

#if defined(_WIN32)
#include <windows.h>  // IWYU pragma: keep
#else
#include <sys/utsname.h>
#endif

#include <cstdlib>  // IWYU pragma: keep
#include <string>
#include <thread>

namespace {

void SetEnv(const char* name, const char* value) {
#if defined(_WIN32)
  _putenv_s(name, value != nullptr ? value : "");
#else
  if (value != nullptr)
    setenv(name, value, 1);
  else
    unsetenv(name);
#endif
}

}  // namespace

TEST(process, argumentsAreEmptyUntilSet) {
  EXPECT_TRUE(oo::process::arguments().empty());
  EXPECT_TRUE(oo::process::processName().empty());
  EXPECT_TRUE(!oo::process::hasArgument("-nodust"));
}

TEST(process, argumentsAreMainsArgvIncludingArgv0) {
  const char* argv[] = {"C:\\Games\\Oolite\\oolite.exe", "-nodust", "-message",
                        "h\xC3\xA9llo world"};
  oo::process::setArguments(4, argv);
  const std::vector<std::string>& args = oo::process::arguments();
  EXPECT_EQ(args.size(), 4u);
  EXPECT_EQ(args[0], std::string("C:\\Games\\Oolite\\oolite.exe"));
  EXPECT_EQ(args[3], std::string("h\xC3\xA9llo world"));
  EXPECT_TRUE(oo::process::hasArgument("-nodust"));
  EXPECT_TRUE(oo::process::hasArgument("-message"));
  EXPECT_TRUE(!oo::process::hasArgument("-nosound"));
  EXPECT_TRUE(
      !oo::process::hasArgument("nodust"));  // exact match, as -containsObject:
}

TEST(process, setArgumentsReplacesAndToleratesNull) {
  const char* argv[] = {"oolite", nullptr};
  oo::process::setArguments(2, argv);
  EXPECT_EQ(oo::process::arguments().size(), 2u);
  EXPECT_EQ(oo::process::arguments()[1], std::string(""));
  EXPECT_TRUE(!oo::process::hasArgument("-nodust"));
}

TEST(process, processNameIsLastComponentWithoutExtension) {
  const char* a[] = {"C:\\Games\\Oolite\\oolite.exe"};
  oo::process::setArguments(1, a);
#if defined(_WIN32)
  EXPECT_EQ(oo::process::processName(), std::string("oolite"));
#endif
  const char* b[] = {"/usr/games/oolite.app/oolite"};
  oo::process::setArguments(1, b);
  EXPECT_EQ(oo::process::processName(), std::string("oolite"));
  const char* c[] = {"relname"};  // GNUstep, measured: argv[0] as typed
  oo::process::setArguments(1, c);
  EXPECT_EQ(oo::process::processName(), std::string("relname"));
  const char* d[] = {"./pi.exe"};
  oo::process::setArguments(1, d);
  EXPECT_EQ(oo::process::processName(), std::string("pi"));
}

TEST(process, processorCountIsHardwareConcurrency) {
  const unsigned hc = std::thread::hardware_concurrency();
  EXPECT_TRUE(oo::process::processorCount() >= 1u);
  if (hc != 0) EXPECT_EQ(oo::process::processorCount(), hc);
}

TEST(process, operatingSystemVersionStringIsGNUsteps) {
  const std::string v = oo::process::operatingSystemVersionString();
#if defined(_WIN32)
  const DWORD version = GetVersion();
  EXPECT_EQ(v, std::to_string(LOBYTE(LOWORD(version))) + "." +
                     std::to_string(HIBYTE(LOWORD(version))));
#else
  struct utsname uts;
  EXPECT_EQ(uname(&uts), 0);
  EXPECT_EQ(v, std::string(uts.release));
#endif
  EXPECT_TRUE(v.find('.') != std::string::npos);
}

TEST(process, envReadsTheLiveEnvironment) {
  SetEnv("OOFND_TEST_PROCESS_VAR", "plain value");
  std::optional<std::string> v = oo::env("OOFND_TEST_PROCESS_VAR");
  EXPECT_TRUE(v.has_value());
  EXPECT_EQ(v.value_or(""), std::string("plain value"));
  EXPECT_TRUE(!oo::env("OOFND_TEST_PROCESS_VAR_THAT_IS_NOT_SET").has_value());
  EXPECT_TRUE(!oo::env(nullptr).has_value());
}

#if defined(_WIN32)
TEST(process, envValuesArriveAsUTF8) {
  EXPECT_TRUE(SetEnvironmentVariableW(L"OOFND_TEST_PROCESS_WIDE", L"h\u00E9llo") !=
           0);
  _wputenv_s(L"OOFND_TEST_PROCESS_WIDE", L"h\u00E9llo");
  EXPECT_EQ(oo::env("OOFND_TEST_PROCESS_WIDE").value_or(""),
              std::string("h\xC3\xA9llo"));
}
#endif
