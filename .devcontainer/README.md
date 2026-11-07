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
- **clangd.** `OOLITE_COMPILE_COMMANDS_DIR=build` makes `mk.sh` write `compile_commands.json`
  into the volume rather than the checkout's root, so the Windows clangd database stays put.
  The container's clangd reads it from `build/`.

## Limits

- The unit, core and headers suites are built only on Windows (see `meson.build`), so
  `tests/run.sh unit` finds no tests here.
- The image doesn't install AppImage or Flatpak tooling, which need FUSE and privileges; CI
  builds those packages.
- cpplint is installed but nothing runs it yet: the lint command, configuration and baseline
  belong to the cpplint adoption spec.
