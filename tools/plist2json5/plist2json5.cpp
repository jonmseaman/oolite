/*	tools/plist2json5/plist2json5.cpp
        Converts property lists to JSON5 (Plist2JSON5.hpp has the rules).

            plist2json5 [--strings | --typed] [--check | --stdout] <path>...
            plist2json5 --parse <path>...

        Each <path> is a .plist file or a directory searched for them. Every
   conversion is read back and compared with the original before anything is
   written. Without --check or --stdout, <name>.json5 is written beside
   <name>.plist (replacing one already there). --check writes nothing and
   reports each file; --stdout prints one file's JSON5. --strings (the default)
   keeps every scalar a string; --typed writes plain numbers and booleans as
   JSON5 numbers and booleans, which changes what scripts see.

        --parse converts nothing: each <path> is a .json5 file or a directory
   searched for them, and each file is read with the game's JSON5 reader.

        Exit status: 0 when every file converted and checked (or, for --parse,
   read), 1 otherwise, 2 for a usage error.
*/

#include "Plist2JSON5.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "oofnd/PListFile.hpp"

namespace {

namespace stdfs = std::filesystem;

std::optional<std::string> readFile(const stdfs::path& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return std::nullopt;
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

bool writeFile(const stdfs::path& path, std::string_view text) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(text.data(), static_cast<std::streamsize>(text.size()));
  return static_cast<bool>(out);
}

bool isPList(const stdfs::path& path) {
  return oo::json5PathFor(path.generic_string()).has_value();
}

bool isJSON5(const stdfs::path& path) {
  return oo::isJSON5Path(path.generic_string());
}

int usage() {
  std::fputs(
      "usage: plist2json5 [--strings | --typed] [--check | --stdout] "
      "<path>...\n"
      "       plist2json5 --parse <path>...\n",
      stderr);
  return 2;
}

// The files `inputs` name: each a file, or a directory searched for the files
// `wanted` accepts. Sorted.
std::vector<stdfs::path> filesIn(const std::vector<stdfs::path>& inputs,
                                 bool (*wanted)(const stdfs::path&)) {
  std::vector<stdfs::path> files;
  for (const stdfs::path& input : inputs) {
    std::error_code ec;
    if (stdfs::is_directory(input, ec)) {
      for (const stdfs::directory_entry& e :
           stdfs::recursive_directory_iterator(input, ec)) {
        if (e.is_regular_file() && wanted(e.path())) files.push_back(e.path());
      }
    } else {
      files.push_back(input);
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

// --parse: every JSON5 file reads. The exit status.
int parseJSON5Files(const std::vector<stdfs::path>& inputs) {
  const std::vector<stdfs::path> files = filesIn(inputs, isJSON5);
  if (files.empty()) {
    std::fputs("plist2json5: no .json5 files found\n", stderr);
    return 1;
  }
  int failures = 0;
  for (const stdfs::path& file : files) {
    const std::string name = file.generic_string();
    const std::optional<std::string> bytes = readFile(file);
    if (!bytes) {
      std::fprintf(stderr, "FAIL %s: cannot read the file\n", name.c_str());
      ++failures;
      continue;
    }
    const std::expected<oo::PList, oo::PListError> tree =
        oo::parseJSON5PList(*bytes);
    if (!tree) {
      std::fprintf(stderr, "FAIL %s: %s\n", name.c_str(),
                   tree.error().message.c_str());
      ++failures;
      continue;
    }
    std::printf("ok   %s\n", name.c_str());
  }
  std::printf("plist2json5: %zu files, %d failed\n", files.size(), failures);
  return failures == 0 ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
  plist2json5::Mode mode = plist2json5::Mode::strings;
  bool checkOnly = false;
  bool toStdout = false;
  bool parseOnly = false;
  bool modeGiven = false;
  std::vector<stdfs::path> inputs;
  for (int i = 1; i < argc; ++i) {
    const std::string_view arg = argv[i];
    if (arg == "--strings") {
      mode = plist2json5::Mode::strings;
      modeGiven = true;
    } else if (arg == "--typed") {
      mode = plist2json5::Mode::typed;
      modeGiven = true;
    } else if (arg == "--parse") {
      parseOnly = true;
    } else if (arg == "--check") {
      checkOnly = true;
    } else if (arg == "--stdout") {
      toStdout = true;
    } else if (arg.starts_with("--")) {
      return usage();
    } else {
      inputs.emplace_back(argv[i]);
    }
  }
  if (inputs.empty() || (checkOnly && toStdout)) return usage();
  if (parseOnly) {
    if (checkOnly || toStdout || modeGiven) return usage();
    return parseJSON5Files(inputs);
  }

  const std::vector<stdfs::path> files = filesIn(inputs, isPList);
  if (files.empty()) {
    std::fputs("plist2json5: no .plist files found\n", stderr);
    return 1;
  }
  if (toStdout && files.size() != 1) return usage();

  int failures = 0;
  for (const stdfs::path& file : files) {
    const std::string name = file.generic_string();
    const std::optional<std::string> bytes = readFile(file);
    if (!bytes) {
      std::fprintf(stderr, "FAIL %s: cannot read the file\n", name.c_str());
      ++failures;
      continue;
    }
    std::expected<std::string, std::string> json5 =
        plist2json5::convert(*bytes, mode);
    std::string problem = json5 ? plist2json5::check(*bytes, *json5, mode)
                                : "cannot convert: " + json5.error();
    if (!problem.empty()) {
      std::fprintf(stderr, "FAIL %s: %s\n", name.c_str(), problem.c_str());
      ++failures;
      continue;
    }
    if (toStdout) {
      std::fwrite(json5->data(), 1, json5->size(), stdout);
    } else if (checkOnly) {
      std::printf("ok   %s\n", name.c_str());
    } else {
      const std::optional<std::string> target = oo::json5PathFor(name);
      if (!target || !writeFile(stdfs::path(*target), *json5)) {
        std::fprintf(stderr, "FAIL %s: cannot write the JSON5 file\n",
                     name.c_str());
        ++failures;
        continue;
      }
      std::printf("wrote %s\n", target->c_str());
    }
  }
  if (!toStdout)
    std::printf("plist2json5: %zu files, %d failed\n", files.size(), failures);
  return failures == 0 ? 0 : 1;
}
