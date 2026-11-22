// The game's text grid (GuiDisplayGen) as a src/UI GridSurface, for the
// classic views. Holds the grid weakly: once the universe drops it, writes do
// nothing, as messages to a nil GUI did.

#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "UI/GridSurface.h"

class GuiDisplayGen;

namespace oo {

class GuiDisplayGenGridSurface final : public oo::ui::GridSurface {
 public:
  explicit GuiDisplayGenGridSurface(std::weak_ptr<GuiDisplayGen> gui);

  void SetTabStops(const std::array<int, kTabStopCount>& stops) override;
  void SetText(int row, const std::string& text) override;
  void SetFields(int row, const std::vector<std::string>& fields) override;
  void SetColor(int row, oo::ui::GridColor color) override;
  void AddLongText(const std::optional<std::string>& text, int row) override;

 private:
  std::weak_ptr<GuiDisplayGen> gui_;
};

}  // namespace oo
