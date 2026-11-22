/*	test_OOFullScreenController.mm
        Unit tests for OOFullScreenController
   (src/Core/OOFullScreenController.h).

        OOFullScreenController is the abstract base of the Mac full-screen
   controllers (Mac-only, not in this tree), so on this build nothing makes one.
   This pins what the base computes: the game view it
   keeps (retained until the controller goes), every "subclass responsibility"
   method's log call and default answer, and -currentDisplayMode, the one method
        with a body, which picks the current index out of the subclass's mode
   list (null for no list, an index past the end, or oo::kNotFound). The test
   subclass is a C++ one. Run: tests/run.sh unit
*/

#include <cstdlib>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "FullScreenController.h"
#include "Logging.h"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked. The one function of it that the base calls is defined here instead,
   and counts.
*/
static int gSubclassResponsibilities = 0;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
  gSubclassResponsibilities++;
}

}  // namespace oo


/*	The view the controller keeps is a C++ view. SDL/MyOpenGLView.mm is not
   linked: its destructor (a view made here has no window or context to
   destroy) and its virtual members (never called) are defined here.
*/
MyOpenGLView::~MyOpenGLView() {}
void MyOpenGLView::endSplashScreen() { std::abort(); }
void MyOpenGLView::updateScreen() { std::abort(); }
void MyOpenGLView::setGameController(::GameController*) { std::abort(); }
bool MyOpenGLView::inFullScreenMode() { std::abort(); }
std::vector<oo::PList> MyOpenGLView::getScreenSizeArray() { std::abort(); }
NSSize MyOpenGLView::currentScreenSize() { std::abort(); }
oo::PList MyOpenGLView::currentScreenMode() { std::abort(); }
void MyOpenGLView::noteMouseInteractionModeChangedFrom(oo::MouseInteractionMode,
                                                       oo::MouseInteractionMode) {
  std::abort();
}
void MyOpenGLView::resetTypedString() { std::abort(); }
void MyOpenGLView::stringToClipboard(const std::string&) { std::abort(); }
NSSize MyOpenGLView::getViewSize() { std::abort(); }

namespace {

oo::PList Mode(unsigned width, unsigned height, unsigned refresh) {
  oo::PList::Dict mode;
  mode.emplace(std::string(oo::kDisplayWidth), oo::PList(width));
  mode.emplace(std::string(oo::kDisplayHeight), oo::PList(height));
  mode.emplace(std::string(oo::kDisplayRefreshRate), oo::PList(refresh));
  return oo::PList(std::move(mode));
}

oo::PList Modes() {
  return oo::PList(oo::PList::Array{Mode(640, 480, 60), Mode(1024, 768, 75),
                                    Mode(1920, 1080, 60)});
}

}  // namespace

// A subclass that answers the two methods currentDisplayMode() asks.
class TestFullScreenController final : public oo::FullScreenController {
 public:
  using oo::FullScreenController::FullScreenController;

  oo::PList displayModes() override { return modes; }

  std::size_t indexOfCurrentDisplayMode() override { return index; }

  oo::PList modes;
  std::size_t index = 0;
};

TEST(OOFullScreenController, baseDefaults) {
  {
    MyOpenGLView* view = oo::test::NewRetained<MyOpenGLView>();
    const std::size_t viewRetains = view->weak_from_this().use_count();
    std::shared_ptr<oo::FullScreenController> controller =
        oo::makeShared<oo::FullScreenController>(view);
    EXPECT_TRUE(controller != nullptr);
    EXPECT_TRUE(controller->gameView() == view);
    EXPECT_TRUE(view->weak_from_this().use_count() == viewRetains + 1);

    gSubclassResponsibilities = 0;
    EXPECT_TRUE(!controller->inFullScreenMode());
    EXPECT_TRUE(gSubclassResponsibilities == 1);
    controller->setFullScreenMode(true);
    EXPECT_TRUE(gSubclassResponsibilities == 2);
    EXPECT_TRUE(controller->displayModes().isNull());
    EXPECT_TRUE(gSubclassResponsibilities == 3);
    EXPECT_TRUE(controller->indexOfCurrentDisplayMode() == oo::kNotFound);
    EXPECT_TRUE(gSubclassResponsibilities == 4);
    EXPECT_TRUE(!controller->setDisplayWidth(800, 600, 60));
    EXPECT_TRUE(gSubclassResponsibilities == 5);
    EXPECT_TRUE(controller->findDisplayModeForWidth(800, 600, 60).isNull());
    EXPECT_TRUE(gSubclassResponsibilities == 6);

    // No mode list: null, after asking only for the list.
    EXPECT_TRUE(controller->currentDisplayMode().isNull());
    EXPECT_TRUE(gSubclassResponsibilities == 7);

    // The one method with an empty body.
    controller->noteMouseInteractionModeChangedFrom(
        oo::MouseInteractionMode::kUiScreenNoInteraction,
        oo::MouseInteractionMode::kFlightWithMouseControl);
    EXPECT_TRUE(gSubclassResponsibilities == 7);

    controller = nullptr;
    EXPECT_TRUE(view->weak_from_this().use_count() == viewRetains);
    oo::test::Release(view);
  }
}

TEST(OOFullScreenController, nilGameView) {
  {
    std::shared_ptr<oo::FullScreenController> controller =
        oo::makeShared<oo::FullScreenController>(nullptr);
    EXPECT_TRUE(controller != nullptr);
    EXPECT_TRUE(controller->gameView() == nullptr);
  }
}

TEST(OOFullScreenController, currentDisplayMode) {
  {
    std::shared_ptr<TestFullScreenController> controller =
        oo::makeShared<TestFullScreenController>(nullptr);
    gSubclassResponsibilities = 0;

    controller->modes = Modes();
    controller->index = 1;
    EXPECT_TRUE(controller->currentDisplayMode() == Mode(1024, 768, 75));
    controller->index = 0;
    EXPECT_TRUE(controller->currentDisplayMode() == Mode(640, 480, 60));
    controller->index = 2;
    EXPECT_TRUE(controller->currentDisplayMode() == Mode(1920, 1080, 60));
    EXPECT_TRUE(controller->currentDisplayMode().get<std::size_t>(
                 std::string(oo::kDisplayWidth)) == 1920);

    // An index past the end, and oo::kNotFound: null.
    controller->index = 3;
    EXPECT_TRUE(controller->currentDisplayMode().isNull());
    controller->index = oo::kNotFound;
    EXPECT_TRUE(controller->currentDisplayMode().isNull());

    // A mode list that is not an array, and an empty one: null.
    controller->index = 0;
    controller->modes = Mode(640, 480, 60);
    EXPECT_TRUE(controller->currentDisplayMode().isNull());
    controller->modes = oo::PList(oo::PList::Array{});
    EXPECT_TRUE(controller->currentDisplayMode().isNull());

    // The subclass's overrides were used: the base logged nothing.
    EXPECT_TRUE(gSubclassResponsibilities == 0);
  }
}
