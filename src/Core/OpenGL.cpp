/*

OOOpenGL.m

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

#include "OpenGL.h"

#include <cmath>
#include <format>
#include <numbers>
#include <string>

#include "LogIndentScope.h"
#include "Logging.h"  // IWYU pragma: keep: used only without NDEBUG or with OO_CHECK_GL_HEAVY
#include "MacroOpenGL.h"
// std::numbers::pi: Windows' <math.h> leaves it out unless _USE_MATH_DEFINES is
// set, and OOMathsBase.h supplies it there.
#include "MathsBase.h"               // IWYU pragma: keep
#include "OpenGLExtensionManager.h"  // IWYU pragma: keep: used only without NDEBUG
#include "oofnd/Log.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/Assert.h"

#ifndef NDEBUG
namespace oo {

static const char* const kLogOpenGLStateDump = "rendering.opengl.stateDump";

}  // namespace oo

#endif

static GLfloat sDisplayScaleFactor = 1.0f;

namespace oo {

bool CheckOpenGLErrors(const char* format, ...) {
  va_list args;

  va_start(args, format);
  // formatted only when an error is found (a copy of the arguments each time it
  // is)
  bool errorOccurred = CheckOpenGLErrors([format, &args]() -> std::string {
    if (format == nullptr) return "<unknown>";
    va_list copy;
    va_copy(copy, args);
    std::string context = oo::str::vformat(format, copy);
    va_end(copy);
    return context;
  });
  va_end(args);

  return errorOccurred;
}

bool CheckOpenGLErrors(const std::function<std::string()>& context) {
  GLenum errCode;
  const GLubyte* errString = nullptr;
  bool errorOccurred = false;
  static bool noReenter;

  if (noReenter) return false;
  noReenter = true;

  OO_ENTER_OPENGL();

  // Short-circut here, because glGetError() is quite expensive.
  if (oo::log::willDisplay("rendering.opengl.error")) {
    std::optional<std::string> contextText;  // built once, at the first error
    for (;;) {
      errCode = glGetError();

      if (errCode == GL_NO_ERROR) break;

      errorOccurred = true;
      errString = gluErrorString(errCode);
      if (!contextText.has_value()) contextText = context();

      // gluErrorString can answer NULL, which %s printed as (null).
      OO_LOG("rendering.opengl.error",
             "OpenGL error: \"{}\" ({:#x}), context: {}",
             errString != nullptr ? reinterpret_cast<const char*>(errString)
                                  : "(null)",
             errCode, *contextText);
    }
  }

#if OO_CHECK_GL_HEAVY
  if (errorOccurred) {
    {
      const oo::LogIndentScope indent;
      LogOpenGLState();
    }
    while (glGetError() != 0) {
    }  // Suppress any errors caused by LogOpenGLState().
  }
#endif

  noReenter = false;
  return errorOccurred;
}

void GLWireframeModeOn() {
  OO_ENTER_OPENGL();

  /* Because of where this is called relative to the
   * OpenGLStateManager methods, pushAttrib/popAttrib causes the
   * OpenGL state to move away from where it should be. So, instead,
   * just switch polygon mode as needed. Linewidth is set already by
   * everything that needs it and not handled by the state
   * manager.  - CIM*/
  //	OOGL(glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT | GL_TEXTURE_BIT));
  OOGL(GLScaledLineWidth(1.0f));
  OOGL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
}

void GLWireframeModeOff() {
  OO_ENTER_OPENGL();
  OOGL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
  //	OOGL(glPopAttrib());
}

}  // namespace oo

void GLDrawBallBillboard(GLfloat radius, GLfloat step, GLfloat z_distance) {
  if ((radius <= 0) || (step < 1)) [[unlikely]]
    return;
  if (radius >= z_distance) [[unlikely]]
    return;  // inside the sphere

  GLfloat i, delta;
  GLfloat s, c;
  GLfloat r;

  OO_ENTER_OPENGL();

  r = static_cast<GLfloat>(radius * z_distance /
                           sqrt(z_distance * z_distance - radius * radius));
  delta = static_cast<GLfloat>(step * std::numbers::pi /
                               180.0f);  // Convert step from degrees to radians

  glVertex3i(0, 0, 0);
  for (i = 0; i < (std::numbers::pi * 2.0); i += delta) {
    s = static_cast<GLfloat>(r * sin(i));
    c = static_cast<GLfloat>(r * cos(i));
    glVertex3f(s, c, 0.0);
  }
  glVertex3f(0.0, r, 0.0);  // repeat the zero value to close
}

static void GLDrawOvalPoints(GLfloat x, GLfloat y, GLfloat z, NSSize siz,
                             GLfloat step) {
  GLfloat ww = static_cast<GLfloat>(0.5 * siz.width);
  GLfloat hh = static_cast<GLfloat>(0.5 * siz.height);
  GLfloat theta;
  GLfloat delta;

  OO_ENTER_OPENGL();

  delta = static_cast<GLfloat>(step * std::numbers::pi / 180.0f);

  for (theta = 0.0f; theta < (2.0f * std::numbers::pi); theta += delta) {
    glVertex3f(static_cast<GLfloat>(x + ww * sin(theta)),
               static_cast<GLfloat>(y + hh * cos(theta)), z);
  }
  glVertex3f(x, y + hh, z);
}

void GLDrawOval(GLfloat x, GLfloat y, GLfloat z, NSSize siz, GLfloat step) {
  OO_ENTER_OPENGL();

  OOGLBEGIN(GL_LINE_STRIP);
  GLDrawOvalPoints(x, y, z, siz, step);
  OOGLEND();
}

void GLDrawFilledOval(GLfloat x, GLfloat y, GLfloat z, NSSize siz,
                      GLfloat step) {
  OO_ENTER_OPENGL();

  OOGLBEGIN(GL_TRIANGLE_FAN);
  GLDrawOvalPoints(x, y, z, siz, step);
  OOGLEND();
}

void GLDrawPoints(std::span<const oo::GLVector> points) {
  OO_ENTER_OPENGL();
  OOGLBEGIN(GL_LINE_STRIP);
  for (const oo::GLVector& point : points) {
    glVertex3f(point.x, point.y, point.z);
  }
  OOGLEND();
  return;
}

void GLDrawFilledPoints(std::span<const oo::GLVector> points) {
  OO_ENTER_OPENGL();
  OOGLBEGIN(GL_TRIANGLE_FAN);
  for (const oo::GLVector& point : points) {
    glVertex3f(point.x, point.y, point.z);
  }
  OOGLEND();
  return;
}

void GLDrawQuadStrip(std::span<const oo::GLVector> points) {
  OO_ENTER_OPENGL();
  OOGLBEGIN(GL_QUAD_STRIP);
  for (const oo::GLVector& point : points) {
    glVertex3f(point.x, point.y, point.z);
  }
  OOGLEND();
  return;
}

void GLScaledLineWidth(GLfloat width) {
  OO_ENTER_OPENGL();
  glLineWidth(width * sDisplayScaleFactor);
}

void GLScaledPointSize(GLfloat size) {
  OO_ENTER_OPENGL();
  glPointSize(size * sDisplayScaleFactor);
}

GLfloat GLGetDisplayScaleFactor() { return sDisplayScaleFactor; }

void GLSetDisplayScaleFactor(GLfloat factor) {
  OOCParameterAssert(factor >= 0.0f && std::isfinite(factor));
  sDisplayScaleFactor = factor;
}

// MARK: LogOpenGLState() and helpers

#ifndef NDEBUG

static void GLDumpLightState(unsigned lightIdx);
static void GLDumpMaterialState();
static void GLDumpCullingState();
static void GLDumpFogState();
static void GLDumpStateFlags();

void LogOpenGLState() {
  unsigned i;

  if (!oo::log::willDisplay(oo::kLogOpenGLStateDump)) return;

  OO_ENTER_OPENGL();

  OO_LOG(oo::kLogOpenGLStateDump, "{}", "OpenGL state dump:");
  const oo::LogIndentScope indent;

  GLDumpMaterialState();
  GLDumpCullingState();
  if (glIsEnabled(GL_LIGHTING)) {
    OO_LOG(oo::kLogOpenGLStateDump, "{}", "Lighting: ENABLED");
    for (i = 0; i != 8; ++i) {
      GLDumpLightState(i);
    }
  } else {
    OO_LOG(oo::kLogOpenGLStateDump, "{}", "Lighting: disabled");
  }

  GLDumpFogState();
  GLDumpStateFlags();

  oo::CheckOpenGLErrors("After state dump");
}

namespace oo {

std::string GLColorToString(GLfloat color[4]) {
#define COLOR_EQUAL(color, r, g, b, a)                            \
  ((color)[0] == (r) && (color)[1] == (g) && (color)[2] == (b) && \
   (color)[3] == (a))
#define COLOR_CASE(r, g, b, a, str)                           \
  do {                                                        \
    if (COLOR_EQUAL(color, (r), (g), (b), (a))) return (str); \
  } while (0)

  COLOR_CASE(1, 1, 1, 1, "white");
  COLOR_CASE(0, 0, 0, 1, "black");
  COLOR_CASE(0, 0, 0, 0, "clear");
  COLOR_CASE(1, 0, 0, 1, "red");
  COLOR_CASE(0, 1, 0, 1, "green");
  COLOR_CASE(0, 0, 1, 1, "blue");
  COLOR_CASE(0, 1, 1, 1, "cyan");
  COLOR_CASE(1, 0, 1, 1, "magenta");
  COLOR_CASE(1, 1, 0, 1, "yellow");

  return std::format("({:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f)", color[0], color[1],
                     color[2], color[3]);
}

}  // namespace oo

static void GLDumpLightState(unsigned lightIdx) {
  bool enabled;
  GLenum lightID = GL_LIGHT0 + lightIdx;
  GLfloat color[4];

  OO_ENTER_OPENGL();

  OOGL(enabled = glIsEnabled(lightID));
  OO_LOG(oo::kLogOpenGLStateDump, "Light {}: {}",
         static_cast<unsigned>(lightIdx), oo::GLFlagToString(enabled));

  if (enabled) {
    const oo::LogIndentScope indent;

    OOGL(glGetLightfv(GL_LIGHT1, GL_AMBIENT, color));
    OO_LOG(oo::kLogOpenGLStateDump, "Ambient: {}", oo::GLColorToString(color));
    OOGL(glGetLightfv(GL_LIGHT1, GL_DIFFUSE, color));
    OO_LOG(oo::kLogOpenGLStateDump, "Diffuse: {}", oo::GLColorToString(color));
    OOGL(glGetLightfv(GL_LIGHT1, GL_SPECULAR, color));
    OO_LOG(oo::kLogOpenGLStateDump, "Specular: {}", oo::GLColorToString(color));
  }
}

static void GLDumpMaterialState() {
  GLfloat color[4];
  GLfloat shininess;
  GLint shadeModel, blendSrc, blendDst, texMode;
  bool blending;

  OO_ENTER_OPENGL();

  OO_LOG(oo::kLogOpenGLStateDump, "{}", "Material state:");
  const oo::LogIndentScope indent;

  OOGL(glGetMaterialfv(GL_FRONT, GL_AMBIENT, color));
  OO_LOG(oo::kLogOpenGLStateDump, "Ambient: {}", oo::GLColorToString(color));

  OOGL(glGetMaterialfv(GL_FRONT, GL_DIFFUSE, color));
  OO_LOG(oo::kLogOpenGLStateDump, "Diffuse: {}", oo::GLColorToString(color));

  OOGL(glGetMaterialfv(GL_FRONT, GL_EMISSION, color));
  OO_LOG(oo::kLogOpenGLStateDump, "Emission: {}", oo::GLColorToString(color));

  OOGL(glGetMaterialfv(GL_FRONT, GL_SPECULAR, color));
  OO_LOG(oo::kLogOpenGLStateDump, "Specular: {}", oo::GLColorToString(color));

  OOGL(glGetMaterialfv(GL_FRONT, GL_SHININESS, &shininess));
  OO_LOG(oo::kLogOpenGLStateDump, "Shininess: {:g}", shininess);

  OOGL(OO_LOG(oo::kLogOpenGLStateDump, "Colour material: {}",
              oo::GLFlagToString(glIsEnabled(GL_COLOR_MATERIAL))));

  OOGL(glGetFloatv(GL_CURRENT_COLOR, color));
  OO_LOG(oo::kLogOpenGLStateDump, "Current color: {}",
         oo::GLColorToString(color));

  OOGL(glGetIntegerv(GL_SHADE_MODEL, &shadeModel));
  OO_LOG(oo::kLogOpenGLStateDump, "Shade model: {}",
         oo::GLEnumToString(static_cast<GLenum>(shadeModel)));

  OOGL(blending = glIsEnabled(GL_BLEND));
  OO_LOG(oo::kLogOpenGLStateDump, "Blending: {}", oo::GLFlagToString(blending));
  if (blending) {
    OOGL(glGetIntegerv(GL_BLEND_SRC, &blendSrc));
    OOGL(glGetIntegerv(GL_BLEND_DST, &blendDst));
    OO_LOG(oo::kLogOpenGLStateDump, "Blend function: {}, {}",
           oo::GLEnumToString(static_cast<GLenum>(blendSrc)),
           oo::GLEnumToString(static_cast<GLenum>(blendDst)));
  }

  OOGL(glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texMode));
  OO_LOG(oo::kLogOpenGLStateDump, "Texture env mode: {}",
         oo::GLEnumToString(static_cast<GLenum>(texMode)));

#if OO_MULTITEXTURE
  if (oo::OpenGLExtensionManager::sharedManager()->textureUnitCount() > 1) {
    GLint textureUnit;
    OOGL(glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &textureUnit));
    OO_LOG(oo::kLogOpenGLStateDump, "Active texture unit: {}",
           oo::GLEnumToString(static_cast<GLenum>(textureUnit)));

    OOGL(glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE_ARB, &textureUnit));
    OO_LOG(oo::kLogOpenGLStateDump, "Active client texture unit: {}",
           oo::GLEnumToString(static_cast<GLenum>(textureUnit)));
  }
#endif
}

static void GLDumpCullingState() {
  OO_ENTER_OPENGL();

  bool enabled;
  OOGL(enabled = glIsEnabled(GL_CULL_FACE));
  OO_LOG(oo::kLogOpenGLStateDump, "Face culling: {}",
         oo::GLFlagToString(enabled));
  if (enabled) {
    GLint value;

    const oo::LogIndentScope indent;

    OOGL(glGetIntegerv(GL_CULL_FACE_MODE, &value));
    OO_LOG(oo::kLogOpenGLStateDump, "Cull face mode: {}",
           oo::GLEnumToString(static_cast<GLenum>(value)));

    OOGL(glGetIntegerv(GL_FRONT_FACE, &value));
    OO_LOG(oo::kLogOpenGLStateDump, "Front face direction: {}",
           oo::GLEnumToString(static_cast<GLenum>(value)));
  }
}

static void GLDumpFogState() {
  bool enabled;
  GLint value;
  GLfloat start, end, density, index;
  GLfloat color[4];

  OO_ENTER_OPENGL();

  OOGL(enabled = glIsEnabled(GL_FOG));
  OO_LOG(oo::kLogOpenGLStateDump, "Fog: {}", oo::GLFlagToString(enabled));
  if (enabled) {
    const oo::LogIndentScope indent;

    OOGL(glGetIntegerv(GL_FOG_MODE, &value));
    OO_LOG(oo::kLogOpenGLStateDump, "Fog mode: {}",
           oo::GLEnumToString(static_cast<GLenum>(value)));

    OOGL(glGetFloatv(GL_FOG_COLOR, color));
    OO_LOG(oo::kLogOpenGLStateDump, "Fog colour: {}",
           oo::GLColorToString(color));

    OOGL(glGetFloatv(GL_FOG_START, &start));
    OOGL(glGetFloatv(GL_FOG_END, &end));
    OO_LOG(oo::kLogOpenGLStateDump, "Fog start, end: {:g}, {:g}", start, end);

    OOGL(glGetFloatv(GL_FOG_DENSITY, &density));
    OO_LOG(oo::kLogOpenGLStateDump, "Fog density: {:g}", density);

    OOGL(glGetFloatv(GL_FOG_DENSITY, &index));
    OO_LOG(oo::kLogOpenGLStateDump, "Fog index: {:g}", index);
  }
}

static void GLDumpStateFlags() {
  OO_ENTER_OPENGL();

#define DUMP_STATE_FLAG(x) \
  OO_LOG(oo::kLogOpenGLStateDump, #x ": {}", oo::GLFlagToString(glIsEnabled(x)))
#define DUMP_GET_FLAG(x)                                                  \
  do {                                                                    \
    GLboolean flag;                                                       \
    glGetBooleanv(x, &flag);                                              \
    OO_LOG(oo::kLogOpenGLStateDump, #x ": {}", oo::GLFlagToString(flag)); \
  } while (0)

  OO_LOG(oo::kLogOpenGLStateDump, "{}", "Selected state flags:");
  const oo::LogIndentScope indent;

  DUMP_STATE_FLAG(GL_VERTEX_ARRAY);
  DUMP_STATE_FLAG(GL_NORMAL_ARRAY);
  DUMP_STATE_FLAG(GL_TEXTURE_COORD_ARRAY);
  DUMP_STATE_FLAG(GL_COLOR_ARRAY);
  DUMP_STATE_FLAG(GL_TEXTURE_2D);
  DUMP_STATE_FLAG(GL_DEPTH_TEST);
  DUMP_GET_FLAG(GL_DEPTH_WRITEMASK);

#undef DUMP_STATE_FLAG
}

#define CASE(x) \
  case x:       \
    return #x

namespace oo {

std::string GLEnumToString(GLenum value) {
  switch (value) {
    // ShadingModel
    CASE(GL_FLAT);
    CASE(GL_SMOOTH);

    // BlendingFactorSrc/BlendingFactorDest
    CASE(GL_ZERO);
    CASE(GL_ONE);
    CASE(GL_DST_COLOR);
    CASE(GL_SRC_COLOR);
    CASE(GL_ONE_MINUS_DST_COLOR);
    CASE(GL_ONE_MINUS_SRC_COLOR);
    CASE(GL_SRC_ALPHA);
    CASE(GL_DST_ALPHA);
    CASE(GL_ONE_MINUS_SRC_ALPHA);
    CASE(GL_ONE_MINUS_DST_ALPHA);
    CASE(GL_SRC_ALPHA_SATURATE);

    // TextureEnvMode
    CASE(GL_MODULATE);
    CASE(GL_DECAL);
    CASE(GL_BLEND);
    CASE(GL_REPLACE);

    // FrontFaceDirection
    CASE(GL_CW);
    CASE(GL_CCW);

    // CullFaceMode
    CASE(GL_FRONT);
    CASE(GL_BACK);
    CASE(GL_FRONT_AND_BACK);

    // FogMode
    CASE(GL_LINEAR);
    CASE(GL_EXP);
    CASE(GL_EXP2);

#if OO_MULTITEXTURE
    // Texture units
#ifdef GL_TEXTURE0
#define TEXCASE CASE
#else
#define TEXCASE(x) CASE(x##_ARB)
#endif
    TEXCASE(GL_TEXTURE0);
    TEXCASE(GL_TEXTURE1);
    TEXCASE(GL_TEXTURE2);
    TEXCASE(GL_TEXTURE3);
    TEXCASE(GL_TEXTURE4);
    TEXCASE(GL_TEXTURE5);
    TEXCASE(GL_TEXTURE6);
    TEXCASE(GL_TEXTURE7);
    TEXCASE(GL_TEXTURE8);
    TEXCASE(GL_TEXTURE9);
    TEXCASE(GL_TEXTURE10);
    TEXCASE(GL_TEXTURE11);
    TEXCASE(GL_TEXTURE12);
    TEXCASE(GL_TEXTURE13);
    TEXCASE(GL_TEXTURE14);
    TEXCASE(GL_TEXTURE15);
#endif

    default:
      return std::format("unknown: {}", value);
  }
}

std::string GLFlagToString(bool value) {
  return value ? "ENABLED" : "disabled";
}

}  // namespace oo

#else

void LogOpenGLState(void) {}

#endif
