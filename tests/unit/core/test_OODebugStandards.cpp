/*	test_OODebugStandards.mm
        Unit tests for the OXP standards checks
   (src/Core/Debug/OODebugStandards.h), a Debug module file with no class.

        The checks read "enforce-oxp-standards" from the user's defaults
   (clamped to off..quit), log a deprecation or an error unless enforcement is
   off, and ask the game controller to quit in quit mode; OOEnforceStandards()
   is true from "enforce" up, and the OXP verifier mode is "warn" from then on,
   when the defaults are no longer read. The tests share that state, so they run
   in order.

        The game controller would bring the whole game into the link, so it is
   this file's stand-in: it answers sharedController() and records the contexts
   it is asked to exit with. The defaults are set in memory (setObject; nothing
   is written without synchronize()). Run: tests/run.sh unit
*/

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "GameController.h"
#include "DebugStandards.h"
#include "oofnd/Defaults.hpp"

namespace {

std::vector<std::string> sExitContexts;

}  // namespace

// C++ definitions of the
// two controller members the code under test calls, with the stand-in's
// answers. The shared controller is storage nothing constructs:
// exitAppWithContext() reads no state.
alignas(
    GameController) unsigned char sControllerStorage[sizeof(GameController)];

GameController* GameController::sharedController() {
  return reinterpret_cast<GameController*>(sControllerStorage);
}

void GameController::exitAppWithContext(const std::string& context) {
  sExitContexts.push_back(context);
}

// First: an out-of-range setting (7) is clamped to quit mode, which enforces,
// logs and asks the game to exit with the message class as its context.
TEST(OODebugStandards, quitModeFromAClampedSetting) {
  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(7));

  EXPECT_TRUE(oo::EnforceStandards());
  EXPECT_TRUE(sExitContexts.empty());

  oo::StandardsDeprecated("a deprecated thing");
  EXPECT_TRUE(sExitContexts.size() == 1 &&
           sExitContexts[0] == "oxp-standards.deprecated");

  oo::StandardsError("an OXP error");
  EXPECT_TRUE(sExitContexts.size() == 2 &&
           sExitContexts[1] == "oxp-standards.error");
}

// The setting is read on every check (OOStandardsSetup never marks it read;
// kept): each level in turn, and a negative one clamped to off.
TEST(OODebugStandards, theSettingIsReadOnEveryCheck) {
  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(0));
  EXPECT_TRUE(!oo::EnforceStandards());
  oo::StandardsError("off: nothing happens");
  EXPECT_TRUE(sExitContexts.size() == 2);

  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(-3));
  EXPECT_TRUE(!oo::EnforceStandards());

  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(1));
  EXPECT_TRUE(!oo::EnforceStandards());
  oo::StandardsDeprecated("warn: only logged");
  EXPECT_TRUE(sExitContexts.size() == 2);

  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(2));
  EXPECT_TRUE(oo::EnforceStandards());
  oo::StandardsError("enforce: only logged");
  EXPECT_TRUE(sExitContexts.size() == 2);

  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(3));
  EXPECT_TRUE(oo::EnforceStandards());
  oo::StandardsError("quit again");
  EXPECT_TRUE(sExitContexts.size() == 3 &&
           sExitContexts[2] == "oxp-standards.error");
}

// The OXP verifier runs in warn mode: nothing is enforced and nothing exits.
TEST(OODebugStandards, verifierModeWarns) {
  oo::SetStandardsForOXPVerifierMode();
  EXPECT_TRUE(!oo::EnforceStandards());

  const size_t before = sExitContexts.size();
  oo::StandardsDeprecated("only logged");
  oo::StandardsError("only logged");
  EXPECT_TRUE(sExitContexts.size() == before);

  // The verifier mode marks the setting read: the defaults stay ignored.
  oo::Defaults::standard().setObject("enforce-oxp-standards", oo::PList(3));
  EXPECT_TRUE(!oo::EnforceStandards());
}
