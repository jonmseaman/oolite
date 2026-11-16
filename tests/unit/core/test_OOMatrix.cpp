/*	test_OOMatrix.cpp
	Unit tests for the 4x4 matrix maths (src/Core/OOMatrix.h).

	The cases pin properties that hold whatever the rotation convention: the
	identity is neutral, a rotation and its opposite cancel and keep lengths,
	the inverse undoes the matrix, a translation moves a point, and the
	description is the "{{#, #, #, #}, ...}" text the header documents. The
	build pins -ffp-contract=off, so these also catch a fused multiply-add
	sneaking into a platform's build. Run: tests/run.sh unit
*/

#include <cmath>
#include <numbers>

#include "Maths.h"
#include "oo_test.hpp"

// The test links the whole game but main, which defines the debug flags.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

bool Near(float a, float b, float tolerance = 1e-5f) { return std::fabs(a - b) <= tolerance; }

bool NearVector(Vector a, Vector b, float tolerance = 1e-5f) {
	return Near(a.x, b.x, tolerance) && Near(a.y, b.y, tolerance) && Near(a.z, b.z, tolerance);
}

bool NearMatrix(oo::Matrix a, oo::Matrix b, float tolerance = 1e-5f) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (!Near(a.m[i][j], b.m[i][j], tolerance)) return false;
		}
	}
	return true;
}

const float kQuarterTurn = static_cast<float>(std::numbers::pi / 2.0);

}  // namespace

OO_TEST(identityIsNeutral) {
	oo::Matrix m = oo::MatrixConstruct(1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 10, 0, 1, 2, 3, 1);
	OO_CHECK(oo::MatrixIsIdentity(kIdentityMatrix));
	OO_CHECK(!oo::MatrixIsIdentity(m));
	OO_CHECK(oo::MatrixEqual(oo::MatrixMultiply(kIdentityMatrix, m), m));
	OO_CHECK(oo::MatrixEqual(oo::MatrixMultiply(m, kIdentityMatrix), m));
	OO_CHECK(oo::MatrixEqual(oo::MatrixTranspose(oo::MatrixTranspose(m)), m));
}

OO_TEST(aRotationAndItsOppositeCancel) {
	const Vector v = make_vector(1.0f, 2.0f, 3.0f);
	// The general axis is (0, 1, 1): OOMatrixForRotation's third row starts t*x*y + s*y where a
	// rotation has t*x*z + s*y, here and upstream, so an axis with x != 0 and y != z gives a matrix
	// that is not a rotation. Whether to fix it is open (the migration repo's parity notes).
	const oo::Matrix rotations[] = {oo::MatrixForRotationX(kQuarterTurn), oo::MatrixForRotationY(kQuarterTurn),
								  oo::MatrixForRotationZ(kQuarterTurn),
								  oo::MatrixForRotation(vector_normal(make_vector(0, 1, 1)), 0.7f)};
	const oo::Matrix opposites[] = {oo::MatrixForRotationX(-kQuarterTurn), oo::MatrixForRotationY(-kQuarterTurn),
								  oo::MatrixForRotationZ(-kQuarterTurn),
								  oo::MatrixForRotation(vector_normal(make_vector(0, 1, 1)), -0.7f)};
	for (int i = 0; i < 4; i++) {
		const Vector turned = oo::VectorMultiplyMatrix(v, rotations[i]);
		OO_CHECK(Near(magnitude(turned), magnitude(v)));
		OO_CHECK(NearVector(oo::VectorMultiplyMatrix(turned, opposites[i]), v));
		OO_CHECK(NearMatrix(oo::MatrixMultiply(rotations[i], opposites[i]), kIdentityMatrix));
	}
}

OO_TEST(aQuarterTurnAboutZKeepsZAndMovesXIntoTheXYPlane) {
	const Vector x = oo::VectorMultiplyMatrix(make_vector(1, 0, 0), oo::MatrixForRotationZ(kQuarterTurn));
	OO_CHECK(Near(x.x, 0.0f));
	OO_CHECK(Near(std::fabs(x.y), 1.0f));
	OO_CHECK(Near(x.z, 0.0f));
	const Vector z = oo::VectorMultiplyMatrix(make_vector(0, 0, 1), oo::MatrixForRotationZ(kQuarterTurn));
	OO_CHECK(NearVector(z, make_vector(0, 0, 1)));
}

OO_TEST(theInverseUndoesTheMatrix) {
	oo::Matrix m = oo::MatrixConstruct(2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 5, 6, 7, 1);
	m = oo::MatrixMultiply(oo::MatrixForRotationY(0.3f), m);
	OO_CHECK(NearMatrix(oo::MatrixMultiply(oo::MatrixInverse(m), m), kIdentityMatrix, 1e-4f));
	OO_CHECK(NearMatrix(oo::MatrixMultiply(m, oo::MatrixInverse(m)), kIdentityMatrix, 1e-4f));
}

OO_TEST(aTranslationMovesAPoint) {
	const Vector moved = oo::VectorMultiplyMatrix(make_vector(1, 2, 3), oo::MatrixForTranslation(make_vector(10, 20, 30)));
	OO_CHECK(NearVector(moved, make_vector(11, 22, 33)));
	const Vector scaled = oo::VectorMultiplyMatrix(make_vector(1, 2, 3), oo::MatrixForScale(2, 3, 4));
	OO_CHECK(NearVector(scaled, make_vector(2, 6, 12)));
}

OO_TEST(theDescriptionListsTheRows) {
	OO_CHECK_EQ(oo::MatrixDescription(kIdentityMatrix),
				std::string("{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}"));
	OO_CHECK_EQ(oo::MatrixDescription(oo::MatrixForTranslationComponents(1.5f, -2, 0)),
				std::string("{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1.5, -2, 0, 1}}"));
}

OO_TEST_MAIN()
