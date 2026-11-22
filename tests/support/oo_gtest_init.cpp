/*	tests/support/oo_gtest_init.cpp
        oo_gtest_init.hpp's definitions.
*/

#include "oo_gtest_init.hpp"

#include <gtest/gtest.h>

#include <csignal>
#include <cstdio>
#include <vector>

namespace oo_gtest {

void InitGoogleTest(int* argc, char*** argv) {
  // 1.17 defines the flag but its headers declare no GTEST_FLAG_SET for it, so
  // it goes in as the first argument.
  static char failIfNoTestLinked[] = "--gtest_fail_if_no_test_linked";
  static std::vector<char*> args;
  args.assign(*argv, *argv + *argc);
  args.insert(args.begin() + (*argc > 0 ? 1 : 0), failIfNoTestLinked);
  args.push_back(nullptr);
  *argc += 1;
  *argv = args.data();
  ::testing::InitGoogleTest(argc, *argv);
}

void NameCaseOnAbort() {
  std::signal(SIGABRT, [](int) {
    const ::testing::TestInfo* info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (info != nullptr) {
      std::fprintf(stderr, "oo_gtest: abort() in case %s.%s\n",
                   info->test_suite_name(), info->name());
    } else {
      std::fprintf(stderr, "oo_gtest: abort() outside any case\n");
    }
    std::fflush(stderr);
  });
}

}  // namespace oo_gtest
