/*	test_OORoleSet.mm
        Unit tests for OORoleSet (src/Core/OORoleSet.h).

        It pins: parsing a role string (probabilities, shipKey roles, the
   thargon rewrite), the normalised role string, lookups, the two role orders,
   the weighted pick, the modified copies (and which of them return the receiver
   itself), equality, and the description. The checks ask the C++ class. Run:
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "RoleSet.h"

namespace {

using Roles = std::vector<std::string>;
using Probabilities = std::map<std::string, float>;

}  // namespace

TEST(OORoleSet, parseRolesFromString) {
  EXPECT_TRUE(oo::ParseRolesFromString("") == Probabilities());
  EXPECT_TRUE(
      oo::ParseRolesFromString("trader pirate(0.5)  hunter") ==
      Probabilities({{"hunter", 1.0f}, {"pirate", 0.5f}, {"trader", 1.0f}}));
  EXPECT_TRUE(oo::ParseRolesFromString("[shipKey] trader") ==
           Probabilities({{"trader", 1.0f}}));  // shipKey roles are not roles
  EXPECT_TRUE(
      oo::ParseRolesFromString("pirate(-1) trader(0)") ==
      Probabilities({{"trader", 0.0f}}));  // negative is dropped, zero kept
  EXPECT_TRUE(
      oo::ParseRolesFromString("police(x)") ==
      Probabilities({{"police", 1.0f}}));  // no number: probability stays 1
  EXPECT_TRUE(oo::ParseRolesFromString("(0.5)") ==
           Probabilities(
               {{"(0.5)", 0.5f}}));  // "(" first: the role is the whole token
  EXPECT_TRUE(oo::ParseRolesFromString("a(2) a(3)") ==
           Probabilities({{"a", 3.0f}}));  // the last one wins
}

TEST(OORoleSet, roleStringAndLookups) {
  std::shared_ptr<oo::RoleSet> set =
      oo::RoleSet::roleSetWithString("trader pirate(0.5) hunter(0.333333333)");
  EXPECT_TRUE(set != nullptr);
  EXPECT_TRUE(set->roleString() ==
           std::optional<std::string>("hunter(0.333333) pirate(0.5) trader"));
  EXPECT_TRUE(set->hasRole("pirate"));
  EXPECT_TRUE(!set->hasRole("police"));
  EXPECT_TRUE(!set->hasRole(""));
  EXPECT_TRUE(set->probabilityForRole("trader") == 1.0f);
  EXPECT_TRUE(set->probabilityForRole("pirate") == 0.5f);
  EXPECT_TRUE(set->probabilityForRole("police") == 0.0f);
  EXPECT_TRUE(
      set->rolesAndProbabilities() ==
      std::optional<Probabilities>(Probabilities(
          {{"hunter", 0.333333333f}, {"pirate", 0.5f}, {"trader", 1.0f}})));

  // roles is in byte order; sortedRoles (and roleString) ignore case.
  std::shared_ptr<oo::RoleSet> mixed = oo::RoleSet::roleSetWithString("b a C");
  EXPECT_TRUE(mixed->roles() == Roles({"C", "a", "b"}));
  EXPECT_TRUE(mixed->sortedRoles() == Roles({"a", "b", "C"}));
  EXPECT_TRUE(mixed->roleString() == std::optional<std::string>("a b C"));

  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("police", 2.0f) != nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("police", 2.0f)->roleString() ==
           std::optional<std::string>("police(2)"));
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("escort(0.25)")->roleString() ==
           std::optional<std::string>("escort(0.25)"));
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("escort", 1.0f)->roles() ==
           Roles({"escort"}));
}

TEST(OORoleSet, noRolesIsNil) {
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("") == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("[shipKey]") == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("pirate(-1)") == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("", 1.0f) == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("pirate", -1.0f) == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("pirate", -0.5f) == nullptr);
}

TEST(OORoleSet, thargonBecomesEquipment) {
  std::shared_ptr<oo::RoleSet> thargon =
      oo::RoleSet::roleSetWithString("thargon(0.3) trader");
  EXPECT_TRUE(!thargon->hasRole("thargon"));
  EXPECT_TRUE(thargon->probabilityForRole("EQ_THARGON") == 0.3f);
  EXPECT_TRUE(thargon->roleString() ==
           std::optional<std::string>("EQ_THARGON(0.3) trader"));

  // Not when EQ_THARGON is already there, nor when its probability is 0.
  std::shared_ptr<oo::RoleSet> both =
      oo::RoleSet::roleSetWithString("thargon EQ_THARGON(0.2)");
  EXPECT_TRUE(both->roles() == Roles({"EQ_THARGON", "thargon"}));
  EXPECT_TRUE(both->probabilityForRole("EQ_THARGON") == 0.2f);
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("thargon(0)")->roles() ==
           Roles({"thargon"}));
}

TEST(OORoleSet, anyRole) {
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("pirate")->anyRole() ==
           std::optional<std::string>("pirate"));
  // Zero total weight: the first role (in byte order) is selected, as selected
  // <= prob at once.
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("b(0) a(0)")->anyRole() ==
           std::optional<std::string>("a"));
  std::shared_ptr<oo::RoleSet> set =
      oo::RoleSet::roleSetWithString("trader pirate(0.5) hunter(2)");
  for (int i = 0; i < 100; i++) {
    std::optional<std::string> role = set->anyRole();
    EXPECT_TRUE(role.has_value() && set->hasRole(*role));
  }
  // Never picked: a role of weight 0 between weighted ones.
  std::shared_ptr<oo::RoleSet> weighted = oo::RoleSet::roleSetWithString("a(1) b(0) c(1)");
  for (int i = 0; i < 100; i++)
    EXPECT_TRUE(weighted->anyRole() != std::optional<std::string>("b"));
}

TEST(OORoleSet, modifiedCopies) {
  std::shared_ptr<oo::RoleSet> set = oo::RoleSet::roleSetWithString("trader pirate(0.5)");

  // Nothing to change: the receiver itself (the set is immutable).
  EXPECT_TRUE(set->roleSetWithAddedRole("trader", 0.25f) == set);
  EXPECT_TRUE(set->roleSetWithAddedRole("", 1.0f) == set);
  EXPECT_TRUE(set->roleSetWithAddedRole("hunter", -1.0f) == set);
  EXPECT_TRUE(set->roleSetWithAddedRoleIfNotSet("pirate", 0.5f) == set);
  EXPECT_TRUE(set->roleSetWithRemovedRole("police") == set);

  std::shared_ptr<oo::RoleSet> added = set->roleSetWithAddedRole("hunter", 0.75f);
  EXPECT_TRUE(added != set &&
           added->roleString() ==
               std::optional<std::string>("hunter(0.75) pirate(0.5) trader"));
  EXPECT_TRUE(set->roleString() ==
           std::optional<std::string>("pirate(0.5) trader"));  // unchanged

  std::shared_ptr<oo::RoleSet> changed =
      set->roleSetWithAddedRoleIfNotSet("pirate", 2.0f);
  EXPECT_TRUE(changed != set && changed->probabilityForRole("pirate") == 2.0f);

  std::shared_ptr<oo::RoleSet> removed = set->roleSetWithRemovedRole("pirate");
  EXPECT_TRUE(removed->roles() == Roles({"trader"}));

  // Removing the last role gives an empty set, not nil.
  std::shared_ptr<oo::RoleSet> empty = removed->roleSetWithRemovedRole("trader");
  EXPECT_TRUE(empty != nullptr && empty->roles().empty());
  EXPECT_TRUE(empty->roleString() == std::optional<std::string>(""));
  EXPECT_TRUE(empty->anyRole() == std::nullopt);
  EXPECT_TRUE(!empty->hasRole("trader"));
}

TEST(OORoleSet, equalityAndDescription) {
  std::shared_ptr<oo::RoleSet> a = oo::RoleSet::roleSetWithString("trader pirate(0.5)");
  std::shared_ptr<oo::RoleSet> b =
      oo::RoleSet::roleSetWithString("pirate(0.5)   trader(1)");
  EXPECT_TRUE(a != b && a->isEqual(b.get()) && b->isEqual(a.get()));
  EXPECT_TRUE(a->hash() == 2 && a->hash() == b->hash());
  EXPECT_TRUE(
      !a->isEqual(oo::RoleSet::roleSetWithString("trader pirate(0.25)").get()));
  EXPECT_TRUE(!a->isEqual(nullptr));

  EXPECT_TRUE(a->descriptionComponents() ==
           std::optional<std::string>("pirate(0.5) trader"));
}

TEST(OORoleSet, cxxRoleSet) {
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("") == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithString("[shipKey]") == nullptr);
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("pirate", -1.0f) == nullptr);

  std::shared_ptr<oo::RoleSet> set =
      oo::RoleSet::roleSetWithString("trader pirate(0.5) thargon(0.25)");
  EXPECT_TRUE(set != nullptr);
  EXPECT_TRUE(set->roleString() ==
           std::optional<std::string>("EQ_THARGON(0.25) pirate(0.5) trader"));
  EXPECT_TRUE(set->descriptionComponents() == set->roleString());
  EXPECT_TRUE(set->hasRole("pirate") && !set->hasRole("thargon") &&
           !set->hasRole(""));
  EXPECT_TRUE(set->probabilityForRole("pirate") == 0.5f &&
           set->probabilityForRole("police") == 0.0f);
  EXPECT_TRUE(set->roles() == Roles({"EQ_THARGON", "pirate", "trader"}));
  EXPECT_TRUE(set->sortedRoles() == Roles({"EQ_THARGON", "pirate", "trader"}));
  EXPECT_TRUE(set->rolesAndProbabilities() ==
           std::optional<Probabilities>(Probabilities(
               {{"EQ_THARGON", 0.25f}, {"pirate", 0.5f}, {"trader", 1.0f}})));
  EXPECT_TRUE(oo::RoleSet::roleSetWithRole("police", 1.0f)->anyRole() ==
           std::optional<std::string>("police"));

  // Nothing to change: the receiver itself.
  EXPECT_TRUE(set->roleSetWithAddedRole("trader", 2.0f) == set);
  EXPECT_TRUE(set->roleSetWithAddedRoleIfNotSet("pirate", 0.5f) == set);
  EXPECT_TRUE(set->roleSetWithRemovedRole("police") == set);

  std::shared_ptr<oo::RoleSet> added = set->roleSetWithAddedRole("hunter", 0.75f);
  EXPECT_TRUE(added != set && added->probabilityForRole("hunter") == 0.75f);
  EXPECT_TRUE(set->roleSetWithAddedRoleIfNotSet("pirate", 2.0f)
               ->probabilityForRole("pirate") == 2.0f);
  std::shared_ptr<oo::RoleSet> empty = oo::RoleSet::roleSetWithRole("trader", 1.0f)
                                 ->roleSetWithRemovedRole("trader");
  EXPECT_TRUE(empty != nullptr && empty->roles().empty() &&
           !empty->anyRole().has_value());

  std::shared_ptr<oo::RoleSet> same =
      oo::RoleSet::roleSetWithString("pirate(0.5) trader EQ_THARGON(0.25)");
  EXPECT_TRUE(set->isEqual(same.get()) && set->hash() == 3 && same->hash() == 3);
  EXPECT_TRUE(!set->isEqual(added.get()) && !set->isEqual(nullptr));
}
