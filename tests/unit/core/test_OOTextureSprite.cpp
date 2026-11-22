/*	test_OOTextureSprite.mm
        Unit tests for OOTextureSprite (src/Core/OOTextureSprite.h).

        A texture sprite is a texture and a size; its only output is the
   textured quad it hands to OpenGL. The test captures that quad in feedback
   mode (GL_FEEDBACK, GL_3D_COLOR_TEXTURE) on the hidden context of
   oo_gl_test_context.hpp, so nothing is rasterised and no pixel is read: each
        corner's window position, colour and texture coordinate, for the plain,
   centred and background blits, with the alpha clamped to [0, 1]; that the
   texture is applied once per blit, that 2D texturing is off again afterwards,
   and that the background blit leaves the size as it found it. The sprite's
   texture is a stand-in OOTexture defined here,
   which answers its original dimensions and counts -apply; the extension
   manager runs with its collaborators stubbed as in test_OOOpenGLStateManager.
   The tests call the C++ class.
   Run: tests/run.sh unit
*/

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "OpenGLExtensionManager.h"
#include "RegExpMatcher.h"
#include "Texture.h"
#include "TextureSprite.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/String.hpp"

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

// OOLogging.mm's (it would bring the resource manager into the link); only the
// state dump uses them.
void LogIndent(void) {}
void LogOutdent(void) {}

}  // namespace oo


// The sprite's texture: its original dimensions, and how often it was applied.
// The stand-in is C++: TestTexture, and the root's own members (a texture
// needs them).
class TestTexture final : public oo::Texture {
 public:
  NSSize originalDimensions() override { return _dimensions; }
  void apply() override { _applied++; }

  NSSize _dimensions = {};
  unsigned _applied = 0;
};

oo::Texture::Texture() {}
namespace oo {

Texture::~Texture() {}

}  // namespace oo

void oo::Texture::apply() {}
void oo::Texture::ensureFinishedLoading() {}
bool oo::Texture::isFinishedLoading() { return true; }
std::optional<std::string> oo::Texture::cacheKey() { return std::nullopt; }
NSSize oo::Texture::dimensions() { return NSSize{}; }
NSSize oo::Texture::originalDimensions() { return dimensions(); }
bool oo::Texture::isMipMapped() { return false; }
oo::PixMap oo::Texture::copyPixMapRepresentation() { return oo::PixMap{}; }
bool oo::Texture::isRectangleTexture() { return false; }
bool oo::Texture::isCubeMap() { return false; }
NSSize oo::Texture::texCoordsScale() { return NSSize{1.0, 1.0}; }
GLint oo::Texture::glTextureName() { return 0; }
#ifndef NDEBUG
std::optional<std::string> oo::Texture::name() { return std::nullopt; }
#endif
std::optional<std::string> oo::Texture::descriptionComponents() const {
  return std::nullopt;
}
std::optional<std::string> oo::Texture::shortDescriptionComponents() const {
  return std::nullopt;
}
std::string oo::Texture::description() const {
  return oo::str::format("<OOTexture %s>",
                         oo::str::pointerDescription(this).c_str());
}
std::string oo::Texture::shortDescription() const { return description(); }
std::string oo::Texture::className() const { return "OOTexture"; }
void oo::Texture::forceRebind() {}

// --- Helpers
// ------------------------------------------------------------------------------------

namespace {

TestTexture* Texture(float width, float height) {
  // What an autoreleased texture is: the texture, kept until the current
  // autorelease scope drains.
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_dimensions = NSMakeSize(width, height);
  oo::autorelease(std::shared_ptr<TestTexture>(texture));
  return texture.get();
}

// The window box of the feedback projection: object x, y in [-kHalf, kHalf], z
// in [-kDepth, kDepth].
constexpr double kHalf = 1024.0;
constexpr double kDepth = 1.0e6;

struct Vertex {
  double x, y, z;  // window coordinates
  double alpha;
  double s, t;  // texture coordinates
};

// Where an object-space point lands in the 16x16 window under the feedback
// projection.
Vertex Expect(double x, double y, double z, double alpha, double s, double t) {
  return Vertex{(x / kHalf + 1.0) * 8.0,
                (y / kHalf + 1.0) * 8.0,
                (-z / kDepth + 1.0) / 2.0,
                alpha,
                s,
                t};
}

bool Near(double a, double b) {
  return std::fabs(a - b) <= 1.0e-3 * (1.0 + std::fabs(b));
}

bool Same(const Vertex& a, const Vertex& b) {
  return Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z) &&
         Near(a.alpha, b.alpha) && Near(a.s, b.s) && Near(a.t, b.t);
}

// Runs draw in feedback mode and answers the vertices of every polygon it
// handed to OpenGL. A quad may come back as one polygon or as two triangles, so
// the caller compares the distinct vertices.
template <typename Draw>
std::vector<Vertex> Feedback(Draw draw) {
  static GLfloat buffer[4096];

  glViewport(0, 0, 16, 16);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-kHalf, kHalf, -kHalf, kHalf, -kDepth, kDepth);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glFeedbackBuffer(sizeof buffer / sizeof *buffer, GL_3D_COLOR_TEXTURE, buffer);
  glRenderMode(GL_FEEDBACK);
  draw();
  GLint count = glRenderMode(GL_RENDER);

  std::vector<Vertex> vertices;
  for (GLint i = 0; i < count;) {
    GLint token = (GLint)buffer[i++];
    if (token == GL_POLYGON_TOKEN) {
      GLint n = (GLint)buffer[i++];
      for (GLint v = 0; v < n; v++, i += 11) {
        // x y z, r g b a, s t r q
        Vertex vertex{buffer[i],     buffer[i + 1], buffer[i + 2],
                      buffer[i + 6], buffer[i + 7], buffer[i + 8]};
        bool seen = false;
        for (const Vertex& old : vertices) seen = seen || Same(old, vertex);
        if (!seen) vertices.push_back(vertex);
      }
    } else if (token == GL_PASS_THROUGH_TOKEN)
      i += 1;
    else if (token == GL_POINT_TOKEN || token == GL_BITMAP_TOKEN ||
             token == GL_DRAW_PIXEL_TOKEN || token == GL_COPY_PIXEL_TOKEN)
      i += 11;
    else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN)
      i += 22;
    else {
      std::fprintf(stderr, "unexpected feedback token %d\n", token);
      break;
    }
  }
  return vertices;
}

// The quad of a blit to (x, y, z) of size (w, h): ACW from the top left, the
// texture upside down.
bool IsQuad(const std::vector<Vertex>& vertices, double x, double y, double z,
            double w, double h, double alpha) {
  const Vertex corners[4] = {
      Expect(x, y + h, z, alpha, 0.0, 0.0),
      Expect(x, y, z, alpha, 0.0, 1.0),
      Expect(x + w, y, z, alpha, 1.0, 1.0),
      Expect(x + w, y + h, z, alpha, 1.0, 0.0),
  };
  bool ok = vertices.size() == 4;
  for (const Vertex& corner : corners) {
    bool found = false;
    for (const Vertex& v : vertices) found = found || Same(v, corner);
    ok = ok && found;
  }
  if (!ok) {
    std::fprintf(
        stderr,
        "quad at (%g, %g, %g) size (%g, %g) alpha %g; got %zu vertices:\n", x,
        y, z, w, h, alpha, vertices.size());
    for (const Vertex& v : vertices)
      std::fprintf(stderr, "  (%g, %g, %g) a=%g st=(%g, %g)\n", v.x, v.y, v.z,
                   v.alpha, v.s, v.t);
  }
  return ok;
}

bool GLReady() {
  if (!OOTestGLContext()) return false;
  (void)
      oo::OpenGLExtensionManager::sharedManager();  // loads glActiveTextureARB &
                                                  // co. for the state manager
  return true;
}

}  // namespace

// --- Tests
// --------------------------------------------------------------------------------------

TEST(OOTextureSprite, noTextureNoSprite) {
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(oo::TextureSprite::initWithTexture(nullptr).get() == nullptr);
    EXPECT_TRUE(oo::TextureSprite::initWithTexture(nullptr, NSMakeSize(4, 4)).get() ==
             nullptr);
  }
}

TEST(OOTextureSprite, sizeIsTheTexturesOrGiven) {
  {
    oo::AutoreleaseScope pool;
    std::shared_ptr<oo::TextureSprite> sprite =
        oo::TextureSprite::initWithTexture(Texture(64, 32));
    EXPECT_TRUE(sprite != nullptr);
    EXPECT_TRUE(sprite->getSize().width == 64 && sprite->getSize().height == 32);

    sprite =
        oo::TextureSprite::initWithTexture(Texture(64, 32), NSMakeSize(10, 20));
    EXPECT_TRUE(sprite->getSize().width == 10 && sprite->getSize().height == 20);
  }
}

TEST(OOTextureSprite, spriteKeepsItsTexture) {
  std::shared_ptr<oo::TextureSprite> sprite;
  TestTexture* texture = nullptr;
  {
    oo::AutoreleaseScope pool;
    texture = Texture(8, 8);
    sprite = oo::TextureSprite::initWithTexture(texture);
    oo::test::Retain(texture);
  }
  EXPECT_TRUE(texture->weak_from_this().use_count() == 2);  // the test's and the sprite's
  sprite = nullptr;
  EXPECT_TRUE(texture->weak_from_this().use_count() == 1);
  oo::test::Release(texture);
}

TEST(OOTextureSprite, blitDrawsTheTexturedQuad) {
  EXPECT_TRUE(GLReady());
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = Texture(64, 32);
    std::shared_ptr<oo::TextureSprite> sprite =
        oo::TextureSprite::initWithTexture(texture, NSMakeSize(100, 50));

    std::vector<Vertex> quad =
        Feedback([&] { sprite->blitToX(10, -20, 30, 0.25f); });
    EXPECT_TRUE(IsQuad(quad, 10, -20, 30, 100, 50, 0.25));
    EXPECT_TRUE(texture->_applied == 1);
    EXPECT_TRUE(!glIsEnabled(GL_TEXTURE_2D));
    EXPECT_TRUE(glIsEnabled(GL_BLEND));  // the overlay state
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);

    // The alpha is clamped to [0, 1].
    EXPECT_TRUE(IsQuad(Feedback([&] { sprite->blitToX(0, 0, 0, 1.5f); }), 0, 0, 0,
                    100, 50, 1.0));
    EXPECT_TRUE(IsQuad(Feedback([&] { sprite->blitToX(0, 0, 0, -1.0f); }), 0, 0, 0,
                    100, 50, 0.0));
    EXPECT_TRUE(texture->_applied == 3);
  }
}

TEST(OOTextureSprite, centredBlits) {
  EXPECT_TRUE(GLReady());
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = Texture(64, 32);
    std::shared_ptr<oo::TextureSprite> sprite =
        oo::TextureSprite::initWithTexture(texture, NSMakeSize(1.5, 0.75));

    EXPECT_TRUE(
        IsQuad(Feedback([&] { sprite->blitCentredToX(100, 200, -5, 0.5f); }),
               100 - 0.75, 200 - 0.375, -5, 1.5, 0.75, 0.5));

    // The background blit scales the size and z by 512 (so that it is behind
    // the ships), and puts the size back afterwards.
    EXPECT_TRUE(IsQuad(
        Feedback([&] { sprite->blitBackgroundCentredToX(1, -2, 3, 1.0f); }),
        1 - 384, -2 - 192, 3 * 512, 768, 384, 1.0));
    EXPECT_TRUE(sprite->getSize().width == 1.5 &&
             sprite->getSize().height == 0.75);
    EXPECT_TRUE(texture->_applied == 2);
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}

// --- The C++ class (after the conversion)
// ---------------------------------------------------------

TEST(OOTextureSprite, cxxSprite) {
  EXPECT_TRUE(GLReady());
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(oo::TextureSprite::initWithTexture(nullptr).get() == nullptr);
    EXPECT_TRUE(oo::TextureSprite::initWithTexture(nullptr, NSMakeSize(4, 4)).get() ==
             nullptr);

    TestTexture* texture = Texture(64, 32);
    std::shared_ptr<oo::TextureSprite> sprite = oo::TextureSprite::initWithTexture(texture);
    EXPECT_TRUE(sprite.get() != nullptr && sprite->getSize().width == 64 &&
             sprite->getSize().height == 32);

    sprite = oo::TextureSprite::initWithTexture(texture, NSMakeSize(100, 50));
    EXPECT_TRUE(IsQuad(Feedback([&] { sprite->blitToX(10, -20, 30, 0.25f); }), 10,
                    -20, 30, 100, 50, 0.25));
    EXPECT_TRUE(IsQuad(Feedback([&] { sprite->blitCentredToX(0, 0, 0, 1.0f); }),
                    -50, -25, 0, 100, 50, 1.0));
    EXPECT_TRUE(sprite->getSize().width == 100 && sprite->getSize().height == 50);

    sprite = oo::TextureSprite::initWithTexture(texture, NSMakeSize(1.5, 0.75));
    EXPECT_TRUE(IsQuad(
        Feedback([&] { sprite->blitBackgroundCentredToX(1, -2, 3, 1.0f); }),
        1 - 384, -2 - 192, 3 * 512, 768, 384, 1.0));
    EXPECT_TRUE(sprite->getSize().width == 1.5 &&
             sprite->getSize().height == 0.75);
    EXPECT_TRUE(texture->_applied == 3);
  }
}
