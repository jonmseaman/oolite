/*	test_OODrawable.mm
        Unit tests for OODrawable (src/Core/OODrawable.h).

        OODrawable is the root of the drawables (OOMesh, OOPlanetDrawable and
   OOSkyDrawable). It computes nothing but defaults, so this pins those, the
   debug size (totalSize() is the object's own size, which a subclass extends
   through the root's totalSize()), and a subclass's overrides and description.
   The test subclass is a C++ one, its instance size the object's. Run:
   tests/run.sh unit
*/

#include <cstring>

#include <gtest/gtest.h>

#include "Drawable.h"

// A subclass that overrides the root's members.
class TestSubDrawable : public oo::Drawable {
 public:
  void renderOpaqueParts() override { opaqueRenders++; }
  bool hasOpaqueParts() override { return true; }
  GLfloat collisionRadius() override { return radius; }
#ifndef NDEBUG
  size_t totalSize() override { return oo::Drawable::totalSize() + 100; }
  size_t objectSize() const override { return sizeof *this; }
#endif

  int opaqueRenders = 0;
  GLfloat radius = 0.0f;
};

// A converted drawable: a C++ subclass. Global, as a game class is, so that its
// description names it as the game's would.
class TestCxxDrawable : public oo::Drawable {
 public:
  void renderTranslucentParts() override { translucentRenders++; }
  bool hasTranslucentParts() override { return true; }
  GLfloat maxDrawDistance() override { return 1000.0f; }
  std::optional<std::string> descriptionComponents() const override {
    return "test";
  }

  int translucentRenders = 0;
};

namespace {

bool IsZero(const BoundingBox& box) {
  return std::memcmp(&box, &kZeroBoundingBox, sizeof box) == 0;
}

}  // namespace

TEST(OODrawable, rootDefaults) {
  {
    const std::shared_ptr<oo::Drawable> drawable = oo::makeShared<oo::Drawable>();
    drawable->renderOpaqueParts();
    drawable->renderTranslucentParts();
    EXPECT_TRUE(!drawable->hasOpaqueParts() && !drawable->hasTranslucentParts());
    EXPECT_TRUE(drawable->collisionRadius() == 0.0f &&
             drawable->maxDrawDistance() == 0.0f);
    EXPECT_TRUE(IsZero(drawable->boundingBox()));
    drawable->setBindingTarget(nullptr);
    drawable->dumpSelfState();
#ifndef NDEBUG
    EXPECT_TRUE(drawable->allTextures().empty());
    EXPECT_TRUE(drawable->totalSize() == drawable->objectSize());
    EXPECT_TRUE(drawable->objectSize() == sizeof(oo::Drawable));
#endif
  }
}

TEST(OODrawable, objCSubclass) {
  {
    const std::shared_ptr<TestSubDrawable> drawable = oo::makeShared<TestSubDrawable>();
    drawable->radius = 12.5f;
    drawable->renderOpaqueParts();
    EXPECT_TRUE(drawable->opaqueRenders == 1);
    EXPECT_TRUE(drawable->hasOpaqueParts() && !drawable->hasTranslucentParts());
    EXPECT_TRUE(drawable->collisionRadius() == 12.5f &&
             drawable->maxDrawDistance() == 0.0f);
#ifndef NDEBUG
    // The root's totalSize() is the subclass's size, not the root's.
    EXPECT_TRUE(drawable->totalSize() == sizeof(TestSubDrawable) + 100);
#endif
    // Virtual calls through the root's type reach the overrides, or the root's
    // own answers.
    oo::Drawable* part = drawable.get();
    part->renderOpaqueParts();
    EXPECT_TRUE(drawable->opaqueRenders == 2);
    EXPECT_TRUE(!part->descriptionComponents().has_value());
  }
}

TEST(OODrawable, cxxDrawableBehindTheFacade) {
  {
    const std::shared_ptr<TestCxxDrawable> drawable = oo::makeShared<TestCxxDrawable>();
    oo::Drawable* part = drawable.get();

    // The entities' calls reach the C++ overrides, and the root's defaults.
    part->renderTranslucentParts();
    EXPECT_TRUE(drawable->translucentRenders == 1);
    EXPECT_TRUE(part->hasTranslucentParts() && !part->hasOpaqueParts());
    EXPECT_TRUE(part->maxDrawDistance() == 1000.0f &&
             part->collisionRadius() == 0.0f);
    EXPECT_TRUE(IsZero(part->boundingBox()));

    const std::string text = part->description();
    EXPECT_TRUE(text.starts_with("<TestCxxDrawable 0x"));
    EXPECT_TRUE(text.ends_with(">{test}"));
  }
}

TEST(OODrawable, autoreleaseKeepsTheDrawable) {
  // OODrawableAutorelease() (an entity's replaced drawable, as [drawable
  // autorelease]): the drawable lives until the pool drains.
  std::shared_ptr<TestCxxDrawable> drawable = oo::makeShared<TestCxxDrawable>();
  TestCxxDrawable* raw = drawable.get();
  {
    oo::DrawableAutorelease(std::shared_ptr<oo::Drawable>(std::move(drawable)));
    raw->renderTranslucentParts();  // still alive
    EXPECT_TRUE(raw->translucentRenders == 1);
  }
  oo::DrawableAutorelease(std::shared_ptr<oo::Drawable>());  // null: nothing
}
