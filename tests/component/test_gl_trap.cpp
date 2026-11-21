/*	tests/component/test_gl_trap.cpp
        The GL trap catches a GL call (Linux and macOS, where the trap is
   linked; meson registers this program only there). The program runs itself
   again as a child that marks the game headless and makes one unguarded GL
   call, OOTexture::applyNone()'s glBindTexture, the way a missed site in
   the game would; the child must die, and its error output must name the
   function. This is the standing form of quickstart step 4 (removing a
   guard and watching test_boot fail).
   Run: meson test -C build/meson_test test_gl_trap
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include "OpenGLExtensionManager.h"
#include "Texture.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

namespace {

bool IsChild() {
  for (const std::string& argument : oo::component::programArguments()) {
    if (argument == "--child") return true;
  }
  return false;
}

}  // namespace

OO_TEST(a_gl_call_while_headless_is_caught) {
  if (IsChild()) {
    oo::OpenGLExtensionManager::setHeadless(true);
    oo::Texture::applyNone();  // glBindTexture: the trap aborts here
    std::printf("the GL call returned: the trap is not linked\n");
    std::exit(0);
  }
  const std::string program = oo::component::programPath();
  const std::string errors = program + ".child-stderr.txt";
  const int status = oo::component::runChild(program, {"--child"}, "", errors);
  std::ifstream in(errors);
  std::stringstream text;
  text << in.rdbuf();
  std::printf("child exit status %d; its error output:\n%s\n", status,
              text.str().c_str());
  OO_CHECK(status != 0);
  OO_CHECK(text.str().find("GL call glBindTexture while headless") !=
           std::string::npos);
  std::remove(errors.c_str());
}

OO_COMPONENT_MAIN()
