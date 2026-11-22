/*	tests/support/oo_gtest.hpp
        Expression forms of EXPECT_TRUE and EXPECT_EQ, for the few checks whose
   result a test uses as a value (bool ok = ...; if (...) ...). Like
   EXPECT_*, they are non-fatal: a failure is reported at the caller's file and
   line and the case goes on. Each operand is evaluated once.

            #include "oo_gtest.hpp"
            if (OO_EXPECT_EQ_V(parse(text).size(), 2u)) { ... }
*/

#pragma once

#include <gtest/gtest.h>

namespace oo_gtest {

inline bool expectTrue(bool ok, const char* expr, const char* file, int line) {
  if (!ok) ADD_FAILURE_AT(file, line) << "Value of: " << expr << "\n  Actual: false\nExpected: true";
  return ok;
}

template <typename A, typename B>
bool expectEq(const A& a, const B& b, const char* aExpr, const char* bExpr,
              const char* file, int line) {
  const bool ok = static_cast<bool>(a == b);
  if (!ok) {
    ADD_FAILURE_AT(file, line) << "Expected equality of these values:\n  " << aExpr
                               << "\n    Which is: " << ::testing::PrintToString(a)
                               << "\n  " << bExpr
                               << "\n    Which is: " << ::testing::PrintToString(b);
  }
  return ok;
}

}  // namespace oo_gtest

#define OO_EXPECT_V(cond) \
  ::oo_gtest::expectTrue(static_cast<bool>(cond), #cond, __FILE__, __LINE__)
#define OO_EXPECT_EQ_V(a, b) \
  ::oo_gtest::expectEq((a), (b), #a, #b, __FILE__, __LINE__)
