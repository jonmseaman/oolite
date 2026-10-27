/*

OOJSGuiScreenKeyDefinition.m


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

#include "OOJSGuiScreenKeyDefinition.h"
#include "OOJSScript.h"
// #import "OOJavaScriptEngine.h"

#include "oofnd/Notification.hpp"
#include "oofnd/String.hpp"
#include "ooscript/JSEngine.hpp"

namespace ooscript {}
using ooscript::Context;
using ooscript::Object;
using ooscript::Value;

// Byte-identical façade <-> jsapi views, local to this call site (JSEngine.hpp:
// Value/Object are byte copies of ooscript::Value/ooscript::Object ; see
// OOJSVector.mm for the same, non-exported, pattern).
namespace {
static inline Object* OOJSFOBJP(ooscript::Object* o) {
  return reinterpret_cast<Object*>(o);
}
}  // namespace

OOJSGuiScreenKeyDefinition::OOJSGuiScreenKeyDefinition() {
  _callback = ooscript::undefinedValue();
  _callbackThis = NULL;

  _owningScript = OOJSScript::currentlyRunningScript();

  oo::NotificationCenter::defaultCenter().addObserver(
      this, kOOJavaScriptEngineWillResetNotificationName,
      OOJavaScriptEngine::sharedEngine(),
      [this](const oo::Notification&) { deleteJSPointers(); });
}

void OOJSGuiScreenKeyDefinition::deleteJSPointers() {
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

OOJSGuiScreenKeyDefinition::~OOJSGuiScreenKeyDefinition() {
  _owningScript = nullptr;

  deleteJSPointers();
}

std::optional<std::string> OOJSGuiScreenKeyDefinition::name() { return _name; }

void OOJSGuiScreenKeyDefinition::setName(
    const std::optional<std::string>& name) {
  _name = name;
}

oo::PList OOJSGuiScreenKeyDefinition::registerKeys() { return _registerKeys; }

void OOJSGuiScreenKeyDefinition::setRegisterKeys(
    const oo::PList& registerKeys) {
  _registerKeys = registerKeys;
}

ooscript::Value OOJSGuiScreenKeyDefinition::callback() { return _callback; }

void OOJSGuiScreenKeyDefinition::setCallback(ooscript::Value callback) {
  ooscript::Context context = OOJSAcquireContext();
  ooscript::removeValueRoot((context), (&_callback));
  _callback = callback;
  OOJSAddGCValueRoot(context, &_callback,
                     "OOJSGuiScreenKeyDefinition callback function");
  OOJSRelinquishContext(context);
}

ooscript::Object OOJSGuiScreenKeyDefinition::callbackThis() {
  return _callbackThis;
}

void OOJSGuiScreenKeyDefinition::setCallbackThis(
    ooscript::Object callbackThis) {
  ooscript::Context context = OOJSAcquireContext();
  ooscript::removeObjectRoot((context), OOJSFOBJP(&_callbackThis));
  _callbackThis = callbackThis;
  OOJSAddGCObjectRoot(context, &_callbackThis,
                      "OOJSGuiScreenKeyDefinition callback this");
  OOJSRelinquishContext(context);
}

void OOJSGuiScreenKeyDefinition::runCallback(const std::string& key) {
  ::OOJavaScriptEngine* engine = OOJavaScriptEngine::sharedEngine();
  ooscript::Context context = OOJSAcquireContext();
  ooscript::Value rval = ooscript::undefinedValue();

  ooscript::Value cKey = OOJSValueFromPList(context, oo::PList(key));

  const oo::WeakRef<OOJSScript> owner = _owningScript;  // local copy needed
  OOJSScript::pushScript(owner);

  engine->callJSFunction(_callback, _callbackThis, 1, &cKey, &rval);

  OOJSScript::popScript(owner.get());

  OOJSRelinquishContext(context);
}

OOComparisonResult OOJSGuiScreenKeyDefinition::interfaceCompare(
    OOJSGuiScreenKeyDefinition* other) {
  // -caseInsensitiveCompare: as it was sent: a nil name answers OOOrderedSame
  // (a message to nil); a nil other name compares as the empty string.
  if (!_name.has_value()) return OOOrderedSame;
  int order = oo::str::caseInsensitiveCompare(
      *_name, ((other != nullptr) ? other->name() : std::nullopt)
                  .value_or(std::string()));
  return (order < 0) ? OOOrderedAscending
                     : ((order > 0) ? OOOrderedDescending : OOOrderedSame);
}
