"""Binds the S9 feature file (spec 008, screen matching) to its step libraries."""

from pytest_bdd import scenarios

from steps.display_steps import *  # noqa: F401,F403  (step definitions register on import)
from steps.world_steps import *  # noqa: F401,F403

scenarios("features/s9_screen_matching.feature")
