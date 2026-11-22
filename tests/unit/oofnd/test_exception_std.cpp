/*	test_exception_std.cpp
        Unit tests for oo::Exception as a standard exception
   (oofnd/Exception.hpp): it is caught by a std::exception handler, what() is its
   reason, and an oo::Exception handler before it still sees the name and the
   reason unchanged. Run: tests/run.sh unit
*/

#include <exception>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "oofnd/Exception.hpp"

static_assert(std::is_base_of_v<std::exception, oo::Exception>);

TEST(exception_std, caughtAsStdExceptionWithTheReason) {
  bool caught = false;
  try {
    throw oo::Exception("OOTestException", "the reason, in UTF-8: \xc3\xa9");
  } catch (const std::exception& e) {
    caught = true;
    EXPECT_EQ(std::string(e.what()),
                std::string("the reason, in UTF-8: \xc3\xa9"));
  }
  EXPECT_TRUE(caught);
}

TEST(exception_std, ooHandlerFirstStillSeesNameAndReason) {
  int which = 0;
  try {
    throw oo::Exception("OOTestException", "why");
  } catch (const oo::Exception& e) {
    which = 1;
    EXPECT_EQ(std::string(e.name()), std::string("OOTestException"));
    EXPECT_EQ(std::string(e.reason()), std::string("why"));
  } catch (const std::exception&) {
    which = 2;
  }
  EXPECT_EQ(which, 1);
}

TEST(exception_std, nullNameAndReasonBecomeEmpty) {
  const oo::Exception e(nullptr, nullptr);
  EXPECT_EQ(std::string(e.name()), std::string());
  EXPECT_EQ(std::string(e.what()), std::string());
}
