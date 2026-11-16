/*

OOOpenGLMatrixManager.h

Manages OpenGL Model, View, etc. matrices.

OOOpenGLMatrixStack has no caller outside this file.

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

#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include "Maths.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

namespace oo {

inline constexpr int LITE_GL_MATRIX_MODELVIEW = 0;
inline constexpr int LITE_GL_MATRIX_PROJECTION = LITE_GL_MATRIX_MODELVIEW + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_PROJECTION =
    LITE_GL_MATRIX_PROJECTION + 1;
inline constexpr int LITE_GL_MATRIX_NORMAL =
    LITE_GL_MATRIX_MODELVIEW_PROJECTION + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_INVERSE =
    LITE_GL_MATRIX_NORMAL + 1;
inline constexpr int LITE_GL_MATRIX_PROJECTION_INVERSE =
    LITE_GL_MATRIX_MODELVIEW_INVERSE + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE =
    LITE_GL_MATRIX_PROJECTION_INVERSE + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_TRANSPOSE =
    LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE + 1;
inline constexpr int LITE_GL_MATRIX_PROJECTION_TRANSPOSE =
    LITE_GL_MATRIX_MODELVIEW_TRANSPOSE + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE =
    LITE_GL_MATRIX_PROJECTION_TRANSPOSE + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_INVERSE_TRANSPOSE =
    LITE_GL_MATRIX_MODELVIEW_PROJECTION_TRANSPOSE + 1;
inline constexpr int LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE =
    LITE_GL_MATRIX_MODELVIEW_INVERSE_TRANSPOSE + 1;
inline constexpr int LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE =
    LITE_GL_MATRIX_PROJECTION_INVERSE_TRANSPOSE + 1;
inline constexpr int LITE_GL_MATRIX_END =
    LITE_GL_MATRIX_MODELVIEW_PROJECTION_INVERSE_TRANSPOSE + 1;

}  // namespace oo

// The GLSL names of the standard matrices, indexed by OOLITE_GL_MATRIX_*.
// Each is a literal, so data() is NUL-terminated.
inline constexpr auto ooliteStandardMatrixUniforms =
    std::to_array<std::string_view>({
        "ooliteModelView",
        "ooliteProjection",
        "ooliteModelViewProjection",
        "ooliteNormalMatrix",
        "ooliteModelViewInverse",
        "ooliteProjectionInverse",
        "ooliteModelViewProjectionInverse",
        "ooliteModelViewTranspose",
        "ooliteProjectionTranspose",
        "ooliteModelViewProjectionTranspose",
        "ooliteModelViewInverseTranspose",
        "ooliteProjectionInverseTranspose",
        "ooliteModelViewProjectionInverseTranspose",
    });
static_assert(ooliteStandardMatrixUniforms.size() == oo::LITE_GL_MATRIX_END);

namespace oo {

class OpenGLMatrixStack : public oo::Object {
 public:
  void push(Matrix matrix);
  Matrix pop();
  std::size_t stackCount();

 private:
  std::vector<Matrix> stack = {};
};

class OpenGLMatrixManager : public oo::Object {
 public:
  OpenGLMatrixManager();
  void loadModelView(Matrix matrix);
  void resetModelView();
  void multModelView(Matrix matrix);
  void translateModelView(Vector vector);
  void rotateModelView(GLfloat angle, Vector axis);
  void scaleModelView(Vector scale);
  void lookAtWithEye(Vector eye, Vector center, Vector up);
  void pushModelView();
  Matrix popModelView();
  Matrix getModelView();
  std::size_t countModelView();
  void syncModelView();
  void loadProjection(Matrix matrix);
  void multProjection(Matrix matrix);
  void translateProjection(Vector vector);
  void rotateProjection(GLfloat angle, Vector axis);
  void scaleProjection(Vector scale);
  void frustumLeft(double l, double r, double b, double t, double n, double f);
  void orthoLeft(double l, double r, double b, double t, double n, double f);
  void perspectiveFovy(double fovy, double aspect, double zNear, double zFar);
  void resetProjection();
  void pushProjection();
  Matrix popProjection();
  Matrix getProjection();
  void syncProjection();
  Matrix getMatrix(int which);
  // An array of (location, matrix index, "mat3" / "mat4") triples, as
  // property-list values.
  oo::PList standardMatrixUniformLocations(GLhandleARB program);

 private:
  void updateModelView();
  void updateProjection();

  Matrix matrices[LITE_GL_MATRIX_END] = {};
  bool valid[LITE_GL_MATRIX_END] = {};
  std::shared_ptr<OpenGLMatrixStack> modelViewStack = {};
  std::shared_ptr<OpenGLMatrixStack> projectionStack = {};
};

void GLPushModelView();
Matrix GLPopModelView();
Matrix GLGetModelView();
void GLResetModelView();
void GLLoadModelView(Matrix matrix);
void GLMultModelView(Matrix matrix);
void GLTranslateModelView(Vector vector);
void GLRotateModelView(GLfloat angle, Vector axis);
void GLScaleModelView(Vector scale);
void GLLookAt(Vector eye, Vector center, Vector up);

void GLResetProjection();
void GLPushProjection();
Matrix GLPopProjection();
Matrix GLGetProjection();
void GLLoadProjection(Matrix matrix);
void GLMultProjection(Matrix matrix);
void GLTranslateProjection(Vector vector);
void GLRotateProjection(GLfloat angle, Vector axis);
void GLScaleProjection(Vector scale);
void GLFrustum(double left, double right, double bottom, double top,
               double near, double far);
void GLOrtho(double left, double right, double bottom, double top, double near,
             double far);
void GLPerspective(double fovy, double aspect, double zNear, double zFar);

Matrix GLGetModelViewProjection();

}  // namespace oo
