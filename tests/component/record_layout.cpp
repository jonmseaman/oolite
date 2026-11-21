/*	tests/component/record_layout.cpp
        Prints the keyboard layout SDL sees, as a recorded-layout table
   (tests/component/layouts/<name>.tsv): for every scancode and the
   modifier states -, shift, caps, shift+caps and altgr, the key code
   SDL_GetKeyFromScancode(scancode, mods, false) gives, where it gives one.
   It starts SDL's real video backend, so the table is the system's current
   layout; --video-driver dummy gives SDL's own built-in (US) map instead.
   Built, not registered as a test.
   Usage: record_layout [--video-driver <name>] [--name <layout name>]
*/

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstring>
#include <string>

namespace {

constexpr const char* kModsNames[] = {"-", "shift", "caps", "shift+caps", "altgr"};
constexpr SDL_Keymod kModsStates[] = {
    SDL_KMOD_NONE, SDL_KMOD_LSHIFT, SDL_KMOD_CAPS,
    static_cast<SDL_Keymod>(SDL_KMOD_LSHIFT | SDL_KMOD_CAPS), SDL_KMOD_MODE};

// The character a key code stands for, in UTF-8, or SDL's name for the key.
std::string Comment(SDL_Keycode key) {
  if (key >= 0x21 && key < 0x110000 && (key & SDLK_SCANCODE_MASK) == 0 &&
      key != 0x7F) {
    char utf8[8] = {};
    SDL_UCS4ToUTF8(key, utf8);
    return utf8;
  }
  const char* name = SDL_GetKeyName(key);
  return name != nullptr && *name != '\0' ? name : "?";
}

}  // namespace

int main(int argc, char** argv) {
  std::string name = "layout";
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::strcmp(argv[i], "--video-driver") == 0) {
      SDL_SetHint(SDL_HINT_VIDEO_DRIVER, argv[++i]);
    } else if (std::strcmp(argv[i], "--name") == 0) {
      name = argv[++i];
    }
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::fprintf(stderr, "record_layout: SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }
  const char* driver = SDL_GetCurrentVideoDriver();
  std::printf("# %s. Recorded with SDL %d.%d.%d, video driver %s, %s, by printing\n",
              name.c_str(), SDL_VERSIONNUM_MAJOR(SDL_GetVersion()),
              SDL_VERSIONNUM_MINOR(SDL_GetVersion()),
              SDL_VERSIONNUM_MICRO(SDL_GetVersion()),
              driver != nullptr ? driver : "(none)", SDL_GetPlatform());
  std::printf("# SDL_GetKeyFromScancode(scancode, mods, false) for every scancode\n");
  std::printf("# (tests/component/record_layout.cpp).\n");
  std::printf("# scancode\tmods\tkeycode\tcomment\n");
  for (int code = 1; code < SDL_SCANCODE_COUNT; ++code) {
    for (int cls = 0; cls < 5; ++cls) {
      const SDL_Keycode key = SDL_GetKeyFromScancode(
          static_cast<SDL_Scancode>(code), kModsStates[cls], false);
      if (key == SDLK_UNKNOWN) continue;
      std::printf("%d\t%s\t0x%x\t%s\n", code, kModsNames[cls],
                  static_cast<unsigned>(key), Comment(key).c_str());
    }
  }
  SDL_Quit();
  return 0;
}
