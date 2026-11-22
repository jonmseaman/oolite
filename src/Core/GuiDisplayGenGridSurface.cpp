#include "GuiDisplayGenGridSurface.h"

#include <cstddef>
#include <utility>

#include "Color.h"
#include "GuiDisplayGen.h"

namespace oo {
namespace {

std::shared_ptr<Color> ColorFor(oo::ui::GridColor color) {
  switch (color) {
    case oo::ui::GridColor::kGreen:
      return Color::greenColor();
    case oo::ui::GridColor::kYellow:
      return Color::yellowColor();
    case oo::ui::GridColor::kWhite:
      return Color::whiteColor();
  }
  return Color::whiteColor();
}

}  // namespace

GuiDisplayGenGridSurface::GuiDisplayGenGridSurface(
    std::weak_ptr<GuiDisplayGen> gui)
    : gui_(std::move(gui)) {}

void GuiDisplayGenGridSurface::SetTabStops(
    const std::array<int, kTabStopCount>& stops) {
  const std::shared_ptr<GuiDisplayGen> gui = gui_.lock();
  if (gui == nullptr) return;
  GUITabSettings tab_stops = {};
  for (std::size_t i = 0; i < stops.size(); ++i) tab_stops[i] = stops[i];
  gui->setTabStops(tab_stops);
}

void GuiDisplayGenGridSurface::SetText(int row, const std::string& text) {
  if (const std::shared_ptr<GuiDisplayGen> gui = gui_.lock()) {
    gui->setText(text, static_cast<GUIRow>(row));
  }
}

void GuiDisplayGenGridSurface::SetFields(
    int row, const std::vector<std::string>& fields) {
  if (const std::shared_ptr<GuiDisplayGen> gui = gui_.lock()) {
    gui->setArray(fields, static_cast<GUIRow>(row));
  }
}

void GuiDisplayGenGridSurface::SetColor(int row, oo::ui::GridColor color) {
  if (const std::shared_ptr<GuiDisplayGen> gui = gui_.lock()) {
    gui->setColor(ColorFor(color).get(), static_cast<GUIRow>(row));
  }
}

void GuiDisplayGenGridSurface::AddLongText(
    const std::optional<std::string>& text, int row) {
  if (const std::shared_ptr<GuiDisplayGen> gui = gui_.lock()) {
    gui->addLongText(text, static_cast<GUIRow>(row), GUIAlignment::kLeft);
  }
}

}  // namespace oo
