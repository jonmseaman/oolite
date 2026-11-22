/*	test_plist_xml.cpp
        The XML plist reader: oo::changeDTDIfApplicable
   (OOPListParsing.m's ChangeDTDIfApplicable, byte for byte) and
   oo::parseXMLPList (GNUstep's GSXMLPListParser over GSSloppyXMLParser). Parse
   expectations were captured from GNUstep base 1.31.1 through
        OOPropertyListFromData's path (DTD change, then
   NSPropertyListSerialization) with a throwaway harness printing
   plist_dump.hpp's format; the tests suffixed GnustepDiffers pin oofnd's
   documented differences where GNUstep raised an exception or depended on the
   machine's time zone. meson test --suite oofnd-plist
*/

#include <gtest/gtest.h>

#include "oofnd/PListXML.hpp"

#include <string>
#include <string_view>

#include "plist_dump.hpp"

using namespace std::string_view_literals;

namespace {

const std::string kDecl = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
const std::string kAppleDTD =
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
    "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
const std::string kAppleComputerDTD =
    "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" "
    "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
const std::string kGNUstepDTD =
    "<!DOCTYPE plist PUBLIC \"-//GNUstep//DTD plist 0.9//EN\" "
    "\"http://www.gnustep.org/plist-0_9.xml\">";

// A plist document as Apple's tools (and most of Oolite's XML plists) write it.
std::string doc(std::string_view body,
                const std::string& head = kDecl + kAppleDTD) {
  return head + "<plist version=\"1.0\">\n" + std::string(body) +
         "\n</plist>\n";
}

// What OOPropertyListFromData does with XML data.
std::string parsed(std::string_view xml) {
  std::expected<oo::PList, oo::PListError> r =
      oo::parseXMLPList(oo::changeDTDIfApplicable(xml));
  return r ? plist_dump::dump(*r) : "ERROR " + r.error().message;
}

const std::string kFailed = "ERROR failed to parse as XML property list";

}  // namespace

TEST(plist_xml, changeDTDReplacesApplesDoctype) {
  const std::string rest = "<plist version=\"1.0\"><string>a</string></plist>";
  const std::string expected = kDecl + kGNUstepDTD + "\n" + rest;
  EXPECT_EQ(oo::changeDTDIfApplicable(kDecl + kAppleDTD + rest), expected);
  EXPECT_EQ(oo::changeDTDIfApplicable(kDecl + kAppleComputerDTD + rest),
              expected);
  // Whatever whitespace separated the declaration and the DOCTYPE becomes one
  // newline; what follows the DOCTYPE is kept byte for byte.
  EXPECT_EQ(oo::changeDTDIfApplicable(
                  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n \t" +
                  kAppleDTD + rest),
              expected);
  EXPECT_EQ(
      oo::changeDTDIfApplicable(kDecl.substr(0, kDecl.size() - 1) +
                                kAppleDTD.substr(0, kAppleDTD.size() - 1)),
      kDecl + kGNUstepDTD);
}

TEST(plist_xml, changeDTDLeavesEverythingElseAlone) {
  const std::string rest = "<plist><string>a</string></plist>";
  // A different declaration (even an equivalent one), a different DOCTYPE, or
  // none.
  EXPECT_EQ(
      oo::changeDTDIfApplicable("<?xml version=\"1.0\"?>\n" + kAppleDTD + rest),
      "<?xml version=\"1.0\"?>\n" + kAppleDTD + rest);
  EXPECT_EQ(
      oo::changeDTDIfApplicable("<?xml version='1.0' encoding='UTF-8'?>\n" +
                                kAppleDTD + rest),
      "<?xml version='1.0' encoding='UTF-8'?>\n" + kAppleDTD + rest);
  EXPECT_EQ(oo::changeDTDIfApplicable(kDecl + rest), kDecl + rest);
  EXPECT_EQ(oo::changeDTDIfApplicable(kDecl + kGNUstepDTD + rest),
              kDecl + kGNUstepDTD + rest);
  EXPECT_EQ(oo::changeDTDIfApplicable(" " + kDecl + kAppleDTD),
              " " + kDecl + kAppleDTD);
  EXPECT_EQ(oo::changeDTDIfApplicable("{ a = b; }"), "{ a = b; }");
  // Shorter than the declaration plus one byte: untouched without looking.
  EXPECT_EQ(
      oo::changeDTDIfApplicable("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  EXPECT_EQ(oo::changeDTDIfApplicable(""), "");
  // The DOCTYPE must be complete.
  const std::string cut = kDecl + kAppleDTD.substr(0, 40);
  EXPECT_EQ(oo::changeDTDIfApplicable(cut), cut);
}

TEST(plist_xml, changeDTDDoesNotChangeTheResult) {
  const std::string x = doc("<dict><key>a</key><string>b</string></dict>");
  EXPECT_TRUE(oo::parseXMLPList(x).value() ==
           oo::parseXMLPList(oo::changeDTDIfApplicable(x)).value());
  EXPECT_TRUE(
      oo::parseXMLPList(doc("<string>a</string>", kDecl + kAppleComputerDTD))
          .has_value());
}

TEST(plist_xml, allValueTypes) {
  EXPECT_EQ(
      parsed(
          doc("<dict>\n\t<key>a</key>\n\t<string>b</string>\n\t<key>n</"
              "key>\n\t<integer>42</integer>\n"
              "\t<key>r</key>\n\t<real>1.5</real>\n\t<key>t</key>\n\t<true/"
              ">\n\t<key>f</key>\n\t<false/>\n"
              "\t<key>d</key>\n\t<data>AAEC</data>\n\t<key>arr</key>\n"
              "\t<array><string>x</string><array/><dict/></array>\n</dict>")),
      "{\"a\"=S\"b\";\"arr\"=[S\"x\",[],{}];\"d\"=D<000102>;\"f\"=B0;\"n\"=I42;"
      "\"r\"=R1.5;\"t\"=B1;}");
  EXPECT_EQ(
      parsed(doc("<dict><key>a</key><dict><key>b</key><array><integer>1</"
                 "integer></array></dict></dict>")),
      "{\"a\"={\"b\"=[I1];};}");
  EXPECT_EQ(
      parsed(doc(
          "<array><string/><string></string><true></true><false/></array>")),
      "[S\"\",S\"\",B1,B0]");
  EXPECT_EQ(
      parsed("<?xml version=\"1.0\"?><plist><dict><key>a</key><date>2001-01-01 "
             "00:00:00 +0000</date></dict></plist>"),
      "{\"a\"=T0.000;}");
  // Integers are unsigned unless the text starts with '-'.
  EXPECT_TRUE(oo::parseXMLPList(doc("<integer>5</integer>"))
               ->getIf<oo::PList::Integer>()
               ->isUnsigned);
  EXPECT_TRUE(!oo::parseXMLPList(doc("<integer>-5</integer>"))
                ->getIf<oo::PList::Integer>()
                ->isUnsigned);
}

TEST(plist_xml, textIsKeptVerbatimInsideValues) {
  EXPECT_EQ(parsed(doc("<string>  spaced  \n text </string>")),
              "S\"  spaced  \\u000A text \"");
  EXPECT_EQ(parsed(doc("<string>line1\r\nline2</string>")),
              "S\"line1\\u000D\\u000Aline2\"");
  EXPECT_EQ(parsed(doc("<string>\x01\x7f</string>")), "S\"\\u0001\\u007F\"");
  EXPECT_EQ(parsed(doc("<dict><key> k </key><string>v</string></dict>")),
              "{\" k \"=S\"v\";}");
  EXPECT_EQ(parsed(doc("<string>caf\xc3\xa9</string>")), "S\"caf\\u00E9\"");
}

TEST(plist_xml, textBetweenTagsIsTrimmedAndGluedOn) {
  EXPECT_EQ(
      parsed(doc("<dict>junk<key>k</key>more<string>v</string></dict>")),
      "{\"junkk\"=S\"morev\";}");
  EXPECT_EQ(parsed(doc("<array>\n  <string>a</string>\n  x y  \n  "
                         "<string>b</string>\n</array>")),
              "[S\"a\",S\"x yb\"]");
  EXPECT_EQ(parsed(doc("<array> &amp; <string>a</string></array>")),
              "[S\"&a\"]");
  // Each chunk (split at entities and at leading/trailing whitespace) is
  // trimmed separately.
  EXPECT_EQ(
      parsed(
          doc("<dict>\n<key>k</key>\n&#32;x&amp;y <string>v</string></dict>")),
      "{\"k\"=S\"x&yv\";}");
  // Text after the root element is harmless.
  EXPECT_EQ(parsed(kDecl + "<plist><string>x</string></plist> trailing"),
              "S\"x\"");
}

TEST(plist_xml, entities) {
  EXPECT_EQ(
      parsed(doc("<string>a &amp; b &lt;c&gt; &quot;d&quot; &apos;e&apos; "
                 "&#65;&#x42;&#x1F600; &unknown; z</string>")),
      "S\"a & b <c> \\\"d\\\" 'e' AB\\uD83D\\uDE00  z\"");
  EXPECT_EQ(parsed(doc("<string>a &amp;&#32; b</string>")), "S\"a &  b\"");
  EXPECT_EQ(parsed(doc("<string>&#xD800;&#0;&#1114112;</string>")),
              "S\"\\uD800\\u0000\"");  // 7 digits: ignored
  EXPECT_EQ(parsed(doc("<string>&#x110000;</string>")), "S\"\"");
  // A '&' must reach its ';' before any '<'; a negative code point is an error.
  EXPECT_EQ(parsed(doc("<string>a &b</string>")), kFailed);
  EXPECT_EQ(
      parsed("<?xml version=\"1.0\"?><plist><string>&#65</string></plist>"),
      kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist><string>a&amp</string></plist>"), kFailed);
  EXPECT_EQ(parsed(doc("<string>&#-5;</string>")), kFailed);
}

TEST(plist_xml, backslashUEscapesInStringsAndKeys) {
  EXPECT_EQ(parsed(doc("<string>\\U00e9\\u00e9\\U12</string>")),
              "S\"\\u00E9\\\\u00e9\\\\U12\"");
  EXPECT_EQ(parsed(doc("<string>\\UD83D\\UDE00</string>")),
              "S\"\\uD83D\\uDE00\"");
  EXPECT_EQ(
      parsed("<?xml version=\"1.0\"?><plist><string>\\U00411</string></plist>"),
      "S\"A1\"");
}

TEST(plist_xml, byteOrderMarks) {
  // Entities and \U replacements are NSStrings from UTF-16 units (U+FEFF and
  // U+FFFE vanish; after a vanished \U the scan skips one character); text
  // chunks are NSStrings from UTF-8 (only a chunk-leading U+FEFF vanishes).
  EXPECT_EQ(parsed("<?xml "
                     "version=\"1.0\"?><plist><array><string>&#xFEFF;a</"
                     "string><string>a&#xFEFF;</string>"
                     "<string>&#xFFFE;</string><string>\\UFEFFa</"
                     "string><string>x\\UFEFF\\U0041</string></array></plist>"),
              "[S\"a\",S\"a\",S\"\",S\"a\",S\"x\\\\U0041\"]");
  EXPECT_EQ(parsed("<?xml version=\"1.0\"?><plist><array><string>\xef\xbb\xbf"
                     "a</string><string>a\xef\xbb\xbf</string>"
                     "<string> \xef\xbb\xbf</string></array></plist>"),
              "[S\"a\",S\"a\\uFEFF\",S\" \"]");
  EXPECT_EQ(parsed("<?xml "
                     "version=\"1.0\"?><plist><array><string>\xef\xbf\xbey</"
                     "string><string>a&amp;\xef\xbb\xbf"
                     "b</string>"
                     "<string>a&amp;\xef\xbf\xbe"
                     "b</string><string>\\UFFFEy</string></array></plist>"),
              "[S\"\\uFFFEy\",S\"a&b\",S\"a&\\uFFFEb\",S\"y\"]");
}

TEST(plist_xml, markupThatIsSkipped) {
  // CDATA is dropped (no foundCDATA callback); comments, PIs, attributes and
  // DOCTYPE subsets are skipped.
  EXPECT_EQ(parsed(doc("<string><![CDATA[cdata <here>]]>after</string>")),
              "S\"after\"");
  EXPECT_EQ(parsed(doc("<string>a<!-- comment -->b</string>")), "S\"ab\"");
  EXPECT_EQ(parsed(doc("<string><!-- c --></string>")), "S\"\"");
  EXPECT_EQ(parsed(kDecl + "<!-- c --><plist><string>x</string></plist>"),
              "S\"x\"");
  EXPECT_EQ(parsed(doc("<string>a</string>", kDecl + "<?foo bar?>\n")),
              "S\"a\"");
  EXPECT_EQ(parsed(doc("<string attr=\"1\" other='2'>x</string>")), "S\"x\"");
  EXPECT_EQ(parsed(doc("<string a=b c>x</string>")), "S\"x\"");
  EXPECT_EQ(parsed(doc("<array><true/><false/><true x=\"1\"/></array>")),
              "[B1,B0,B1]");
  EXPECT_EQ(
      parsed(doc(
          "<string>x</string>",
          kDecl +
              "<!DOCTYPE plist [ <!ENTITY foo \"bar\"> <!ELEMENT x ANY> ]>\n")),
      "S\"x\"");
  EXPECT_EQ(parsed(doc("<string>a</string>", "<?xml version=\"1.0\"?>\n")),
              "S\"a\"");
  EXPECT_EQ(parsed("<?xml version=\"1.0\" "
                     "encoding=\"UTF-8\"?><plist><string>x</string></plist>"),
              "S\"x\"");
}

TEST(plist_xml, keysAndResults) {
  EXPECT_EQ(
      parsed(doc("<dict><key>k</key><key>j</key><string>v</string></dict>")),
      "{\"j\"=S\"v\";}");
  EXPECT_EQ(parsed(doc("<dict><key>a</key><string>1</string><key>a</"
                         "key><string>2</string></dict>")),
              "{\"a\"=S\"2\";}");
  EXPECT_EQ(parsed(kDecl + "<plist><dict><key>k</key></dict></plist>"), "{}");
  EXPECT_EQ(
      parsed(kDecl +
             "<plist><array><key>k</key><string>v</string></array></plist>"),
      "[S\"v\"]");
  // The last completed top-level value wins; a nested <plist> is transparent.
  EXPECT_EQ(parsed(doc("<string>a</string><string>b</string>")), "S\"b\"");
  EXPECT_EQ(parsed(doc("<array><string>a</string><plist><string>b</string></"
                         "plist></array>")),
              "[S\"a\",S\"b\"]");
  // Nothing completed: nil, and no error.
  EXPECT_EQ(parsed(doc("")), "nil");
  EXPECT_EQ(parsed("<?xml version=\"1.0\"?><plist><key>k</key></plist>"),
              "nil");
  EXPECT_TRUE(oo::parseXMLPList(doc("")).has_value());
}

TEST(plist_xml, numbersAndData) {
  EXPECT_EQ(parsed(doc("<integer> 12 </integer>")), "I12");
  EXPECT_EQ(parsed(doc("<array><integer>-3</integer><integer>+4</"
                         "integer><integer>abc</integer><integer>1.9</integer>"
                         "<integer>99999999999999999999</integer><integer></"
                         "integer></array>")),
              "[I-3,I4,I0,I1,I9223372036854775807,I0]");
  // " -5" does not start with '-', so it is stored unsigned: (unsigned long
  // long)-5.
  EXPECT_EQ(parsed(doc("<integer> -5</integer>")), "U18446744073709551611");
  EXPECT_EQ(
      parsed(doc(
          "<array><real>1e3</real><real> "
          "2.5x</real><real>nan</real><real>-inf</real><real></real></array>")),
      "[R1000,R2.5,Rnan,R-inf,R0]");
  EXPECT_EQ(parsed(doc("<array><data>AA "
                         "EC\nAw==</data><data></data><data>!!</data><data>A</"
                         "data><data>AB=C</data></array>")),
              "[D<00010203>,D<>,D<>,D<>,D<0010>]");
  EXPECT_EQ(parsed(doc("<array><data>AAEC=</data><data>AAECAw</"
                         "data><data>A-_B</data></array>")),
              "[D<000102>,D<00010203>,D<03efc1>]");
  EXPECT_EQ(parsed(kDecl + "<plist><data>AAEC</data></plist>"), "D<000102>");
  EXPECT_EQ(parsed(doc("<data>\xc3\xa9</data>")), kFailed);  // not ASCII
}

TEST(plist_xml, encodings) {
  // Not valid UTF-8: the whole document is read as ISO-8859-1.
  EXPECT_EQ(parsed(doc("<string>caf\xc3\xa9 \xff</string>")),
              "S\"caf\\u00C3\\u00A9 \\u00FF\"");
  EXPECT_EQ(
      parsed(
          "<?xml version=\"1.0\" "
          "encoding=\"ISO-8859-1\"?><plist><string>caf\xe9</string></plist>"),
      "S\"caf\\u00E9\"");
  EXPECT_EQ(parsed(doc("<string>caf\xc3\xa9</string>",
                         "<?xml version=\"1.0\" encoding=\"us-ascii\"?>\n")),
              "S\"caf\\u00C3\\u00A9\"");
}

TEST(plist_xml, structuralFailures) {
  EXPECT_EQ(parsed(doc("<dict><string>v</string></dict>")),
              kFailed);  // value without key
  EXPECT_EQ(parsed(doc("<array><foo>1</foo></array>")),
              kFailed);  // unknown element
  EXPECT_EQ(parsed(kDecl + "<plist><string>a<b>c</b>d</string></plist>"),
              kFailed);
  EXPECT_EQ(parsed(doc("<dict><key>a<b/>c</key><string>v</string></dict>")),
              kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist><string>x</string></plist><extra/>"),
              kFailed);
  EXPECT_EQ(parsed(doc("<array><string>a</string></dict>")),
              kFailed);  // nesting
  EXPECT_EQ(parsed(doc("<string>x</STRING>")), kFailed);
  EXPECT_EQ(parsed(doc("<array><string>a</string>")), kFailed);  // unclosed
  EXPECT_EQ(
      parsed(kDecl + "<plist><dict><key>k</key>\n<string>v</string>\n</dict>"),
      kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist><string>a</string"), kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist><string>a</string/></plist>"), kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist a"), kFailed);
  EXPECT_EQ(parsed(doc("<string =b>x</string>")),
              kFailed);  // empty attribute name
}

TEST(plist_xml, theDeclarationMustBeTheFirstByte) {
  EXPECT_EQ(parsed("  " + doc("<string>a</string>")), kFailed);
  EXPECT_EQ(parsed("\xef\xbb\xbf" + doc("<string>a</string>")),
              kFailed);  // BOM
}

TEST(plist_xml, whereGNUstepRaisesOofndFails_GnustepDiffers) {
  // GNUstep raises NSInvalidArgumentException out of the parser ("Tried to add
  // nil to array", "Tried to add nil value for key 'a' to dictionary"); oofnd
  // reports a failed parse.
  EXPECT_EQ(parsed(doc("<array><date>junk</date></array>")), kFailed);
  EXPECT_EQ(parsed(kDecl + "<plist a=\"unterminated></plist>"), kFailed);
  // At top level GNUstep's nil date is simply the (nil) result.
  EXPECT_EQ(parsed("<?xml version=\"1.0\"?><plist><date>junk</date></plist>"),
              "nil");
}

TEST(plist_xml, aTokenCutAtTheEndOfTheDataMustBeUtf8) {
  // Found by the GNUstep differential fuzzer. At the end of the
  // data cget() does not advance, so a tag, declaration word, attribute or run
  // of text that reaches the end loses its last byte. GNUstep makes each an
  // NSString with NewUTF8STR(); when the cut falls inside a character that is
  // nil and the parse fails ("invalid character in tag", "... declaration
  // name", "invalid character data"). Expectations captured from
  // GNUstep 1.31.1.
  const std::string h =
      "<?xml version=\"1.0\"?><plist><string>x</string></plist>";
  const std::string e = "\xc3\xa9";
  EXPECT_EQ(parsed(h + "<" + e), kFailed);   // tag
  EXPECT_EQ(parsed(h + "</" + e), kFailed);  // closing tag
  EXPECT_EQ(parsed(h + "<\xe2\x82\xac"), kFailed);
  EXPECT_EQ(parsed("<?" + e), kFailed);     // processing instruction
  EXPECT_EQ(parsed(h + " " + e), kFailed);  // character data
  EXPECT_EQ(parsed(h + "x" + e), kFailed);
  EXPECT_EQ(parsed(h + "<!" + e), kFailed);          // declaration
  EXPECT_EQ(parsed(h + "<!ELEMENT " + e), kFailed);  // declaration name
  EXPECT_EQ(parsed(h + "<!DOCTYPE a" + e), kFailed);
  EXPECT_EQ(parsed(h + "<!ATTLIST x a " + e), kFailed);  // attribute type
  EXPECT_EQ(parsed(h + "<!DOCTYPE x [<!" + e),
              kFailed);                         // nested declaration
  EXPECT_EQ(parsed(h + "<a " + e), kFailed);  // attribute name
  // When the byte cut off is ASCII, or the character ends before the data does,
  // all is well.
  EXPECT_EQ(parsed(h + e + " "), "S\"x\"");
  EXPECT_EQ(parsed(h + "a" + e + "b"), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!" + e + " "), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!DOCTYPE " + e + " "), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!DOCTYPE abc " + e), "S\"x\"");
  EXPECT_EQ(parsed(h + "<a " + e + " "), "S\"x\"");
  EXPECT_EQ(parsed(h + "<a"), "S\"x\"");
}

TEST(plist_xml, namesLoseALeadingBomAsNSStringsDo) {
  // Found by the GNUstep differential fuzzer: tag names,
  // declaration names and attribute names are NSStrings made from UTF-8, which
  // drops a leading U+FEFF.
  const std::string bom = "\xef\xbb\xbf";
  const std::string x = "<?xml version=\"1.0\"?>";
  EXPECT_EQ(parsed(x + "<plist><" + bom + "array><string>a</string></" + bom +
                     "array></plist>"),
              "[S\"a\"]");
  EXPECT_EQ(
      parsed(x + "<plist><array><string>a</string></" + bom + "array></plist>"),
      "[S\"a\"]");
  EXPECT_EQ(parsed("<?xml version=\"1.0\"" + bom +
                     " a=\"b\"?><plist><string>x</string></plist>"),
              kFailed);  // empty attribute name
  EXPECT_EQ(parsed(x + "<plist " + bom + "=\"1\"><string>x</string></plist>"),
              kFailed);
}

TEST(plist_xml, anEmptyOrUnknownEncodingNameReadsAsUtf8) {
  // Found by the GNUstep differential fuzzer: encoding="" (or a
  // name GNUstep does not know) is GSUndefinedEncoding, which is read as UTF-8,
  // not refused.
  EXPECT_EQ(parsed("<?xml version=\"1.0\" "
                     "encoding=\"\"?><plist><string>x</string></plist>"),
              "S\"x\"");
  EXPECT_EQ(parsed("<?xml version=\"1.0\" "
                     "encoding=\"\"UTF-8\"?><plist><string>x</string></plist>"),
              "S\"x\"");
  EXPECT_EQ(parsed("<?xml version=\"1.0\" "
                     "encoding=\"\"?><plist><string>\xe9</string></plist>"),
              "S\"\\u00E9\"");
  EXPECT_EQ(
      parsed("<?xml version=\"1.0\" "
             "encoding=\"foo\"?><plist><string>\xc3\xa9</string></plist>"),
      "S\"\\u00E9\"");
}

TEST(plist_xml, whereGNUstepNeverReturnsOofndFails_GnustepDiffers) {
  // Found by the GNUstep differential fuzzer. An <!ATTLIST whose
  // unquoted default runs to the end of the data: -_newQarg steps back onto the
  // last byte and the declaration loop re-reads it forever, so GNUstep never
  // returns. oofnd used to loop forever with it; it now fails the parse.
  const std::string h =
      "<?xml version=\"1.0\"?><plist><string>x</string></plist>";
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b c"), kFailed);
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b"), kFailed);
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b c d"), kFailed);
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b c\xc3\xa9"), kFailed);
  // Ending inside a quote or a #keyword terminates in GNUstep too, and is not
  // an error.
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b 'c"), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b \"c\""), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b #c"), "S\"x\"");
  EXPECT_EQ(parsed(h + "<!ATTLIST x a b c>"), "S\"x\"");
  // A '<' or '</' at the very end: GNUstep asks for a string of length -1 and
  // raises NSMallocException.
  EXPECT_EQ(parsed(h + "<"), kFailed);
  EXPECT_EQ(parsed(h + "</"), kFailed);
}

TEST(plist_xml, dateTextIsReadAsACStringInLatin1) {
  // Found by the GNUstep differential fuzzer. NSCalendarDate
  // parses [description cString] (ISO-8859-1 in the game's build), which stops
  // at a NUL; a character beyond U+00FF raises NSCharacterConversionException
  // out of GNUstep's parser, and oofnd fails the parse.
  const std::string x = "<?xml version=\"1.0\"?><plist>";
  EXPECT_EQ(parsed(x + "<array><date>2001-01-01 00:00:00 "
                         "+0000\xc3\xa9</date></array></plist>"),
              "[T0.000]");
  EXPECT_EQ(parsed(x + "<array><date>2001-01-01 00:00:00 "
                         "+0000\xc4\x81</date></array></plist>"),
              kFailed);
  EXPECT_EQ(
      parsed(x + "<date>2001-01-01 00:00:00 +0000\xe2\x82\xac</date></plist>"),
      kFailed);
  EXPECT_EQ(parsed(x + "<date>2001-01-01 00:00:00 +0000&#0;1</date></plist>"),
              "T0.000");
  EXPECT_EQ(parsed(x + "<date>2001-01-01 00:00:0&#0;5 +0000</date></plist>"),
              "nil");
}

TEST(plist_xml, zuluDatesAreUtc_GnustepDiffers) {
  // GNUstep read these in the machine's local zone (T18000.000 and
  // T15652800.000 on the UTC-5 capture machine); the "+0000" form is
  // zone-independent and matches.
  EXPECT_EQ(parsed(doc("<array><date>2001-01-01T00:00:00Z</"
                         "date><date>2001-01-01 00:00:00 +0000</date>"
                         "<date>2001-07-01T00:00:00Z</date></array>")),
              "[T0.000,T0.000,T15638400.000]");
}
