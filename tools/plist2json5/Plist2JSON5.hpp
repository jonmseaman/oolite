/*	tools/plist2json5/Plist2JSON5.hpp
        The converter behind tools/plist2json5: a property list's text as JSON5
   (oofnd/PListJSON5.hpp), and the check that the JSON5 reads back as the same
   data.

            std::expected<std::string, std::string> json5 =
   plist2json5::convert(bytes, plist2json5::Mode::strings);

        OpenStep (and GNUstep) text plists are converted token by token, so
   comments, key order, layout and blank lines stay where they were: `(`/`)`
   become `[`/`]`, `=` becomes `:`, `;` becomes `,`, and each key and scalar is
   re-spelled. Every scalar's value comes from oofnd's own OpenStep scanner run
   on that token alone, so GNUstep's escape rules are kept exactly. XML plists
   carry no comments and their dictionaries no order, so they are written from
   the node tree (oo::writeJSON5PList).

        Modes. `strings` writes every OpenStep scalar as a JSON5 string, so the
   data reads back identical (==) and scripts see the same strings as before.
   `typed` writes an UNQUOTED OpenStep token as a JSON5 bool, integer or real
   when it is exactly one of
            yes no true false YES NO                    -> bool
            -?(0|[1-9][0-9]*)        (fits int64_t)     -> integer
            -?(0|[1-9][0-9]*)\.[0-9]+([eE][+-]?[0-9]+)? -> real
   and anything else as a string. A real is written as the double the game's
   getter reads from the string (PListGet.hpp), which is the token itself
   unless the token has more digits than that reading keeps.

        check() compares the JSON5's tree with the plist's: equal (==) in
   `strings` mode; in `typed` mode a bool, integer or real must be what
   get<bool>, get<long long> or get<double> reads from the plist's string.
   Integers compare by value in both modes: the XML reader marks non-negative
   <integer>s unsigned, a flag JSON5 can't carry.
   GNUstep's whitespace byte 0x08 becomes a space, and a // comment's CR or
   U+2028/U+2029 (line ends in JSON5, not in OpenStep) become spaces; comment
   bytes that are not UTF-8 are read as Latin-1.
*/

#pragma once

#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include "oofnd/PList.hpp"
#include "oofnd/PListGet.hpp"
#include "oofnd/PListJSON5.hpp"
#include "oofnd/PListOldStyle.hpp"
#include "oofnd/PListParsing.hpp"
#include "oofnd/PListWriting.hpp"

namespace plist2json5 {

enum class Mode { strings, typed };

namespace detail {

inline bool isDigit(char c) noexcept { return c >= '0' && c <= '9'; }

// -?(0|[1-9][0-9]*) and the length of the match.
inline std::size_t integerPrefix(std::string_view s) noexcept {
  std::size_t i = 0;
  if (i < s.size() && s[i] == '-') ++i;
  if (i >= s.size() || !isDigit(s[i])) return 0;
  if (s[i] == '0') return i + 1;
  while (i < s.size() && isDigit(s[i])) ++i;
  return i;
}

inline bool isTypedInteger(std::string_view s) noexcept {
  if (integerPrefix(s) != s.size() || s.empty()) return false;
  long long v = 0;
  const std::from_chars_result r =
      std::from_chars(s.data(), s.data() + s.size(), v);
  return r.ec == std::errc() && r.ptr == s.data() + s.size();
}

inline bool isTypedReal(std::string_view s) noexcept {
  std::size_t i = integerPrefix(s);
  if (i == 0 || i >= s.size() || s[i] != '.') return false;
  ++i;
  const std::size_t fraction = i;
  while (i < s.size() && isDigit(s[i])) ++i;
  if (i == fraction) return false;
  if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
    ++i;
    if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
    const std::size_t exponent = i;
    while (i < s.size() && isDigit(s[i])) ++i;
    if (i == exponent) return false;
  }
  return i == s.size();
}

inline std::optional<bool> typedBool(std::string_view s) noexcept {
  if (s == "yes" || s == "true" || s == "YES") return true;
  if (s == "no" || s == "false" || s == "NO") return false;
  return std::nullopt;
}

// What the game's getters read from a plist string.
inline oo::PList asArray(const std::string& s) {
  return oo::PList(oo::PList::Array{oo::PList(s)});
}

inline double gameDouble(const std::string& s) {
  return asArray(s).at<double>(0, std::numeric_limits<double>::quiet_NaN());
}

// UTF-8 as it is; any other byte as Latin-1.
inline void appendCommentText(std::string& out, std::string_view text,
                              bool lineComment) {
  for (std::size_t i = 0; i < text.size();) {
    std::uint32_t c = 0;
    std::size_t n = oo::plist_detail::decodeUtf8At(text, i, c);
    if (n == 0) {
      oo::plist_detail::appendCodePointWTF8(
          out, static_cast<unsigned char>(text[i]));
      ++i;
      continue;
    }
    // A line end in JSON5 but not in OpenStep; a CR right before the LF that
    // ends the comment is a CRLF, and stays.
    if (lineComment &&
        (c == 0x2028 || c == 0x2029 || (c == '\r' && i + 1 != text.size()))) {
      out += ' ';
      i += n;
      continue;
    }
    out.append(text.substr(i, n));
    i += n;
  }
}

class OpenStepConverter {
 public:
  OpenStepConverter(std::string_view src, Mode mode) : s_(src), mode_(mode) {}

  std::expected<std::string, std::string> run() {
    copySpace();
    if (!item()) return std::unexpected(error_);
    copySpace();
    if (pos_ < s_.size())
      return std::unexpected(
          std::string("extra data after the top-level item"));
    return std::move(out_);
  }

 private:
  std::string_view s_;
  Mode mode_;
  std::size_t pos_ = 0;
  std::string out_;
  std::string error_;

  char peek() const noexcept { return pos_ < s_.size() ? s_[pos_] : '\0'; }

  bool fail(std::string why) {
    if (error_.empty())
      error_ = std::move(why) + " at byte " + std::to_string(pos_ + 1);
    return false;
  }

  // Whitespace and comments, copied.
  void copySpace() {
    while (pos_ < s_.size()) {
      const auto c = static_cast<unsigned char>(s_[pos_]);
      if (oo::plist_detail::isPListSpace(c)) {
        out_ += c == 0x08 ? ' ' : static_cast<char>(c);
        ++pos_;
      } else if (c == '/' && pos_ + 1 < s_.size() && s_[pos_ + 1] == '/') {
        std::size_t end = s_.find('\n', pos_);
        if (end == std::string_view::npos) end = s_.size();
        out_ += "//";
        appendCommentText(out_, s_.substr(pos_ + 2, end - pos_ - 2), true);
        pos_ = end;
      } else if (c == '/' && pos_ + 1 < s_.size() && s_[pos_ + 1] == '*') {
        std::size_t end = s_.find("*/", pos_ + 2);
        const bool closed = end != std::string_view::npos;
        if (!closed) end = s_.size();
        out_ += "/*";
        appendCommentText(out_, s_.substr(pos_ + 2, end - pos_ - 2), false);
        out_ += "*/";
        pos_ = closed ? end + 2 : end;
      } else {
        return;
      }
    }
  }

  // The token at pos_ through the OpenStep scanner, alone.
  std::optional<oo::PList> scan(std::string_view token) {
    std::expected<oo::PList, oo::PListError> r = oo::parseOldStylePList(token);
    if (!r || r->isNull()) {
      fail("cannot read the token " + std::string(token.substr(0, 40)) + " (" +
           (r ? std::string("no value") : r.error().message) + ")");
      return std::nullopt;
    }
    return std::move(*r);
  }

  // A key or scalar: its value, and whether it was unquoted.
  struct Scalar {
    oo::PList value;
    bool unquoted = false;
  };

  std::optional<Scalar> scalar() {
    const std::size_t start = pos_;
    const char c = peek();
    if (c == '"') {
      ++pos_;
      while (pos_ < s_.size() && s_[pos_] != '"')
        pos_ += s_[pos_] == '\\' ? 2 : 1;
      if (pos_ >= s_.size()) {
        fail("unterminated string");
        return std::nullopt;
      }
      ++pos_;
      std::optional<oo::PList> v = scan(s_.substr(start, pos_ - start));
      if (!v) return std::nullopt;
      return Scalar{std::move(*v), false};
    }
    if (c == '<') {
      const std::size_t end = s_.find('>', pos_);
      if (end == std::string_view::npos) {
        fail("unterminated <...>");
        return std::nullopt;
      }
      pos_ = end + 1;
      std::optional<oo::PList> v = scan(s_.substr(start, pos_ - start));
      if (!v) return std::nullopt;
      return Scalar{std::move(*v), false};
    }
    while (pos_ < s_.size() &&
           !oo::plist_detail::isQuotable(static_cast<unsigned char>(s_[pos_])))
      ++pos_;
    // An unquoted string is its bytes (possibly none: ",," is "").
    return Scalar{oo::PList(std::string(s_.substr(start, pos_ - start))), true};
  }

  bool emitScalar(const Scalar& sc) {
    const std::string* text = sc.value.getIf<std::string>();
    if (text == nullptr) {
      // <data>, or a GNUstep <*I...>/<*R...>/<*B...>/<*D...> value.
      oo::plist_detail::JSON5Writer writer;
      if (!writer.format(sc.value, 0, out_)) return fail(writer.error);
      return true;
    }
    if (mode_ == Mode::typed && sc.unquoted) {
      if (std::optional<bool> b = typedBool(*text)) {
        out_ += *b ? "true" : "false";
        return true;
      }
      if (isTypedInteger(*text)) {
        out_ += *text;
        return true;
      }
      if (isTypedReal(*text)) {
        const double game = gameDouble(*text);
        double literal = 0.0;
        std::from_chars(text->data(), text->data() + text->size(), literal);
        if (game == literal)
          out_ += *text;
        else
          oo::plist_detail::appendJSON5Real(out_, game);
        return true;
      }
    }
    oo::plist_detail::appendJSON5String(out_, *text);
    return true;
  }

  bool item() {
    switch (peek()) {
      case '{':
        return dict();
      case '(':
        return array();
      default: {
        std::optional<Scalar> sc = scalar();
        return sc && emitScalar(*sc);
      }
    }
  }

  bool array() {
    ++pos_;
    out_ += '[';
    for (;;) {
      copySpace();
      if (peek() == ')') {
        ++pos_;
        out_ += ']';
        return true;
      }
      if (pos_ >= s_.size()) return fail("unterminated array");
      if (!item()) return false;
      copySpace();
      if (peek() == ',') {
        ++pos_;
        out_ += ',';
        continue;
      }
      if (peek() == ')') continue;
      if (pos_ >= s_.size()) return fail("unterminated array");
      return fail("expected ',' or ')'");
    }
  }

  bool dict() {
    ++pos_;
    out_ += '{';
    for (;;) {
      copySpace();
      if (peek() == '}') {
        ++pos_;
        out_ += '}';
        return true;
      }
      if (pos_ >= s_.size()) return fail("unterminated dictionary");
      std::optional<Scalar> key = scalar();
      if (!key) return false;
      const std::string* name = key->value.getIf<std::string>();
      if (name == nullptr) return fail("a key that is not a string");
      oo::plist_detail::appendJSON5Key(out_, *name);
      // "key = value" becomes "key: value"; space holding a comment or a
      // line break stays.
      const std::size_t mark = out_.size();
      copySpace();
      if (out_.find_first_not_of(" \t", mark) == std::string::npos)
        out_.resize(mark);
      if (peek() != '=') return fail("expected '='");
      ++pos_;
      out_ += ':';
      copySpace();
      if (!item()) return false;
      copySpace();
      if (peek() == ';') {
        ++pos_;
        out_ += ',';
        continue;
      }
      if (peek() == '}') continue;
      if (pos_ >= s_.size()) return fail("unterminated dictionary");
      return fail("expected ';' or '}'");
    }
  }
};

inline bool isXML(std::string_view bytes) {
  std::size_t i = 0;
  while (i < bytes.size() &&
         oo::plist_detail::isPListSpace(static_cast<unsigned char>(bytes[i])))
    ++i;
  return bytes.size() - i > 2 && bytes[i] == '<' && bytes[i + 1] == '?';
}

inline std::string describe(const oo::PList& v) {
  if (const std::string* s = v.getIf<std::string>())
    return "string \"" + *s + "\"";
  return oo::typeName(v.type());
}

// Equal, or (typed) a typed JSON5 value the game reads the same from the
// plist's string. `where` gets the path to the first difference.
inline bool equivalent(const oo::PList& json5, const oo::PList& plist,
                       Mode mode, std::string& where) {
  if (mode == Mode::typed && plist.isString() && json5.type() != plist.type()) {
    const std::string& s = *plist.getIf<std::string>();
    const oo::PList a = asArray(s);
    bool same = false;
    if (const bool* b = json5.getIf<bool>()) {
      same = a.at<bool>(0, !*b) == *b;
    } else if (const oo::PList::Integer* i =
                   json5.getIf<oo::PList::Integer>()) {
      const long long want = i->value;
      const long long other =
          want == std::numeric_limits<long long>::max() ? want - 1 : want + 1;
      same = !i->isUnsigned && a.at<long long>(0, other) == want;
    } else if (const double* d = json5.getIf<double>()) {
      same = gameDouble(s) == *d;
    }
    if (!same)
      where += ": JSON5 " + describe(json5) + ", plist " + describe(plist);
    return same;
  }
  if (json5.type() != plist.type()) {
    where += ": JSON5 " + describe(json5) + ", plist " + describe(plist);
    return false;
  }
  if (const oo::PList::Array* ja = json5.getIf<oo::PList::Array>()) {
    const oo::PList::Array& pa = *plist.getIf<oo::PList::Array>();
    if (ja->size() != pa.size()) {
      where += ": JSON5 has " + std::to_string(ja->size()) +
               " elements, plist " + std::to_string(pa.size());
      return false;
    }
    for (std::size_t i = 0; i < ja->size(); ++i) {
      const std::size_t mark = where.size();
      where += "[" + std::to_string(i) + "]";
      if (!equivalent((*ja)[i], pa[i], mode, where)) return false;
      where.resize(mark);
    }
    return true;
  }
  if (const oo::PList::Dict* jd = json5.getIf<oo::PList::Dict>()) {
    const oo::PList::Dict& pd = *plist.getIf<oo::PList::Dict>();
    for (const auto& [key, value] : pd) {
      if (!jd->contains(key)) {
        where += ": JSON5 lacks the key \"" + key + "\"";
        return false;
      }
    }
    for (const auto& [key, value] : *jd) {
      const auto it = pd.find(key);
      if (it == pd.end()) {
        where += ": JSON5 has an extra key \"" + key + "\"";
        return false;
      }
      const std::size_t mark = where.size();
      where += "." + key;
      if (!equivalent(value, it->second, mode, where)) return false;
      where.resize(mark);
    }
    return true;
  }
  // Integers by value: the XML reader marks every non-negative <integer> as
  // unsigned (GNUstep's NSNumber), which JSON5 has no way to say.
  if (const oo::PList::Integer* ji = json5.getIf<oo::PList::Integer>()) {
    const oo::PList::Integer& pi = *plist.getIf<oo::PList::Integer>();
    if (ji->value == pi.value &&
        (ji->isUnsigned == pi.isUnsigned || ji->value >= 0))
      return true;
  } else if (json5 == plist) {
    return true;
  }
  where += ": JSON5 " + describe(json5) + ", plist " + describe(plist);
  return false;
}

}  // namespace detail

// A plist's text as JSON5, or why it could not be converted.
inline std::expected<std::string, std::string> convert(std::string_view bytes,
                                                       Mode mode) {
  if (detail::isXML(bytes)) {
    std::expected<oo::PList, oo::PListError> tree =
        oo::parsePropertyList(bytes);
    if (!tree) return std::unexpected(tree.error().message);
    std::expected<oo::Data, oo::PListError> out = oo::writeJSON5PList(*tree);
    if (!out) return std::unexpected(out.error().message);
    return std::string(reinterpret_cast<const char*>(out->bytes()),
                       out->length());
  }
  std::string_view body = oo::plist_detail::utf8WithoutLeadingBOM(bytes);
  std::expected<std::string, std::string> out =
      detail::OpenStepConverter(body, mode).run();
  if (out && (out->empty() || out->back() != '\n')) *out += '\n';
  return out;
}

// Empty when `json5` reads back as the same data as `plist`; otherwise what
// differs.
inline std::string check(std::string_view plist, std::string_view json5,
                         Mode mode) {
  std::expected<oo::PList, oo::PListError> original =
      oo::parsePropertyList(plist);
  if (!original) return "the plist does not parse: " + original.error().message;
  std::expected<oo::PList, oo::PListError> back = oo::parseJSON5PList(json5);
  if (!back) return "the JSON5 does not parse: " + back.error().message;
  // XML input is written from its (typed) tree, so it must match exactly.
  const Mode compare = detail::isXML(plist) ? Mode::strings : mode;
  std::string where = "(top)";
  if (detail::equivalent(*back, *original, compare, where)) return {};
  return "the data differs at " + where;
}

}  // namespace plist2json5
