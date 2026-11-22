/*	tests/support/oo_gtest_init.hpp
        What every Google Test program in the tree does before RUN_ALL_TESTS(),
   for oo_gtest_main.cpp's main() and for a program with a main of its own
   (oo_component.cpp). Linked through oo_gtest_dep.
*/

#pragma once

namespace oo_gtest {

// ::testing::InitGoogleTest with --gtest_fail_if_no_test_linked put first, so
// a program that links no TEST fails, while a later flag on the command line
// still wins. On return *argc and *argv hold only the arguments Google Test
// did not take.
void InitGoogleTest(int* argc, char*** argv);

// On abort(), prints "oo_gtest: abort() in case <Suite>.<name>" to stderr.
void NameCaseOnAbort();

}  // namespace oo_gtest
