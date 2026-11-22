/*	test_GuiDisplayGen.mm
        Unit tests for -[GuiDisplayGen rowAtVirtualJoystickPosition:]
   (src/Core/GuiDisplayGen.h).

        A mouse click on a GUI screen activates a row. It used to activate
   UNIVERSE->cursor_row, which only -drawGUI:drawCursor:YES recomputes, i.e. the
   row under the pointer at the LAST RENDER. After a stall longer than the GUI
   tier's 1 s settle, the pointer's motion events and the click arrive in one
   tick, and the click activated a row the pointer had only crossed (G5: aimed
   at the start screen's row 26, activated row 22, NEWGAME). The click now asks
   the GUI for the row under the pointer as it is at click time
   (PlayerEntityControls ClickedGUIRow), and the render uses the same method, so
   the two cannot disagree. These tests pin that method's maths on the main GUI
   (480 px high, rows 16 px from 40 px down) and that it needs no render.

        GuiDisplayGen's object references Universe and the player, so the test
   links the whole game but main (tests/unit/core/meson.build entry ['*'])
   and defines the one global main.mm did, gDebugFlags. UNIVERSE is nil;
   -init and this method do not use it.

        The tests after these four pin what the class's units compute (the two
   initialisers, the sizes, the title rule, the rows' texts and keys, the
   selection and its skip rows, the colours and the colour settings, the fades'
   alpha, the tab-stop override, clearing and the two resizes); the last
   (cxxAPIAnswersTheSame) pins the C++ API. The units they reach (-clear's
   -clearBackground, -getLastLines, -setArray:forRow:,
   -printLineNoScroll:...) run as
   they are. The settings -init reads (gui-settings.plist, through the real
   ResourceManager) depend on where the test runs, so the tests that need them
   check that they are a dictionary first and pin nothing of their content. Run:
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "GuiDisplayGen.h"
#include "Color.h"
#include "OOJavaScriptEngine.h"
#include "OOJSEngineCore.h"

#include "oofnd/Data.hpp"
#include "oofnd/FileSystem.hpp"
#include "oofnd/String.hpp"

#include "oofnd/Exception.hpp"	// oo::Exception

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <process.h>
#include <string>


// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif


namespace {

// The virtual-joystick y that puts the pointer at the vertical middle of main-GUI row `row`
// (the inverse of the row maths: row r spans cursor_y in (200 - 16 r, 216 - 16 r]).
double MiddleOfRow(int row)
{
	double cursor_y = 0.5 * MAIN_GUI_PIXEL_HEIGHT - MAIN_GUI_PIXEL_ROW_START - MAIN_GUI_ROW_HEIGHT * (row - 0.5);
	return -cursor_y / MAIN_GUI_PIXEL_HEIGHT;
}

}	// namespace


TEST(GuiDisplayGen, everyRowUnderItsOwnMiddle)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		EXPECT_TRUE(gui->rowHeight() == MAIN_GUI_ROW_HEIGHT && gui->rowStart() == MAIN_GUI_PIXEL_ROW_START);
		// Rows 0..28 are on screen: row 29's middle (cursor_y -256) is below the GUI's bottom edge
		// (-240), where the pointer is clamped into row 28 (pointerOffTheScreenIsClampedToItsEdge).
		for (int row = 0; row <= 28; row++)
		{
			EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, MiddleOfRow(row))), row);
			// x does not move the row.
			EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.4, MiddleOfRow(row))), row);
			EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(-0.4, MiddleOfRow(row))), row);
		}
	}
}


TEST(GuiDisplayGen, rowBorders)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		// Row 26 spans cursor_y in (-216, -200]; half a pixel inside and outside each border.
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 200.5 / 480.0)), 26);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 215.5 / 480.0)), 26);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 216.5 / 480.0)), 27);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 199.5 / 480.0)), 25);
	}
}


TEST(GuiDisplayGen, pointerOffTheScreenIsClampedToItsEdge)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		// cursor_y is clamped to +-240 (half the GUI's height), as the render clamps the pointer.
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 10.0)), 28);	// 1 + floor(440 / 16)
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 0.5)), 28);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, -10.0)), -2);	// 1 + floor(-40 / 16)
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, -0.5)), -2);
	}
}


TEST(GuiDisplayGen, answersForThePointerNowWithoutARender)
{
	{
		// The G5 failure: the pointer tweened from row 20 through 22 to 26 and the click came before
		// any render. Nothing is drawn here; the row follows the pointer alone.
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, MiddleOfRow(20))), 20);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, MiddleOfRow(22))), 22);
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, MiddleOfRow(26))), 26);
	}
}


// --- The class shell, sizes, colours, fades, rows and selection ---

namespace {

bool SameComponents(oo::Color *a, oo::Color *b)
{
	if (a == nullptr || b == nullptr)  return a == b;
	oo::RGBAComponents ca = a->rgbaComponents(), cb = b->rgbaComponents();
	return ca.r == cb.r && ca.g == cb.g && ca.b == cb.b && ca.a == cb.a;
}


std::optional<std::string> RowString(GuiDisplayGen *gui, oo::GUIRow row)
{
	const oo::PList text = gui->objectForRow(row);
	if (const std::string *string = text.getIf<std::string>())  return *string;
	return std::nullopt;
}


/*	Selecting a row reports it to the player's scripts (guiSelectedRowChanged), and naming that event
	(OOJSID) needs the JavaScript engine, even though PLAYER is nil and nothing is sent. The engine
	is started once, as test_OOJSScript starts it: in a scratch home and game folder, so that it
	reads none of the machine's own files.
*/
void StartJavaScript()
{
	static bool started = false;
	if (started)  return;
	started = true;
	namespace stdfs = std::filesystem;
	const stdfs::path root = stdfs::temp_directory_path() / ("oo-test-guidisplaygen-" + std::to_string(static_cast<unsigned long>(::_getpid())));
	stdfs::remove_all(root);
	stdfs::create_directories(root / "Resources");
	EXPECT_TRUE(::_putenv_s("HOMEPATH", root.string().c_str()) == 0);
	stdfs::current_path(root);
	const std::string info = "{ CFBundleVersion = \"9.9.9-test\"; }";
	EXPECT_TRUE(oo::fs::writeFile(root / "Resources" / "Info-gnustep.plist", oo::Data(info.data(), info.size()), oo::fs::WriteMode::kDirect).has_value());
	(void)OOJavaScriptEngine::sharedEngine();
}


// A small GUI made by the second initialiser: 5 rows, no user settings.
std::shared_ptr<GuiDisplayGen> SmallGUI(const std::optional<std::string> &title)
{
	return oo::makeShared<GuiDisplayGen>(NSMakeSize(200, 100), 4, 5, 12, 8, title);
}

}	// namespace


TEST(GuiDisplayGen, initDefaults)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		EXPECT_TRUE(gui->size().width == 480 && gui->size().height == 480);
		EXPECT_EQ(gui->columns(), 6u);
		EXPECT_EQ(gui->rows(), 30u);
		EXPECT_EQ(gui->rowHeight(), 16u);
		EXPECT_EQ(gui->rowStart(), 40);
		EXPECT_TRUE(gui->getTitle() == std::optional<std::string>(""));	// empty, not none
		Vector position = gui->getDrawPosition();
		EXPECT_TRUE(position.x == 0.0f && position.y == 0.0f && position.z == 640.0f);
		EXPECT_TRUE(SameComponents(gui->getTextColor(), oo::Color::yellowColor().get()));
		EXPECT_TRUE(gui->getTextCommsColor() == nullptr);
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>("."));
		EXPECT_TRUE(RowString(gui.get(), 29) == std::optional<std::string>("."));
		EXPECT_TRUE(gui->objectForRow(30).isNull());
		EXPECT_TRUE(gui->objectForRow(-1).isNull());
		EXPECT_TRUE(gui->keyForRow(3) == std::optional<std::string>("3"));
		EXPECT_TRUE(gui->keyForRow(30) == std::nullopt);
		EXPECT_EQ(gui->rowForKey("17"), 17);
		EXPECT_EQ(gui->rowForKey("x"), -1);
		EXPECT_EQ(gui->rowForKey(std::nullopt), -1);
		EXPECT_EQ(gui->getSelectedRow(), -1);	// nothing selectable yet
		EXPECT_TRUE(gui->getSelectableRange().location == 0 && gui->getSelectableRange().length == 0);
		EXPECT_EQ(gui->alpha(), 0.0f);
		EXPECT_EQ(gui->getStatusPage(), 0u);
	}
}


TEST(GuiDisplayGen, initWithPixelSize)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		EXPECT_TRUE(gui->size().width == 200 && gui->size().height == 100);
		EXPECT_EQ(gui->columns(), 4u);
		EXPECT_EQ(gui->rows(), 5u);
		EXPECT_EQ(gui->rowHeight(), 12u);
		EXPECT_EQ(gui->rowStart(), 8);
		EXPECT_TRUE(gui->getTitle() == std::optional<std::string>("T"));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>(""));
		EXPECT_TRUE(gui->objectForRow(5).isNull());
		EXPECT_TRUE(gui->keyForRow(0) == std::optional<std::string>(""));
		EXPECT_EQ(gui->rowForKey(""), 0);	// the first empty key
		EXPECT_TRUE(SameComponents(gui->getTextColor(), oo::Color::yellowColor().get()));
		EXPECT_TRUE(gui->userSettings().isNull());
		Vector position = gui->getDrawPosition();
		EXPECT_TRUE(position.x == 0.0f && position.y == 0.0f && position.z == 0.0f);
		// The title is kept as given: an empty one stays empty, none stays none.
		EXPECT_TRUE(SmallGUI("")->getTitle() == std::optional<std::string>(""));
		EXPECT_TRUE(SmallGUI(std::nullopt)->getTitle() == std::nullopt);
	}
}


TEST(GuiDisplayGen, titleAndDrawPosition)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setTitle("Hello");
		EXPECT_TRUE(gui->getTitle() == std::optional<std::string>("Hello"));
		gui->setTitle("");	// an empty title is no title
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
		gui->setTitle("Again");
		gui->setTitle(std::nullopt);
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
		gui->setDrawPosition(make_vector(1.0f, -2.0f, 3.5f));
		Vector position = gui->getDrawPosition();
		EXPECT_TRUE(position.x == 1.0f && position.y == -2.0f && position.z == 3.5f);
	}
}


TEST(GuiDisplayGen, rowTextsAndKeys)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setText("one", 1);
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("one"));
		gui->setText(std::optional<std::string>("two"), 2, oo::GUIAlignment::kRight);
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>("two"));
		gui->setText(std::optional<std::string>(std::nullopt), 2, oo::GUIAlignment::kLeft);	// none: no change
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>("two"));
		gui->setText("out", 5);	// out of range: ignored
		gui->setText("out", -1);
		EXPECT_TRUE(gui->objectForRow(5).isNull());
		gui->setKey("k3", 3);
		gui->setKey("k9", 9);	// ignored
		EXPECT_TRUE(gui->keyForRow(3) == std::optional<std::string>("k3"));
		EXPECT_EQ(gui->rowForKey("k3"), 3);
		EXPECT_EQ(gui->rowForKey("k9"), -1);
		gui->setArray({ "a", "b" }, 4);	// a row of columns
		EXPECT_TRUE(gui->objectForRow(4).isArray());
		EXPECT_EQ(gui->objectForRow(4).count(), 2u);

		// The last two rows' text, colour and fade time (-getLastLines) show
    // the colour.
		gui->setColor(oo::Color::redColor().get(), 3);
		gui->setColor(oo::Color::greenColor().get(), 7);	// ignored
		gui->setText("three", 3);
		gui->setText("four", 4);
		const oo::PList lines = gui->getLastLines();
		EXPECT_EQ(lines.count(), 6u);
		EXPECT_TRUE(lines.at<std::string>(0) == "three");
		EXPECT_TRUE(lines.at<std::string>(1) == "1 0 0 1");
		EXPECT_TRUE(lines.at<std::string>(3) == "four");
		EXPECT_TRUE(lines.at<std::string>(4) == "0 1 0 1");	// the initialiser's green
	}
}


TEST(GuiDisplayGen, selectionSkipsSkipRows)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>();
		gui->clear();	// every key is SKIP-ROW
		EXPECT_TRUE(gui->keyForRow(0) == std::optional<std::string>(GUI_KEY_SKIP));
		for (oo::GUIRow row = 2; row <= 6; row++)
		{
			gui->setKey(oo::str::format("key%d", (int)row), row);
			gui->setText(oo::str::format("text%d", (int)row), row);
		}
		gui->setKey(std::string(GUI_KEY_SKIP), 4);
		gui->setArray({ "first", "second" }, 6);
		gui->setSelectableRange(NSMakeRange(2, 5));
		EXPECT_TRUE(gui->getSelectableRange().location == 2 && gui->getSelectableRange().length == 5);

		EXPECT_TRUE(!gui->setSelectedRow(4));	// a skip row
		EXPECT_TRUE(!gui->setSelectedRow(7));	// outside the range
		EXPECT_TRUE(gui->setSelectedRow(3));
		EXPECT_EQ(gui->getSelectedRow(), 3);
		EXPECT_TRUE(gui->setSelectedRow(3));	// already selected
		EXPECT_TRUE(gui->selectedRowKey() == std::optional<std::string>("key3"));
		EXPECT_TRUE(gui->selectedRowText() == std::optional<std::string>("text3"));

		EXPECT_TRUE(gui->setNextRow(1));	// over the skip row
		EXPECT_EQ(gui->getSelectedRow(), 5);
		EXPECT_TRUE(gui->setNextRow(1));
		EXPECT_EQ(gui->getSelectedRow(), 6);
		EXPECT_TRUE(gui->selectedRowText() == std::optional<std::string>("first"));	// the first column
		EXPECT_TRUE(!gui->setNextRow(1));	// off the end: unchanged
		EXPECT_EQ(gui->getSelectedRow(), 6);
		EXPECT_TRUE(gui->setNextRow(-2));
		EXPECT_EQ(gui->getSelectedRow(), 2);	// 4 is a skip row, 2 is not

		EXPECT_TRUE(gui->setLastSelectableRow());
		EXPECT_EQ(gui->getSelectedRow(), 6);
		EXPECT_TRUE(gui->setFirstSelectableRow());
		EXPECT_EQ(gui->getSelectedRow(), 2);

		gui->setNoSelectedRow();
		EXPECT_EQ(gui->getSelectedRow(), -1);
		EXPECT_TRUE(gui->selectedRowKey() == std::nullopt);

		// A selection outside the selectable range is reported as none, but kept.
		EXPECT_TRUE(gui->setSelectedRow(5));
		gui->setSelectableRange(NSMakeRange(10, 2));
		EXPECT_EQ(gui->getSelectedRow(), -1);
		EXPECT_TRUE(gui->selectedRowKey() == std::optional<std::string>("key5"));

		// Nothing selectable (rows 10 and 11 are skip rows, or an empty range): none.
		EXPECT_TRUE(!gui->setFirstSelectableRow());
		EXPECT_TRUE(!gui->setLastSelectableRow());
		EXPECT_TRUE(gui->selectedRowKey() == std::nullopt);
		gui->setSelectableRange(NSMakeRange(0, 0));
		EXPECT_TRUE(!gui->setFirstSelectableRow());
		EXPECT_TRUE(!gui->setLastSelectableRow());
	}
}


TEST(GuiDisplayGen, currentRowIsWhereALineIsPrinted)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setCurrentRow(3);
		gui->printLineNoScroll("printed", oo::GUIAlignment::kLeft, nullptr, 0.0f, "pk", nullptr);
		EXPECT_TRUE(RowString(gui.get(), 3) == std::optional<std::string>("printed"));
		EXPECT_TRUE(gui->keyForRow(3) == std::optional<std::string>("pk"));
		gui->setCurrentRow(5);	// out of range: no current row (and no text cursor)
		gui->setCurrentRow(1);
		gui->printLineNoScroll("again", oo::GUIAlignment::kLeft, nullptr, 0.0f, std::nullopt, nullptr);
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("again"));
		gui->setShowTextCursor(true);
	}
}


TEST(GuiDisplayGen, alphaAndFades)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setAlpha(0.5f);
		EXPECT_EQ(gui->alpha(), 0.5f);	// max alpha 1
		gui->setMaxAlpha(0.5f);
		gui->setAlpha(0.5f);
		EXPECT_EQ(gui->alpha(), 0.25f);
		gui->fadeOutFromTime(10.0, 2.0);
		gui->fadeOutFromTime(10.0, 0.0);
		gui->stopFadeOuts();
		EXPECT_EQ(gui->alpha(), 0.25f);	// fading happens as the GUI is drawn
		gui->setAlpha(0.0f);
		gui->fadeOutFromTime(10.0, 2.0);	// nothing to fade
		EXPECT_EQ(gui->alpha(), 0.0f);
	}
}


TEST(GuiDisplayGen, coloursAndColourSettings)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		std::shared_ptr<oo::Color>	red = oo::Color::redColor();
		gui->setTextColor(red.get());
		EXPECT_TRUE(gui->getTextColor() == red.get());
		gui->setTextColor(nullptr);
		EXPECT_TRUE(SameComponents(gui->getTextColor(), oo::Color::yellowColor().get()));
		gui->setTextCommsColor(red.get());
		EXPECT_TRUE(gui->getTextCommsColor() == red.get());
		gui->setTextCommsColor(nullptr);
		EXPECT_TRUE(SameComponents(gui->getTextCommsColor(), oo::Color::yellowColor().get()));
		gui->setBackgroundColor(red.get());
		gui->setBackgroundColor(nullptr);

		// No settings: the default, else the text colour, as a copy.
		gui->setTextColor(oo::Color::blueColor().get());
		EXPECT_TRUE(SameComponents(gui->colorFromSetting("anything", nullptr).get(), oo::Color::blueColor().get()));
		EXPECT_TRUE(SameComponents(gui->colorFromSetting(std::nullopt, nullptr).get(), oo::Color::blueColor().get()));
		EXPECT_TRUE(SameComponents(gui->colorFromSetting("anything", oo::Color::greenColor().get()).get(), oo::Color::greenColor().get()));
		gui->setGuiColorSettingFromKey("k", red.get());	// no settings: nothing to change
		EXPECT_TRUE(gui->userSettings().isNull());

		// With settings (gui-settings.plist, when the test finds it), a set colour is read back.
		std::shared_ptr<GuiDisplayGen> main = oo::makeShared<GuiDisplayGen>();
		if (main->userSettings().isDict())
		{
			main->setGuiColorSettingFromKey("oo_test_colour", red.get());
			EXPECT_TRUE(main->userSettings().find("oo_test_colour") != nullptr);
			EXPECT_TRUE(SameComponents(main->colorFromSetting("oo_test_colour", oo::Color::greenColor().get()).get(), red.get()));
			main->setGuiColorSettingFromKey("oo_test_colour", nullptr);
			EXPECT_TRUE(main->userSettings().find("oo_test_colour") == nullptr);
			EXPECT_TRUE(SameComponents(main->colorFromSetting("oo_test_colour", oo::Color::greenColor().get()).get(), oo::Color::greenColor().get()));
		}
	}
}


TEST(GuiDisplayGen, tabStops)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		oo::GUITabSettings stops = { 0, 10, 20 };
		gui->setTabStops(stops);
		gui->setTabStops(NULL);
		// No settings: the stops are left as they are.
		gui->overrideTabs(stops, "equipment_tabs", 3);
		EXPECT_TRUE(stops[0] == 0 && stops[1] == 10 && stops[2] == 20);
		gui->overrideTabs(NULL, "equipment_tabs", 3);
		gui->setCharacterSize(NSMakeSize(8, 8));
		gui->setShowAdvancedNavArray(true);
	}
}


TEST(GuiDisplayGen, clearAndResize)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setText("x", 2);
		gui->setKey("k", 2);
		gui->setSelectableRange(NSMakeRange(0, 3));
		gui->clearAndKeepBackground(true);
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>(""));
		EXPECT_TRUE(gui->keyForRow(2) == std::optional<std::string>(GUI_KEY_SKIP));
		EXPECT_TRUE(gui->getSelectableRange().length == 0);
		gui->setTitle("T");
		gui->clear();
		EXPECT_TRUE(gui->getTitle() == std::nullopt);

		// Resize with explicit metrics: the title follows -setTitle:'s rule.
		gui->resizeWithPixelSize(NSMakeSize(300, 200), 3, 4, 10, 5, "");
		EXPECT_TRUE(gui->size().width == 300 && gui->size().height == 200);
		EXPECT_EQ(gui->columns(), 3u);
		EXPECT_EQ(gui->rows(), 4u);
		EXPECT_EQ(gui->rowHeight(), 10u);
		EXPECT_EQ(gui->rowStart(), 5);
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
		EXPECT_TRUE(gui->objectForRow(4).isNull());	// the row range is 0..3 now
		gui->resizeWithPixelSize(NSMakeSize(300, 200), 3, 4, 10, 5, "R");
		EXPECT_TRUE(gui->getTitle() == std::optional<std::string>("R"));

		// Resize to a character height: the metrics follow from it, and a title moves the rows down.
		gui->resizeTo(NSMakeSize(320, 168), 16, "R");
		EXPECT_EQ(gui->columns(), 20u);
		EXPECT_EQ(gui->rows(), 10u);
		EXPECT_EQ(gui->rowHeight(), 16u);
		EXPECT_EQ(gui->rowStart(), 48);	// 2.75 * 16 + 0.5 * (168 - 160)
		EXPECT_TRUE(gui->getTitle() == std::nullopt);	// the closing -clear drops the title the rows made room for
		EXPECT_TRUE(RowString(gui.get(), 9) == std::optional<std::string>(""));
		EXPECT_TRUE(gui->keyForRow(9) == std::optional<std::string>(GUI_KEY_SKIP));
		EXPECT_TRUE(gui->objectForRow(10).isNull());
		gui->resizeTo(NSMakeSize(320, 168), 16, std::nullopt);
		EXPECT_EQ(gui->rowStart(), 20);	// 16 + 0.5 * 8
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
	}
}


// --- The C++ API ---

TEST(GuiDisplayGen, cxxAPIAnswersTheSame)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>(NSMakeSize(200, 100), 4, 5, 12, 8, std::optional<std::string>("T"));
		EXPECT_EQ(gui->columns(), 4u);
		EXPECT_EQ(gui->rows(), 5u);
		EXPECT_EQ(gui->rowHeight(), 12u);
		EXPECT_EQ(gui->rowStart(), 8);
		EXPECT_TRUE(gui->getTitle() == std::optional<std::string>("T"));
		gui->setTitle("");
		EXPECT_TRUE(gui->getTitle() == std::nullopt);
		gui->setText("one", 1);
		gui->setKey("k1", 1);
		EXPECT_TRUE(gui->objectForRow(1) == oo::PList("one"));
		EXPECT_TRUE(gui->keyForRow(1) == std::optional<std::string>("k1"));
		EXPECT_EQ(gui->rowForKey("k1"), 1);
		gui->setSelectableRange(NSMakeRange(0, 3));
		EXPECT_TRUE(gui->getSelectableRange().location == 0 && gui->getSelectableRange().length == 3);
		gui->setMaxAlpha(0.5f);
		gui->setAlpha(1.0f);
		EXPECT_EQ(gui->alpha(), 0.5f);
		gui->setDrawPosition(make_vector(1.0f, 2.0f, 3.0f));
		EXPECT_TRUE(gui->getDrawPosition().z == 3.0f);
		EXPECT_TRUE(SameComponents(gui->getTextColor(), oo::Color::yellowColor().get()));
		EXPECT_EQ(gui->rowAtVirtualJoystickPosition(NSMakePoint(0.0, 0.0)), 4);	// 1 + floor((50 - 8) / 12)
		gui->clearAndKeepBackground(true);
		EXPECT_TRUE(gui->keyForRow(1) == std::optional<std::string>(GUI_KEY_SKIP));

		std::shared_ptr<GuiDisplayGen> main = oo::makeShared<GuiDisplayGen>();
		EXPECT_EQ(main->rows(), 30u);
		EXPECT_TRUE(main->objectForRow(0) == oo::PList("."));
	}
}


// --- Text layout and printing, arrays, scrolling, textures, the status page
// --- The unit test links no font: until the HUD's
// text engine has loaded oolite-font.plist every glyph is 0 wide, so a line
// always fits and long text is split only at its newlines.

TEST(GuiDisplayGen, longTextAndReflow)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		EXPECT_EQ(gui->addLongText(std::nullopt, 1, oo::GUIAlignment::kLeft), 1);
		EXPECT_EQ(gui->addLongText("one line", 1, oo::GUIAlignment::kLeft), 2);
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("one line"));
		EXPECT_EQ(gui->addLongText("a\nb\nc", 2, oo::GUIAlignment::kCenter), 5);
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>("a"));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>("c"));

		EXPECT_TRUE(gui->reflowTextForMFD(std::nullopt) == std::optional<std::string>(""));
		EXPECT_TRUE(gui->reflowTextForMFD("one two\nthree") == std::optional<std::string>("one two\nthree\n"));
		EXPECT_TRUE(gui->reflowTextForMFD("") == std::optional<std::string>("\n"));
	}
}


TEST(GuiDisplayGen, printingScrollsAndKeepsTheLines)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");	// 5 rows
		gui->setCurrentRow(3);
		std::vector<std::string> printed;
		gui->printLongText("first\nsecond", oo::GUIAlignment::kLeft, oo::Color::redColor().get(), 2.0f, "pk", &printed);
		EXPECT_TRUE(printed == (std::vector<std::string>{ "first", "second" }));
		// "first" goes on row 3; "second" would go on the last row, so everything scrolls up by one
		// before it is printed: "first" on row 2, row 3 empty.
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>("first"));
		EXPECT_TRUE(RowString(gui.get(), 3) == std::optional<std::string>(""));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>("second"));
		EXPECT_TRUE(gui->keyForRow(4) == std::optional<std::string>("pk"));
		// The current row is still the last: the next line scrolls everything up by one again.
		gui->printLongText("third", oo::GUIAlignment::kLeft, nullptr, 0.0f, std::nullopt, nullptr);
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("first"));
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>(""));
		EXPECT_TRUE(RowString(gui.get(), 3) == std::optional<std::string>("second"));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>("third"));
		gui->printLongText(std::nullopt, oo::GUIAlignment::kLeft, nullptr, 0.0f, std::nullopt, &printed);
		EXPECT_EQ(printed.size(), 2u);

		const oo::PList lines = gui->getLastLines();
		EXPECT_TRUE(lines.at<std::string>(0) == "second");
		EXPECT_TRUE(lines.at<std::string>(1) == "1 0 0 1");
		EXPECT_TRUE(lines.at<double>(2) == 2.0);
		EXPECT_TRUE(lines.at<std::string>(3) == "third");

		gui->scrollUp(2);
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>(""));
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("second"));
		EXPECT_TRUE(RowString(gui.get(), 2) == std::optional<std::string>("third"));
		EXPECT_TRUE(RowString(gui.get(), 3) == std::optional<std::string>(""));
		EXPECT_TRUE(gui->keyForRow(4) == std::optional<std::string>(""));

		gui->leaveLastLine();
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>(""));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>(""));	// the last row is kept (it was empty)
		const oo::PList last = gui->getLastLines();
		EXPECT_TRUE(last.at<double>(5) > 0.39 && last.at<double>(5) < 0.41);	// it fades
	}
}


TEST(GuiDisplayGen, arraysAndInsertedItems)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setArray({ "a", "b", "c" }, 0);
		EXPECT_EQ(gui->objectForRow(0).count(), 3u);
		gui->setArray({ "x" }, 5);	// out of range
		EXPECT_TRUE(gui->objectForRow(5).isNull());

		for (oo::GUIRow row = 0; row < 5; row++)
		{
			gui->setText(oo::str::format("r%d", (int)row), row);
			gui->setKey(oo::str::format("k%d", (int)row), row);
		}
		const oo::PList items(oo::PList::Array{ oo::PList("new1"), oo::PList(oo::PList::Array{ oo::PList("c1"), oo::PList("c2") }) });
		const oo::PList keys(oo::PList::Array{ oo::PList("n1"), oo::PList("n2") });
		gui->insertItemsFromArray(items, keys, 1, nullptr);
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>("r0"));
		EXPECT_TRUE(RowString(gui.get(), 1) == std::optional<std::string>("new1"));
		EXPECT_TRUE(gui->objectForRow(2).isArray());
		EXPECT_TRUE(RowString(gui.get(), 3) == std::optional<std::string>("r1"));
		EXPECT_TRUE(RowString(gui.get(), 4) == std::optional<std::string>("r2"));
		EXPECT_TRUE(gui->keyForRow(2) == std::optional<std::string>("n2"));
		EXPECT_TRUE(gui->keyForRow(4) == std::optional<std::string>("k2"));

		// Without keys the inserted rows get empty keys; nothing to insert changes nothing.
		gui->insertItemsFromArray(oo::PList(oo::PList::Array{ oo::PList("only") }), oo::PList(), 0, oo::Color::redColor().get());
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>("only"));
		EXPECT_TRUE(gui->keyForRow(0) == std::optional<std::string>(""));
		gui->insertItemsFromArray(oo::PList(), oo::PList(), 0, nullptr);
		gui->insertItemsFromArray(oo::PList(oo::PList::Array{}), oo::PList(), 0, nullptr);
		EXPECT_TRUE(RowString(gui.get(), 0) == std::optional<std::string>("only"));

		// Keys of another length raise.
		bool raised = false;
		try
		{
			gui->insertItemsFromArray(items, oo::PList(oo::PList::Array{ oo::PList("one") }), 0, nullptr);
		}
		catch (const oo::Exception &)
		{
			raised = true;
		}
		EXPECT_TRUE(raised);
	}
}


TEST(GuiDisplayGen, statusPage)
{
	{
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setStatusPage(0);
		EXPECT_EQ(gui->getStatusPage(), 1u);
		gui->setStatusPage(2);
		EXPECT_EQ(gui->getStatusPage(), 3u);
		gui->setStatusPage(-1);
		EXPECT_EQ(gui->getStatusPage(), 2u);
		gui->setStatusPage(-2);	// would reach 0: back to the first page
		EXPECT_EQ(gui->getStatusPage(), 1u);
		gui->drawEquipmentList(oo::PList(), 0.0f);	// nothing to draw
		gui->drawEquipmentList(oo::PList(oo::PList::Array{}), 0.0f);
	}
}


TEST(GuiDisplayGen, noTexturesWithoutAUniverse)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		// No texture is named: no sprite.
		EXPECT_TRUE(!gui->setBackgroundTextureDescriptor(oo::PList()));
		EXPECT_TRUE(!gui->setForegroundTextureDescriptor(oo::PList()));
		EXPECT_TRUE(!gui->setBackgroundTextureKey(std::nullopt));
		EXPECT_TRUE(!gui->setForegroundTextureKey("anything"));
		EXPECT_TRUE(!gui->preloadGUITexture(oo::PList()));
		gui->clearBackground();
		gui->setBackgroundTextureSpecial(oo::GUIBackgroundSpecial::kLong, true);
		gui->setBackgroundTextureSpecial(oo::GUIBackgroundSpecial::kNone, false);

		// A JS null is "no texture" (an empty descriptor), and an empty string the same.
		JSContext *context = OOJSAcquireContext();
		const oo::PList none = gui->textureDescriptorFromJSValue(JS_NULL, context, "test");
		EXPECT_TRUE(none.isDict() && none.count() == 0);
		OOJSRelinquishContext(context);
	}
}


TEST(GuiDisplayGen, cxxSlice2API)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>(NSMakeSize(200, 100), 4, 5, 12, 8, std::optional<std::string>("T"));
		EXPECT_EQ(gui->addLongText("a\nb", 0, oo::GUIAlignment::kLeft), 2);
		EXPECT_TRUE(gui->objectForRow(1) == oo::PList("b"));
		EXPECT_TRUE(gui->reflowTextForMFD("x y") == std::optional<std::string>("x y\n"));
		gui->setCurrentRow(2);
		std::vector<std::string> printed;
		gui->printLineNoScroll("p", oo::GUIAlignment::kLeft, nullptr, 0.0f, std::optional<std::string>("pk"), &printed);
		EXPECT_TRUE(gui->keyForRow(2) == std::optional<std::string>("pk") && printed.size() == 1);
		gui->setArray({ "c1", "c2" }, 3);
		EXPECT_TRUE(gui->objectForRow(3).isArray());
		gui->scrollUp(1);
		EXPECT_TRUE(gui->objectForRow(2).isArray());
		gui->setStatusPage(0);
		EXPECT_EQ(gui->getStatusPage(), 1u);
		EXPECT_TRUE(!gui->setBackgroundTextureDescriptor(oo::PList()));
		EXPECT_TRUE(!gui->preloadGUITexture(oo::PList()));
		gui->clearBackground();
		gui->leaveLastLine();
		EXPECT_TRUE(gui->getLastLines().isArray());
	}
}


// --- Drawing, the star chart's title, found systems
// --- No universe and no player (both nil): the drawing has no view to ask, no
// chart to draw and no found systems, so these pin what the units do without
// them.

TEST(GuiDisplayGen, drawingWithoutAUniverse)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		EXPECT_EQ(gui->drawGUI(0.0f, false), 0);	// too faint to draw: nothing, row 0
		gui->setAlpha(0.5f);
		gui->fadeOutFromTime(0.0, 1.0);
		EXPECT_EQ(gui->drawGUI(0.0f, false), 0);
		EXPECT_EQ(gui->alpha(), 0.5f);	// the fade moves only as the GUI is drawn visibly
		gui->drawGUIBackground();	// no background sprite
		gui->refreshStarChart();
		EXPECT_EQ(gui->targetNextFoundSystem(1), 0);	// not on a chart screen: the player's target
		EXPECT_EQ(gui->targetNextFoundSystem(0), 0);
	}
}


TEST(GuiDisplayGen, starChartTitleWithoutAUniverse)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = SmallGUI("T");
		gui->setStarChartTitle();
		const std::optional<std::string> title = gui->getTitle();
		std::printf("  star chart title: %s\n", title ? title->c_str() : "(none)");
		EXPECT_TRUE(title != std::optional<std::string>("T"));	// replaced
	}
}


TEST(GuiDisplayGen, cxxSlice3API)
{
	{
		StartJavaScript();
		std::shared_ptr<GuiDisplayGen> gui = oo::makeShared<GuiDisplayGen>(NSMakeSize(200, 100), 4, 5, 12, 8, std::optional<std::string>("T"));
		EXPECT_EQ(gui->drawGUI(0.0f, false), 0);
		gui->drawGUIBackground();
		gui->refreshStarChart();
		EXPECT_EQ(gui->targetNextFoundSystem(1), 0);
		gui->setStarChartTitle();
		EXPECT_TRUE(gui->getTitle() != std::optional<std::string>("T"));
	}
}
