/*

OOOXPVerifierStage.cpp

The subclass responsibilities are virtual.


Copyright (C) 2007-2013 Jens Ayton

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

*/

#include "OXPVerifierStage.h"

#include <assert.h>

#include <string>
#include <vector>

#include "oofnd/Exception.hpp"
#include "oofnd/String.hpp"

#if OO_OXP_VERIFIER_ENABLED

#include <format>
#include <typeinfo>

#include "Demangle.h"
#include "oofnd/Log.hpp"

// Adding a stage to a set of stages: identity, no duplicates, null ignored.
namespace {

void AddStage(std::vector<std::shared_ptr<oo::OXPVerifierStage>>& stages,
              oo::OXPVerifierStage* stage) {
  if (stage == nullptr) return;
  if (std::ranges::find(stages, stage, [](const auto& owner) {
        return owner.get();
      }) == stages.end())
    stages.emplace_back(oo::share(stage));
}

}  // namespace

// OOObject's -description wraps this as "<Class 0x...>{"name"}", which is what
// this class's own -description printed. name() is a subclass's override, so it
// cannot be const.
std::optional<std::string> oo::OXPVerifierStage::descriptionComponents() const {
  return "\"" +
         const_cast<oo::OXPVerifierStage*>(this)->name().value_or("(null)") +
         "\"";
}

// "<Class 0x...>{name}", the stage's description.
std::string oo::OXPVerifierStage::description() const {
  return std::format("<{} {}>{{", className(),
                     oo::str::pointerDescription(this)) +
         descriptionComponents().value_or("") + "}";
}

// The C++ class's name (demangled, "cxx::" dropped).
std::string oo::OXPVerifierStage::className() const {
  std::string result = oo::DescriptionClassName(typeid(*this).name());
  if (result.starts_with("cxx::")) result.erase(0, 5);
  return result;
}

// The C++ verifier, borrowed: the verifier keeps its stages and outlives them
// while they run.
oo::OXPVerifier* oo::OXPVerifierStage::verifier() { return _verifier; }

bool oo::OXPVerifierStage::completed() { return _hasRun; }

std::optional<std::string> oo::OXPVerifierStage::name() {
  OOLogGenericSubclassResponsibility();
  return std::nullopt;
}

std::optional<std::vector<std::string>> oo::OXPVerifierStage::dependencies() {
  return std::nullopt;
}

std::optional<std::vector<std::string>> oo::OXPVerifierStage::dependents() {
  return std::nullopt;
}

bool oo::OXPVerifierStage::shouldRun() { return true; }

void oo::OXPVerifierStage::run() { OOLogGenericSubclassResponsibility(); }

// Internal (was the OOInternal category).

void oo::OXPVerifierStage::setVerifier(oo::OXPVerifier* verifier) {
  _verifier = verifier;  // Not retained.
}

bool oo::OXPVerifierStage::isDependentOf(oo::OXPVerifierStage* stage) {
  if (stage == nullptr) return false;

  // Direct dependency check.
  if (std::ranges::find(_dependencies, stage, [](const auto& owner) {
        return owner.get();
      }) != _dependencies.end())
    return true;

  // Recursive dependency check.
  for (const auto& directDep : _dependencies) {
    if (directDep->isDependentOf(stage)) return true;
  }

  return false;
}

void oo::OXPVerifierStage::registerDependency(
    oo::OXPVerifierStage* dependency) {
  AddStage(_dependencies, dependency);
  AddStage(_incompleteDependencies, dependency);

  if (dependency != nullptr) dependency->registerDepedent(this);
}

bool oo::OXPVerifierStage::canRun() { return _canRun; }

void oo::OXPVerifierStage::performRun() {
  assert(_canRun && !_hasRun);

  oo::log::pushIndent();
  try {
    run();
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    // %@ of a Foundation exception printed GNUstep's -description, "<ClassName:
    // 0x...> NAME:... REASON:..."; OOException has no -description, so the same
    // layout is spelled out.
    OO_LOG("verifyOXP.exception",
           "***** Exception while running verification stage \"{}\": "
           "<OOException: {}> NAME:{} REASON:{}",
           name().value_or("(null)"),
           oo::str::pointerDescription(exception->object()), exception->name(),
           exception->reason());
  }
  oo::log::popIndent();

  _hasRun = true;
  _canRun = false;
  notifyDependents();
}

void oo::OXPVerifierStage::noteSkipped() {
  assert(_canRun && !_hasRun);

  _hasRun = true;
  _canRun = false;
  notifyDependents();
}

void oo::OXPVerifierStage::dependencyRegistrationComplete() {
  _canRun = _incompleteDependencies.empty();
}

std::vector<std::shared_ptr<oo::OXPVerifierStage>>
oo::OXPVerifierStage::resolvedDependencies() {
  return _dependencies;
}

std::vector<std::shared_ptr<oo::OXPVerifierStage>>
oo::OXPVerifierStage::resolvedDependents() {
  return _dependents;
}

// Private (was the OOPrivate category).

void oo::OXPVerifierStage::registerDepedent(oo::OXPVerifierStage* dependent) {
  assert(!isDependentOf(dependent));

  AddStage(_dependents, dependent);
}

void oo::OXPVerifierStage::dependencyCompleted(
    oo::OXPVerifierStage* dependency) {
  const auto where =
      std::ranges::find(_incompleteDependencies, dependency,
                        [](const auto& owner) { return owner.get(); });
  if (where != _incompleteDependencies.end())
    _incompleteDependencies.erase(where);
  if (_incompleteDependencies.empty()) _canRun = true;
}

// -makeObjectsPerformSelector:withObject: over the dependents: each is told
// once, in the order they registered.
void oo::OXPVerifierStage::notifyDependents() {
  const std::vector<std::shared_ptr<oo::OXPVerifierStage>> dependents =
      _dependents;
  for (const auto& dependent : dependents) {
    dependent->dependencyCompleted(this);
  }
}

#endif  // OO_OXP_VERIFIER_ENABLED
