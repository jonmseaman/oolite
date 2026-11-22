/*	test_OOOpenGLMatrixManager.mm
        Unit tests for OOOpenGLMatrixManager and OOOpenGLMatrixStack
   (src/Core/OOOpenGLMatrixManager.h).

        The expectations cover: the stack, the model-view and projection
   arithmetic (including where translate writes, which differs between the two),
   the degenerate-argument guards of frustum/ortho/perspective, the derived
   matrices and their cache, what sync loads into OpenGL, the uniform locations
   of a real shader program, and the OOGL* functions, which reach the manager
   through the C++ game view's getOpenGLMatrixManager() (this file's Universe
   and view stubs). The GL context is a hidden window's
   (oo_gl_test_context.hpp); the extension manager, which loads the shader entry
   points, runs with its collaborators stubbed as in
   test_OOOpenGLExtensionManager. Run: tests/run.sh unit
*/

#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "OpenGLExtensionManager.h"
#include "OpenGLMatrixManager.h"
#include "RegExpMatcher.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/PList.hpp"

// --- Stubs
// --------------------------------------------------------------------------------------

#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}

// OORegExpMatcher: the stub answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher() {
  return oo::makeShared<oo::RegExpMatcher>();
}
bool oo::RegExpMatcher::string(const std::string&, const std::string&) {
  return false;
}
namespace oo {

RegExpMatcher::~RegExpMatcher() {}

// OOLogging.mm's (it would bring the resource manager into the link); in the
// debug flavour OOOpenGL.mm's OOGL error check and state dump call them.
void LogIndent(void) {}
void LogOutdent(void) {}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

}  // namespace oo


// UNIVERSE is gSharedUniverse; its getGameView() answers sView, the C++ view,
// whose manager the OOGL* functions use. The C++ view's members they reach are
// stubbed here, and its virtual members (never called).
class Universe;
Universe* gSharedUniverse = nullptr;

namespace {

std::shared_ptr<MyOpenGLView> sView;

}  // namespace

MyOpenGLView::~MyOpenGLView() {}
oo::OpenGLMatrixManager* MyOpenGLView::getOpenGLMatrixManager() {
  return matrixManager.get();
}
void MyOpenGLView::endSplashScreen() { std::abort(); }
void MyOpenGLView::updateScreen() { std::abort(); }
void MyOpenGLView::setGameController(::GameController*) { std::abort(); }
bool MyOpenGLView::inFullScreenMode() { std::abort(); }
std::vector<oo::PList> MyOpenGLView::getScreenSizeArray() { std::abort(); }
NSSize MyOpenGLView::currentScreenSize() { std::abort(); }
oo::PList MyOpenGLView::currentScreenMode() { std::abort(); }
void MyOpenGLView::noteMouseInteractionModeChangedFrom(oo::MouseInteractionMode,
                                                       oo::MouseInteractionMode) {
  std::abort();
}
void MyOpenGLView::resetTypedString() { std::abort(); }
void MyOpenGLView::stringToClipboard(const std::string&) { std::abort(); }
NSSize MyOpenGLView::getViewSize() { std::abort(); }

#include "UniverseStandIn.h"

// The universe as the code under test sees it: a stand-in for the C++ class,
// with the header's names and signatures. It answers virtual members, so it has
// the game's vtable (UniverseStandIn.h) and OOTestUniverse overrides them.
class Universe : public oo::Object {
 public:
  OO_UNIVERSE_STAND_IN_VIRTUAL_MEMBERS

  MyOpenGLView* view;
};

class OOTestUniverse : public Universe {
 public:
  ::MyOpenGLView* getGameView() override {
    return view;
  }  // what -gameView answered
};

// --- Helpers
// ------------------------------------------------------------------------------------

namespace {

bool Near(oo::Matrix a, oo::Matrix b) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) {
      if (std::fabs(a.m[i][j] - b.m[i][j]) > 1e-5f) return false;
    }
  return true;
}

bool Same(oo::Matrix a, oo::Matrix b) { return oo::MatrixEqual(a, b); }

const oo::Matrix kSample =
    oo::MatrixConstruct(1, 2, 3, 0, 0, 1, 4, 0, 5, 6, 0, 0, 7, 8, 9, 1);

GLuint Shader(GLenum type, const char* source) {
  GLhandleARB shader = glCreateShaderObjectARB(type);
  glShaderSourceARB(shader, 1, &source, NULL);
  glCompileShaderARB(shader);
  return shader;
}

}  // namespace

// --- Tests
// --------------------------------------------------------------------------------------

TEST(OOOpenGLMatrixManager, stack) {
  {
    std::shared_ptr<oo::OpenGLMatrixStack> stack = oo::makeShared<oo::OpenGLMatrixStack>();
    EXPECT_TRUE(stack->stackCount() == 0);
    EXPECT_TRUE(Same(stack->pop(),
                  kIdentityMatrix));  // empty: identity, and still empty
    EXPECT_TRUE(stack->stackCount() == 0);
    stack->push(kSample);
    stack->push(kZeroMatrix);
    EXPECT_TRUE(stack->stackCount() == 2);
    EXPECT_TRUE(Same(stack->pop(), kZeroMatrix));
    EXPECT_TRUE(Same(stack->pop(), kSample));
    EXPECT_TRUE(stack->stackCount() == 0);
  }
}

TEST(OOOpenGLMatrixManager, modelView) {
  {
    std::shared_ptr<oo::OpenGLMatrixManager> m = oo::makeShared<oo::OpenGLMatrixManager>();
    EXPECT_TRUE(Same(m->getModelView(), kIdentityMatrix));
    EXPECT_TRUE(Same(m->getProjection(), kIdentityMatrix));
    EXPECT_TRUE(m->countModelView() == 0);

    m->loadModelView(kSample);
    EXPECT_TRUE(Same(m->getModelView(), kSample));
    m->multModelView(oo::MatrixForScale(2, 3, 4));
    EXPECT_TRUE(Same(m->getModelView(),
                  oo::MatrixMultiply(oo::MatrixForScale(2, 3, 4), kSample)));

    m->resetModelView();
    m->translateModelView(make_vector(1, 2, 3));
    EXPECT_TRUE(
        Same(m->getModelView(), oo::MatrixForTranslationComponents(1, 2, 3)));
    EXPECT_TRUE(m->getModelView().m[3][0] == 1 && m->getModelView().m[3][1] == 2 &&
             m->getModelView().m[3][2] == 3);

    m->resetModelView();
    m->scaleModelView(make_vector(2, 3, 4));
    EXPECT_TRUE(Same(m->getModelView(), oo::MatrixForScale(2, 3, 4)));
    m->resetModelView();
    m->rotateModelView(0.5f, make_vector(0, 0, 1));
    EXPECT_TRUE(Same(m->getModelView(),
                  oo::MatrixForRotation(make_vector(0, 0, 1), 0.5f)));

    m->resetModelView();
    m->lookAtWithEye(make_vector(0, 0, 5), make_vector(0, 0, 0),
                     make_vector(0, 1, 0));
    EXPECT_TRUE(Near(
        m->getModelView(),
        oo::MatrixConstruct(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -5, 1)));

    m->loadModelView(kSample);
    m->pushModelView();
    m->pushModelView();
    EXPECT_TRUE(m->countModelView() == 2);
    m->resetModelView();
    EXPECT_TRUE(Same(m->popModelView(), kSample));
    EXPECT_TRUE(Same(m->getModelView(), kSample));
    EXPECT_TRUE(Same(m->popModelView(), kSample));
    EXPECT_TRUE(m->countModelView() == 0);
    EXPECT_TRUE(Same(m->popModelView(), kIdentityMatrix));  // empty stack
  }
}

TEST(OOOpenGLMatrixManager, projection) {
  {
    std::shared_ptr<oo::OpenGLMatrixManager> m = oo::makeShared<oo::OpenGLMatrixManager>();

    m->loadProjection(kSample);
    EXPECT_TRUE(Same(m->getProjection(), kSample));
    EXPECT_TRUE(Same(m->getModelView(), kIdentityMatrix));
    m->multProjection(oo::MatrixForScale(2, 3, 4));
    EXPECT_TRUE(Same(m->getProjection(),
                  oo::MatrixMultiply(oo::MatrixForScale(2, 3, 4), kSample)));

    // Unlike the model view, translate writes the last column.
    m->resetProjection();
    m->translateProjection(make_vector(1, 2, 3));
    EXPECT_TRUE(Same(
        m->getProjection(),
        oo::MatrixConstruct(1, 0, 0, 1, 0, 1, 0, 2, 0, 0, 1, 3, 0, 0, 0, 1)));

    m->resetProjection();
    m->scaleProjection(make_vector(2, 3, 4));
    EXPECT_TRUE(Same(m->getProjection(), oo::MatrixForScale(2, 3, 4)));
    m->resetProjection();
    m->rotateProjection(0.25f, make_vector(1, 0, 0));
    EXPECT_TRUE(Same(m->getProjection(),
                  oo::MatrixForRotation(make_vector(1, 0, 0), 0.25f)));

    m->resetProjection();
    m->frustumLeft(-1, 1, -2, 2, 1, 3);
    EXPECT_TRUE(
        Near(m->getProjection(), oo::MatrixConstruct(1, 0, 0, 0, 0, 0.5f, 0, 0, 0,
                                                   0, -2, -1, 0, 0, -3, 0)));
    m->resetProjection();
    m->orthoLeft(0, 4, 0, 2, -1, 1);
    EXPECT_TRUE(
        Near(m->getProjection(), oo::MatrixConstruct(0.5f, 0, 0, 0, 0, 1, 0, 0, 0,
                                                   0, -1, 0, -1, -1, 0, 1)));
    m->resetProjection();
    m->perspectiveFovy(90, 2, 1, 3);
    EXPECT_TRUE(
        Near(m->getProjection(), oo::MatrixConstruct(0.5f, 0, 0, 0, 0, 1, 0, 0, 0,
                                                   0, -2, -1, 0, 0, -3, 0)));

    // Degenerate arguments change nothing.
    m->loadProjection(kSample);
    m->frustumLeft(1, 1, -1, 1, 1, 2);
    m->frustumLeft(-1, 1, 1, 1, 1, 2);
    m->frustumLeft(-1, 1, -1, 1, 2, 2);
    m->frustumLeft(-1, 1, -1, 1, 0, 2);
    m->frustumLeft(-1, 1, -1, 1, 1, -2);
    m->orthoLeft(1, 1, -1, 1, 1, 2);
    m->orthoLeft(-1, 1, 1, 1, 1, 2);
    m->orthoLeft(-1, 1, -1, 1, 2, 2);
    m->perspectiveFovy(60, 0, 1, 2);
    m->perspectiveFovy(60, 1, 2, 2);
    EXPECT_TRUE(Same(m->getProjection(), kSample));

    m->pushProjection();
    m->resetProjection();
    EXPECT_TRUE(Same(m->popProjection(), kSample));
    EXPECT_TRUE(Same(m->popProjection(), kIdentityMatrix));  // empty stack
  }
}

TEST(OOOpenGLMatrixManager, derivedMatrices) {
  {
    std::shared_ptr<oo::OpenGLMatrixManager> m = oo::makeShared<oo::OpenGLMatrixManager>();
    for (int i = 0; i < oo::LITE_GL_MATRIX_END; i++)
      EXPECT_TRUE(Same(m->getMatrix(i), kIdentityMatrix));
    EXPECT_TRUE(Same(m->getMatrix(-1), kIdentityMatrix));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_END), kIdentityMatrix));

    const oo::Matrix mv =
        oo::MatrixMultiply(oo::MatrixForRotation(make_vector(0, 1, 0), 0.3f),
                         oo::MatrixForTranslationComponents(1, 2, 3));
    const oo::Matrix p =
        oo::MatrixConstruct(1, 0, 0, 0, 0, 2, 0, 0, 0, 0, -2, -1, 0, 0, -3, 0);
    m->loadModelView(mv);
    m->loadProjection(p);
    const oo::Matrix mvp = oo::MatrixMultiply(mv, p);
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW), mv));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_PROJECTION), p));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION), mvp));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_INVERSE),
                  oo::MatrixInverse(mv)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_PROJECTION_INVERSE),
                  oo::MatrixInverse(p)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE),
                  oo::MatrixInverse(mvp)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_TRANSPOSE),
                  oo::MatrixTranspose(mv)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_PROJECTION_TRANSPOSE),
                  oo::MatrixTranspose(p)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE),
                  oo::MatrixTranspose(mvp)));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_INVERSE_TRANSPOSE),
                  oo::MatrixTranspose(oo::MatrixInverse(mv))));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE),
                  oo::MatrixTranspose(oo::MatrixInverse(p))));
    EXPECT_TRUE(Same(
        m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE),
        oo::MatrixTranspose(oo::MatrixInverse(mvp))));

    // The normal matrix of a rotation is the rotation (the translation dropped,
    // det 1).
    EXPECT_TRUE(Near(m->getMatrix(oo::LITE_GL_MATRIX_NORMAL),
                  oo::MatrixForRotation(make_vector(0, 1, 0), 0.3f)));
    // A uniform scale s: inverse-transpose is 1/s, normalised by the cube root
    // of det = 1/s^3.
    m->loadModelView(oo::MatrixForScale(2, 2, 2));
    EXPECT_TRUE(
        Near(m->getMatrix(oo::LITE_GL_MATRIX_NORMAL), oo::MatrixForScale(1, 1, 1)));
    m->loadModelView(oo::MatrixForScale(2, 4, 8));  // det 64, cube root 4
    EXPECT_TRUE(Near(m->getMatrix(oo::LITE_GL_MATRIX_NORMAL),
                  oo::MatrixForScale(2, 1, 0.5f)));
    // A singular model view: the inverse's determinant is 0 and no
    // normalisation happens.
    m->loadModelView(kZeroMatrix);
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_NORMAL),
                  oo::MatrixTranspose(oo::MatrixInverse(oo::MatrixConstruct(
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1)))));

    // Derived matrices are cached until their source changes: a projection
    // change leaves the normal matrix and the model view inverse alone, and
    // refreshes the rest.
    m->loadModelView(mv);
    const oo::Matrix normal = m->getMatrix(oo::LITE_GL_MATRIX_NORMAL);
    const oo::Matrix mvInverse = m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_INVERSE);
    (void)m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION);
    m->loadProjection(kIdentityMatrix);
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_NORMAL), normal));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_INVERSE), mvInverse));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION), mv));
    EXPECT_TRUE(Same(m->getMatrix(oo::LITE_GL_MATRIX_PROJECTION_INVERSE),
                  kIdentityMatrix));
  }
}

TEST(OOOpenGLMatrixManager, syncLoadsOpenGL) {
  EXPECT_TRUE(OOTestGLContext());
  {
    std::shared_ptr<oo::OpenGLMatrixManager> m = oo::makeShared<oo::OpenGLMatrixManager>();
    m->loadModelView(kSample);
    m->loadProjection(oo::MatrixForScale(2, 3, 4));

    glMatrixMode(GL_TEXTURE);
    m->syncModelView();
    GLint mode = 0;
    glGetIntegerv(GL_MATRIX_MODE, &mode);
    EXPECT_TRUE(mode == GL_MODELVIEW);
    EXPECT_TRUE(Same(oo::MatrixLoadGLMatrix(GL_MODELVIEW_MATRIX), kSample));

    m->syncProjection();
    glGetIntegerv(GL_MATRIX_MODE, &mode);
    EXPECT_TRUE(mode == GL_PROJECTION);
    EXPECT_TRUE(Same(oo::MatrixLoadGLMatrix(GL_PROJECTION_MATRIX),
                  oo::MatrixForScale(2, 3, 4)));
    EXPECT_TRUE(Same(oo::MatrixLoadGLMatrix(GL_MODELVIEW_MATRIX), kSample));
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}

TEST(OOOpenGLMatrixManager, standardUniformLocations) {
  EXPECT_TRUE(OOTestGLContext());
  EXPECT_TRUE(oo::OpenGLExtensionManager::sharedManager()
               ->shadersSupported());  // loads the ARB entry points
  {
    std::shared_ptr<oo::OpenGLMatrixManager> m = oo::makeShared<oo::OpenGLMatrixManager>();

    const char* vertex =
        "uniform mat4 ooliteModelViewProjection;\n"
        "uniform mat3 ooliteNormalMatrix;\n"
        "uniform mat4 ooliteProjectionInverseTranspose;\n"
        "varying vec3 n;\n"
        "void main() { n = ooliteNormalMatrix * gl_Normal; gl_Position = "
        "ooliteProjectionInverseTranspose * ooliteModelViewProjection * "
        "gl_Vertex; }\n";
    const char* fragment =
        "varying vec3 n;\n"
        "void main() { gl_FragColor = vec4(n, 1.0); }\n";
    GLhandleARB program = glCreateProgramObjectARB();
    glAttachObjectARB(program, Shader(GL_VERTEX_SHADER_ARB, vertex));
    glAttachObjectARB(program, Shader(GL_FRAGMENT_SHADER_ARB, fragment));
    glLinkProgramARB(program);
    GLint linked = 0;
    glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);
    EXPECT_TRUE(linked);

    const oo::PList locations = m->standardMatrixUniformLocations(program);
    const oo::PList::Array* array = locations.getIf<oo::PList::Array>();
    EXPECT_TRUE(array != nullptr && array->size() == 3);
    if (array != nullptr && array->size() == 3) {
      const int expectedIndex[3] = {
          oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION, oo::LITE_GL_MATRIX_NORMAL,
          oo::LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE};
      const char* expectedName[3] = {"ooliteModelViewProjection",
                                     "ooliteNormalMatrix",
                                     "ooliteProjectionInverseTranspose"};
      const char* expectedType[3] = {"mat4", "mat3", "mat4"};
      for (int k = 0; k < 3; k++) {
        const oo::PList::Array* triple = (*array)[k].getIf<oo::PList::Array>();
        EXPECT_TRUE(triple != nullptr && triple->size() == 3);
        if (triple == nullptr || triple->size() != 3) continue;
        EXPECT_TRUE((*triple)[0].int64Value() ==
                 glGetUniformLocationARB(program, expectedName[k]));
        EXPECT_TRUE((*triple)[1].int64Value() == expectedIndex[k]);
        EXPECT_TRUE((*triple)[2].getIf<std::string>() != nullptr &&
                 *(*triple)[2].getIf<std::string>() == expectedType[k]);
      }
    }
    EXPECT_TRUE(
        std::string(ooliteStandardMatrixUniforms[oo::LITE_GL_MATRIX_NORMAL]) ==
        "ooliteNormalMatrix");
    glDeleteObjectARB(program);

    // A program with none of them.
    const oo::PList none = m->standardMatrixUniformLocations(0);
    EXPECT_TRUE(none.getIf<oo::PList::Array>() != nullptr &&
             none.getIf<oo::PList::Array>()->empty());
    glGetError();
  }
}

// The view's state is private; MyOpenGLView.h names this struct its friend so
// the checks below read and set the same member.
struct MyOpenGLViewTestAccess {
  static auto& matrixManager(MyOpenGLView* view) { return view->matrixManager; }
};

TEST(OOOpenGLMatrixManager, openGLFunctionsUseTheGameViewsManager) {
  EXPECT_TRUE(OOTestGLContext());
  {
    std::shared_ptr<oo::OpenGLMatrixManager> manager =
        oo::makeShared<oo::OpenGLMatrixManager>();
    oo::OpenGLMatrixManager* m = manager.get();
    sView = oo::makeShared<MyOpenGLView>();
    MyOpenGLViewTestAccess::matrixManager(sView.get()) = manager;
    OOTestUniverse* universe = new OOTestUniverse();
    universe->view = sView.get();
    gSharedUniverse = universe;

    oo::GLResetModelView();
    oo::GLLoadModelView(kSample);
    EXPECT_TRUE(Same(m->getModelView(), kSample));
    EXPECT_TRUE(Same(oo::MatrixLoadGLMatrix(GL_MODELVIEW_MATRIX), kSample));
    oo::GLPushModelView();
    EXPECT_TRUE(m->countModelView() == 1);
    oo::GLMultModelView(oo::MatrixForScale(2, 2, 2));
    oo::GLTranslateModelView(make_vector(1, 0, 0));
    oo::GLRotateModelView(0.1f, make_vector(0, 0, 1));
    oo::GLScaleModelView(make_vector(1, 2, 1));
    oo::GLLookAt(make_vector(0, 0, 1), make_vector(0, 0, 0),
               make_vector(0, 1, 0));
    EXPECT_TRUE(Same(oo::GLGetModelView(), m->getModelView()));
    EXPECT_TRUE(
        Same(oo::MatrixLoadGLMatrix(GL_MODELVIEW_MATRIX), m->getModelView()));
    EXPECT_TRUE(Same(oo::GLPopModelView(), kSample));
    EXPECT_TRUE(Same(oo::MatrixLoadGLMatrix(GL_MODELVIEW_MATRIX), kSample));

    oo::GLResetProjection();
    oo::GLPerspective(90, 2, 1, 3);
    EXPECT_TRUE(Same(oo::GLGetProjection(), m->getProjection()));
    EXPECT_TRUE(
        Same(oo::MatrixLoadGLMatrix(GL_PROJECTION_MATRIX), m->getProjection()));
    oo::GLPushProjection();
    oo::GLLoadProjection(kSample);
    oo::GLMultProjection(oo::MatrixForScale(1, 2, 3));
    oo::GLTranslateProjection(make_vector(1, 2, 3));
    oo::GLRotateProjection(0.2f, make_vector(0, 1, 0));
    oo::GLScaleProjection(make_vector(3, 2, 1));
    oo::GLFrustum(-1, 1, -1, 1, 1, 10);
    oo::GLOrtho(-1, 1, -1, 1, -1, 1);
    EXPECT_TRUE(
        Same(oo::MatrixLoadGLMatrix(GL_PROJECTION_MATRIX), m->getProjection()));
    const oo::Matrix mvp =
        oo::MatrixMultiply(m->getModelView(), m->getProjection());
    EXPECT_TRUE(Same(oo::GLGetModelViewProjection(), mvp));
    EXPECT_TRUE(Same(oo::GLPopProjection(),
                  oo::MatrixConstruct(0.5f, 0, 0, 0, 0, 1, 0, 0, 0, 0, -2, -1, 0,
                                    0, -3, 0)));

    // No universe: nothing is changed, and the getters answer a zero matrix, as
    // nil did.
    gSharedUniverse = nullptr;
    oo::GLLoadModelView(kIdentityMatrix);
    oo::GLPushModelView();
    EXPECT_TRUE(Same(m->getModelView(), kSample));
    EXPECT_TRUE(m->countModelView() == 0);
    EXPECT_TRUE(Same(oo::GLGetModelView(), kZeroMatrix));
    EXPECT_TRUE(Same(oo::GLPopModelView(), kZeroMatrix));
    EXPECT_TRUE(Same(oo::GLGetProjection(), kZeroMatrix));
    EXPECT_TRUE(Same(oo::GLGetModelViewProjection(), kZeroMatrix));
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}
