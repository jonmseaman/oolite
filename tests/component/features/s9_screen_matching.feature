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
