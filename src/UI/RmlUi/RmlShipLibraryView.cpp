#include "UI/RmlUi/RmlShipLibraryView.h"

#include <RmlUi/Core.h>

#include <cstddef>
#include <format>
#include <optional>

#include "UI/ShipLibrary/ShipLibraryModel.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

namespace oo::ui {
namespace {

constexpr const char* kModelName = "ship_library";
constexpr const char* kDocument = "ship_library.rml";

std::string OrEmpty(const std::optional<std::string>& text) {
  return text.value_or(std::string());
}

int IndexArgument(const Rml::VariantList& arguments) {
  return arguments.empty() ? -1 : arguments[0].Get<int>(-1);
}

}  // namespace

std::unique_ptr<RmlShipLibraryView> RmlShipLibraryView::Create(
    ShipLibraryViewModel& view_model, RmlDocumentHost& host) {
  std::unique_ptr<RmlShipLibraryView> view(
      new RmlShipLibraryView(view_model, host));
  if (!view->Load()) return nullptr;
  return view;
}

RmlShipLibraryView::RmlShipLibraryView(ShipLibraryViewModel& view_model,
                                       RmlDocumentHost& host)
    : view_model_(view_model), host_(host) {}

RmlShipLibraryView::~RmlShipLibraryView() {
  view_model_.Detach(this);
  if (registered_) host_.Unregister(this);
  Rml::Context& context = host_.context();
  if (document_ != nullptr) context.UnloadDocument(document_);
  // Detaches the unloaded document's elements from the bindings at once, so
  // nothing reads bindings_ after this view is gone.
  if (model_created_) context.RemoveDataModel(kModelName);
}

bool RmlShipLibraryView::Load() {
  Rml::Context& context = host_.context();
  Rml::DataModelConstructor constructor = context.CreateDataModel(kModelName);
  if (!constructor) {
    Rml::Log::Message(Rml::Log::LT_ERROR, "Could not create the %s data model.",
                      kModelName);
    return false;
  }
  model_created_ = true;

  if (Rml::StructHandle<ShipRow> row = constructor.RegisterStruct<ShipRow>()) {
    row.RegisterMember("id", &ShipRow::id);
    row.RegisterMember("name", &ShipRow::name);
  }
  constructor.RegisterArray<std::vector<ShipRow>>();
  constructor.RegisterArray<std::vector<std::string>>();

  constructor.Bind("previous_category", &bindings_.previous_category);
  constructor.Bind("category", &bindings_.category);
  constructor.Bind("next_category", &bindings_.next_category);
  constructor.Bind("ships", &bindings_.ships);
  constructor.Bind("selected", &bindings_.selected);
  constructor.Bind("shown", &bindings_.shown);
  constructor.Bind("category_singular", &bindings_.category_singular);
  constructor.Bind("class_name", &bindings_.class_name);
  constructor.Bind("summary", &bindings_.summary);
  constructor.Bind("has_stats", &bindings_.has_stats);
  constructor.Bind("stats", &bindings_.stats);
  constructor.Bind("has_description", &bindings_.has_description);
  constructor.Bind("description", &bindings_.description);

  // Commands: the document's controls call the view-model, nothing else.
  constructor.BindEventCallback(
      "select_ship", [this](Rml::DataModelHandle, Rml::Event&,
                            const Rml::VariantList& arguments) {
        const int index = IndexArgument(arguments);
        if (index >= 0) view_model_.SelectShip(static_cast<std::size_t>(index));
      });
  constructor.BindEventCallback(
      "previous_category",
      [this](Rml::DataModelHandle, Rml::Event&, const Rml::VariantList&) {
        view_model_.PreviousCategory();
      });
  constructor.BindEventCallback(
      "next_category",
      [this](Rml::DataModelHandle, Rml::Event&, const Rml::VariantList&) {
        view_model_.NextCategory();
      });
  constructor.BindEventCallback(
      "exit", [this](Rml::DataModelHandle, Rml::Event&,
                     const Rml::VariantList&) { view_model_.Exit(); });

  const std::string path = host_.DocumentPath(kDocument).string();
  document_ = context.LoadDocument(path);
  if (document_ == nullptr) {
    // Through RmlUi's log, which the host sends to the game's.
    Rml::Log::Message(Rml::Log::LT_ERROR,
                      "Could not load %s; the ship library falls back to the "
                      "classic screen.",
                      path.c_str());
    return false;
  }
  document_->Show(Rml::ModalFlag::None, Rml::FocusFlag::None);
  host_.Register(this);
  registered_ = true;
  view_model_.Attach(this);
  Refresh();
  return true;
}

void RmlShipLibraryView::Refresh() {
  const ShipLibraryModel& model = view_model_.model();
  bindings_.previous_category = view_model_.PreviousCategoryName();
  bindings_.category = view_model_.CurrentCategoryName();
  bindings_.next_category = view_model_.NextCategoryName();

  const std::vector<ShipLibraryEntry>& ships = view_model_.ships();
  bindings_.ships.clear();
  bindings_.ships.reserve(ships.size());
  for (std::size_t i = 0; i < ships.size(); ++i) {
    bindings_.ships.push_back(
        {std::format("ship-{}", i), OrEmpty(ships[i].name)});
  }
  const int selected = view_model_.selected_category() == model.category
                           ? static_cast<int>(view_model_.selected_ship())
                           : -1;
  scroll_pending_ = scroll_pending_ || selected != bindings_.selected;
  bindings_.selected = selected;
  bindings_.shown = static_cast<int>(model.ship);

  const ShipSheet& sheet = model.sheet;
  bindings_.category_singular = sheet.category_singular;
  bindings_.class_name = OrEmpty(sheet.class_name);
  bindings_.summary = OrEmpty(sheet.summary);
  bindings_.has_stats = sheet.has_stats;
  bindings_.stats = {sheet.speed,     sheet.turn_rate, sheet.cargo,
                     sheet.generator, sheet.shields,   sheet.witchspace,
                     sheet.weapons,   sheet.turrets,   sheet.size};
  bindings_.has_description =
      sheet.has_description && sheet.description.has_value();
  bindings_.description = OrEmpty(sheet.description);

  if (Rml::DataModelHandle handle =
          host_.context().GetDataModel(kModelName).GetModelHandle()) {
    handle.DirtyAllVariables();
  }
}

ShipStage RmlShipLibraryView::Stage() const {
  return ShipStage{.distance_scale = 0.8, .x_offset = 0.19, .y_offset = 0.08};
}

bool RmlShipLibraryView::IsShowing() const {
  return document_ != nullptr && document_->IsVisible();
}

void RmlShipLibraryView::OnContextUpdated() {
  if (!scroll_pending_ || document_ == nullptr) return;
  scroll_pending_ = false;
  if (bindings_.selected < 0) return;
  Rml::Element* row =
      document_->GetElementById(std::format("ship-{}", bindings_.selected));
  // Scrolls only when the row is out of view, and then to the list's middle.
  if (row != nullptr) {
    row->ScrollIntoView(Rml::ScrollIntoViewOptions(
        Rml::ScrollAlignment::Adaptive, Rml::ScrollAlignment::Nearest,
        Rml::ScrollBehavior::Smooth, Rml::ScrollParentage::Closest));
  }
}

}  // namespace oo::ui
