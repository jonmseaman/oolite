// The RmlUi ship library: Resources/UI/ship_library.rml bound to the
// view-model through the "ship_library" data model. The document shows what
// the bindings hold; clicks call the view-model's commands.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "UI/RmlUi/RmlDocumentHost.h"
#include "UI/ShipLibrary/ShipLibraryView.h"

namespace Rml {
class ElementDocument;
}  // namespace Rml

namespace oo::ui {

class ShipLibraryViewModel;

class RmlShipLibraryView final : public ShipLibraryView,
                                 public RmlDocumentView {
 public:
  // The view, or null when its document did not load (the log says why); the
  // caller then shows the classic grid.
  static std::unique_ptr<RmlShipLibraryView> Create(
      ShipLibraryViewModel& view_model, RmlDocumentHost& host);
  ~RmlShipLibraryView() override;
  RmlShipLibraryView(const RmlShipLibraryView&) = delete;
  RmlShipLibraryView& operator=(const RmlShipLibraryView&) = delete;

  void Refresh() override;
  // Closer than the classic screen, and right of the list, above the sheet.
  ShipStage Stage() const override;

  bool IsShowing() const override;
  void OnContextUpdated() override;

  // What the document binds to (public for the data model's registration).
  struct ShipRow {
    std::string id;  // "ship-<index>", for scrolling it into view
    std::string name;
  };

  struct Bindings {
    std::string previous_category;
    std::string category;
    std::string next_category;
    std::vector<ShipRow> ships;
    int selected = -1;  // the ship asked for last
    int shown = -1;     // the ship on show
    std::string category_singular;
    std::string class_name;
    std::string summary;
    bool has_stats = false;
    std::vector<std::string> stats;  // speed, turn rate, cargo, ... size
    bool has_description = false;
    std::string description;
  };

 private:
  RmlShipLibraryView(ShipLibraryViewModel& view_model, RmlDocumentHost& host);
  bool Load();

  ShipLibraryViewModel& view_model_;
  RmlDocumentHost& host_;
  Rml::ElementDocument* document_ = nullptr;  // owned by the context
  bool model_created_ = false;
  bool registered_ = false;
  bool scroll_pending_ = false;
  Bindings bindings_;
};

}  // namespace oo::ui
