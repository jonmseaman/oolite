/*	test_std_differences.cpp
        What an oofnd component does where the standard
   library has no equivalent or answers differently, for the cases no other test
   pins. Run: tests/run.sh unit
*/

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "oofnd/Exception.hpp"

// oo::Finally: @finally. C++23 has no scope guard (std::scope_exit is a TS).
TEST(std_differences, finallyRunsWhenTheScopeIsLeftNormally) {
  std::string order;
  {
    oo::Finally finally([&] { order += "finally"; });
    order += "body,";
  }
  EXPECT_EQ(order, std::string("body,finally"));
}

TEST(std_differences, finallyRunsWhenAnExceptionLeavesTheScope) {
  std::string order;
  try {
    oo::Finally finally([&] { order += "finally,"; });
    order += "body,";
    throw std::runtime_error("out");
  } catch (const std::runtime_error&) {
    order += "handler";
  }
  EXPECT_EQ(order, std::string("body,finally,handler"));
}
