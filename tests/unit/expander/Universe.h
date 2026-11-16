// Stub for tests/unit/expander: only what
// src/Core/OOStringExpander.mm uses.
#pragma once
#include "Foundation.h"
#include "Maths.h"
#include "Types.h"
#ifdef __cplusplus
#include <optional>
#include <string>

#include "oofnd/PList.hpp"
#endif
// The system manager is C++: the bridge calls its member.
class OOHarnessSystemManager {
 public:
  Random_Seed getRandomSeedForCurrentSystem();
};
// The universe is C++: the bridge calls the members of the same names, as
// OOStringExpander.mm calls them in the game:
//   [UNIVERSE descriptions]                  -> descriptions()
//   [UNIVERSE getSystemName:s (forGalaxy:g)] -> getSystemName(s (, g))
//   [UNIVERSE systemManager]                     -> getSystemManager()
class Universe {
 public:
  const oo::PList* descriptions();
  std::optional<std::string> getSystemName(oo::SystemID sys);
  std::optional<std::string> getSystemName(oo::SystemID sys, oo::GalaxyID gal);
  OOHarnessSystemManager* getSystemManager();
};
#ifdef __cplusplus
#endif
namespace oo {

Universe* GetUniverse(void);

}  // namespace oo

#ifdef __cplusplus
#endif
#define UNIVERSE oo::GetUniverse()
