/*	test_OOEncodingConverter.mm
        Unit tests for OOEncodingConverter (src/Core/OOEncodingConverter.h).

        It pins what the converter computes: the encoding
   read from a font property list (an unknown or missing name is no encoding),
   the substitutions (in key order; a value that is not a string substitutes "";
   a substitutions entry that is not a dictionary is ignored), conversion with
   and without an encoding, a repeated conversion (the cache), the description
   and StringFromEncoding(). The calls here are the C++ class's. Run:
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "EncodingConverter.h"

/*	The game's OOStringParsing.mm reaches the JavaScript engine, so it is
   not linked. The one function of it that OOCache names (in its DEBUG_GRAPHVIZ
   dump, which no test calls) is defined here instead.
*/
std::string EscapedGraphVizString(const std::string& string) {
  return string;
}

namespace {

std::string Bytes(const oo::Data& data) {
  return std::string(reinterpret_cast<const char*>(data.bytes()),
                     data.length());
}

oo::PList FontPList(const oo::PList& encoding, const oo::PList& substitutions) {
  oo::PList::Dict font;
  if (!encoding.isNull()) font.emplace("encoding", encoding);
  if (!substitutions.isNull()) font.emplace("substitutions", substitutions);
  return oo::PList(std::move(font));
}

oo::PList Substitutions(
    std::initializer_list<std::pair<const char*, oo::PList>> entries) {
  oo::PList::Dict result;
  for (const auto& [key, value] : entries) result.emplace(key, value);
  return oo::PList(std::move(result));
}

std::shared_ptr<oo::EncodingConverter> ConverterForFont(const oo::PList& font) {
  return oo::makeShared<oo::EncodingConverter>(font);
}

}  // namespace

TEST(OOEncodingConverter, fontPListLatin1) {
  {
    std::shared_ptr<oo::EncodingConverter> converter = ConverterForFont(
        FontPList("windows-latin-1",
                  Substitutions({{"\xE2\x98\x85", "\b"}, {"q", "Q"}})));
    EXPECT_TRUE(converter != nullptr);
    EXPECT_TRUE(converter->encoding() == std::optional<oo::str::Encoding>(
                                          oo::str::Encoding::kWindowsCP1252));
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string("abc"));
    EXPECT_EQ(Bytes(converter->convertString("\xE2\x98\x85 q")),
                std::string("\b Q"));
    // The second conversion of a string is answered from the cache, with the
    // same bytes.
    EXPECT_EQ(Bytes(converter->convertString("\xE2\x98\x85 q")),
                std::string("\b Q"));
    // U+00E9 is 0xE9 in code page 1252.
    EXPECT_EQ(Bytes(converter->convertString("caf\xC3\xA9")),
                std::string("caf\xE9"));
    EXPECT_EQ(Bytes(converter->convertString("")), std::string());
    EXPECT_TRUE(converter->descriptionComponents() ==
             std::optional<std::string>("encoding: 12"));
  }
}

TEST(OOEncodingConverter, substitutionOrderAndValues) {
  {
    // Applied in key order: "ab" before "b".
    std::shared_ptr<oo::EncodingConverter> converter = ConverterForFont(
        FontPList("windows-latin-1", Substitutions({{"b", "Y"}, {"ab", "X"}})));
    EXPECT_EQ(Bytes(converter->convertString("abb")), std::string("XY"));

    // A value that is not a string substitutes "".
    converter = ConverterForFont(
        FontPList("windows-latin-1", Substitutions({{"x", oo::PList(5)}})));
    EXPECT_EQ(Bytes(converter->convertString("axb")), std::string("ab"));

    // Substitutions that are not a dictionary are ignored.
    converter = ConverterForFont(FontPList("windows-latin-1", oo::PList("x")));
    EXPECT_EQ(Bytes(converter->convertString("axb")), std::string("axb"));
  }
}

TEST(OOEncodingConverter, otherEncodings) {
  {
    std::shared_ptr<oo::EncodingConverter> converter =
        ConverterForFont(FontPList("windows-cyrillic", oo::PList()));
    EXPECT_TRUE(converter->encoding() == std::optional<oo::str::Encoding>(
                                          oo::str::Encoding::kWindowsCP1251));
    // U+0416 (Zhe) is 0xC6 in code page 1251.
    EXPECT_EQ(Bytes(converter->convertString("\xD0\x96")),
                std::string("\xC6"));
    EXPECT_TRUE(converter->descriptionComponents() ==
             std::optional<std::string>("encoding: 11"));

    converter = oo::makeShared<oo::EncodingConverter>(
        oo::str::Encoding::kWindowsCP1253, Substitutions({{"a", "b"}}));
    EXPECT_TRUE(converter->encoding() == std::optional<oo::str::Encoding>(
                                          oo::str::Encoding::kWindowsCP1253));
    EXPECT_EQ(Bytes(converter->convertString("aa")), std::string("bb"));
    EXPECT_TRUE(converter->descriptionComponents() ==
             std::optional<std::string>("encoding: 13"));

    converter = oo::makeShared<oo::EncodingConverter>(
        oo::str::Encoding::kWindowsCP1254, oo::PList());
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string("abc"));
  }
}

TEST(OOEncodingConverter, noEncoding) {
  {
    // An unknown encoding name: no encoding, and every conversion is empty.
    std::shared_ptr<oo::EncodingConverter> converter =
        ConverterForFont(FontPList("klingon", Substitutions({{"a", "b"}})));
    EXPECT_TRUE(converter != nullptr);
    EXPECT_TRUE(!converter->encoding().has_value());
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string());
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string());
    // (oo::kNotFound's low 32 bits)
    EXPECT_TRUE(converter->descriptionComponents() ==
             std::optional<std::string>("encoding: 4294967295"));

    // No encoding key, and an encoding that is not a string.
    converter = ConverterForFont(FontPList(oo::PList(), oo::PList()));
    EXPECT_TRUE(!converter->encoding().has_value());
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string());
    converter = ConverterForFont(FontPList(oo::PList(12), oo::PList()));
    EXPECT_TRUE(!converter->encoding().has_value());

    converter = oo::makeShared<oo::EncodingConverter>(std::nullopt, oo::PList());
    EXPECT_TRUE(!converter->encoding().has_value());
    EXPECT_EQ(Bytes(converter->convertString("abc")), std::string());
  }
}

TEST(OOEncodingConverter, stringFromEncoding) {
  EXPECT_EQ(std::string(StringFromEncoding(12)),
              std::string("windows-latin-1"));
  EXPECT_EQ(std::string(StringFromEncoding(15)),
              std::string("windows-latin-2"));
  EXPECT_EQ(std::string(StringFromEncoding(11)),
              std::string("windows-cyrillic"));
  EXPECT_EQ(std::string(StringFromEncoding(13)),
              std::string("windows-greek"));
  EXPECT_EQ(std::string(StringFromEncoding(14)),
              std::string("windows-turkish"));
  EXPECT_TRUE(StringFromEncoding(1) == nullptr);
}
