/*	test_OOHPVector.cpp
	Unit tests for the double-precision vectors (src/Core/OOHPVector.h).

	The cases pin the arithmetic (add, subtract, scale, dot, cross, distance,
	normal and its fallbacks), the conversions to and from the float Vector,
	and the "(x, y, z)" description. Positions far from the origin are where
	HPVector matters, so one case checks a difference that a float would lose.
	Run: tests/run.sh unit
*/

#include <cmath>

#include <gtest/gtest.h>

#include "Maths.h"

// The test links the whole game but main, which defines the debug flags.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

bool Near(double a, double b, double tolerance = 1e-12) { return std::fabs(a - b) <= tolerance; }

bool NearHP(HPVector a, HPVector b, double tolerance = 1e-12) {
	return Near(a.x, b.x, tolerance) && Near(a.y, b.y, tolerance) && Near(a.z, b.z, tolerance);
}

}  // namespace

TEST(OOHPVector, arithmetic) {
	const HPVector a = make_HPvector(1, 2, 3);
	const HPVector b = make_HPvector(-4, 5, 0.5);
	EXPECT_TRUE(NearHP(HPvector_add(a, b), make_HPvector(-3, 7, 3.5)));
	EXPECT_TRUE(NearHP(HPvector_subtract(a, b), make_HPvector(5, -3, 2.5)));
	EXPECT_TRUE(NearHP(HPvector_multiply_scalar(a, 2), make_HPvector(2, 4, 6)));
	EXPECT_TRUE(NearHP(HPvector_flip(a), make_HPvector(-1, -2, -3)));
	EXPECT_TRUE(Near(HPdot_product(a, b), -4 + 10 + 1.5));
	EXPECT_TRUE(Near(HPmagnitude2(a), 14));
	EXPECT_TRUE(Near(HPdistance(a, a), 0));
	EXPECT_TRUE(Near(HPdistance2(a, b), 25 + 9 + 6.25));
	EXPECT_TRUE(HPvector_equal(a, make_HPvector(1, 2, 3)));
	EXPECT_TRUE(!HPvector_equal(a, b));
}

TEST(OOHPVector, theTrueCrossProductIsPerpendicular) {
	const HPVector a = make_HPvector(1, 2, 3);
	const HPVector b = make_HPvector(-4, 5, 0.5);
	const HPVector c = HPtrue_cross_product(a, b);
	EXPECT_TRUE(Near(HPdot_product(c, a), 0, 1e-9));
	EXPECT_TRUE(Near(HPdot_product(c, b), 0, 1e-9));
	EXPECT_TRUE(NearHP(HPtrue_cross_product(make_HPvector(1, 0, 0), make_HPvector(0, 1, 0)), make_HPvector(0, 0, 1)));
}

TEST(OOHPVector, normalAndItsFallbacks) {
	EXPECT_TRUE(Near(HPmagnitude(HPvector_normal(make_HPvector(3, 4, 12))), 1));
	EXPECT_TRUE(NearHP(HPvector_normal(make_HPvector(0, 0, -7)), make_HPvector(0, 0, -1)));
	EXPECT_TRUE(NearHP(HPvector_normal_or_xbasis(kZeroHPVector), make_HPvector(1, 0, 0)));
	EXPECT_TRUE(NearHP(HPvector_normal_or_ybasis(kZeroHPVector), make_HPvector(0, 1, 0)));
	EXPECT_TRUE(NearHP(HPvector_normal_or_zbasis(kZeroHPVector), make_HPvector(0, 0, 1)));
}

TEST(OOHPVector, farFromTheOriginTheDifferenceSurvives) {
	const HPVector station = make_HPvector(123456789.0, -98765432.0, 5.0e8);
	const HPVector ship = HPvector_add(station, make_HPvector(0.25, -0.5, 0.125));
	EXPECT_TRUE(NearHP(HPvector_subtract(ship, station), make_HPvector(0.25, -0.5, 0.125), 1e-6));
}

TEST(OOHPVector, conversionsToAndFromVector) {
	const Vector v = make_vector(1.5f, -2.25f, 3.0f);
	EXPECT_TRUE(NearHP(vectorToHPVector(v), make_HPvector(1.5, -2.25, 3.0)));
	const Vector back = HPVectorToVector(make_HPvector(1.5, -2.25, 3.0));
	EXPECT_TRUE(back.x == 1.5f && back.y == -2.25f && back.z == 3.0f);
}

TEST(OOHPVector, theDescriptionIsTheComponents) {
	EXPECT_EQ(HPVectorDescription(make_HPvector(1, -2.5, 0)), std::string("(1, -2.5, 0)"));
	const std::vector<double> parts = ArrayFromHPVector(make_HPvector(1, -2.5, 0));
	EXPECT_TRUE(parts.size() == 3 && parts[0] == 1 && parts[1] == -2.5 && parts[2] == 0);
}
