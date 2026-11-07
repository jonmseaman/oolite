# Linux development container: the Ubuntu image the "Test Oolite package" workflow builds on
# (.github/workflows/test_builds.yaml), with LLVM at the major version the clang-format CI check
# pins (clang-format-version in .github/workflows/build-all.yaml).
FROM ubuntu:26.04

ARG LLVM_VERSION=22
ARG CPPLINT_VERSION=2.0.2

ENV DEBIAN_FRONTEND=noninteractive \
    PYTHONUTF8=1 \
    LANG=C.UTF-8

# Mesa gives the tests software GL: on Linux they run the game with SDL's offscreen driver.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates curl git sudo less file xz-utils \
        python3 python3-pip python3-venv pipx \
        libgl1-mesa-dri libegl-mesa0 libglx-mesa0 \
        clang-${LLVM_VERSION} lld-${LLVM_VERSION} llvm-${LLVM_VERSION} \
        clang-format-${LLVM_VERSION} clang-tidy-${LLVM_VERSION} clangd-${LLVM_VERSION} \
    && rm -rf /var/lib/apt/lists/*

# clang.ini names clang, clang++, lld and llvm-ar without a version suffix, and mk.sh format runs
# plain clang-format: put this LLVM first so they resolve to it.
ENV PATH=/usr/lib/llvm-${LLVM_VERSION}/bin:$PATH

# The game's own build dependencies, from the script CI runs (Debian names live there).
COPY ShellScripts/ /tmp/oolite-shellscripts/
RUN apt-get update \
    && /tmp/oolite-shellscripts/Linux/install_packages_root.sh --core \
    && rm -rf /var/lib/apt/lists/* /tmp/oolite-shellscripts

RUN PIPX_HOME=/opt/pipx PIPX_BIN_DIR=/usr/local/bin pipx install "cpplint==${CPPLINT_VERSION}"

# Work as the image's own 'ubuntu' user (uid 1000), with passwordless sudo for installing extras.
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu

USER ubuntu
