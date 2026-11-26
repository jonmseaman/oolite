#pragma once

#include <filesystem>

#include "oofnd/PList.hpp"

namespace oo {

// What the game turns on at run time. Built once from
// Resources/Config/features.plist in main() and read-only after that.
class FeatureConfig {
 public:
  FeatureConfig() = default;

  // Keys missing from the dict keep their defaults.
  static FeatureConfig FromPList(const PList& plist);
  // A missing, unreadable or malformed file gives the defaults.
  static FeatureConfig FromFile(const std::filesystem::path& path);

  bool IsDumpPlanetInfoEnabled() const { return dump_planet_info_; }

 private:
  bool dump_planet_info_ = false;
};

// The process's config. Call installFeatureConfig once, in main() before
// other threads start; features() returns the defaults until then.
// TODO: Use dependency injection in the future instead of singletons.
void install_feature_config(const FeatureConfig& config);
const FeatureConfig& features();

}  // namespace oo
