/*

OOProbabilitySet.cpp

The class cluster is a C++ hierarchy.

Copyright (C) 2008-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


IMPLEMENTATION NOTES
OOProbabilitySet is implemented as a class cluster with two abstract classes,
two special-case implementations for immutable sets with zero or one object,
and two general implementations (one mutable and one immutable). The general
implementations are the only non-trivial ones.

The general immutable implementation, OOConcreteProbabilitySet, consists of
two parallel arrays, one of objects and one of cumulative weights. The
"cumulative weight" for an entry is the sum of its weight and the cumulative
weight of the entry to the left (i.e., with a lower index), with the implicit
entry -1 having a cumulative weight of 0. Since weight cannot be negative,
this means that cumulative weights increase to the right (not strictly
increasing, though, since weights may be zero). We can thus find an object
with a given cumulative weight through a binary search.

OOConcreteMutableProbabilitySet is a naïve implementation using arrays. It
could be optimized, but isn't expected to be used much except for building
sets that will then be immutablized.

*/

#include "ProbabilitySet.h"

#include <cstddef>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "legacy_random.h"
#include "oofnd/Exception.hpp"
#include "oofnd/PList.hpp"

namespace {

constexpr const char* kObjectsKey = "objects";
constexpr const char* kWeightsKey = "weights";

// The property list representation: the objects (the elements themselves) and
// their weights (single reals).
oo::PList PropertyListRepresentation(const oo::PList* objects,
                                     const float* weights, std::size_t count) {
  oo::PList::Array objectList, weightList;
  for (std::size_t i = 0; i < count; ++i) {
    objectList.push_back(objects[i]);
    weightList.push_back(oo::PList::singleReal(weights[i]));
  }
  oo::PList::Dict result;
  result.emplace(kObjectsKey, oo::PList(std::move(objectList)));
  result.emplace(kWeightsKey, oo::PList(std::move(weightList)));
  return oo::PList(std::move(result));
}

// == of two values (a string is equal to the same string); an Object node is
// equal to the same node.
bool SameElement(const oo::PList& element, const oo::PList& object) {
  return element == object;
}

// -indexOfObject: of the old object array: the first object that is the same,
// or -isEqual:.
std::size_t IndexOfObject(const std::vector<oo::PList>& objects,
                          const oo::PList& object) {
  for (std::size_t i = 0; i < objects.size(); ++i) {
    if (SameElement(objects[i], object)) return i;
  }
  return oo::kNotFound;
}

}  // namespace

namespace {

class EmptyProbabilitySet final : public oo::ProbabilitySet {
 public:
  static EmptyProbabilitySet* singleton();

  oo::PList propertyListRepresentation() override;
  oo::PList randomObject() override;
  float weightForObject(const oo::PList& object) override;
  float sumOfWeights() override;
  std::size_t count() override;
  std::vector<oo::PList> allElements() override;
  std::shared_ptr<oo::MutableProbabilitySet> mutableCopy() override;
};

class SingleObjectProbabilitySet final : public oo::ProbabilitySet {
 public:
  bool initWithObject(const oo::PList& object, float weight);

  oo::PList propertyListRepresentation() override;
  oo::PList randomObject() override;
  float weightForObject(const oo::PList& object) override;
  float sumOfWeights() override;
  std::size_t count() override;
  std::vector<oo::PList> allElements() override;
  std::shared_ptr<oo::MutableProbabilitySet> mutableCopy() override;

 private:
  oo::PList _object = {};
  float _weight = {};
};

class ConcreteProbabilitySet final : public oo::ProbabilitySet {
 public:
  ~ConcreteProbabilitySet() override;

  bool initWithObjects(const oo::PList* objects, const float* weights,
                       std::size_t count);

  oo::PList propertyListRepresentation() override;
  std::size_t count() override;
  oo::PList randomObject() override;
  float weightForObject(const oo::PList& object) override;
  float sumOfWeights() override;
  std::vector<oo::PList> allElements() override;
  std::shared_ptr<oo::MutableProbabilitySet> mutableCopy() override;

 private:
  oo::PList privObjectForWeight(float target);

  std::size_t _count = {};
  std::vector<oo::PList> _objects;
  std::vector<float>
      _cumulativeWeights;  // Each cumulative weight is weight of object at
                           // this index + weight of all objects to left.
  float _sumOfWeights = {};
};

class ConcreteMutableProbabilitySet final : public oo::MutableProbabilitySet {
 public:
  // (-initPriv, which leaves the object and weight arrays empty, is the
  // constructor.)
  void initPrivWithObjectArray(
      const std::vector<oo::PList>& objects, const std::vector<float>& weights,
      float sumOfWeights);  // For internal use by mutableCopy
  void initWithObjects(const oo::PList* objects, const float* weights,
                       std::size_t count);
  bool initWithPropertyListRepresentation(const oo::PList& plist);

  oo::PList propertyListRepresentation() override;
  std::size_t count() override;
  oo::PList randomObject() override;
  float weightForObject(const oo::PList& object) override;
  float sumOfWeights() override;
  std::vector<oo::PList> allElements() override;
  void setWeight(float weight, const oo::PList& object) override;
  void removeObject(const oo::PList& object) override;
  std::shared_ptr<oo::ProbabilitySet> copy() override;
  std::shared_ptr<oo::MutableProbabilitySet> mutableCopy() override;

 private:
  std::vector<oo::PList> _objects = {};
  std::vector<float> _weights = {};
  float _sumOfWeights = {};
};

std::shared_ptr<oo::MutableProbabilitySet> MakeConcreteMutable(
    const oo::PList* objects, const float* weights, std::size_t count) {
  std::shared_ptr<ConcreteMutableProbabilitySet> result =
      oo::makeShared<ConcreteMutableProbabilitySet>();
  result->initWithObjects(objects, weights, count);
  return result;
}

}  // namespace

// Abstract class just tosses allocations over to concrete class.

std::shared_ptr<oo::ProbabilitySet> oo::ProbabilitySet::probabilitySet() {
  return oo::share(EmptyProbabilitySet::singleton());
}

std::shared_ptr<oo::ProbabilitySet>
oo::ProbabilitySet::probabilitySetWithObjects(const oo::PList* objects,
                                              const float* weights,
                                              std::size_t count) {
  // Zero objects: return empty-set singleton.
  if (count == 0) return probabilitySet();

  // If count is not zero and one of the paramters is nil, we've got us a
  // programming error.
  if (objects == nullptr || weights == nullptr) {
    oo::RaiseException(
        oo::InvalidArgumentException,
        "Attempt to create %s with non-zero count but nil objects or weights.",
        "OOProbabilitySet");
    abort();  // unreachable: +raise:format: does not return (the analyser
              // cannot see that through a message)
  }

  // Single object: simple one-object set. Expected to be quite common.
  if (count == 1) {
    std::shared_ptr<SingleObjectProbabilitySet> result =
        oo::makeShared<SingleObjectProbabilitySet>();
    if (!result->initWithObject(objects[0], weights[0])) return nullptr;
    return result;
  }

  // Otherwise, use general implementation.
  std::shared_ptr<ConcreteProbabilitySet> result =
      oo::makeShared<ConcreteProbabilitySet>();
  if (!result->initWithObjects(objects, weights, count)) return nullptr;
  return result;
}

std::shared_ptr<oo::ProbabilitySet>
oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
    const oo::PList& plist) {
  const oo::PList& representation = plist;
  const oo::PList* objects = representation.find(kObjectsKey);
  const oo::PList* weights = representation.find(kWeightsKey);
  std::size_t i = 0, count = 0;
  std::vector<oo::PList> rawObjects;
  std::vector<float> rawWeights;

  // Validate
  if (objects == nullptr || !objects->isArray() || weights == nullptr ||
      !weights->isArray()) {
    return nullptr;
  }
  count = objects->count();
  if (count != weights->count()) {
    return nullptr;
  }

  // Extract contents.
  rawObjects.reserve(count);
  rawWeights.reserve(count);

  // Extract objects.
  for (i = 0; i < count; ++i) {
    rawObjects.push_back(*objects->at(i));
  }

  // Extract and convert weights.
  for (i = 0; i < count; ++i) {
    rawWeights.push_back(fmax(weights->at<float>(i), 0.0f));
  }

  return probabilitySetWithObjects(rawObjects.data(), rawWeights.data(), count);
}

// oo::DescriptionOf (OODescription.h) wraps this as "<Class 0x...>{count=N}",
// as the legacy -descriptionComponents did.
std::optional<std::string> oo::ProbabilitySet::descriptionComponents() const {
  // (count() is not const; the fixed const signature reaches it
  // through a cast)
  return std::format("count={}",
                     const_cast<oo::ProbabilitySet*>(this)->count());
}

std::shared_ptr<oo::ProbabilitySet> oo::ProbabilitySet::copy() {
  // Immutable: a copy is the set itself. (-copyWithZone: made a new set through
  // the property list only for a different zone, which C++ does not have.)
  return oo::share(this);
}

std::shared_ptr<oo::MutableProbabilitySet> oo::ProbabilitySet::mutableCopy() {
  return oo::MutableProbabilitySet::
      probabilitySetWithPropertyListRepresentation(
          propertyListRepresentation());
}

// (OOExtendedProbabilitySet)

bool oo::ProbabilitySet::containsObject(const oo::PList& object) {
  return weightForObject(object) >= 0.0f;
}

float oo::ProbabilitySet::probabilityForObject(const oo::PList& object) {
  float weight = weightForObject(object);
  if (weight > 0) weight /= sumOfWeights();

  return weight;
}

namespace {

EmptyProbabilitySet* sOOEmptyProbabilitySetSingleton =
    nullptr;  // never released (it was an immortal singleton)

EmptyProbabilitySet* EmptyProbabilitySet::singleton() {
  if (sOOEmptyProbabilitySetSingleton == nullptr) {
    // Never destroyed: a heap std::shared_ptr that is never deleted.
    static std::shared_ptr<EmptyProbabilitySet>*
        sOOEmptyProbabilitySetSingletonHolder = nullptr;  // never deleted
    sOOEmptyProbabilitySetSingletonHolder =
        new std::shared_ptr<EmptyProbabilitySet>(
            oo::makeShared<EmptyProbabilitySet>());
    sOOEmptyProbabilitySetSingleton =
        sOOEmptyProbabilitySetSingletonHolder->get();
  }

  return sOOEmptyProbabilitySetSingleton;
}

oo::PList EmptyProbabilitySet::propertyListRepresentation() {
  return PropertyListRepresentation(nullptr, nullptr, 0);
}

oo::PList EmptyProbabilitySet::randomObject() { return oo::PList(); }

float EmptyProbabilitySet::weightForObject(const oo::PList& /*object*/) {
  return -1.0f;
}

float EmptyProbabilitySet::sumOfWeights() { return 0.0f; }

std::size_t EmptyProbabilitySet::count() { return 0; }

std::vector<oo::PList> EmptyProbabilitySet::allElements() {
  return std::vector<oo::PList>();
}

std::shared_ptr<oo::MutableProbabilitySet> EmptyProbabilitySet::mutableCopy() {
  // A mutable copy of an empty probability set is equivalent to a new empty
  // mutable probability set.
  return oo::makeShared<ConcreteMutableProbabilitySet>();
}

bool SingleObjectProbabilitySet::initWithObject(const oo::PList& object,
                                                float weight) {
  if (object.isNull()) {
    return false;
  }

  _object = object;
  _weight = fmax(weight, 0.0f);

  return true;
}

oo::PList SingleObjectProbabilitySet::propertyListRepresentation() {
  return PropertyListRepresentation(&_object, &_weight, 1);
}

oo::PList SingleObjectProbabilitySet::randomObject() { return _object; }

float SingleObjectProbabilitySet::weightForObject(const oo::PList& object) {
  if (SameElement(_object, object))
    return _weight;
  else
    return -1.0f;
}

float SingleObjectProbabilitySet::sumOfWeights() { return _weight; }

std::size_t SingleObjectProbabilitySet::count() { return 1; }

std::vector<oo::PList> SingleObjectProbabilitySet::allElements() {
  return std::vector<oo::PList>{_object};
}

std::shared_ptr<oo::MutableProbabilitySet>
SingleObjectProbabilitySet::mutableCopy() {
  return MakeConcreteMutable(&_object, &_weight, 1);
}

bool ConcreteProbabilitySet::initWithObjects(const oo::PList* objects,
                                             const float* weights,
                                             std::size_t count) {
  std::size_t i = 0;
  float cuWeight = 0.0f;

  assert(count > 1 && objects != nullptr && weights != nullptr);
  if (objects == nullptr || weights == nullptr) {
    // Unreachable (probabilitySetWithObjects() raises first); copying an
    // element forms a reference, where the old -retain of an id element was a
    // message to nil.
    return false;
  }

  // Allocate arrays
  _objects.resize(count);
  _cumulativeWeights.resize(count);

  // Fill in arrays, copy objects, add up weights.
  for (i = 0; i != count; ++i) {
    _objects[i] = objects[i];
    cuWeight += weights[i];
    _cumulativeWeights[i] = cuWeight;
  }
  _count = count;
  _sumOfWeights = cuWeight;

  return true;
}

ConcreteProbabilitySet::~ConcreteProbabilitySet() = default;

oo::PList ConcreteProbabilitySet::propertyListRepresentation() {
  std::vector<float> weights;
  float cuWeight = 0.0f, sum = 0.0f;
  std::size_t i = 0;

  weights.reserve(_count);
  for (i = 0; i < _count; ++i) {
    cuWeight = _cumulativeWeights[i];
    weights.push_back(cuWeight - sum);
    sum = cuWeight;
  }

  return PropertyListRepresentation(_objects.data(), weights.data(), _count);
}

std::size_t ConcreteProbabilitySet::count() { return _count; }

oo::PList ConcreteProbabilitySet::privObjectForWeight(float target) {
  /*	Select an object at random. This is a binary search in the cumulative
          weights array. Since weights of zero are allowed, there may be several
          objects with the same cumulative weight, in which case we select the
          leftmost, i.e. the one where the delta is non-zero.
  */

  std::size_t low = 0, high = _count - 1, idx = 0;
  float weight = 0.0f;

  while (low < high) {
    idx = (low + high) / 2;
    weight = _cumulativeWeights[idx];
    if (weight > target) {
      if (idx == 0) [[unlikely]]
        break;
      high = idx - 1;
    } else if (weight < target) {
      low = idx + 1;
    } else {
      break;
    }
  }

  if (weight > target) {
    while (idx > 0 && _cumulativeWeights[idx - 1] >= target) --idx;
  } else {
    while (idx < (_count - 1) && _cumulativeWeights[idx] < target) ++idx;
  }

  assert(idx < _count);
  return _objects[idx];
}

oo::PList ConcreteProbabilitySet::randomObject() {
  if (_sumOfWeights <= 0.0f) return oo::PList();
  return privObjectForWeight(randf() * _sumOfWeights);
}

float ConcreteProbabilitySet::weightForObject(const oo::PList& object) {
  std::size_t i;

  // Can't have a null object in collection.
  if (object.isNull()) return -1.0f;

  // Perform linear search, then get weight by subtracting cumulative weight
  // from cumulative weight to left.
  for (i = 0; i < _count; ++i) {
    if (SameElement(_objects[i], object)) {
      float leftWeight = (i != 0) ? _cumulativeWeights[i - 1] : 0.0f;
      return _cumulativeWeights[i] - leftWeight;
    }
  }

  // If we got here, object not found.
  return -1.0f;
}

float ConcreteProbabilitySet::sumOfWeights() { return _sumOfWeights; }

std::vector<oo::PList> ConcreteProbabilitySet::allElements() {
  return _objects;
}

std::shared_ptr<oo::MutableProbabilitySet>
ConcreteProbabilitySet::mutableCopy() {
  std::shared_ptr<oo::MutableProbabilitySet> result;
  std::vector<float> weights(_count);
  std::size_t i = 0;
  float weight = 0.0f, sum = 0.0f;

  // Convert cumulative weights to "plain" weights.
  for (i = 0; i < _count; ++i) {
    weight = _cumulativeWeights[i];
    weights[i] = weight - sum;
    sum += weights[i];
  }

  result = MakeConcreteMutable(_objects.data(), weights.data(), _count);

  return result;
}

}  // namespace

std::shared_ptr<oo::MutableProbabilitySet>
oo::MutableProbabilitySet::probabilitySet() {
  return oo::makeShared<ConcreteMutableProbabilitySet>();
}

std::shared_ptr<oo::MutableProbabilitySet>
oo::MutableProbabilitySet::probabilitySetWithObjects(const oo::PList* objects,
                                                     const float* weights,
                                                     std::size_t count) {
  return MakeConcreteMutable(objects, weights, count);
}

std::shared_ptr<oo::MutableProbabilitySet>
oo::MutableProbabilitySet::probabilitySetWithPropertyListRepresentation(
    const oo::PList& plist) {
  std::shared_ptr<ConcreteMutableProbabilitySet> result =
      oo::makeShared<ConcreteMutableProbabilitySet>();
  if (!result->initWithPropertyListRepresentation(plist)) return nullptr;
  return result;
}

std::shared_ptr<oo::ProbabilitySet> oo::MutableProbabilitySet::copy() {
  return oo::ProbabilitySet::probabilitySetWithPropertyListRepresentation(
      propertyListRepresentation());
}

namespace {

// For internal use by mutableCopy
void ConcreteMutableProbabilitySet::initPrivWithObjectArray(
    const std::vector<oo::PList>& objects, const std::vector<float>& weights,
    float sumOfWeights) {
  assert(objects.size() == weights.size() && sumOfWeights >= 0.0f);

  _objects = objects;
  _weights = weights;
  _sumOfWeights = sumOfWeights;
}

void ConcreteMutableProbabilitySet::initWithObjects(const oo::PList* objects,
                                                    const float* weights,
                                                    std::size_t count) {
  std::size_t i = 0;

  // Validate parameters.
  if (count != 0 && (objects == nullptr || weights == nullptr)) {
    oo::RaiseException(
        oo::InvalidArgumentException,
        "Attempt to create %s with non-zero count but nil objects or weights.",
        "OOMutableProbabilitySet");
    abort();  // unreachable: +raise:format: does not return (the analyser
              // cannot see that through a message)
  }

  // Set up & go.
  for (i = 0; i != count; ++i) {
    setWeight(fmax(weights[i], 0.0f), objects[i]);
  }
}

bool ConcreteMutableProbabilitySet::initWithPropertyListRepresentation(
    const oo::PList& plist) {
  bool OK = true;
  const oo::PList& representation = plist;
  const oo::PList* objects = nullptr;
  const oo::PList* weights = nullptr;
  std::size_t i = 0, count = 0;

  if (OK) {
    objects = representation.find(kObjectsKey);
    weights = representation.find(kWeightsKey);

    // Validate
    if (objects == nullptr || !objects->isArray() || weights == nullptr ||
        !weights->isArray()) {
      OK = false;
    } else {
      count = objects->count();
      if (count != weights->count()) OK = false;
    }
  }

  if (OK) {
    for (i = 0; i < count; ++i) {
      setWeight(weights->at<float>(i), *objects->at(i));
    }
  }

  return OK;
}

oo::PList ConcreteMutableProbabilitySet::propertyListRepresentation() {
  return PropertyListRepresentation(_objects.data(), _weights.data(),
                                    _objects.size());
}

std::size_t ConcreteMutableProbabilitySet::count() { return _objects.size(); }

oo::PList ConcreteMutableProbabilitySet::randomObject() {
  float target = 0.0f, sum = 0.0f, sumOfWeights;
  std::size_t i = 0, count = 0;

  sumOfWeights = this->sumOfWeights();
  target = randf() * sumOfWeights;
  count = _objects.size();
  if (count == 0 || sumOfWeights <= 0.0f) return oo::PList();

  for (i = 0; i < count; ++i) {
    sum += _weights[i];
    if (sum >= target) return _objects[i];
  }

  OO_LOG("probabilitySet.broken",
         "{} fell off end, returning first object. Nominal sum = {:f}, target "
         "= {:f}, actual sum = {:f}, count = {}. {}",
         __PRETTY_FUNCTION__, sumOfWeights, target, sum, count,
         "This is an internal error, please report it.");
  return _objects[0];
}

float ConcreteMutableProbabilitySet::weightForObject(const oo::PList& object) {
  float result = -1.0f;

  if (!object.isNull()) {
    std::size_t index = IndexOfObject(_objects, object);
    if (index != oo::kNotFound) {
      result = _weights[index];
      if (index != 0) result -= _weights[index - 1];
    }
  }
  return result;
}

float ConcreteMutableProbabilitySet::sumOfWeights() {
  if (_sumOfWeights < 0.0f) {
    std::size_t i, count;
    count = this->count();

    _sumOfWeights = 0.0f;
    for (i = 0; i < count; ++i) {
      _sumOfWeights += _weights[i];
    }
  }
  return _sumOfWeights;
}

std::vector<oo::PList> ConcreteMutableProbabilitySet::allElements() {
  return _objects;
}

void ConcreteMutableProbabilitySet::setWeight(float weight,
                                              const oo::PList& object) {
  if (object.isNull()) return;

  weight = fmax(weight, 0.0f);
  std::size_t index = IndexOfObject(_objects, object);
  if (index == oo::kNotFound) {
    _objects.emplace_back(object);
    _weights.push_back(weight);
    if (_sumOfWeights >= 0) {
      _sumOfWeights += weight;
    }
    // Else, _sumOfWeights is invalid and will need to be recalculated on
    // demand.
  } else {
    _sumOfWeights =
        -1.0f;  // Simply subtracting the relevant weight doesn't work if the
                // weight is large, due to floating-point precision issues.
    _weights[index] = weight;
  }
}

void ConcreteMutableProbabilitySet::removeObject(const oo::PList& object) {
  if (object.isNull()) return;

  std::size_t index = IndexOfObject(_objects, object);
  if (index != oo::kNotFound) {
    _objects.erase(_objects.begin() + static_cast<std::ptrdiff_t>(index));
    _sumOfWeights =
        -1.0f;  // Simply subtracting the relevant weight doesn't work if the
                // weight is large, due to floating-point precision issues.
    _weights.erase(_weights.begin() + static_cast<std::ptrdiff_t>(index));
  }
}

std::shared_ptr<oo::ProbabilitySet> ConcreteMutableProbabilitySet::copy() {
  std::size_t count = 0;

  count = _objects.size();
  if (count == 0) [[unlikely]]
    return oo::ProbabilitySet::probabilitySet();

  return oo::ProbabilitySet::probabilitySetWithObjects(_objects.data(),
                                                       _weights.data(), count);
}

std::shared_ptr<oo::MutableProbabilitySet>
ConcreteMutableProbabilitySet::mutableCopy() {
  // (the arrays are copied)
  std::shared_ptr<ConcreteMutableProbabilitySet> result =
      oo::makeShared<ConcreteMutableProbabilitySet>();
  result->initPrivWithObjectArray(_objects, _weights, _sumOfWeights);
  return result;
}

}  // namespace
