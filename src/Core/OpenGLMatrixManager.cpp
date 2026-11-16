/*

OOOpenGLMatrixManager.cpp

Manages OpenGL Model, View, etc. matrices.

Oolite
Copyright (C) 2004-2014 Giles C Williams and contributors

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

#include "OpenGLMatrixManager.h"

#include <numbers>
#include <string>
#include <utility>

#include "MacroOpenGL.h"
#include "MyOpenGLView.h"
#include "Universe.h"

// -init and -dealloc only called super: the implicit constructor and
// destructor.

void oo::OpenGLMatrixStack::push(oo::Matrix matrix) { stack.push_back(matrix); }

oo::Matrix oo::OpenGLMatrixStack::pop() {
  if (stack.empty()) {
    return kIdentityMatrix;
  }
  oo::Matrix matrix = stack.back();
  stack.pop_back();
  return matrix;
}

std::size_t oo::OpenGLMatrixStack::stackCount() { return stack.size(); }

void oo::OpenGLMatrixManager::updateModelView() {
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION] = false;
  valid[oo::LITE_GL_MATRIX_NORMAL] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_INVERSE] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_INVERSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_INVERSE_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE] = false;
}

void oo::OpenGLMatrixManager::updateProjection() {
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_INVERSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE] = false;
  valid[oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE] = false;
}

oo::OpenGLMatrixManager::OpenGLMatrixManager() {
  int i;
  for (i = 0; i < oo::LITE_GL_MATRIX_END; i++) {
    switch (i) {
      case oo::LITE_GL_MATRIX_MODELVIEW:
      case oo::LITE_GL_MATRIX_PROJECTION:
        matrices[i] = kIdentityMatrix;
        valid[i] = true;
        break;

      default:
        valid[i] = false;
        break;
    }
  }
  modelViewStack = oo::makeShared<oo::OpenGLMatrixStack>();
  projectionStack = oo::makeShared<oo::OpenGLMatrixStack>();
}

// -dealloc released the two stacks: their std::shared_ptr members do.

void oo::OpenGLMatrixManager::loadModelView(oo::Matrix matrix) {
  matrices[oo::LITE_GL_MATRIX_MODELVIEW] = matrix;
  updateModelView();
  return;
}

void oo::OpenGLMatrixManager::resetModelView() {
  matrices[oo::LITE_GL_MATRIX_MODELVIEW] = kIdentityMatrix;
  updateModelView();
}

void oo::OpenGLMatrixManager::multModelView(oo::Matrix matrix) {
  matrices[oo::LITE_GL_MATRIX_MODELVIEW] =
      oo::MatrixMultiply(matrix, matrices[oo::LITE_GL_MATRIX_MODELVIEW]);
  updateModelView();
}

void oo::OpenGLMatrixManager::translateModelView(Vector vector) {
  oo::Matrix matrix = kIdentityMatrix;
  matrix.m[3][0] = vector.x;
  matrix.m[3][1] = vector.y;
  matrix.m[3][2] = vector.z;
  multModelView(matrix);
}

void oo::OpenGLMatrixManager::rotateModelView(GLfloat angle, Vector axis) {
  multModelView(oo::MatrixForRotation(axis, angle));
}

void oo::OpenGLMatrixManager::scaleModelView(Vector scale) {
  multModelView(oo::MatrixForScale(scale.x, scale.y, scale.z));
}

void oo::OpenGLMatrixManager::lookAtWithEye(Vector eye, Vector center,
                                            Vector up) {
  Vector k = vector_normal(vector_subtract(eye, center));
  Vector i = cross_product(up, k);
  Vector j = cross_product(k, i);
  oo::Matrix m1 =
      oo::MatrixConstruct(i.x, j.x, k.x, 0.0, i.y, j.y, k.y, 0.0, i.z, j.z, k.z,
                          0.0, -eye.x, -eye.y, -eye.z, 1.0);
  multModelView(m1);
  return;
}

void oo::OpenGLMatrixManager::pushModelView() {
  modelViewStack->push(matrices[oo::LITE_GL_MATRIX_MODELVIEW]);
}

oo::Matrix oo::OpenGLMatrixManager::popModelView() {
  matrices[oo::LITE_GL_MATRIX_MODELVIEW] = modelViewStack->pop();
  updateModelView();
  return matrices[oo::LITE_GL_MATRIX_MODELVIEW];
}

oo::Matrix oo::OpenGLMatrixManager::getModelView() {
  return matrices[oo::LITE_GL_MATRIX_MODELVIEW];
}

std::size_t oo::OpenGLMatrixManager::countModelView() {
  return modelViewStack->stackCount();
}

void oo::OpenGLMatrixManager::syncModelView() {
  OO_ENTER_OPENGL();
  OOGL(glMatrixMode(GL_MODELVIEW));
  GLLoadOOMatrix(getModelView());
  return;
}

void oo::OpenGLMatrixManager::loadProjection(oo::Matrix matrix) {
  matrices[oo::LITE_GL_MATRIX_PROJECTION] = matrix;
  updateProjection();
  return;
}

void oo::OpenGLMatrixManager::multProjection(oo::Matrix matrix) {
  matrices[oo::LITE_GL_MATRIX_PROJECTION] =
      oo::MatrixMultiply(matrix, matrices[oo::LITE_GL_MATRIX_PROJECTION]);
  updateProjection();
}

void oo::OpenGLMatrixManager::translateProjection(Vector vector) {
  oo::Matrix matrix = kIdentityMatrix;
  matrix.m[0][3] = vector.x;
  matrix.m[1][3] = vector.y;
  matrix.m[2][3] = vector.z;
  multProjection(matrix);
}

void oo::OpenGLMatrixManager::rotateProjection(GLfloat angle, Vector axis) {
  multProjection(oo::MatrixForRotation(axis, angle));
}

void oo::OpenGLMatrixManager::scaleProjection(Vector scale) {
  multProjection(oo::MatrixForScale(scale.x, scale.y, scale.z));
}

void oo::OpenGLMatrixManager::frustumLeft(double l, double r, double b,
                                          double t, double n, double f) {
  if (l == r || t == b || n == f || n <= 0 || f <= 0) return;
  multProjection(oo::MatrixConstruct(
      static_cast<oo::Scalar>(2 * n / (r - l)), 0.0, 0.0, 0.0, 0.0,
      static_cast<oo::Scalar>(2 * n / (t - b)), 0.0, 0.0,
      static_cast<oo::Scalar>((r + l) / (r - l)),
      static_cast<oo::Scalar>((t + b) / (t - b)),
      static_cast<oo::Scalar>(-(f + n) / (f - n)), -1.0, 0.0, 0.0,
      static_cast<oo::Scalar>(-2 * f * n / (f - n)), 0.0));
}

void oo::OpenGLMatrixManager::orthoLeft(double l, double r, double b, double t,
                                        double n, double f) {
  if (l == r || t == b || n == f) return;
  multProjection(
      oo::MatrixConstruct(static_cast<oo::Scalar>(2 / (r - l)), 0.0, 0.0, 0.0,
                          0.0, static_cast<oo::Scalar>(2 / (t - b)), 0.0, 0.0,
                          0.0, 0.0, static_cast<oo::Scalar>(2 / (n - f)), 0.0,
                          static_cast<oo::Scalar>((l + r) / (l - r)),
                          static_cast<oo::Scalar>((b + t) / (b - t)),
                          static_cast<oo::Scalar>((n + f) / (n - f)), 1.0));
}

void oo::OpenGLMatrixManager::perspectiveFovy(double fovy, double aspect,
                                              double zNear, double zFar) {
  if (aspect == 0.0 || zNear == zFar) return;
  double f = 1.0 / tan(std::numbers::pi * fovy / 360);
  multProjection(oo::MatrixConstruct(
      static_cast<oo::Scalar>(f / aspect), 0.0, 0.0, 0.0, 0.0,
      static_cast<oo::Scalar>(f), 0.0, 0.0, 0.0, 0.0,
      static_cast<oo::Scalar>((zFar + zNear) / (zNear - zFar)), -1.0, 0.0, 0.0,
      static_cast<oo::Scalar>(2 * zFar * zNear / (zNear - zFar)), 0.0));
}

void oo::OpenGLMatrixManager::resetProjection() {
  matrices[oo::LITE_GL_MATRIX_PROJECTION] = kIdentityMatrix;
  updateProjection();
}

void oo::OpenGLMatrixManager::pushProjection() {
  projectionStack->push(matrices[oo::LITE_GL_MATRIX_PROJECTION]);
}

oo::Matrix oo::OpenGLMatrixManager::popProjection() {
  matrices[oo::LITE_GL_MATRIX_PROJECTION] = projectionStack->pop();
  updateProjection();
  return matrices[oo::LITE_GL_MATRIX_PROJECTION];
}

oo::Matrix oo::OpenGLMatrixManager::getProjection() {
  return matrices[oo::LITE_GL_MATRIX_PROJECTION];
}

void oo::OpenGLMatrixManager::syncProjection() {
  OO_ENTER_OPENGL();
  OOGL(glMatrixMode(GL_PROJECTION));
  GLLoadOOMatrix(getProjection());
  return;
}

oo::Matrix oo::OpenGLMatrixManager::getMatrix(int which) {
  if (which < 0 || which >= oo::LITE_GL_MATRIX_END) return kIdentityMatrix;
  if (valid[which]) return matrices[which];
  oo::Scalar d;
  switch (which) {
    case oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION:
      matrices[which] =
          oo::MatrixMultiply(matrices[oo::LITE_GL_MATRIX_MODELVIEW],
                             matrices[oo::LITE_GL_MATRIX_PROJECTION]);
      break;
    case oo::LITE_GL_MATRIX_NORMAL:
      matrices[which] = matrices[oo::LITE_GL_MATRIX_MODELVIEW];
      matrices[which].m[3][0] = 0.0;
      matrices[which].m[3][1] = 0.0;
      matrices[which].m[3][2] = 0.0;
      matrices[which].m[0][3] = 0.0;
      matrices[which].m[1][3] = 0.0;
      matrices[which].m[2][3] = 0.0;
      matrices[which].m[3][3] = 1.0;
      matrices[which] = oo::MatrixTranspose(
          oo::MatrixInverseWithDeterminant(matrices[which], &d));
      if (d != 0.0) {
        d = powf(fabsf(d), static_cast<float>(1.0 / 3));
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 3; j++) {
            matrices[which].m[i][j] /= d;
          }
        }
      }
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_INVERSE:
      matrices[which] =
          oo::MatrixInverse(matrices[oo::LITE_GL_MATRIX_MODELVIEW]);
      break;
    case oo::LITE_GL_MATRIX_PROJECTION_INVERSE:
      matrices[which] =
          oo::MatrixInverse(matrices[oo::LITE_GL_MATRIX_PROJECTION]);
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE:
      matrices[which] =
          oo::MatrixInverse(getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION));
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_TRANSPOSE:
      matrices[which] =
          oo::MatrixTranspose(matrices[oo::LITE_GL_MATRIX_MODELVIEW]);
      break;
    case oo::LITE_GL_MATRIX_PROJECTION_TRANSPOSE:
      matrices[which] =
          oo::MatrixTranspose(matrices[oo::LITE_GL_MATRIX_PROJECTION]);
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE:
      matrices[which] = oo::MatrixTranspose(
          getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION));
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_INVERSE_TRANSPOSE:
      matrices[which] =
          oo::MatrixTranspose(getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_INVERSE));
      break;
    case oo::LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE:
      matrices[which] =
          oo::MatrixTranspose(getMatrix(oo::LITE_GL_MATRIX_PROJECTION_INVERSE));
      break;
    case oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE:
      matrices[which] = oo::MatrixTranspose(
          getMatrix(oo::LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE));
      break;
  }
  valid[which] = true;
  return matrices[which];
}

oo::PList oo::OpenGLMatrixManager::standardMatrixUniformLocations(
    GLhandleARB program) {
  GLint location;
  std::size_t i;
  oo::PList::Array locationSet;

  OO_ENTER_OPENGL();

  for (i = 0; i < oo::LITE_GL_MATRIX_END; i++) {
    const std::string name(ooliteStandardMatrixUniforms[i]);  // NUL-terminated
    location = glGetUniformLocationARB(program, name.c_str());
    if (location >= 0) {
      if (i == oo::LITE_GL_MATRIX_NORMAL) {
        locationSet.push_back(oo::PList(oo::PList::Array{
            oo::PList(location), oo::PList(static_cast<std::ptrdiff_t>(i)),
            oo::PList("mat3")}));
      } else {
        locationSet.push_back(oo::PList(oo::PList::Array{
            oo::PList(location), oo::PList(static_cast<std::ptrdiff_t>(i)),
            oo::PList("mat4")}));
      }
    }
  }
  return oo::PList(std::move(locationSet));
}

namespace {

// The game view's manager, taken from the C++ view. Null when there is no
// universe or view, and each OOGL function below then does what its messages to
// nil did: nothing, or a zero matrix.
oo::OpenGLMatrixManager* GameViewMatrixManager() {
  MyOpenGLView* view =
      (UNIVERSE != nullptr ? UNIVERSE->getGameView() : nullptr);
  return (view != nullptr) ? view->getOpenGLMatrixManager() : nullptr;
}

}  // namespace

namespace oo {

void GLPushModelView() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->pushModelView();
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

Matrix GLPopModelView() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  Matrix matrix =
      matrixManager != nullptr ? matrixManager->popModelView() : kZeroMatrix;
  if (matrixManager != nullptr) matrixManager->syncModelView();
  return matrix;
}

Matrix GLGetModelView() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  Matrix matrix =
      matrixManager != nullptr ? matrixManager->getModelView() : kZeroMatrix;
  return matrix;
}

void GLResetModelView() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->resetModelView();
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLLoadModelView(Matrix matrix) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->loadModelView(matrix);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLMultModelView(Matrix matrix) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->multModelView(matrix);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLTranslateModelView(Vector vector) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->translateModelView(vector);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLRotateModelView(GLfloat angle, Vector axis) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->rotateModelView(angle, axis);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLScaleModelView(Vector scale) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->scaleModelView(scale);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLLookAt(Vector eye, Vector center, Vector up) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->lookAtWithEye(eye, center, up);
  if (matrixManager != nullptr) matrixManager->syncModelView();
}

void GLResetProjection() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->resetProjection();
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLPushProjection() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->pushProjection();
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

Matrix GLPopProjection() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  Matrix matrix =
      matrixManager != nullptr ? matrixManager->popProjection() : kZeroMatrix;
  if (matrixManager != nullptr) matrixManager->syncProjection();
  return matrix;
}

Matrix GLGetProjection() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  Matrix matrix =
      matrixManager != nullptr ? matrixManager->getProjection() : kZeroMatrix;
  return matrix;
}

void GLLoadProjection(Matrix matrix) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->loadProjection(matrix);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLMultProjection(Matrix matrix) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->multProjection(matrix);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLTranslateProjection(Vector vector) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->translateProjection(vector);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLRotateProjection(GLfloat angle, Vector axis) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->rotateProjection(angle, axis);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLScaleProjection(Vector scale) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr) matrixManager->scaleProjection(scale);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLFrustum(double frleft, double frright, double frbottom, double frtop,
               double frnear, double frfar) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr)
    matrixManager->frustumLeft(frleft, frright, frbottom, frtop, frnear, frfar);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLOrtho(double orleft, double orright, double orbottom, double ortop,
             double ornear, double orfar) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr)
    matrixManager->orthoLeft(orleft, orright, orbottom, ortop, ornear, orfar);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

void GLPerspective(double fovy, double aspect, double zNear, double zFar) {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  if (matrixManager != nullptr)
    matrixManager->perspectiveFovy(fovy, aspect, zNear, zFar);
  if (matrixManager != nullptr) matrixManager->syncProjection();
}

Matrix GLGetModelViewProjection() {
  OpenGLMatrixManager* matrixManager = GameViewMatrixManager();
  return matrixManager != nullptr
             ? matrixManager->getMatrix(LITE_GL_MATRIX_MODELVIEW_PROJECTION)
             : kZeroMatrix;
}

}  // namespace oo
