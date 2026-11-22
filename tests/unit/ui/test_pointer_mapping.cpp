/*	tests/unit/ui/test_pointer_mapping.cpp
	Where a pointer position in the window falls in the RmlUi context, when
	the game draws into a target of another size stretched over the window
	(as on a high-density Mac, where a 1512x917-point window showed a
	1536x1536-pixel target and the hover landed rows away from the pointer).
	Run: meson test -C build/meson_dev --suite ui
*/

#include <gtest/gtest.h>

#include "UI/RmlUi/PointerMapping.h"

using oo::ui::ContextPoint;
using oo::ui::PointerToContext;

namespace {

void ExpectAt(ContextPoint at, int x, int y) {
  EXPECT_EQ(at.x, x);
  EXPECT_EQ(at.y, y);
}

}  // namespace

TEST(PointerMapping, the_corners_of_the_window_are_the_corners_of_the_context) {
  ExpectAt(PointerToContext(0.0f, 0.0f, 1512, 917, 1536, 1536), 0, 0);
  ExpectAt(PointerToContext(1512.0f, 917.0f, 1512, 917, 1536, 1536), 1536, 1536);
}

TEST(PointerMapping, a_stretched_target_maps_each_axis_in_proportion) {
  // Half way down a 917-point window is half way down a 1536-pixel context,
  // not 2 x 458.5 = 917 pixels as the display's density would say.
  ExpectAt(PointerToContext(756.0f, 458.5f, 1512, 917, 1536, 1536), 768, 768);
}

TEST(PointerMapping, a_window_drawn_at_double_density_doubles) {
  ExpectAt(PointerToContext(100.0f, 50.0f, 1512, 917, 3024, 1834), 200, 100);
}

TEST(PointerMapping, a_window_of_no_size_maps_to_the_origin) {
  ExpectAt(PointerToContext(10.0f, 10.0f, 0, 0, 1024, 768), 0, 0);
}
