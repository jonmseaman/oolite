/*	test_OOConcreteTexture.mm
        Unit tests for OOConcreteTexture
   (src/Core/Materials/OOConcreteTexture.h), the texture that a loader fills and
   OpenGL holds.

        A concrete texture is made with a loader (here a generator, so no file
   is read and the resource manager is never reached), caches itself under its
   key, and on first use waits for the loader, uploads the pixels to OpenGL
   (with mip-maps when asked) and answers their size. It links the whole game
   but main (tests/unit/core/meson.build entry ['*']), on the hidden GL context
   of oo_gl_test_context.hpp; the generator runs on the game's own work manager.
   The texture is made by the C++ factories, and the cases ask the C++ texture,
   kept until the pool drains. Run: tests/run.sh unit
*/

#include <cstdlib>
#include <cstring>

#include <gtest/gtest.h>

#include "ConcreteTexture.h"
#include "TextureGenerator.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

// A generator of a 4 x 2 RGBA image whose bytes count up from 1, or of nothing.
class TestGenerator : public oo::TextureGenerator {
 public:
  void loadTexture() override {
    if (_empty) return;
    _width = 4;
    _height = 2;
    _format = oo::kPixMapRGBA;
    oo::PixelBuffer buffer = oo::MakePixelBuffer(4 * 2 * 4);
    uint8_t* bytes = buffer->data();
    for (unsigned i = 0; i < 4 * 2 * 4; i++) bytes[i] = (uint8_t)(i + 1);
    _data = buffer;
  }

  bool _empty = false;
};

// What an autoreleased texture is: the C++ texture, kept until the current
// autorelease scope drains (oo::autorelease).
static oo::Texture* Autoreleased(const std::shared_ptr<oo::Texture>& texture) {
  if (texture == nullptr) return nullptr;
  oo::autorelease(std::shared_ptr<oo::Texture>(texture));
  return texture.get();
}

namespace {

bool SameSize(NSSize size, double width, double height) {
  return size.width == width && size.height == height;
}

GLint BoundTexture() {
  GLint bound = -1;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
  return bound;
}

// The loaders' sizes come from their one-time set-up, which a path with no
// known extension runs (and then answers nil, reading nothing).
void SetUpLoaders() {
  {
    oo::AutoreleaseScope pool;
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

// The generators made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<TestGenerator>> gKeptGenerators;

TestGenerator* NewGenerator(oo::TextureFlags options) {
  const std::shared_ptr<TestGenerator> generator = oo::makeShared<TestGenerator>();
  if (!generator->initWithPath(std::string("test generator"), options))
    return nullptr;
  gKeptGenerators.push_back(generator);
  return generator.get();
}

TestGenerator* QueuedGenerator(oo::TextureFlags options, bool empty) {
  TestGenerator* generator = NewGenerator(options);
  generator->_empty = empty;
  EXPECT_TRUE(generator->enqueue());
  return generator;
}

// The texture the factory makes, kept until the pool drains as its facade was
// (null for none).
oo::Texture* Texture(oo::TextureLoader* loader,
                   const std::optional<std::string>& key, oo::TextureFlags options) {
  return Autoreleased(
      oo::ConcreteTexture::initWithLoader(loader, key, options, 0.5f, 0.0f));
}

void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

}  // namespace

TEST(OOConcreteTexture, noLoaderNoTexture) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(Texture(nullptr, std::string("test:none"), oo::TextureFlags::kDefaultOptions) ==
             nullptr);
    EXPECT_TRUE(oo::ConcreteTexture::initWithPath(
                 "no-such-loader.type", std::string("test:none"),
                 oo::TextureFlags::kDefaultOptions, 0.5f, 0.0f) == nullptr);
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:none")) == nullptr);
  }
}

TEST(OOConcreteTexture, beforeLoading) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const oo::TextureFlags options =
        oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear;
    oo::Texture* texture = Texture(QueuedGenerator(options, false),
                                 std::string("test:before"), options);
    EXPECT_TRUE(texture != nullptr);
    EXPECT_TRUE(texture->cacheKey() == std::optional<std::string>("test:before"));
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:before")) ==
             texture);
    EXPECT_TRUE(SameSize(texture->texCoordsScale(), 1,
                      1));  // no rectangle texture: no wait
    EXPECT_TRUE(!texture->isRectangleTexture() && !texture->isCubeMap());
#ifndef NDEBUG
    EXPECT_TRUE(texture->name() == std::optional<std::string>("<TestGenerator>"));
#endif
    const std::string text = texture->description();
    EXPECT_TRUE(text.starts_with("<OOConcreteTexture 0x") &&
             text.ends_with(">{test:before, loading}"));
    EXPECT_TRUE(texture->shortDescription().ends_with(">{test:before}"));
  }
  ClearCache();
}

TEST(OOConcreteTexture, loadsAndUploads) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const oo::TextureFlags options =
        oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear;
    oo::Texture* texture = Texture(QueuedGenerator(options, false),
                                 std::string("test:linear"), options);
    texture->ensureFinishedLoading();
    EXPECT_TRUE(texture->isFinishedLoading());
    // The loader scales the 4 x 2 image to 2 x 2; the original size is kept.
    EXPECT_TRUE(SameSize(texture->dimensions(), 2, 2) &&
             SameSize(texture->originalDimensions(), 4, 2));
    EXPECT_TRUE(!texture->isMipMapped());
    const GLint name = texture->glTextureName();
    EXPECT_TRUE(name != 0);
    EXPECT_TRUE(texture->description().ends_with(">{test:linear, 2 x 2}"));
#ifndef NDEBUG
    EXPECT_TRUE(texture->dataSize() == 4);
#endif

    // Applying binds it.
    glBindTexture(GL_TEXTURE_2D, 0);
    texture->apply();
    EXPECT_TRUE(BoundTexture() == name);

    // A generated texture keeps its bytes (it cannot be reloaded): the copy is
    // of them.
    oo::PixMap pixMap = texture->copyPixMapRepresentation();
    EXPECT_TRUE(oo::IsValidPixMap(pixMap) && pixMap.width == 2 &&
             pixMap.height == 2 && pixMap.format == oo::kPixMapRGBA &&
             pixMap.rowBytes == 8);
    const uint8_t* bytes = (const uint8_t*)pixMap.pixels;
    EXPECT_TRUE(bytes != NULL && bytes[0] == 3);  // the scaled first pixel
    oo::FreePixMap(&pixMap);

    // A graphics reset deletes the GL texture; the next use uploads it again.
    texture->forceRebind();
    texture->apply();
    EXPECT_TRUE(texture->glTextureName() != 0 &&
             BoundTexture() == texture->glTextureName());
    EXPECT_TRUE(SameSize(texture->dimensions(), 2, 2));
  }
  ClearCache();
}

TEST(OOConcreteTexture, mipMapped) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const oo::TextureFlags options =
        oo::TextureFlags::kMinFilterMipMap | oo::TextureFlags::kMagFilterLinear;
    oo::Texture* texture =
        Texture(QueuedGenerator(options, false), std::string("test:mip"), options);
    EXPECT_TRUE(texture->isMipMapped());  // waits for loading
    EXPECT_TRUE(SameSize(texture->dimensions(), 2, 2));
    EXPECT_TRUE(texture->glTextureName() != 0);
    GLint maxLevel = -1;
    glBindTexture(GL_TEXTURE_2D, texture->glTextureName());
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &maxLevel);
    EXPECT_TRUE(maxLevel == 1);
#ifndef NDEBUG
    EXPECT_TRUE(texture->dataSize() == 5);  // 2 * 2, mip-mapped: * 4 / 3
#endif
  }
  ClearCache();
}

TEST(OOConcreteTexture, failedLoad) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::Texture* texture =
        Texture(QueuedGenerator(oo::TextureFlags::kDefaultOptions, true),
                std::string("test:empty"), oo::TextureFlags::kDefaultOptions);
    EXPECT_TRUE(SameSize(texture->dimensions(), 0, 0));
    EXPECT_TRUE(texture->glTextureName() == 0 && texture->isFinishedLoading());
    EXPECT_TRUE(texture->description().ends_with(">{test:empty, LOAD ERROR}"));
    EXPECT_TRUE(oo::IsNullPixMap(texture->copyPixMapRepresentation()));
    texture->forceRebind();  // not valid: nothing to delete
    texture->apply();
  }
  ClearCache();
}

TEST(OOConcreteTexture, deallocUncaches) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const oo::TextureFlags options =
        oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear;
    oo::Texture* texture = Texture(QueuedGenerator(options, false),
                                 std::string("test:gone"), options);
    texture->ensureFinishedLoading();
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gone")) ==
             texture);
  }
  // Kept by the recent textures until they are cleared; then released, and
  // uncached.
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gone")) != nullptr);
  ClearCache();
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gone")) == nullptr);
#ifndef NDEBUG
  {
    oo::AutoreleaseScope pool;
    for (const std::shared_ptr<oo::Texture>& texture : oo::Texture::allTextures()) {
      EXPECT_TRUE(texture->cacheKey() != std::optional<std::string>("test:gone"));
    }
  }
#endif
}

// --- The C++ API (after the conversion)
// ---------------------------------------------------------

TEST(OOConcreteTexture, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(oo::ConcreteTexture::initWithLoader(nullptr, std::string("test:cxx"),
                                               oo::TextureFlags::kDefaultOptions, 0.5f,
                                               0.0f) == nullptr);
    EXPECT_TRUE(oo::ConcreteTexture::initWithPath(
                 "no-such-loader.type", std::string("test:cxx"),
                 oo::TextureFlags::kDefaultOptions, 0.5f, 0.0f) == nullptr);

    const oo::TextureFlags options =
        oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear;
    const std::shared_ptr<oo::ConcreteTexture> texture =
        oo::ConcreteTexture::initWithLoader(QueuedGenerator(options, false),
                                          std::string("test:cxx"), options,
                                          0.5f, 0.0f);
    EXPECT_TRUE(texture != nullptr &&
             texture->cacheKey() == std::optional<std::string>("test:cxx"));
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cxx")) ==
             texture.get());
    EXPECT_TRUE(texture->descriptionComponents() ==
             std::optional<std::string>("test:cxx, loading"));
    EXPECT_TRUE(texture->shortDescriptionComponents() ==
             std::optional<std::string>("test:cxx"));

    texture->ensureFinishedLoading();
    EXPECT_TRUE(texture->isFinishedLoading());
    EXPECT_TRUE(SameSize(texture->dimensions(), 2, 2) &&
             SameSize(texture->originalDimensions(), 4, 2));
    EXPECT_TRUE(texture->glTextureName() != 0 && !texture->isMipMapped());
    EXPECT_TRUE(texture->descriptionComponents() ==
             std::optional<std::string>("test:cxx, 2 x 2"));
    glBindTexture(GL_TEXTURE_2D, 0);
    texture->apply();
    EXPECT_TRUE(BoundTexture() == texture->glTextureName());
#ifndef NDEBUG
    EXPECT_TRUE(texture->name() == std::optional<std::string>("<TestGenerator>") &&
             texture->dataSize() == 4);
    texture->setTrace(true);
    texture->setTrace(false);
#endif
  }
  ClearCache();
}
