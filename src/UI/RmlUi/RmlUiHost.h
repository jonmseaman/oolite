// The RmlUi look: a UiHost whose screens are RmlUi documents (Resources/UI)
// bound to their view-models. The only header outside src/UI/RmlUi that names
// it; it includes no RmlUi header.

#pragma once

#include <filesystem>
#include <memory>

#include "UI/UiHost.h"

struct SDL_Window;

namespace oo::ui {

struct RmlUiHostOptions {
  // The game's built-in Resources directory; documents, styles and fonts are
  // read from its UI folder only, never from an expansion.
  std::filesystem::path resource_root;
  // The game's window, whose GL context is current.
  SDL_Window* window = nullptr;
};

// The RmlUi host, or null when RmlUi cannot start (a font or the window is
// missing); the game then keeps the classic look. One at a time.
std::unique_ptr<UiHost> MakeRmlUiHost(const RmlUiHostOptions& options);

}  // namespace oo::ui
