/*	test_FeatureConfig.cpp
        Unit tests for oo::FeatureConfig (src/Core/FeatureConfig.h).

        It pins what the config reads from Resources/Config/features.plist:
   each switch's default, the plist key that turns it on, that a missing or
   malformed file gives the defaults, and that features() returns what was
   installed. Run: tests/run.sh unit
*/

#include <expected>
#include <filesystem>
#include <string>
#include <system_error>
#include <utility>

#include "FeatureConfig.h"
#include "oo_test.hpp"
#include "oofnd/Data.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/PListParsing.hpp"

namespace {

namespace fs = oo::fs;
using oo::FeatureConfig;
using oo::PList;

// A fresh, empty directory per test.
fs::Path scratch(const char* name) {
  std::error_code ec;
  fs::Path dir = std::filesystem::temp_directory_path(ec) /
                 "core-test-feature-config" / name;
  std::filesystem::remove_all(dir, ec);
  return dir;
}

fs::Path put(const fs::Path& file, const std::string& text) {
  OO_CHECK(fs::createDirectories(file.parent_path()));
  OO_CHECK(fs::writeFile(file, oo::Data::fromString(text)));
  return file;
}

PList dict(const char* key, PList value) {
  PList::Dict d;
  d[key] = std::move(value);
  return PList(std::move(d));
}

// The checkout's Resources/Config/features.plist: up from this source file,
// else up from the working directory.
fs::Path shippedFeaturesPlist() {
  std::error_code ec;
  const fs::Path relative =
      fs::Path("Resources") / "Config" / "features.plist";
  for (fs::Path start : {std::filesystem::absolute(__FILE__, ec).parent_path(),
                         std::filesystem::current_path(ec)}) {
    for (fs::Path dir = start; !dir.empty(); dir = dir.parent_path()) {
      if (std::filesystem::exists(dir / relative, ec)) return dir / relative;
      if (dir == dir.parent_path()) break;
    }
  }
  return {};
}

}  // namespace

OO_TEST(defaultsAreOff) {
  OO_CHECK(!FeatureConfig().isDumpPlanetInfoEnabled());
}

OO_TEST(fromPListReadsDumpPlanetInfo) {
  OO_CHECK(FeatureConfig::fromPList(dict("dump-planet-info", PList(true)))
               .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromPList(dict("dump-planet-info", PList(false)))
                .isDumpPlanetInfoEnabled());
  OO_CHECK(FeatureConfig::fromPList(dict("dump-planet-info", PList("YES")))
               .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromPList(PList(PList::Dict{}))
                .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromPList(dict("other-key", PList(true)))
                .isDumpPlanetInfoEnabled());
}

OO_TEST(fromFileReadsAnOpenStepPList) {
  const fs::Path dir = scratch("openstep");
  OO_CHECK(FeatureConfig::fromFile(
               put(dir / "on.plist", "{ \"dump-planet-info\" = YES; }"))
               .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromFile(
                put(dir / "off.plist", "{ \"dump-planet-info\" = NO; }"))
                .isDumpPlanetInfoEnabled());
}

OO_TEST(fromFileGivesTheDefaultsWhenItCannotRead) {
  const fs::Path dir = scratch("unreadable");
  OO_CHECK(!FeatureConfig::fromFile(dir / "missing.plist")
                .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromFile(put(dir / "empty.plist", ""))
                .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromFile(put(dir / "garbage.plist", "{ = ;;; ("))
                .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromFile(put(dir / "array.plist", "(YES)"))
                .isDumpPlanetInfoEnabled());
  OO_CHECK(!FeatureConfig::fromFile(dir).isDumpPlanetInfoEnabled());
}

OO_TEST(shippedFileParsesAndIsOff) {
  const fs::Path shipped = shippedFeaturesPlist();
  OO_CHECK(!shipped.empty());
  fs::Result<oo::Data> bytes = fs::readFile(shipped);
  OO_CHECK(bytes.has_value());
  if (!bytes) return;
  std::expected<PList, oo::PListError> plist =
      oo::parsePropertyList(bytes->stringView());
  OO_CHECK(plist.has_value() && plist->isDict());
  if (!plist) return;
  OO_CHECK(plist->find("dump-planet-info") != nullptr);
  OO_CHECK(!FeatureConfig::fromFile(shipped).isDumpPlanetInfoEnabled());
}

OO_TEST(featuresReturnsTheInstalledConfig) {
  OO_CHECK(!oo::features().isDumpPlanetInfoEnabled());
  oo::installFeatureConfig(
      FeatureConfig::fromPList(dict("dump-planet-info", PList(true))));
  OO_CHECK(oo::features().isDumpPlanetInfoEnabled());
  oo::installFeatureConfig(FeatureConfig());
  OO_CHECK(!oo::features().isDumpPlanetInfoEnabled());
}

OO_TEST_MAIN()
