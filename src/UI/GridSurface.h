// The classic screens' text grid, as the views in src/UI see it: rows of
// tab-separated fields with a colour each. The game's GuiDisplayGen is one
// (GuiDisplayGenGridSurface); a test can supply its own.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace oo::ui {

enum class GridColor : std::uint8_t { kGreen, kYellow, kWhite };

class GridSurface {
 public:
  static constexpr std::size_t kTabStopCount = 3;

  virtual ~GridSurface() = default;

  virtual void SetTabStops(const std::array<int, kTabStopCount>& stops) = 0;
  virtual void SetText(int row, const std::string& text) = 0;
  // The row's fields, one per tab stop.
  virtual void SetFields(int row, const std::vector<std::string>& fields) = 0;
  virtual void SetColor(int row, GridColor color) = 0;
  // Left-aligned text wrapped from <row> down; absent text shows nothing.
  virtual void AddLongText(const std::optional<std::string>& text, int row) = 0;
};

}  // namespace oo::ui
