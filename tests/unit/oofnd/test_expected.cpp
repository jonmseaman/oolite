/*	test_expected.cpp
        Unit tests for std::expected as the game uses it: std::expected<T, E>,
   std::unexpected<E>, std::unexpect and std::bad_expected_access, checked against
   the behaviour C++23 specifies.

        A Tracked value type counts its live instances, so every construction
   path, assignment transition (value <-> error) and swap case is also checked
   for leaks and double destruction; it is worth running this file under
   AddressSanitizer as well.
*/

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <expected>

namespace {

enum class Err { kNone, kParse, kIO };

// A non-trivial type that counts live instances and remembers whether it was
// moved from.
struct Tracked {
  static inline int live = 0;
  int v;
  bool movedFrom = false;

  Tracked() : v(0) { ++live; }
  explicit Tracked(int x) : v(x) { ++live; }
  Tracked(int a, int b) noexcept : v(a * 100 + b) { ++live; }
  Tracked(std::initializer_list<int> il, int extra) noexcept : v(extra) {
    for (int i : il) v += i;
    ++live;
  }
  Tracked(const Tracked& o) : v(o.v) { ++live; }
  Tracked(Tracked&& o) noexcept : v(o.v) {
    o.movedFrom = true;
    ++live;
  }
  Tracked& operator=(const Tracked& o) {
    v = o.v;
    return *this;
  }
  Tracked& operator=(Tracked&& o) noexcept {
    v = o.v;
    o.movedFrom = true;
    return *this;
  }
  ~Tracked() { --live; }
  bool operator==(const Tracked& o) const { return v == o.v; }
};

using ExpT = std::expected<Tracked, std::string>;
using ExpI = std::expected<int, Err>;
using ExpV = std::expected<void, Err>;

ExpI parsePositive(int x) {
  if (x < 0) return std::unexpected(Err::kParse);
  return x;
}

ExpV checkNonZero(int x) {
  if (x == 0) return std::unexpected(Err::kIO);
  return {};
}

// --- compile-time contract
// --------------------------------------------------------------------

static_assert(std::is_same_v<ExpI::value_type, int>);
static_assert(std::is_same_v<ExpI::error_type, Err>);
static_assert(std::is_same_v<ExpI::unexpected_type, std::unexpected<Err>>);
static_assert(std::is_same_v<ExpI::rebind<double>, std::expected<double, Err>>);
static_assert(std::is_same_v<ExpV::value_type, void>);

// Triviality propagates where the standard requires it.
static_assert(std::is_trivially_copy_constructible_v<ExpI>);
static_assert(std::is_trivially_move_constructible_v<ExpI>);
static_assert(std::is_trivially_destructible_v<ExpI>);
static_assert(std::is_trivially_destructible_v<ExpV>);
static_assert(!std::is_trivially_destructible_v<ExpT>);

// Move-only payloads make a move-only Expected.
using ExpU = std::expected<std::unique_ptr<int>, Err>;
static_assert(!std::is_copy_constructible_v<ExpU>);
static_assert(std::is_move_constructible_v<ExpU>);
static_assert(std::is_move_assignable_v<ExpU>);
static_assert(!std::is_copy_assignable_v<ExpU>);

// Implicit from a value and from Unexpected; explicit from in_place / unexpect.
static_assert(std::is_convertible_v<int, ExpI>);
static_assert(std::is_convertible_v<std::unexpected<Err>, ExpI>);
static_assert(
    !std::is_convertible_v<Err, ExpI>);  // a bare error is not an error
static_assert(!std::is_convertible_v<std::in_place_t, ExpI>);
static_assert(!std::is_convertible_v<int, Tracked>);
static_assert(
    !std::is_convertible_v<int, ExpT>);  // explicit(!is_convertible_v<U, T>)
static_assert(std::is_constructible_v<ExpT, int>);

// Unexpected CTAD and constraints.
static_assert(
    std::is_same_v<decltype(std::unexpected(Err::kIO)), std::unexpected<Err>>);
static_assert(!std::is_convertible_v<Err, std::unexpected<Err>>);  // explicit

// constexpr use: the polyfill is usable in constant expressions like
// std::expected.
constexpr int constexprUse() {
  ExpI a = 5;
  ExpI b = std::unexpected(Err::kIO);
  ExpI c = a.transform([](int x) { return x * 3; });
  b = 7;
  a = std::unexpected(Err::kParse);
  return *c + *b + (a.error() == Err::kParse ? 100 : 0) + a.value_or(1000);
}
static_assert(constexprUse() == 15 + 7 + 100 + 1000);

constexpr bool constexprVoid() {
  ExpV v;
  ExpV e(std::unexpect, Err::kIO);
  return v.has_value() && !e.has_value() && e.error() == Err::kIO &&
         e.error_or(Err::kNone) == Err::kIO;
}
static_assert(constexprVoid());

// --- construction
// -----------------------------------------------------------------------------

TEST(expected, defaultConstructsValue) {
  ExpI a;
  EXPECT_TRUE(a.has_value());
  EXPECT_TRUE(static_cast<bool>(a));
  EXPECT_EQ(*a, 0);
  {
    ExpT t;
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(Tracked::live, 1);
  }
  EXPECT_EQ(Tracked::live, 0);
}

TEST(expected, constructFromValueAndUnexpected) {
  ExpI v = 42;
  EXPECT_TRUE(v.has_value());
  EXPECT_EQ(v.value(), 42);
  EXPECT_EQ(*v, 42);

  ExpI e = std::unexpected(Err::kParse);
  EXPECT_TRUE(!e.has_value());
  EXPECT_TRUE(!e);
  EXPECT_TRUE(e.error() == Err::kParse);

  // Converting Unexpected<G> -> Expected<T, E>.
  std::expected<int, std::string> s = std::unexpected("bad");
  EXPECT_TRUE(!s.has_value());
  EXPECT_EQ(s.error(), std::string("bad"));
  const std::unexpected<const char*> lvalueUnex("lvalue");
  std::expected<int, std::string> s2 = lvalueUnex;
  EXPECT_EQ(s2.error(), std::string("lvalue"));
}

TEST(expected, inPlaceAndUnexpectConstruction) {
  {
    ExpT a(std::in_place, 3, 4);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a->v, 304);
    ExpT b(std::in_place, {1, 2, 3}, 10);
    EXPECT_EQ(b->v, 16);
    EXPECT_EQ(Tracked::live, 2);

    ExpT c(std::unexpect, 5, 'x');
    EXPECT_TRUE(!c.has_value());
    EXPECT_EQ(c.error(), std::string("xxxxx"));
    EXPECT_EQ(Tracked::live, 2);

    std::expected<int, std::vector<int>> d(std::unexpect, {1, 2, 3});
    EXPECT_EQ(d.error().size(), 3u);

    std::unexpected<std::vector<int>> u(std::in_place, {4, 5});
    EXPECT_EQ(u.error().size(), 2u);
    std::unexpected<std::string> u2(std::in_place, 3, 'z');
    EXPECT_EQ(u2.error(), std::string("zzz"));
  }
  EXPECT_EQ(Tracked::live, 0);
}

TEST(expected, copyAndMoveConstruction) {
  {
    ExpT a(std::in_place, 7);
    ExpT b = a;
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(b->v, 7);
    EXPECT_EQ(Tracked::live, 2);

    ExpT c = std::move(a);
    EXPECT_EQ(c->v, 7);
    EXPECT_TRUE(a->movedFrom);
    EXPECT_TRUE(a.has_value());  // a moved-from Expected still holds a
                              // (moved-from) value
    EXPECT_EQ(Tracked::live, 3);

    ExpT e(std::unexpect, "oops");
    ExpT f = e;
    ExpT g = std::move(e);
    EXPECT_TRUE(!f.has_value() && !g.has_value());
    EXPECT_EQ(f.error(), std::string("oops"));
    EXPECT_EQ(g.error(), std::string("oops"));
    EXPECT_EQ(Tracked::live, 3);
  }
  EXPECT_EQ(Tracked::live, 0);
}

TEST(expected, convertingConstructionFromOtherExpected) {
  std::expected<int, const char*> src = 9;
  std::expected<long, std::string> dst = src;
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(*dst, 9L);

  std::expected<int, const char*> srcErr = std::unexpected("e");
  std::expected<long, std::string> dstErr = std::move(srcErr);
  EXPECT_TRUE(!dstErr.has_value());
  EXPECT_EQ(dstErr.error(), std::string("e"));

  std::expected<void, const char*> vsrc = std::unexpected("v");
  std::expected<void, std::string> vdst = vsrc;
  EXPECT_EQ(vdst.error(), std::string("v"));

  // Expected<bool, E> from an Expected is the special case the standard carves
  // out: it converts element-wise, never via Expected's own operator bool.
  std::expected<int, Err> fromInt = 0;
  std::expected<bool, Err> b = fromInt;
  EXPECT_TRUE(b.has_value());
  EXPECT_TRUE(*b == false);
}

TEST(expected, moveOnlyPayload) {
  ExpU a(std::make_unique<int>(5));
  EXPECT_TRUE(a.has_value());
  EXPECT_EQ(**a, 5);
  ExpU b = std::move(a);
  EXPECT_EQ(**b, 5);
  EXPECT_TRUE(*a == nullptr);
  std::unique_ptr<int> taken = std::move(b).value();
  EXPECT_EQ(*taken, 5);

  std::expected<int, std::unique_ptr<int>> e(std::unexpect,
                                            std::make_unique<int>(8));
  std::unique_ptr<int> err = std::move(e).error();
  EXPECT_EQ(*err, 8);
}

// --- assignment
// -------------------------------------------------------------------------------

TEST(expected, assignmentAllTransitions) {
  {
    ExpT a(std::in_place, 1);
    ExpT b(std::in_place, 2);
    ExpT e1(std::unexpect, "one");
    ExpT e2(std::unexpect, "two");
    EXPECT_EQ(Tracked::live, 2);

    a = b;  // value <- value
    EXPECT_EQ(a->v, 2);
    EXPECT_EQ(Tracked::live, 2);

    a = e1;  // value <- error: the Tracked is destroyed
    EXPECT_TRUE(!a.has_value());
    EXPECT_EQ(a.error(), std::string("one"));
    EXPECT_EQ(Tracked::live, 1);

    a = e2;  // error <- error
    EXPECT_EQ(a.error(), std::string("two"));

    a = b;  // error <- value: a Tracked is constructed
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a->v, 2);
    EXPECT_EQ(Tracked::live, 2);

    a = std::move(e1);  // move: value <- error
    EXPECT_TRUE(!a.has_value());
    EXPECT_EQ(Tracked::live, 1);
    a = std::move(b);  // move: error <- value
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(Tracked::live, 2);

    a = Tracked(11);  // from a value
    EXPECT_EQ(a->v, 11);
    a = std::unexpected(std::string("u"));  // from an Unexpected
    EXPECT_EQ(a.error(), std::string("u"));
    EXPECT_EQ(Tracked::live, 1);
    a = Tracked(12);  // value into an error-holding Expected
    EXPECT_EQ(a->v, 12);
    const std::unexpected<std::string> lu("lvalue");
    a = lu;
    EXPECT_EQ(a.error(), std::string("lvalue"));

    const ExpT& self = a;
    a = self;  // self-assignment is harmless
    EXPECT_EQ(a.error(), std::string("lvalue"));
  }
  EXPECT_EQ(Tracked::live, 0);
}

TEST(expected, emplace) {
  {
    ExpT a(std::unexpect, "err");
    Tracked& t = a.emplace(4, 2);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(t.v, 402);
    EXPECT_EQ(&t, &*a);
    a.emplace({1, 1}, 1);
    EXPECT_EQ(a->v, 3);
    EXPECT_EQ(Tracked::live, 1);

    ExpV v(std::unexpect, Err::kIO);
    v.emplace();
    EXPECT_TRUE(v.has_value());
  }
  EXPECT_EQ(Tracked::live, 0);
}

// --- swap
// -------------------------------------------------------------------------------------

TEST(expected, swapAllCases) {
  {
    ExpT a(std::in_place, 1), b(std::in_place, 2);
    a.swap(b);
    EXPECT_EQ(a->v, 2);
    EXPECT_EQ(b->v, 1);

    ExpT e1(std::unexpect, "x"), e2(std::unexpect, "y");
    swap(e1, e2);
    EXPECT_EQ(e1.error(), std::string("y"));
    EXPECT_EQ(e2.error(), std::string("x"));

    a.swap(e1);  // value <-> error
    EXPECT_TRUE(!a.has_value());
    EXPECT_EQ(a.error(), std::string("y"));
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(e1->v, 2);

    a.swap(e1);  // error <-> value
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a->v, 2);
    EXPECT_EQ(e1.error(), std::string("y"));
    EXPECT_EQ(Tracked::live, 2);

    ExpV v1, v2(std::unexpect, Err::kIO);
    v1.swap(v2);
    EXPECT_TRUE(!v1.has_value() && v2.has_value());
    EXPECT_TRUE(v1.error() == Err::kIO);
    swap(v1, v2);
    EXPECT_TRUE(v1.has_value() && !v2.has_value());

    std::unexpected<int> u1(1), u2(2);
    swap(u1, u2);
    EXPECT_EQ(u1.error(), 2);
    EXPECT_EQ(u2.error(), 1);
  }
  EXPECT_EQ(Tracked::live, 0);
}

// --- observers
// --------------------------------------------------------------------------------

TEST(expected, valueThrowsBadExpectedAccess) {
  ExpI e = std::unexpected(Err::kIO);
  bool caught = false;
  try {
    (void)e.value();
  } catch (const std::bad_expected_access<Err>& ex) {
    caught = true;
    EXPECT_TRUE(ex.error() == Err::kIO);
    EXPECT_TRUE(std::string(ex.what()).size() > 0);
  }
  EXPECT_TRUE(caught);

  // The base class catches every error type, as bad_expected_access<void> does.
  caught = false;
  try {
    std::expected<int, std::string> s(std::unexpect, "moved");
    (void)std::move(s).value();
  } catch (const std::bad_expected_access<void>&) {
    caught = true;
  }
  EXPECT_TRUE(caught);

  caught = false;
  try {
    checkNonZero(0).value();
  } catch (const std::bad_expected_access<Err>& ex) {
    caught = ex.error() == Err::kIO;
  }
  EXPECT_TRUE(caught);

  checkNonZero(1).value();  // no throw
}

TEST(expected, valueOrAndErrorOr) {
  ExpI v = 3;
  ExpI e = std::unexpected(Err::kParse);
  EXPECT_EQ(v.value_or(9), 3);
  EXPECT_EQ(e.value_or(9), 9);
  EXPECT_TRUE(v.error_or(Err::kNone) == Err::kNone);
  EXPECT_TRUE(e.error_or(Err::kNone) == Err::kParse);

  std::expected<std::string, int> s(std::unexpect, 1);
  EXPECT_EQ(std::move(s).value_or("fallback"), std::string("fallback"));
  std::expected<std::string, int> s2("have");
  EXPECT_EQ(std::move(s2).value_or("fallback"), std::string("have"));

  ExpV vv;
  EXPECT_TRUE(vv.error_or(Err::kNone) == Err::kNone);
}

TEST(expected, refQualifiedAccessors) {
  ExpT a(std::in_place, 5);
  const ExpT& ca = a;
  static_assert(std::is_same_v<decltype(*a), Tracked&>);
  static_assert(std::is_same_v<decltype(*ca), const Tracked&>);
  static_assert(std::is_same_v<decltype(*std::move(a)), Tracked&&>);
  static_assert(std::is_same_v<decltype(*std::move(ca)), const Tracked&&>);
  static_assert(std::is_same_v<decltype(a.value()), Tracked&>);
  static_assert(std::is_same_v<decltype(std::move(a).value()), Tracked&&>);
  static_assert(std::is_same_v<decltype(ca.error()), const std::string&>);
  static_assert(std::is_same_v<decltype(std::move(a).error()), std::string&&>);
  EXPECT_EQ(ca->v, 5);
  a->v = 6;
  EXPECT_EQ(ca.value().v, 6);
  Tracked moved = *std::move(a);
  EXPECT_EQ(moved.v, 6);
  EXPECT_TRUE(a->movedFrom);
}

// --- monadic operations
// -----------------------------------------------------------------------

TEST(expected, andThen) {
  auto half = [](int x) -> ExpI {
    if (x % 2) return std::unexpected(Err::kParse);
    return x / 2;
  };
  EXPECT_EQ(*parsePositive(8).and_then(half), 4);
  EXPECT_TRUE(parsePositive(7).and_then(half).error() == Err::kParse);
  EXPECT_TRUE(parsePositive(-1).and_then(half).error() == Err::kParse);

  int calls = 0;
  auto count = [&calls](int) -> ExpI {
    ++calls;
    return 0;
  };
  ExpI err = std::unexpected(Err::kIO);
  EXPECT_TRUE(err.and_then(count).error() == Err::kIO);
  EXPECT_EQ(calls, 0);  // short-circuits

  // Into void and out of void.
  ExpV r = parsePositive(0).and_then(checkNonZero);
  EXPECT_TRUE(r.error() == Err::kIO);
  ExpI fromVoid = checkNonZero(1).and_then([]() -> ExpI { return 17; });
  EXPECT_EQ(*fromVoid, 17);

  // Value category is forwarded: an rvalue Expected hands the callable an
  // rvalue.
  ExpU u(std::make_unique<int>(3));
  auto next = std::move(u).and_then(
      [](std::unique_ptr<int>&& p) -> std::expected<int, Err> {
        return *p + 1;
      });
  EXPECT_EQ(*next, 4);
}

TEST(expected, orElse) {
  auto recover = [](Err e) -> ExpI {
    return e == Err::kIO ? ExpI(0) : ExpI(std::unexpect, e);
  };
  ExpI io = std::unexpected(Err::kIO);
  ExpI parse = std::unexpected(Err::kParse);
  EXPECT_EQ(*io.or_else(recover), 0);
  EXPECT_TRUE(parse.or_else(recover).error() == Err::kParse);
  EXPECT_EQ(*ExpI(5).or_else(recover), 5);

  // or_else may change the error type.
  std::expected<int, std::string> described =
      parse.or_else([](Err) -> std::expected<int, std::string> {
        return std::unexpected(std::string("parse"));
      });
  EXPECT_EQ(described.error(), std::string("parse"));

  ExpV v = checkNonZero(0).or_else([](Err) -> ExpV { return {}; });
  EXPECT_TRUE(v.has_value());
}

TEST(expected, transform) {
  auto r =
      parsePositive(4).transform([](int x) { return std::to_string(x * 2); });
  static_assert(std::is_same_v<decltype(r), std::expected<std::string, Err>>);
  EXPECT_EQ(*r, std::string("8"));
  EXPECT_TRUE(parsePositive(-4).transform([](int x) { return x; }).error() ==
           Err::kParse);

  // To void, and from void.
  int seen = 0;
  auto toVoid = parsePositive(6).transform([&seen](int x) { seen = x; });
  static_assert(std::is_same_v<decltype(toVoid), ExpV>);
  EXPECT_TRUE(toVoid.has_value());
  EXPECT_EQ(seen, 6);
  auto fromVoid = checkNonZero(1).transform([] { return 2.5; });
  EXPECT_TRUE(*fromVoid == 2.5);

  // The result is built directly from the call (no move of the value type
  // needed).
  struct Immovable {
    int v;
    explicit Immovable(int x) : v(x) {}
    Immovable(Immovable&&) = delete;
  };
  auto imm = parsePositive(3).transform([](int x) { return Immovable(x); });
  EXPECT_EQ(imm->v, 3);
}

TEST(expected, transformError) {
  auto describe = [](Err e) {
    return e == Err::kParse ? std::string("parse") : std::string("io");
  };
  auto r = parsePositive(-2).transform_error(describe);
  static_assert(std::is_same_v<decltype(r), std::expected<int, std::string>>);
  EXPECT_EQ(r.error(), std::string("parse"));
  EXPECT_EQ(*parsePositive(2).transform_error(describe), 2);

  auto rv = checkNonZero(0).transform_error(describe);
  static_assert(std::is_same_v<decltype(rv), std::expected<void, std::string>>);
  EXPECT_EQ(rv.error(), std::string("io"));
  EXPECT_TRUE(checkNonZero(1).transform_error(describe).has_value());
}

// --- equality
// ---------------------------------------------------------------------------------

TEST(expected, equality) {
  ExpI a = 1, b = 1, c = 2;
  ExpI e1 = std::unexpected(Err::kIO), e2 = std::unexpected(Err::kIO),
       e3 = std::unexpected(Err::kParse);
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a != c);
  EXPECT_TRUE(a != e1);
  EXPECT_TRUE(e1 == e2);
  EXPECT_TRUE(e1 != e3);
  EXPECT_TRUE(a == 1);
  EXPECT_TRUE(1 == a);  // C++20 reversed candidate, as with std::expected
  EXPECT_TRUE(a != 2);
  EXPECT_TRUE(e1 != 1);
  EXPECT_TRUE(e1 == std::unexpected(Err::kIO));
  EXPECT_TRUE(a != std::unexpected(Err::kIO));

  // Heterogeneous comparison.
  std::expected<long, Err> la = 1L;
  EXPECT_TRUE(a == la);

  ExpV v1, v2, ve = std::unexpected(Err::kIO);
  EXPECT_TRUE(v1 == v2);
  EXPECT_TRUE(v1 != ve);
  EXPECT_TRUE(ve == std::unexpected(Err::kIO));
  EXPECT_TRUE(std::unexpected(1) == std::unexpected(1L));
  EXPECT_TRUE(std::unexpected(1) != std::unexpected(2));
}

// --- the shape oofnd APIs use
// -----------------------------------------------------------------

struct ParseError {
  std::string where;
  int line;
};

std::expected<std::vector<int>, ParseError> parseList(
    const std::string& s, const std::string& whereFrom) {
  std::vector<int> out;
  int line = 1;
  for (char ch : s) {
    if (ch == '\n') {
      ++line;
      continue;
    }
    if (ch < '0' || ch > '9')
      return std::unexpected(ParseError{whereFrom, line});
    out.push_back(ch - '0');
  }
  return out;
}

TEST(expected, errorPathShape) {
  auto ok = parseList("12\n3", "a.plist");
  EXPECT_TRUE(ok.has_value());
  EXPECT_EQ(ok->size(), 3u);

  auto bad = parseList("1\n2\nx", "b.plist");
  EXPECT_TRUE(!bad.has_value());
  EXPECT_EQ(bad.error().where, std::string("b.plist"));
  EXPECT_EQ(bad.error().line, 3);

  auto total = ok.transform([](const std::vector<int>& v) {
    int t = 0;
    for (int i : v) t += i;
    return t;
  });
  EXPECT_EQ(*total, 6);
}

}  // namespace
