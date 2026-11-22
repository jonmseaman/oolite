/*	test_OOPolygonSprite.mm
        Unit tests for OOPolygonSprite (src/Core/OOPolygonSprite.h).

        It links the whole game but main (tests/unit/core/meson.build entry
   ['*']: the sprite reaches the graphics reset manager, the extension manager
   and, in debug builds, the resource manager). There is no GL context, so
   nothing is drawn: the sprite's tesselation (GLU) runs at creation and is
   observable as success or nil. The expectations cover: which data arrays make
   a sprite (flat pairs, arrays of contours, clockwise, duplicate and non-finite
   vertices) and which answer nil (no contours), the description (the debug
   name), and what the class answers to (the graphics reset client's
   -resetGraphicsState, and the HUD's beacon icon category; the C++ interfaces
   OOGraphicsResetClient and OOHUDBeaconIcon). Run: bash
   tests/run.sh unit
*/

#include <limits>

#include <gtest/gtest.h>

#include "GraphicsResetManager.h"
#include "PolygonSprite.h"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

oo::PList Numbers(std::initializer_list<double> values) {
  oo::PList::Array array;
  for (double v : values) array.push_back(oo::PList(v));
  return oo::PList(std::move(array));
}

oo::PList Contours(std::initializer_list<oo::PList> contours) {
  return oo::PList(oo::PList::Array(contours));
}

std::shared_ptr<oo::PolygonSprite> Sprite(const oo::PList& data, const char* name) {
  return oo::PolygonSprite::initWithDataArray(data, 0.5f, name);
}

}  // namespace

TEST(OOPolygonSprite, dataArraysThatMakeASprite) {
  {
    const oo::PList square = Numbers({0, 0, 1, 0, 1, 1, 0, 1});
    EXPECT_TRUE(Sprite(square, "flat") != nullptr);  // one contour as flat pairs
    EXPECT_TRUE(Sprite(Contours({square}), "contours") !=
             nullptr);  // array of contours
    EXPECT_TRUE(Sprite(Numbers({0, 0, 0, 1, 1, 1, 1, 0}), "clockwise") !=
             nullptr);  // flipped
    EXPECT_TRUE(Sprite(Numbers({0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0}),
                    "duplicates") != nullptr);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(Sprite(Numbers({0, 0, nan, 1, 1, 0, inf, 3, 1, 1, 0, 1}),
                    "non-finite") != nullptr);
    EXPECT_TRUE(Sprite(Contours({square,
                              Numbers({0.25, 0.25, 0.75, 0.25, 0.75, 0.75})}),
                    "two") != nullptr);
    EXPECT_TRUE(Sprite(Numbers({0, 0, 1, 0}), "degenerate") !=
             nullptr);  // no area: nothing to fill
  }
}

TEST(OOPolygonSprite, emptyDataAnswersNil) {
  {
    EXPECT_TRUE(Sprite(oo::PList(oo::PList::Array()), "empty") == nullptr);
    EXPECT_TRUE(oo::PolygonSprite::initWithDataArray(oo::PList(), 1.0f, "null") ==
             nullptr);
  }
}

TEST(OOPolygonSprite, descriptionAndProtocols) {
  {
    std::shared_ptr<oo::PolygonSprite> sprite =
        Sprite(Numbers({0, 0, 1, 0, 1, 1}), "triangle");
    std::string text = sprite->description();
    EXPECT_TRUE(text.starts_with("<OOPolygonSprite 0x"));
#ifndef NDEBUG
    EXPECT_TRUE(text.ends_with(">{triangle}"));
#endif
    // The protocols are C++ interfaces; drawFilled/drawOutline are members.
    EXPECT_TRUE(dynamic_cast<oo::HUDBeaconIcon*>(sprite.get()) != nullptr);
    oo::GraphicsResetClient* client =
        dynamic_cast<oo::GraphicsResetClient*>(sprite.get());
    EXPECT_TRUE(client != nullptr);
    client->resetGraphicsState();  // no buffers made yet: nothing to delete
  }
}

TEST(OOPolygonSprite, cxxSprite) {
  std::shared_ptr<oo::PolygonSprite> sprite = oo::PolygonSprite::initWithDataArray(
      Numbers({0, 0, 1, 0, 1, 1, 0, 1}), 0.5f, "square");
  EXPECT_TRUE(sprite.get() != nullptr);
#ifndef NDEBUG
  EXPECT_TRUE(sprite->descriptionComponents() ==
           std::optional<std::string>("square"));
#endif
  sprite->resetGraphicsState();
  EXPECT_TRUE(oo::PolygonSprite::initWithDataArray(oo::PList(oo::PList::Array()),
                                              0.5f, "empty")
               .get() == nullptr);
}
