// Where a pointer position in the window falls in the RmlUi context. SDL gives
// pointer positions in window points; the context is sized to what the game
// draws into, which fills the window but need not have its pixel size (on a
// high-density display the game may draw into a smaller target that is then
// stretched over the window). So the position is mapped in proportion, not by
// the display's pixel density.

#pragma once

namespace oo::ui {

struct ContextPoint {
  int x = 0;
  int y = 0;
};

// <x>, <y>: the pointer in window points; <window_width>, <window_height>: the
// window in points; <context_width>, <context_height>: the context in pixels.
constexpr ContextPoint PointerToContext(float x, float y, int window_width,
                                        int window_height, int context_width,
                                        int context_height) {
  if (window_width <= 0 || window_height <= 0) return ContextPoint{};
  const float scale_x =
      static_cast<float>(context_width) / static_cast<float>(window_width);
  const float scale_y =
      static_cast<float>(context_height) / static_cast<float>(window_height);
  // Rounded to the nearest pixel (positions are never negative inside the
  // window; outside it RmlUi treats any position as off its elements).
  const float cx = x * scale_x;
  const float cy = y * scale_y;
  return ContextPoint{static_cast<int>(cx < 0.0f ? cx - 0.5f : cx + 0.5f),
                      static_cast<int>(cy < 0.0f ? cy - 0.5f : cy + 0.5f)};
}

}  // namespace oo::ui
