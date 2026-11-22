#include "UI/RmlUi/RmlUiHost.h"

#include <RmlUi/Core.h>
#include <RmlUi_Renderer_GL2.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "UI/RmlUi/RmlDocumentHost.h"
#include "UI/RmlUi/RmlShipLibraryView.h"
#include "UI/ShipLibrary/GridShipLibraryView.h"
#include "oofnd/Log.hpp"

namespace oo::ui {
namespace {

constexpr const char* kContextName = "oolite";

// RmlUi's clock and log, through SDL and the game's log.
class RmlSystemInterface final : public Rml::SystemInterface {
 public:
  double GetElapsedTime() override {
    return static_cast<double>(SDL_GetTicksNS()) / 1.0e9;
  }

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch (type) {
      case Rml::Log::LT_ERROR:
      case Rml::Log::LT_ASSERT:
        OO_LOG_ERR("ui.rmlui.error", "{}", message);
        break;
      case Rml::Log::LT_WARNING:
        OO_LOG_WARN("ui.rmlui.warning", "{}", message);
        break;
      default:
        OO_LOG("ui.rmlui", "{}", message);
        break;
    }
    return true;
  }
};

int RmlMouseButton(Uint8 sdl_button) {
  switch (sdl_button) {
    case SDL_BUTTON_LEFT:
      return 0;
    case SDL_BUTTON_RIGHT:
      return 1;
    case SDL_BUTTON_MIDDLE:
      return 2;
    default:
      return 3;
  }
}

// Saves the game's GL state around RmlUi's frame, which sets its own viewport,
// projection, blending, stencil and client arrays.
class SavedGLState {
 public:
  SavedGLState() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
  }

  ~SavedGLState() {
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopClientAttrib();
    glPopAttrib();  // restores the matrix mode too
  }

  SavedGLState(const SavedGLState&) = delete;
  SavedGLState& operator=(const SavedGLState&) = delete;
};

class RmlUiHost final : public UiHost, public RmlDocumentHost {
 public:
  explicit RmlUiHost(RmlUiHostOptions options) : options_(std::move(options)) {}

  ~RmlUiHost() override {
    if (context_ != nullptr) Rml::RemoveContext(kContextName);
    if (initialised_) Rml::Shutdown();
  }

  RmlUiHost(const RmlUiHost&) = delete;
  RmlUiHost& operator=(const RmlUiHost&) = delete;

  bool Start() {
    if (options_.window == nullptr) return false;
    Rml::SetSystemInterface(&system_);
    Rml::SetRenderInterface(&renderer_);
    if (!Rml::Initialise()) return false;
    initialised_ = true;

    for (std::string_view face :
         {"LatoLatin-Regular.ttf", "LatoLatin-Bold.ttf"}) {
      const std::string path =
          (options_.resource_root / "UI" / "fonts" / face).string();
      if (!Rml::LoadFontFace(path)) {
        OO_LOG("ui.rmlui.error", "Could not load the font {}.", path);
        return false;
      }
    }

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(options_.window, &width, &height);
    context_ = Rml::CreateContext(kContextName, Rml::Vector2i(width, height));
    if (context_ == nullptr) return false;
    Resize(width, height);
    OO_LOG("ui.rmlui", "RmlUi {} started, {}x{} pixels, density {}.",
           Rml::GetVersion(), width, height, PixelDensity());
    return true;
  }

  // -- UiHost --

  bool HandleEvent(const SDL_Event& event) override {
    if (!IsCoveringGui()) return false;
    const float density = PixelDensity();
    switch (event.type) {
      case SDL_EVENT_MOUSE_MOTION:
        context_->ProcessMouseMove(
            static_cast<int>(std::lround(event.motion.x * density)),
            static_cast<int>(std::lround(event.motion.y * density)), 0);
        return false;  // the game still tracks the pointer
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        return !context_->ProcessMouseButtonDown(
            RmlMouseButton(event.button.button), 0);
      case SDL_EVENT_MOUSE_BUTTON_UP:
        return !context_->ProcessMouseButtonUp(
            RmlMouseButton(event.button.button), 0);
      case SDL_EVENT_MOUSE_WHEEL:
        // SDL: positive is away from the user; RmlUi: positive scrolls down.
        return !context_->ProcessMouseWheel(
            Rml::Vector2f(-event.wheel.x, -event.wheel.y), 0);
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        context_->ProcessMouseLeave();
        return false;
      default:
        return false;
    }
  }

  void Resize(int pixel_width, int pixel_height) override {
    if (context_ == nullptr) return;
    context_->SetDimensions(Rml::Vector2i(pixel_width, pixel_height));
    // One dp is one point of a 1024-pixel-wide window: the layout scales with
    // the window, as the classic grid does, and text stays sharp.
    context_->SetDensityIndependentPixelRatio(static_cast<float>(pixel_width) /
                                              1024.0f);
    renderer_.SetViewport(pixel_width, pixel_height);
  }

  bool IsCoveringGui() const override {
    return std::any_of(
        views_.begin(), views_.end(),
        [](const RmlDocumentView* view) { return view->IsShowing(); });
  }

  void Render() override {
    if (context_ == nullptr || !IsCoveringGui()) return;
    // RmlUi draws into whatever the game is drawing into this pass (the
    // window, or the game's own target when it renders off screen), so it
    // takes the size of the viewport the game set.
    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] > 0 && viewport[3] > 0 &&
        (viewport[2] != context_->GetDimensions().x ||
         viewport[3] != context_->GetDimensions().y)) {
      OO_LOG("ui.rmlui", "Drawing at {}x{} pixels.", viewport[2], viewport[3]);
      Resize(viewport[2], viewport[3]);
    }
    const SavedGLState saved;
    context_->Update();
    for (RmlDocumentView* view : std::vector<RmlDocumentView*>(views_)) {
      view->OnContextUpdated();
    }
    renderer_.BeginFrame();
    context_->Render();
    renderer_.EndFrame();
  }

  std::unique_ptr<ShipLibraryView> MakeShipLibraryView(
      ShipLibraryViewModel& view_model, GridSurface& classic) override {
    if (std::unique_ptr<RmlShipLibraryView> view =
            RmlShipLibraryView::Create(view_model, *this)) {
      return view;
    }
    return std::make_unique<GridShipLibraryView>(view_model, classic);
  }

  // -- RmlDocumentHost --

  Rml::Context& context() override { return *context_; }

  std::filesystem::path DocumentPath(
      std::string_view file_name) const override {
    return options_.resource_root / "UI" / file_name;
  }

  void Register(RmlDocumentView* view) override { views_.push_back(view); }

  void Unregister(RmlDocumentView* view) override { std::erase(views_, view); }

 private:
  float PixelDensity() const {
    const float density = SDL_GetWindowPixelDensity(options_.window);
    return density > 0.0f ? density : 1.0f;
  }

  RmlUiHostOptions options_;
  RmlSystemInterface system_;
  RenderInterface_GL2 renderer_;
  bool initialised_ = false;
  Rml::Context* context_ = nullptr;  // owned by RmlUi
  std::vector<RmlDocumentView*> views_;
};

}  // namespace

std::unique_ptr<UiHost> MakeRmlUiHost(const RmlUiHostOptions& options) {
  auto host = std::make_unique<RmlUiHost>(options);
  if (!host->Start()) {
    OO_LOG("ui.rmlui.error",
           "RmlUi could not start; the screens keep the classic look.");
    return nullptr;
  }
  return host;
}

}  // namespace oo::ui
