/*

OOJSPopulatorDefinition.mm


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

#include "OOJSPopulatorDefinition.h"
#include "OOJSScript.h"
#include "OOJSVector.h"
#include "OOJavaScriptEngine.h"
#include "OOMaths.h"
#include "oofnd/Notification.hpp"
#include "ooscript/JSEngine.hpp"

/*
        The two byte-identical façade <-> jsapi view helpers below are local to
   this call site.
*/

namespace ooscript {}
using ooscript::Context;
using ooscript::Object;
using ooscript::Value;

namespace {
static inline Object* OOJSFOBJP(ooscript::Object* o) {
  return reinterpret_cast<Object*>(o);
}
}  // namespace

OOJSPopulatorDefinition::OOJSPopulatorDefinition() {
  _callback = ooscript::undefinedValue();
  _callbackThis = NULL;

  _owningScript = OOJSScript::currentlyRunningScript();

  oo::NotificationCenter::defaultCenter().addObserver(
      this, kOOJavaScriptEngineWillResetNotificationName,
      OOJavaScriptEngine::sharedEngine(),
      [this](const oo::Notification&) { deleteJSPointers(); });
}

void OOJSPopulatorDefinition::deleteJSPointers() {
  ooscript::Context context = OOJSAcquireContext();
  _callback = ooscript::undefinedValue();
  _callbackThis = NULL;
  ooscript::removeValueRoot((context), (&_callback));
  ooscript::removeObjectRoot((context), OOJSFOBJP(&_callbackThis));

  OOJSRelinquishContext(context);

  oo::NotificationCenter::defaultCenter().removeObserver(
      this, kOOJavaScriptEngineWillResetNotificationName,
      OOJavaScriptEngine::sharedEngine());
}

OOJSPopulatorDefinition::~OOJSPopulatorDefinition() {
  _owningScript = nullptr;

  deleteJSPointers();
}

ooscript::Value OOJSPopulatorDefinition::callback() { return _callback; }

void OOJSPopulatorDefinition::setCallback(ooscript::Value callback) {
  ooscript::Context context = OOJSAcquireContext();
  ooscript::removeValueRoot((context), (&_callback));
  _callback = callback;
  OOJSAddGCValueRoot(context, &_callback,
                     "OOJSPopulatorDefinition callback function");
  OOJSRelinquishContext(context);
}

ooscript::Object OOJSPopulatorDefinition::callbackThis() {
  return _callbackThis;
}

void OOJSPopulatorDefinition::setCallbackThis(ooscript::Object callbackThis) {
  ooscript::Context context = OOJSAcquireContext();
  ooscript::removeObjectRoot((context), OOJSFOBJP(&_callbackThis));
  _callbackThis = callbackThis;
  OOJSAddGCObjectRoot(context, &_callbackThis,
                      "OOJSPopulatorDefinition callback this");
  OOJSRelinquishContext(context);
}

void OOJSPopulatorDefinition::runPopulatorCallback(HPVector location) {
  ::OOJavaScriptEngine* engine = OOJavaScriptEngine::sharedEngine();
  ooscript::Context context = OOJSAcquireContext();
  ooscript::Value loc, rval = ooscript::undefinedValue();

  VectorToJSValue(context, HPVectorToVector(location), &loc);

  const oo::WeakRef<OOJSScript> owner = _owningScript;  // local copy needed
  OOJSScript::pushScript(owner);

  engine->callJSFunction(_callback, _callbackThis, 1, &loc, &rval);

  OOJSScript::popScript(owner.get());

  OOJSRelinquishContext(context);
}

namespace {
class PopulatorDefinitionForeign final : public oo::PListForeign {
 public:
  explicit PopulatorDefinitionForeign(
      oo::Ref<OOJSPopulatorDefinition> definition)
      : definition_(std::move(definition)) {}
  OOJSPopulatorDefinition* definition() const noexcept {
    return definition_.get();
  }
  std::string className() const override { return "OOJSPopulatorDefinition"; }
  std::string description() const override {
    return "<OOJSPopulatorDefinition>";
  }

 private:
  oo::Ref<OOJSPopulatorDefinition> definition_;
};
}  // namespace

oo::PList OOJSPopulatorDefinitionToPList(
    oo::Ref<OOJSPopulatorDefinition> definition) {
  if (!definition) return oo::PList();
  return oo::PList(oo::PList::Object(
      oo::makeRef<PopulatorDefinitionForeign>(std::move(definition))));
}

OOJSPopulatorDefinition* OOJSPopulatorDefinitionIn(const oo::PList& plist) {
  if (const oo::PList::Object* node = plist.getIf<oo::PList::Object>()) {
    if (const auto* foreign =
            dynamic_cast<const PopulatorDefinitionForeign*>(node->get()))
      return foreign->definition();
  }
  return nullptr;
}
