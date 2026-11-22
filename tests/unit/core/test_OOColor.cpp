/*	test_OOColor.mm
        Unit tests for OOColor (src/Core/OOColor.h).

        A class's test lives in tests/unit/core/test_<Class>.mm and
   links the game's own objects for the class and what they need
   (tests/unit/core/meson.build), so it tests exactly what the game runs. It
   pins what the class computes: component arithmetic, the four description
   forms, the text, and the selectors' answers asked of the C++ colour. Run:
   tests/run.sh unit
*/

#include <cmath>

#include <gtest/gtest.h>

#include "Color.h"

namespace {

bool Near(float a, float b) { return std::fabs(a - b) < 1e-5f; }

bool HasRGBA(oo::Color* c, float r, float g, float b, float a) {
  return c != nullptr && Near(c->redComponent(), r) &&
         Near(c->greenComponent(), g) && Near(c->blueComponent(), b) &&
         Near(c->alphaComponent(), a);
}

oo::PList Dict(
    std::initializer_list<std::pair<const char*, oo::PList>> entries) {
  oo::PList::Dict dict;
  for (const auto& [key, value] : entries) dict[key] = value;
  return oo::PList(std::move(dict));
}

}  // namespace

TEST(OOColor, factoriesAndComponents) {
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithRed(0.25f, 0.5f, 0.75f, 1.0f).get(), 0.25f,
                   0.5f, 0.75f, 1.0f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithWhite(0.5f, 0.25f).get(), 0.5f, 0.5f, 0.5f,
                   0.25f));
  EXPECT_TRUE(
      HasRGBA(oo::Color::colorWithRGBAComponents({0.1f, 0.2f, 0.3f, 0.4f}).get(),
              0.1f, 0.2f, 0.3f, 0.4f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithHue(0.0f, 1.0f, 1.0f, 1.0f).get(), 1.0f,
                   0.0f, 0.0f, 1.0f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithHue(0.5f, 1.0f, 1.0f, 0.5f).get(), 0.0f,
                   1.0f, 1.0f, 0.5f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithHue(0.25f, 0.0f, 0.4f, 1.0f).get(), 0.4f,
                   0.4f, 0.4f, 1.0f));  // grey: s == 0
  EXPECT_TRUE(HasRGBA(
      oo::Color::colorWithHSBAComponents({240.0f, 1.0f, 1.0f, 1.0f}).get(), 0.0f,
      0.0f, 1.0f, 1.0f));

  std::shared_ptr<oo::Color> c = oo::Color::colorWithRed(0.2f, 0.4f, 0.6f, 0.8f);
  float r, g, b, a;
  c->getRed(&r, &g, &b, &a);
  EXPECT_TRUE(Near(r, 0.2f) && Near(g, 0.4f) && Near(b, 0.6f) && Near(a, 0.8f));
  oo::RGBAComponents rgba = c->rgbaComponents();
  EXPECT_TRUE(rgba.r == r && rgba.g == g && rgba.b == b && rgba.a == a);
  std::vector<float> array = c->normalizedArray();
  EXPECT_TRUE(array == std::vector<float>({r, g, b, a}));
}

TEST(OOColor, namedColours) {
  EXPECT_TRUE(HasRGBA(oo::Color::blackColor().get(), 0, 0, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::darkGrayColor().get(), 1.0f / 3.0f, 1.0f / 3.0f,
                   1.0f / 3.0f, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::lightGrayColor().get(), 2.0f / 3.0f, 2.0f / 3.0f,
                   2.0f / 3.0f, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::whiteColor().get(), 1, 1, 1, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::grayColor().get(), 0.5f, 0.5f, 0.5f, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::redColor().get(), 1, 0, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::greenColor().get(), 0, 1, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::blueColor().get(), 0, 0, 1, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::cyanColor().get(), 0, 1, 1, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::yellowColor().get(), 1, 1, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::magentaColor().get(), 1, 0, 1, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::orangeColor().get(), 1, 0.5f, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::purpleColor().get(), 0.5f, 0, 0.5f, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::brownColor().get(), 0.6f, 0.4f, 0.2f, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::clearColor().get(), 0, 0, 0, 0));
  EXPECT_TRUE(oo::Color::blackColor()->isBlack());
  EXPECT_TRUE(oo::Color::clearColor()->isBlack());
  EXPECT_TRUE(oo::Color::whiteColor()->isWhite());
  EXPECT_TRUE(!oo::Color::colorWithWhite(1.0f, 0.5f)->isWhite());
}

TEST(OOColor, hueSaturationBrightness) {
  std::shared_ptr<oo::Color> c = oo::Color::colorWithRed(0.0f, 0.5f, 1.0f, 0.25f);
  float h, s, b, a;
  c->getHue(&h, &s, &b, &a);
  EXPECT_TRUE(Near(h, c->hueComponent()) && Near(s, c->saturationComponent()) &&
           Near(b, c->brightnessComponent()));
  EXPECT_TRUE(Near(a, 0.25f) && Near(s, 1.0f) && Near(b, 1.0f));
  EXPECT_TRUE(std::fabs(h - 210.0f) <
           0.05f);  // 0..360, with the 0.0001 delta bias
  oo::HSBAComponents hsba = c->hsbaComponents();
  EXPECT_TRUE(hsba.h == h && hsba.s == s && hsba.b == b && hsba.a == a);
  EXPECT_EQ(oo::Color::blackColor()->saturationComponent(), 0.0f);
}

TEST(OOColor, colorFromString) {
  EXPECT_TRUE(HasRGBA(oo::Color::colorFromString("1 0.5 0").get(), 1, 0.5f, 0, 1));
  EXPECT_TRUE(HasRGBA(oo::Color::colorFromString("0.1 0.2 0.3 0.4").get(), 0.1f,
                   0.2f, 0.3f, 0.4f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorFromString("255 0 51").get(), 1, 0, 0.2f,
                   1));  // 0..255, alpha rescaled
  EXPECT_TRUE(HasRGBA(oo::Color::colorFromString("255 0 51 102").get(), 1, 0, 0.2f,
                   0.4f));
  EXPECT_TRUE(oo::Color::colorFromString("0.1 0.2") == nullptr);
  EXPECT_TRUE(oo::Color::colorFromString("red") == nullptr);
}

TEST(OOColor, colorWithDescription) {
  EXPECT_TRUE(oo::Color::colorWithDescription(oo::PList()) == nullptr);
  EXPECT_TRUE(oo::Color::colorWithDescription(oo::PList(true)) == nullptr);

  // A string: a named colour, or components.
  EXPECT_TRUE(HasRGBA(
      oo::Color::colorWithDescription(oo::PList(std::string("magentaColor")))
          .get(),
      1, 0, 1, 1));
  EXPECT_TRUE(oo::Color::colorWithDescription(oo::PList(std::string("fooColor"))) ==
           nullptr);
  EXPECT_TRUE(oo::Color::colorWithDescription(oo::PList(std::string("Color"))) ==
           nullptr);
  EXPECT_TRUE(HasRGBA(
      oo::Color::colorWithDescription(oo::PList(std::string("0 0 1"))).get(), 0,
      0, 1, 1));

  // An array: its elements' descriptions joined with spaces.
  oo::PList::Array components = {oo::PList(std::string("0.5")),
                                 oo::PList(std::int64_t(1)), oo::PList(0.25)};
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithDescription(oo::PList(components)).get(),
                   0.5f, 1, 0.25f, 1));

  // A dictionary: HSB(A) when it has a hue, else RGB(A); alpha, or else
  // opacity.
  EXPECT_TRUE(
      HasRGBA(oo::Color::colorWithDescription(Dict({{"hue", oo::PList(120.0)},
                                                  {"value", oo::PList(0.5)},
                                                  {"opacity", oo::PList(0.5)}}))
                  .get(),
              0, 0.5f, 0, 0.5f));
  EXPECT_TRUE(
      HasRGBA(oo::Color::colorWithDescription(
                  Dict({{"red", oo::PList(0.5)}, {"blue", oo::PList(1.0)}}))
                  .get(),
              0.5f, 0, 1, 1));
  EXPECT_TRUE(HasRGBA(
      oo::Color::colorWithDescription(Dict({{"green", oo::PList(1.0)},
                                          {"alpha", oo::PList(0.5)},
                                          {"opacity", oo::PList(0.25)}}))
          .get(),
      0, 1, 0, 0.5f));

  // The saturation factor: applied to any description, and to a HSB
  // dictionary's own saturation.
  std::shared_ptr<oo::Color> desaturated =
      oo::Color::colorWithDescription(oo::PList(std::string("redColor")), 0.5f);
  EXPECT_TRUE(Near(desaturated->saturationComponent(), 0.5f) &&
           Near(desaturated->brightnessComponent(), 1.0f));
  std::shared_ptr<oo::Color> scaled = oo::Color::colorWithDescription(
      Dict({{"hue", oo::PList(0.0)}, {"saturation", oo::PList(0.8)}}), 0.5f);
  EXPECT_TRUE(Near(scaled->saturationComponent(), 0.4f));

  // Bright: at least 0.5 brightness, alpha forced to 1; bright enough is
  // returned as is.
  std::shared_ptr<oo::Color> bright = oo::Color::brightColorWithDescription(
      oo::PList(std::string("0 0 0.25 0.5")));
  EXPECT_TRUE(Near(bright->brightnessComponent(), 0.5f) &&
           Near(bright->alphaComponent(), 1.0f));
  EXPECT_TRUE(HasRGBA(oo::Color::brightColorWithDescription(
                       oo::PList(std::string("0 0.75 0 0.5")))
                       .get(),
                   0, 0.75f, 0, 0.5f));
  EXPECT_TRUE(oo::Color::brightColorWithDescription(oo::PList()) == nullptr);
}

TEST(OOColor, derivedColours) {
  std::shared_ptr<oo::Color> opaque = oo::Color::colorWithRed(0.5f, 0.5f, 0.5f, 1.0f);
  EXPECT_TRUE(opaque->premultipliedColor() == opaque);  // itself, as before
  EXPECT_TRUE(HasRGBA(
      oo::Color::colorWithRed(0.5f, 1.0f, 0.2f, 0.5f)->premultipliedColor().get(),
      0.25f, 0.5f, 0.1f, 1.0f));
  EXPECT_TRUE(HasRGBA(oo::Color::colorWithRed(0.5f, 0.8f, 0.1f, 0.3f)
                       ->colorWithBrightnessFactor(2.0f)
                       .get(),
                   1.0f, 1.0f, 0.2f, 0.3f));
  EXPECT_TRUE(
      HasRGBA(oo::Color::blackColor()
                  ->blendedColorWithFraction(0.25f, oo::Color::whiteColor().get())
                  .get(),
              0.25f, 0.25f, 0.25f, 1.0f));
}

TEST(OOColor, text) {
  std::shared_ptr<oo::Color> c = oo::Color::colorWithRed(1.0f, 0.5f, 0.0f, 1.0f);
  EXPECT_TRUE(c->descriptionComponents() ==
           std::optional<std::string>("1, 0.5, 0, 1"));
  EXPECT_TRUE(c->rgbaDescription() ==
           std::optional<std::string>("{1, 0.5, 0, 1}"));
  EXPECT_TRUE(oo::Color::redColor()->hsbaDescription() ==
           std::optional<std::string>("{0, 1, 1, 1}"));
  EXPECT_EQ(oo::RGBAComponentsDescription({0.12345f, 1, 0, 0.5f}),
              std::string("{0.123, 1, 0, 0.5}"));
  EXPECT_EQ(oo::HSBAComponentsDescription({359.9f, 1, 0.5f, 1}),
              std::string("{359, 1, 0.5, 1}"));
}

TEST(OOColor, facadeForwards) {
  {
    // What the selectors answer, asked of the C++ colour.
    std::shared_ptr<oo::Color> red = oo::Color::redColor();
    EXPECT_TRUE(red->redComponent() == 1.0f && red->greenComponent() == 0.0f &&
             red->alphaComponent() == 1.0f);
    EXPECT_TRUE(red->normalizedArray() == std::vector<float>({1, 0, 0, 1}));
    EXPECT_TRUE(red->rgbaDescription() ==
             std::optional<std::string>("{1, 0, 0, 1}"));
    EXPECT_TRUE(HasRGBA(
        oo::Color::colorWithDescription(oo::PList(std::string("0 1 0"))).get(), 0,
        1, 0, 1));
    EXPECT_TRUE(oo::Color::colorWithDescription(
                 oo::PList(std::string("nonsense"))) == nullptr);
    EXPECT_TRUE(oo::Color::colorWithDescription(oo::PList()) == nullptr);
    EXPECT_TRUE(HasRGBA(
        red->blendedColorWithFraction(0.5f, oo::Color::blueColor().get()).get(),
        0.5f, 0, 0.5f, 1));

    std::string text = red->description();  // what "%@" printed for the facade
    EXPECT_TRUE(text.starts_with("<OOColor 0x") &&
             text.ends_with(">{1, 0, 0, 1}"));
  }
}

TEST(OOColor, facadeIdentity) {
  {
    std::shared_ptr<oo::Color> opaque = oo::Color::colorWithRed(0.5f, 0.5f, 0.5f, 1.0f);
    EXPECT_TRUE(opaque->premultipliedColor() ==
             opaque);  // returned itself before the conversion

    // An Object node's colour comes back as the same object (the colour is the
    // node's foreign object).
    oo::PList node = oo::ColorObjectNode(opaque.get());
    EXPECT_TRUE(oo::Color::colorWithDescription(node) == opaque);
    EXPECT_TRUE(oo::Color::colorWithDescription(node).get() == opaque.get());
    EXPECT_TRUE(oo::ColorInObjectNode(node) == opaque.get());
    EXPECT_TRUE(node.getIf<oo::PList::Object>()->get()->className() == "OOColor");
    EXPECT_TRUE(oo::Color::redColor() !=
             oo::Color::redColor());  // distinct objects, as before
  }
}
