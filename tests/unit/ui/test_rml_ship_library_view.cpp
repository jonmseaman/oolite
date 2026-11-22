/*	tests/unit/ui/test_rml_ship_library_view.cpp
	The RmlUi ship library without a window: RmlUi's own layout and input
	against Resources/UI/ship_library.rml and its fonts, with a renderer that
	draws nothing. Checks what the document shows, that a click, the wheel
	and the controls reach the view-model, and that game text stays text.
	Run: meson test -C build/meson_dev --suite ui
*/

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <RmlUi/Core.h>
#include <gtest/gtest.h>

#include "UI/RmlUi/RmlDocumentHost.h"
#include "UI/RmlUi/RmlShipLibraryView.h"
#include "UI/ShipLibrary/ShipLibraryModel.h"
#include "UI/ShipLibrary/ShipLibraryNavigator.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

#ifndef OO_TEST_UI_RESOURCES
#error OO_TEST_UI_RESOURCES (Resources/UI) is defined by tests/unit/ui/meson.build
#endif

using oo::ui::RmlShipLibraryView;
using oo::ui::ShipLibraryCategory;
using oo::ui::ShipLibraryViewModel;
using oo::ui::ShipSheet;

namespace {

class NullRenderer : public Rml::RenderInterface {
 public:
  Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex>,
                                              Rml::Span<const int>) override {
    return 1;
  }
  void RenderGeometry(Rml::CompiledGeometryHandle, Rml::Vector2f, Rml::TextureHandle) override {}
  void ReleaseGeometry(Rml::CompiledGeometryHandle) override {}
  Rml::TextureHandle LoadTexture(Rml::Vector2i& dimensions, const Rml::String&) override {
    dimensions = Rml::Vector2i(1, 1);
    return 1;
  }
  Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte>, Rml::Vector2i) override {
    return 1;
  }
  void ReleaseTexture(Rml::TextureHandle) override {}
  void EnableScissorRegion(bool) override {}
  void SetScissorRegion(Rml::Rectanglei) override {}
};

// One RmlUi for the program, as the game has one.
class RmlEnvironment : public ::testing::Environment {
 public:
  void SetUp() override {
    Rml::SetRenderInterface(&renderer_);
    ASSERT_TRUE(Rml::Initialise());
    const std::filesystem::path fonts = std::filesystem::path(OO_TEST_UI_RESOURCES) / "fonts";
    ASSERT_TRUE(Rml::LoadFontFace((fonts / "LatoLatin-Regular.ttf").string()));
    ASSERT_TRUE(Rml::LoadFontFace((fonts / "LatoLatin-Bold.ttf").string()));
  }
  void TearDown() override { Rml::Shutdown(); }

 private:
  NullRenderer renderer_;
};

const ::testing::Environment* const kRml =
    ::testing::AddGlobalTestEnvironment(new RmlEnvironment);

class Host : public oo::ui::RmlDocumentHost {
 public:
  Host() { context_ = Rml::CreateContext("test", Rml::Vector2i(1024, 768)); }
  ~Host() override { Rml::RemoveContext("test"); }
  Rml::Context& context() override { return *context_; }
  std::filesystem::path DocumentPath(std::string_view file_name) const override {
    return std::filesystem::path(OO_TEST_UI_RESOURCES) / file_name;
  }
  void Register(oo::ui::RmlDocumentView* view) override { views.push_back(view); }
  void Unregister(oo::ui::RmlDocumentView* view) override { std::erase(views, view); }

  std::vector<oo::ui::RmlDocumentView*> views;

 private:
  Rml::Context* context_ = nullptr;
};

struct Shown {
  std::size_t category;
  std::size_t ship;
  bool operator==(const Shown&) const = default;
};

class FakeNavigator : public oo::ui::ShipLibraryNavigator {
 public:
  void ShowShip(std::size_t category, std::size_t index) override {
    asked.push_back({category, index});
  }
  void Exit() override { ++exits; }
  std::vector<Shown> asked;
  int exits = 0;
};

ShipLibraryCategory Category(const std::string& id, std::size_t ships) {
  ShipLibraryCategory category;
  category.id = id;
  category.plural_name = id + "s";
  for (std::size_t i = 0; i < ships; ++i) {
    category.entries.push_back({id + std::to_string(i), id + " " + std::to_string(i)});
  }
  return category;
}

std::string TextOf(Rml::Element* element) {
  return element != nullptr ? element->GetInnerRML() : std::string("(missing)");
}

void Click(Rml::Context& context, Rml::Element* element) {
  ASSERT_NE(element, nullptr);
  const Rml::Vector2f at =
      element->GetAbsoluteOffset(Rml::BoxArea::Border) + element->GetBox().GetSize() * 0.5f;
  context.ProcessMouseMove(static_cast<int>(at.x), static_cast<int>(at.y), 0);
  context.ProcessMouseButtonDown(0, 0);
  context.ProcessMouseButtonUp(0, 0);
  context.Update();
}

// A library of 30 ships and 3 categories, showing ship <ship>, in a document.
struct Screen {
  explicit Screen(std::size_t ship = 2) {
    std::vector<ShipLibraryCategory> categories = {Category("ship", 30), Category("station", 2),
                                                   Category("thing", 1)};
    categories[0].entries[4].name = "<b>Bold</b> & <span>markup</span>";
    view_model.SetCategories(categories, 0, ship);
    ShipSheet sheet;
    sheet.category_singular = "Ship";
    sheet.class_name = "Cobra Mark III";
    sheet.summary = "Heavy multi-role";
    sheet.has_stats = true;
    sheet.speed = "Speed: Average";
    sheet.has_description = true;
    sheet.description = "The first truly successful multi-role design.";
    view_model.OnShipShown(0, ship, sheet);
    view = RmlShipLibraryView::Create(view_model, host);
    host.context().Update();
  }
  Rml::Element* Get(const std::string& id) {
    return document() != nullptr ? document()->GetElementById(id) : nullptr;
  }
  Rml::ElementDocument* document() { return host.context().GetDocument(0); }

  Host host;
  FakeNavigator navigator;
  ShipLibraryViewModel view_model{navigator};
  std::unique_ptr<RmlShipLibraryView> view;
};

}  // namespace

TEST(RmlShipLibraryView, loads_and_shows_the_view_models_state) {
  Screen screen;
  ASSERT_NE(screen.view, nullptr);
  EXPECT_TRUE(screen.view->IsShowing());
  EXPECT_EQ(screen.host.views.size(), 1u);
  EXPECT_EQ(TextOf(screen.Get("ship-name")), "Cobra Mark III");
  EXPECT_EQ(TextOf(screen.Get("category")), "ships");
  EXPECT_EQ(TextOf(screen.Get("ship-0")), "ship 0");
  EXPECT_EQ(TextOf(screen.Get("ship-29")), "ship 29");
  EXPECT_TRUE(screen.Get("ship-2")->IsClassSet("selected"));
  EXPECT_TRUE(screen.Get("ship-2")->IsClassSet("shown"));
  EXPECT_FALSE(screen.Get("ship-3")->IsClassSet("selected"));
}

TEST(RmlShipLibraryView, clicking_a_ship_asks_the_view_model_for_it) {
  Screen screen;
  Click(screen.host.context(), screen.Get("ship-5"));
  EXPECT_EQ(screen.navigator.asked, (std::vector<Shown>{{0, 5}}));
  EXPECT_TRUE(screen.Get("ship-5")->IsClassSet("selected"));  // at once
  EXPECT_TRUE(screen.Get("ship-2")->IsClassSet("shown"));     // until it flies in
}

TEST(RmlShipLibraryView, the_category_arrows_and_back_are_commands) {
  Screen screen;
  Rml::Element* categories = screen.Get("categories");
  ASSERT_NE(categories, nullptr);
  Click(screen.host.context(), categories->GetChild(2));  // next
  EXPECT_EQ(screen.navigator.asked.back(), (Shown{1, 0}));
  Click(screen.host.context(), screen.Get("back"));
  EXPECT_EQ(screen.navigator.exits, 1);
}

TEST(RmlShipLibraryView, the_wheel_scrolls_a_long_list) {
  Screen screen;
  Rml::Element* list = screen.Get("ship-scroll");
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(list->GetScrollTop(), 0.0f);
  Rml::Context& context = screen.host.context();
  Rml::Element* row = screen.Get("ship-1");
  const Rml::Vector2f at = row->GetAbsoluteOffset(Rml::BoxArea::Border);
  context.ProcessMouseMove(static_cast<int>(at.x) + 4, static_cast<int>(at.y) + 4, 0);
  context.ProcessMouseWheel(Rml::Vector2f(0.0f, 3.0f), 0);
  for (int i = 0; i < 30; ++i) context.Update();  // smooth scrolling settles
  EXPECT_GT(list->GetScrollTop(), 0.0f);
}

TEST(RmlShipLibraryView, game_text_is_shown_as_text_not_markup) {
  Screen screen;
  Rml::Element* row = screen.Get("ship-4");
  ASSERT_NE(row, nullptr);
  // The name is one text node: no <b> or <span> element was made from it.
  EXPECT_EQ(row->GetNumChildren(), 1);
  Rml::ElementText* text = rmlui_dynamic_cast<Rml::ElementText*>(row->GetChild(0));
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->GetText(), "<b>Bold</b> & <span>markup</span>");
}

TEST(RmlShipLibraryView, a_view_model_change_reaches_the_document) {
  Screen screen;
  screen.view_model.OnShipShown(0, 7, ShipSheet{.category_singular = "Ship",
                                                .class_name = "Fer-de-Lance"});
  screen.host.context().Update();
  EXPECT_EQ(TextOf(screen.Get("ship-name")), "Fer-de-Lance");
  EXPECT_TRUE(screen.Get("ship-7")->IsClassSet("shown"));
  EXPECT_FALSE(screen.Get("ship-2")->IsClassSet("shown"));
}

TEST(RmlShipLibraryView, destroying_the_view_unloads_its_document) {
  Screen screen;
  screen.view.reset();
  screen.host.context().Update();
  EXPECT_EQ(screen.host.context().GetNumDocuments(), 0);
  EXPECT_TRUE(screen.host.views.empty());
  // The view-model outlives its view and can still be told things.
  screen.view_model.OnShipShown(0, 1, ShipSheet{});
}
