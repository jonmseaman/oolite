/*	test_filesystem.cpp
        Unit tests for oofnd/FileSystem.hpp: oo::fs, the
   replacement for NSFileManager, the NSFileManager (OOExtensions) category and
   NSData's file I/O. Each test works in its own fresh directory under the
   system temporary directory and removes it afterwards.

        The semantics checked are the ones the header's banner promises, which
   are GNUstep's: existence follows links and never fails, creating an existing
   directory succeeds, removing a missing path fails, moving never overwrites,
   atomic writes replace, paths are UTF-8.
*/

#include <algorithm>
#include <atomic>
#include <string>
#include <system_error>
#include <vector>

#include <gtest/gtest.h>

#include "oofnd/FileSystem.hpp"

namespace {

namespace fs = oo::fs;

// A fresh, empty directory for one test, deleted with everything in it at scope
// exit.
class Scratch {
 public:
  Scratch() {
    static std::atomic<int> counter{0};
    std::error_code ec;
    root_ = std::filesystem::temp_directory_path(ec) /
            ("oofnd-fs-test-" +
             std::to_string(static_cast<long long>(
                 std::filesystem::file_time_type::clock::now()
                     .time_since_epoch()
                     .count())) +
             "-" + std::to_string(counter++));
    std::filesystem::remove_all(root_, ec);
    std::filesystem::create_directories(root_, ec);
  }
  ~Scratch() {
    std::error_code ec;
    std::filesystem::remove_all(root_, ec);
  }
  const fs::Path& root() const { return root_; }
  fs::Path operator/(const char* name) const { return root_ / name; }

 private:
  fs::Path root_;
};

bool writeText(const fs::Path& p, std::string_view text) {
  return fs::writeFile(p, oo::Data::fromString(text), fs::WriteMode::kDirect)
      .has_value();
}

std::string readText(const fs::Path& p) {
  auto r = fs::readFile(p);
  return r ? r->toString() : std::string("<error>");
}

}  // namespace

TEST(filesystem, utf8PathsRoundTrip) {
  const std::string name = "caf\xc3\xa9-\xe2\x98\x85";  // café-★
  const fs::Path p = fs::pathFromUTF8(name);
  EXPECT_EQ(fs::utf8String(p), name);

  // utf8String spells separators as GNUstep does ('/'); nativeUTF8String leaves
  // them alone.
  const fs::Path mixed = fs::pathFromUTF8("C:/Games\\Oolite") / "oolite-saves";
#if defined(_WIN32)
  EXPECT_EQ(fs::utf8String(mixed),
              std::string("C:/Games/Oolite/oolite-saves"));
  EXPECT_EQ(fs::nativeUTF8String(mixed),
              std::string("C:/Games\\Oolite\\oolite-saves"));
#else
  EXPECT_EQ(fs::utf8String(mixed),
              std::string("C:/Games\\Oolite/oolite-saves"));
#endif

  Scratch s;
  const fs::Path file = s.root() / p;
  EXPECT_TRUE(writeText(file, "x"));
  EXPECT_TRUE(fs::fileExists(file));
  auto names = fs::directoryContents(s.root());
  EXPECT_TRUE(names.has_value());
  EXPECT_TRUE(names && names->size() == 1 && names->front() == name);
}

TEST(filesystem, fileTypeDistinguishesFilesDirectoriesAndNothing) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "f.txt", "hello"));
  EXPECT_TRUE(fs::fileType(s / "f.txt") == fs::FileType::kRegular);
  EXPECT_TRUE(fs::fileType(s.root()) == fs::FileType::kDirectory);
  EXPECT_TRUE(fs::fileType(s / "missing") == fs::FileType::kNone);
  EXPECT_TRUE(fs::fileType(s / "missing" / "deeper") == fs::FileType::kNone);
  EXPECT_TRUE(fs::fileExists(s / "f.txt"));
  EXPECT_TRUE(!fs::isDirectory(s / "f.txt"));
  EXPECT_TRUE(fs::isDirectory(s.root()));
  EXPECT_TRUE(!fs::fileExists(s / "missing"));
  EXPECT_TRUE(!fs::fileExists(fs::Path()));
}

TEST(filesystem, readFileReturnsTheBytes) {
  Scratch s;
  const std::string binary(
      "\x00\x01\xff\r\n\x1a"
      "end",
      9);
  EXPECT_TRUE(fs::writeFile(s / "b.bin", oo::Data::fromString(binary),
                         fs::WriteMode::kDirect)
               .has_value());
  auto r = fs::readFile(s / "b.bin");
  EXPECT_TRUE(r.has_value());
  EXPECT_TRUE(r &&
           r->toString() == binary);  // binary mode: no CRLF or ^Z translation
  EXPECT_TRUE(fs::fileSize(s / "b.bin").value_or(0) == 9u);
}

TEST(filesystem, readFileOfALargeFileCrossesTheBufferBoundary) {
  Scratch s;
  std::string big(200 * 1024 + 17, '\0');
  for (std::size_t i = 0; i < big.size(); ++i)
    big[i] = static_cast<char>(i * 7);
  EXPECT_TRUE(writeText(s / "big", big));
  EXPECT_TRUE(readText(s / "big") == big);
}

TEST(filesystem, readFileOfAnEmptyFileIsEmptyData) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "empty", ""));
  auto r = fs::readFile(s / "empty");
  EXPECT_TRUE(r.has_value() && r->empty());
}

TEST(filesystem, readFileErrors) {
  Scratch s;
  auto missing = fs::readFile(s / "nope");
  EXPECT_TRUE(!missing.has_value());
  EXPECT_TRUE(!missing && missing.error() == std::errc::no_such_file_or_directory);
  auto dir = fs::readFile(s.root());
  EXPECT_TRUE(!dir && dir.error() == std::errc::is_a_directory);
}

TEST(filesystem, writeFileDirectAndAtomicBothReplaceExistingContents) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "a", "first, longer contents"));
  EXPECT_TRUE(writeText(s / "a", "second"));
  EXPECT_EQ(readText(s / "a"), std::string("second"));

  EXPECT_TRUE(fs::writeFile(s / "a", oo::Data::fromString("third"),
                         fs::WriteMode::kAtomic)
               .has_value());
  EXPECT_EQ(readText(s / "a"), std::string("third"));
  EXPECT_TRUE(fs::writeFile(s / "new", oo::Data::fromString("fresh"))
               .has_value());  // atomic is the default
  EXPECT_EQ(readText(s / "new"), std::string("fresh"));

  // The atomic write's temporary is gone afterwards.
  auto names = fs::directoryContents(s.root());
  EXPECT_TRUE(names && names->size() == 2);
}

TEST(filesystem, writeFileIntoAMissingDirectoryFails) {
  Scratch s;
  EXPECT_TRUE(!fs::writeFile(s / "no" / "such" / "dir.txt",
                          oo::Data::fromString("x"), fs::WriteMode::kDirect)
                .has_value());
  EXPECT_TRUE(!fs::writeFile(s / "no" / "such" / "dir.txt",
                          oo::Data::fromString("x"), fs::WriteMode::kAtomic)
                .has_value());
  auto names = fs::directoryContents(s.root());
  EXPECT_TRUE(names && names->empty());
}

TEST(filesystem, directoryContentsListsNamesOnly) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "b.txt", "b"));
  EXPECT_TRUE(writeText(s / "a.txt", "a"));
  EXPECT_TRUE(fs::createDirectories(s / "sub").has_value());
  EXPECT_TRUE(writeText(s / "sub" / "inner", "i"));
  auto names = fs::directoryContents(s.root());
  EXPECT_TRUE(names.has_value());
  if (!names) return;
  std::vector<std::string> sorted = *names;
  std::sort(sorted.begin(), sorted.end());
  EXPECT_TRUE((sorted == std::vector<std::string>{"a.txt", "b.txt", "sub"}));

  EXPECT_TRUE(!fs::directoryContents(s / "missing").has_value());
  EXPECT_TRUE(!fs::directoryContents(s / "a.txt").has_value());
}

TEST(filesystem, createDirectoriesMakesIntermediatesAndToleratesExisting) {
  Scratch s;
  const fs::Path deep = s / "x" / "y" / "z";
  EXPECT_TRUE(fs::createDirectories(deep).has_value());
  EXPECT_TRUE(fs::isDirectory(deep));
  EXPECT_TRUE(
      fs::createDirectories(deep).has_value());  // already there: still success

  EXPECT_TRUE(writeText(s / "file", "f"));
  auto blocked = fs::createDirectories(s / "file");
  EXPECT_TRUE(!blocked && blocked.error() == std::errc::not_a_directory);
  EXPECT_TRUE(!fs::createDirectories(s / "file" / "below").has_value());
}

TEST(filesystem, removeItemRemovesFilesAndTreesButNotNothing) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "f", "f"));
  EXPECT_TRUE(fs::removeItem(s / "f").has_value());
  EXPECT_TRUE(!fs::fileExists(s / "f"));

  EXPECT_TRUE(fs::createDirectories(s / "t" / "u").has_value());
  EXPECT_TRUE(writeText(s / "t" / "u" / "leaf", "l"));
  EXPECT_TRUE(fs::removeItem(s / "t").has_value());
  EXPECT_TRUE(!fs::fileExists(s / "t"));

  auto missing = fs::removeItem(s / "t");
  EXPECT_TRUE(!missing && missing.error() == std::errc::no_such_file_or_directory);
}

TEST(filesystem, moveItemRenamesButNeverOverwrites) {
  Scratch s;
  EXPECT_TRUE(writeText(s / "Latest.log", "new"));
  EXPECT_TRUE(fs::moveItem(s / "Latest.log", s / "Previous.log").has_value());
  EXPECT_TRUE(!fs::fileExists(s / "Latest.log"));
  EXPECT_EQ(readText(s / "Previous.log"), std::string("new"));

  EXPECT_TRUE(writeText(s / "Latest.log", "newer"));
  auto clash = fs::moveItem(s / "Latest.log", s / "Previous.log");
  EXPECT_TRUE(!clash && clash.error() == std::errc::file_exists);
  EXPECT_EQ(readText(s / "Previous.log"), std::string("new"));
  EXPECT_EQ(readText(s / "Latest.log"), std::string("newer"));

  auto missing = fs::moveItem(s / "absent", s / "elsewhere");
  EXPECT_TRUE(!missing && missing.error() == std::errc::no_such_file_or_directory);

  EXPECT_TRUE(fs::createDirectories(s / "d1" / "inside").has_value());
  EXPECT_TRUE(fs::moveItem(s / "d1", s / "d2").has_value());
  EXPECT_TRUE(fs::isDirectory(s / "d2" / "inside"));
}

TEST(filesystem, currentDirectoryCanBeChangedAndRestored) {
  Scratch s;
  auto original = fs::currentDirectory();
  EXPECT_TRUE(original.has_value());
  if (!original) return;
  EXPECT_TRUE(fs::setCurrentDirectory(s.root()).has_value());
  auto now = fs::currentDirectory();
  EXPECT_TRUE(now && std::filesystem::equivalent(*now, s.root()));
  EXPECT_TRUE(writeText("relative.txt", "r"));
  EXPECT_TRUE(fs::fileExists(s / "relative.txt"));
  EXPECT_TRUE(!fs::setCurrentDirectory(s / "missing").has_value());
  EXPECT_TRUE(fs::setCurrentDirectory(*original).has_value());
  auto back = fs::currentDirectory();
  EXPECT_TRUE(back && *back == *original);
}

TEST(filesystem, freeSpaceAndFileSize) {
  Scratch s;
  auto space = fs::freeSpace(s.root());
  EXPECT_TRUE(space.has_value() && *space > 0);
  EXPECT_TRUE(!fs::fileSize(s / "missing").has_value());
  EXPECT_TRUE(writeText(s / "five", "12345"));
  EXPECT_TRUE(fs::fileSize(s / "five").value_or(0) == 5u);
}
