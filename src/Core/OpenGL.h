/*

OOOpenGL.h

Do whatever is appropriate to get gl.h, glu.h and glext.h included.

Also declares OpenGL-related utility functions.


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

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "EnumFlags.h"
#include "Foundation.h"
#include "OpenGLCheck.h"
#include "OpenGLOnly.h"

namespace oo {

enum class ShaderSetting : std::int32_t {
  // NOTE: numerical values are available to scripts.
  kNotSupported = 0,
  kOff = 1,
  kSimple = 2,
  kFull = 3
};

}  // namespace oo

// The numbers the GPU configuration and scripts use.
static_assert(oo::ToUnderlying(oo::ShaderSetting::kNotSupported) == 0);
static_assert(oo::ToUnderlying(oo::ShaderSetting::kOff) == 1);
static_assert(oo::ToUnderlying(oo::ShaderSetting::kSimple) == 2);
static_assert(oo::ToUnderlying(oo::ShaderSetting::kFull) == 3);

namespace oo {

// The names the game writes for OOShaderSetting (saves, logs and plists).
constexpr std::string_view OOEnumName(ShaderSetting value) noexcept {
  switch (value) {
    case ShaderSetting::kOff:
      return "SHADERS_OFF";
    case ShaderSetting::kSimple:
      return "SHADERS_SIMPLE";
    case ShaderSetting::kFull:
      return "SHADERS_FULL";
    case ShaderSetting::kNotSupported:
      return "SHADERS_NOT_SUPPORTED";
  }
  return {};
}

}  // namespace oo

inline constexpr auto NULL_SHADER = (static_cast<GLhandleARB>(0));

// GLhandleARB is a pointer type on macOS and an integer elsewhere; convert it
// to a GLuint for the core GL 2.0+ shader API.
namespace oo {

template <class Handle>
GLuint GLProgramName(Handle handle) {
  if constexpr (std::is_pointer_v<Handle>) {
    return static_cast<GLuint>(reinterpret_cast<std::uintptr_t>(handle));
  } else {
    return static_cast<GLuint>(handle);
  }
}

/*
        OOSetOpenGLState(stateType)

        Set OpenGL state to one of two standard states.

        In Deployment builds, this only modifies the state that differs between
        the two standard states, and only when the last set state is not the
   same as the specified target.

        In Debug builds, it tests many different OpenGL state variables,
   complains if they aren't in the expected state, and corrects them.


        The correct procedure for using these functions is:
          1. Call OOSetOpenGLState(<most appropriate state>)
          2. Make any state changes needed
          3. Draw
          4. Reverse state changes from stage 2
          5. Call OOVerifyOpenGLState()
          6. Call OOCheckOpenGLErrors().


        The states are:
        OPENGL_STATE_OPAQUE - standard state for drawing solid objects like
   OOMesh and planets. This can be considered the Oolite baseline state. Differs
   from standard GL state as follows: GL_LIGHTING is on GL_LIGHT1 is on
                GL_TEXTURE_2D is on
                Blend mode is GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA (but GL_BLEND
   is off) GL_FOG may be either on or off (as it's set in Universe's draw loop)
                GL_VERTEX_ARRAY is on
                GL_NORMAL_ARRAY is on
                GL_DEPTH_TEST is on
                GL_CULL_FACE is on

        OPENGL_STATE_TRANSLUCENT_PASS is used during the translucent phase of
   the universe draw loop. This usage needs to be cleaned up. Differs from
   OPENGL_STATE_OPAQUE as follows: GL_LIGHTING is off GL_TEXTURE_2D is off
                GL_VERTEX_ARRAY is off
                GL_NORMAL_ARRAY is off
                GL_DEPTH_WRITEMASK is off

        OPENGL_STATE_ADDITIVE_BLENDING is used for glowy special effects.
        Differs from OPENGL_STATE_OPAQUE as follows:
                GL_LIGHTING is off
                GL_TEXTURE_2D is off
                GL_BLEND is on
                Blend mode is GL_SRC_ALPHA, GL_ONE
                GL_NORMAL_ARRAY is off
                GL_DEPTH_WRITEMASK is off
                GL_CULL_FACE is off

        OPENGL_STATE_OVERLAY is used for UI elements, which are unlit and don't
   use z-buffering. Differs from OPENGL_STATE_OPAQUE as follows: GL_LIGHTING is
   off GL_TEXTURE_2D is off GL_BLEND is on GL_FOG is off GL_NORMAL_ARRAY is off
                GL_DEPTH_TEST is off
                GL_DEPTH_WRITEMASK is off
                GL_CULL_FACE is off
*/
enum class OpenGLStateID : int {
  kOpaque,
  kTranslucentPass,
  kAdditiveBlending,
  kOverlay,

  kInternalUseOnly,
};

}  // namespace oo

static_assert(std::to_underlying(oo::OpenGLStateID::kOpaque) == 0);
static_assert(std::to_underlying(oo::OpenGLStateID::kTranslucentPass) == 1);
static_assert(std::to_underlying(oo::OpenGLStateID::kAdditiveBlending) == 2);
static_assert(std::to_underlying(oo::OpenGLStateID::kOverlay) == 3);
static_assert(std::to_underlying(oo::OpenGLStateID::kInternalUseOnly) == 4);

#if OO_GL_STATE_VERIFICATION
namespace oo {

void SetOpenGLState_(OpenGLStateID state, const char* function, unsigned line);
void VerifyOpenGLState_(const char* function, unsigned line);

}  // namespace oo

#define OOSetOpenGLState(STATE) \
  oo::SetOpenGLState_(STATE, __FUNCTION__, __LINE__)
#define OOVerifyOpenGLState() oo::VerifyOpenGLState_(__FUNCTION__, __LINE__)
#else
void OOSetOpenGLState(oo::OpenGLStateID state);
#define OOVerifyOpenGLState() \
  do {                        \
  } while (0)
#endif

// Inform the SetState/VerifyState mechanism that the OpenGL context has been
// reset to its initial state.
namespace oo {

void ResetGLStateVerifier();

}  // namespace oo

/*	LogOpenGLState()
        Write a bunch of OpenGL state information to the log.
*/
void LogOpenGLState();

/*	GLScaledLineWidth()
        GLScaledPointSize()
        GLGetDisplayScaleFactor()
        GLSetDisplayScaleFactor()

        These functions wrap glLineWidth() and glPointSize(), and multiply the
        specified size by a "display scale factor". This is currently used to
        support Retina display modes in Mac OS X 10.7 and later.

        The default display scale factor is 1.0.
*/
void GLScaledLineWidth(GLfloat width);
void GLScaledPointSize(GLfloat size);
GLfloat GLGetDisplayScaleFactor();
void GLSetDisplayScaleFactor(GLfloat factor);

/*	OOGLWireframeModeOn()
        OOGLWireframeModeOff()
        Enable/disable polygon-to-lines wireframe rendering with line width
   of 1.
*/
namespace oo {

void GLWireframeModeOn();
void GLWireframeModeOff();

}  // namespace oo

/*	GLDrawBallBillboard()
        Draws a circle corresponding to a sphere of given radius at given
   distance. Assumes Z buffering will be disabled.
*/
void GLDrawBallBillboard(GLfloat radius, GLfloat step, GLfloat z_distance);

/*	GLDrawOval(), GLDrawFilledOval()
        Draw axis-alligned ellipses, as outline and fill respectively.
*/
void GLDrawOval(GLfloat x, GLfloat y, GLfloat z, NSSize siz, GLfloat step);
void GLDrawFilledOval(GLfloat x, GLfloat y, GLfloat z, NSSize siz,
                      GLfloat step);

/*	GLDrawPoints(), GLDrawFilledPoints()
        Draw array of points, as outline and fill respectively.
*/

namespace oo {

struct GLVector {
  GLfloat x;
  GLfloat y;
  GLfloat z;
};

}  // namespace oo

void GLDrawPoints(std::span<const oo::GLVector> points);
void GLDrawFilledPoints(std::span<const oo::GLVector> points);
void GLDrawQuadStrip(std::span<const oo::GLVector> points);

namespace oo {

inline constexpr ShaderSetting kShaderSettingDefault =
    ShaderSetting::kNotSupported;

}  // namespace oo

// The shader mode strings (OOShaderSettingFromString() and friends) are
// declared in OOConstToString.h.

#ifndef NDEBUG

namespace oo {

std::string GLColorToString(GLfloat color[4]);
std::string GLEnumToString(GLenum value);
std::string GLFlagToString(bool value);

}  // namespace oo

#endif
