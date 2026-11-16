/*

OOModelVerifierStage.m


Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#include "ModelVerifierStage.h"

#include <format>
#include <memory>
#include <string>
#include <utility>

#if OO_OXP_VERIFIER_ENABLED

#include "FileScannerVerifierStage.h"
#include "oofnd/Log.hpp"

static const char* const kStageName = "Testing models";

const char* const oo::ModelVerifierStage::kName = kStageName;

std::string oo::ModelVerifierStage::nameForReverseDependencyForVerifier(
    oo::OXPVerifier* verifier) {
  oo::OXPVerifierStage* stage = verifier->stageWithName(kStageName);
  if (stage == nullptr) {
    const std::shared_ptr<oo::ModelVerifierStage> newStage =
        oo::makeShared<oo::ModelVerifierStage>();
    verifier->registerStage(newStage.get());
  }

  return kStageName;
}

std::optional<std::string> oo::ModelVerifierStage::name() { return kStageName; }

bool oo::ModelVerifierStage::shouldRun() { return !_modelsToCheck.empty(); }

void oo::ModelVerifierStage::run() {
  OO_LOG("verifyOXP.models.unimplemented", "{}",
         "TODO: implement model verifier.");

  for (const oo::ModelVerifierEntry& info : _modelsToCheck) {
    {
      checkModel(info.name, info.context, info.materials, info.shaders);
    }
  }
  _modelsToCheck.clear();
}

bool oo::ModelVerifierStage::modelNamed(
    const std::string& name, const std::optional<std::string>& entryName,
    const std::string& fileName, const oo::PList& materials,
    const oo::PList& shaders) {
  oo::FileScannerVerifierStage* fileScanner = nullptr;
  std::string context;

  if (name.empty()) return false;

  if (entryName.has_value())
    context = std::format("entry \"{}\" of {}", *entryName, fileName);
  else
    context = fileName;

  fileScanner = static_cast<oo::FileScannerVerifierStage*>(
      verifier()->stageWithName(oo::FileScannerVerifierStage::kName));
  if (fileScanner == nullptr ||
      !fileScanner->fileExists(name, "Models", context, true)) {
    return false;
  }

  oo::ModelVerifierEntry info{name, context, materials, shaders};
  if (std::find(_modelsToCheck.begin(), _modelsToCheck.end(), info) ==
      _modelsToCheck.end()) {
    _modelsToCheck.push_back(std::move(info));
  }

  return true;
}

void oo::ModelVerifierStage::checkModel(const std::string& name,
                                        const std::string& context,
                                        const oo::PList&, const oo::PList&) {
  OO_LOG("verifyOXP.verbose.model.unimp",
         "- Pretending to verify model {} referenced in {}.", name, context);
  // FIXME: this should check DAT files.
}

#endif
