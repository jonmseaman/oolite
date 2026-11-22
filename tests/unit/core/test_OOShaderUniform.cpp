/*	test_OOShaderUniform.mm
        Unit tests for OOShaderUniform (src/Core/Materials/OOShaderUniform.h).

        A uniform of a GLSL program, a constant or bound to a property of an
   object that it asks for the value on every -apply. This pins what it
   computes, on a real program compiled in a hidden GL context and read back
   with glGetUniform: which initialisers fail (no program, no uniform of that
   name, a null colour, no selector); each constant's value as GL gets it (int,
   float, vector, colour, quaternion as vector or matrix, matrix); a binding to
   each supported return type with its conversions (clamp, normalise,
   quaternion to matrix), the methods it refuses (arguments, unsupported types,
   not implemented), and a target that has gone; and the description. The
   cases use the C++ factories (null where an initialiser fails). Run: bash
   tests/run.sh unit
*/

#include <cmath>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Color.h"
#include "RegExpMatcher.h"
#include "ShaderUniform.h"
#include "ShaderUniformMethodType.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/String.hpp"

// --- Stubs
// --------------------------------------------------------------------------------------

// OOLogging.mm's (it would bring the resource manager into the link).
namespace oo {

void LogIndent(void) {}
void LogOutdent(void) {}

}  // namespace oo


// The extension manager's collaborators, as test_OOOpenGLExtensionManager stubs
// them.
#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}

// OORegExpMatcher stub: answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher() {
  return oo::makeShared<oo::RegExpMatcher>();
}
bool oo::RegExpMatcher::string(const std::string&, const std::string&) {
  return false;
}
namespace oo {

RegExpMatcher::~RegExpMatcher() {}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

}  // namespace oo


// The shader program (OOShaderProgram.mm reaches the resource manager): the one
// member the uniform calls, program(), answering the program the test linked.
// OOShaderProgram stand-in: definitions of the members the uniform uses.
static GLhandleARB gStandInProgram = 0;

namespace oo {

ShaderProgram::~ShaderProgram() {}

}  // namespace oo

GLhandleARB oo::ShaderProgram::program() { return _program; }
std::shared_ptr<oo::ShaderProgram> oo::ShaderProgram::shaderProgramWithVertexShaderName(
    const std::string&, const std::string&, const std::optional<std::string>&,
    const oo::PList&) {
  std::shared_ptr<oo::ShaderProgram> made = std::shared_ptr<oo::ShaderProgram>(new oo::ShaderProgram(), oo::Dispose{});
  made->_program = gStandInProgram;
  return made;
}

namespace {

typedef void(APIENTRY* GetUniformfv)(GLhandleARB, GLint, GLfloat*);
typedef void(APIENTRY* GetUniformiv)(GLhandleARB, GLint, GLint*);

GLhandleARB gProgram = 0;
GetUniformfv gGetUniformfv = nullptr;
GetUniformiv gGetUniformiv = nullptr;
oo::ShaderProgram* gShaderProgram = nullptr;
std::shared_ptr<oo::ShaderProgram> gShaderProgramHeld;

// A program with one uniform of each kind, each used so that the linker keeps
// it.
bool SetUpProgram() {
  if (gProgram != 0) return true;
  if (!OOTestGLContext()) return false;
  oo::OpenGLExtensionManager::sharedManager();  // loads the ARB shader entry
                                              // points
  gGetUniformfv = (GetUniformfv)SDL_GL_GetProcAddress("glGetUniformfvARB");
  gGetUniformiv = (GetUniformiv)SDL_GL_GetProcAddress("glGetUniformivARB");
  if (gGetUniformfv == nullptr || gGetUniformiv == nullptr) return false;

  const char* vertex =
      "uniform mat4 uMatrix;\n"
      "uniform vec4 uVector;\n"
      "uniform vec2 uPoint;\n"
      "void main() { gl_Position = uMatrix * gl_Vertex + uVector + "
      "vec4(uPoint, 0.0, 0.0); }\n";
  const char* fragment =
      "uniform int uInt;\n"
      "uniform float uFloat;\n"
      "void main() { gl_FragColor = vec4(float(uInt), uFloat, 0.0, 1.0); }\n";

  GLhandleARB vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  glShaderSourceARB(vs, 1, &vertex, NULL);
  glCompileShaderARB(vs);
  GLhandleARB fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  glShaderSourceARB(fs, 1, &fragment, NULL);
  glCompileShaderARB(fs);
  gProgram = glCreateProgramObjectARB();
  glAttachObjectARB(gProgram, vs);
  glAttachObjectARB(gProgram, fs);
  glLinkProgramARB(gProgram);
  glUseProgramObjectARB(gProgram);

  gStandInProgram = gProgram;
  gShaderProgramHeld = oo::ShaderProgram::shaderProgramWithVertexShaderName(
      "", "", std::nullopt, oo::PList());
  gShaderProgram = gShaderProgramHeld.get();
  return glGetUniformLocationARB(gProgram, "uFloat") != -1;
}

std::vector<GLfloat> Floats(const char* name, int count) {
  std::vector<GLfloat> values(16, -99.0f);
  gGetUniformfv(gProgram, glGetUniformLocationARB(gProgram, name),
                values.data());
  values.resize(count);
  return values;
}

GLint Int(const char* name) {
  GLint value = -99;
  gGetUniformiv(gProgram, glGetUniformLocationARB(gProgram, name), &value);
  return value;
}

bool Near(const std::vector<GLfloat>& a, std::initializer_list<GLfloat> b) {
  if (a.size() != b.size()) return false;
  size_t i = 0;
  for (GLfloat e : b) {
    if (std::fabs(a[i++] - e) > 1e-5f) return false;
  }
  return true;
}

void Reset() {
  glUseProgramObjectARB(gProgram);
  glUniform1iARB(glGetUniformLocationARB(gProgram, "uInt"), 0);
  glUniform1fARB(glGetUniformLocationARB(gProgram, "uFloat"), 0.0f);
  const GLfloat zero[16] = {};
  glUniform4fvARB(glGetUniformLocationARB(gProgram, "uVector"), 1, zero);
  glUniform2fvARB(glGetUniformLocationARB(gProgram, "uPoint"), 1, zero);
  glUniformMatrix4fvARB(glGetUniformLocationARB(gProgram, "uMatrix"), 1,
                        GL_FALSE, zero);
}

// The uniform's description, as its description() prints it (nil:
// "(null)").
std::string Description(const std::shared_ptr<oo::ShaderUniform>& uniform) {
  return uniform->description().value_or("(null)");
}

}  // namespace

TEST(OOShaderUniform, initialisersThatFail) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uFloat", nullptr, GLfloat(1)) ==
             nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uMissing", gShaderProgram,
                                           GLfloat(1)) == nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uVector", gShaderProgram,
                                           static_cast<oo::Color*>(nullptr)) ==
             nullptr);
  }
}

TEST(OOShaderUniform, constants) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    Reset();
    oo::ShaderUniform::initWithName("uInt", gShaderProgram, GLint(3))->apply();
    oo::ShaderUniform::initWithName("uFloat", gShaderProgram, 0.75f)->apply();
    EXPECT_TRUE(Int("uInt") == 3 && Near(Floats("uFloat", 1), {0.75f}));

    GLfloat v[4] = {1, 2, 3, 4};
    oo::ShaderUniform::initWithName("uVector", gShaderProgram, v)->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {1, 2, 3, 4}));
    oo::ShaderUniform::initWithName(
        "uVector", gShaderProgram,
        oo::Color::colorWithRed(0.1f, 0.2f, 0.3f, 0.4f).get())
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.1f, 0.2f, 0.3f, 0.4f}));

    // A quaternion as a vector is x, y, z, w; as a matrix it is its rotation.
    Quaternion q = {0.5f, 0.5f, 0.5f, 0.5f};
    oo::ShaderUniform::initWithName("uVector", gShaderProgram, q, false)->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.5f, 0.5f, 0.5f, 0.5f}));
    oo::ShaderUniform::initWithName("uMatrix", gShaderProgram, q, true)->apply();
    const oo::Matrix rotation = oo::MatrixForQuaternionRotation(q);
    const std::vector<GLfloat> m = Floats("uMatrix", 16);
    bool same = true;
    for (int i = 0; i < 16; i++)
      same = same &&
             std::fabs(m[i] - OOMatrixValuesForOpenGL(rotation)[i]) < 1e-5f;
    EXPECT_TRUE(same);

    oo::ShaderUniform::initWithName("uMatrix", gShaderProgram,
                                  oo::MatrixForScale(2, 3, 4))
        ->apply();
    EXPECT_TRUE(Near(Floats("uMatrix", 16),
                  {2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1}));
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}

// A constant ignores a binding target (through the entity target's setter).
TEST(OOShaderUniform, constantIgnoresABindingTarget) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    oo::AutoreleaseScope pool;
    Reset();
    const std::shared_ptr<oo::ShaderUniform> constant =
        oo::ShaderUniform::initWithName("uFloat", gShaderProgram, 0.25f);
    constant->setEntityBindingTarget(nullptr);
    constant->apply();
    EXPECT_TRUE(Near(Floats("uFloat", 1), {0.25f}));
  }
}

TEST(OOShaderUniform, description) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    const std::string location =
        std::to_string(glGetUniformLocationARB(gProgram, "uInt"));
    const std::string text = Description(
        oo::ShaderUniform::initWithName("uInt", gShaderProgram, GLint(1)));
    EXPECT_TRUE(text.starts_with("<OOShaderUniform 0x") &&
             text.ends_with(">{" + location + ": int uInt = 1;}"));
    EXPECT_TRUE(Description(
                 oo::ShaderUniform::initWithName("uFloat", gShaderProgram, 0.5f))
                 .ends_with(": float uFloat = 0.5;}"));
    GLfloat v[4] = {1, 2, 3, 4};
    EXPECT_TRUE(
        Description(oo::ShaderUniform::initWithName("uVector", gShaderProgram, v))
            .ends_with(": vec4 uVector = (1, 2, 3);}"));
    EXPECT_TRUE(Description(oo::ShaderUniform::initWithName(
                             "uMatrix", gShaderProgram, kIdentityMatrix))
                 .ends_with(": matrix uMatrix = " +
                            oo::MatrixDescription(kIdentityMatrix) + ";}"));
  }
}

// A uniform bound to a C++ object's member (the dust shader's uWarp and
// uOffsetPlayerPosition): the value is read from the target on every apply,
// converted as a method's was, and a target that has gone sets
// nothing.
namespace {

class CxxTarget : public oo::Object {
 public:
  Vector vector = kZeroVector;
};

void CxxTargetVector(oo::Object* target, oo::ShaderBindingValue& outValue) {
  outValue.v = static_cast<CxxTarget*>(target)->vector;
}

}  // namespace

TEST(OOShaderUniform, cxxTargetBindings) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uVector", nullptr, nullptr, "vector",
                                           oo::kShaderUniformTypeVector,
                                           CxxTargetVector, oo::UniformConvertOptions::kNone) == nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uMissing", gShaderProgram, nullptr,
                                           "vector", oo::kShaderUniformTypeVector,
                                           CxxTargetVector, oo::UniformConvertOptions::kNone) == nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uVector", gShaderProgram, nullptr,
                                           "vector", oo::kShaderUniformTypeVector,
                                           nullptr, oo::UniformConvertOptions::kNone) == nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName(
                 "uVector", gShaderProgram, nullptr, "vector",
                 oo::kShaderUniformTypeInvalid, CxxTargetVector, oo::UniformConvertOptions::kNone) == nullptr);

    Reset();
    std::shared_ptr<CxxTarget> target = oo::makeShared<CxxTarget>();
    target->vector = make_vector(3, 0, 4);
    std::shared_ptr<oo::ShaderUniform> u = oo::ShaderUniform::initWithName(
        "uVector", gShaderProgram, target.get(), "vector",
        oo::kShaderUniformTypeVector, CxxTargetVector, oo::UniformConvertOptions::kNone);
    EXPECT_TRUE(u != nullptr);
    u->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {3, 0, 4, 1}));
    target->vector = make_vector(0, 2, 10);
    u->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0, 2, 10, 1}));
    oo::ShaderUniform::initWithName("uVector", gShaderProgram, target.get(),
                                  "vector", oo::kShaderUniformTypeVector,
                                  CxxTargetVector, oo::UniformConvertOptions::kNormalize)
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0, 0.196116f, 0.980581f, 1}));
    EXPECT_TRUE(Description(u).find(": vec4 uVector = [<0x") !=
                 std::string::npos &&
             Description(u).ends_with("> vector];}"));

    // The target held weakly: once it has gone, apply sets nothing.
    target = nullptr;
    const GLfloat nine[4] = {9, 9, 9, 9};
    glUniform4fvARB(glGetUniformLocationARB(gProgram, "uVector"), 1, nine);
    u->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {9, 9, 9, 9}));
    EXPECT_TRUE(Description(u).ends_with(": vec4 uVector = 0;}"));

    // No target: nothing set.
    oo::ShaderUniform::initWithName("uVector", gShaderProgram, nullptr, "vector",
                                  oo::kShaderUniformTypeVector, CxxTargetVector,
                                  oo::UniformConvertOptions::kNone)
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {9, 9, 9, 9}));
  }
}

// The convert options and value kinds, through the C++ target factory: the type
// is the one the bound method's return type gave, the getter answers what the
// method answered.
namespace {

class ConvertTarget : public oo::Object {
 public:
  int _int = 0;
  float _float = 0;
};

ConvertTarget* Target(oo::Object* target) {
  return static_cast<ConvertTarget*>(target);
}

void IntValue(oo::Object* t, oo::ShaderBindingValue& v) {
  v.i = Target(t)->_int;
}
void ByteValue(oo::Object*, oo::ShaderBindingValue& v) { v.i = 7; }
void FloatValue(oo::Object* t, oo::ShaderBindingValue& v) {
  v.f = Target(t)->_float;
}
void DoubleValue(oo::Object*, oo::ShaderBindingValue& v) { v.f = 2.5; }
void VectorValue(oo::Object*, oo::ShaderBindingValue& v) {
  v.v = make_vector(3, 0, 4);
}
void HPVectorValue(oo::Object*, oo::ShaderBindingValue& v) {
  v.hpv = make_HPvector(0, 6, 8);
}
void QuaternionValue(oo::Object*, oo::ShaderBindingValue& v) {
  Quaternion q = {0.5f, 0.5f, 0.5f, 0.5f};
  v.q = q;
}
void MatrixValue(oo::Object*, oo::ShaderBindingValue& v) {
  v.m = oo::MatrixForScale(2, 3, 4);
}
void PointValue(oo::Object*, oo::ShaderBindingValue& v) {
  v.p = NSMakePoint(1.5, -2.5);
}
void ColorValue(oo::Object*, oo::ShaderBindingValue& v) {
  static std::shared_ptr<oo::Color> color =
      oo::Color::colorWithRed(0.25f, 0.5f, 0.75f, 1.0f);
  v.color = color.get();
}
void NotAColor(oo::Object*, oo::ShaderBindingValue&) {}

std::shared_ptr<oo::ShaderUniform> BoundCxx(const char* uniform, ConvertTarget* target,
                                  oo::ShaderUniformType type,
                                  oo::ShaderCxxBindingGetter get,
                                  oo::UniformConvertOptions options = {}) {
  return oo::ShaderUniform::initWithName(uniform, gShaderProgram, target,
                                       "property", type, get, options);
}

}  // namespace

TEST(OOShaderUniform, cxxTargetConvertOptions) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    oo::AutoreleaseScope pool;
    std::shared_ptr<ConvertTarget> held = oo::makeShared<ConvertTarget>();
    ConvertTarget* target = held.get();
    target->_int = 5;
    target->_float = 1.75f;
    Reset();

    // Each supported value kind, applied on every apply from the target's
    // current value.
    const std::shared_ptr<oo::ShaderUniform> u =
        BoundCxx("uInt", target, oo::kShaderUniformTypeInt, IntValue);
    u->apply();
    EXPECT_TRUE(Int("uInt") == 5);
    target->_int = 6;
    u->apply();
    EXPECT_TRUE(Int("uInt") == 6);
    BoundCxx("uInt", target, oo::kShaderUniformTypeInt, IntValue,
             oo::UniformConvertOptions::kClamp)
        ->apply();  // clamped: non-zero is 1
    EXPECT_TRUE(Int("uInt") == 1);
    BoundCxx("uInt", target, oo::kShaderUniformTypeUnsignedChar, ByteValue)
        ->apply();
    EXPECT_TRUE(Int("uInt") == 7);

    BoundCxx("uFloat", target, oo::kShaderUniformTypeFloat, FloatValue)->apply();
    EXPECT_TRUE(Near(Floats("uFloat", 1), {1.75f}));
    BoundCxx("uFloat", target, oo::kShaderUniformTypeFloat, FloatValue,
             oo::UniformConvertOptions::kClamp)
        ->apply();
    EXPECT_TRUE(Near(Floats("uFloat", 1), {1.0f}));
    BoundCxx("uFloat", target, oo::kShaderUniformTypeDouble, DoubleValue)
        ->apply();
    EXPECT_TRUE(Near(Floats("uFloat", 1), {2.5f}));

    BoundCxx("uVector", target, oo::kShaderUniformTypeVector, VectorValue)
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {3, 0, 4, 1}));
    BoundCxx("uVector", target, oo::kShaderUniformTypeVector, VectorValue,
             oo::UniformConvertOptions::kNormalize)
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.6f, 0, 0.8f, 1}));
    BoundCxx("uVector", target, oo::kShaderUniformTypeHPVector, HPVectorValue,
             oo::UniformConvertOptions::kNormalize)
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0, 0.6f, 0.8f, 1}));
    BoundCxx("uVector", target, oo::kShaderUniformTypeQuaternion, QuaternionValue)
        ->apply();  // not to matrix: x, y, z, w
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.5f, 0.5f, 0.5f, 0.5f}));
    BoundCxx("uVector", target, oo::kShaderUniformTypeColor, ColorValue)->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.25f, 0.5f, 0.75f, 1}));
    BoundCxx("uVector", target, oo::kShaderUniformTypeVector, VectorValue)
        ->apply();
    BoundCxx("uVector", target, oo::kShaderUniformTypeObject, NotAColor)
        ->apply();  // an object that is not a colour: nothing set
    EXPECT_TRUE(Near(Floats("uVector", 4), {3, 0, 4, 1}));

    BoundCxx("uMatrix", target, oo::kShaderUniformTypeMatrix, MatrixValue)
        ->apply();
    EXPECT_TRUE(Near(Floats("uMatrix", 16),
                  {2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1}));
    BoundCxx("uMatrix", target, oo::kShaderUniformTypeQuaternion, QuaternionValue,
             oo::UniformConvertOptions::kToMatrix)
        ->apply();
    const Quaternion q = {0.5f, 0.5f, 0.5f, 0.5f};
    const oo::Matrix rotation = oo::MatrixForQuaternionRotation(q);
    EXPECT_TRUE(std::fabs(Floats("uMatrix", 16)[1] -
                       OOMatrixValuesForOpenGL(rotation)[1]) < 1e-5f);

    BoundCxx("uPoint", target, oo::kShaderUniformTypePoint, PointValue)->apply();
    EXPECT_TRUE(Near(Floats("uPoint", 2), {1.5f, -2.5f}));
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}

// --- The C++ class and the root
// -----------------------------------------

TEST(OOShaderUniform, cxxAPI) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uFloat", nullptr, 1.0f) == nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uMissing", gShaderProgram, 1.0f) ==
             nullptr);
    EXPECT_TRUE(oo::ShaderUniform::initWithName("uVector", gShaderProgram,
                                           static_cast<oo::Color*>(nullptr)) ==
             nullptr);

    Reset();
    oo::ShaderUniform::initWithName("uInt", gShaderProgram, GLint(4))->apply();
    oo::ShaderUniform::initWithName("uFloat", gShaderProgram, 0.5f)->apply();
    EXPECT_TRUE(Int("uInt") == 4 && Near(Floats("uFloat", 1), {0.5f}));
    oo::ShaderUniform::initWithName(
        "uVector", gShaderProgram,
        oo::Color::colorWithRed(0.1f, 0.2f, 0.3f, 0.4f).get())
        ->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.1f, 0.2f, 0.3f, 0.4f}));
    const Quaternion q = {0.5f, 0.5f, 0.5f, 0.5f};
    oo::ShaderUniform::initWithName("uVector", gShaderProgram, q, false)->apply();
    EXPECT_TRUE(Near(Floats("uVector", 4), {0.5f, 0.5f, 0.5f, 0.5f}));
    oo::ShaderUniform::initWithName("uMatrix", gShaderProgram,
                                  oo::MatrixForScale(2, 3, 4))
        ->apply();
    EXPECT_TRUE(Near(Floats("uMatrix", 16),
                  {2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1}));
  }
}

// The description prints the uniform's own address.
TEST(OOShaderUniform, descriptionPrintsTheUniform) {
  if (!SetUpProgram()) {
    EXPECT_TRUE(false);
    return;
  }
  const std::shared_ptr<oo::ShaderUniform> u =
      oo::ShaderUniform::initWithName("uInt", gShaderProgram, GLint(2));
  EXPECT_TRUE(Description(u).starts_with(
      "<OOShaderUniform " + oo::str::pointerDescription(u.get()) + ">{"));
}
