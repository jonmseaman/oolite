Feature: Screen matching (spec 008)

  The space view's field of view can be worked out from the player's screen and seat. With
  screen matching off, which is the default, everything is as it was before the feature.

  Scenario: Screen matching is off by default and the field of view is the preference
    Given the preference "fov-value" is "60"
    And a universe seeded with 20260910
    Then the game setting "fovValue" is about 60

  # C5, the projection pin (spec 008 research R8a): the projection with screen matching off,
  # recorded before any of the feature's drawing changes. These values are never edited: a task
  # that changes them has changed the game for players who do not use screen matching. At
  # windows wider than 16:9 only the values the 16:9 HUD area (FR-016) leaves alone are pinned.
  Scenario: The projection at 1920x1080 with screen matching off is as before the feature
    Given the preference "fov-value" is "57.2"
    And a window of 1920x1080
    And a universe seeded with 20260910
    Then the projection's "right" is about 0.5452177 within 0.000001
    And the projection's "left" is about -0.5452177 within 0.000001
    And the projection's "top" is about 0.3066849 within 0.000001
    And the projection's "bottom" is about -0.3066849 within 0.000001
    And the projection's "displayZ" is about 853.3333 within 0.001
    And the projection's "windowDisplayZ" is about 853.3333 within 0.001
    And the projection's "hudArea" equals [0, 0, 1920, 1080]
    And the projection's "fovValue" is about 57.2 within 0.0001
    And the projection's "screenMatching" equals false

  Scenario: The projection at 2560x1080 with screen matching off is as before the feature
    Given the preference "fov-value" is "57.2"
    And a window of 2560x1080
    And a universe seeded with 20260910
    Then the projection's "right" is about 0.5452177 within 0.000001
    And the projection's "top" is about 0.2300137 within 0.000001
    And the projection's "windowDisplayZ" is about 1137.7778 within 0.001
    And the projection's "fovValue" is about 57.2 within 0.0001

  Scenario: The projection at 3840x1080 with screen matching off is as before the feature
    Given the preference "fov-value" is "57.2"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the projection's "right" is about 0.5452177 within 0.000001
    And the projection's "top" is about 0.1533425 within 0.000001
    And the projection's "windowDisplayZ" is about 1706.6666 within 0.001
    And the projection's "fovValue" is about 57.2 within 0.0001

  # H5 (spec 008 FR-016): on a window wider than 16:9 the HUD and the GUI are laid out in the
  # centred 16:9 area, at full height, while the space view keeps the whole window (C5 above).
  Scenario: The HUD area on a 3840x1080 window is its centred 16:9 part
    Given the preference "fov-value" is "57.2"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the projection's "hudArea" equals [960, 0, 1920, 1080]
    And the projection's "displayZ" is about 853.3333 within 0.001
    And the projection's "windowDisplayZ" is about 1706.6666 within 0.001

  # C2 (spec 008 Story 1 and 2): with screen matching on, the field of view is worked out from
  # the screen and the seat. screen-window-fraction pins the share of the panel the window
  # covers, so the answer does not depend on the monitor the test runs on.
  Scenario: A 27-inch 16:9 flat screen at 60 cm gives 53.0 degrees
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "685.8"
    And the preference "screen-aspect" is "1.7777778"
    And the preference "viewer-distance-mm" is "600"
    And the preference "screen-window-fraction" is "1"
    And a window of 1920x1080
    And a universe seeded with 20260910
    Then the game setting "fovValue" is about 53.0
    And the projection's "screenMatching" equals true
    And the projection's "right" is about 0.4981 within 0.001

  # C2 for a curved screen (spec 008 Story 2): the Samsung Odyssey G9, 49" 32:9 at 1000R, seen
  # from 80 cm, fills 84 degrees across.
  Scenario: A 49-inch 32:9 1000R screen at 80 cm gives 84.0 degrees
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the game setting "fovValue" is about 84.0
    And the projection's "screenMatching" equals true

  # The final warp (spec 008 FR-006a, T020-T021): a curved screen is drawn as a flat render plus
  # the warp, and its buffers are complete.
  Scenario: A curved screen in flight runs the warp pass
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the log has a "display.warp.pass" line
    And no log line contains "framebuffer not complete"

  # With screen matching off nothing new is drawn (research R4).
  Scenario: Screen matching off never runs the warp pass
    Given the preference "fov-value" is "57.2"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the log has no "display.warp.pass" line

  # C3 (spec 008 research R13): with the warp, a marker sits where the curved screen shows its
  # target, near the edge too, not where a flat projection would put it.
  Scenario: The reticle follows its target to the edge of a curved screen
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    And a buoy targeted 35 degrees right of the nose at 10 km
    Then the projection's "warped" equals true
    And the reticle is within 2 px of where the curved screen shows its target

  # FR-017: without shaders there is no warp. A curved screen is drawn classic flat at the same
  # physical angle, and the change applies without a restart.
  Scenario: Lowering the detail level drops the warp and keeps the angle
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    When the detail level is set to "DETAIL_LEVEL_MINIMUM"
    Then the projection's "warped" equals false
    And the game setting "fovValue" is about 84.0
    And the log has a "display.warp.pass" line
    And the last "display.warp.pass" line says "off"

  # Story 3 (T023): a custom horizontal angle wider than the physical one, its look changed and
  # Physical restored, all while flying. Settings changed live are not given on the command line,
  # where they would win over the change.
  Scenario: A custom 120-degree angle on the G9, its look changed live, then Physical again
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    When the display preference "fov-custom-horizontal" is set to "120"
    And the display preference "fov-mode" is set to "custom"
    Then the game setting "fovValue" is about 120.0
    And the projection's "warped" equals true
    When the display preference "fov-wide-look" is set to "even"
    Then the game setting "fovValue" is about 120.0
    And the projection's "warped" equals true
    When the display preference "fov-mode" is set to "physical"
    Then the game setting "fovValue" is about 84.0

  # T024 (research R4): on a flat screen the warp runs only for Panini (the default look) or even
  # spacing above the physical angle; classic flat at the same angle is an ordinary perspective.
  Scenario: A flat screen at a custom 120 degrees warps for Panini but not for classic flat
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "685.8"
    And the preference "screen-aspect" is "1.7777778"
    And the preference "viewer-distance-mm" is "600"
    And the preference "screen-window-fraction" is "1"
    And the preference "fov-mode" is "custom"
    And the preference "fov-custom-horizontal" is "120"
    And a window of 1920x1080
    And a universe seeded with 20260910
    Then the game setting "fovValue" is about 120.0
    And the projection's "warped" equals true
    When the display preference "fov-wide-look" is set to "classic"
    Then the projection's "warped" equals false
    And the game setting "fovValue" is about 120.0

  # T025 (research R14): the player pushes its field of view every update; with screen matching
  # on that push must not move the angle off the one worked out from the screen.
  Scenario: Flying does not move the screen-matched angle
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And the preference "fov-value" is "57.2"
    And a window of 3840x1080
    And a universe seeded with 20260910
    When the simulation runs for 60 ticks
    Then the game setting "fovValue" is about 84.0

  # C4 (spec 008 FR-009, FR-010, SC-004): with no size entered, screen matching takes the size the
  # display reports. screen-detected-size-mm stands in for the platform's answer (the G9's
  # 1198 x 337 mm, on a 3840 x 1080 display). Entering only the curvature and the distance then
  # gives the G9's angle.
  Scenario: A detected size fills in the screen, and curvature and distance finish the job
    Given the preference "screen-matching" is "YES"
    And the preference "screen-detected-size-mm" is "1198x337@3840x1080"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And a window of 3840x1080
    And a universe seeded with 20260910
    Then the projection's "sizeSource" equals "detected"
    And the game setting "fovValue" is about 84.0

  Scenario: An entered size is never overwritten by detection
    Given the preference "screen-matching" is "YES"
    And the preference "screen-size-source" is "entered"
    And the preference "screen-diagonal-mm" is "685.8"
    And the preference "screen-aspect" is "1.7777778"
    And the preference "screen-detected-size-mm" is "1198x337"
    And the preference "viewer-distance-mm" is "600"
    And the preference "screen-window-fraction" is "1"
    And a window of 1920x1080
    And a universe seeded with 20260910
    Then the projection's "sizeSource" equals "entered"
    And the game setting "fovValue" is about 53.0

  # T026 (FR-018, research R6): a very wide angle is drawn as several slices, each with its own
  # yaw, warped together; no OpenGL error on the way.
  Scenario: A custom 160 degrees on the G9 is drawn in slices
    Given the preference "screen-matching" is "YES"
    And the preference "screen-diagonal-mm" is "1244.6"
    And the preference "screen-aspect" is "3.5555556"
    And the preference "screen-curvature-mm" is "1000"
    And the preference "viewer-distance-mm" is "800"
    And the preference "screen-window-fraction" is "1"
    And the preference "fov-mode" is "custom"
    And the preference "fov-custom-horizontal" is "160"
    And a window of 3840x1080
    And a universe seeded with 20260910
    When OpenGL errors are logged
    Then the game setting "fovValue" is about 160.0
    And the projection's "warped" equals true
    And the projection's "slices" is at least 2
    And no log line contains "OpenGL error"
