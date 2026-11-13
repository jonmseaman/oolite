"""G13 - a click on a wide window lands where it does on a 16:9 one (spec 008 H2, T015, FR-016).

On a window wider than 16:9 the GUI is laid out in the centred 16:9 HUD area, so the mouse must
be measured against that area, not the whole window. GUI rows are picked by height alone, which
the HUD area never changes, so a menu click cannot tell; the charts can: a click on the
short-range chart picks the system under the pointer from both coordinates.

The test is differential, so it needs none of the chart's drawing maths. The window is set to
1920x1080, where the HUD area is the whole window, and to 3840x1080, where it is the centred
1920x1080. In each, the chart is recentred (Home) and clicked at the same offset from the HUD
area's centre, and the game's own ``player.ship.infoSystem`` is read back. The two must be the
same system, and not the one the player is in (so the click moved the cursor at all). Measured
against the whole window, the wide window's click would land at half the horizontal offset; the
test first checks, at 16:9, that half the offset picks another system, so that mistake cannot
pass.

Reuses G6's private staged app (resizing writes the window size into the defaults), its console
witness and its resize helper, and G4's start: Start New Commander, then Normal Start.
"""

import time

import pyautogui
import pytest

from conftest import assert_no_surviving_game_processes
from test_g4_start_game_hud_renders import (
    NORMAL_START_ROW,
    START_NEW_COMMANDER_ROW,
)
from test_g6_window_lifecycle import (  # noqa: F401  (app_dir and window_witness are fixtures)
    app_dir,
    client_size,
    set_client_rect,
    wait_for,
    window_witness,
)

NARROW = (1920, 1080)
WIDE = (3840, 1080)
ORIGIN = (0, 0)

# The click, in pixels from the HUD area's centre: right and a little up. Far enough from the
# current system to reach another one, near enough that the chart does not scroll.
CLICK_OFFSET = (480, -60)

SCREEN_TIMEOUT_SECONDS = 30
SETTLE_SECONDS = 1.0
EXIT_TIMEOUT_SECONDS = 10


def _await(predicate, what, timeout=SCREEN_TIMEOUT_SECONDS):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if predicate():
            return
        time.sleep(0.5)
    pytest.fail(f"timed out waiting for {what}")


def _resize(game, witness, size):
    set_client_rect(game.hwnd, ORIGIN, size)
    wait_for(lambda: client_size(game.hwnd) == size)
    seen = witness.wait_for_game_window_size(size)
    assert seen == size, f"asked for a {size} client area; the game reports {seen}"
    time.sleep(SETTLE_SECONDS)


def _open_short_range_chart(game, witness):
    """F6 opens the chart the player last used, and toggles between the two charts."""
    seen = []
    for _ in range(3):
        game.press_key("f6")
        time.sleep(2 * SETTLE_SECONDS)
        screen = witness.evaluate("guiScreen")
        seen.append(screen)
        if screen == "GUI_SCREEN_SHORT_RANGE_CHART":
            return
    pytest.fail(f"F6 did not open the short-range chart; the screens seen were {seen}")


def _click_chart(game, witness, offset=None):
    """Recentre the chart, click at ``offset`` from the HUD area's centre, read infoSystem."""
    offset = offset or CLICK_OFFSET
    game.press_key("home")
    time.sleep(SETTLE_SECONDS)
    left, top, width, height = game.client_rect()
    x = left + width // 2 + offset[0]
    y = top + height // 2 + offset[1]
    game.assert_focused()
    game.assert_click_point_is_ours(x, y)
    # Aim first and let a frame read the pointer back (conftest.aim_at_row's reasoning), then
    # click without moving: a press and release within 2 virtual pixels is a click, not a drag.
    pyautogui.moveTo(x, y, duration=0.2)
    time.sleep(SETTLE_SECONDS)
    # Held across a few frames: the chart samples the button per frame (gvMouseLeftButton) and
    # acts on the release it sees after a press it saw.
    pyautogui.mouseDown(x, y)
    time.sleep(0.25)
    pyautogui.mouseUp(x, y)
    time.sleep(SETTLE_SECONDS)
    return int(witness.evaluate("player.ship.infoSystem"))


def test_g13_wide_window_chart_click(window_witness, game):  # noqa: F811
    witness = window_witness.accept()
    assert game.proc.poll() is None, "the game exited before the test could start"

    # Into a game: Start New Commander, then Normal Start, as G4 does.
    game.select_row(START_NEW_COMMANDER_ROW)
    game.confirm_row(START_NEW_COMMANDER_ROW)
    assert witness.await_screen("GUI_SCREEN_NEWGAME") == "GUI_SCREEN_NEWGAME"
    time.sleep(SETTLE_SECONDS)
    game.select_row(NORMAL_START_ROW)
    game.confirm_row(NORMAL_START_ROW)
    _await(lambda: witness.evaluate("player.ship.status") in
           ("STATUS_DOCKED", "STATUS_IN_FLIGHT"), "a commander in the game")
    time.sleep(2 * SETTLE_SECONDS)
    here = int(witness.evaluate("system.ID"))

    # 16:9: the HUD area is the whole window.
    _resize(game, witness, NARROW)
    game.focus()
    _open_short_range_chart(game, witness)
    narrow_pick = _click_chart(game, witness)
    assert narrow_pick != here, (
        f"the click at {CLICK_OFFSET} from the centre left the chart on the current system "
        f"({here}) at {NARROW}; it must reach another system for the comparison to mean anything"
    )
    # The mistake this test exists for measures the wide window's click against the whole
    # window, which halves its horizontal offset. That is only caught if half the offset picks
    # a different system; check it here, where the HUD area is the whole window.
    half = (CLICK_OFFSET[0] // 2, CLICK_OFFSET[1])
    half_pick = _click_chart(game, witness, half)
    assert half_pick != narrow_pick, (
        f"a click at half the horizontal offset ({half}) picks the same system ({narrow_pick}) "
        f"as {CLICK_OFFSET} does, so this chart cannot tell a correct click from a halved one; "
        "CLICK_OFFSET must be moved"
    )

    # 32:9: the HUD area is the centred 1920x1080. The same click must pick the same system.
    _resize(game, witness, WIDE)
    game.focus()
    assert witness.evaluate("guiScreen") == "GUI_SCREEN_SHORT_RANGE_CHART"
    wide_pick = _click_chart(game, witness)
    assert wide_pick == narrow_pick, (
        f"the same click, {CLICK_OFFSET} from the HUD area's centre, picked system {narrow_pick} "
        f"at {NARROW} and {wide_pick} at {WIDE}. The GUI is laid out in the centred 16:9 area, "
        "so the mouse must be measured against it (spec 008 FR-016, H2)"
    )

    # Out, the way a user closes a running game (G4's step 5).
    game.close_window()
    assert game.proc.wait(timeout=EXIT_TIMEOUT_SECONDS) == 0
    assert_no_surviving_game_processes(game.proc.pid)
