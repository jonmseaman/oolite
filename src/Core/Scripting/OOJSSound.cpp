/*

OOJSSound.mm

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

#include <cstring>

#include "OOJSPrivateObject.h"
#include "OOJSSound.h"
#include "OOJSSoundSource.h"
#include "OOJavaScriptEngine.h"
#include "OOMusicController.h"
#include "OOSound.h"
#include "ResourceManager.h"
#include "Universe.h"
#include "oofnd/String.hpp"
#include "ooscript/JSEngine.hpp"

/*
        The shared OOJSObjectWrapperToString, OOJSObjectWrapperFinalize and
   OOJSUnconstructableConstruct natives are used as-is in the class and method
   tables.
*/

namespace ooscript {}
using ooscript::CallArgs;
using ooscript::ClassDef;
using ooscript::ClassFlag;
using ooscript::Context;
using ooscript::FunctionSpec;
using ooscript::Object;
using ooscript::PropertyFlag;
using ooscript::PropertyId;
using ooscript::PropertySpec;
using ooscript::Value;

namespace {
static ooscript::Object sSoundPrototype;
}  // namespace

namespace {
static OOSound* GetNamedSound(const std::string& name);
}  // namespace

// OOJSSoundHolder (OOJSSound.h): the JS glue of a Sound object's slot.
OOJSSoundHolder::OOJSSoundHolder(OOSound* inSound) : _sound(inSound) {}
OOJSSoundHolder::~OOJSSoundHolder() = default;
OOSound* OOJSSoundHolder::sound() const { return _sound.get(); }
ooscript::Value OOJSSoundHolder::jsValueInContext(ooscript::Context context) {
  return OOJSSoundJSValueInContext(_sound.get(), context);
}
void OOJSSoundHolder::clearJSSelf(ooscript::Object) {}  // no wrapper is kept
std::optional<std::string> OOJSSoundHolder::jsDescription() {
  return OOJSSoundJSDescription(_sound.get());
}

namespace {
static bool SoundGetProperty(Context cx, Object obj, PropertyId propID,
                             Value* value);
}  // namespace

// Static methods
namespace {
static bool SoundToString(ooscript::Context cx, ooscript::CallArgs& oojsArgs);
}  // namespace
namespace {
static bool SoundStaticLoad(ooscript::Context cx, ooscript::CallArgs& oojsArgs);
}  // namespace
namespace {
static bool SoundStaticMusicSoundSource(ooscript::Context cx,
                                        ooscript::CallArgs& oojsArgs);
}  // namespace
namespace {
static bool SoundStaticPlayMusic(ooscript::Context cx,
                                 ooscript::CallArgs& oojsArgs);
}  // namespace
namespace {
static bool SoundStaticStopMusic(ooscript::Context cx,
                                 ooscript::CallArgs& oojsArgs);
}  // namespace

namespace {
static ClassDef sSoundClass = {
    "Sound",
    ClassFlag::HasPrivate,

    nullptr,           // addProperty (engine default: PropertyStub)
    nullptr,           // delProperty (engine default: PropertyStub)
    SoundGetProperty,  // getProperty
    nullptr,           // setProperty (engine default: StrictPropertyStub)
    nullptr,           // enumerate (engine default: EnumerateStub)
    nullptr,  // newEnumerate (ooscript::ClassFlag::NewEnumerate not used)
    nullptr,  // resolve (engine default: ResolveStub)
    nullptr,  // convert (engine default: ConvertStub)
    OOJSCxxObjectWrapperFinalize,  // finalize
    nullptr,                       // call
    nullptr,                       // construct
    nullptr,  // backend: owned by the façade backend, must start null
};
}  // namespace

enum : std::uint8_t {
  // Property IDs
  kSound_name
};

namespace {
static PropertySpec sSoundProperties[] = {
    // JS name					ID
    // flags getter	setter
    {"name", kSound_name,
     PropertyFlag::Permanent | PropertyFlag::Enumerate |
         PropertyFlag::ReadOnly | PropertyFlag::Shared,
     nullptr, nullptr},
    {0}};
}  // namespace

namespace {
static FunctionSpec sSoundMethods[] = {
    // JS name					Function
    // min args
    {
        "toString",
        SoundToString,
        0,
        0,
    },
    {0}};
}  // namespace

namespace {
static FunctionSpec sSoundStaticMethods[] = {
    // JS name					Function
    // min args
    {
        "load",
        SoundStaticLoad,
        1,
        0,
    },
    {
        "musicSoundSource",
        SoundStaticMusicSoundSource,
        0,
        0,
    },
    {
        "playMusic",
        SoundStaticPlayMusic,
        1,
        0,
    },
    {
        "stopMusic",
        SoundStaticStopMusic,
        0,
        0,
    },
    {0}};
}  // namespace

// DEFINE_JS_OBJECT_GETTER's equivalent for the slot's holder
// (OOJSPrivateObject.h): the sound, or null for Sound.prototype, which has
// none.
namespace {
static bool JSSoundGetSound(ooscript::Context context, ooscript::Object object,
                            OOSound** outSound) {
  OOJSSoundHolder* holder = nullptr;
  if (!OOJSGetCxxPrivate(context, object, &sSoundClass, &holder)) return false;
  *outSound = (holder != nullptr) ? holder->sound() : nullptr;
  return true;
}
}  // namespace

// The class's converter: no Objective-C object is left to hand over; the
// sound's own callers ask SoundFromJSValue().
namespace {
static oo::PList SoundConverter(ooscript::Context, ooscript::Object) {
  return oo::PList();
}
}  // namespace

// *** Public ***

void InitOOJSSound(ooscript::Context context, ooscript::Object global) {
  Object proto = ooscript::initClass(
      (context), (global), nullptr, &sSoundClass, OOJSUnconstructableConstruct,
      0, sSoundProperties, sSoundMethods, nullptr, sSoundStaticMethods);
  sSoundPrototype = (proto);
  OOJSRegisterObjectConverter(&sSoundClass, SoundConverter);
}

OOSound* SoundFromJSValue(ooscript::Context context, ooscript::Value value) {
  OOJS_PROFILE_ENTER

  OOJSPauseTimeLimiter();
  if ((PLAYER != nullptr ? PLAYER->status() : OOEntityStatus{}) !=
          STATUS_START_GAME &&
      ooscript::isString(value)) {
    return GetNamedSound(
        cxx_OOStringFromJSValue(context, value).value_or(std::string()));
  } else {
    // OOJSNativeObjectOfClassFromJSValue(context, value, [OOSound class]): the
    // sound of a Sound object, else null.
    if (!ooscript::isObject(value)) return nullptr;
    ooscript::Object object = ooscript::toObject(value);
    if (object == nullptr ||
        !OOJSIsMemberOfSubclass(context, object, &sSoundClass))
      return nullptr;
    OOJSSoundHolder* holder = static_cast<OOJSSoundHolder*>(
        static_cast<oo::RefCounted*>(ooscript::getPrivate(context, object)));
    return (holder != nullptr) ? holder->sound() : nullptr;
  }
  OOJSResumeTimeLimiter();

  OOJS_PROFILE_EXIT
}

// *** Implementation stuff ***

namespace {
static bool SoundGetProperty(Context cx, Object obj, PropertyId propID,
                             Value* value) {
  if (!ooscript::isInt32Id(propID)) return true;

  ooscript::Context context = (cx);
  ooscript::Object thisObj = (obj);

  OOJS_NATIVE_ENTER(context)

  OOSound* sound = nullptr;  // null for Sound.prototype

  if (EXPECT_NOT(!JSSoundGetSound(context, thisObj, &sound))) return false;

  switch (ooscript::idToInt32(propID)) {
    case kSound_name: {
      const std::optional<std::string> name =
          (sound != nullptr) ? sound->name()
                             : std::nullopt;  // a message to nil
      *value = OOJSValueFromPList(
          context, name.has_value() ? oo::PList(*name) : oo::PList());
      return true;
    }

    default:
      OOJSReportBadPropertySelector(context, thisObj, (propID),
                                    sSoundProperties);
      return false;
  }

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static OOSound* GetNamedSound(const std::string& name) {
  OOSound* sound = nullptr;

  if (oo::str::hasPrefix(name, "[") && oo::str::hasSuffix(name, "]")) {
    sound = OOSoundWithCustomSoundKey(name);
  } else {
    sound = ResourceManager::ooSoundNamed(name, "Sounds");
  }

  return sound;
}
}  // namespace

// *** Methods ***

// toString() : String
namespace {
static bool SoundToString(ooscript::Context cx, ooscript::CallArgs& oojsArgs) {
  return OOJSCxxObjectWrapperToString(cx, oojsArgs, &sSoundClass);
}
}  // namespace

// *** Static methods ***

// load(name : String) : Sound
namespace {
static bool SoundStaticLoad(ooscript::Context context,
                            ooscript::CallArgs& oojsArgs) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> name;
  OOSound* sound = nullptr;

  if (oojsArgs.count() > 0)
    name = cxx_OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (!name.has_value()) {
    cxx_OOJSReportBadArguments(context, "Sound", "load",
                               MIN(oojsArgs.count(), 1U), OOJS_ARGV,
                               std::nullopt, "string");
    return false;
  }

  OOJS_BEGIN_FULL_NATIVE(context)
  sound = GetNamedSound(*name);
  OOJS_END_FULL_NATIVE

  OOJS_RETURN(OOJSSoundJSValueInContext(sound, context));

  OOJS_NATIVE_EXIT
}
}  // namespace

namespace {
static bool SoundStaticMusicSoundSource(ooscript::Context context,
                                        ooscript::CallArgs& oojsArgs) {
  OOJS_NATIVE_ENTER(context)
  OOSoundSource* musicSource = nullptr;
  OOJS_BEGIN_FULL_NATIVE(context)
  musicSource = OOMusicController::sharedController()->soundSource();
  OOJS_END_FULL_NATIVE
  OOJS_RETURN(OOJSSoundSourceJSValueInContext(musicSource, context));
  OOJS_NATIVE_EXIT
}
}  // namespace

// playMusic(name : String [, loop : Boolean] [, gain : float])
namespace {
static bool SoundStaticPlayMusic(ooscript::Context context,
                                 ooscript::CallArgs& oojsArgs) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> name;
  bool loop = false;
  double gain = OO_DEFAULT_SOUNDSOURCE_GAIN;

  if (oojsArgs.count() > 0)
    name = cxx_OOStringFromJSValue(context, OOJS_ARGV[0]);
  if (!name.has_value()) {
    cxx_OOJSReportBadArguments(context, "Sound", "playMusic",
                               MIN(oojsArgs.count(), 1U), OOJS_ARGV,
                               std::nullopt, "string");
    return false;
  }
  if (oojsArgs.count() > 1) {
    if (!ooscript::valueToBoolean(context, (OOJS_ARGV[1]), &loop)) {
      cxx_OOJSReportBadArguments(context, "Sound", "playMusic", 1,
                                 OOJS_ARGV + 1, std::nullopt, "boolean");
      return false;
    }
  }

  if (oojsArgs.count() > 2) {
    if (!cxx_OOJSArgumentListGetNumber(context, "Sound", "playMusic", 2,
                                       OOJS_ARGV + 2, &gain, NULL)) {
      cxx_OOJSReportBadArguments(context, "Sound", "playMusic", 1,
                                 OOJS_ARGV + 2, std::nullopt, "float");
      return false;
    }
  }

  OOJS_BEGIN_FULL_NATIVE(context)
  OOMusicController::sharedController()->playMusicNamed(
      *name, (loop ? true : false), (float)gain);
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}
}  // namespace

// Sound.stopMusic([name : String])
namespace {
static bool SoundStaticStopMusic(ooscript::Context context,
                                 ooscript::CallArgs& oojsArgs) {
  OOJS_NATIVE_ENTER(context)

  std::optional<std::string> name;

  if (oojsArgs.count() > 0) {
    name = cxx_OOStringFromJSValue(context, OOJS_ARGV[0]);
    if (EXPECT_NOT(!name.has_value())) {
      cxx_OOJSReportBadArguments(context, "Sound", "stopMusic",
                                 oojsArgs.count(), OOJS_ARGV, std::nullopt,
                                 "string or no argument");
      return false;
    }
  }

  OOJS_BEGIN_FULL_NATIVE(context)
  OOMusicController* controller = OOMusicController::sharedController();
  if (!name.has_value() || name == controller->playingMusic()) {
    OOMusicController::sharedController()->stop();
  }
  OOJS_END_FULL_NATIVE

  OOJS_RETURN_VOID;

  OOJS_NATIVE_EXIT
}
}  // namespace

// The bodies of OOSound (OOJavaScriptExtentions). A null sound is JS null.
ooscript::Value OOJSSoundJSValueInContext(OOSound* sound,
                                          ooscript::Context context) {
  ooscript::Object jsSelf = NULL;
  ooscript::Value result = ooscript::nullValue();

  if (sound == nullptr) return result;

  jsSelf = (ooscript::newObject((context), &sSoundClass, (sSoundPrototype),
                                nullptr));
  if (jsSelf != NULL) {
    const oo::Ref<OOJSSoundHolder> holder = oo::makeRef<OOJSSoundHolder>(sound);
    if (!OOJSSetCxxPrivate(context, jsSelf, holder.get())) jsSelf = NULL;
  }
  if (jsSelf != NULL) result = ooscript::objectValue(jsSelf);

  return result;
}

std::optional<std::string> OOJSSoundJSDescription(OOSound* sound) {
  return oo::str::format("[Sound \"%s\"]",
                         ((sound != nullptr) ? sound->name() : std::nullopt)
                             .value_or("(null)")
                             .c_str());
}

std::optional<std::string> OOJSSoundJSClassName(void) {
  return std::string("Sound");
}
