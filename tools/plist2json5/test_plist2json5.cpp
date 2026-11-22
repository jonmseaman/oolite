/*	tools/plist2json5/test_plist2json5.cpp
        plist2json5::convert and plist2json5::check (Plist2JSON5.hpp): the
   OpenStep text converted token by token with its layout and comments, both
   modes' typing rules, and the check catching a JSON5 file that reads back
   differently. meson test --suite plist2json5
*/

#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "Plist2JSON5.hpp"

using plist2json5::Mode;

namespace {

std::string converted(std::string_view plist, Mode mode = Mode::strings) {
  std::expected<std::string, std::string> r = plist2json5::convert(plist, mode);
  if (!r) return "ERROR " + r.error();
  const std::string problem = plist2json5::check(plist, *r, mode);
  return problem.empty() ? *r : "CHECK " + problem;
}

}  // namespace

TEST(plist2json5, punctuationAndLayout) {
  EXPECT_EQ(converted("{\n\tkey = value;\n\tlist = (a, b, );\n}\n"),
              "{\n\tkey: \"value\",\n\tlist: [\"a\", \"b\", ],\n}\n");
  EXPECT_EQ(converted("( 1, \"two\" )"), "[ \"1\", \"two\" ]\n");
  EXPECT_EQ(converted("{ a = 1 }"), "{ a: \"1\" }\n");  // no final ';'
  EXPECT_EQ(converted("{}"), "{}\n");
  EXPECT_EQ(converted("()"), "[]\n");
}

TEST(plist2json5, commentsStay) {
  EXPECT_EQ(converted("// head\n{ /* a */ a = b; // tail\n}"),
              "// head\n{ /* a */ a: \"b\", // tail\n}\n");
  EXPECT_EQ(converted("{ a /* why */ = b; }"), "{ a /* why */ : \"b\", }\n");
  EXPECT_EQ(converted("{ a\n\t= b; }"), "{ a\n\t: \"b\", }\n");
}

TEST(plist2json5, commentsAreMadeSafeForJSON5) {
  // A CR or U+2028 ends a // comment in JSON5 but not in OpenStep.
  EXPECT_EQ(converted("( a // x\ry\n)"), "[ \"a\" // x y\n]\n");
  EXPECT_EQ(converted("( a // x\xe2\x80\xa8y\n)"), "[ \"a\" // x y\n]\n");
  EXPECT_EQ(converted("( a // crlf\r\n)"), "[ \"a\" // crlf\r\n]\n");
  // Latin-1 in a comment becomes UTF-8.
  EXPECT_EQ(converted("( a /* caf\xe9 */ )"),
              "[ \"a\" /* caf\xc3\xa9 */ ]\n");
  // GNUstep counts backspace as whitespace; JSON5 doesn't.
  EXPECT_EQ(converted("(\ba)"), "[ \"a\"]\n");
}

TEST(plist2json5, keysAreBareWhenTheyCanBe) {
  EXPECT_EQ(converted("{ \"0 0\" = x; adder-player = y; \"plain\" = z; }"),
              "{ \"0 0\": \"x\", \"adder-player\": \"y\", plain: \"z\", }\n");
}

TEST(plist2json5, stringsKeepTheirOpenStepValues) {
  EXPECT_EQ(converted(R"(( "tab\tnew\nquote\"", "\101\U00e9", "a\u12" ))"),
              "[ \"tab\\tnew\\nquote\\\"\", \"A\xc3\xa9\", \"a\" ]\n");
  EXPECT_EQ(converted("( a,, b )"), "[ \"a\",\"\", \"b\" ]\n");
  EXPECT_EQ(converted("( \"%H [$text|cr] %@\" )"),
              "[ \"%H [$text|cr] %@\" ]\n");
}

TEST(plist2json5, dataAndGNUstepExtensions) {
  EXPECT_EQ(converted("( <000102>, <*I5>, <*R1.5>, <*BY> )"),
              "[ {$data: \"AAEC\"}, 5, 1.5, true ]\n");
}

TEST(plist2json5, stringsModeTypesNothing) {
  EXPECT_EQ(converted("{ a = yes; b = 12; c = 1.5; }"),
              "{ a: \"yes\", b: \"12\", c: \"1.5\", }\n");
}

TEST(plist2json5, typedModeTypesPlainUnquotedTokens) {
  EXPECT_EQ(
      converted("( yes, no, YES, NO, true, false, 0, -12, 1.5, -0.25, 2.0e3 )",
                Mode::typed),
      "[ true, false, true, false, true, false, 0, -12, 1.5, -0.25, 2.0e3 ]\n");
}

TEST(plist2json5, typedModeLeavesEverythingElseAString) {
  EXPECT_EQ(converted("( \"12\", \"yes\", 007, 1., .5, 1e3, 0x10, Yes, on, "
                        "\"0.5 0.5\", 99999999999999999999 )",
                        Mode::typed),
              "[ \"12\", \"yes\", \"007\", \"1.\", \".5\", \"1e3\", \"0x10\", "
              "\"Yes\", \"on\", \"0.5 0.5\", \"99999999999999999999\" ]\n");
}

TEST(plist2json5, typedRealsAreWhatTheGameReads) {
  // NSScanner keeps 18 significant digits, so this many digits reads as a
  // slightly different double than the literal; the converter writes that.
  const std::string out =
      converted("( 0.12345678901234567890123 )", Mode::typed);
  EXPECT_TRUE(out.rfind("[ 0.", 0) == 0);
  EXPECT_TRUE(out.find("CHECK") == std::string::npos);
}

TEST(plist2json5, xmlIsWrittenFromTheTree) {
  EXPECT_EQ(
      converted("<?xml version=\"1.0\"?><plist><dict><key>b</key>"
                "<integer>0</integer><key>a</key><true/></dict></plist>"),
      "{\n\ta: true,\n\tb: 0,\n}\n");
}

TEST(plist2json5, checkCatchesADifference) {
  EXPECT_EQ(plist2json5::check("{ a = b; }", "{a: \"c\"}", Mode::strings),
              "the data differs at (top).a: JSON5 string \"c\", plist string "
              "\"b\"");
  EXPECT_EQ(plist2json5::check("( 12 )", "[12]", Mode::strings),
              "the data differs at (top)[0]: JSON5 integer, plist string "
              "\"12\"");
  EXPECT_TRUE(plist2json5::check("( 12 )", "[12]", Mode::typed).empty());
  EXPECT_EQ(plist2json5::check("( 12 )", "[13]", Mode::typed),
              "the data differs at (top)[0]: JSON5 integer, plist string "
              "\"12\"");
  EXPECT_EQ(plist2json5::check("( x )", "[true]", Mode::typed),
              "the data differs at (top)[0]: JSON5 bool, plist string \"x\"");
  EXPECT_EQ(plist2json5::check("{ a = b; }", "{}", Mode::strings),
              "the data differs at (top): JSON5 lacks the key \"a\"");
  EXPECT_EQ(plist2json5::check("( a )", "[", Mode::strings),
              "the JSON5 does not parse: JSON5 parse failed at line 1, column "
              "2 - unterminated array");
}

TEST(plist2json5, badInputIsReported) {
  EXPECT_EQ(converted("{ a = b"), "ERROR unterminated dictionary at byte 8");
  EXPECT_EQ(converted("{ a b; }"), "ERROR expected '=' at byte 5");
}
