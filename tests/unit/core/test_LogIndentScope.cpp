/*	test_LogIndentScope.cpp
        Unit tests for oo::LogIndentScope (src/Core/LogIndentScope.h): one
   level of log indentation for a scope's lifetime, restored however the scope
   is left.
        Run: tests/run.sh unit
*/

#include <stdexcept>

#include "LogIndentScope.h"
#include "oo_test.hpp"
#include "oofnd/Log.hpp"

OO_TEST(indentsForTheScopeAndRestores) {
  const unsigned before = oo::log::indentLevel();
  {
    const oo::LogIndentScope indent;
    OO_CHECK(oo::log::indentLevel() == before + 1);
    {
      const oo::LogIndentScope inner;
      OO_CHECK(oo::log::indentLevel() == before + 2);
    }
    OO_CHECK(oo::log::indentLevel() == before + 1);
  }
  OO_CHECK(oo::log::indentLevel() == before);
}

static unsigned IndentedEarlyReturn(bool early) {
  const oo::LogIndentScope indent;
  if (early) return oo::log::indentLevel();
  return oo::log::indentLevel();
}

OO_TEST(restoresOnAnEarlyReturn) {
  const unsigned before = oo::log::indentLevel();
  OO_CHECK(IndentedEarlyReturn(true) == before + 1);
  OO_CHECK(oo::log::indentLevel() == before);
}

OO_TEST(restoresWhenAnExceptionLeavesTheScope) {
  const unsigned before = oo::log::indentLevel();
  bool caught = false;
  try {
    const oo::LogIndentScope indent;
    throw std::runtime_error("leave");
  } catch (const std::runtime_error&) {
    caught = true;
  }
  OO_CHECK(caught);
  OO_CHECK(oo::log::indentLevel() == before);
}

OO_TEST_MAIN()
