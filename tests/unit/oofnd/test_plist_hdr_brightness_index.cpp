/*	test_plist_hdr_brightness_index.cpp
        Pins today's (upstream's) behaviour for an HDR max
   brightness that is not in descriptions.plist's hdr_maxBrightness_array.
   Decision: keep upstream behaviour (Jon may overrule).

        The two game sites are PlayerEntity.mm (the game-options screen's
   HDRMAXBRIGHTNESS row) and PlayerEntityControls.mm (the left/right arrow on
   that row). Both run only when OOLITE_WINDOWS and the game view's hdrOutput()
   is true, from a full PlayerEntity with a Universe, a GUI and key state; no
   core test has a stand-in for that, and one would be far beyond this test's
   size. So this test pins the behaviour at helper level instead: each site's
   index arithmetic is copied below verbatim (minus the log and the GUI calls)
   and run on the oofnd pieces it depends on, the int narrowing of oo::kNotFound
   and oo::PList::at<int> out of range.

        What it pins, for an unmatched brightness:
          * static_cast<int>(oo::kNotFound) is -1 and widening it back never equals
   oo::kNotFound, so the "not found -> index 0" fallback never runs (as in
   upstream's Objective-C);
          * options screen: at<int>(static_cast<std::size_t>(-1)) is out of
   range and answers the fallback 0 (upstream: -oo_intAtIndex: answered 0) - the
   row shows the 0-nit label;
          * arrow keys: -1 + 1 = 0 (the first entry, 400) and -1 - 1 = -2,
   wrapped to count - 1 (the last entry, 1000); both in range. meson test
   --suite oofnd-plist
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "oofnd/PListGet.hpp"
#include "oofnd/PListOldStyle.hpp"
#include "oofnd/objc/FoundationTypes.h"

namespace {

// descriptions.plist, hdr_maxBrightness_array, as shipped. Unquoted old-style
// tokens are strings.
oo::PList shippedBrightnesses() {
  auto parsed = oo::parseOldStylePList("( 400, 500, 600, 800, 1000 )");
  return parsed.has_value() ? *parsed : oo::PList();
}

// PlayerEntity.mm, the options screen row: the index lookup and the value read.
int optionsScreenBrightnessValue(const oo::PList& brightnesses,
                                 int currentMaxBrightness, bool* fallbackRan) {
  const std::string currentBrightness = std::to_string(currentMaxBrightness);
  int brightnessIdx = static_cast<int>(oo::kNotFound);
  for (std::size_t i = 0; i < brightnesses.count(); i++) {
    const oo::PList* element = brightnesses.at(i);
    if (element->isString() &&
        *element->getIf<std::string>() == currentBrightness) {
      brightnessIdx = static_cast<int>(i);
      break;
    }
  }

  *fallbackRan = false;
  if (static_cast<std::ptrdiff_t>(brightnessIdx) == oo::kNotFound) {
    *fallbackRan = true;
    brightnessIdx = 0;
  }

  return brightnesses.at<int>(static_cast<std::size_t>(brightnessIdx));
}

// PlayerEntityControls.mm, the arrow keys on that row: the new index and the
// value it selects.
int controlsBrightnessValue(const oo::PList& brightnesses,
                            int currentMaxBrightness, int direction,
                            bool* fallbackRan) {
  const std::string currentBrightness = std::to_string(currentMaxBrightness);
  int brightnessIdx = static_cast<int>(oo::kNotFound);
  for (std::size_t i = 0; i < brightnesses.count(); i++) {
    const oo::PList* element = brightnesses.at(i);
    if (element->isString() &&
        *element->getIf<std::string>() == currentBrightness) {
      brightnessIdx = static_cast<int>(i);
      break;
    }
  }

  *fallbackRan = false;
  if (static_cast<std::ptrdiff_t>(brightnessIdx) == oo::kNotFound) {
    *fallbackRan = true;
    brightnessIdx = 0;
  }

  brightnessIdx += direction;
  int count = static_cast<int>(brightnesses.count());
  if (brightnessIdx < 0) brightnessIdx = count - 1;
  if (brightnessIdx >= count) brightnessIdx = 0;

  return brightnesses.at<int>(static_cast<std::size_t>(brightnessIdx));
}

void notFoundNarrowsToMinusOneAndNeverComparesEqual() {
  const int narrowed = static_cast<int>(oo::kNotFound);
  EXPECT_EQ(narrowed, -1);
  EXPECT_TRUE(static_cast<std::ptrdiff_t>(narrowed) != oo::kNotFound);
  EXPECT_EQ(static_cast<std::size_t>(narrowed), SIZE_MAX);
}

void atIntOutOfRangeAnswersTheFallback() {
  const oo::PList brightnesses = shippedBrightnesses();
  EXPECT_TRUE(brightnesses.isArray());
  EXPECT_EQ(brightnesses.count(), std::size_t{5});
  EXPECT_TRUE(brightnesses.at(SIZE_MAX) == nullptr);
  EXPECT_EQ(brightnesses.at<int>(SIZE_MAX), 0);
  EXPECT_EQ(brightnesses.at<int>(brightnesses.count()), 0);
  EXPECT_EQ(brightnesses.at<int>(SIZE_MAX, 77), 77);
  EXPECT_EQ(brightnesses.at<int>(0), 400);
  EXPECT_EQ(brightnesses.at<int>(4), 1000);
}

void optionsScreenMatchedBrightnessShowsItsOwnValue() {
  const oo::PList brightnesses = shippedBrightnesses();
  bool fallbackRan = true;
  EXPECT_EQ(optionsScreenBrightnessValue(brightnesses, 600, &fallbackRan),
              600);
  EXPECT_TRUE(!fallbackRan);
}

void optionsScreenUnmatchedBrightnessReadsSizeMaxAndShowsZero() {
  const oo::PList brightnesses = shippedBrightnesses();
  bool fallbackRan = true;
  EXPECT_EQ(optionsScreenBrightnessValue(brightnesses, 750, &fallbackRan), 0);
  EXPECT_TRUE(
      !fallbackRan);  // upstream behaviour: the 400-nit fallback never runs
}

void optionsScreenMissingArrayShowsZero() {
  bool fallbackRan = true;
  EXPECT_EQ(optionsScreenBrightnessValue(oo::PList(), 750, &fallbackRan), 0);
  EXPECT_TRUE(!fallbackRan);
}

void controlsMatchedBrightnessStepsAndWraps() {
  const oo::PList brightnesses = shippedBrightnesses();
  bool fallbackRan = true;
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 600, 1, &fallbackRan), 800);
  EXPECT_TRUE(!fallbackRan);
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 600, -1, &fallbackRan),
              500);
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 1000, 1, &fallbackRan),
              400);
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 400, -1, &fallbackRan),
              1000);
}

void controlsUnmatchedBrightnessRightGoesToFirstLeftWrapsToLast() {
  const oo::PList brightnesses = shippedBrightnesses();
  bool fallbackRan = true;
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 750, 1, &fallbackRan), 400);
  EXPECT_TRUE(!fallbackRan);
  fallbackRan = true;
  EXPECT_EQ(controlsBrightnessValue(brightnesses, 750, -1, &fallbackRan),
              1000);
  EXPECT_TRUE(!fallbackRan);
}

}  // namespace

// One TEST per case function, in the order they were registered. The cases
// stay functions in the anonymous namespace (misc-use-anonymous-namespace).
TEST(plist_hdr_brightness_index, notFoundNarrowsToMinusOneAndNeverComparesEqual) { notFoundNarrowsToMinusOneAndNeverComparesEqual(); }
TEST(plist_hdr_brightness_index, atIntOutOfRangeAnswersTheFallback) { atIntOutOfRangeAnswersTheFallback(); }
TEST(plist_hdr_brightness_index, optionsScreenMatchedBrightnessShowsItsOwnValue) { optionsScreenMatchedBrightnessShowsItsOwnValue(); }
TEST(plist_hdr_brightness_index, optionsScreenUnmatchedBrightnessReadsSizeMaxAndShowsZero) { optionsScreenUnmatchedBrightnessReadsSizeMaxAndShowsZero(); }
TEST(plist_hdr_brightness_index, optionsScreenMissingArrayShowsZero) { optionsScreenMissingArrayShowsZero(); }
TEST(plist_hdr_brightness_index, controlsMatchedBrightnessStepsAndWraps) { controlsMatchedBrightnessStepsAndWraps(); }
TEST(plist_hdr_brightness_index, controlsUnmatchedBrightnessRightGoesToFirstLeftWrapsToLast) { controlsUnmatchedBrightnessRightGoesToFirstLeftWrapsToLast(); }
