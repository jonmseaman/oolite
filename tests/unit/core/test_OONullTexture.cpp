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

#include "NullTexture.h"
#include "oo_gl_test_context.hpp"
#include "oo_test.hpp"

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

OO_TEST(oneSharedInstance) {
  {
    oo::Texture* shared = oo::Texture::nullTexture().get();
    OO_CHECK(shared != nullptr && shared == oo::Texture::nullTexture().get());
    OO_CHECK(shared == oo::Texture::nullTexture().get());
    OO_CHECK(dynamic_cast<oo::NullTexture*>(shared) != nullptr);
  }
  // It outlives the pools it was handed out in.
  {
    OO_CHECK(oo::Texture::nullTexture().get() ==
             oo::NullTexture::sharedNullTexture());
  }
}

OO_TEST(answersAsAnEmptyTexture) {
  {
    oo::Texture* none = oo::Texture::nullTexture().get();
    OO_CHECK(SameSize(none->dimensions(), 0, 0) &&
             SameSize(none->originalDimensions(), 0, 0));
    OO_CHECK(!none->isMipMapped());
    OO_CHECK(none->isFinishedLoading() && !none->cacheKey().has_value());
    OO_CHECK(!none->isRectangleTexture() && !none->isCubeMap());
    OO_CHECK(SameSize(none->texCoordsScale(), 1, 1));
    OO_CHECK(oo::IsNullPixMap(none->copyPixMapRepresentation()));
    none->ensureFinishedLoading();
    none->forceRebind();  // nothing to rebind
#ifndef NDEBUG
    OO_CHECK(none->name() == std::optional<std::string>("<null texture>"));
    OO_CHECK(none->dataSize() == 0);
#endif
    OO_CHECK(none->description().starts_with("<OONullTexture 0x"));
  }
}

OO_TEST(applyingBindsNoTexture) {
  OO_CHECK(OOTestGLContext());
  {
    GLuint name = 0;
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    OO_CHECK(BoundTexture() == (GLint)name);
    oo::Texture::nullTexture()->apply();
    OO_CHECK(BoundTexture() == 0);
    glDeleteTextures(1, &name);
  }
}

OO_TEST(graphicsResetLeavesItAlone) {
  OO_CHECK(OOTestGLContext());
  {
    oo::Texture* shared = oo::Texture::nullTexture().get();
    oo::Texture::rebindAllTextures();
    OO_CHECK(oo::Texture::nullTexture().get() == shared);
    OO_CHECK(SameSize(shared->dimensions(), 0, 0));
  }
}

// --- The C++ API (after the conversion)
// ---------------------------------------------------------

OO_TEST(cxxApi) {
  OO_CHECK(OOTestGLContext());
  oo::NullTexture* none = oo::NullTexture::sharedNullTexture();
  OO_CHECK(none != nullptr && none == oo::NullTexture::sharedNullTexture());

  // Through the root's virtual members, as the materials call a texture.
  oo::Texture* texture = none;
  OO_CHECK(SameSize(texture->dimensions(), 0, 0) &&
           SameSize(texture->originalDimensions(), 0, 0));
  OO_CHECK(!texture->isMipMapped() && texture->isFinishedLoading() &&
           !texture->cacheKey().has_value());
  texture->forceRebind();
#ifndef NDEBUG
  OO_CHECK(texture->name() == std::optional<std::string>("<null texture>"));
#endif
  GLuint name = 0;
  glGenTextures(1, &name);
  glBindTexture(GL_TEXTURE_2D, name);
  texture->apply();
  OO_CHECK(BoundTexture() == 0);
  glDeleteTextures(1, &name);
}

OO_TEST_MAIN()
