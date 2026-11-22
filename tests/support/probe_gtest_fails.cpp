/*	tests/support/probe_gtest_fails.cpp
        Must fail (check_probe.py): one passing case, one failing EXPECT_EQ, whose
   message must show the expression and both values.
*/

#include <gtest/gtest.h>

TEST(probe, passes) { EXPECT_EQ(2, 1 + 1); }
TEST(probe, fails) { EXPECT_EQ(2, 1 + 2); }
