"""G12 - the screen-matching settings (spec 008 T017, FR-001, FR-005, FR-015).

Game Options' Field Of View row opens the Screen Matching screen on Enter. On it, turning screen
matching on changes the field of view the game reports, and changing the viewing distance changes
it again before the player leaves the screen: every change applies at once (FR-015).

Reuses G5's debug-console witness (test_g5_screen_round_trips.py) to read the game's own
``guiScreen`` and ``oolite.gameSettings.fovValue``, for the reasons G5's docstring gives: neither
transition is logged, so the log could only ever be a proxy.

Leaves screen matching OFF again before it exits, so a later run of any tier starts from the
default.
"""

import os
import time

import pytest

from conftest import assert_clean_exit, assert_no_surviving_game_processes
from test_g5_screen_round_trips import (  # noqa: F401  (screen_witness is a fixture)
    START_SCREEN,
    ScreenWitness,
    screen_witness,
)

GAME_OPTIONS_ROW = 25
EXIT_GAME_ROW = 27

# Game Options' Field Of View row: GUI_ROW(GAME, FOV) with the speech rows present, as on the
# Windows build (PlayerEntity.h's row list; G5 measured the Back row at 20 on the same build). A
# wrong row cannot pass: arrival at the Screen Matching screen is asserted below.
GAME_OPTIONS_FOV_ROW = 14
GAME_OPTIONS_BACK_ROW = 20

# PlayerEntityScreenMatching.cpp's rows.
MATCHING_ROW = 4
DISTANCE_ROW = 10
BACK_ROW = 22

GAME_OPTIONS = "GUI_SCREEN_GAMEOPTIONS"
SCREEN_MATCHING = "GUI_SCREEN_SCREENMATCHING"

EXIT_TIMEOUT_SECONDS = 10
SETTLE_SECONDS = 0.5


def _evaluate(witness, js, timeout=15):
    """A JS expression's value, over the witness's console connection (as gui_screen reads)."""
    import select

    from test_g5_screen_round_trips import (
        CONSOLE_OUTPUT,
        MESSAGE_KEY,
        PACKET_TYPE_KEY,
        PERFORM_COMMAND,
        ScreenWitnessError,
    )

    head = f"@@g12-{os.getpid()}-{witness.port}"
    tail = f"{int(time.time() * 1000) % 1000000}@@"
    marker = head + tail
    witness._send({
        PACKET_TYPE_KEY: PERFORM_COMMAND,
        MESSAGE_KEY: (
            'debugConsole.consoleMessage("command-result", '
            f'"{head}" + "{tail}" + String({js}) + "{head}" + "{tail}");'
        ),
    })
    deadline = time.time() + timeout
    while time.time() < deadline:
        readable, _, _ = select.select([witness._conn], [], [], 1)
        if not readable:
            continue
        packet = witness._recv()
        if packet is None:
            raise ScreenWitnessError("the console connection closed mid-query")
        if packet.get(PACKET_TYPE_KEY) != CONSOLE_OUTPUT:
            continue
        text = str(packet.get(MESSAGE_KEY, ""))
        if text.count(marker) >= 2:
            return text.split(marker)[1]
    raise ScreenWitnessError(f"no answer to {js} within {timeout}s")


def _fov(witness):
    return float(_evaluate(witness, "oolite.gameSettings.fovValue"))


def _press_right_on(game, row):
    # The double click is what selects a row for the game (a single click is sampled per frame
    # and can be missed); on this screen it activates nothing but Back. GameWindow.press_key, not
    # pyautogui: the arrows need the extended-key scancode or the game receives a keypad key
    # (conftest.py, press_key).
    game.confirm_row(row)
    time.sleep(SETTLE_SECONDS)
    game.press_key("right")
    time.sleep(SETTLE_SECONDS)


def test_g12_screen_matching_settings(screen_witness, game):  # noqa: F811
    witness = screen_witness.accept()
    assert witness.await_screen(START_SCREEN) == START_SCREEN

    # 1. Game Options, then Enter on Field Of View opens the Screen Matching screen.
    game.confirm_row(GAME_OPTIONS_ROW)
    assert witness.await_screen(GAME_OPTIONS) == GAME_OPTIONS
    game.select_row(GAME_OPTIONS_FOV_ROW)
    game.confirm_row(GAME_OPTIONS_FOV_ROW)
    seen = witness.await_screen(SCREEN_MATCHING)
    assert seen == SCREEN_MATCHING, (
        f"Enter on Game Options' Field Of View row left the game on {seen}, not "
        f"{SCREEN_MATCHING}"
    )

    before = _fov(witness)

    # 2. Screen matching on: the reported field of view becomes the physical one, at once.
    _press_right_on(game, MATCHING_ROW)
    assert witness.gui_screen() == SCREEN_MATCHING, "changing a setting left the screen"
    on = _fov(witness)
    assert on != pytest.approx(before, abs=0.05), (
        f"turning screen matching on left fovValue at {on}; it should follow the screen"
    )

    # 3. A longer viewing distance narrows it, still without leaving the screen (FR-015).
    _press_right_on(game, DISTANCE_ROW)
    farther = _fov(witness)
    assert farther < on - 0.05, (
        f"moving the seat back left fovValue at {farther} (was {on}); a farther seat must see "
        "the screen at a smaller angle"
    )
    assert witness.gui_screen() == SCREEN_MATCHING

    # 4. Put the defaults back: matching off returns the old field of view.
    _press_right_on(game, MATCHING_ROW)
    restored = _fov(witness)
    assert restored == pytest.approx(before, abs=0.05), (
        f"turning screen matching off left fovValue at {restored}, not the {before} it had"
    )

    # 5. Back to Game Options, back to the start screen, and out.
    game.confirm_row(BACK_ROW)
    assert witness.await_screen(GAME_OPTIONS) == GAME_OPTIONS
    game.confirm_row(GAME_OPTIONS_BACK_ROW)
    assert witness.await_screen(START_SCREEN) == START_SCREEN
    game.select_row(EXIT_GAME_ROW)
    game.confirm_row(EXIT_GAME_ROW)
    assert game.proc.wait(timeout=EXIT_TIMEOUT_SECONDS) == 0
    assert_no_surviving_game_processes(game.proc.pid)
    assert_clean_exit(game)
