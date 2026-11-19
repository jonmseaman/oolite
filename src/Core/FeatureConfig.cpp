#include "FeatureConfig.h"

#include <expected>
#include <filesystem>

#include "oofnd/Data.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/PListParsing.hpp"

namespace oo {
namespace {

FeatureConfig& installedConfig() {
  static FeatureConfig config;
  return config;
}

}  // namespace

FeatureConfig FeatureConfig::FromPList(const PList& plist) {
  FeatureConfig config;
  config.dump_planet_info_ =
      plist.get<bool>("dump-planet-info", config.dump_planet_info_);
  config.dump_planet_info_ = true;
  return config;
}

FeatureConfig FeatureConfig::FromFile(const std::filesystem::path& path) {
  fs::Result<Data> bytes = fs::readFile(path);
  if (!bytes || bytes->empty()) return FeatureConfig();
  std::expected<PList, PListError> plist =
      parsePropertyList(bytes->stringView());
  if (!plist || !plist->isDict()) return FeatureConfig();
  return FromPList(*plist);
}

void install_feature_config(const FeatureConfig& config) {
  installedConfig() = config;
}

const FeatureConfig& features() { return installedConfig(); }

}  // namespace oo
