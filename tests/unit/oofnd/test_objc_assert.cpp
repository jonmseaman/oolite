/*	test_objc_assert.mm
        Unit tests for OOAssert.h, the Foundation-free assertions.
   The expected reasons are gnustep-base 1.31.1's text for NSAssert
   / NSCAssert / NSParameterAssert, pinned with a throwaway harness when the
        seam was written:

            probe.m:15  Assertion failed in Probe(instance), method fail:.  x is
   3 here probe.m:16  Assertion failed in Probe(instance), method param:.
   Invalid parameter not satisfying: p != NULL probe.m:17  Assertion failed in
   Probe(instance), method classFail.  class side probe.m:20  Assertion failed
   in void CFail(int).  y is 4

        all raised as NSInternalInconsistencyException. This test builds without
   NDEBUG, so the macros are live. The method forms (OOAssert,
   OOParameterAssert) and their cases (the first three probe lines) went with
   libobjc2: no game code called them; the function forms stay.
*/

#include <string>

#include <gtest/gtest.h>

#include "oofnd/objc/Assert.h"

#ifdef NDEBUG
#error test_objc_assert.mm tests the live macros; it must not be built with NDEBUG
#endif

namespace {

long sLine = 0;

struct Outcome {
  bool raised = false;
  std::string name;
  std::string reason;
};

template <class F>
Outcome Run(F body) {
  Outcome outcome;
  try {
    body();
  } catch (const oo::Exception& e) {
    outcome.raised = true;
    outcome.name = e.name();
    outcome.reason = e.reason();
  }
  return outcome;
}

std::string At(long line) {
  return std::string(__FILE__) + ":" + std::to_string(line) +
         "  Assertion failed in ";
}

// clang-format off: each assertion must be on the line sLine records.
void CFail(int y) { sLine = __LINE__; OOCAssert(y == 0, "y is %d", y); }
void CParam(void *p) { sLine = __LINE__; OOCParameterAssert(p != NULL); }
// clang-format on

}  // namespace

TEST(objc_assert, functionAssertionUsesPrettyFunction) {
  Outcome o = ::Run([] { CFail(4); });
  EXPECT_TRUE(o.raised);
  EXPECT_EQ(o.name, std::string("NSInternalInconsistencyException"));
#if defined(__GNUC__) && !defined(__clang__)
  EXPECT_EQ(o.reason, At(sLine) + "void {anonymous}::CFail(int).  y is 4");
#else
  EXPECT_EQ(o.reason,
              At(sLine) + "void (anonymous namespace)::CFail(int).  y is 4");
#endif

  Outcome p = ::Run([] { CParam(nullptr); });
  EXPECT_TRUE(p.raised);
#if defined(__GNUC__) && !defined(__clang__)
  EXPECT_EQ(p.reason, At(sLine) +
                            "void {anonymous}::CParam(void*).  Invalid "
                            "parameter not satisfying: p != NULL");
#else
  EXPECT_EQ(p.reason, At(sLine) +
                            "void (anonymous namespace)::CParam(void *).  "
                            "Invalid parameter not satisfying: p != NULL");
#endif
}

TEST(objc_assert, trueConditionsDoNotRaise) {
  int dummy = 0;
  EXPECT_TRUE(!::Run([] { CFail(0); }).raised);
  EXPECT_TRUE(!::Run([&] { CParam(&dummy); }).raised);
}
