/*	test_plist_json5.cpp
        oo::parseJSON5PList (oofnd/PListJSON5.hpp), oo::writeJSON5PList
   (oofnd/PListWriting.hpp) and oo::loadPListFile (oofnd/PListFile.hpp): the
   JSON5 1.0.0 grammar (https://spec.json5.org) read into an oo::PList, the
   mapping rules in PListJSON5.hpp's header comment, the writer's round trip,
   and the rule that <name>.json5 stands in for <name>.plist. There is no
   GNUstep oracle for JSON5; the expectations are the spec's and the mapping
   rules'. meson test --suite oofnd-plist
*/

#include "oofnd/PListJSON5.hpp"

#include <cmath>
#include <cstdint>
#include <expected>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "oo_test.hpp"
#include "oofnd/PListFile.hpp"
#include "oofnd/PListWriting.hpp"
#include "plist_dump.hpp"

using namespace std::string_view_literals;

namespace {

std::string parsed(std::string_view json5) {
  std::expected<oo::PList, oo::PListError> r = oo::parseJSON5PList(json5);
  return r ? oo_test::dump(*r) : "ERROR " + r.error().message;
}

std::string written(const oo::PList& plist) {
  std::expected<oo::Data, oo::PListError> r = oo::writeJSON5PList(plist);
  if (!r) return "ERROR " + r.error().message;
  return std::string(reinterpret_cast<const char*>(r->bytes()), r->length());
}

// Written, then read back: equal to the original.
bool roundTrips(const oo::PList& plist) {
  const std::string text = written(plist);
  std::expected<oo::PList, oo::PListError> back = oo::parseJSON5PList(text);
  return back && *back == plist;
}

oo::PList dict(std::vector<std::pair<std::string, oo::PList>> entries) {
  oo::PList::Dict d;
  for (auto& [k, v] : entries) d[k] = std::move(v);
  return oo::PList(std::move(d));
}

}  // namespace

// --- values ------------------------------------------------------------------

OO_TEST(scalarsMapToTheirPListKinds) {
  OO_CHECK_EQ(parsed("true"), "B1");
  OO_CHECK_EQ(parsed("false"), "B0");
  OO_CHECK_EQ(parsed("\"text\""), "S\"text\"");
  OO_CHECK_EQ(parsed("'text'"), "S\"text\"");
  OO_CHECK_EQ(parsed("42"), "I42");
  OO_CHECK_EQ(parsed("-7"), "I-7");
  OO_CHECK_EQ(parsed("+7"), "I7");
  OO_CHECK_EQ(parsed("2.5"), "R2.5");
}

OO_TEST(objectsAndArraysNest) {
  OO_CHECK_EQ(parsed("{a: 1, 'b': [true, 2.5, \"x\",], /* c */ c: {}}"),
              "{\"a\"=I1;\"b\"=[B1,R2.5,S\"x\"];\"c\"={};}");
  OO_CHECK_EQ(parsed("[]"), "[]");
  OO_CHECK_EQ(parsed("{}"), "{}");
  OO_CHECK_EQ(parsed("[[[]]]"), "[[[]]]");
  OO_CHECK_EQ(parsed("[1,]"), "[I1]");
  OO_CHECK_EQ(parsed("{a:1,}"), "{\"a\"=I1;}");
}

OO_TEST(anArrayCanBeTheTopLevel) {
  // equipment.plist's shape: positional rows of mixed kinds.
  OO_CHECK_EQ(parsed("[[0, 2, \"Fuel\", \"EQ_FUEL\", {available_to_all: true}]]"),
              "[[I0,I2,S\"Fuel\",S\"EQ_FUEL\",{\"available_to_all\"=B1;}]]");
}

OO_TEST(repeatedKeysKeepTheLast) {
  OO_CHECK_EQ(parsed("{a: 1, a: 2}"), "{\"a\"=I2;}");
}

OO_TEST(nullIsAnError) {
  OO_CHECK_EQ(parsed("null"),
              "ERROR JSON5 parse failed at line 1, column 1 - null has no "
              "property-list equivalent");
  OO_CHECK_EQ(parsed("{a: null}"),
              "ERROR JSON5 parse failed at line 1, column 5 - null has no "
              "property-list equivalent");
  // A key may be spelled null.
  OO_CHECK_EQ(parsed("{null: 1}"), "{\"null\"=I1;}");
}

// --- keys --------------------------------------------------------------------

OO_TEST(keysMayBeIdentifiersOrStrings) {
  OO_CHECK_EQ(parsed("{$a_1: 1, _b: 2, \"0 0\": 3, 'adder-player': 4}"),
              "{\"$a_1\"=I1;\"0 0\"=I3;\"_b\"=I2;\"adder-player\"=I4;}");
  OO_CHECK_EQ(parsed("{true: 1, Infinity: 2}"),
              "{\"Infinity\"=I2;\"true\"=I1;}");
  OO_CHECK_EQ(parsed("{\\u0061b: 1}"), "{\"ab\"=I1;}");
  OO_CHECK_EQ(parsed("{caf\xc3\xa9: 1}"), "{\"caf\\u00E9\"=I1;}");
}

OO_TEST(badKeysAreErrors) {
  OO_CHECK_EQ(parsed("{1a: 1}"),
              "ERROR JSON5 parse failed at line 1, column 2 - expected a key");
  OO_CHECK_EQ(parsed("{a-b: 1}"),
              "ERROR JSON5 parse failed at line 1, column 3 - expected ':' "
              "after a key");
  OO_CHECK_EQ(parsed("{a 1}"),
              "ERROR JSON5 parse failed at line 1, column 4 - expected ':' "
              "after a key");
  OO_CHECK_EQ(parsed("{\\x61: 1}"),
              "ERROR JSON5 parse failed at line 1, column 2 - only \\u "
              "escapes are allowed in a key");
}

// --- strings -----------------------------------------------------------------

OO_TEST(stringEscapes) {
  OO_CHECK_EQ(parsed(R"("\b\f\n\r\t\v\0")"),
              "S\"\\u0008\\u000C\\u000A\\u000D\\u0009\\u000B\\u0000\"");
  OO_CHECK_EQ(parsed(R"("\'\"\\\/\q")"), "S\"'\\\"\\\\/q\"");
  OO_CHECK_EQ(parsed(R"("\x41\u00e9")"), "S\"A\\u00E9\"");
  OO_CHECK_EQ(parsed(R"('it\'s "quoted"')"), "S\"it's \\\"quoted\\\"\"");
}

OO_TEST(surrogateEscapesPairOrStayAlone) {
  OO_CHECK_EQ(parsed(R"("\uD83D\uDE00")"), "S\"\\uD83D\\uDE00\"");
  OO_CHECK_EQ(parsed(R"("\uD83D")"), "S\"\\uD83D\"");
  OO_CHECK_EQ(parsed(R"("\uDE00\uD83D")"), "S\"\\uDE00\\uD83D\"");
  OO_CHECK_EQ(parsed(R"("\uD83Dx")"), "S\"\\uD83Dx\"");
}

OO_TEST(lineContinuations) {
  OO_CHECK_EQ(parsed("\"a\\\nb\""), "S\"ab\"");
  OO_CHECK_EQ(parsed("\"a\\\r\nb\""), "S\"ab\"");
  OO_CHECK_EQ(parsed("\"a\\\rb\""), "S\"ab\"");
  OO_CHECK_EQ(parsed("\"a\\\xe2\x80\xa8" "b\""), "S\"ab\"");
  // U+2028 and U+2029 may appear unescaped.
  OO_CHECK_EQ(parsed("\"a\xe2\x80\xa9" "b\""), "S\"a\\u2029b\"");
}

OO_TEST(badStringsAreErrors) {
  OO_CHECK_EQ(parsed("\"a\nb\""),
              "ERROR JSON5 parse failed at line 1, column 3 - line break in a "
              "string (use \\n, or \\ at the end of the line)");
  OO_CHECK_EQ(parsed("\"abc"),
              "ERROR JSON5 parse failed at line 1, column 1 - unterminated "
              "string");
  OO_CHECK_EQ(parsed(R"("\1")"),
              "ERROR JSON5 parse failed at line 1, column 2 - decimal escapes "
              "are not allowed");
  OO_CHECK_EQ(parsed(R"("\01")"),
              "ERROR JSON5 parse failed at line 1, column 2 - \\0 followed by "
              "a digit");
  OO_CHECK_EQ(parsed(R"("\x4")"),
              "ERROR JSON5 parse failed at line 1, column 2 - \\x needs two "
              "hex digits");
  OO_CHECK_EQ(parsed(R"("\u12g4")"),
              "ERROR JSON5 parse failed at line 1, column 2 - \\u needs four "
              "hex digits");
  OO_CHECK_EQ(parsed("\"\xff\""),
              "ERROR JSON5 parse failed at line 1, column 2 - invalid UTF-8");
  OO_CHECK_EQ(parsed("\"\xc0\xaf\""),  // overlong '/'
              "ERROR JSON5 parse failed at line 1, column 2 - invalid UTF-8");
  OO_CHECK_EQ(parsed("\"\xed\xa0\x80\""),  // an encoded surrogate
              "ERROR JSON5 parse failed at line 1, column 2 - invalid UTF-8");
}

// --- numbers -----------------------------------------------------------------

OO_TEST(integersAreSignedThenUnsignedThenReal) {
  OO_CHECK_EQ(parsed("9223372036854775807"), "I9223372036854775807");
  OO_CHECK_EQ(parsed("-9223372036854775808"), "I-9223372036854775808");
  OO_CHECK_EQ(parsed("9223372036854775808"), "U9223372036854775808");
  OO_CHECK_EQ(parsed("18446744073709551615"), "U18446744073709551615");
  OO_CHECK_EQ(parsed("18446744073709551616"), "R1.8446744073709552e+19");
  OO_CHECK_EQ(parsed("-9223372036854775809"), "R-9.2233720368547758e+18");
  OO_CHECK_EQ(parsed("-0"), "I0");
}

OO_TEST(hexIntegers) {
  OO_CHECK_EQ(parsed("0x1F"), "I31");
  OO_CHECK_EQ(parsed("0XaB"), "I171");
  OO_CHECK_EQ(parsed("-0x10"), "I-16");
  OO_CHECK_EQ(parsed("0xFFFFFFFFFFFFFFFF"), "U18446744073709551615");
  OO_CHECK_EQ(parsed("0x10000000000000000"), "R1.8446744073709552e+19");
}

OO_TEST(realsTakeEveryJSON5Spelling) {
  OO_CHECK_EQ(parsed(".5"), "R0.5");
  OO_CHECK_EQ(parsed("5."), "R5");
  OO_CHECK_EQ(parsed("-.5"), "R-0.5");
  OO_CHECK_EQ(parsed("1e3"), "R1000");
  OO_CHECK_EQ(parsed("1E-2"), "R0.01");
  OO_CHECK_EQ(parsed("2.5e+1"), "R25");
  OO_CHECK_EQ(parsed("0.0"), "R0");
  OO_CHECK_EQ(parsed("1e400"), "Rinf");
  OO_CHECK_EQ(parsed("1e-400"), "R0");
}

OO_TEST(infinityAndNaN) {
  std::expected<oo::PList, oo::PListError> r = oo::parseJSON5PList("Infinity");
  OO_CHECK(r && r->isReal() && std::isinf(r->doubleValue()) &&
           r->doubleValue() > 0);
  r = oo::parseJSON5PList("-Infinity");
  OO_CHECK(r && r->isReal() && std::isinf(r->doubleValue()) &&
           r->doubleValue() < 0);
  r = oo::parseJSON5PList("+Infinity");
  OO_CHECK(r && r->isReal() && r->doubleValue() > 0);
  r = oo::parseJSON5PList("NaN");
  OO_CHECK(r && r->isReal() && std::isnan(r->doubleValue()));
  r = oo::parseJSON5PList("-NaN");
  OO_CHECK(r && r->isReal() && std::isnan(r->doubleValue()));
}

OO_TEST(badNumbersAreErrors) {
  OO_CHECK_EQ(parsed("01"),
              "ERROR JSON5 parse failed at line 1, column 2 - leading zeros "
              "are not allowed");
  OO_CHECK_EQ(parsed("."),
              "ERROR JSON5 parse failed at line 1, column 1 - not a number");
  OO_CHECK_EQ(parsed("-"),
              "ERROR JSON5 parse failed at line 1, column 1 - not a number");
  OO_CHECK_EQ(parsed("1e"),
              "ERROR JSON5 parse failed at line 1, column 3 - an exponent "
              "needs digits");
  OO_CHECK_EQ(parsed("0x"),
              "ERROR JSON5 parse failed at line 1, column 3 - 0x needs hex "
              "digits");
  OO_CHECK_EQ(parsed("12abc"),
              "ERROR JSON5 parse failed at line 1, column 3 - unexpected "
              "character after a number");
  OO_CHECK_EQ(parsed("Infinit"),
              "ERROR JSON5 parse failed at line 1, column 1 - not a number");
}

// --- whitespace and comments -------------------------------------------------

OO_TEST(whitespaceAndCommentsAreSkipped) {
  OO_CHECK_EQ(parsed("// first\n{ /* inner */ a /* x */ : // tail\n 1 }\n// end"),
              "{\"a\"=I1;}");
  OO_CHECK_EQ(parsed("\t\v\f \xc2\xa0\xef\xbb\xbf\xe2\x80\x83\xe3\x80\x80 1"),
              "I1");
  OO_CHECK_EQ(parsed("1 // no newline at the end"), "I1");
  OO_CHECK_EQ(parsed("\xef\xbb\xbf{}"), "{}");  // a leading BOM
}

OO_TEST(badDocumentsAreErrors) {
  OO_CHECK_EQ(parsed(""),
              "ERROR JSON5 parse failed at line 1, column 1 - empty document");
  OO_CHECK_EQ(parsed(" // only a comment"),
              "ERROR JSON5 parse failed at line 1, column 19 - empty document");
  OO_CHECK_EQ(parsed("1 2"),
              "ERROR JSON5 parse failed at line 1, column 3 - extra data "
              "after the value");
  OO_CHECK_EQ(parsed("/* open"),
              "ERROR JSON5 parse failed at line 1, column 1 - unterminated "
              "comment");
  OO_CHECK_EQ(parsed("["),
              "ERROR JSON5 parse failed at line 1, column 2 - unterminated "
              "array");
  OO_CHECK_EQ(parsed("{"),
              "ERROR JSON5 parse failed at line 1, column 2 - unterminated "
              "object");
  OO_CHECK_EQ(parsed("[1, 2"),
              "ERROR JSON5 parse failed at line 1, column 6 - unterminated "
              "array");
  OO_CHECK_EQ(parsed("[1 2]"),
              "ERROR JSON5 parse failed at line 1, column 4 - expected ',' or "
              "']'");
  OO_CHECK_EQ(parsed("{a: 1"),
              "ERROR JSON5 parse failed at line 1, column 6 - unterminated "
              "object");
  OO_CHECK_EQ(parsed("[,]"),
              "ERROR JSON5 parse failed at line 1, column 2 - unexpected "
              "character");
  OO_CHECK_EQ(parsed("[1,,]"),
              "ERROR JSON5 parse failed at line 1, column 4 - unexpected "
              "character");
  OO_CHECK_EQ(parsed("[undefined]"),
              "ERROR JSON5 parse failed at line 1, column 2 - unexpected "
              "character");
}

OO_TEST(errorsCountLinesAndColumns) {
  OO_CHECK_EQ(parsed("{\n  a: 1,\n  b: ?\n}"),
              "ERROR JSON5 parse failed at line 3, column 6 - unexpected "
              "character");
  OO_CHECK_EQ(parsed("{\r\n  a: ?}"),
              "ERROR JSON5 parse failed at line 2, column 6 - unexpected "
              "character");
  OO_CHECK_EQ(parsed("[\r1,\xe2\x80\xa8?]"),
              "ERROR JSON5 parse failed at line 3, column 1 - unexpected "
              "character");
}

OO_TEST(nestingIsLimited) {
  const std::string ok = std::string(512, '[') + std::string(512, ']');
  OO_CHECK(oo::parseJSON5PList(ok).has_value());
  const std::string deep = std::string(513, '[') + std::string(513, ']');
  OO_CHECK_EQ(parsed(deep),
              "ERROR JSON5 parse failed at line 1, column 513 - nesting deeper "
              "than 512 levels");
  const std::string hostile(1000000, '{');
  OO_CHECK(!oo::parseJSON5PList(hostile).has_value());
}

// --- $data and $date ---------------------------------------------------------

OO_TEST(dataAndDateObjects) {
  OO_CHECK_EQ(parsed("{$data: \"AAEC\"}"), "D<000102>");
  OO_CHECK_EQ(parsed("{\"$data\": \"\"}"), "D<>");
  OO_CHECK_EQ(parsed("{$data: \"AA==\"}"), "D<00>");
  OO_CHECK_EQ(parsed("{$data: \"AAE=\"}"), "D<0001>");
  OO_CHECK_EQ(parsed("{$date: \"2001-01-01T00:00:00Z\"}"), "T0.000");
  OO_CHECK_EQ(parsed("{$date: \"2001-01-02T00:00:01Z\"}"), "T86401.000");
  OO_CHECK_EQ(parsed("{$date: \"2000-12-31T23:59:59Z\"}"), "T-1.000");
  // 1154 days and 12 hours.
  OO_CHECK_EQ(parsed("{$date: \"2004-02-29T12:00:00Z\"}"), "T99748800.000");
}

OO_TEST(datesAgreeWithTheXMLReadersParser) {
  for (const char* s : {"2001-01-01T00:00:00Z", "1970-01-01T00:00:00Z",
                        "2004-02-29T12:34:56Z", "2100-12-31T23:59:59Z",
                        "1600-03-01T00:00:00Z", "0001-01-01T00:00:00Z"}) {
    const std::optional<double> mine = oo::plist_detail::parseJSON5Date(s);
    const std::optional<double> xml = oo::plist_detail::parseCalendarDate(s, true);
    OO_CHECK(mine.has_value() && xml.has_value() && *mine == *xml);
  }
}

OO_TEST(dataAndDateNeedExactlyOneStringEntry) {
  OO_CHECK_EQ(parsed("{$data: 1}"), "{\"$data\"=I1;}");
  OO_CHECK_EQ(parsed("{$data: \"AA==\", b: 1}"),
              "{\"$data\"=S\"AA==\";\"b\"=I1;}");
  OO_CHECK_EQ(parsed("{$other: \"x\"}"), "{\"$other\"=S\"x\";}");
}

OO_TEST(badDataAndDatesAreErrors) {
  OO_CHECK_EQ(parsed("{$data: \"AAE\"}"),
              "ERROR JSON5 parse failed at line 1, column 9 - $data is not "
              "base64");
  OO_CHECK_EQ(parsed("{$data: \"AA=E\"}"),
              "ERROR JSON5 parse failed at line 1, column 9 - $data is not "
              "base64");
  OO_CHECK_EQ(parsed("{$data: \"A Q==\"}"),
              "ERROR JSON5 parse failed at line 1, column 9 - $data is not "
              "base64");
  OO_CHECK_EQ(parsed("{$date: \"2001-02-29T00:00:00Z\"}"),
              "ERROR JSON5 parse failed at line 1, column 9 - $date is not "
              "YYYY-MM-DDTHH:MM:SSZ");
  OO_CHECK_EQ(parsed("{$date: \"2001-01-01 00:00:00 +0000\"}"),
              "ERROR JSON5 parse failed at line 1, column 9 - $date is not "
              "YYYY-MM-DDTHH:MM:SSZ");
}

// --- the writer --------------------------------------------------------------

OO_TEST(writerLayout) {
  OO_CHECK_EQ(written(dict({{"a", 1},
                            {"0 0", oo::PList::Array{oo::PList(true), 2.5}},
                            {"e", oo::PList::Dict{}}})),
              "{\n"
              "\t\"0 0\": [\n"
              "\t\ttrue,\n"
              "\t\t2.5,\n"
              "\t],\n"
              "\ta: 1,\n"
              "\te: {},\n"
              "}\n");
  OO_CHECK_EQ(written(oo::PList(oo::PList::Array{})), "[]\n");
}

OO_TEST(writerScalars) {
  OO_CHECK_EQ(written(oo::PList(1.0)), "1.0\n");
  OO_CHECK_EQ(written(oo::PList(0.1)), "0.1\n");
  OO_CHECK_EQ(written(oo::PList(1e300)), "1e+300\n");
  OO_CHECK_EQ(written(oo::PList(-std::numeric_limits<double>::infinity())),
              "-Infinity\n");
  OO_CHECK_EQ(written(oo::PList(std::numeric_limits<double>::quiet_NaN())),
              "NaN\n");
  OO_CHECK_EQ(written(oo::PList::unsignedInteger(18446744073709551615ULL)),
              "18446744073709551615\n");
  OO_CHECK_EQ(written(oo::PList(std::string("q\"\\\n\x01\xc3\xa9"))),
              "\"q\\\"\\\\\\n\\x01\xc3\xa9\"\n");
  OO_CHECK_EQ(written(oo::PList(std::string("\xed\xa0\xbd"))), "\"\\uD83D\"\n");
  const std::uint8_t bytes[] = {0, 1, 2};
  OO_CHECK_EQ(written(oo::PList(oo::Data(bytes, 3))), "{$data: \"AAEC\"}\n");
  OO_CHECK_EQ(written(oo::PList(oo::PList::Date{86401.0})),
              "{$date: \"2001-01-02T00:00:01Z\"}\n");
}

OO_TEST(writerRefusesWhatCannotReadBack) {
  OO_CHECK_EQ(written(oo::PList()), "ERROR null cannot be written as JSON5");
  OO_CHECK_EQ(written(dict({{"$data", "AA=="}})),
              "ERROR a dict whose only entry is a string under \"$data\" "
              "cannot be written as JSON5 (it would read back as data)");
  OO_CHECK_EQ(written(dict({{"$date", "x"}})),
              "ERROR a dict whose only entry is a string under \"$date\" "
              "cannot be written as JSON5 (it would read back as a date)");
  OO_CHECK(roundTrips(dict({{"$data", 1}})));
  OO_CHECK(roundTrips(dict({{"$data", "AA=="}, {"b", 1}})));
}

OO_TEST(writerRoundTrips) {
  const std::uint8_t bytes[] = {0xde, 0xad, 0xbe, 0xef, 0x00};
  const oo::PList tree = dict({
      {"strings", oo::PList::Array{"", "plain", "with space", "tab\there",
                                   "caf\xc3\xa9", "\xf0\x9f\x98\x80",
                                   "\xed\xb8\x80 lone low", "nul\0byte"sv}},
      {"numbers", oo::PList::Array{0, -1, oo::PList::signedInteger(INT64_MIN),
                                   oo::PList::unsignedInteger(UINT64_MAX), 0.5,
                                   -2.0, 1e-300, 123456789.125}},
      {"flags", oo::PList::Array{true, false}},
      {"blob", oo::Data(bytes, sizeof bytes)},
      {"when", oo::PList::Date{-31536000.0}},
      {"nested", dict({{"0 0", dict({{"x-y", oo::PList::Array{}}})}})},
      {"$keys", dict({{"$data", 5}, {"$date", oo::PList::Array{}}})},
  });
  OO_CHECK(roundTrips(tree));
}

// --- <name>.json5 beside <name>.plist ----------------------------------------

namespace {

struct FakeFiles {
  std::map<std::string, std::string> files;
  std::vector<std::string> reads;

  std::optional<oo::Data> operator()(const std::string& path) {
    reads.push_back(path);
    auto it = files.find(path);
    if (it == files.end()) return std::nullopt;
    return oo::Data(it->second.data(), it->second.size());
  }
};

oo::PListFileLoad load(FakeFiles& fs, const std::string& path) {
  return oo::loadPListFile(path, [&](const std::string& p) { return fs(p); });
}

}  // namespace

OO_TEST(json5PathForOnlyRenamesPlists) {
  OO_CHECK(oo::json5PathFor("Config/shipdata.plist") ==
           std::optional<std::string>("Config/shipdata.json5"));
  OO_CHECK(oo::json5PathFor("A.OXZ/Config/X.PList") ==
           std::optional<std::string>("A.OXZ/Config/X.json5"));
  OO_CHECK(!oo::json5PathFor("save.oolite-save").has_value());
  OO_CHECK(!oo::json5PathFor("shipdata.json5").has_value());
  OO_CHECK(!oo::json5PathFor(".plist").has_value());
  OO_CHECK(!oo::json5PathFor("dir/.plist").has_value());
}

OO_TEST(aPlistAloneIsReadAsBefore) {
  FakeFiles fs{{{"c/e.plist", "( a )"}}, {}};
  oo::PListFileLoad r = load(fs, "c/e.plist");
  OO_CHECK(!r.isJSON5);
  OO_CHECK_EQ(r.source, "c/e.plist");
  OO_CHECK(r.plistData.has_value() && r.plistData->length() == 5);
  OO_CHECK(fs.reads == (std::vector<std::string>{"c/e.json5", "c/e.plist"}));
}

OO_TEST(aJSON5FileStandsInForItsPlist) {
  FakeFiles fs{{{"c/e.json5", "[1]"}}, {}};
  oo::PListFileLoad r = load(fs, "c/e.plist");
  OO_CHECK(r.isJSON5);
  OO_CHECK(!r.shadowedPlist);
  OO_CHECK_EQ(r.source, "c/e.json5");
  OO_CHECK_EQ(oo_test::dump(r.plist), "[I1]");
  OO_CHECK(!r.plistData.has_value());
}

OO_TEST(bothPresentReadsTheJSON5AndSaysSo) {
  FakeFiles fs{{{"c/e.json5", "[1]"}, {"c/e.plist", "( 2 )"}}, {}};
  oo::PListFileLoad r = load(fs, "c/e.plist");
  OO_CHECK(r.isJSON5);
  OO_CHECK(r.shadowedPlist);
  OO_CHECK_EQ(oo_test::dump(r.plist), "[I1]");
}

OO_TEST(aBadJSON5FileIsAnErrorNotAFallback) {
  FakeFiles fs{{{"c/e.json5", "[1"}, {"c/e.plist", "( 2 )"}}, {}};
  oo::PListFileLoad r = load(fs, "c/e.plist");
  OO_CHECK(r.isJSON5);
  OO_CHECK(r.plist.isNull());
  OO_CHECK(r.error.has_value() &&
           r.error->message ==
               "JSON5 parse failed at line 1, column 3 - unterminated array");
  OO_CHECK(!r.plistData.has_value());
}

OO_TEST(otherFilesAreReadAsTheyAre) {
  FakeFiles fs{{{"Saved/x.oolite-save", "{}"}}, {}};
  oo::PListFileLoad r = load(fs, "Saved/x.oolite-save");
  OO_CHECK(!r.isJSON5);
  OO_CHECK(r.plistData.has_value());
  OO_CHECK(fs.reads == (std::vector<std::string>{"Saved/x.oolite-save"}));
  FakeFiles none;
  OO_CHECK_EQ(load(none, "c/missing.plist").source, "");
}

OO_TEST(aJSON5PathIsReadAsJSON5) {
  OO_CHECK(oo::isJSON5Path("Config/shipdata.json5"));
  OO_CHECK(oo::isJSON5Path("AIs/X.JSON5"));
  OO_CHECK(!oo::isJSON5Path("Config/shipdata.plist"));
  OO_CHECK(!oo::isJSON5Path(".json5"));
  OO_CHECK(!oo::isJSON5Path("dir/.json5"));

  FakeFiles fs{{{"AIs/n.json5", "{GLOBAL: {}}"}, {"AIs/bad.json5", "{"}}, {}};
  oo::PListFileLoad r = load(fs, "AIs/n.json5");
  OO_CHECK(r.isJSON5);
  OO_CHECK(!r.shadowedPlist);
  OO_CHECK_EQ(r.source, "AIs/n.json5");
  OO_CHECK_EQ(oo_test::dump(r.plist), "{\"GLOBAL\"={};}");
  OO_CHECK(fs.reads == (std::vector<std::string>{"AIs/n.json5"}));

  oo::PListFileLoad bad = load(fs, "AIs/bad.json5");
  OO_CHECK(bad.isJSON5);
  OO_CHECK(bad.error.has_value());
  OO_CHECK(!bad.plistData.has_value());

  oo::PListFileLoad missing = load(fs, "AIs/missing.json5");
  OO_CHECK(!missing.isJSON5);
  OO_CHECK_EQ(missing.source, "");
  OO_CHECK(!missing.plistData.has_value());
}

OO_TEST(existingPListPathPrefersTheJSON5) {
  const std::set<std::string> files = {"a/both.json5", "a/both.plist",
                                       "a/old.plist", "a/new.json5",
                                       "a/model.dat"};
  auto exists = [&](const std::string& p) { return files.contains(p); };
  OO_CHECK(oo::existingPListPath("a/both.plist", exists) ==
           std::optional<std::string>("a/both.json5"));
  OO_CHECK(oo::existingPListPath("a/old.plist", exists) ==
           std::optional<std::string>("a/old.plist"));
  OO_CHECK(oo::existingPListPath("a/new.plist", exists) ==
           std::optional<std::string>("a/new.json5"));
  OO_CHECK(oo::existingPListPath("a/new.json5", exists) ==
           std::optional<std::string>("a/new.json5"));
  OO_CHECK(oo::existingPListPath("a/model.dat", exists) ==
           std::optional<std::string>("a/model.dat"));
  OO_CHECK(!oo::existingPListPath("a/none.plist", exists).has_value());
  OO_CHECK(!oo::existingPListPath("a/model.json5", exists).has_value());
}

OO_TEST(parsePListFileReadsEitherFormat) {
  FakeFiles fs{{{"c/old.plist", "{ a = 1; }"},
                {"c/new.json5", "{a: 1}"},
                {"c/bad.json5", "{a: }"},
                {"c/bad.plist", "{ a = ; }"}},
               {}};
  auto parse = [&](const std::string& path) {
    return oo::parsePListFile(path,
                              [&](const std::string& p) { return fs(p); });
  };
  std::expected<oo::PList, oo::PListError> old = parse("c/old.plist");
  OO_CHECK(old.has_value() && oo_test::dump(*old) == "{\"a\"=S\"1\";}");
  std::expected<oo::PList, oo::PListError> json5 = parse("c/new.plist");
  OO_CHECK(json5.has_value() && oo_test::dump(*json5) == "{\"a\"=I1;}");
  std::expected<oo::PList, oo::PListError> none = parse("c/none.plist");
  OO_CHECK(none.has_value() && none->isNull());
  OO_CHECK(!parse("c/bad.plist").has_value());
  OO_CHECK(!parse("c/bad.json5").has_value());
}

OO_TEST_MAIN()
