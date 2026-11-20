/*

OOMatrix.m

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

#include <format>
#include <string>

#include "Maths.h"
#include "OpenGLCheck.h"  // OOGL()
// glUniformMatrix3fvARB is an extension pointer declared here on Windows; other
// platforms get the prototype from the GL headers.
#include "OpenGLExtensionPointers.h"  // IWYU pragma: keep

const oo::Matrix kZeroMatrix = {.m = {
                                    {0.0f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 0.0f},
                                }};

namespace oo {

Matrix MatrixForRotation(Vector axis, Scalar angle) {
  axis = vector_normal(axis);

  Scalar x = axis.x, y = axis.y, z = axis.z;
  Scalar s = sin(angle), c = cos(angle);
  Scalar t = 1.0f - c;

  // Lots of opportunity for common subexpression elimintation here, but I'll
  // leave it to the compiler for now.
  return MatrixConstruct(t * x * x + c, t * x * y + s * z, t * x * z - s * y,
                         0.0f, t * x * y - s * z, t * y * y + c,
                         t * y * z + s * x, 0.0f, t * x * y + s * y,
                         t * y * z - s * x, t * z * z + c, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f);
}

Matrix MatrixForQuaternionRotation(Quaternion orientation) {
  Scalar w, wz, wy, wx;
  Scalar x, xz, xy, xx;
  Scalar y, yz, yy;
  Scalar z, zz;

  Quaternion q = orientation;
  quaternion_normalize(&q);

  w = q.w;
  z = q.z;
  y = q.y;
  x = q.x;

  xx = 2.0f * x;
  yy = 2.0f * y;
  zz = 2.0f * z;
  wx = w * xx;
  wy = w * yy;
  wz = w * zz;
  xx = x * xx;
  xy = x * yy;
  xz = x * zz;
  yy = y * yy;
  yz = y * zz;
  zz = z * zz;

  return MatrixConstruct(1.0f - yy - zz, xy - wz, xz + wy, 0.0f, xy + wz,
                         1.0f - xx - zz, yz - wx, 0.0f, xz - wy, yz + wx,
                         1.0f - xx - yy, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

bool MatrixEqual(Matrix a, Matrix b) {
  Scalar* ma = &a.m[0][0];
  Scalar* mb = &b.m[0][0];

  unsigned i;
  for (i = 0; i < 16; i++) {
    if (*ma++ != *mb++) return false;
  }

  return true;
}

Matrix MatrixMultiply(Matrix a, Matrix b) {
  unsigned i = 0;
  Matrix r;

  // This is amenable to significant optimization with Altivec, and presumably
  // also SSE.
  for (i = 0; i != 4; ++i) {
    r.m[i][0] = a.m[i][0] * b.m[0][0] + a.m[i][1] * b.m[1][0] +
                a.m[i][2] * b.m[2][0] + a.m[i][3] * b.m[3][0];
    r.m[i][1] = a.m[i][0] * b.m[0][1] + a.m[i][1] * b.m[1][1] +
                a.m[i][2] * b.m[2][1] + a.m[i][3] * b.m[3][1];
    r.m[i][2] = a.m[i][0] * b.m[0][2] + a.m[i][1] * b.m[1][2] +
                a.m[i][2] * b.m[2][2] + a.m[i][3] * b.m[3][2];
    r.m[i][3] = a.m[i][0] * b.m[0][3] + a.m[i][1] * b.m[1][3] +
                a.m[i][2] * b.m[2][3] + a.m[i][3] * b.m[3][3];
  }

  return r;
}

Vector VectorMultiplyMatrix(Vector v, Matrix m) {
  Scalar x, y, z, w;

  x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
  y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
  z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];
  w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3];

  w = 1.0f / w;
  return make_vector(x * w, y * w, z * w);
}

// HPVect: this loses precision because the matrix is single-precision
// Best used for rotation matrices only - use HPvector_add for
// translations of vectors if possible
HPVector HPVectorMultiplyMatrix(HPVector v, Matrix m) {
  Scalar x, y, z, w;

  x = static_cast<Scalar>(m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z +
                          m.m[3][0]);
  y = static_cast<Scalar>(m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z +
                          m.m[3][1]);
  z = static_cast<Scalar>(m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z +
                          m.m[3][2]);
  w = static_cast<Scalar>(m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z +
                          m.m[3][3]);

  w = 1.0f / w;
  return make_HPvector(x * w, y * w, z * w);
}

Matrix MatrixOrthogonalize(Matrix m) {
  //	Simple orthogonalization: make everything orthogonal to everything else.

  Vector i;  // = { m.m[0][0], m.m[1][0], m.m[2][0] };	// Overwritten without
             // being used
  Vector j = {m.m[0][1], m.m[1][1], m.m[2][1]};
  Vector k = {m.m[0][2], m.m[1][2], m.m[2][2]};

  k = vector_normal(k);
  i = vector_normal(cross_product(j, k));
  j = cross_product(k, i);

  m.m[0][0] = i.x;
  m.m[1][0] = i.y;
  m.m[2][0] = i.z;
  m.m[0][1] = j.x;
  m.m[1][1] = j.y;
  m.m[2][1] = j.z;
  m.m[0][2] = k.x;
  m.m[1][2] = k.y;
  m.m[2][2] = k.z;

  return m;
}

std::string MatrixDescription(Matrix matrix) {
  return std::format(
      "{{{{{:g}, {:g}, {:g}, {:g}}}, {{{:g}, {:g}, {:g}, {:g}}}, {{{:g}, {:g}, "
      "{:g}, {:g}}}, {{{:g}, {:g}, {:g}, {:g}}}}}",
      matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
      matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
      matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
      matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
}

Matrix MatrixForBillboard(HPVector bbPos, HPVector eyePos) {
  Vector v0, v1, v2, arbv;

  v0 = HPVectorToVector(HPvector_subtract(bbPos, eyePos));
  // once the subtraction is done safe to lose precision since
  // we're now dealing with relatively small vectors
  v0 = vector_normal_or_fallback(v0, kBasisZVector);

  // arbitrary axis - not aligned with v0
  if (v0.x == 0.0 && v0.y == 0.0) [[unlikely]]
    arbv = kBasisXVector;
  else
    arbv = kBasisZVector;

  v1 = cross_product(v0, arbv);  // 90 degrees to (v0 x arb1)
  v2 = cross_product(v0, v1);    // 90 degrees to (v0 x v1)

  return MatrixFromBasisVectors(v1, v2, v0);
}

void MatrixRowSwap(Matrix* M, int row1, int row2) {
  if (row1 < 0 || row1 >= 4 || row2 < 0 || row2 >= 4 || row1 == row2) {
    return;
  }
  float x;
  int i;
  for (i = 0; i < 4; i++) {
    x = M->m[row1][i];
    M->m[row1][i] = M->m[row2][i];
    M->m[row2][i] = x;
  }
  return;
}

void MatrixRowScale(Matrix* M, int row, Scalar factor) {
  if (row < 0 || row >= 4) {
    return;
  }
  int i;
  for (i = 0; i < 4; i++) {
    M->m[row][i] *= factor;
  }
  return;
}

void MatrixRowOperation(Matrix* M, int row1, Scalar factor1, int row2,
                        Scalar factor2) {
  if (row1 < 0 || row1 >= 4 || row2 < 0 || row2 >= 4) {
    return;
  }
  int i;
  for (i = 0; i < 4; i++) {
    M->m[row1][i] = factor1 * M->m[row1][i] + factor2 * M->m[row2][i];
  }
  return;
}

void MatrixColumnSwap(Matrix* M, int column1, int column2) {
  if (column1 < 0 || column1 >= 4 || column2 < 0 || column2 >= 4 ||
      column1 == column2) {
    return;
  }
  float x;
  int i;
  for (i = 0; i < 4; i++) {
    x = M->m[i][column1];
    M->m[i][column1] = M->m[i][column2];
    M->m[i][column2] = x;
  }
  return;
}

void MatrixColumnScale(Matrix* M, int column, Scalar factor) {
  if (column < 0 || column >= 4) {
    return;
  }
  int i;
  for (i = 0; i < 4; i++) {
    M->m[i][column] *= factor;
  }
  return;
}

void MatrixColumnOperation(Matrix* M, int column1, Scalar factor1, int column2,
                           Scalar factor2) {
  if (column1 < 0 || column1 >= 4 || column2 < 0 || column2 >= 4) {
    return;
  }
  int i;
  for (i = 0; i < 4; i++) {
    M->m[i][column1] = factor1 * M->m[i][column1] + factor2 * M->m[i][column2];
  }
  return;
}

Matrix MatrixLeftTransform(Matrix A, Matrix B) {
  int i, j;
  bool found;
  for (i = 0; i < 4; i++) {
    if (A.m[i][i] == 0.0) {
      found = false;
      for (j = i + 1; j < 4; j++) {
        if (A.m[j][i] != 0.0) {
          found = true;
          MatrixColumnSwap(&A, i, j);
          MatrixColumnSwap(&B, i, j);
          break;
        }
      }
      if (!found) {
        return kZeroMatrix;
      }
    }
    MatrixColumnScale(&B, i, 1 / A.m[i][i]);
    MatrixColumnScale(&A, i, 1 / A.m[i][i]);
    for (j = i + 1; j < 4; j++) {
      MatrixColumnOperation(&B, j, 1, i, -A.m[i][j]);
      MatrixColumnOperation(&A, j, 1, i, -A.m[i][j]);
    }
  }
  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      MatrixColumnOperation(&B, j, 1, i, -A.m[i][j]);
      MatrixColumnOperation(&A, j, 1, i, -A.m[i][j]);
    }
  }
  return B;
}

Matrix MatrixRightTransform(Matrix A, Matrix B) {
  int i, j;
  bool found;
  for (i = 0; i < 4; i++) {
    if (A.m[i][i] == 0.0) {
      found = false;
      for (j = i + 1; j < 4; j++) {
        if (A.m[j][i] != 0.0) {
          found = true;
          MatrixRowSwap(&A, i, j);
          MatrixRowSwap(&B, i, j);
          break;
        }
      }
      if (!found) {
        return kZeroMatrix;
      }
    }
    MatrixRowScale(&B, i, 1 / A.m[i][i]);
    MatrixRowScale(&A, i, 1 / A.m[i][i]);
    for (j = i + 1; j < 4; j++) {
      if (A.m[j][i] != 0.0) {
        MatrixRowOperation(&B, j, 1, i, -A.m[j][i]);
        MatrixRowOperation(&A, j, 1, i, -A.m[j][i]);
      }
    }
  }
  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (A.m[j][i] != 0.0) {
        MatrixRowOperation(&B, j, 1, i, -A.m[j][i]);
        MatrixRowOperation(&A, j, 1, i, -A.m[j][i]);
      }
    }
  }
  return B;
}

Matrix MatrixInverse(Matrix M) {
  return MatrixLeftTransform(M, kIdentityMatrix);
}

Matrix MatrixInverseWithDeterminant(Matrix M, Scalar* d) {
  int i, j;
  Matrix B = kIdentityMatrix;
  int best_row;
  float max_value;

  *d = 1.0;
  for (i = 0; i < 4; i++) {
    max_value = fabsf(M.m[i][i]);
    best_row = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(M.m[j][i]) > max_value) {
        best_row = j;
        max_value = fabs(M.m[j][i]);
      }
    }
    if (max_value == 0.0) {
      *d = 0.0;
      return kZeroMatrix;
    }
    if (best_row != i) {
      MatrixRowSwap(&M, i, best_row);
      MatrixRowSwap(&B, i, best_row);
      *d *= -1;
    }
    *d /= M.m[i][i];
    MatrixRowScale(&B, i, 1 / M.m[i][i]);
    MatrixRowScale(&M, i, 1 / M.m[i][i]);
    for (j = i + 1; j < 4; j++) {
      if (M.m[j][i] != 0.0) {
        MatrixRowOperation(&B, j, 1, i, -M.m[j][i]);
        MatrixRowOperation(&M, j, 1, i, -M.m[j][i]);
      }
    }
  }
  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (M.m[j][i] != 0.0) {
        MatrixRowOperation(&B, j, 1, i, -M.m[j][i]);
        MatrixRowOperation(&M, j, 1, i, -M.m[j][i]);
      }
    }
  }
  return B;
}

}  // namespace oo

void GLUniformMatrix3(int location, oo::Matrix M) {
  oo::Scalar m[9];
  m[0] = M.m[0][0];
  m[1] = M.m[0][1];
  m[2] = M.m[0][2];
  m[3] = M.m[1][0];
  m[4] = M.m[1][1];
  m[5] = M.m[1][2];
  m[6] = M.m[2][0];
  m[7] = M.m[2][1];
  m[8] = M.m[2][2];
  OOGL(glUniformMatrix3fvARB(location, 1, GL_FALSE,
                             m));  // GL_FALSE: objc.h's NO is not in C++
}
