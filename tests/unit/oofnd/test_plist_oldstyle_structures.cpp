/*	test_plist_oldstyle_structures.cpp
        The old-style scanner's arrays, dictionaries, data and nesting,
   including GNUstep's <*...> and <[...]> extensions. As in
   test_plist_oldstyle_strings.cpp, every expected value and message was
   captured from GNUstep base 1.31.1's NSPropertyListSerialization; the two
   tests suffixed GnustepDiffers pin oofnd's documented differences instead.
   meson test
   --suite oofnd-plist
*/

#include <gtest/gtest.h>

#include "oofnd/PListOldStyle.hpp"

#include <string>
#include <string_view>

#include "plist_dump.hpp"

namespace {

std::string parsed(std::string_view in) {
  std::expected<oo::PList, oo::PListError> r = oo::parseOldStylePList(in);
  return r ? plist_dump::dump(*r) : "ERROR " + r.error().message;
}

std::string failure(std::string_view detail) {
  return "ERROR Parse failed at " + std::string(detail);
}

oo::PListFormat formatOf(std::string_view in) {
  oo::PListFormat f = oo::PListFormat::kXML;
  (void)oo::parseOldStylePList(in, &f);
  return f;
}

}  // namespace

TEST(plist_oldstyle_structures, arrays) {
  EXPECT_EQ(parsed("()"), "[]");
  EXPECT_EQ(parsed("(a, \"b c\", (d), {e = f;})"),
              "[S\"a\",S\"b c\",[S\"d\"],{\"e\"=S\"f\";}]");
  EXPECT_EQ(parsed("(1 , 2 ,3)"),
              "[S\"1\",S\"2\",S\"3\"]");              // numbers are strings
  EXPECT_EQ(parsed("(a, b,)"), "[S\"a\",S\"b\"]");  // trailing comma
  EXPECT_EQ(parsed("(a,,b)"),
              "[S\"a\",S\"\",S\"b\"]");  // empty unquoted item
}

TEST(plist_oldstyle_structures, arrayErrors) {
  EXPECT_EQ(
      parsed("(a b)"),
      failure("line 1 (char 4) - unexpected character (wanted ',' or ')')"));
  EXPECT_EQ(parsed("(a, b"),
              failure("line 1 (char 6) - reached end of string"));
  EXPECT_EQ(
      parsed("("),
      failure("line 1 (char 2) - unexpected end of string when parsing array"));
  EXPECT_EQ(parsed("( a ) junk"),
              failure("line 1 (char 7) - extra data after parsed string"));
}

TEST(plist_oldstyle_structures, dictionaries) {
  EXPECT_EQ(parsed("{}"), "{}");
  EXPECT_EQ(parsed("{ a = b; \"c d\" = (1, 2); e = { f = g; }; }"),
              "{\"a\"=S\"b\";\"c d\"=[S\"1\",S\"2\"];\"e\"={\"f\"=S\"g\";};}");
  EXPECT_EQ(parsed("{ \"a\" = 1; b = 2 ; }"), "{\"a\"=S\"1\";\"b\"=S\"2\";}");
  // The last ';' may be missing (GSMacOSXCompatible is NO).
  EXPECT_EQ(parsed("{ a = b }"), "{\"a\"=S\"b\";}");
  EXPECT_EQ(parsed("{ a = b; c = d }"), "{\"a\"=S\"b\";\"c\"=S\"d\";}");
  // A repeated key keeps the last value; empty unquoted keys and values are
  // allowed.
  EXPECT_EQ(parsed("{ a = b; a = c; }"), "{\"a\"=S\"c\";}");
  EXPECT_EQ(parsed("{ = b; }"), "{\"\"=S\"b\";}");
  EXPECT_EQ(parsed("{ a = ; }"), "{\"a\"=S\"\";}");
}

TEST(plist_oldstyle_structures, dictionaryErrors) {
  EXPECT_EQ(parsed("{ a b; }"),
              failure("line 1 (char 5) - unexpected character (wanted '=')"));
  EXPECT_EQ(
      parsed("{ a = b, }"),
      failure("line 1 (char 8) - unexpected character (wanted ';' or '}')"));
  EXPECT_EQ(parsed("{ a = b;"), failure("line 1 (char 9) - unexpected end of "
                                          "string when parsing dictionary"));
  EXPECT_EQ(parsed("{a=b;}{c=d;}"),
              failure("line 1 (char 7) - extra data after parsed string"));
  EXPECT_EQ(
      parsed("{ a = \"\xff\"; }"),
      failure(
          "line 1 (char 9) - invalid utf8 data while parsing quoted string"));
}

TEST(plist_oldstyle_structures, nonStringKeysAreRejected_GnustepDiffers) {
  // GNUstep accepts these ({(x) = b;} is a dictionary keyed by an array); oofnd
  // keys are strings.
  EXPECT_EQ(parsed("{ (x) = b; }"),
              failure("line 1 (char 6) - non-string key in dictionary"));
  EXPECT_EQ(parsed("{ <00> = b; }"),
              failure("line 1 (char 7) - non-string key in dictionary"));
}

TEST(plist_oldstyle_structures, hexData) {
  EXPECT_EQ(parsed("<0a0B ff>"), "D<0a0bff>");
  EXPECT_EQ(parsed("<>"), "D<>");
  EXPECT_EQ(parsed("< 01 02 >"), "D<0102>");
  // Comments count as the space between octets.
  EXPECT_EQ(parsed("<01 // x\n 02>"), "D<0102>");
  EXPECT_TRUE(formatOf("<01>") == oo::PListFormat::kOpenStep);
}

TEST(plist_oldstyle_structures, hexDataErrors) {
  EXPECT_EQ(parsed("<012>"),
              failure("line 1 (char 4) - unexpected character (wanted '>')"));
  EXPECT_EQ(parsed("<0g>"),
              failure("line 1 (char 2) - unexpected character (wanted '>')"));
  EXPECT_EQ(
      parsed("<01"),
      failure("line 1 (char 2) - unexpected end of string when parsing data"));
  // A comment may swallow the first '>'; scanning then stops at the next
  // non-space.
  EXPECT_EQ(parsed("<01 /* > */ 02>"),
              failure("line 1 (char 13) - unexpected character (wanted '>')"));
  // A comment running to the end: GNUstep reads past the buffer here; oofnd
  // reports the same message GNUstep produced.
  EXPECT_EQ(parsed("(<01 //>)"),
              failure("line 1 (char 10) - unexpected character (wanted '>')"));
}

TEST(plist_oldstyle_structures, typedValueExtension) {
  EXPECT_EQ(parsed("(<*I12>, <*I-5>, <*I18446744073709551615>, "
                     "<*I99999999999999999999>, "
                     "<*I-99999999999999999999>, <*I\"7\">, <*Iabc>)"),
              "[I12,I-5,U18446744073709551615,U18446744073709551615,I-"
              "9223372036854775808,I7,I0]");
  EXPECT_EQ(parsed("(<*R1.5>, <*R-0.25e2>, <*Rjunk>, <*R\"2.5\">)"),
              "[R1.5,R-25,R0,R2.5]");
  EXPECT_EQ(parsed("(<*BY>, <*BN>, <*BYES>, <*BNO>)"), "[B1,B0,B1,B0]");
  EXPECT_EQ(parsed("<*I\"\">"), "I0");  // "" is too short to be unquoted
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 +0000>; }"),
              "{\"key\"=T0.000;}");
  // Using an extension makes the format GNUstep rather than OpenStep.
  EXPECT_TRUE(formatOf("<*I1>") == oo::PListFormat::kGNUstep);
  EXPECT_TRUE(formatOf("(a)") == oo::PListFormat::kOpenStep);
  // Signedness is kept for the writers.
  EXPECT_TRUE(
      oo::parseOldStylePList("<*I5>")->getIf<oo::PList::Integer>()->isUnsigned);
  EXPECT_TRUE(!oo::parseOldStylePList("<*I-5>")
                ->getIf<oo::PList::Integer>()
                ->isUnsigned);
}

TEST(plist_oldstyle_structures, typedValueErrors) {
  EXPECT_EQ(parsed("<*Bx>"), failure("line 1 (char 5) - bad value for bool"));
  EXPECT_EQ(parsed("<*Q1>"),
              failure("line 1 (char 5) - unrecognized type code after '<*'"));
  EXPECT_EQ(parsed("<*I>"),
              failure("line 1 (char 4) - missing type code after '<*'"));
  EXPECT_EQ(parsed("<*>"),
              failure("line 1 (char 3) - missing type code after '<*'"));
  EXPECT_EQ(
      parsed("<*I12"),
      failure("line 1 (char 6) - unexpected end of string when parsing data"));
  EXPECT_EQ(
      parsed("( <*I12 )"),
      failure("line 1 (char 10) - unexpected end of string when parsing data"));
}

TEST(plist_oldstyle_structures, base64Extension) {
  EXPECT_EQ(
      parsed("(<[AAEC]>, <[]>, <[AAE=]>, <[AA==]>, <[ A A E C ]>, <[=]>)"),
      "[D<000102>,D<>,D<0001>,D<00>,D<000102>,D<00>]");
  EXPECT_EQ(parsed("(<[!!]>)"), "[D<>]");
  EXPECT_TRUE(formatOf("<[AA==]>") == oo::PListFormat::kGNUstep);
  EXPECT_EQ(
      parsed("<[AA"),
      failure("line 1 (char 5) - unexpected end of string when parsing data"));
  EXPECT_EQ(
      parsed("<[AA]"),
      failure("line 1 (char 6) - unexpected end of string when parsing ']>'"));
}

TEST(plist_oldstyle_structures, undecodableExtensionsAreNilWithoutAnError) {
  // GNUstep returns nil for the item and sets no error, so the whole plist is
  // nil and OOPropertyListFromData logs "<no error message>". oofnd: a null
  // PList, not an error.
  EXPECT_EQ(parsed("<[AAE=A]>"), "nil");
  EXPECT_EQ(parsed("(<*Dgarbage>, x)"), "nil");
  EXPECT_TRUE(oo::parseOldStylePList("{ a = (<[AAE=A]>); }").has_value());
}

TEST(plist_oldstyle_structures, dateFieldsNSCalendarDateRejects) {
  // Found by the GNUstep differential fuzzer:
  // -initWithString:calendarFormat: is nil for a day or month of 0 ("Day of
  // month is zero"), so the whole plist is nil.
  EXPECT_EQ(parsed("{ key = <*D2001-01-00 00:00:00 +0000>; }"), "nil");
  EXPECT_EQ(parsed("{ key = <*D2001-00-01 00:00:00 +0000>; }"), "nil");
  EXPECT_EQ(parsed("{ key = <*D2001-01- 00:00:00 +0000>; }"),
              "nil");  // " 0": the day is 0
  EXPECT_EQ(parsed("{ key = <*D2001-0-01 00:00:00 +0000>; }"), "nil");
  EXPECT_EQ(parsed("(<*D2001-13-01 00:00:00 +0000>)"),
              "[T31536000.000]");  // 13 rolls over
}

TEST(plist_oldstyle_structures, dateZoneBeyondEighteenHoursIsUtc_GnustepDiffers) {
  // +timeZoneForSecondsFromGMT: is nil beyond 18 hours and the date falls back
  // to the local zone; oofnd reads it as UTC, as the GNUstep
  // oracle pinned to UTC does.
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 +1800>; }"),
              "{\"key\"=T-64800.000;}");
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 +1801>; }"),
              "{\"key\"=T0.000;}");
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 -1801>; }"),
              "{\"key\"=T0.000;}");
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 +0960>; }"),
              "{\"key\"=T-36000.000;}");
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:00 +19>; }"),
              "{\"key\"=T0.000;}");
  EXPECT_EQ(parsed("{ key = <*D2001-01-01 00:00:005060708090a0b0c0d0e0f>; }"),
              "{\"key\"=T0.000;}");
}

TEST(plist_oldstyle_structures, nestingAndComments) {
  EXPECT_EQ(parsed("{a=(1,{b=<01>;c=({},());});d=\"x\";}"),
              "{\"a\"=[S\"1\",{\"b\"=D<01>;\"c\"=[{},[]];}];\"d\"=S\"x\";}");
  EXPECT_EQ(
      parsed("/*c*/ { /* k */ a /* e */ = /* v */ b /* s */ ; // x\n } // end"),
      "{\"a\"=S\"b\";}");
  EXPECT_EQ(parsed("(a) // trailing"), "[S\"a\"]");
}

TEST(plist_oldstyle_structures, errorPositionsInNestedInput) {
  EXPECT_EQ(
      parsed("\n{\n a = b;\n c = d e;\n}"),
      failure("line 4 (char 19) - unexpected character (wanted ';' or '}')"));
  EXPECT_EQ(parsed("(\n a,\n \"unterminated\n)"),
              failure("line 4 (char 23) - reached end of string while parsing "
                      "quoted string"));
}
