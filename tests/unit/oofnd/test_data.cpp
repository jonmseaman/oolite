/*	test_data.cpp
        Unit tests for oofnd/Data.hpp: oo::Data, the replacement
   for NSData and NSMutableData, checked against the NSData behaviour its banner
   maps (construction, bytes and length, appending, zero-filled growth,
   subranges, equality by content).
*/

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "oofnd/Data.hpp"

TEST(data, emptyDataHasNoBytesButAValidPointer) {
  const oo::Data d;
  EXPECT_TRUE(d.empty());
  EXPECT_EQ(d.length(), 0u);
  EXPECT_TRUE(d.bytes() != nullptr);
  EXPECT_TRUE(d.span().empty());
  EXPECT_TRUE(d.stringView().empty());
  unsigned char dst[1] = {7};
  std::memcpy(dst, d.bytes(), d.length());
  EXPECT_EQ(dst[0], 7);
}

TEST(data, dataWithBytesCopiesThem) {
  unsigned char src[] = {1, 2, 3, 0, 255};
  oo::Data d(src, sizeof src);
  src[0] = 9;
  EXPECT_EQ(d.length(), 5u);
  EXPECT_EQ(d.bytes()[0], 1);
  EXPECT_EQ(d.bytes()[3], 0);
  EXPECT_EQ(d.bytes()[4], 255);
}

TEST(data, nullOrZeroLengthBytesMakeEmptyData) {
  EXPECT_TRUE(oo::Data(nullptr, 0).empty());
  EXPECT_TRUE(oo::Data(nullptr, 12).empty());
  const char c = 'x';
  EXPECT_TRUE(oo::Data(&c, 0).empty());
}

TEST(data, fromStringIsTheUtf8BytesWithoutTerminator) {
  const oo::Data d = oo::Data::fromString("caf\xc3\xa9");
  EXPECT_EQ(d.length(), 5u);
  EXPECT_EQ(d.bytes()[3], 0xc3);
  EXPECT_EQ(d.stringView(), std::string_view("caf\xc3\xa9"));
  EXPECT_EQ(d.toString(), std::string("caf\xc3\xa9"));
}

TEST(data, embeddedNulsSurvive) {
  const std::string s("a\0b", 3);
  const oo::Data d = oo::Data::fromString(s);
  EXPECT_EQ(d.length(), 3u);
  EXPECT_EQ(d.toString(), s);
}

TEST(data, appendBytesAndData) {
  oo::Data d = oo::Data::fromString("ab");
  d.append("cd", 2);
  d.append(oo::Data::fromString("ef"));
  EXPECT_EQ(d.toString(), std::string("abcdef"));
  d.append(nullptr, 3);
  EXPECT_EQ(d.length(), 6u);
}

TEST(data, appendingToItselfDoublesTheContents) {
  oo::Data d = oo::Data::fromString("xyz");
  d.append(d);
  EXPECT_EQ(d.toString(), std::string("xyzxyz"));
}

TEST(data, setLengthGrowsWithZerosAndTruncates) {
  oo::Data d = oo::Data::fromString("abc");
  d.setLength(5);
  EXPECT_EQ(d.length(), 5u);
  EXPECT_EQ(d.bytes()[3], 0);
  EXPECT_EQ(d.bytes()[4], 0);
  d.setLength(1);
  EXPECT_EQ(d.toString(), std::string("a"));
}

TEST(data, mutableBytesWriteThrough) {
  oo::Data d = oo::Data::fromString("abc");
  d.mutableBytes()[1] = 'X';
  EXPECT_EQ(d.toString(), std::string("aXc"));
  oo::Data empty;
  EXPECT_TRUE(empty.mutableBytes() == nullptr);
}

TEST(data, subdataIsTheRangeClampedToTheBytes) {
  const oo::Data d = oo::Data::fromString("0123456789");
  EXPECT_EQ(d.subdata(2, 3).toString(), std::string("234"));
  EXPECT_EQ(d.subdata(0, 10), d);
  EXPECT_EQ(d.subdata(8, 100).toString(), std::string("89"));
  EXPECT_TRUE(d.subdata(10, 1).empty());
  EXPECT_TRUE(d.subdata(50, 1).empty());
  EXPECT_TRUE(d.subdata(3, 0).empty());
}

TEST(data, equalityIsByContent) {
  EXPECT_TRUE(oo::Data::fromString("abc") == oo::Data::fromString("abc"));
  EXPECT_TRUE(!(oo::Data::fromString("abc") == oo::Data::fromString("abd")));
  EXPECT_TRUE(!(oo::Data::fromString("abc") == oo::Data::fromString("ab")));
  EXPECT_TRUE(oo::Data() == oo::Data(nullptr, 0));
}

TEST(data, copiesAreIndependentAndMovesTransfer) {
  oo::Data a = oo::Data::fromString("abc");
  oo::Data b = a;
  b.mutableBytes()[0] = 'z';
  EXPECT_EQ(a.toString(), std::string("abc"));
  EXPECT_EQ(b.toString(), std::string("zbc"));

  const std::uint8_t* before = a.bytes();
  oo::Data c = std::move(a);
  EXPECT_TRUE(c.bytes() == before);
  EXPECT_EQ(c.toString(), std::string("abc"));
}

TEST(data, vectorRoundTrip) {
  std::vector<std::uint8_t> v{4, 5, 6};
  const std::uint8_t* raw = v.data();
  oo::Data d(std::move(v));
  EXPECT_TRUE(d.bytes() == raw);
  EXPECT_EQ(d.vector().size(), 3u);
  std::vector<std::uint8_t> out = std::move(d).vector();
  EXPECT_EQ(out.size(), 3u);
  EXPECT_EQ(out[2], 6);
}
