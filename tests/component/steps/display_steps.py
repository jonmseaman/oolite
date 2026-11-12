"""Steps for spec 008's screen-matching scenarios (tests/component/features/s9_screen_matching.feature).

Kept apart from world_steps.py, whose vocabulary is ship behaviour. These steps add two things a
display scenario needs: preferences for the scenario's game, passed on its command line before it
starts, and reads of what the game reports about its projection.
"""

import json

from pytest_bdd import given, then, parsers

# Degrees: the spec's tolerance for every reported angle (SC-001).
ANGLE_TOLERANCE = 0.5


@given(parsers.parse('the preference "{key}" is "{value}"'))
def preference_is(world, key, value):
    """Set one preference for the game this scenario starts. Must come before the game starts."""
    if world.console is not None:
        raise AssertionError("a preference step must come before the game is started")
    world.preferences[key] = value


@given(parsers.parse("a window of {width:d}x{height:d}"))
def window_of(world, width, height):
    """The game's window size, as its window_width and window_height preferences."""
    preference_is(world, "window_width", str(width))
    preference_is(world, "window_height", str(height))


@then(parsers.parse('the game setting "{name}" is about {expected:g}'))
def game_setting_is_about(world, name, expected):
    value = float(world.console.evaluate(f"oolite.gameSettings.{name}"))
    assert abs(value - expected) <= ANGLE_TOLERANCE, (
        f"oolite.gameSettings.{name} is {value}, expected {expected} (+/- {ANGLE_TOLERANCE})"
    )


def display_projection(world):
    """The debug console's read of the projection in use (contracts/settings.md, Debug read)."""
    return json.loads(world.console.evaluate("JSON.stringify(console.__displayProjection())"))


@then(parsers.parse('the projection\'s "{field}" is about {expected:g} within {tolerance:g}'))
def projection_field_is_about(world, field, expected, tolerance):
    value = display_projection(world)[field]
    assert abs(float(value) - expected) <= tolerance, (
        f"projection {field} is {value}, expected {expected} (+/- {tolerance})"
    )


@then(parsers.parse('the projection\'s "{field}" equals {expected}'))
def projection_field_is(world, field, expected):
    value = display_projection(world)[field]
    assert value == json.loads(expected), f"projection {field} is {value!r}, expected {expected}"
