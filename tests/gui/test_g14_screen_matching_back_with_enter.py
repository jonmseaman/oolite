"""G14 - Enter on the Screen Matching screen's Back row stays on Game Options (spec 008 FR-005).

Back returns to Game Options with its Field Of View row selected, and Enter on that row opens
the Screen Matching screen. Oolite samples keys per frame, so the Enter that chose Back is still
down on Game Options' first frame; unless it is cleared there, Game Options reopens the screen at
once and Back never leaves it (Jon's hand check, 2026-11-13). G12 leaves by double-click, which
the screen clears, so it cannot see this; this test leaves with the keyboard.

Reuses G5's debug-console witness and G12's rows.
"""

import time

from conftest import assert_clean_exit, assert_no_surviving_game_processes
from test_g5_screen_round_trips import (  # noqa: F401  (screen_witness is a fixture)
    START_SCREEN,
    screen_witness,
)
from test_g12_screen_matching_settings import (
    BACK_ROW,
    EXIT_GAME_ROW,
    GAME_OPTIONS,
    GAME_OPTIONS_BACK_ROW,
    GAME_OPTIONS_FOV_ROW,
    GAME_OPTIONS_ROW,
    SCREEN_MATCHING,
)

EXIT_TIMEOUT_SECONDS = 10
SETTLE_SECONDS = 0.5
# Long enough for the game to have run many frames after the press, so a bounce back to the
# Screen Matching screen has happened if it is going to.
STAY_SECONDS = 2.0


def test_g14_screen_matching_back_with_enter(screen_witness, game):  # noqa: F811
    witness = screen_witness.accept()
    assert witness.await_screen(START_SCREEN) == START_SCREEN

    # Game Options, then Field Of View opens the Screen Matching screen.
    game.confirm_row(GAME_OPTIONS_ROW)
    assert witness.await_screen(GAME_OPTIONS) == GAME_OPTIONS
    game.select_row(GAME_OPTIONS_FOV_ROW)
    game.confirm_row(GAME_OPTIONS_FOV_ROW)
    assert witness.await_screen(SCREEN_MATCHING) == SCREEN_MATCHING
    time.sleep(SETTLE_SECONDS)

    # Select Back with the keyboard, then Enter. The screen opens on its first row, and Up from
    # the first row wraps to the last selectable one, BACK_ROW. (A single click is sampled per
    # frame and can be missed, so the mouse is not used to select it.)
    assert BACK_ROW == 22
    game.press_key("up")
    time.sleep(SETTLE_SECONDS)
    game.press_key("enter")
    time.sleep(STAY_SECONDS)
    seen = witness.gui_screen()
    assert seen == GAME_OPTIONS, (
        f"Enter on the Screen Matching screen's Back row left the game on {seen}, not "
        f"{GAME_OPTIONS}: the Enter that chose Back reopened the screen from Game Options' "
        "Field Of View row"
    )

    # Out, as G12 leaves.
    game.confirm_row(GAME_OPTIONS_BACK_ROW)
    assert witness.await_screen(START_SCREEN) == START_SCREEN
    game.select_row(EXIT_GAME_ROW)
    game.confirm_row(EXIT_GAME_ROW)
    assert game.proc.wait(timeout=EXIT_TIMEOUT_SECONDS) == 0
    assert_no_surviving_game_processes(game.proc.pid)
    assert_clean_exit(game)
