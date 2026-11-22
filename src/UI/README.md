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
