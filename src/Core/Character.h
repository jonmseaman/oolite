/*

OOCharacter.h

Represents an NPC person (as opposed to an NPC ship).

Ships' crews and the character pool hold std::shared_ptr<OOCharacter>, and a
character is its own PList::Object payload (oo::PListForeign) for its script's
"character".

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

#pragma once

#include <memory>
#include <optional>
#include <string>

#include "OOJSPropID.h"
#include "OOScript.h"  // std::shared_ptr<OOScript> members and results
#include "Types.h"
#include "legacy_random.h"
#include "oofnd/PList.hpp"

namespace oo {

class Character : public oo::PListForeign {
 public:
  Character() = default;  // [[OOCharacter alloc] init]: every field zero
  Character(const std::string& role, SystemID s);

  static std::shared_ptr<Character> characterWithRole(const std::string& c_role,
                                                      SystemID s);
  static std::shared_ptr<Character> randomCharacterWithRole(
      const std::string& c_role, SystemID s);
  // a dictionary (JS crew definitions may hold any object, as an Object node)
  static std::shared_ptr<Character> characterWithDictionary(
      const oo::PList& c_dict);

  std::optional<std::string> planetOfOrigin();
  SystemID planetIDOfOrigin();
  std::optional<std::string> species();

  void basicSetUp();
  bool castInRole(const std::string& role);

  std::optional<std::string> name();  // nullopt: none
  void setName(const std::optional<std::string>& value);

  std::optional<std::string> shortDescription();
  // nullopt: none
  void setShortDescription(const std::optional<std::string>& value);

  int legalStatus();
  void setLegalStatus(int value);

  CreditsQuantity insuranceCredits();
  void setInsuranceCredits(CreditsQuantity value);

  oo::PList legacyScript();  // an array of script actions; null: none
  void setLegacyScript(const oo::PList& scriptActions);
  ::OOScript* script();
  void setCharacterScript(const std::string& scriptName);
  void doScriptEvent(OOJSPropID message);

  oo::PList infoForScripting();  // a dictionary

  // What "%@" prints between the braces of <OOCharacter 0x...>{...}
  // (OODescription.h).
  std::optional<std::string> descriptionComponents() const;
  // The JavaScript class the character answers.
  std::optional<std::string> oo_jsClassName();

  // oo::PListForeign: the class name, and "%@" as <OOCharacter 0x...>{...}.
  std::string className() const override;
  std::string description() const override;

 private:
  Character(Random_Seed characterSeed,
            SystemID systemSeed);  // -initWithGenSeed:andOriginalSystem:
  void setCharacterFromDictionary(const oo::PList& dict);

  void setOriginSystem(SystemID value);
  Random_Seed genSeed();
  void setGenSeed(Random_Seed value);

  std::optional<std::string> _name;
  std::optional<std::string> _shortDescription;
  SystemID _originSystem = {};
  Random_Seed _genSeed = {};
  int _legalStatus = {};
  CreditsQuantity _insuranceCredits = {};
  oo::PList _scriptActions;  // an array; null: none
  std::shared_ptr<OOScript> _script;
};

}  // namespace oo
