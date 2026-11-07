# Linux dev container

An Ubuntu 26.04 container for building and testing the Linux game from a Windows (or any)
checkout. It uses the image the "Test Oolite package" workflow builds on, with LLVM 22.1.2 from
Ubuntu's archive. That is the clang-format version the CI format check runs.

## Use

Open `oolite-dev` in VS Code and choose **Dev Containers: Reopen in Container**. Then, in the
container's terminal:

```bash
./mk.sh format-check      # what CI's clang-format-check job checks
./mk.sh format            # fix it
./mk.sh build test        # the Linux build, Clang 22 + lld
tests/run.sh unit         # oofnd, core and headers suites
tests/run.sh smoke        # launch, snapshot, quit
tests/run.sh component    # Gherkin scenarios over the debug console
cpplint --version         # 2.0.2, the version the cpplint spec baselined with
```

The game runs headless with SDL's offscreen driver on Mesa's software GL, so smoke and
component tests need no display. The GUI tier is Windows-only.

## What it changes

- **Git.** `oolite-dev` is usually a submodule, whose `.git` file points at the parent repo, so
  the container mounts the parent folder and opens `oolite-dev` inside it.
- **Build trees.** `build/` is a Docker volume, `oolite-linux-build`, which hides the Windows
  `build/` from the container and builds far faster than the bind mount. To start over:
  `docker volume rm oolite-linux-build`.
- **Links.** `OOLITE_MAX_LINKS=4` makes `mk.sh` cap meson's parallel links. Every core unit
  test links the whole game with LTO, and dozens of those at once exhaust Docker Desktop's
  memory.
- **clangd.** `OOLITE_COMPILE_COMMANDS_DIR=build` makes `mk.sh` write `compile_commands.json`
  into the volume rather than the checkout's root, so the Windows clangd database stays put.
  The container's clangd reads it from `build/`.

## Limits

- Off Windows the unit tests are built only when `meson test` asks for them, so
  `./mk.sh build` builds just the game. The first `tests/run.sh unit` links about 140 core
  tests and takes a while on the LTO `test` flavour. `BUILD_TYPE=debug` (after
  `./mk.sh build debug`) links without LTO.
- Tests written for the Windows toolchain or a Windows-only feature are left out on Linux.
  `tests/unit/oofnd/meson.build` and `tests/unit/core/meson.build` list each one with its reason.
- Two core tests fail on Linux and are left running so they stay visible:
  - `test_OOProbabilitySet` (`objectElements`) keeps a pointer into the temporary vector that
    `allElements()` returns, and reads it after the vector is destroyed.
  - `test_OOShaderProgram` (`programsFromSources`) expects a `GL_INVALID_VALUE` that Ubuntu's
    Mesa doesn't raise.
- The image doesn't install AppImage or Flatpak tooling, which need FUSE and privileges; CI
  builds those packages.
- cpplint is installed but nothing runs it yet: the lint command, configuration and baseline
  belong to the cpplint adoption spec.
