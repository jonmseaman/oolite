/*	test_OOProbabilitySet.mm
        Unit tests for OOProbabilitySet and OOMutableProbabilitySet
   (src/Core/OOProbabilitySet.h).

        It pins: the empty set (one shared object), the one-object and general
   immutable sets, weights (clamped at zero), sums, probabilities, the weighted
   pick (a zero weight is never picked; all zero picks nothing), the
   property-list round trip, element identity (strings by value, Object nodes by
   object), the mutable set (including its weight lookup, which subtracts the
   previous entry's weight), copies both ways, the description, and the
   exception for a non-zero count with no arrays. The calls here are the C++
   classes'. Run: tests/run.sh unit
*/

#include <cmath>
#include <cstring>

#include <gtest/gtest.h>

#include "oofnd/PList.hpp"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ProbabilitySet.h"
#include "legacy_random.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Exception.hpp"

namespace {

oo::PList Str(const char* string) { return oo::PList(std::string(string)); }

bool Near(float a, float b) { return std::fabs(a - b) < 1e-5f; }

std::vector<std::string> Strings(const std::vector<oo::PList>& values) {
  std::vector<std::string> result;
  for (const oo::PList& value : values)
    result.push_back(value.isString() ? *value.getIf<std::string>()
                                      : std::string("?"));
  return result;
}

const oo::PList kABC[] = {Str("a"), Str("b"), Str("c")};
const float kWeights[] = {1.0f, 0.0f, 2.0f};

}  // namespace

TEST(OOProbabilitySet, emptySet) {
  {
    std::shared_ptr<oo::ProbabilitySet> empty = oo::ProbabilitySet::probabilitySet();
    EXPECT_TRUE(empty != nullptr &&
             empty == oo::ProbabilitySet::probabilitySet());  // one shared object
    EXPECT_TRUE(empty ==
             oo::ProbabilitySet::probabilitySetWithObjects(NULL, NULL, 0));
    EXPECT_TRUE(empty->copy() == empty);
    EXPECT_TRUE(empty->count() == 0);
    EXPECT_TRUE(empty->randomObject().isNull());
    EXPECT_TRUE(empty->weightForObject(Str("a")) == -1.0f);
    EXPECT_TRUE(empty->sumOfWeights() == 0.0f);
    EXPECT_TRUE(empty->allElements().empty());
    EXPECT_TRUE(!empty->containsObject(Str("a")));
    EXPECT_TRUE(empty->propertyListRepresentation() ==
             oo::PList(oo::PList::Dict(
                 {{"objects", oo::PList(oo::PList::Array())},
                  {"weights", oo::PList(oo::PList::Array())}})));
    EXPECT_TRUE(empty->descriptionComponents() ==
             std::optional<std::string>("count=0"));
  }
}

TEST(OOProbabilitySet, oneObjectSet) {
  {
    const oo::PList object = Str("cobra");
    const float weight = -2.0f;
    std::shared_ptr<oo::ProbabilitySet> one =
        oo::ProbabilitySet::probabilitySetWithObjects(&object, &weight, 1);
    EXPECT_TRUE(one->count() == 1);
    EXPECT_TRUE(one->weightForObject(Str("cobra")) == 0.0f);  // clamped
    EXPECT_TRUE(one->weightForObject(Str("viper")) == -1.0f);
    EXPECT_TRUE(one->randomObject() == object);  // even at weight 0
    EXPECT_TRUE(one->sumOfWeights() == 0.0f);
    EXPECT_TRUE(Strings(one->allElements()) ==
             std::vector<std::string>({"cobra"}));
    EXPECT_TRUE(one->copy() == one);  // immutable: copy is retain
    EXPECT_TRUE(one->descriptionComponents() ==
             std::optional<std::string>("count=1"));

    const float two = 2.0f;
    std::shared_ptr<oo::ProbabilitySet> weighted =
        oo::ProbabilitySet::probabilitySetWithObjects(&object, &two, 1);
    EXPECT_TRUE(weighted->probabilityForObject(Str("cobra")) == 1.0f);
    EXPECT_TRUE(weighted->propertyListRepresentation() ==
             oo::PList(oo::PList::Dict(
                 {{"objects", oo::PList(oo::PList::Array({object}))},
                  {"weights", oo::PList(oo::PList::Array(
                                  {oo::PList::singleReal(2.0f)}))}})));
  }
}

TEST(OOProbabilitySet, generalSet) {
  {
    std::shared_ptr<oo::ProbabilitySet> set =
        oo::ProbabilitySet::probabilitySetWithObjects(kABC, kWeights, 3);
    EXPECT_TRUE(set->count() == 3);
    EXPECT_TRUE(Strings(set->allElements()) ==
             std::vector<std::string>({"a", "b", "c"}));
    EXPECT_TRUE(set->weightForObject(Str("a")) == 1.0f &&
             set->weightForObject(Str("b")) == 0.0f &&
             set->weightForObject(Str("c")) == 2.0f);
    EXPECT_TRUE(set->weightForObject(Str("d")) == -1.0f &&
             set->weightForObject(oo::PList()) == -1.0f);
    EXPECT_TRUE(set->sumOfWeights() == 3.0f);
    EXPECT_TRUE(Near(set->probabilityForObject(Str("a")), 1.0f / 3.0f) &&
             set->probabilityForObject(Str("b")) == 0.0f &&
             set->probabilityForObject(Str("d")) == -1.0f);
    EXPECT_TRUE(set->containsObject(Str("b")) && !set->containsObject(Str("d")));
    EXPECT_TRUE(set->copy() == set);
    EXPECT_TRUE(set->descriptionComponents() ==
             std::optional<std::string>("count=3"));

    ranrot_srand(12345);  // (unseeded, randf() is always 0)
    int picks[3] = {0, 0, 0};
    for (int i = 0; i < 300; i++) {
      const oo::PList pick = set->randomObject();
      for (int j = 0; j < 3; j++)
        if (pick == kABC[j]) picks[j]++;
    }
    EXPECT_TRUE(picks[0] > 0 && picks[1] == 0 && picks[2] > 0 &&
             picks[0] + picks[2] == 300);  // weight 0: never

    const float zeros[] = {0.0f, 0.0f, 0.0f};
    EXPECT_TRUE(oo::ProbabilitySet::probabilitySetWithObjects(kABC, zeros, 3)
                 ->randomObject()
                 .isNull());

    // The weights come back from the cumulative ones.
    oo::PList::Array weights = {oo::PList::singleReal(1.0f),
                                oo::PList::singleReal(0.0f),
                                oo::PList::singleReal(2.0f)};
    EXPECT_TRUE(set->propertyListRepresentation() ==
             oo::PList(oo::PList::Dict(
                 {{"objects", oo::PList(oo::PList::Array(std::begin(kABC),
                                                         std::end(kABC)))},
                  {"weights", oo::PList(weights)}})));
  }
}

TEST(OOProbabilitySet, propertyListRoundTrip) {
  {
    std::shared_ptr<oo::ProbabilitySet> set =
        oo::ProbabilitySet::probabilitySetWithObjects(kABC, kWeights, 3);
    std::shared_ptr<oo::ProbabilitySet> again =
        oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
            set->propertyListRepresentation());
    EXPECT_TRUE(again->propertyListRepresentation() ==
             set->propertyListRepresentation());

    // Negative weights are clamped; mismatched or missing arrays give nil.
    oo::PList negative(oo::PList::Dict(
        {{"objects", oo::PList(oo::PList::Array({Str("x"), Str("y")}))},
         {"weights", oo::PList(oo::PList::Array(
                         {oo::PList(-1.0), oo::PList(std::int64_t(3))}))}}));
    std::shared_ptr<oo::ProbabilitySet> clamped =
        oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
            negative);
    EXPECT_TRUE(clamped->weightForObject(Str("x")) == 0.0f &&
             clamped->weightForObject(Str("y")) == 3.0f);
    oo::PList mismatched(
        oo::PList::Dict({{"objects", oo::PList(oo::PList::Array({Str("x")}))},
                         {"weights", oo::PList(oo::PList::Array())}}));
    EXPECT_TRUE(oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
                 mismatched) == nullptr);
    EXPECT_TRUE(oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
                 oo::PList()) == nullptr);
    EXPECT_TRUE(
        oo::MutableProbabilitySet::probabilitySetWithPropertyListRepresentation(
            mismatched) == nullptr);

    oo::PList one(oo::PList::Dict(
        {{"objects", oo::PList(oo::PList::Array({Str("x")}))},
         {"weights", oo::PList(oo::PList::Array({oo::PList(0.5)}))}}));
    EXPECT_TRUE(oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(one)
                 ->weightForObject(Str("x")) == 0.5f);
  }
}

// Object elements: C++ objects in Object nodes.
namespace {

class TestForeign final : public oo::PListForeign {
 public:
  std::string className() const override { return "TestForeign"; }
  std::string description() const override { return "<TestForeign>"; }
};

oo::PList Node(const std::shared_ptr<TestForeign>& object) {
  return oo::PList(oo::PList::Object(object));
}

}  // namespace

TEST(OOProbabilitySet, objectElements) {
  {
    oo::AutoreleaseScope pool;
    std::shared_ptr<TestForeign> object = oo::makeShared<TestForeign>();
    const oo::PList elements[] = {Node(object), Str("s")};
    const float weights[] = {1.0f, 1.0f};
    std::shared_ptr<oo::ProbabilitySet> set =
        oo::ProbabilitySet::probabilitySetWithObjects(elements, weights, 2);
    EXPECT_TRUE(set->weightForObject(Node(object)) ==
             1.0f);  // the same object in another node
    EXPECT_TRUE(set->weightForObject(Node(oo::makeShared<TestForeign>())) == -1.0f);
    const oo::PList::Object* node =
        set->allElements()[0].getIf<oo::PList::Object>();
    EXPECT_TRUE(node != nullptr && node->get() == object.get());
  }
}

TEST(OOProbabilitySet, mutableSet) {
  {
    std::shared_ptr<oo::MutableProbabilitySet> set =
        oo::MutableProbabilitySet::probabilitySet();
    EXPECT_TRUE(set != nullptr &&
             set.get() != oo::ProbabilitySet::probabilitySet().get() &&
             set->count() == 0);
    EXPECT_TRUE(set->randomObject().isNull() && set->sumOfWeights() == 0.0f);
    EXPECT_TRUE(set->descriptionComponents() ==
             std::optional<std::string>("count=0"));

    set->setWeight(1.0f, Str("a"));
    set->setWeight(3.0f, Str("b"));
    set->setWeight(-1.0f, Str("c"));    // clamped to 0
    set->setWeight(5.0f, oo::PList());  // ignored
    EXPECT_TRUE(set->count() == 3 &&
             Strings(set->allElements()) ==
                 std::vector<std::string>({"a", "b", "c"}));
    EXPECT_TRUE(set->sumOfWeights() == 4.0f);
    // The mutable set's lookup subtracts the previous entry's weight, as if
    // they were cumulative.
    EXPECT_TRUE(set->weightForObject(Str("a")) == 1.0f);
    EXPECT_TRUE(set->weightForObject(Str("b")) == 2.0f);
    EXPECT_TRUE(set->weightForObject(Str("c")) == -3.0f);
    EXPECT_TRUE(set->weightForObject(Str("d")) == -1.0f);

    set->setWeight(2.0f, Str("a"));  // an update recomputes the sum on demand
    EXPECT_TRUE(set->count() == 3 && set->sumOfWeights() == 5.0f);
    set->removeObject(Str("b"));
    set->removeObject(Str("zz"));
    set->removeObject(oo::PList());
    EXPECT_TRUE(Strings(set->allElements()) ==
                 std::vector<std::string>({"a", "c"}) &&
             set->sumOfWeights() == 2.0f);
    for (int i = 0; i < 100; i++)
      EXPECT_TRUE(set->randomObject() == Str("a"));  // c has weight 0

    std::shared_ptr<oo::MutableProbabilitySet> made =
        oo::MutableProbabilitySet::probabilitySetWithObjects(kABC, kWeights, 3);
    EXPECT_TRUE(made->count() == 3 && made->sumOfWeights() == 3.0f);
    made->setWeight(1.0f, Str("d"));
    EXPECT_TRUE(made->count() == 4);
    std::shared_ptr<oo::MutableProbabilitySet> fromPList =
        oo::MutableProbabilitySet::probabilitySetWithPropertyListRepresentation(
            made->propertyListRepresentation());
    EXPECT_TRUE(fromPList->count() == 4 && fromPList->sumOfWeights() == 4.0f);
  }
}

TEST(OOProbabilitySet, copies) {
  {
    // Mutable to immutable: the empty set, a one-object set, a general set.
    std::shared_ptr<oo::MutableProbabilitySet> mutableSet =
        oo::MutableProbabilitySet::probabilitySet();
    EXPECT_TRUE(mutableSet->copy() == oo::ProbabilitySet::probabilitySet());
    mutableSet->setWeight(2.0f, Str("a"));
    std::shared_ptr<oo::ProbabilitySet> one = mutableSet->copy();
    EXPECT_TRUE(one.get() != mutableSet.get() && one->count() == 1 &&
             one->weightForObject(Str("a")) == 2.0f);
    mutableSet->setWeight(0.5f, Str("b"));
    mutableSet->setWeight(1.5f, Str("c"));
    std::shared_ptr<oo::ProbabilitySet> general = mutableSet->copy();
    EXPECT_TRUE(general->count() == 3 && general->sumOfWeights() == 4.0f);
    EXPECT_TRUE(general->weightForObject(Str("a")) == 2.0f &&
             general->weightForObject(Str("b")) == 0.5f &&
             general->weightForObject(Str("c")) == 1.5f);

    // Immutable to mutable, and mutable to mutable: independent.
    std::shared_ptr<oo::MutableProbabilitySet> back = general->mutableCopy();
    EXPECT_TRUE(back->count() == 3 && back->sumOfWeights() == 4.0f);
    back->removeObject(Str("a"));
    EXPECT_TRUE(back->count() == 2 && general->count() == 3);
    // (A removal leaves the sum to be recomputed on demand, and a mutable copy
    // asserts that it is known: -sumOfWeights first, as the game never copies a
    // mutable set it has edited.)
    EXPECT_TRUE(back->sumOfWeights() == 2.0f);
    std::shared_ptr<oo::MutableProbabilitySet> twin = back->mutableCopy();
    twin->setWeight(9.0f, Str("z"));
    EXPECT_TRUE(twin->count() == 3 && back->count() == 2 &&
             twin->sumOfWeights() == 11.0f);
    EXPECT_TRUE(one->mutableCopy()->count() == 1);
    EXPECT_TRUE(oo::ProbabilitySet::probabilitySet()->mutableCopy()->count() == 0);
  }
}

TEST(OOProbabilitySet, nonZeroCountWithoutArraysRaises) {
  {
    const char* name = nullptr;
    std::optional<oo::Exception> kept;  // what the autorelease pool kept alive
    try {
      (void)oo::ProbabilitySet::probabilitySetWithObjects(NULL, NULL, 2);
    } catch (const oo::Exception& e)  // was @catch (OOException *e)
    {
      kept = e;
      name = kept->name();
    }
    EXPECT_TRUE(name != nullptr &&
             std::strcmp(name, oo::InvalidArgumentException) == 0);
  }
}
