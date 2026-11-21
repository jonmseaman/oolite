/*

OOShaderUniform.m


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

#include "ShaderUniform.h"

#include <memory>
#include <string>

#include "Entity.h"  // the shader binding targets' informal protocols (-superShaderBindingTarget)

#if OO_SHADERS

#include <format>
#include <optional>
#include <string_view>
#include <variant>

#include "Maths.h"
#include "ShaderProgram.h"
#include "ShaderUniformMethodType.h"
#include "oofnd/String.hpp"

// The public initialisers, each [[self alloc] init...] of the old (self is
// result): a new uniform whose initialiser answered nil is dropped (null)
// instead of released.

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    GLint constValue) {
  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    result->type = oo::kShaderUniformTypeInt;
    result->value = constValue;
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    GLfloat constValue) {
  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    result->type = oo::kShaderUniformTypeFloat;
    result->value = constValue;
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    GLfloat constValue[4]) {
  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    result->type = oo::kShaderUniformTypeVector;
    result->value = std::array<GLfloat, 4>{constValue[0], constValue[1],
                                           constValue[2], constValue[3]};
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    oo::Color* constValue) {
  if (constValue == nullptr) [[unlikely]] {
    return nullptr;
  }

  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    result->type = oo::kShaderUniformTypeVector;
    result->value = std::array<GLfloat, 4>{
        constValue->redComponent(), constValue->greenComponent(),
        constValue->blueComponent(), constValue->alphaComponent()};
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    Quaternion constValue, bool asMatrix) {
  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    if (asMatrix) {
      result->type = oo::kShaderUniformTypeMatrix;
      result->value = oo::MatrixForQuaternionRotation(constValue);
    } else {
      result->type = oo::kShaderUniformTypeVector;
      result->value = std::array<GLfloat, 4>{constValue.x, constValue.y,
                                             constValue.z, constValue.w};
    }
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    oo::Matrix constValue) {
  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  if (!result->initWithName(uniformName, shaderProgram)) return nullptr;
  {
    result->type = oo::kShaderUniformTypeMatrix;
    result->value = constValue;
  }

  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithName(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    oo::Object* target, const std::string& property, uint8_t boundType,
    oo::ShaderCxxBindingGetter get, oo::UniformConvertOptions options) {
  /*	The selector initialiser's checks and log line, for a C++ target's
     member: the member's getter replaces the method found by selector, and the
     type is the method's return type it replaced. A target that is itself a
     C++ object has no super target.
  */
  if (shaderProgram == nullptr || get == nullptr) [[unlikely]]
    return nullptr;
  if (boundType == oo::kShaderUniformTypeInvalid ||
      boundType >= oo::kShaderUniformTypeCount) [[unlikely]]
    return nullptr;

  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  result->location =
      glGetUniformLocationARB(shaderProgram->program(), uniformName.c_str());
  if (result->location == -1) {
    OO_LOG("shader.uniform.bind.failed",
           "Could not bind uniform \"{}\" to {} (no uniform of that name could "
           "be found).",
           uniformName, property);
    return nullptr;
  }

  result->name = uniformName;
  result->isBinding = true;
  result->convertClamp = oo::Any(options & oo::UniformConvertOptions::kClamp);
  result->convertNormalize =
      oo::Any(options & oo::UniformConvertOptions::kNormalize);
  result->convertToMatrix =
      oo::Any(options & oo::UniformConvertOptions::kToMatrix);
  result->type = boundType;
  result->value = Binding{get};
  result->cxxTargetOnly = true;
  result->cxxProperty = property;
  result->cxxTarget = oo::share(target);
  result->isActiveBinding = target != nullptr;
  return result;
}

std::shared_ptr<oo::ShaderUniform> oo::ShaderUniform::initWithEntityTarget(
    const std::string& uniformName, oo::ShaderProgram* shaderProgram,
    Entity* target, const char* property, oo::UniformConvertOptions options) {
  // The selector initialiser's checks and log line, its target the entity: the
  // same checks and log line, which names the class of the entity's object as
  // -[{} {}] did. The property is kept by name, where its selector was looked
  // up (a NULL name gave a NULL selector).
  if (shaderProgram == nullptr || property == nullptr) [[unlikely]]
    return nullptr;

  std::shared_ptr<oo::ShaderUniform> result =
      std::shared_ptr<oo::ShaderUniform>(new oo::ShaderUniform(),
                                         oo::Dispose{});
  result->location =
      glGetUniformLocationARB(shaderProgram->program(), uniformName.c_str());
  if (result->location == -1) {
    const oo::ShaderEntityHooks* hooks = oo::ShaderEntityHooksIfSet();
    const std::string className =
        (target != nullptr && hooks != nullptr)
            ? hooks->className(target)
            : std::string("(null)");  // its object's class
    OO_LOG("shader.uniform.bind.failed",
           "Could not bind uniform \"{}\" to -[{} {}] (no uniform of that name "
           "could be found).",
           uniformName, className, property);
    return nullptr;
  }

  result->name = uniformName;
  result->isBinding = true;
  result->cxxProperty = property;
  result->convertClamp = oo::Any(options & oo::UniformConvertOptions::kClamp);
  result->convertNormalize =
      oo::Any(options & oo::UniformConvertOptions::kNormalize);
  result->convertToMatrix =
      oo::Any(options & oo::UniformConvertOptions::kToMatrix);
  result->bindToSuper =
      oo::Any(options & oo::UniformConvertOptions::kBindToSuperTarget);

  if (target != nullptr) result->setEntityBindingTarget(target);
  return result;
}

namespace oo {

ShaderUniform::~ShaderUniform() {}

}  // namespace oo

std::optional<std::string> oo::ShaderUniform::description() {
  std::optional<std::string> valueDesc;
  std::optional<std::string_view> valueType;

  if (isBinding && entityMember) {
    // An entity's member: the class, the entity's address and the property.
    oo::Object* cxxObject = cxxTarget.lock().get();
    if (cxxObject != nullptr)
      valueDesc =
          std::format("[<{} {}> {}]", "Entity",
                      oo::str::pointerDescription(cxxObject), cxxProperty);
    else
      valueDesc = "0";
  } else if (isBinding && cxxTargetOnly) {
    // A C++ target: its address and the member's name.
    oo::Object* cxxObject = cxxTarget.lock().get();
    if (cxxObject != nullptr)
      valueDesc = std::format(
          "[<{}> {}]", oo::str::pointerDescription(cxxObject), cxxProperty);
    else
      valueDesc = "0";
  } else if (isBinding) {
    // Nothing bound (an entity binding no row answered).
    valueDesc = "0";
  } else {
    switch (type) {
      case oo::kShaderUniformTypeInt:
        valueDesc = std::format("{}", std::get<GLint>(value));
        break;

      case oo::kShaderUniformTypeFloat:
        valueDesc = std::format("{:g}", std::get<GLfloat>(value));
        break;

      case oo::kShaderUniformTypeVector: {
        const auto& vector = std::get<std::array<GLfloat, 4>>(value);
        Vector v = {vector[0], vector[1], vector[2]};
        valueDesc = VectorDescription(v);
      } break;

      case oo::kShaderUniformTypeMatrix:
        valueDesc = oo::MatrixDescription(std::get<oo::Matrix>(value));
        break;
    }
  }

  switch (type) {
    case oo::kShaderUniformTypeChar:
    case oo::kShaderUniformTypeUnsignedChar:
    case oo::kShaderUniformTypeShort:
    case oo::kShaderUniformTypeUnsignedShort:
    case oo::kShaderUniformTypeInt:
    case oo::kShaderUniformTypeUnsignedInt:
    case oo::kShaderUniformTypeLong:
    case oo::kShaderUniformTypeUnsignedLong:
      valueType = "int";
      break;

    case oo::kShaderUniformTypeFloat:
    case oo::kShaderUniformTypeDouble:
      valueType = "float";
      break;

    case oo::kShaderUniformTypeVector:
    case oo::kShaderUniformTypeHPVector:
      valueType = "vec4";
      break;

    case oo::kShaderUniformTypeQuaternion:
      valueType = "vec4 (quaternion)";
      break;

    case oo::kShaderUniformTypeMatrix:
      valueType = "matrix";
      break;

    case oo::kShaderUniformTypePoint:
      valueType = "vec2";
      break;

    // what the colour bindings printed while they were objects
    case oo::kShaderUniformTypeColor:
    case oo::kShaderUniformTypeObject:
      valueType = "object-binding";
      break;
  }
  if (!valueType.has_value()) valueDesc = "INVALID";
  if (!valueDesc.has_value()) valueDesc = "INVALID";

  /*	Examples:
                  <OOShaderUniform 0xf00>{1: int tex1 = 1;}
                  <OOShaderUniform 0xf00>{3: float laser_heat_level =
     [<ShipEntity 0xba8> laserHeatLevel];} The class is named as a literal.
  */
  return std::format("<{} {}>{{{}: {} {} = {};}}", "OOShaderUniform",
                     oo::str::pointerDescription(this), location,
                     valueType.value_or("(null)"), name, *valueDesc);
}

void oo::ShaderUniform::apply() {
  if (isBinding) {
    if (isActiveBinding) {
      applyBinding();
    }
  } else {
    applySimple();
  }
}

void oo::ShaderUniform::setEntityBindingTarget(Entity* target) {
  const oo::ShaderEntityHooks* hooks = oo::ShaderEntityHooksIfSet();

  if (!isBinding) return;
  if (cxxTargetOnly)
    return;  // a C++ target's member is bound once, by its factory

  /*	The steps for an entity target: the super target followed
     up the entities (superTarget hook), then the member table's row
     for the name, read from the entity.
  */
  entityMember = false;
  if (target != nullptr && hooks != nullptr && bindToSuper) {
    for (;;) {
      Entity* superCandidate = hooks->superTarget(target);
      if (superCandidate == nullptr || superCandidate == target) break;
      target = superCandidate;
    }
  }

  oo::ShaderMemberBinding memberBinding = {};
  if (target != nullptr && hooks != nullptr &&
      hooks->memberBinding(target, cxxProperty.c_str(), &memberBinding)) {
    value = Binding{memberBinding.get};
    cxxTarget = oo::share(memberBinding.target);
    type = static_cast<uint8_t>(memberBinding.type);
    entityMember = true;
    isActiveBinding = true;
    return;
  }

  /*	No row binds the name for this entity: the entity's object was bound by
     selector, which it did not answer (the member table holds every name the
     object answered), so the binding failed and was logged; with no object (or
     no hooks) nothing was bound or logged.
  */
  value = Binding{};
  cxxTarget.reset();
  isActiveBinding = false;
  if (target != nullptr && hooks != nullptr &&
      hooks->className(target) != "(null)") {
    OO_LOG("shader.uniform.bind.failed",
           "Shader could not bind uniform \"{}\" to -[{} {}] ({}).", name,
           hooks->className(target), cxxProperty,
           "target does not respond to selector");
  }
}

// Designated initializer.
bool oo::ShaderUniform::initWithName(const std::string& uniformName,
                                     oo::ShaderProgram* shaderProgram) {
  bool OK = true;

  if (shaderProgram == nullptr) [[unlikely]]
    OK = false;

  // ([super init] could not fail: the object is already constructed.)

  if (OK) {
    location =
        glGetUniformLocationARB(shaderProgram->program(), uniformName.c_str());
    if (location == -1) OK = false;
  }

  if (OK) {
    name = uniformName;
  }

  // [self release]; self = nil where !OK: the factory drops the object.
  return OK;
}

void oo::ShaderUniform::applySimple() {
  switch (type) {
    case oo::kShaderUniformTypeInt:
      OOGL(glUniform1iARB(location, std::get<GLint>(value)));
      break;

    case oo::kShaderUniformTypeFloat:
      OOGL(glUniform1fARB(location, std::get<GLfloat>(value)));
      break;

    case oo::kShaderUniformTypeVector:
      OOGL(glUniform4fvARB(location, 1,
                           std::get<std::array<GLfloat, 4>>(value).data()));
      break;

    case oo::kShaderUniformTypeMatrix:
      GLUniformMatrix(location, std::get<oo::Matrix>(value));
  }
}

void oo::ShaderUniform::applyBinding() {
  GLint iVal;
  GLfloat fVal;
  Vector vVal;
  HPVector hpvVal;
  GLfloat expVVal[4];
  oo::Matrix mVal;
  Quaternion qVal;
  NSPoint pVal = {};
  bool isInt = false, isFloat = false, isVector = false, isMatrix = false,
       isPoint = false;

  /*	Design note: if the object has been dealloced, or an exception occurs,
          do nothing. Shaders can specify a default value for uniforms, which
          will be used when no setting has been provided by the host program.

          I considered clearing the binding if the underlying object is
          gone, but adding code to save a small amount of spacein a case that
          shouldn't occur in normal usage is silly.
  */
  // A member binding, or a C++ target's, reads its value once, into the field
  // its type uses. Nothing else is bound.
  oo::ShaderBindingValue member = {};
  if (!cxxTargetOnly && !entityMember) return;
  const auto* binding = std::get_if<Binding>(&value);
  if (binding == nullptr || binding->cxxGet == nullptr) return;
  {
    // The C++ target (the entity, or the C++ object), held weakly: once it has
    // gone, nothing is set.
    oo::Object* cxxObject = cxxTarget.lock().get();
    if (cxxObject == nullptr) return;
    binding->cxxGet(cxxObject, member);
  }

  switch (type) {
    case oo::kShaderUniformTypeChar:
    case oo::kShaderUniformTypeUnsignedChar:
    case oo::kShaderUniformTypeShort:
    case oo::kShaderUniformTypeUnsignedShort:
    case oo::kShaderUniformTypeInt:
    case oo::kShaderUniformTypeUnsignedInt:
    case oo::kShaderUniformTypeLong:
    case oo::kShaderUniformTypeUnsignedLong:
      iVal = static_cast<GLint>(member.i);
      isInt = true;
      break;

    case oo::kShaderUniformTypeFloat:
    case oo::kShaderUniformTypeDouble:
      fVal = static_cast<GLfloat>(member.f);
      isFloat = true;
      break;

    case oo::kShaderUniformTypeVector:
      vVal = member.v;
      if (convertNormalize) vVal = vector_normal(vVal);
      expVVal[0] = vVal.x;
      expVVal[1] = vVal.y;
      expVVal[2] = vVal.z;
      expVVal[3] = 1.0f;
      isVector = true;
      break;

    case oo::kShaderUniformTypeHPVector:
      hpvVal = member.hpv;
      if (convertNormalize) hpvVal = HPvector_normal(hpvVal);
      expVVal[0] = static_cast<GLfloat>(hpvVal.x);
      expVVal[1] = static_cast<GLfloat>(hpvVal.y);
      expVVal[2] = static_cast<GLfloat>(hpvVal.z);
      expVVal[3] = 1.0f;
      isVector = true;
      break;

    case oo::kShaderUniformTypeQuaternion:
      qVal = member.q;
      if (convertToMatrix) {
        mVal = oo::MatrixForQuaternionRotation(qVal);
        isMatrix = true;
      } else {
        expVVal[0] = qVal.x;
        expVVal[1] = qVal.y;
        expVVal[2] = qVal.z;
        expVVal[3] = qVal.w;
        isVector = true;
      }
      break;

    case oo::kShaderUniformTypeMatrix:
      mVal = member.m;
      isMatrix = true;
      break;

    case oo::kShaderUniformTypePoint:
      pVal = member.p;
      isPoint = true;
      break;

    case oo::kShaderUniformTypeObject:
      // An object that is not a colour: nothing set. (The number-object case
      // went with Foundation: no bindable method returns one; the whitelisted
      // object-valued bindings, laserColor and fogUniform, answer the C++
      // colour: the case below.)
      break;

    case oo::kShaderUniformTypeColor:
      if (::oo::Color* color = member.color) {
        expVVal[0] = color->redComponent();
        expVVal[1] = color->greenComponent();
        expVVal[2] = color->blueComponent();
        expVVal[3] = color->alphaComponent();
        isVector = true;
      }
      break;
  }

  if (isFloat) {
    if (convertClamp) fVal = oo::Clamp_0_1_f(fVal);
    OOGL(glUniform1fARB(location, fVal));
  } else if (isInt) {
    if (convertClamp) iVal = iVal ? 1 : 0;
    OOGL(glUniform1iARB(location, iVal));
  } else if (isPoint) {
    GLfloat v2[2] = {static_cast<GLfloat>(pVal.x),
                     static_cast<GLfloat>(pVal.y)};
    OOGL(glUniform2fvARB(location, 1, v2));
  } else if (isVector) {
    OOGL(glUniform4fvARB(location, 1, expVVal));
  } else if (isMatrix) {
    GLUniformMatrix(location, mVal);
  }
}

#endif  // OO_SHADERS
