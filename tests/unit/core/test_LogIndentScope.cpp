/*	test_LogIndentScope.cpp
        Unit tests for oo::LogIndentScope (src/Core/LogIndentScope.h): one
   level of log indentation for a scope's lifetime, restored however the scope
   is left.
        Run: tests/run.sh unit
*/

#include <stdexcept>

#include <gtest/gtest.h>

#include "LogIndentScope.h"
#include "oofnd/Log.hpp"

TEST(LogIndentScope, indentsForTheScopeAndRestores) {
  const unsigned before = oo::log::indentLevel();
  {
    const oo::LogIndentScope indent;
    EXPECT_TRUE(oo::log::indentLevel() == before + 1);
    {
      const oo::LogIndentScope inner;
      EXPECT_TRUE(oo::log::indentLevel() == before + 2);
    }
    EXPECT_TRUE(oo::log::indentLevel() == before + 1);
  }
  EXPECT_TRUE(oo::log::indentLevel() == before);
}

static unsigned IndentedEarlyReturn(bool early) {
  const oo::LogIndentScope indent;
  if (early) return oo::log::indentLevel();
  return oo::log::indentLevel();
}

TEST(LogIndentScope, restoresOnAnEarlyReturn) {
  const unsigned before = oo::log::indentLevel();
  EXPECT_TRUE(IndentedEarlyReturn(true) == before + 1);
  EXPECT_TRUE(oo::log::indentLevel() == before);
}

TEST(LogIndentScope, restoresWhenAnExceptionLeavesTheScope) {
  const unsigned before = oo::log::indentLevel();
  bool caught = false;
  try {
    const oo::LogIndentScope indent;
    throw std::runtime_error("leave");
  } catch (const std::runtime_error&) {
    caught = true;
  }
  EXPECT_TRUE(caught);
  EXPECT_TRUE(oo::log::indentLevel() == before);
}
