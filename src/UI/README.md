# src/UI

The interface layer: full-screen GUI screens as **model, view-model and views** (MVVM), so a
screen's state and commands live apart from how it is drawn. Spec: `specs/007-rmlui-gui` in the
parent repo. Only the ship library is here so far (a prototype).

| Piece | What it is | Example |
| --- | --- | --- |
| Model | plain data the game fills | `ShipLibrary/ShipLibraryModel.h` |
| Navigator | what the view-model asks of the game (an interface the game implements) | `ShipLibrary/ShipLibraryNavigator.h`, `src/Core/ShipLibraryGameAdapter.*` |
| View-model | state views bind to, and the commands keys, joystick and pointer call | `ShipLibrary/ShipLibraryViewModel.*` |
| Views | the classic text grid, or an RmlUi document bound to the view-model | `ShipLibrary/GridShipLibraryView.*`, `RmlUi/` |
| Host | made once by `GameController` (the composition root) and injected into `MyOpenGLView` (input) and `Universe` (drawing, view factory) | `UiHost.h`, `NullUiHost.*` |

Rules:

- Nothing here includes a game class. The game's side comes in through an interface (a navigator,
  `GridSurface`) that `src/Core` implements.
- Only `RmlUi/` includes RmlUi. The rest builds alone in the `ui` unit suite (`tests/unit/ui`).
- Objects are given what they use in their constructors; the long-lived game objects built in two
  phases (`Universe`, `MyOpenGLView`) take the host through a setter. No `sharedX()` singletons.
- Text from game data or scripts reaches a document only as data-binding text (`{{ }}`), never as
  markup.

## Trying the RmlUi look

Run the game with `-modern-ui`, or set the default `ui-look` to `modern`, and open the ship library
(start screen, or `mission.runShipLibrary()` while docked). The classic grid stays the default, and
the headless game always uses it.

## Prototype findings (spec 007)

To copy into `specs/007-rmlui-gui` Notes when a plan is written:

- **MVVM fits RmlUi.** The view-model maps one-to-one onto an RmlUi data model. The RmlUi view is
  about 150 lines, the document and stylesheet are data, and the classic grid is just another view
  of the same view-model (its rows match the old ones exactly, checked by `test_grid_ship_library_view`).
- **Pointer positions must be mapped in proportion, not by display density.** The game draws its
  GUI pass into a target that need not be the window's pixel size: on a Retina Mac it was
  1536x1536 for a 1512x917-point window early on, and the window itself is replaced after start-up.
  The host sizes RmlUi from the GL viewport of each pass and takes the window from each event
  (`PointerMapping.h`).
- **GL state.** RmlUi's GL2 renderer runs inside push/pop of all attributes, client attributes and
  matrices. The game's state checks stay quiet.
- **Input split.** Keys and joystick stay with the game, which calls view-model commands with its
  repeat latches. Only pointer events go to RmlUi. An RmlUi control that leaves the screen must not
  destroy its own view mid-event, so Exit is deferred to the next update.
- **Changed behaviour.** Two presses before a ship has flown in now move two ships; the classic
  code moved one ship for two Nexts and three for two Previouses. The classic tab stops beyond the
  third are now zero, where they were uninitialised.
- **Not done here.** The document's own words ("Ship Library", "Back", the key hint) are English
  in the RML, not description keys. There is no classic font and no in-game look switch. Under the
  RmlUi look the grid is not written, so debug-console row reads see nothing (FR-013). Opened
  in-game, the HUD is still drawn under the library, as it is in the classic look. Windows and Linux
  builds are untried: they need FreeType (MSYS2 `mingw-w64-ucrt-x86_64-freetype`; the parent repo's
  `tools/setup-windows.sh` does not install it yet).
