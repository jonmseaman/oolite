// stb_image_write's implementation, in the third-party library (third_party/meson.build)
// so that it builds without the game's own warning flags.
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
