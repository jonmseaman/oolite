/*	test_OONullTexture.mm
        Unit tests for OONullTexture (src/Core/Materials/OONullTexture.h), the
   texture that applies no texture, a leaf of the textures root.

        The null texture is a process-wide singleton that OOTexture's
   +nullTexture answers. It links the whole game but main
   (tests/unit/core/meson.build entry ['*']), on the hidden GL context of
   oo_gl_test_context.hpp, because applying it unbinds the current texture. The
   expectations cover: one shared instance, what it answers as a texture (no
   size, not mip-mapped, the root's defaults), that applying it binds no
   texture, that a graphics reset leaves it alone, and its debug name. The cases
   call the C++ texture OOTexture::nullTexture() answers; a class check is a
   dynamic_cast of the C++ texture. Run: tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "NullTexture.h"
#include "oo_gl_test_context.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

bool SameSize(NSSize size, double width, double height) {
  return size.width == width && size.height == height;
}

GLint BoundTexture() {
  GLint bound = -1;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
  return bound;
}

}  // namespace

TEST(OONullTexture, oneSharedInstance) {
  {
    oo::Texture* shared = oo::Texture::nullTexture().get();
    EXPECT_TRUE(shared != nullptr && shared == oo::Texture::nullTexture().get());
    EXPECT_TRUE(shared == oo::Texture::nullTexture().get());
    EXPECT_TRUE(dynamic_cast<oo::NullTexture*>(shared) != nullptr);
  }
  // It outlives the pools it was handed out in.
  {
    EXPECT_TRUE(oo::Texture::nullTexture().get() ==
             oo::NullTexture::sharedNullTexture());
  }
}

TEST(OONullTexture, answersAsAnEmptyTexture) {
  {
    oo::Texture* none = oo::Texture::nullTexture().get();
    EXPECT_TRUE(SameSize(none->dimensions(), 0, 0) &&
             SameSize(none->originalDimensions(), 0, 0));
    EXPECT_TRUE(!none->isMipMapped());
    EXPECT_TRUE(none->isFinishedLoading() && !none->cacheKey().has_value());
    EXPECT_TRUE(!none->isRectangleTexture() && !none->isCubeMap());
    EXPECT_TRUE(SameSize(none->texCoordsScale(), 1, 1));
    EXPECT_TRUE(oo::IsNullPixMap(none->copyPixMapRepresentation()));
    none->ensureFinishedLoading();
    none->forceRebind();  // nothing to rebind
#ifndef NDEBUG
    EXPECT_TRUE(none->name() == std::optional<std::string>("<null texture>"));
    EXPECT_TRUE(none->dataSize() == 0);
#endif
    EXPECT_TRUE(none->description().starts_with("<OONullTexture 0x"));
  }
}

TEST(OONullTexture, applyingBindsNoTexture) {
  EXPECT_TRUE(OOTestGLContext());
  {
    GLuint name = 0;
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    EXPECT_TRUE(BoundTexture() == (GLint)name);
    oo::Texture::nullTexture()->apply();
    EXPECT_TRUE(BoundTexture() == 0);
    glDeleteTextures(1, &name);
  }
}

TEST(OONullTexture, graphicsResetLeavesItAlone) {
  EXPECT_TRUE(OOTestGLContext());
  {
    oo::Texture* shared = oo::Texture::nullTexture().get();
    oo::Texture::rebindAllTextures();
    EXPECT_TRUE(oo::Texture::nullTexture().get() == shared);
    EXPECT_TRUE(SameSize(shared->dimensions(), 0, 0));
  }
}

// --- The C++ API (after the conversion)
// ---------------------------------------------------------

TEST(OONullTexture, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  oo::NullTexture* none = oo::NullTexture::sharedNullTexture();
  EXPECT_TRUE(none != nullptr && none == oo::NullTexture::sharedNullTexture());

  // Through the root's virtual members, as the materials call a texture.
  oo::Texture* texture = none;
  EXPECT_TRUE(SameSize(texture->dimensions(), 0, 0) &&
           SameSize(texture->originalDimensions(), 0, 0));
  EXPECT_TRUE(!texture->isMipMapped() && texture->isFinishedLoading() &&
           !texture->cacheKey().has_value());
  texture->forceRebind();
#ifndef NDEBUG
  EXPECT_TRUE(texture->name() == std::optional<std::string>("<null texture>"));
#endif
  GLuint name = 0;
  glGenTextures(1, &name);
  glBindTexture(GL_TEXTURE_2D, name);
  texture->apply();
  EXPECT_TRUE(BoundTexture() == 0);
  glDeleteTextures(1, &name);
}
