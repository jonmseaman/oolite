/*	test_OOMathsHeaders.cpp
        Unit tests for the maths headers' layout since upstream #748: each one
   (OOVector.h, OOQuaternion.h, OOFastArithmetic.h, ...) may be included on its
   own, in any order, rather than only through OOMaths.h (the umbrella's
   INCLUDED_OOMATHS_h guard, OOMATHS_STANDALONE and OOMATHS_OPENGL_INTEGRATION
   are gone). What they share (OOScalar, the M_ constants, ABS) is
   OOMathsBase.h's, which each includes. The case includes three of them
   directly, without OOMaths.h, and uses their inline functions. Run: bash
   meson test -C build/meson_test test_OOMathsHeaders
*/

#include <gtest/gtest.h>

#include "FastArithmetic.h"
#include "Quaternion.h"  // directly, not through OOMaths.h
#include "Vector.h"

#if defined(INCLUDED_OOMATHS_h) || defined(OOMATHS_STANDALONE) || \
    defined(OOMATHS_OPENGL_INTEGRATION)
constexpr bool kUmbrellaMacros = true;
#else
constexpr bool kUmbrellaMacros = false;
#endif

#include <algorithm>
#include <numbers>
#include <type_traits>

// The headers work on their own, and the umbrella's switches are gone.
TEST(OOMathsHeaders, mathsHeadersStandAlone) {
  EXPECT_TRUE(!kUmbrellaMacros);
  EXPECT_TRUE((std::is_same_v<oo::Scalar, GLfloat>));  // always the GL type
  Vector v = vector_add(make_vector(1, 2, 3), make_vector(1, 1, 1));
  EXPECT_TRUE(v.x == 2 && v.y == 3 && v.z == 4);
  EXPECT_TRUE(dot_product(v, make_vector(1, 0, 0)) == 2);
  Quaternion q = make_quaternion(1, 0, 0, 0);
  EXPECT_TRUE(quaternion_dot_product(q, q) == 1);
  EXPECT_TRUE(oo::Clamp_0_1_f(2.0f) == 1.0f);
  EXPECT_TRUE(std::min(3, 4) == 3 && std::max(3, 4) == 4 && std::numbers::pi > 3.14);
}
