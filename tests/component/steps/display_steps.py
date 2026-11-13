"""Steps for spec 008's screen-matching scenarios (tests/component/features/s9_screen_matching.feature).

Kept apart from world_steps.py, whose vocabulary is ship behaviour. These steps add two things a
display scenario needs: preferences for the scenario's game, passed on its command line before it
starts, and reads of what the game reports about its projection.
"""

import json

from pytest_bdd import given, then, when, parsers

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


def _closed_log_lines(world):
    """Latest.log's lines, read after the game has quit so the log is complete and closed."""
    import os

    world.close()
    log = os.path.join(world.output_dir, "Latest.log")
    if not os.path.isfile(log):
        raise AssertionError(f"no log written at {log}")
    with open(log, "r", encoding="utf-8", errors="replace") as handle:
        return [line.rstrip() for line in handle]


@then(parsers.parse('the log has a "{log_class}" line'))
def log_has_class(world, log_class):
    tag = f"[{log_class}]"
    assert any(tag in line for line in _closed_log_lines(world)), f"no {tag} line in Latest.log"


@then(parsers.parse('the log has no "{log_class}" line'))
def log_has_no_class(world, log_class):
    tag = f"[{log_class}]"
    found = [line for line in _closed_log_lines(world) if tag in line]
    assert not found, f"{tag} lines in Latest.log:\n" + "\n".join(found[:10])


@then(parsers.parse('no log line contains "{text}"'))
def no_log_line_contains(world, text):
    found = [line for line in _closed_log_lines(world) if text in line]
    assert not found, f"lines containing {text!r} in Latest.log:\n" + "\n".join(found[:10])


@given(parsers.parse("a buoy targeted {degrees:g} degrees right of the nose at {km:g} km"))
def buoy_targeted_right_of_nose(world, degrees, km):
    """A still target at a known bearing, made the player's primary target, with its reticle.

    The player only takes a target in flight (PlayerEntity::addTarget), and a launch is still
    STATUS_LAUNCHING for a while after docked turns false.
    """
    import time

    deadline = time.time() + 30
    while world.console.evaluate("player.ship.status").strip() != "STATUS_IN_FLIGHT":
        if time.time() > deadline:
            raise AssertionError("the player never reached STATUS_IN_FLIGHT")
        time.sleep(0.5)
    world.console.perform(
        "(function () {"
        # The reticle dial needs this (Resources/Config/hud.plist); the scenario save lacks it.
        "  player.ship.awardEquipment('EQ_SCANNER_SHOW_MISSILE_TARGET');"
        "  var s = player.ship, a = %f * Math.PI / 180, d = %f;"
        "  var p = s.position.add(s.vectorForward.multiply(d * Math.cos(a)))"
        "                    .add(s.vectorRight.multiply(d * Math.sin(a)));"
        "  var b = system.addShips('buoy', 1, p, 0)[0];"
        "  b.velocity = [0, 0, 0];"
        "  s.target = b;"
        "})();" % (degrees, km * 1000.0)
    )
    time.sleep(1.0)
    target = world.console.evaluate("String(player.ship.target)")
    assert "Buoy" in target, f"the buoy did not become the target (target is {target})"


def _curved_window_point(direction, world):
    """Where a scene direction meets a curved panel, as a window point (spec 008 C3).

    Worked out here from the panel's shape alone, not from OODisplayGeometry: the eye at the
    origin looking down +z, the panel's centre at z = distance, its axis at z = distance - radius;
    the window covers the whole panel (screen-window-fraction 1). At the physical angle the scene
    and eye directions are the same.
    """
    import math

    prefs = world.preferences
    diagonal = float(prefs["screen-diagonal-mm"])
    aspect = float(prefs["screen-aspect"])
    radius = float(prefs["screen-curvature-mm"])
    distance = float(prefs["viewer-distance-mm"])
    width_px, height_px = float(prefs["window_width"]), float(prefs["window_height"])
    arc = diagonal * aspect / math.sqrt(aspect * aspect + 1)
    height = arc / aspect
    dx, dy, dz = direction
    axis = distance - radius
    # |t (dx, dz) - (0, axis)| = radius, the far root.
    a = dx * dx + dz * dz
    b = -2 * dz * axis
    c = axis * axis - radius * radius
    t = (-b + math.sqrt(b * b - 4 * a * c)) / (2 * a)
    angle = math.atan2(t * dx, t * dz - axis)
    x = width_px / 2 + radius * angle / arc * width_px
    y = height_px / 2 - t * dy / height * height_px
    return x, y


@then(parsers.parse("the reticle is within {pixels:g} px of where the curved screen shows its target"))
def reticle_on_curved_target(world, pixels):
    reticle = display_projection(world)["reticle"]
    assert reticle is not None, "no reticle has been drawn"
    expected = _curved_window_point(reticle["direction"], world)
    got = reticle["window"]
    error = ((got[0] - expected[0]) ** 2 + (got[1] - expected[1]) ** 2) ** 0.5
    assert error <= pixels, (
        f"reticle drawn at {got}, the target is shown at {expected} ({error:.1f} px away)"
    )


@when(parsers.parse('the detail level is set to "{level}"'))
def detail_level_set(world, level):
    import time

    world.console.perform(f'console.detailLevel = "{level}";')
    time.sleep(1.0)


@then(parsers.parse('the last "{log_class}" line says "{text}"'))
def last_log_line_says(world, log_class, text):
    tag = f"[{log_class}]"
    found = [line for line in _closed_log_lines(world) if tag in line]
    assert found, f"no {tag} line in Latest.log"
    assert text in found[-1], f"the last {tag} line is {found[-1]!r}, expected it to say {text!r}"
