// tinyexr's implementation, in the third-party library (third_party/meson.build) so that
// it builds without the game's own warning flags. Windows only, as
// OOSaveEXRSnapshot.cpp is.
#ifdef WIN32
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"
#endif
