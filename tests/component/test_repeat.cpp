/*	tests/component/test_repeat.cpp
        The same seed gives the same game (SC-002, data-model.md, Scenario
   digest): test_s1_police_kills_pirate is run twice, each writing the
   scenario digest of every step with --digest-out, and the two files must
   be the same. A difference is reported at the first step that differs.
   repeat_100.sh does the same a hundred times.
   Run: tests/run.sh component --cpp -k repeat
*/

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "oo_component.hpp"

namespace {

constexpr const char* kScenario = "test_s1_police_kills_pirate";

std::vector<std::string> Lines(const std::string& path) {
  std::vector<std::string> lines;
  std::ifstream in(path);
  for (std::string line; std::getline(in, line);) lines.push_back(line);
  return lines;
}

}  // namespace

TEST(repeat, two_runs_of_s1_give_the_same_digest_at_every_step) {
  const std::string program = oo::component::siblingProgram(kScenario);
  const std::string base = oo::component::programPath();
  std::vector<std::vector<std::string>> runs;
  for (int run = 1; run <= 2; ++run) {
    const std::string digests = base + ".run" + std::to_string(run) + ".digests";
    const int status = oo::component::runChild(program, {"--digest-out", digests},
                                               "", "");
    std::printf("run %d: %s exited %d\n", run, kScenario, status);
    EXPECT_EQ(status, 0);
    runs.push_back(Lines(digests));
    std::remove(digests.c_str());
  }
  EXPECT_TRUE(!runs[0].empty());
  std::printf("%zu and %zu steps\n", runs[0].size(), runs[1].size());
  EXPECT_EQ(runs[0].size(), runs[1].size());
  const std::size_t n = runs[0].size() < runs[1].size() ? runs[0].size() : runs[1].size();
  for (std::size_t i = 0; i < n; ++i) {
    if (runs[0][i] != runs[1][i]) {
      std::printf("first difference at line %zu: '%s' against '%s'\n", i + 1,
                  runs[0][i].c_str(), runs[1][i].c_str());
      EXPECT_TRUE(runs[0][i] == runs[1][i]);
      return;
    }
  }
}

OO_COMPONENT_MAIN()
