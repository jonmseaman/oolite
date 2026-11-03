/*	test_OOJSExhaustPlume.cpp
        Unit tests for the ExhaustPlume JS binding
   (src/Core/Scripting/OOJSExhaustPlume.h/.cpp) and its OOExhaustPlumeEntity
   members (the C++ class's overrides of the root's JS members).

        It runs the JS class in a real QuickJS-ng context, links the game's own
   objects for the binding, the engine's class registration (OOJSClasses.cpp),
   the private-slot glue (OOJSPrivateObject.cpp) and the exception translator
   (OOJSEngineNativeWrappers.cpp), and stands in for the entity classes (Entity
   and ShipEntity answer only the members the binding calls), for the Vector3D
   conversions (a vector is the array [x, y, z] here) and for the rest of the
   engine (OOJavaScriptEngine.cpp), with the engine headers' linkage. The plume
   is the C++ OOExhaustPlumeEntity, which the binding finds through its
   object's holder: the stand-in is that C++ class, declared with the game
   header's names and signatures but not its class (OOExhaustPlumeEntity.h
   pulls in the game's classes), under a stand-in C++ root. The binding header
   is not included; InitOOJSExhaustPlume is declared here. The expectations
   were written against the Objective-C file and run on it first; they pin the
   JS-visible behaviour (size, remove(), a stale plume, a non-plume, a native's
   exception) and what the entity answers the engine. Run: tests/run.sh unit
*/

#include <quickjs.h>

#include "OOFoundation.h"
#include "OOMaths.h"
#include "oofnd/PList.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/OOException.h"

// MARK: The entity classes, as far as the binding sees them
// ---------------------------------------

// The C++ root, as far as the binding and the root's JS members reach it.
class Entity : public oo::RefCounted {
 public:
  virtual ~Entity();
  virtual JSClassID jsClassID();
  virtual std::optional<std::string> jsClassName();
  virtual bool isVisibleToScripts();
  Entity* owner();  // the binding calls it (its object's -owner did)
};

/*	A plume whose scale has x = -1 raises from scale(), one with x = -2
   throws a C++ exception, so the test sees what an exception under a native
   becomes.
*/
class OOExhaustPlumeEntity : public Entity {
 public:
  Vector scale();
  void setScale(Vector scale);

  JSClassID jsClassID() override;
  std::optional<std::string> jsClassName() override;
  bool isVisibleToScripts() override;

  Vector _scale = {};
};

// What an entity's JS object holds: a weak reference to the C++ entity.
#include "OOJSEntityHolder.h"

// The entity's object, whose JS members ask its C++ part.
class TestEntityObject final : public oo::RefCounted {
 public:
  JSClassID jsClassID();
  std::optional<std::string> cxx_oo_jsClassName();
  bool isVisibleToScripts();

  oo::Ref<Entity> _cxxEntity;  // the plume's C++ part (oo::ToCxx reads it)
  ::TestEntityObject* _owner = nullptr;
};

// A ship: the C++ part of its object, with the one member the binding calls
// (declared as ShipEntity.h declares it).
class ShipEntity : public Entity {
 public:
  void removeExhaust(OOExhaustPlumeEntity* exhaust);

  OOExhaustPlumeEntity* _removed = nullptr;
};

void InitOOJSExhaustPlume(JSContext* context, JSValueConst global);
JSClassID JSExhaustPlumeClassID(void);
// The bodies the stand-in's overrides call as the C++ class's do (declared in
// OOJSExhaustPlume.h).
std::optional<std::string> OOJSExhaustPlumeJSClassName(void);
bool OOJSExhaustPlumeIsVisibleToScripts(void);

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>

#include "OOJSEngineCore.h"
#include "OOJSPrivateObject.h"
#include "oo_test.hpp"

void ShipEntity::removeExhaust(OOExhaustPlumeEntity* exhaust) {
  _removed = exhaust;
}

// The root's JS members, as the game's ask the C++ part
// (EntityOOJavaScriptExtensions.cpp): the engine asks the wrapped object.
JSClassID TestEntityObject::jsClassID() { return _cxxEntity->jsClassID(); }
std::optional<std::string> TestEntityObject::cxx_oo_jsClassName() {
  return _cxxEntity->jsClassName();
}
bool TestEntityObject::isVisibleToScripts() {
  return _cxxEntity->isVisibleToScripts();
}

Entity::~Entity() {}
JSClassID Entity::jsClassID() { return 0; }
std::optional<std::string> Entity::jsClassName() {
  return std::string("Entity");
}
bool Entity::isVisibleToScripts() { return false; }

Vector OOExhaustPlumeEntity::scale() {
  if (_scale.x == -1)
    OORaiseException(OOInvalidArgumentException, "scale %s", "boom");
  if (_scale.x == -2) throw std::runtime_error("cxx boom");
  return _scale;
}

void OOExhaustPlumeEntity::setScale(Vector scale) { _scale = scale; }

// The binding's members, as the C++ class's overrides answer them.
JSClassID OOExhaustPlumeEntity::jsClassID() {
  return ::JSExhaustPlumeClassID();
}
std::optional<std::string> OOExhaustPlumeEntity::jsClassName() {
  return ::OOJSExhaustPlumeJSClassName();
}
bool OOExhaustPlumeEntity::isVisibleToScripts() {
  return ::OOJSExhaustPlumeIsVisibleToScripts();
}

// MARK: What the rest of the engine provides
// ------------------------------------------------------

JSContext* gOOJSMainThreadContext = nullptr;  // OOJavaScriptEngine.cpp's

void cxx_OOJSReportErrorWithArguments(JSContext* context, const char* format,
                                      va_list args) {
  std::string msg = oo::str::vformat(format, args);
  JS_ThrowPlainError(context, "%s", msg.c_str());
}

void cxx_OOJSReportError(JSContext* context, const char* format, ...) {
  va_list args;
  va_start(args, format);
  cxx_OOJSReportErrorWithArguments(context, format, args);
  va_end(args);
}

void OOJSReportPendingException(JSContext* context) {
  JS_FreeValue(context, JS_GetException(context));
}

// As the engine's: a native that fails with nothing reported ends the script
// with an uncatchable error.
JSValue OOJSNativeFailure(JSContext* context) {
  if (JS_HasException(context)) return JS_EXCEPTION;
  JSValue error = JS_NewError(context);
  JS_SetUncatchableError(context, error);
  return JS_Throw(context, error);
}

std::optional<std::string> cxx_OOStringFromJSValue(JSContext* context,
                                                   JSValueConst value) {
  if (JS_IsNull(value) || JS_IsUndefined(value)) return std::nullopt;
  std::size_t length = 0;
  const char* chars = JS_ToCStringLen(context, &length, value);
  if (chars == nullptr) {
    JS_FreeValue(context, JS_GetException(context));
    return std::nullopt;
  }
  std::string result(chars, length);
  JS_FreeCString(context, chars);
  return result;
}

JSAtom cxx_OOJSIDFromString(const std::string& string) {
  return JS_NewAtomLen(OOJSAcquireContext(), string.data(), string.size());
}

namespace {
JSClassID sFakeEntityClassID = 0;
JSClassDef sFakeEntityClass = {
    .class_name = "Entity",
    .finalizer = OOJSObjectWrapperFinalize,  // the slot holds a holder
};
std::map<JSClassID, int> sConverters;

// Entity's toString(), as the game's (OOJSEntity.cpp).
JSValue FakeEntityToString(OOJS_NATIVE_ARGS) {
  (void)argc;
  (void)oojsArgv;
  return OOJSCxxObjectWrapperToString(context, OOJS_THIS, sFakeEntityClassID);
}
const JSCFunctionListEntry sFakeEntityMethods[] = {
    JS_CFUNC_DEF("toString", 0, FakeEntityToString),
};
}  // namespace

Entity* gOOJSPlayerIfStale = nullptr;  // the player's C++ entity

JSClassID JSEntityClassID(void) { return sFakeEntityClassID; }

void OOJSReportBadPropertySelector(JSContext* context, JSValueConst, int,
                                   const JSCFunctionListEntry*, size_t) {
  cxx_OOJSReportError(context, "bad property selector");
}

void OOJSReportBadPropertyValue(JSContext* context, JSValueConst, int,
                                const JSCFunctionListEntry*, size_t,
                                JSValueConst) {
  cxx_OOJSReportError(context, "bad property value");
}

void OOJSRegisterObjectConverter(JSClassID classID,
                                 OOJSClassConverterCallback) {
  sConverters[classID]++;
}

// The engine's object getter is OOJSPrivateObject.cpp's (linked): the JS class
// must be a subclass of the required one, and the slot holds the entity's
// holder.

// The shared toString(), which OOJSCxxObjectWrapperToString() hands a `this` of
// another class.
JSValue OOJSObjectWrapperToString(OOJS_NATIVE_ARGS) {
  (void)oojsThis;
  (void)argc;
  (void)oojsArgv;
  return JS_NewString(context, "[object]");
}

void OOJSObjectWrapperFinalize(JSRuntime* runtime, JSValueConst thisObj) {
  OOJSCxxObjectWrapperFinalize(runtime, thisObj);
}

JSValue OOJSUnconstructableConstruct(OOJS_NATIVE_ARGS) {
  (void)oojsThis;
  (void)argc;
  (void)oojsArgv;
  cxx_OOJSReportError(context, "unconstructable");
  return JS_EXCEPTION;
}

// A vector is the array [x, y, z] here: enough to see what the binding hands
// over and takes.
bool VectorToJSValue(JSContext* context, Vector vector, JSValue* outValue) {
  JSValue array = JS_NewArray(context);
  if (JS_IsException(array)) return false;
  JS_SetPropertyUint32(context, array, 0, JS_NewNumber(context, vector.x));
  JS_SetPropertyUint32(context, array, 1, JS_NewNumber(context, vector.y));
  JS_SetPropertyUint32(context, array, 2, JS_NewNumber(context, vector.z));
  *outValue = array;
  return true;
}

bool JSValueToVector(JSContext* context, JSValueConst value,
                     Vector* outVector) {
  double v[3] = {};
  if (!JS_IsArray(value)) return false;
  for (std::uint32_t i = 0; i < 3; i++) {
    JSValue element = JS_GetPropertyUint32(context, value, i);
    const bool ok = !JS_IsException(element) &&
                    JS_ToFloat64(context, &v[i], element) == 0;
    JS_FreeValue(context, element);
    if (!ok) return false;
  }
  *outVector = make_vector(v[0], v[1], v[2]);
  return true;
}

int sLimiterPauses = 0;
int sProfileDepth = 0;

void OOJSProfileEnter(struct OOJSProfileStackFrame*, const char*) {
  sProfileDepth++;
}
void OOJSProfileExit(struct OOJSProfileStackFrame*) { sProfileDepth--; }

void OOJSPauseTimeLimiter(void) { sLimiterPauses++; }
void OOJSResumeTimeLimiter(void) { sLimiterPauses--; }

#ifndef NDEBUG
void OOJSUnreachable(const char* function, const char*, unsigned) {
  std::printf("unreachable reached in %s\n", function);
  abort();
}
#endif

oo::PList OOJSEntityObjectConverter(JSContext*, JSValueConst) {
  return oo::PList();
}

// The holder's glue is OOJSEntity.cpp's, which the test does not link; the
// binding never asks it.
JSValue OOJSEntityHolder::jsValueInContext(JSContext*) { return JS_UNDEFINED; }
void OOJSEntityHolder::clearJSSelf(JSValueConst) {}
std::optional<std::string> OOJSEntityHolder::jsDescription() {
  return std::nullopt;
}

// oo::ToObjC (Entity+ObjCBridge.mm): the object of each C++ part the test
// wrapped, nil for any other (the binding asks it where its natives still
// message the object).
namespace {
std::map<Entity*, TestEntityObject*> sObjects;
}
::TestEntityObject* TestObjectOf(Entity* entity);
::TestEntityObject* TestObjectOf(Entity* entity) {
  auto found = sObjects.find(entity);
  return found != sObjects.end() ? found->second : nullptr;
}

// The owner its object's -owner answered, on the same object field.
Entity* Entity::owner() {
  ::TestEntityObject* object = TestObjectOf(this)->_owner;
  return object != nullptr ? object->_cxxEntity.get()
                           : nullptr;  // its C++ part
}

// MARK: The context
// -------------------------------------------------------------------------------

namespace {

JSRuntime* sRuntime = nullptr;
JSContext* sContext = nullptr;
JSValue sGlobal = JS_UNDEFINED;              // held for the life of the test
TestEntityObject* sPlume = nullptr;          // the plume's object
OOExhaustPlumeEntity* sPlumePart = nullptr;  // its C++ part
TestEntityObject* sShip = nullptr;           // a ship's object and its C++ part
ShipEntity* sShipPart = nullptr;

// A JS object of the class, with the class's prototype and the entity's holder
// in the private slot; owned by the caller.
JSValue JSValueForObject(JSClassID jsClass, TestEntityObject* entity) {
  JSValue object = JS_NewObjectClass(sContext, static_cast<int>(jsClass));
  if (entity->_cxxEntity == nullptr)
    entity->_cxxEntity = oo::makeRef<Entity>();  // a plain entity's C++ part
  if (JS_IsException(object) ||
      !OOJSSetCxxPrivate(
          object,
          oo::makeRef<OOJSEntityHolder>(entity->_cxxEntity.get()).get())) {
    JS_FreeValue(sContext, object);
    return JS_NULL;  // the slot holds the C++ entity
  }
  sObjects[entity->_cxxEntity.get()] = entity;
  return object;
}

// A JS object for an entity, as the entity's jsValueInContext() makes it: the
// class its JS members name, and the entity in the private slot.
JSValue JSValueForEntity(TestEntityObject* entity) {
  return JSValueForObject(entity->jsClassID(), entity);
}

// Defines a global; takes value's reference.
void Define(const char* name, JSValue value) {
  JS_SetPropertyStr(sContext, sGlobal, name, value);
}

void SetUpContext() {
  if (sContext != nullptr) return;
  sRuntime = JS_NewRuntime();
  sContext = JS_NewContext(sRuntime);
  gOOJSMainThreadContext = sContext;
  sGlobal = JS_GetGlobalObject(sContext);
  OOJSRegisterClass(&sFakeEntityClassID, &sFakeEntityClass);
  OOJSInitClass(sContext, sGlobal, sFakeEntityClassID, JS_UNDEFINED,
                OOJSUnconstructableConstruct, 0, nullptr, 0,
                sFakeEntityMethods, OOJS_COUNT(sFakeEntityMethods), nullptr,
                0);
  InitOOJSExhaustPlume(sContext, sGlobal);

  sShip = oo::makeRef<TestEntityObject>()
              .leakRef();  // kept for the life of the test
  sShip->_cxxEntity = oo::makeRef<ShipEntity>();
  sShipPart = static_cast<ShipEntity*>(sShip->_cxxEntity.get());
  sPlume = oo::makeRef<TestEntityObject>().leakRef();
  const oo::Ref<OOExhaustPlumeEntity> part =
      oo::makeRef<OOExhaustPlumeEntity>();
  sPlume->_cxxEntity = part;
  sPlumePart = part.get();
  sPlumePart->_scale = make_vector(1, 2, 3.5);
  sPlume->_owner = sShip;
  Define("plume", JSValueForEntity(sPlume));
  Define("ship", JSValueForObject(sFakeEntityClassID, sShip));
}

// Evaluates src and gives its result as a string ("undefined", "null", ...), or
// "threw: <message>".
std::string Eval(const char* src) {
  SetUpContext();
  std::string wrapped = std::string("(function () { try { return String(") +
                        src +
                        "); } catch (e) { return 'threw: ' + (e && e.message "
                        "!== undefined ? e.message : e); } })()";
  JSValue result = JS_Eval(sContext, wrapped.c_str(), wrapped.size(),
                           "test.js", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(result)) {
    JS_FreeValue(sContext, JS_GetException(sContext));
    return "<evaluation failed>";
  }
  std::string text =
      cxx_OOStringFromJSValue(sContext, result).value_or("<not a string>");
  JS_FreeValue(sContext, result);
  return text;
}

// Eval, and print what came back when it is not what the check expects.
std::string EvalShown(const char* src, const char* expected) {
  std::string result = Eval(src);
  if (result != expected)
    std::printf("    %s\n    gave: %s\n", src, result.c_str());
  return result;
}
#define OO_CHECK_EVAL(src, expected) \
  OO_CHECK_EQ(EvalShown(src, expected), expected)

}  // namespace

// MARK: Tests
// -------------------------------------------------------------------------------------

OO_TEST(registration) {
  SetUpContext();
  JSClassID plumeClass = sPlume->jsClassID();
  OO_CHECK(plumeClass != 0 && OOJSClassName(plumeClass) != nullptr &&
           std::strcmp(OOJSClassName(plumeClass), "ExhaustPlume") == 0);
  JSValue prototype = JS_GetClassProto(sContext, plumeClass);
  OO_CHECK(JS_IsObject(prototype));
  JS_FreeValue(sContext, prototype);
  OO_CHECK(OOJSIsSubclass(plumeClass, sFakeEntityClassID));
  OO_CHECK_EQ(sConverters[plumeClass], 1);
  OO_CHECK(sPlume->cxx_oo_jsClassName() ==
           std::optional<std::string>("ExhaustPlume"));
  OO_CHECK(sPlume->isVisibleToScripts());
  OO_CHECK_EVAL("typeof ExhaustPlume", "function");
  OO_CHECK_EVAL("new ExhaustPlume()", "threw: unconstructable");
  OO_CHECK_EVAL(
      "Object.getPrototypeOf(ExhaustPlume.prototype) === Entity.prototype",
      "true");
  OO_CHECK_EVAL("plume instanceof ExhaustPlume", "true");
}

OO_TEST(properties) {
  OO_CHECK_EVAL("plume.size", "1,2,3.5");
  OO_CHECK_EVAL("Object.keys(ExhaustPlume.prototype).join()", "size");
  OO_CHECK_EVAL(
      "(function () { plume.size = [4, 5, 6]; return plume.size; })()",
      "4,5,6");
  OO_CHECK(sPlumePart->_scale.x == 4 && sPlumePart->_scale.y == 5 &&
           sPlumePart->_scale.z == 6);
  OO_CHECK_EVAL("(function () { plume.size = 'big'; return plume.size; })()",
                "threw: bad property value");
  OO_CHECK_EVAL("plume.size", "4,5,6");
}

OO_TEST(remove) {
  SetUpContext();
  sShipPart->_removed = nullptr;
  OO_CHECK_EVAL("plume.remove()", "undefined");
  OO_CHECK(sShipPart->_removed == sPlumePart);
  // The prototype has no entity: the binding's getter fails without reporting
  // an error, which ends the script uncatchably, and nothing is removed.
  sShipPart->_removed = nullptr;
  OO_CHECK_EVAL("ExhaustPlume.prototype.remove()", "<evaluation failed>");
  OO_CHECK(sShipPart->_removed == nullptr);
}

OO_TEST(otherObjects) {
  // An entity that is not a plume, and the prototype: the binding's getter
  // fails without reporting an error, which ends the script uncatchably.
  OO_CHECK_EVAL(
      "Object.getOwnPropertyDescriptor(ExhaustPlume.prototype, "
      "'size').get.call(ship)",
      "<evaluation failed>");
  OO_CHECK_EVAL("ExhaustPlume.prototype.remove.call(ship)",
                "<evaluation failed>");
  OO_CHECK_EVAL("ExhaustPlume.prototype.size", "<evaluation failed>");
  // Something that is not an entity: the engine's getter reports it.
  OO_CHECK_EVAL(
      "Object.getOwnPropertyDescriptor(ExhaustPlume.prototype, "
      "'size').get.call({})",
      "threw: Native method expected Entity, got [object Object].");
  OO_CHECK_EVAL("plume.size", "4,5,6");
}

OO_TEST(nativeExceptions) {
  SetUpContext();
  // The property getter is a native (OOJS_NATIVE_ENTER): a raise from the
  // entity is a JS error, and so is a C++ exception.
  sPlumePart->_scale = make_vector(-1, 0, 0);
  OO_CHECK_EVAL("plume.size", "threw: Native exception: scale boom");
  sPlumePart->_scale = make_vector(-2, 0, 0);
  OO_CHECK_EVAL("plume.size", "threw: Native exception: cxx boom");
  sPlumePart->_scale = make_vector(1, 1, 1);
  OO_CHECK_EVAL("plume.size", "1,1,1");
  OO_CHECK_EQ(sLimiterPauses, 0);
  OO_CHECK_EQ(sProfileDepth, 0);
}

OO_TEST_MAIN()
