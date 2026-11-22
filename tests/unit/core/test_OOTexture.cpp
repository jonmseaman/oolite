/*	test_OOTexture.mm
        Unit tests for OOTexture (src/Core/Materials/OOTexture.h), the root of
   the textures.

        OOTexture is abstract. OOConcreteTexture and OONullTexture derive from
   it in their own files. The root keeps the
   texture caches (the live textures by cache key, unretained; the recent
   textures, retained; every live texture, for graphics resets) and is the
   factory that answers a cached texture or a new OOConcreteTexture. It links
   the whole game but main (tests/unit/core/meson.build entry ['*']), on the
   hidden GL context of oo_gl_test_context.hpp, because the factory checks the
   GL extensions on first use. Nothing here reaches the resource manager: the
   named texture is found in the cache by its key, and the generator's -enqueue
   is the test's, so no file is read and no worker thread runs.

        The cases call the C++ members. Two subclasses stand for the concrete
   textures: TestTexture overrides
   what OOConcreteTexture does and caches itself under a key, BareTexture
   overrides nothing, so it answers the root's defaults. An autoreleased texture
   is kept until its scope drains (oo::autorelease). After them comes the C++
   API. Run: tests/run.sh unit
*/

#include <algorithm>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "ConcreteTexture.h"
#include "NullTexture.h"
#include "Texture.h"
#include "TextureGenerator.h"
#include "TextureInternal.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

static int gTestDeallocs = 0;

// A texture as OOConcreteTexture is one: it caches itself under its key, and
// uncaches itself.
class TestTexture final : public oo::Texture {
 public:
  explicit TestTexture(std::optional<std::string> key) : _key(std::move(key)) {}
  // Made, then cached under its key (an owner is needed first, as the game's
  // OOConcreteTexture is cached after it is made).
  static std::shared_ptr<TestTexture> make(std::optional<std::string> key) {
    std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>(std::move(key));
    texture->addToCaches();
    return texture;
  }
  ~TestTexture() override {
    gTestDeallocs++;
    removeFromCaches();
  }

  void apply() override { _applies++; }
  NSSize dimensions() override { return NSMakeSize(8, 4); }
  bool isMipMapped() override { return true; }
  void forceRebind() override { _rebinds++; }
  std::optional<std::string> cacheKey() override { return _key; }
  GLint glTextureName() override { return 7; }
  std::optional<std::string> descriptionComponents() const override {
    return std::string("test");
  }
#ifndef NDEBUG
  std::optional<std::string> name() override {
    return std::string("test texture");
  }
#endif

  std::optional<std::string> _key;
  int _rebinds = 0;
  int _applies = 0;
};

// A texture that overrides nothing: the root's own answers.
class BareTexture final : public oo::Texture {};

// What an autoreleased texture is: the C++ texture, kept until the current
// autorelease scope drains (oo::autorelease).
static oo::Texture* Autoreleased(const std::shared_ptr<oo::Texture>& texture) {
  if (texture == nullptr) return nullptr;
  oo::autorelease(std::shared_ptr<oo::Texture>(texture));
  return texture.get();
}

// A generator that is never queued: enqueue() answers what the test says, and
// counts.
class TestGenerator : public oo::TextureGenerator {
 public:
  std::optional<std::string> cacheKey() override { return _key; }
  bool enqueue() override {
    _enqueues++;
    return _accept;
  }

  std::optional<std::string> _key;
  bool _accept = false;
  int _enqueues = 0;
};

namespace {

TestTexture* MakeTexture(const std::optional<std::string>& key) {
  return static_cast<TestTexture*>(Autoreleased(TestTexture::make(key)));
}

// The generators made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<TestGenerator>> gKeptGenerators;

TestGenerator* MakeGenerator(const std::optional<std::string>& key,
                             bool accept) {
  const std::shared_ptr<TestGenerator> made = oo::makeShared<TestGenerator>();
  if (!made->initWithPath(std::string("test generator"), oo::TextureFlags::kNone)) return nullptr;
  gKeptGenerators.push_back(made);
  TestGenerator* generator = made.get();
  generator->_key = key;
  generator->_accept = accept;
  return generator;
}

// -clearCache autoreleases the recent textures (a C++ cache), which needs a
// scope to be released.
void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

bool SameSize(NSSize size, double width, double height) {
  return size.width == width && size.height == height;
}

#ifndef NDEBUG
bool Contains(const std::vector<std::shared_ptr<oo::Texture>>& textures,
              oo::Texture* texture) {
  return std::ranges::find(textures, texture, [](const auto& owner) {
           return owner.get();
         }) != textures.end();
}
#endif

}  // namespace

TEST(OOTexture, rootDefaults) {
  {
    oo::AutoreleaseScope pool;
    oo::Texture* bare = Autoreleased(oo::makeShared<BareTexture>());
    EXPECT_TRUE(bare->isFinishedLoading());
    EXPECT_TRUE(!bare->cacheKey().has_value());
    EXPECT_TRUE(!bare->isRectangleTexture() && !bare->isCubeMap());
    EXPECT_TRUE(SameSize(bare->texCoordsScale(), 1, 1));
    EXPECT_TRUE(oo::IsNullPixMap(bare->copyPixMapRepresentation()));
    bare->ensureFinishedLoading();

    // The subclass responsibilities log and answer zero.
    EXPECT_TRUE(SameSize(bare->dimensions(), 0, 0));
    EXPECT_TRUE(SameSize(bare->originalDimensions(), 0, 0));
    EXPECT_TRUE(!bare->isMipMapped());
    EXPECT_TRUE(bare->glTextureName() == 0);
    bare->apply();
#ifndef NDEBUG
    EXPECT_TRUE(!bare->name().has_value());
    EXPECT_TRUE(bare->dataSize() == 0);
#endif
    EXPECT_TRUE(bare->description().starts_with("<BareTexture 0x"));
  }
}

TEST(OOTexture, subclassOverrides) {
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = MakeTexture(std::nullopt);
    EXPECT_TRUE(SameSize(texture->originalDimensions(), 8,
                      4));  // the root's, from -dimensions
    EXPECT_TRUE(SameSize(texture->texCoordsScale(), 1, 1));
    EXPECT_TRUE(texture->glTextureName() == 7 && texture->isMipMapped());
    texture->apply();
    EXPECT_TRUE(texture->_applies == 1);
#ifndef NDEBUG
    EXPECT_TRUE(texture->dataSize() == 42);  // 8 * 4, mip-mapped: * 4 / 3
    EXPECT_TRUE(texture->name() == std::optional<std::string>("test texture"));
    texture->setTrace(true);
    texture->setTrace(false);
#endif
    const std::string text = texture->description();
    EXPECT_TRUE(text.starts_with("<TestTexture 0x") && text.ends_with(">{test}"));
  }
}

TEST(OOTexture, cachesKeepAndForget) {
  ClearCache();
  const int deallocs = gTestDeallocs;
  TestTexture* texture = nullptr;
  oo::Texture* bare = nullptr;
  {
    oo::AutoreleaseScope pool;
    texture = MakeTexture(std::string("test:cache"));
    bare = oo::test::NewRetained<BareTexture>();
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cache")) ==
             texture);
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::nullopt) == nullptr);
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:none")) == nullptr);
  }

  // The recent textures keep it alive; the live textures find it.
  EXPECT_TRUE(gTestDeallocs == deallocs);
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cache")) ==
           texture);
#ifndef NDEBUG
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(Contains(oo::Texture::cachedTexturesByAge(), texture));
    EXPECT_TRUE(!Contains(oo::Texture::cachedTexturesByAge(),
                       bare));  // no key: not cached
    const std::vector<std::shared_ptr<oo::Texture>> all = oo::Texture::allTextures();
    EXPECT_TRUE(Contains(all, texture) && Contains(all, bare));
  }
#endif

  // Clearing forgets both caches, and releases what only they kept.
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cache")) ==
             nullptr);
  }
  EXPECT_TRUE(gTestDeallocs == deallocs + 1);
#ifndef NDEBUG
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(oo::Texture::cachedTexturesByAge().empty());
    const std::vector<std::shared_ptr<oo::Texture>> all = oo::Texture::allTextures();
    EXPECT_TRUE(Contains(all, bare));
    EXPECT_TRUE(
        std::none_of(all.begin(), all.end(), [](const std::shared_ptr<oo::Texture>& t) {
          return dynamic_cast<TestTexture*>(t.get()) != nullptr;
        }));
  }
#endif
  oo::test::Release(bare);
}

TEST(OOTexture, rebindAllTextures) {
  const int deallocs = gTestDeallocs;
  {
    oo::AutoreleaseScope pool;
    TestTexture* cached = MakeTexture(std::string("test:rebind"));
    TestTexture* plain = MakeTexture(std::nullopt);
    oo::Texture::rebindAllTextures();
    EXPECT_TRUE(cached->_rebinds == 1 && plain->_rebinds == 1);

    // The recent textures are dropped; the live textures still find it while it
    // lives.
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:rebind")) ==
             cached);
#ifndef NDEBUG
    EXPECT_TRUE(oo::Texture::cachedTexturesByAge().empty());
#endif
  }
  EXPECT_TRUE(gTestDeallocs == deallocs + 2);
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:rebind")) == nullptr);
}

TEST(OOTexture, namedTextureFromTheCache) {
  EXPECT_TRUE(OOTestGLContext());
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(oo::Texture::textureWithName(std::nullopt, std::string("Textures"))
                 .get() == nullptr);

    // Linear filtering: no anisotropy and no LOD bias in the key, whatever the
    // extensions.
    const std::string key = oo::GenerateTextureCacheKey(
        std::string("Textures"), "test.png",
        oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear, 0.5f, -0.25f);
    EXPECT_TRUE(key == "Textures/test.png:0x0006/0/0");
    TestTexture* texture = MakeTexture(key);

    EXPECT_TRUE(oo::Texture::textureWithName(
                 std::string("test.png"), std::string("Textures"),
                 oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear |
                     oo::TextureFlags::kNoFNFMessage,
                 kOOTextureDefaultAnisotropy, kOOTextureDefaultLODBias)
                 .get() == texture);
    EXPECT_TRUE(oo::Texture::textureWithConfiguration(oo::PList(oo::PList::Dict{
                 {"name", oo::PList("test.png")},
                 {"min_filter", oo::PList("linear")}})).get() == texture);
    EXPECT_TRUE(oo::Texture::textureWithConfiguration(
                 oo::PList(oo::PList::Dict{{"name", oo::PList("test.png")}}),
                 oo::TextureFlags::kMinFilterLinear).get() == texture);

    // A specifier that names no texture.
    EXPECT_TRUE(oo::Texture::textureWithConfiguration(oo::PList(
                 oo::PList::Dict{{"min_filter", oo::PList("linear")}})) == nullptr);
    EXPECT_TRUE(oo::Texture::textureWithConfiguration(oo::PList(3.0)).get() == nullptr);
    EXPECT_TRUE(oo::Texture::textureWithConfiguration(oo::PList()).get() == nullptr);
    ClearCache();
  }
}

TEST(OOTexture, texturesFromGenerators) {
  {
    oo::AutoreleaseScope pool;
    ClearCache();
    EXPECT_TRUE(oo::Texture::textureWithGenerator(nullptr).get() == nullptr);
    TestGenerator* refusing = MakeGenerator(std::string("test:refused"), false);
    EXPECT_TRUE(oo::Texture::textureWithGenerator(refusing).get() == nullptr &&
             refusing->_enqueues == 1);

    TestGenerator* generator = MakeGenerator(std::string("test:gen"), true);
    oo::Texture* first = Autoreleased(oo::Texture::textureWithGenerator(generator));
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(first) != nullptr) &&
             generator->_enqueues == 1);
    EXPECT_TRUE(first->cacheKey() == std::optional<std::string>("test:gen"));
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gen")) ==
             first);

    // Cached: the same texture, not queued again; unless the caller forces a
    // new one.
    EXPECT_TRUE(oo::Texture::textureWithGenerator(generator).get() == first &&
             generator->_enqueues == 1);
    oo::Texture* second =
        Autoreleased(oo::Texture::textureWithGenerator(generator, true));
    EXPECT_TRUE(second != nullptr && second != first && generator->_enqueues == 2);
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gen")) ==
             second);

    // No key: never cached.
    TestGenerator* uncached = MakeGenerator(std::nullopt, true);
    oo::Texture* a = Autoreleased(oo::Texture::textureWithGenerator(uncached));
    oo::Texture* b = Autoreleased(oo::Texture::textureWithGenerator(uncached));
    EXPECT_TRUE(a != nullptr && b != nullptr && a != b && uncached->_enqueues == 2);
  }

  /*	The first texture's -dealloc (when the recent textures replaced it and
     the pool drained) uncached its key from the live textures, although the
     second texture had it by then.
  */
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:gen")) == nullptr);
  ClearCache();
}

TEST(OOTexture, nullTexture) {
  EXPECT_TRUE(OOTestGLContext());
  {
    oo::AutoreleaseScope pool;
    oo::Texture* none = oo::Texture::nullTexture().get();
    EXPECT_TRUE(dynamic_cast<oo::NullTexture*>(none) != nullptr &&
             none == oo::Texture::nullTexture().get());
    EXPECT_TRUE(SameSize(none->dimensions(), 0, 0) && !none->isMipMapped());
#ifndef NDEBUG
    EXPECT_TRUE(none->name() == std::optional<std::string>("<null texture>"));
#endif
    none->forceRebind();

    // Applying it is applying none.
    GLuint name = 0;
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    none->apply();
    GLint bound = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
    EXPECT_TRUE(bound == 0);
    glDeleteTextures(1, &name);
  }
}

TEST(OOTexture, applyNone) {
  EXPECT_TRUE(OOTestGLContext());
  GLuint name = 0;
  glGenTextures(1, &name);
  glBindTexture(GL_TEXTURE_2D, name);
  GLint bound = -1;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
  EXPECT_TRUE(bound == (GLint)name);
  oo::Texture::applyNone();
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
  EXPECT_TRUE(bound == 0);
  glDeleteTextures(1, &name);
}

// --- The C++ API (after the conversion)
// ---------------------------------------------------------

// A converted texture, as OOConcreteTexture is one: a C++ subclass, cached
// under its key. Global, so it prints its name as a global class's.
class TestCxxTexture final : public oo::Texture {
 public:
  explicit TestCxxTexture(std::optional<std::string> key)
      : _key(std::move(key)) {}
  // Made, then cached under its key (an owner is needed first).
  static std::shared_ptr<TestCxxTexture> make(std::optional<std::string> key) {
    std::shared_ptr<TestCxxTexture> texture =
        oo::makeShared<TestCxxTexture>(std::move(key));
    texture->addToCaches();
    return texture;
  }
  ~TestCxxTexture() override { removeFromCaches(); }

  void apply() override { applies++; }
  NSSize dimensions() override { return NSMakeSize(16, 2); }
  bool isMipMapped() override { return false; }
  void forceRebind() override { rebinds++; }
  std::optional<std::string> cacheKey() override { return _key; }
  GLint glTextureName() override { return 9; }
  std::optional<std::string> descriptionComponents() const override {
    return std::string("cxx");
  }

  int applies = 0;
  int rebinds = 0;

 private:
  std::optional<std::string> _key;
};

TEST(OOTexture, cxxApi) {
  ClearCache();
  {
    oo::AutoreleaseScope pool;
    // The factories answer the C++ texture, retained.
    EXPECT_TRUE(
        !oo::Texture::textureWithName(std::nullopt, std::string("Textures")));
    const std::shared_ptr<oo::Texture> none = oo::Texture::nullTexture();
    EXPECT_TRUE(dynamic_cast<oo::NullTexture*>(none.get()) != nullptr &&
             none.get() == oo::Texture::nullTexture().get());
    TestGenerator* generator = MakeGenerator(std::string("test:cxxgen"), true);
    const std::shared_ptr<oo::Texture> generated =
        oo::Texture::textureWithGenerator(generator);
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(generated.get()) != nullptr));
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cxxgen")) ==
             generated.get());
    EXPECT_TRUE(oo::Texture::textureWithGenerator(generator, false).get() ==
                 generated.get() &&
             generator->_enqueues == 1);
    EXPECT_TRUE(!oo::Texture::textureWithConfiguration(oo::PList(3.0)));
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::nullopt) == nullptr);
    ClearCache();
  }

  // The root's own answers.
  const std::shared_ptr<oo::Texture> root = oo::makeShared<oo::Texture>();
  EXPECT_TRUE(root->isFinishedLoading() && !root->cacheKey().has_value());
  EXPECT_TRUE(SameSize(root->dimensions(), 0, 0) &&
           SameSize(root->originalDimensions(), 0, 0));
  EXPECT_TRUE(SameSize(root->texCoordsScale(), 1, 1) && !root->isMipMapped() &&
           root->glTextureName() == 0);
  EXPECT_TRUE(!root->isRectangleTexture() && !root->isCubeMap() &&
           oo::IsNullPixMap(root->copyPixMapRepresentation()));
  EXPECT_TRUE(!root->descriptionComponents().has_value());
  root->apply();
  root->ensureFinishedLoading();
  root->forceRebind();
#ifndef NDEBUG
  EXPECT_TRUE(!root->name().has_value() && root->dataSize() == 0);
#endif
}

TEST(OOTexture, cxxTextureBehindTheFacade) {
  const int deallocs = gTestDeallocs;
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<TestCxxTexture> texture =
        TestCxxTexture::make(std::string("test:cxx"));
    oo::Texture* facade = texture.get();  // what the callers hold

    // The callers' calls reach the C++ overrides, and the root's defaults.
    EXPECT_TRUE(SameSize(facade->dimensions(), 16, 2) &&
             SameSize(facade->originalDimensions(), 16, 2));
    EXPECT_TRUE(facade->glTextureName() == 9 && !facade->isMipMapped() &&
             facade->isFinishedLoading());
    EXPECT_TRUE(facade->cacheKey() == std::optional<std::string>("test:cxx"));
    facade->apply();
    EXPECT_TRUE(texture->applies == 1);
#ifndef NDEBUG
    EXPECT_TRUE(facade->dataSize() == 32);
#endif
    const std::string text = facade->description();
    EXPECT_TRUE(text.starts_with("<TestCxxTexture 0x") &&
             text.ends_with(">{cxx}"));

    // The caches hold it by key; a graphics reset reaches it.
    EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cxx")) ==
             facade);
    oo::Texture::rebindAllTextures();
    EXPECT_TRUE(texture->rebinds == 1);
#ifndef NDEBUG
    {
      oo::AutoreleaseScope pool;
      EXPECT_TRUE(Contains(oo::Texture::allTextures(), facade));
    }
#endif
  }
  ClearCache();
  EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:cxx")) ==
           nullptr);  // destroyed: uncached
  EXPECT_TRUE(gTestDeallocs == deallocs);
}

/*	clearCache() hands the old recent-texture cache to
   oo::autorelease, as upstream's [sRecentTextures autorelease] handed it to the
   enclosing pool. In the game that is the frame loop's or the start-up's
   oo::AutoreleaseScope. The cache, and the textures only it keeps,
   are released at the drain of the NEAREST open scope: not at the call, not at
   an outer scope's drain, and nothing is counted as autoreleased without a
   scope.
*/
TEST(OOTexture, clearCacheReleasesAtTheNearestScopesDrain) {
  ClearCache();
  const int deallocs = gTestDeallocs;
  const std::size_t leaked = oo::AutoreleaseScope::leakedWithoutScope();
  {
    oo::AutoreleaseScope outer;  // stands for the frame loop's scope
    {
      oo::AutoreleaseScope made;
      MakeTexture(std::string("test:drain"));
    }
    EXPECT_TRUE(gTestDeallocs == deallocs);  // only the recent textures keep it
    const std::size_t outerPending = outer.pendingCount();
    {
      oo::AutoreleaseScope nearest;
      oo::Texture::clearCache();
      EXPECT_TRUE(nearest.pendingCount() ==
               1);  // the old cache, waiting for the drain
      EXPECT_TRUE(outer.pendingCount() == outerPending);
      EXPECT_TRUE(gTestDeallocs == deallocs);  // no earlier than the drain
      EXPECT_TRUE(oo::Texture::existingTextureForKey(std::string("test:drain")) ==
               nullptr);  // forgotten at once

      // With no recent textures left, a second clear hands nothing over.
      oo::Texture::clearCache();
      EXPECT_TRUE(nearest.pendingCount() == 1);
    }
    EXPECT_TRUE(gTestDeallocs ==
             deallocs + 1);  // released at the nearest scope's drain
    EXPECT_TRUE(outer.pendingCount() == outerPending);
  }
  EXPECT_TRUE(gTestDeallocs == deallocs + 1);
  EXPECT_TRUE(oo::AutoreleaseScope::leakedWithoutScope() == leaked);
}
