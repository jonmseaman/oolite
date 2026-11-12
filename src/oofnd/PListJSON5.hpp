/*	oofnd/PListJSON5.hpp
        oo::parseJSON5PList(): a JSON5 1.0.0 document (https://spec.json5.org)
   read into the same oo::PList tree the OpenStep and XML readers produce, so a
   data file can be <name>.json5 instead of <name>.plist (oofnd/PListFile.hpp).

            auto r = oo::parseJSON5PList(bytes);  // std::expected<PList, ...>

   An error's message is as below; log r.error().description() as the plist
   readers' errors are logged.

        Mapping:

          * object -> dict, array -> array, string -> string, true/false ->
   bool.
          * A number with no fraction and no exponent (decimal or 0x hex) ->
   integer: signed when it fits int64_t, unsigned when it fits uint64_t, else
   real. Anything else (".5", "5.", "1e3", Infinity, NaN) -> real. "-0" is the
   integer 0.
          * {"$data": "<base64>"} -> data and {"$date": "YYYY-MM-DDTHH:MM:SSZ"}
   -> date, for an object with exactly that one key and a string value. Bad
   base64 or a bad date is an error.
          * null is an error: a property list has no null.

        Strings are UTF-8 (anything else is an error); \u escapes that leave a
   surrogate unpaired keep it as WTF-8, as the other readers keep lone
   surrogates (PList.hpp). Repeated keys: the last wins, as in the OpenStep
   reader. Bare keys are ASCII identifiers, \u escapes, or any non-ASCII
   character that is not whitespace or a line terminator (looser than ES5's
   IdentifierName, which needs Unicode category tables). Arrays and objects may
   nest 512 deep.

        Errors read "JSON5 parse failed at line L, column C - <reason>": lines
   end at LF, CR, CRLF, U+2028 and U+2029; columns count bytes from 1.
*/

#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "oofnd/Data.hpp"
#include "oofnd/PList.hpp"

namespace oo {

namespace plist_detail {

inline constexpr unsigned kJSON5MaxDepth = 512;

// Appends one code point as UTF-8; a surrogate (U+D800-U+DFFF) gets its own
// 3-byte sequence (WTF-8).
inline void appendCodePointWTF8(std::string& out, std::uint32_t c) {
  if (c < 0x80) {
    out += static_cast<char>(c);
  } else if (c < 0x800) {
    out += static_cast<char>(0xC0 | (c >> 6));
    out += static_cast<char>(0x80 | (c & 0x3F));
  } else if (c < 0x10000) {
    out += static_cast<char>(0xE0 | (c >> 12));
    out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
    out += static_cast<char>(0x80 | (c & 0x3F));
  } else {
    out += static_cast<char>(0xF0 | (c >> 18));
    out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
    out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
    out += static_cast<char>(0x80 | (c & 0x3F));
  }
}

// Strict UTF-8: one code point at s[i]. Length 0 when the bytes there are not
// a well-formed sequence (overlong forms, encoded surrogates and values past
// U+10FFFF included).
inline std::size_t decodeUtf8At(std::string_view s, std::size_t i,
                                std::uint32_t& c) noexcept {
  const auto byte = [&](std::size_t k) -> unsigned {
    return i + k < s.size() ? static_cast<unsigned char>(s[i + k]) : 0x100;
  };
  const auto cont = [&](std::size_t k) { return (byte(k) & 0xC0) == 0x80; };
  const unsigned b = byte(0);
  if (b < 0x80) {
    c = b;
    return 1;
  }
  if (b >= 0xC2 && b <= 0xDF && cont(1)) {
    c = ((b & 0x1F) << 6) | (byte(1) & 0x3F);
    return 2;
  }
  if (b >= 0xE0 && b <= 0xEF && cont(1) && cont(2)) {
    c = ((b & 0x0F) << 12) | ((byte(1) & 0x3F) << 6) | (byte(2) & 0x3F);
    if (c < 0x800 || (c >= 0xD800 && c <= 0xDFFF)) return 0;
    return 3;
  }
  if (b >= 0xF0 && b <= 0xF4 && cont(1) && cont(2) && cont(3)) {
    c = ((b & 0x07) << 18) | ((byte(1) & 0x3F) << 12) |
        ((byte(2) & 0x3F) << 6) | (byte(3) & 0x3F);
    if (c < 0x10000 || c > 0x10FFFF) return 0;
    return 4;
  }
  return 0;
}

// JSON5 WhiteSpace (TAB, VT, FF, SP, NBSP, BOM and the other Zs characters).
constexpr bool isJSON5Space(std::uint32_t c) noexcept {
  return c == 0x09 || c == 0x0B || c == 0x0C || c == 0x20 || c == 0xA0 ||
         c == 0xFEFF || c == 0x1680 || (c >= 0x2000 && c <= 0x200A) ||
         c == 0x202F || c == 0x205F || c == 0x3000;
}

constexpr bool isJSON5LineTerminator(std::uint32_t c) noexcept {
  return c == 0x0A || c == 0x0D || c == 0x2028 || c == 0x2029;
}

constexpr bool isJSON5IdentifierStart(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' ||
         c == '_';
}

constexpr bool isJSON5Digit(char c) noexcept { return c >= '0' && c <= '9'; }

constexpr int json5HexValue(char c) noexcept {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

// Standard base64 with '=' padding and nothing else (what writeJSON5PList
// writes). nullopt for anything malformed.
inline std::optional<Data> decodeStrictBase64(std::string_view s) {
  const auto value = [](char c) -> int {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
  };
  if (s.size() % 4 != 0) return std::nullopt;
  std::vector<std::uint8_t> out;
  out.reserve(s.size() / 4 * 3);
  for (std::size_t i = 0; i < s.size(); i += 4) {
    const bool last = i + 4 == s.size();
    const int a = value(s[i]);
    const int b = value(s[i + 1]);
    if (a < 0 || b < 0) return std::nullopt;
    out.push_back(static_cast<std::uint8_t>((a << 2) | (b >> 4)));
    if (last && s[i + 2] == '=') {
      if (s[i + 3] != '=' || (b & 0x0F) != 0) return std::nullopt;
      break;
    }
    const int c = value(s[i + 2]);
    if (c < 0) return std::nullopt;
    out.push_back(static_cast<std::uint8_t>(((b & 0x0F) << 4) | (c >> 2)));
    if (last && s[i + 3] == '=') {
      if ((c & 0x03) != 0) return std::nullopt;
      break;
    }
    const int d = value(s[i + 3]);
    if (d < 0) return std::nullopt;
    out.push_back(static_cast<std::uint8_t>(((c & 0x03) << 6) | d));
  }
  return Data(out.data(), out.size());
}

// "YYYY-MM-DDTHH:MM:SSZ" exactly, with fields in range -> seconds since
// 2001-01-01Z (PList::Date). nullopt otherwise.
inline std::optional<double> parseJSON5Date(std::string_view s) {
  static constexpr std::string_view kShape = "dddd-dd-ddTdd:dd:ddZ";
  if (s.size() != kShape.size()) return std::nullopt;
  for (std::size_t i = 0; i < s.size(); ++i) {
    if (kShape[i] == 'd' ? !isJSON5Digit(s[i]) : s[i] != kShape[i])
      return std::nullopt;
  }
  const auto field = [&](std::size_t at, std::size_t n) {
    int v = 0;
    for (std::size_t i = at; i < at + n; ++i) v = v * 10 + (s[i] - '0');
    return v;
  };
  const int year = field(0, 4);
  const int month = field(5, 2);
  const int day = field(8, 2);
  const int hour = field(11, 2);
  const int minute = field(14, 2);
  const int second = field(17, 2);
  const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  static constexpr int kDays[] = {31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12 || day < 1 ||
      day > kDays[month - 1] + (month == 2 && leap ? 1 : 0) || hour > 23 ||
      minute > 59 || second > 59)
    return std::nullopt;
  // days_from_civil (H. Hinnant), then from 1970-01-01 to 2001-01-01.
  const int y = year - (month <= 2 ? 1 : 0);
  const int era = (y >= 0 ? y : y - 399) / 400;
  const int yoe = y - era * 400;
  const int doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  const long long days = static_cast<long long>(era) * 146097 + doe - 719468;
  return static_cast<double>(days - 11323) * 86400.0 +
         static_cast<double>(hour * 3600 + minute * 60 + second);
}

class JSON5Scanner {
 public:
  explicit JSON5Scanner(std::string_view bytes) noexcept : s_(bytes) {}

  std::expected<PList, PListError> parse() {
    skipSpace();
    if (!failed() && atEnd()) fail("empty document");
    if (failed()) return error();
    std::optional<PList> value = parseValue(0);
    if (!value) return error();
    skipSpace();
    if (!failed() && !atEnd()) fail("extra data after the value");
    if (failed()) return error();
    return std::move(*value);
  }

 private:
  std::string_view s_;
  std::size_t pos_ = 0;
  std::optional<std::size_t> errPos_;
  std::string err_;

  bool atEnd() const noexcept { return pos_ >= s_.size(); }
  bool failed() const noexcept { return errPos_.has_value(); }
  char peek(std::size_t k = 0) const noexcept {
    return pos_ + k < s_.size() ? s_[pos_ + k] : '\0';
  }

  void fail(const char* reason) { failAt(pos_, reason); }
  void failAt(std::size_t at, const char* reason) {
    if (errPos_) return;  // keep the first error
    errPos_ = at;
    err_ = reason;
  }

  std::unexpected<PListError> error() const {
    std::size_t line = 1;
    std::size_t lineStart = 0;
    const std::size_t at = errPos_.value_or(pos_);
    for (std::size_t i = 0; i < at && i < s_.size();) {
      std::uint32_t c = 0;
      std::size_t n = decodeUtf8At(s_, i, c);
      if (n == 0) n = 1;
      if (c == '\r' && i + 1 < s_.size() && s_[i + 1] == '\n') n = 2;
      i += n;
      if (isJSON5LineTerminator(c)) {
        ++line;
        lineStart = i;
      }
    }
    return std::unexpected(PListError{
        "JSON5 parse failed at line " + std::to_string(line) + ", column " +
        std::to_string(at - lineStart + 1) + " - " + err_});
  }

  // Code point at pos_ (0 length: not UTF-8).
  std::size_t current(std::uint32_t& c) const noexcept {
    return decodeUtf8At(s_, pos_, c);
  }

  // Whitespace, line terminators and comments.
  void skipSpace() {
    while (!atEnd()) {
      const char ch = peek();
      if (ch == '/' && peek(1) == '/') {
        pos_ += 2;
        while (!atEnd()) {
          std::uint32_t c = 0;
          const std::size_t n = current(c);
          if (n == 0) return fail("invalid UTF-8");
          if (isJSON5LineTerminator(c)) break;
          pos_ += n;
        }
        continue;
      }
      if (ch == '/' && peek(1) == '*') {
        const std::size_t start = pos_;
        const std::size_t close = s_.find("*/", pos_ + 2);
        if (close == std::string_view::npos)
          return failAt(start, "unterminated comment");
        for (std::size_t i = pos_ + 2; i < close;) {
          std::uint32_t c = 0;
          const std::size_t n = decodeUtf8At(s_, i, c);
          if (n == 0) return failAt(i, "invalid UTF-8");
          i += n;
        }
        pos_ = close + 2;
        continue;
      }
      std::uint32_t c = 0;
      const std::size_t n = current(c);
      if (n == 0) return fail("invalid UTF-8");
      if (!isJSON5Space(c) && !isJSON5LineTerminator(c)) return;
      pos_ += n;
    }
  }

  std::optional<PList> parseValue(unsigned depth) {
    switch (peek()) {
      case '{':
        return parseObject(depth + 1);
      case '[':
        return parseArray(depth + 1);
      case '"':
      case '\'': {
        std::optional<std::string> s = parseString();
        if (!s) return std::nullopt;
        return PList(std::move(*s));
      }
      default:
        break;
    }
    const char ch = peek();
    if (isJSON5Digit(ch) || ch == '-' || ch == '+' || ch == '.' || ch == 'I' ||
        ch == 'N')
      return parseNumber();
    if (matchWord("true")) return PList(true);
    if (matchWord("false")) return PList(false);
    if (matchWord("null")) {
      failAt(pos_ - 4, "null has no property-list equivalent");
      return std::nullopt;
    }
    fail(atEnd() ? "unexpected end of input" : "unexpected character");
    return std::nullopt;
  }

  bool identifierContinuesAt(std::size_t i) const noexcept {
    if (i >= s_.size()) return false;
    const char ch = s_[i];
    if (isJSON5IdentifierStart(ch) || isJSON5Digit(ch) || ch == '\\')
      return true;
    std::uint32_t c = 0;
    const std::size_t n = decodeUtf8At(s_, i, c);
    return n > 1 && !isJSON5Space(c) && !isJSON5LineTerminator(c);
  }

  // A keyword, not followed by an identifier character.
  bool matchWord(std::string_view word) {
    if (s_.substr(pos_, word.size()) != word ||
        identifierContinuesAt(pos_ + word.size()))
      return false;
    pos_ += word.size();
    return true;
  }

  std::optional<PList> parseObject(unsigned depth) {
    if (depth > kJSON5MaxDepth) {
      fail("nesting deeper than 512 levels");
      return std::nullopt;
    }
    ++pos_;  // '{'
    PList::Dict dict;
    for (;;) {
      skipSpace();
      if (failed()) return std::nullopt;
      if (peek() == '}') {
        ++pos_;
        break;
      }
      std::optional<std::string> key = parseKey();
      if (!key) return std::nullopt;
      skipSpace();
      if (failed()) return std::nullopt;
      if (peek() != ':') {
        fail("expected ':' after a key");
        return std::nullopt;
      }
      ++pos_;
      skipSpace();
      if (failed()) return std::nullopt;
      const std::size_t valueStart = pos_;
      std::optional<PList> value = parseValue(depth);
      if (!value) return std::nullopt;
      if (dict.empty() && (*key == "$data" || *key == "$date") &&
          value->isString())
        specialStart_ = valueStart;
      dict.insert_or_assign(std::move(*key), std::move(*value));
      skipSpace();
      if (failed()) return std::nullopt;
      if (peek() == ',') {
        ++pos_;
        continue;
      }
      if (peek() == '}') {
        ++pos_;
        break;
      }
      fail(atEnd() ? "unterminated object" : "expected ',' or '}'");
      return std::nullopt;
    }
    return special(std::move(dict));
  }

  std::size_t specialStart_ = 0;

  // {"$data": "..."} and {"$date": "..."}.
  std::optional<PList> special(PList::Dict dict) {
    if (dict.size() != 1) return PList(std::move(dict));
    const auto& [key, value] = *dict.begin();
    const std::string* text = value.getIf<std::string>();
    if (text == nullptr) return PList(std::move(dict));
    if (key == "$data") {
      std::optional<Data> data = decodeStrictBase64(*text);
      if (!data) {
        failAt(specialStart_, "$data is not base64");
        return std::nullopt;
      }
      return PList(std::move(*data));
    }
    if (key == "$date") {
      const std::optional<double> date = parseJSON5Date(*text);
      if (!date) {
        failAt(specialStart_, "$date is not YYYY-MM-DDTHH:MM:SSZ");
        return std::nullopt;
      }
      return PList(PList::Date{*date});
    }
    return PList(std::move(dict));
  }

  std::optional<PList> parseArray(unsigned depth) {
    if (depth > kJSON5MaxDepth) {
      fail("nesting deeper than 512 levels");
      return std::nullopt;
    }
    ++pos_;  // '['
    PList::Array array;
    for (;;) {
      skipSpace();
      if (failed()) return std::nullopt;
      if (atEnd()) {
        fail("unterminated array");
        return std::nullopt;
      }
      if (peek() == ']') {
        ++pos_;
        break;
      }
      std::optional<PList> value = parseValue(depth);
      if (!value) return std::nullopt;
      array.push_back(std::move(*value));
      skipSpace();
      if (failed()) return std::nullopt;
      if (peek() == ',') {
        ++pos_;
        continue;
      }
      if (peek() == ']') {
        ++pos_;
        break;
      }
      fail(atEnd() ? "unterminated array" : "expected ',' or ']'");
      return std::nullopt;
    }
    return PList(std::move(array));
  }

  // \uXXXX at pos_ (the backslash), for strings and identifiers.
  std::optional<std::uint32_t> parseUnicodeEscape() {
    std::uint32_t c = 0;
    for (std::size_t k = 2; k < 6; ++k) {
      const int h = json5HexValue(peek(k));
      if (h < 0) {
        fail("\\u needs four hex digits");
        return std::nullopt;
      }
      c = c * 16 + static_cast<std::uint32_t>(h);
    }
    pos_ += 6;
    return c;
  }

  // Appends a \u escape's unit, pairing it with a following low-surrogate
  // escape when it is a high surrogate.
  bool appendUnicodeEscape(std::string& out) {
    std::optional<std::uint32_t> c = parseUnicodeEscape();
    if (!c) return false;
    if (*c >= 0xD800 && *c <= 0xDBFF && peek() == '\\' && peek(1) == 'u') {
      const std::size_t save = pos_;
      std::optional<std::uint32_t> low = parseUnicodeEscape();
      if (!low) return false;
      if (*low >= 0xDC00 && *low <= 0xDFFF) {
        appendCodePointWTF8(out,
                            0x10000 + ((*c - 0xD800) << 10) + (*low - 0xDC00));
        return true;
      }
      pos_ = save;  // not a pair: the next escape stands alone
    }
    appendCodePointWTF8(out, *c);
    return true;
  }

  std::optional<std::string> parseKey() {
    if (peek() == '"' || peek() == '\'') return parseString();
    std::string key;
    const char first = peek();
    if (!(isJSON5IdentifierStart(first) || first == '\\' ||
          identifierContinuesAt(pos_)) ||
        isJSON5Digit(first)) {
      fail(atEnd() ? "unterminated object" : "expected a key");
      return std::nullopt;
    }
    while (identifierContinuesAt(pos_)) {
      const char ch = peek();
      if (isJSON5IdentifierStart(ch) || isJSON5Digit(ch)) {
        key += ch;
        ++pos_;
        continue;
      }
      if (ch == '\\') {
        if (peek(1) != 'u') {
          fail("only \\u escapes are allowed in a key");
          return std::nullopt;
        }
        if (!appendUnicodeEscape(key)) return std::nullopt;
        continue;
      }
      std::uint32_t c = 0;
      const std::size_t n = current(c);
      key.append(s_.substr(pos_, n));
      pos_ += n;
    }
    return key;
  }

  std::optional<std::string> parseString() {
    const char quote = peek();
    const std::size_t start = pos_;
    ++pos_;
    std::string out;
    for (;;) {
      if (atEnd()) {
        failAt(start, "unterminated string");
        return std::nullopt;
      }
      const char ch = peek();
      if (ch == quote) {
        ++pos_;
        return out;
      }
      if (ch == '\n' || ch == '\r') {
        fail("line break in a string (use \\n, or \\ at the end of the line)");
        return std::nullopt;
      }
      if (ch != '\\') {
        // A run of plain ASCII in one go, then one other character.
        std::size_t run = pos_;
        while (run < s_.size()) {
          const auto b = static_cast<unsigned char>(s_[run]);
          if (b >= 0x80 || b == static_cast<unsigned char>(quote) ||
              b == '\\' || b == '\n' || b == '\r')
            break;
          ++run;
        }
        if (run > pos_) {
          out.append(s_.substr(pos_, run - pos_));
          pos_ = run;
          continue;
        }
        std::uint32_t c = 0;
        const std::size_t n = current(c);
        if (n == 0) {
          fail("invalid UTF-8");
          return std::nullopt;
        }
        out.append(s_.substr(pos_, n));
        pos_ += n;
        continue;
      }
      // An escape.
      const char e = peek(1);
      switch (e) {
        case '\0':
          if (pos_ + 1 >= s_.size()) {
            failAt(start, "unterminated string");
            return std::nullopt;
          }
          out += '\0';  // a raw NUL after the backslash is itself
          pos_ += 2;
          continue;
        case 'b':
          out += '\b';
          break;
        case 'f':
          out += '\f';
          break;
        case 'n':
          out += '\n';
          break;
        case 'r':
          out += '\r';
          break;
        case 't':
          out += '\t';
          break;
        case 'v':
          out += '\v';
          break;
        case '0':
          if (isJSON5Digit(peek(2))) {
            fail("\\0 followed by a digit");
            return std::nullopt;
          }
          out += '\0';
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          fail("decimal escapes are not allowed");
          return std::nullopt;
        case 'x': {
          const int hi = json5HexValue(peek(2));
          const int lo = json5HexValue(peek(3));
          if (hi < 0 || lo < 0) {
            fail("\\x needs two hex digits");
            return std::nullopt;
          }
          appendCodePointWTF8(out, static_cast<std::uint32_t>(hi * 16 + lo));
          pos_ += 4;
          continue;
        }
        case 'u':
          if (!appendUnicodeEscape(out)) return std::nullopt;
          continue;
        case '\n':
          pos_ += 2;  // line continuation
          continue;
        case '\r':
          pos_ += peek(2) == '\n' ? 3 : 2;
          continue;
        default: {
          // U+2028/U+2029 continue the line; any other character is itself.
          ++pos_;
          std::uint32_t c = 0;
          const std::size_t n = current(c);
          if (n == 0) {
            fail("invalid UTF-8");
            return std::nullopt;
          }
          if (c != 0x2028 && c != 0x2029) out.append(s_.substr(pos_, n));
          pos_ += n;
          continue;
        }
      }
      pos_ += 2;
    }
  }

  std::optional<PList> parseNumber() {
    const std::size_t start = pos_;
    bool negative = false;
    if (peek() == '+' || peek() == '-') {
      negative = peek() == '-';
      ++pos_;
    }
    const auto endsCleanly = [&]() {
      if (identifierContinuesAt(pos_)) {
        fail("unexpected character after a number");
        return false;
      }
      return true;
    };
    if (matchWord("Infinity")) {
      return PList(negative ? -std::numeric_limits<double>::infinity()
                            : std::numeric_limits<double>::infinity());
    }
    if (matchWord("NaN"))
      return PList(std::numeric_limits<double>::quiet_NaN());

    if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
      pos_ += 2;
      const std::size_t digits = pos_;
      std::uint64_t magnitude = 0;
      bool overflow = false;
      double real = 0.0;
      while (json5HexValue(peek()) >= 0) {
        const auto h = static_cast<std::uint64_t>(json5HexValue(peek()));
        if (magnitude > (std::numeric_limits<std::uint64_t>::max() >> 4))
          overflow = true;
        magnitude = (magnitude << 4) | h;
        real = real * 16.0 + static_cast<double>(h);
        ++pos_;
      }
      if (pos_ == digits) {
        fail("0x needs hex digits");
        return std::nullopt;
      }
      if (!endsCleanly()) return std::nullopt;
      if (overflow) return PList(negative ? -real : real);
      return integer(negative, magnitude);
    }

    const std::size_t digitsStart = pos_;
    std::size_t intDigits = 0;
    if (peek() == '0') {
      ++pos_;
      intDigits = 1;
      if (isJSON5Digit(peek())) {
        fail("leading zeros are not allowed");
        return std::nullopt;
      }
    } else {
      while (isJSON5Digit(peek())) {
        ++pos_;
        ++intDigits;
      }
    }
    bool isReal = false;
    std::size_t fracDigits = 0;
    if (peek() == '.') {
      isReal = true;
      ++pos_;
      while (isJSON5Digit(peek())) {
        ++pos_;
        ++fracDigits;
      }
    }
    if (intDigits == 0 && fracDigits == 0) {
      failAt(start, "not a number");
      return std::nullopt;
    }
    bool negativeExponent = false;
    if (peek() == 'e' || peek() == 'E') {
      isReal = true;
      ++pos_;
      if (peek() == '+' || peek() == '-') {
        negativeExponent = peek() == '-';
        ++pos_;
      }
      if (!isJSON5Digit(peek())) {
        fail("an exponent needs digits");
        return std::nullopt;
      }
      while (isJSON5Digit(peek())) ++pos_;
    }
    if (!endsCleanly()) return std::nullopt;

    const std::string_view text = s_.substr(digitsStart, pos_ - digitsStart);
    if (!isReal) {
      std::uint64_t magnitude = 0;
      const std::from_chars_result r =
          std::from_chars(text.data(), text.data() + text.size(), magnitude);
      if (r.ec == std::errc()) return integer(negative, magnitude);
      // Too big for 64 bits: a real, as below.
    }
    double value = 0.0;
    const std::from_chars_result r =
        std::from_chars(text.data(), text.data() + text.size(), value,
                        std::chars_format::general);
    if (r.ec == std::errc::result_out_of_range) {
      value = negativeExponent ? 0.0 : std::numeric_limits<double>::infinity();
    } else if (r.ec != std::errc() || r.ptr != text.data() + text.size()) {
      failAt(start, "not a number");
      return std::nullopt;
    }
    return PList(negative ? -value : value);
  }

  // Signed when it fits int64_t, unsigned when it fits uint64_t, else real.
  static PList integer(bool negative, std::uint64_t magnitude) {
    constexpr auto kMax =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    if (!negative) {
      if (magnitude <= kMax)
        return PList::signedInteger(static_cast<std::int64_t>(magnitude));
      return PList::unsignedInteger(magnitude);
    }
    if (magnitude <= kMax)
      return PList::signedInteger(-static_cast<std::int64_t>(magnitude));
    if (magnitude == kMax + 1)
      return PList::signedInteger(std::numeric_limits<std::int64_t>::min());
    return PList(-static_cast<double>(magnitude));
  }
};

}  // namespace plist_detail

inline std::expected<PList, PListError> parseJSON5PList(
    std::string_view bytes) {
  return plist_detail::JSON5Scanner(bytes).parse();
}

}  // namespace oo
