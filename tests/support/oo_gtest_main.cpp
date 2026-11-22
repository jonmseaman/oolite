/*	tests/support/oo_gtest_main.cpp
        The main() every Google Test program in the tree links
   (oo_gtest_main_dep), in place of gtest_main. It adds two things gtest_main
   lacks (oo_gtest_init.hpp): a program that links no TEST fails, so an empty or
   mis-registered program cannot pass vacuously; and an abort() names the case
   that was running.

        Cases run in source order (shuffle stays off). Run one case with
   --gtest_filter=<Suite>.<name>; list them with --gtest_list_tests.
*/

#include <gtest/gtest.h>

#include "oo_gtest_init.hpp"

int main(int argc, char** argv) {
  ::oo_gtest::InitGoogleTest(&argc, &argv);
  ::oo_gtest::NameCaseOnAbort();
  return RUN_ALL_TESTS();
}
