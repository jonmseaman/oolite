/*	tests/support/probe_gtest_aborts.cpp
        Must fail (check_probe.py), naming the case on stderr: abort() in a case.
*/

#include <gtest/gtest.h>

#include <cstdlib>

TEST(probe, aborts) { std::abort(); }
