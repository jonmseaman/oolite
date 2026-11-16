/*

OOMatrix.h

Mathematical framework for Oolite.

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#pragma once

#include "MathsBase.h"

// Each maths header includes what it uses, so OOMaths.h may list them in any
// order.
#include "HPVector.h"
#include "Quaternion.h"
#include "Vector.h"

namespace oo {

struct Matrix {
  Scalar m[4][4];
};

}  // namespace oo

inline constexpr oo::Matrix kIdentityMatrix = {.m = {{1.0f, 0.0f, 0.0f, 0.0f},
                                                     {0.0f, 1.0f, 0.0f, 0.0f},
                                                     {0.0f, 0.0f, 1.0f, 0.0f},
                                                     {0.0f, 0.0f, 0.0f, 1.0f}}};
// Stays an extern: test_OOJSCall.cpp defines its own.
extern const oo::Matrix
    kZeroMatrix; /* {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} */

/* Matrix construction and standard primitive matrices */
namespace oo {

inline Matrix MatrixConstruct(Scalar aa, Scalar ab, Scalar ac, Scalar ad,
                              Scalar ba, Scalar bb, Scalar bc, Scalar bd,
                              Scalar ca, Scalar cb, Scalar cc, Scalar cd,
                              Scalar da, Scalar db, Scalar dc,
                              Scalar dd) INLINE_CONST_FUNC;

inline Matrix MatrixFromOrientationAndPosition(
    Quaternion orientation, Vector position) INLINE_CONST_FUNC;

inline Matrix MatrixFromBasisVectorsAndPosition(
    Vector i, Vector j, Vector k, Vector position) INLINE_CONST_FUNC;
inline Matrix MatrixFromBasisVectors(Vector i, Vector j,
                                     Vector k) INLINE_CONST_FUNC;

inline Matrix MatrixForScale(Scalar sx, Scalar sy, Scalar sz) INLINE_CONST_FUNC;
inline Matrix MatrixForScaleUniform(Scalar s) INLINE_CONST_FUNC;

inline Matrix MatrixForRotationX(Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixForRotationY(Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixForRotationZ(Scalar angle) INLINE_CONST_FUNC;
Matrix MatrixForRotation(Vector axis, Scalar angle) CONST_FUNC;
Matrix MatrixForQuaternionRotation(Quaternion orientation);

inline Matrix MatrixForTranslation(Vector v) INLINE_CONST_FUNC;
inline Matrix MatrixForTranslationComponents(Scalar dx, Scalar dy,
                                             Scalar dz) INLINE_CONST_FUNC;

Matrix MatrixForBillboard(HPVector bbPos, HPVector eyePos) CONST_FUNC;

/* Matrix transformations */
inline Matrix MatrixTranslate(Matrix m, Vector offset) INLINE_CONST_FUNC;
inline Matrix MatrixHPTranslate(Matrix m, HPVector offset) INLINE_CONST_FUNC;
inline Matrix MatrixTranslateComponents(Matrix m, Scalar dx, Scalar dy,
                                        Scalar dz) INLINE_CONST_FUNC;

inline Matrix MatrixScale(Matrix m, Scalar sx, Scalar sy,
                          Scalar sz) INLINE_CONST_FUNC;
inline Matrix MatrixScaleUniform(Matrix m, Scalar s) INLINE_CONST_FUNC;

inline Matrix MatrixRotateX(Matrix m, Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixRotateY(Matrix m, Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixRotateZ(Matrix m, Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixRotate(Matrix m, Vector axis,
                           Scalar angle) INLINE_CONST_FUNC;
inline Matrix MatrixRotateQuaternion(Matrix m,
                                     Quaternion quat) INLINE_CONST_FUNC;
inline Matrix MatrixTranspose(Matrix m) INLINE_CONST_FUNC;

bool MatrixEqual(Matrix a, Matrix b) CONST_FUNC;
inline bool MatrixIsIdentity(Matrix m) INLINE_CONST_FUNC;

/* Matrix multiplication */
Matrix MatrixMultiply(Matrix a, Matrix b) CONST_FUNC;
Vector VectorMultiplyMatrix(Vector v, Matrix m) CONST_FUNC;
HPVector HPVectorMultiplyMatrix(HPVector v, Matrix m) CONST_FUNC;

/* Extraction */
inline oo::BasisVectors MatrixGetBasisVectors(Matrix m) ALWAYS_INLINE_FUNC;

/* Orthogonalizion - avoidance of distortions due to numerical inaccuracy. */
Matrix MatrixOrthogonalize(Matrix m) CONST_FUNC;

}  // namespace oo

/*	OpenGL conveniences. Need to be macros to work with OOMacroOpenGL. */
#define OOMatrixValuesForOpenGL(M) (&(M).m[0][0])
#define GLMultOOMatrix(M)                             \
  do {                                                \
    oo::Matrix m_ = M;                                \
    OOGL(glMultMatrixf(OOMatrixValuesForOpenGL(m_))); \
  } while (0)
#define GLLoadOOMatrix(M)                             \
  do {                                                \
    oo::Matrix m_ = M;                                \
    OOGL(glLoadMatrixf(OOMatrixValuesForOpenGL(m_))); \
  } while (0)
#define GLMultTransposeOOMatrix(M)                             \
  do {                                                         \
    oo::Matrix m_ = M;                                         \
    OOGL(glMultTransposeMatrixf(OOMatrixValuesForOpenGL(m_))); \
  } while (0)
#define GLLoadTransposeOOMatrix(M)                             \
  do {                                                         \
    oo::Matrix m_ = M;                                         \
    OOGL(glLoadTransposeMatrixf(OOMatrixValuesForOpenGL(m_))); \
  } while (0)
#define GLUniformMatrix(location, M)                         \
  do {                                                       \
    OOGL(glUniformMatrix4fvARB(location, 1, false,           \
                               OOMatrixValuesForOpenGL(M))); \
  } while (0)
void GLUniformMatrix3(GLint location, oo::Matrix M);

namespace oo {

inline Matrix MatrixLoadGLMatrix(GLenum matrixID) ALWAYS_INLINE_FUNC;

}  // namespace oo

#include <cstddef>
#include <string>

namespace oo {

std::string MatrixDescription(Matrix matrix);  // "{{#, #, #, #}, {#, #, #, #},
                                               // {#, #, #, #}, {#, #, #, #}}"

// Row operations

// swap row1 and row2 of M
void MatrixRowSwap(Matrix* M, int row1, int row2);
// scale row of M by factor
void MatrixRowScale(Matrix* m, int row, Scalar factor);
// replace row 1 of M with factor1 * row1 + factor2 * row2
void MatrixRowOperation(Matrix* M, int row1, Scalar factor1, int row2,
                        Scalar factor2);

// Column operations

// swap column1 and column2 of M
void MatrixColumnSwap(Matrix* M, int column1, int column2);
// scale column of M by factor
void MatrixColumnScale(Matrix* M, int column, Scalar factor);
// replace column1 of M with factor1 * column1 + factor2 + row2
void MatrixColumnOperation(Matrix* M, int column1, Scalar factor1, int column2,
                           Scalar factor2);

// Transforms between square matrices

// return matrix X such that XA = B, or zero matrix if X doesn't exist.
Matrix MatrixLeftTransform(Matrix A, Matrix B);
// return matrix X such that AX = B, or zero matrix if X doesn't exist.
Matrix MatrixRightTransform(Matrix A, Matrix B);

// Matrix inversion

Matrix MatrixInverse(Matrix M);
// Inverts matrix returning determinant of inverse in
Matrix MatrixInverseWithDeterminant(Matrix M, Scalar* determinant);

/*** Only inline definitions beyond this point ***/

inline Matrix MatrixConstruct(Scalar aa, Scalar ab, Scalar ac, Scalar ad,
                              Scalar ba, Scalar bb, Scalar bc, Scalar bd,
                              Scalar ca, Scalar cb, Scalar cc, Scalar cd,
                              Scalar da, Scalar db, Scalar dc, Scalar dd) {
  Matrix r = {
      {{aa, ab, ac, ad}, {ba, bb, bc, bd}, {ca, cb, cc, cd}, {da, db, dc, dd}}};
  return r;
}

inline Matrix MatrixFromOrientationAndPosition(Quaternion orientation,
                                               Vector position) {
  Matrix m = MatrixForQuaternionRotation(orientation);
  return MatrixTranslate(m, position);
}

inline Matrix MatrixFromBasisVectorsAndPosition(Vector i, Vector j, Vector k,
                                                Vector p) {
  return MatrixConstruct(i.x, i.y, i.z, 0.0f, j.x, j.y, j.z, 0.0f, k.x, k.y,
                         k.z, 0.0f, p.x, p.y, p.z, 1.0f);
}

inline Matrix MatrixFromBasisVectors(Vector i, Vector j, Vector k) {
  return MatrixFromBasisVectorsAndPosition(i, j, k, kZeroVector);
}

/* Standard primitive transformation matrices: */
Matrix MatrixForRotationX(Scalar angle) {
  Scalar s, c;

  s = static_cast<Scalar>(sin(angle));
  c = static_cast<Scalar>(cos(angle));

  return MatrixConstruct(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
}

Matrix MatrixForRotationY(Scalar angle) {
  Scalar s, c;

  s = static_cast<Scalar>(sin(angle));
  c = static_cast<Scalar>(cos(angle));

  return MatrixConstruct(c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0, 0, 0, 0, 1);
}

Matrix MatrixForRotationZ(Scalar angle) {
  Scalar s, c;

  s = static_cast<Scalar>(sin(angle));
  c = static_cast<Scalar>(cos(angle));

  return MatrixConstruct(c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

inline Matrix MatrixForTranslationComponents(Scalar dx, Scalar dy, Scalar dz) {
  return MatrixConstruct(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, dx, dy, dz, 1);
}

inline Matrix MatrixForTranslation(Vector v) {
  return MatrixForTranslationComponents(v.x, v.y, v.z);
}

inline Matrix MatrixTranslateComponents(Matrix m, Scalar dx, Scalar dy,
                                        Scalar dz) {
  m.m[3][0] += dx;
  m.m[3][1] += dy;
  m.m[3][2] += dz;
  return m;
}

inline Matrix MatrixTranslate(Matrix m, Vector offset) {
  return MatrixTranslateComponents(m, offset.x, offset.y, offset.z);
}

inline Matrix MatrixHPTranslate(Matrix m, HPVector offset) {
  return MatrixTranslateComponents(m, static_cast<Scalar>(offset.x),
                                   static_cast<Scalar>(offset.y),
                                   static_cast<Scalar>(offset.z));
}

inline Matrix MatrixForScale(Scalar sx, Scalar sy, Scalar sz) {
  return MatrixConstruct(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
}

inline Matrix MatrixForScaleUniform(Scalar s) {
  return MatrixForScale(s, s, s);
}

inline Matrix MatrixScale(Matrix m, Scalar sx, Scalar sy, Scalar sz) {
  return MatrixMultiply(m, MatrixForScale(sx, sy, sz));
}

inline Matrix MatrixScaleUniform(Matrix m, Scalar s) {
  return MatrixScale(m, s, s, s);
}

inline Matrix MatrixRotateX(Matrix m, Scalar angle) {
  return MatrixMultiply(m, MatrixForRotationX(angle));
}

inline Matrix MatrixRotateY(Matrix m, Scalar angle) {
  return MatrixMultiply(m, MatrixForRotationY(angle));
}

inline Matrix MatrixRotateZ(Matrix m, Scalar angle) {
  return MatrixMultiply(m, MatrixForRotationZ(angle));
}

inline Matrix MatrixRotate(Matrix m, Vector axis, Scalar angle) {
  return MatrixMultiply(m, MatrixForRotation(axis, angle));
}

inline Matrix MatrixRotateQuaternion(Matrix m, Quaternion quat) {
  return MatrixMultiply(m, MatrixForQuaternionRotation(quat));
}

inline bool MatrixIsIdentity(Matrix m) {
  return MatrixEqual(m, kIdentityMatrix);
}

inline oo::BasisVectors MatrixGetBasisVectors(Matrix m) {
  return {make_vector(m.m[0][0], m.m[1][0], m.m[2][0]),
          make_vector(m.m[0][1], m.m[1][1], m.m[2][1]),
          make_vector(m.m[0][2], m.m[1][2], m.m[2][2])};
}

inline Matrix MatrixTranspose(Matrix m) {
  return MatrixConstruct(m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0], m.m[0][1],
                         m.m[1][1], m.m[2][1], m.m[3][1], m.m[0][2], m.m[1][2],
                         m.m[2][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[2][3],
                         m.m[3][3]);
}

inline Matrix MatrixLoadGLMatrix(GLenum matrixID) {
  Matrix m;
  glGetFloatv(matrixID, OOMatrixValuesForOpenGL(m));
  return m;
}

}  // namespace oo
