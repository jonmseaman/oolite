/*	test_OOPListSchemaVerifier.mm
        Unit tests for OOPListSchemaVerifier
   (src/Core/OXPVerifier/OOPListSchemaVerifier.h).

        It pins what the verifier answers to its one caller
   (OOCheckShipDataPListVerifierStage): nil for a null schema, the delegate,
   verifying valid and invalid property lists (the failure the delegate is told
   about: code, reason, key path and expected type), $definitions macros and a
   bad schema, delegated types (answered, refused, throwing, or a delegate that
   does not handle them), a delegate that stops at the first failure, no failure
   handler at all, and +descriptionForKeyPath:. The delegate is the C++
   interface OOPListSchemaVerifierDelegate; the test delegate records what it is
   told (reference counted, so that it is still checked not retained). A
   delegate that answers nothing cannot be written in C++, so
   aDelegateThatHandlesNothing verifies with no delegate. The C++ API checks are
   delegateIsHandedTheVerifier. Run: tests/run.sh unit
   test_OOPListSchemaVerifier
*/

#include <cstdlib>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Description.h"
#include "PListSchemaVerifier.h"
#include "StringParsing.h"
#include "oofnd/PListParsing.hpp"
#include "oofnd/Exception.hpp"

// Link stubs: OOPListGameTypes.mm reaches OOStringParsing.mm's scanners for
// vector and quaternion values, whose object links the game. No schema here
// uses those types, so each stub aborts.
std::optional<Vector> ScanVectorFromString(const std::optional<std::string>&) {
  std::abort();
}
std::optional<HPVector> ScanHPVectorFromString(const std::optional<std::string>&) {
  std::abort();
}
std::optional<Quaternion> ScanQuaternionFromString(const std::optional<std::string>&) {
  std::abort();
}

namespace {

oo::PList Parse(const char* text) {
  auto result = oo::parsePropertyList(text);
  EXPECT_TRUE(result.has_value());
  return result ? *result : oo::PList();
}

std::string KeyPathText(const oo::PList& keyPath) {
  return oo::PListSchemaVerifier::descriptionForKeyPath(keyPath).value_or(
      "(nullopt)");
}

// One failure as the delegate saw it: "<code> <key path> | <reason> | <expected
// type>".
std::string FailureText(const oo::PListSchemaVerifierError& error,
                        const oo::PList& localSchema) {
  const oo::PList* keyPath = error.userInfo.find(kPListKeyPathErrorKey);
  return std::to_string(error.code) + " " +
         KeyPathText(keyPath != nullptr ? *keyPath : oo::PList()) + " | " +
         error.failureReason.value_or("(nullopt)") + " | " +
         oo::DescriptionOf(localSchema);
}

}  // namespace

// A delegate that records what it is told. Delegated types answer
// `delegatedAnswer`, or raise. (Reference counted, so the test can check the
// verifier does not retain it.)
class TestSchemaDelegate : public oo::Object,
                           public oo::PListSchemaVerifierDelegate {
 public:
  std::vector<std::string> failures;
  std::vector<std::string> delegated;  // "<name> <type key> <key path> <value>"
  std::vector<oo::PListSchemaVerifier*>
      verifiers;  // the verifier each call was handed
  bool delegatedAnswer = false;
  bool delegatedRaises = false;
  bool delegatedError = false;
  bool continueAfterFailure = false;

  bool verifierTestProperty(
      oo::PListSchemaVerifier* verifier, const oo::PList& rootPList,
      const std::string& name, const oo::PList& subPList,
      const oo::PList& keyPath, const oo::PList& typeKey,
      std::optional<oo::PListSchemaVerifierError>* outError) override {
    (void)rootPList;
    verifiers.push_back(verifier);
    delegated.push_back(name + " " + oo::DescriptionOf(typeKey) + " " +
                        KeyPathText(keyPath) + " " +
                        oo::DescriptionOf(subPList));
    if (delegatedRaises)
      oo::RaiseException(oo::GenericException, "delegate refuses %s", name.c_str());
    if (delegatedError && outError != nullptr)
      *outError = oo::PListSchemaVerifierError{
          "test.domain", 42, std::string("delegate's own error"), oo::PList()};
    return delegatedAnswer;
  }

  bool verifierFailedForProperty(oo::PListSchemaVerifier* verifier,
                                 const oo::PList& rootPList,
                                 const std::string& name,
                                 const oo::PList& subPList,
                                 const oo::PListSchemaVerifierError& error,
                                 const oo::PList& localSchema) override {
    (void)rootPList;
    (void)subPList;
    verifiers.push_back(verifier);
    failures.push_back(name + ": " + FailureText(error, localSchema));
    return continueAfterFailure;
  }
};

namespace {

const char* const kShipSchema =
    "{ type = dictionary; allowOthers = NO; requiredKeys = ( name ); schema = {"
    "  name = string; count = positiveInteger; roles = { type = array; "
    "valueType = $role; };"
    "  scale = { type = float; minimum = 0.5; maximum = 2; };"
    "}; $definitions = { $role = { type = string; }; }; }";

oo::PList GoodShip() {
  return oo::PList(oo::PList::Dict{
      {"name", oo::PList("Cobra")},
      {"count", oo::PList(3)},
      {"roles",
       oo::PList(oo::PList::Array{oo::PList("trader"), oo::PList("hunter")})},
      {"scale", oo::PList(1.5)},
  });
}

}  // namespace

TEST(OOPListSchemaVerifier, nullSchemaGivesNil) {
  EXPECT_TRUE(oo::PListSchemaVerifier::verifierWithSchema(oo::PList()) == nullptr);
  const std::shared_ptr<oo::PListSchemaVerifier> allocated =
      oo::PListSchemaVerifier::initWithSchema(oo::PList());
  EXPECT_TRUE(allocated == nullptr);
  EXPECT_TRUE(oo::PListSchemaVerifier::verifierWithSchema(Parse("string")) !=
           nullptr);
}

TEST(OOPListSchemaVerifier, keepsItsDelegate) {
  {
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(Parse(kShipSchema));
    EXPECT_TRUE(verifier->delegate() == nullptr);
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    verifier->setDelegate(delegate.get());
    EXPECT_TRUE(verifier->delegate() == delegate.get());
    EXPECT_TRUE(delegate->weak_from_this().use_count() == 1);  // not retained
    verifier->setDelegate(nullptr);
    EXPECT_TRUE(verifier->delegate() == nullptr);
  }
}

TEST(OOPListSchemaVerifier, acceptsAValidPropertyList) {
  {
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(Parse(kShipSchema));
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    verifier->setDelegate(delegate.get());
    EXPECT_TRUE(verifier->verifyPropertyList(GoodShip(), "cobra"));
    EXPECT_TRUE(delegate->failures.empty());
    EXPECT_TRUE(delegate->verifiers.empty());
  }
}

TEST(OOPListSchemaVerifier, reportsEachFailureToTheDelegate) {
  {
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(Parse(kShipSchema));
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    delegate->continueAfterFailure = true;
    verifier->setDelegate(delegate.get());

    oo::PList::Dict ship = *GoodShip().getIf<oo::PList::Dict>();
    ship["count"] = oo::PList(-2);
    ship["roles"] =
        oo::PList(oo::PList::Array{oo::PList("trader"), oo::PList(7)});
    ship["scale"] = oo::PList(3.0);
    ship["colour"] = oo::PList("red");
    EXPECT_TRUE(!verifier->verifyPropertyList(oo::PList(ship), "bad"));

    // As found before the conversion: count = -2 is not reported as a
    // positiveInteger failure (pinned, not judged).

    const std::vector<std::string> expected = {
        "bad: 9 root | Unpermitted key \"colour\" in dictionary. | "
        "{\"$definitions\" = {\"$role\" = {type = string; }; }; allowOthers = "
        "NO; requiredKeys = (name); schema = {count = positiveInteger; name = "
        "string; roles = {type = array; valueType = \"$role\"; }; scale = "
        "{maximum = 2; minimum = \"0.5\"; type = float; }; }; type = "
        "dictionary; }",
        "bad: 2 roles[1] | Expected string, found number. | $role",
        "bad: 4 scale | Number is too large (3, maximum is 2). | {maximum = 2; "
        "minimum = \"0.5\"; type = float; }",
    };
    EXPECT_TRUE(delegate->failures == expected);
    for (oo::PListSchemaVerifier* seen : delegate->verifiers)
      EXPECT_TRUE(seen == verifier.get());
  }
}

TEST(OOPListSchemaVerifier, stopsWhenTheDelegateSaysSo) {
  {
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(Parse(kShipSchema));
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    delegate->continueAfterFailure = false;
    verifier->setDelegate(delegate.get());

    oo::PList::Dict ship = *GoodShip().getIf<oo::PList::Dict>();
    ship.erase("name");
    ship["count"] = oo::PList(-2);
    EXPECT_TRUE(!verifier->verifyPropertyList(oo::PList(ship), "stop"));
    EXPECT_EQ(delegate->failures.size(), 1u);
    EXPECT_TRUE(
        !delegate->failures.empty() &&
        delegate->failures[0] ==
            "stop: 10 root | Required keys (name) missing from dictionary. | "
            "{\"$definitions\" = {\"$role\" = {type = string; }; }; "
            "allowOthers = NO; requiredKeys = (name); schema = {count = "
            "positiveInteger; name = string; roles = {type = array; valueType "
            "= \"$role\"; }; scale = {maximum = 2; minimum = \"0.5\"; type = "
            "float; }; }; type = dictionary; }");
  }
}

TEST(OOPListSchemaVerifier, reportsABadSchema) {
  {
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    delegate->continueAfterFailure = true;

    const std::shared_ptr<oo::PListSchemaVerifier> undefinedMacro =
        oo::PListSchemaVerifier::verifierWithSchema(
            Parse("{ type = $nothing; }"));
    undefinedMacro->setDelegate(delegate.get());
    EXPECT_TRUE(!undefinedMacro->verifyPropertyList(oo::PList("x"), "macro"));

    const std::shared_ptr<oo::PListSchemaVerifier> unknownType =
        oo::PListSchemaVerifier::verifierWithSchema(Parse("{ type = gizmo; }"));
    unknownType->setDelegate(delegate.get());
    EXPECT_TRUE(!unknownType->verifyPropertyList(oo::PList("x"), "unknown"));

    const std::shared_ptr<oo::PListSchemaVerifier> noType =
        oo::PListSchemaVerifier::verifierWithSchema(Parse("{ minimum = 1; }"));
    noType->setDelegate(delegate.get());
    EXPECT_TRUE(!noType->verifyPropertyList(oo::PList("x"), "untyped"));

    const std::vector<std::string> expected = {
        "macro: 102 root | Bad schema: reference to undefined macro "
        "\"$nothing\". | {type = \"$nothing\"; }",
        "unknown: 103 root | Bad schema: unknown type \"gizmo\". | {type = "
        "gizmo; }",
        "untyped: 101 root | Bad schema: invalid type specifier for path root "
        "(no type specified). | {minimum = 1; }",
    };
    EXPECT_TRUE(delegate->failures == expected);
  }
}

TEST(OOPListSchemaVerifier, asksTheDelegateAboutDelegatedTypes) {
  {
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(
            Parse("{ type = array; valueType = { type = delegatedType; "
                  "baseType = string; key = shipKey; }; }"));
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    delegate->continueAfterFailure = true;
    verifier->setDelegate(delegate.get());
    const oo::PList list(
        oo::PList::Array{oo::PList("adder"), oo::PList("viper")});

    delegate->delegatedAnswer = true;
    EXPECT_TRUE(verifier->verifyPropertyList(list, "yes"));

    delegate->delegatedAnswer = false;
    EXPECT_TRUE(!verifier->verifyPropertyList(list, "no"));

    delegate->delegatedAnswer = true;
    delegate->delegatedError = true;
    EXPECT_TRUE(!verifier->verifyPropertyList(list, "error"));

    delegate->delegatedError = false;
    delegate->delegatedRaises = true;
    EXPECT_TRUE(!verifier->verifyPropertyList(list, "raises"));

    const std::vector<std::string> expectedDelegated = {
        "yes shipKey [0] adder",    "yes shipKey [1] viper",
        "no shipKey [0] adder",     "no shipKey [1] viper",
        "error shipKey [0] adder",  "error shipKey [1] viper",
        "raises shipKey [0] adder", "raises shipKey [1] viper",
    };
    const std::vector<std::string> expectedFailures = {
        "no: 13 [0] | Value at [0] does not match delegated type \"shipKey\". "
        "| {baseType = string; key = shipKey; type = delegatedType; }",
        "no: 13 [1] | Value at [1] does not match delegated type \"shipKey\". "
        "| {baseType = string; key = shipKey; type = delegatedType; }",
        "error: 13 [0] | Value at [0] does not match delegated type "
        "\"shipKey\". | {baseType = string; key = shipKey; type = "
        "delegatedType; }",
        "error: 13 [1] | Value at [1] does not match delegated type "
        "\"shipKey\". | {baseType = string; key = shipKey; type = "
        "delegatedType; }",
        "raises: 13 [0] | Value at [0] does not match delegated type "
        "\"shipKey\". | {baseType = string; key = shipKey; type = "
        "delegatedType; }",
        "raises: 13 [1] | Value at [1] does not match delegated type "
        "\"shipKey\". | {baseType = string; key = shipKey; type = "
        "delegatedType; }",
    };
    EXPECT_TRUE(delegate->delegated == expectedDelegated);
    EXPECT_TRUE(delegate->failures == expectedFailures);
    for (oo::PListSchemaVerifier* seen : delegate->verifiers)
      EXPECT_TRUE(seen == verifier.get());
  }
}

TEST(OOPListSchemaVerifier, aDelegateThatHandlesNothing) {
  {
    // Delegated types pass; a failure stops verification.
    const std::shared_ptr<oo::PListSchemaVerifier> verifier =
        oo::PListSchemaVerifier::verifierWithSchema(
            Parse("{ type = array; valueType = { type = delegatedType; "
                  "baseType = string; key = shipKey; }; }"));
    verifier->setDelegate(nullptr);
    EXPECT_TRUE(verifier->verifyPropertyList(
        oo::PList(oo::PList::Array{oo::PList("adder"), oo::PList("viper")}),
        "mute"));
    EXPECT_TRUE(!verifier->verifyPropertyList(oo::PList(4), "mute"));

    const std::shared_ptr<oo::PListSchemaVerifier> orphan =
        oo::PListSchemaVerifier::verifierWithSchema(Parse("string"));
    EXPECT_TRUE(!orphan->verifyPropertyList(oo::PList(4), "orphan"));
    EXPECT_TRUE(orphan->verifyPropertyList(oo::PList("four"), "orphan"));
  }
}

TEST(OOPListSchemaVerifier, describesKeyPaths) {
  EXPECT_EQ(KeyPathText(oo::PList()), "root");
  EXPECT_EQ(KeyPathText(oo::PList(oo::PList::Array{})), "root");
  EXPECT_EQ(KeyPathText(oo::PList(oo::PList::Array{
                  oo::PList("adder-player"), oo::PList("custom_views"),
                  oo::PList(0), oo::PList("view_description")})),
              "adder-player.custom_views[0].view_description");
  EXPECT_EQ(
      KeyPathText(oo::PList(oo::PList::Array{oo::PList(2), oo::PList("x")})),
      "[2].x");
  EXPECT_EQ(KeyPathText(oo::PList(oo::PList::Array{oo::PList(1.5)})),
              "[1.5]");
  EXPECT_EQ(KeyPathText(oo::PList(oo::PList::Array{
                  oo::PList("a"), oo::PList(oo::PList::Dict{})})),
              "(nullopt)");
  EXPECT_EQ(KeyPathText(oo::PList("not an array")), "root");
}

// The C++ verifier hands its delegate itself.
TEST(OOPListSchemaVerifier, delegateIsHandedTheVerifier) {
  {
    EXPECT_TRUE(oo::PListSchemaVerifier::verifierWithSchema(oo::PList()).get() ==
             nullptr);

    std::shared_ptr<oo::PListSchemaVerifier> made =
        oo::PListSchemaVerifier::verifierWithSchema(Parse("string"));
    const std::shared_ptr<TestSchemaDelegate> delegate =
        oo::makeShared<TestSchemaDelegate>();
    made->setDelegate(delegate.get());
    EXPECT_TRUE(made->delegate() == delegate.get());
    EXPECT_TRUE(!made->verifyPropertyList(oo::PList(4), "made"));
    EXPECT_TRUE(made->verifyPropertyList(oo::PList("four"), "made"));
    EXPECT_EQ(delegate->verifiers.size(), 1u);
    EXPECT_TRUE(!delegate->verifiers.empty() &&
             delegate->verifiers[0] == made.get());
    EXPECT_EQ(oo::PListSchemaVerifier::descriptionForKeyPath(
                    oo::PList(oo::PList::Array{oo::PList("a"), oo::PList(3)}))
                    .value_or(""),
                "a[3]");
  }
}
