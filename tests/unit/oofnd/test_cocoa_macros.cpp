/*	test_cocoa_macros.cpp
        Regression test: the oofnd headers game code includes (ResourcePaths.hpp
   and so FileSystem.hpp, Data.hpp, Expected.hpp) compile together and work.
*/


#include "oofnd/ResourcePaths.hpp"  // and so FileSystem.hpp, Data.hpp, Expected.hpp



#include "oo_test.hpp"

OO_TEST(headersIncludedUnderTheMacrosWork) {
  const oo::Data d = oo::Data::fromString("ok");
  OO_CHECK_EQ(d.length(), 2u);
  OO_CHECK_EQ(oo::fs::utf8String(oo::fs::pathFromUTF8("a/b")),
              std::string("a/b"));
  oo::PathEnvironment env;
  env.platform = oo::PathEnvironment::Platform::posix;
  env.getenv = [](const std::string&) -> std::optional<std::string> {
    return std::string("/home/x");
  };
  OO_CHECK_EQ(oo::fs::utf8String(oo::ResourcePaths(env).homeDirectory()),
              std::string("/home/x"));
}

OO_TEST_MAIN()
