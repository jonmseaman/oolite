/*	tests/support/test_gtest_harness.cpp
        The tests of oo_gtest.hpp and of how the tree uses Google Test: cases run
   in source order, and the value forms report once, at the caller, and return
   the result. A failing program is probe_gtest_fails, run by check_probe.py.
   Run: meson test -C build/meson_<type> --suite gtest
*/

#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "oo_gtest.hpp"

namespace {

std::vector<std::string>& Ran() {
  static std::vector<std::string> ran;
  return ran;
}

int g_calls = 0;
int CountedOne() {
  ++g_calls;
  return 1;
}

// Runs body with failures captured instead of reported, and returns them.
template <typename Body>
std::vector<::testing::TestPartResult> Captured(Body body) {
  ::testing::TestPartResultArray results;
  {
    ::testing::ScopedFakeTestPartResultReporter reporter(
        ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD,
        &results);
    body();
  }
  std::vector<::testing::TestPartResult> out;
  for (int i = 0; i < results.size(); ++i) out.push_back(results.GetTestPartResult(i));
  return out;
}

}  // namespace

TEST(gtest_harness, first) { Ran().push_back("first"); }
TEST(gtest_harness, second) { Ran().push_back("second"); }

TEST(gtest_harness, cases_run_in_source_order) {
  const std::vector<std::string> expected{"first", "second"};
  EXPECT_EQ(Ran(), expected);
}

TEST(gtest_harness, expect_v_returns_the_result_and_reports_once_at_the_caller) {
  bool trueResult = false;
  bool falseResult = true;
  int failLine = 0;
  const std::vector<::testing::TestPartResult> results = Captured([&] {
    trueResult = OO_EXPECT_V(1 + 1 == 2);
    failLine = __LINE__ + 1;
    falseResult = OO_EXPECT_V(1 + 1 == 3);
  });
  EXPECT_TRUE(trueResult);
  EXPECT_FALSE(falseResult);
  ASSERT_EQ(results.size(), 1u);
  EXPECT_TRUE(results[0].nonfatally_failed());
  EXPECT_EQ(results[0].line_number(), failLine);
  EXPECT_NE(std::string(results[0].message()).find("1 + 1 == 3"), std::string::npos);
}

TEST(gtest_harness, expect_eq_v_evaluates_once_and_shows_both_values) {
  g_calls = 0;
  bool equal = false;
  bool unequal = true;
  const std::vector<::testing::TestPartResult> results = Captured([&] {
    equal = OO_EXPECT_EQ_V(CountedOne(), 1);
    unequal = OO_EXPECT_EQ_V(CountedOne(), 2);
  });
  EXPECT_TRUE(equal);
  EXPECT_FALSE(unequal);
  EXPECT_EQ(g_calls, 2);
  ASSERT_EQ(results.size(), 1u);
  const std::string message = results[0].message();
  EXPECT_NE(message.find("CountedOne()"), std::string::npos);
  EXPECT_NE(message.find("Which is: 1"), std::string::npos);
  EXPECT_NE(message.find("Which is: 2"), std::string::npos);
}
