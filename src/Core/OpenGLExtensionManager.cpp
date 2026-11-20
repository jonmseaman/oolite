/*

OOOpenGLExtensionManager.m

Copyright (C) 2007-2013 Jens Ayton and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "OpenGLExtensionManager.h"

#include <stdlib.h>

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ConstToString.h"
#include "FunctionAttributes.h"
#include "Headless.h"
#include "RegExpMatcher.h"
#include "ResourceManager.h"
#include "oofnd/Exception.hpp"
#include "oofnd/Log.hpp"
#include "oofnd/Process.hpp"
#include "oofnd/String.hpp"

/*	OpenGL version required, currently 1.1 or later (basic stuff like
        glBindTexture(), glDrawArrays()). We probably have implicit requirements
        for later versions, but I don't feel like auditing.
        -- Ahruman
        We need at least 3.0 for the Frame Buffer Objects now in use. Might as
        well go all the way to 3.3.
        -- Nikos 20220817
*/
constexpr int kMinMajorVersion = 3;
constexpr int kMinMinorVersion = 3;

#if OOLITE_WINDOWS
/*	Define the function pointers for the OpenGL extensions used in the game
        (required for Windows only).
*/
namespace oo {

static void BadOpenGLExtensionUsed() GCC_ATTR((noreturn, used));

}  // namespace oo

#if OO_SHADERS

PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB =
    reinterpret_cast<PFNGLUSEPROGRAMOBJECTARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB =
    reinterpret_cast<PFNGLGETUNIFORMLOCATIONARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM1IARBPROC glUniform1iARB =
    reinterpret_cast<PFNGLUNIFORM1IARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM1FARBPROC glUniform1fARB =
    reinterpret_cast<PFNGLUNIFORM1FARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB =
    reinterpret_cast<PFNGLUNIFORMMATRIX3FVARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB =
    reinterpret_cast<PFNGLUNIFORMMATRIX4FVARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM4FVARBPROC glUniform4fvARB =
    reinterpret_cast<PFNGLUNIFORM4FVARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB =
    reinterpret_cast<PFNGLGETOBJECTPARAMETERIVARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB =
    reinterpret_cast<PFNGLCREATESHADEROBJECTARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLGETINFOLOGARBPROC glGetInfoLogARB =
    reinterpret_cast<PFNGLGETINFOLOGARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB =
    reinterpret_cast<PFNGLCREATEPROGRAMOBJECTARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLATTACHOBJECTARBPROC glAttachObjectARB =
    reinterpret_cast<PFNGLATTACHOBJECTARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB =
    reinterpret_cast<PFNGLDELETEOBJECTARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLLINKPROGRAMARBPROC glLinkProgramARB =
    reinterpret_cast<PFNGLLINKPROGRAMARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLCOMPILESHADERARBPROC glCompileShaderARB =
    reinterpret_cast<PFNGLCOMPILESHADERARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLSHADERSOURCEARBPROC glShaderSourceARB =
    reinterpret_cast<PFNGLSHADERSOURCEARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM2FVARBPROC glUniform2fvARB =
    reinterpret_cast<PFNGLUNIFORM2FVARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB =
    reinterpret_cast<PFNGLBINDATTRIBLOCATIONARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB =
    reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB =
    reinterpret_cast<PFNGLVERTEXATTRIBPOINTERARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB =
    reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYARBPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB =
    reinterpret_cast<PFNGLVALIDATEPROGRAMARBPROC>(&oo::BadOpenGLExtensionUsed);
#endif

#if OO_SHADERS || OO_MULTITEXTURE
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB =
    reinterpret_cast<PFNGLACTIVETEXTUREARBPROC>(&oo::BadOpenGLExtensionUsed);
#endif

#if OO_MULTITEXTURE
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB =
    reinterpret_cast<PFNGLCLIENTACTIVETEXTUREARBPROC>(
        &oo::BadOpenGLExtensionUsed);
#endif

#if OO_USE_VBO
PFNGLGENBUFFERSARBPROC glGenBuffersARB =
    reinterpret_cast<PFNGLGENBUFFERSARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB =
    reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDBUFFERARBPROC glBindBufferARB =
    reinterpret_cast<PFNGLBINDBUFFERARBPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBUFFERDATAARBPROC glBufferDataARB =
    reinterpret_cast<PFNGLBUFFERDATAARBPROC>(&oo::BadOpenGLExtensionUsed);
#endif

#if OO_USE_FBO
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT =
    reinterpret_cast<PFNGLGENFRAMEBUFFERSEXTPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT =
    reinterpret_cast<PFNGLBINDFRAMEBUFFEREXTPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT =
    reinterpret_cast<PFNGLGENRENDERBUFFERSEXTPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT =
    reinterpret_cast<PFNGLBINDRENDERBUFFEREXTPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT =
    reinterpret_cast<PFNGLRENDERBUFFERSTORAGEEXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT =
    reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT =
    reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DEXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT =
    reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT =
    reinterpret_cast<PFNGLDELETEFRAMEBUFFERSEXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT =
    reinterpret_cast<PFNGLDELETERENDERBUFFERSEXTPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers =
    reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer =
    reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage =
    reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers =
    reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer =
    reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer =
    reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D =
    reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays =
    reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGENBUFFERSPROC glGenBuffers =
    reinterpret_cast<PFNGLGENBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDVERTEXARRAYPROC glBindVertexArray =
    reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBINDBUFFERPROC glBindBuffer =
    reinterpret_cast<PFNGLBINDBUFFERPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBUFFERDATAPROC glBufferData =
    reinterpret_cast<PFNGLBUFFERDATAPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer =
    reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray =
    reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLUSEPROGRAMPROC glUseProgram =
    reinterpret_cast<PFNGLUSEPROGRAMPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation =
    reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM1IPROC glUniform1i =
    reinterpret_cast<PFNGLUNIFORM1IPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLACTIVETEXTUREPROC glActiveTexture =
    reinterpret_cast<PFNGLACTIVETEXTUREPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate =
    reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM1FPROC glUniform1f =
    reinterpret_cast<PFNGLUNIFORM1FPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLUNIFORM2FVPROC glUniform2fv =
    reinterpret_cast<PFNGLUNIFORM2FVPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers =
    reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers =
    reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays =
    reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDELETEBUFFERSPROC glDeleteBuffers =
    reinterpret_cast<PFNGLDELETEBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLDRAWBUFFERSPROC glDrawBuffers =
    reinterpret_cast<PFNGLDRAWBUFFERSPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus =
    reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample =
    reinterpret_cast<PFNGLTEXIMAGE2DMULTISAMPLEPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample =
    reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(
        &oo::BadOpenGLExtensionUsed);
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer =
    reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(&oo::BadOpenGLExtensionUsed);
PFNGLCLAMPCOLORPROC glClampColor =
    reinterpret_cast<PFNGLCLAMPCOLORPROC>(&oo::BadOpenGLExtensionUsed);
#endif
#endif

namespace {
constexpr const char* kLogOpenGLShaderSupport =
    "rendering.opengl.shader.support";
}

namespace {

oo::OpenGLExtensionManager* sSingleton =
    nullptr;  // +1, never released (the retained singleton)

}  // namespace

// Read integer from string, advancing string to end of read data.
static unsigned IntegerFromString(const GLubyte** ioString);

namespace {

std::vector<std::string> ArrayOfExtensions(
    const std::optional<std::string>& extensionString) {
  std::vector<std::string> result;
  if (!extensionString.has_value()) return result;
  for (std::string& extStr : oo::str::split(*extensionString, " ")) {
    if (!extStr.empty()) result.push_back(std::move(extStr));
  }
  return result;
}

// What -initWithUTF8String: gave for a glGetString() result: nil for NULL.
std::optional<std::string> OptionalGLString(const GLubyte* string) {
  if (string == nullptr) return std::nullopt;
  return std::string(reinterpret_cast<const char*>(string));
}

}  // namespace

namespace {
bool sHeadless = false;  // OOOpenGLExtensionManager::setHeadless
}  // namespace

void oo::OpenGLExtensionManager::setHeadless(bool headless) {
  sHeadless = headless;
}

bool oo::OpenGLExtensionManager::isHeadless() { return sHeadless; }

// Declared in OOHeadless.h; defined here, beside the state it reads, so that
// whatever links the manager links this too.
namespace oo {

bool GraphicsAvailable() { return !sHeadless; }

}  // namespace oo

void oo::OpenGLExtensionManager::reset() {
  const GLubyte *versionString = nullptr, *curr = nullptr;

  if (sHeadless) {
    // No context: nothing to ask the driver, and nothing is supported.
    extensions.clear();
    vendor.reset();
    renderer.reset();
    major = minor = release_ = 0;
#if OO_SHADERS
    shadersAvailable = false;
    defaultShaderSetting = oo::ShaderSetting::kNotSupported;
    maximumShaderSetting = oo::ShaderSetting::kNotSupported;
    textureImageUnitCount_ = 0;
#endif
#if OO_USE_VBO
    vboSupported_ = false;
#endif
#if OO_USE_FBO
    fboSupported_ = false;
#endif
#if OO_MULTITEXTURE
    textureCombinersSupported_ = false;
    textureUnitCount_ = 0;
#endif
    usePointSmoothing_ = false;
    useLineSmoothing_ = false;
    useDustShader_ = false;
    ResourceManager::paths();  // as below, for the log's order
    OO_LOG("rendering.opengl.version", "{}",
           "OpenGL: none (a headless game has no context).");
    return;
  }

  const std::optional<std::string> extensionsStr =
      OptionalGLString(glGetString(GL_EXTENSIONS));
  {
    const std::vector<std::string> extensionList =
        ArrayOfExtensions(extensionsStr);
    extensions =
        std::set<std::string>(extensionList.begin(), extensionList.end());
  }

  vendor = OptionalGLString(glGetString(GL_VENDOR));
  renderer = OptionalGLString(glGetString(GL_RENDERER));

  versionString = glGetString(GL_VERSION);
  if (versionString != nullptr) {
    /*	String is supposed to be "major.minorFOO" or
     "major.minor.releaseFOO" where FOO is an empty string or
     a string beginning with space.
     */
    curr = versionString;
    major = IntegerFromString(&curr);
    if (*curr == '.') {
      curr++;
      minor = IntegerFromString(&curr);
    }
    if (*curr == '.') {
      curr++;
      release_ = IntegerFromString(&curr);
    }
  }

  /*	For aesthetic reasons, cause the ResourceManager to initialize its
   search paths here. If we don't, the search path dump ends up in
   the middle of the OpenGL stuff.
   */
  ResourceManager::paths();

  OO_LOG(
      "rendering.opengl.version",
      "OpenGL renderer version: {}.{}.{} (\"{}\"). Vendor: \"{}\". Renderer: "
      "\"{}\".",
      major, minor, release_,
      versionString ? reinterpret_cast<const char*>(versionString) : "(null)",
      vendor.value_or("(null)"), renderer.value_or("(null)"));
  {
    // Listed in byte order of the name (the set's hash order before).
    std::string extensionList;
    for (const std::string& extension : extensions) {
      if (!extensionList.empty()) extensionList += ", ";
      extensionList += extension;
    }
    OO_LOG("rendering.opengl.extensions", "OpenGL extensions ({}):\n{}",
           extensions.size(), extensionList);
  }

#if OOLITE_MACOS
  /*	macOS only offers a 2.1 legacy context alongside fixed-function GL, but
          it provides what Oolite needs from 3.3 as extensions.
  */
  if (versionIsAtLeastMajor(2, 1) &&
      haveExtension("GL_ARB_framebuffer_object") &&
      haveExtension("GL_APPLE_vertex_array_object") &&
      haveExtension("GL_ARB_texture_float") &&
      haveExtension("GL_ARB_color_buffer_float") &&
      haveExtension("GL_ARB_draw_buffers")) {
    // OK
  } else
#endif
      if (!versionIsAtLeastMajor(kMinMajorVersion, kMinMinorVersion)) {
    OO_LOG("rendering.opengl.version.insufficient",
           "***** Oolite requires OpenGL version {}.{} or later.",
           static_cast<unsigned>(kMinMajorVersion),
           static_cast<unsigned>(kMinMinorVersion));
    oo::RaiseException(
        "OoliteOpenGLTooOldException",
        "Oolite requires at least OpenGL %u.%u. You have %u.%u (\"%s\").",
        kMinMajorVersion, kMinMinorVersion, major, minor, versionString);
  }

  const std::optional<std::string> versionStr = OptionalGLString(versionString);
  const oo::PList gpuConfig =
      lookUpPerGPUSettingsWithVersionString(versionStr, extensionsStr);

#if OO_SHADERS
  checkShadersSupported();

  if (shadersAvailable) {
    defaultShaderSetting = oo::ShaderSettingFromString(
        gpuConfig.get<std::string>("default_shader_level", "SHADERS_FULL"));
    maximumShaderSetting = oo::ShaderSettingFromString(
        gpuConfig.get<std::string>("maximum_shader_level", "SHADERS_FULL"));
    if (maximumShaderSetting <= oo::ShaderSetting::kOff) {
      shadersAvailable = false;
      maximumShaderSetting = oo::ShaderSetting::kNotSupported;
      {
        // A name that is not a string fell back to the renderer, which may
        // itself be nil.
        const oo::PList* name = gpuConfig.find("name");
        const bool hasName =
            name != nullptr && (name->isString() || name->isNumber());
        OO_LOG(kLogOpenGLShaderSupport,
               "Shaders will not be used (disallowed for GPU type \"{}\").",
               hasName ? gpuConfig.get<std::string>("name")
                       : renderer.value_or("(null)"));
      }
    }
    if (maximumShaderSetting < defaultShaderSetting) {
      defaultShaderSetting = maximumShaderSetting;
    }

    if (shadersAvailable) {
      OO_LOG(kLogOpenGLShaderSupport, "{}", "Shaders are supported.");
    }
  } else {
    defaultShaderSetting = oo::ShaderSetting::kNotSupported;
    maximumShaderSetting = oo::ShaderSetting::kNotSupported;
  }

  GLint texImageUnitOverride =
      gpuConfig.get<int>("texture_image_units", textureImageUnitCount_);
  if (texImageUnitOverride < textureImageUnitCount_)
    textureImageUnitCount_ = std::max(texImageUnitOverride, 0);
#endif

#if OO_USE_VBO
  checkVBOSupported();
#endif
#if OO_USE_FBO
  checkFBOSupported();
#endif
#if OO_MULTITEXTURE
  checkTextureCombinersSupported();
  GLint texUnitOverride =
      gpuConfig.get<int>("texture_units", textureUnitCount_);
  if (texUnitOverride < textureUnitCount_)
    textureUnitCount_ = std::max(texUnitOverride, 0);
#endif

  usePointSmoothing_ =
      gpuConfig.get<bool>("smooth_points", true) ? true : false;
  useLineSmoothing_ = gpuConfig.get<bool>("smooth_lines", true) ? true : false;
  useDustShader_ = gpuConfig.get<bool>("use_dust_shader", true) ? true : false;
}

namespace oo {

OpenGLExtensionManager::~OpenGLExtensionManager() {
  if (sSingleton == this) sSingleton = nullptr;
}

}  // namespace oo

oo::OpenGLExtensionManager* oo::OpenGLExtensionManager::sharedManager() {
  // NOTE: assumes single-threaded first access. See header.
  if (sSingleton == nullptr) {
    // The singleton category's +allocWithZone: recorded the instance before
    // -init ran -reset, so it is recorded here before reset() runs too.
    static std::shared_ptr<oo::OpenGLExtensionManager>* sSingletonHolder =
        nullptr;  // never deleted
    sSingletonHolder = new std::shared_ptr<oo::OpenGLExtensionManager>(
        oo::makeShared<oo::OpenGLExtensionManager>());
    sSingleton = sSingletonHolder->get();
    sSingleton->reset();
  }
  return sSingleton;
}

bool oo::OpenGLExtensionManager::haveExtension(const std::string& extension) {
  // The Foundation set was documented as thread-safe under OS X, but I'm not
  // sure about GNUstep. -- Ahruman
  const std::lock_guard guard(lock);
  return extensions.contains(extension);
}

bool oo::OpenGLExtensionManager::shadersSupported() {
#if OO_SHADERS
  return shadersAvailable;
#else
  return false;
#endif
}

bool oo::OpenGLExtensionManager::shadersForceDisabled() {
#if OO_SHADERS
  return shadersForceDisabled_;
#else
  return true;
#endif
}

oo::GraphicsDetail oo::OpenGLExtensionManager::defaultDetailLevel() {
#if OO_SHADERS
  if (defaultShaderSetting < oo::ShaderSetting::kFull) {
    return oo::GraphicsDetail::kMinimum;
  } else {
    return oo::kGraphicsDetailMaximum;
  }
#else
  return oo::ShaderSetting::kNotSupported;
#endif
}

oo::GraphicsDetail oo::OpenGLExtensionManager::maximumDetailLevel() {
#if OO_SHADERS
  if (maximumShaderSetting < oo::ShaderSetting::kFull) {
    return oo::GraphicsDetail::kMinimum;
  } else {
    return oo::kGraphicsDetailMaximum;
  }
#else
  return oo::GraphicsDetail::kMinimum;
#endif
}

GLint oo::OpenGLExtensionManager::textureImageUnitCount() {
#if OO_SHADERS
  return textureImageUnitCount_;
#else
  return 0;
#endif
}

bool oo::OpenGLExtensionManager::vboSupported() {
#if OO_USE_VBO
  return vboSupported_;
#else
  return false;
#endif
}

bool oo::OpenGLExtensionManager::fboSupported() {
#if OO_USE_FBO
  return fboSupported_;
#else
  return false;
#endif
}

bool oo::OpenGLExtensionManager::textureCombinersSupported() {
#if OO_MULTITEXTURE
  return textureCombinersSupported_;
#else
  return false;
#endif
}

GLint oo::OpenGLExtensionManager::textureUnitCount() {
#if OO_MULTITEXTURE
  return textureUnitCount_;
#else
  return 0;
#endif
}

std::size_t oo::OpenGLExtensionManager::majorVersionNumber() { return major; }

std::size_t oo::OpenGLExtensionManager::minorVersionNumber() { return minor; }

std::size_t oo::OpenGLExtensionManager::releaseVersionNumber() {
  return release_;
}

oo::OpenGLExtensionManager::Version oo::OpenGLExtensionManager::version() {
  return {major, minor, release_};
}

bool oo::OpenGLExtensionManager::versionIsAtLeastMajor(unsigned maj,
                                                       unsigned min) {
  return major > maj || (major == maj && minor >= min);
}

std::optional<std::string> oo::OpenGLExtensionManager::vendorString() {
  return vendor;
}

std::optional<std::string> oo::OpenGLExtensionManager::rendererString() {
  return renderer;
}

bool oo::OpenGLExtensionManager::usePointSmoothing() {
  return usePointSmoothing_;
}

bool oo::OpenGLExtensionManager::useLineSmoothing() {
  return useLineSmoothing_;
}

bool oo::OpenGLExtensionManager::useDustShader() { return useDustShader_; }

static unsigned IntegerFromString(const GLubyte** ioString) {
  if (ioString == nullptr) [[unlikely]]
    return 0;

  unsigned result = 0;
  const GLubyte* curr = *ioString;

  while ('0' <= *curr && *curr <= '9') {
    result = result * 10 + *curr++ - '0';
  }

  *ioString = curr;
  return result;
}

#if OO_SHADERS

/**
 * \ingroup cli
 * Scans the command line for -noshaders or --noshaders arguments.
 */
void oo::OpenGLExtensionManager::checkShadersSupported() {
  shadersAvailable = false;
  shadersForceDisabled_ = false;

  /* Some cards claim to support shaders but do so extremely
   * badly. These are listed in gpu-settings.plist where we know
   * about them; for those we don't being able to run with
   * -noshaders may help get the game up and running at a frame rate
   * where thegraphics settings can be changed.  - CIM */
  // scan for shader overrides: -noshaders || --noshaders
  for (const std::string& arg : oo::process::arguments()) {
    if (arg == "-noshaders" || arg == "--noshaders") {
      shadersForceDisabled_ = true;
      OO_LOG(kLogOpenGLShaderSupport, "{}",
             "Shaders will not be used (disabled on command line).");
      return;
    }
  }

  static constexpr std::array<std::string_view, 5> requiredExtension = {
      "GL_ARB_shading_language_100", "GL_ARB_fragment_shader",
      "GL_ARB_vertex_shader",        "GL_ARB_multitexture",
      "GL_ARB_shader_objects",
  };

  for (std::string_view required : requiredExtension) {
    if (!haveExtension(std::string(required))) {
      OO_LOG(kLogOpenGLShaderSupport,
             "Shaders will not be used (OpenGL extension {} is not available).",
             required);
      return;
    }
  }

#if OOLITE_WINDOWS
  glGetObjectParameterivARB =
      reinterpret_cast<PFNGLGETOBJECTPARAMETERIVARBPROC>(
          wglGetProcAddress("glGetObjectParameterivARB"));
  glCreateShaderObjectARB = reinterpret_cast<PFNGLCREATESHADEROBJECTARBPROC>(
      wglGetProcAddress("glCreateShaderObjectARB"));
  glGetInfoLogARB = reinterpret_cast<PFNGLGETINFOLOGARBPROC>(
      wglGetProcAddress("glGetInfoLogARB"));
  glCreateProgramObjectARB = reinterpret_cast<PFNGLCREATEPROGRAMOBJECTARBPROC>(
      wglGetProcAddress("glCreateProgramObjectARB"));
  glAttachObjectARB = reinterpret_cast<PFNGLATTACHOBJECTARBPROC>(
      wglGetProcAddress("glAttachObjectARB"));
  glDeleteObjectARB = reinterpret_cast<PFNGLDELETEOBJECTARBPROC>(
      wglGetProcAddress("glDeleteObjectARB"));
  glLinkProgramARB = reinterpret_cast<PFNGLLINKPROGRAMARBPROC>(
      wglGetProcAddress("glLinkProgramARB"));
  glCompileShaderARB = reinterpret_cast<PFNGLCOMPILESHADERARBPROC>(
      wglGetProcAddress("glCompileShaderARB"));
  glShaderSourceARB = reinterpret_cast<PFNGLSHADERSOURCEARBPROC>(
      wglGetProcAddress("glShaderSourceARB"));
  glUseProgramObjectARB = reinterpret_cast<PFNGLUSEPROGRAMOBJECTARBPROC>(
      wglGetProcAddress("glUseProgramObjectARB"));
  glActiveTextureARB = reinterpret_cast<PFNGLACTIVETEXTUREARBPROC>(
      wglGetProcAddress("glActiveTextureARB"));
  glGetUniformLocationARB = reinterpret_cast<PFNGLGETUNIFORMLOCATIONARBPROC>(
      wglGetProcAddress("glGetUniformLocationARB"));
  glUniform1iARB = reinterpret_cast<PFNGLUNIFORM1IARBPROC>(
      wglGetProcAddress("glUniform1iARB"));
  glUniform1fARB = reinterpret_cast<PFNGLUNIFORM1FARBPROC>(
      wglGetProcAddress("glUniform1fARB"));
  glUniformMatrix3fvARB = reinterpret_cast<PFNGLUNIFORMMATRIX3FVARBPROC>(
      wglGetProcAddress("glUniformMatrix3fvARB"));
  glUniformMatrix4fvARB = reinterpret_cast<PFNGLUNIFORMMATRIX4FVARBPROC>(
      wglGetProcAddress("glUniformMatrix4fvARB"));
  glUniform4fvARB = reinterpret_cast<PFNGLUNIFORM4FVARBPROC>(
      wglGetProcAddress("glUniform4fvARB"));
  glUniform2fvARB = reinterpret_cast<PFNGLUNIFORM2FVARBPROC>(
      wglGetProcAddress("glUniform2fvARB"));
  glBindAttribLocationARB = reinterpret_cast<PFNGLBINDATTRIBLOCATIONARBPROC>(
      wglGetProcAddress("glBindAttribLocationARB"));
  glEnableVertexAttribArrayARB =
      reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYARBPROC>(
          wglGetProcAddress("glEnableVertexAttribArrayARB"));
  glVertexAttribPointerARB = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERARBPROC>(
      wglGetProcAddress("glVertexAttribPointerARB"));
  glDisableVertexAttribArrayARB =
      reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYARBPROC>(
          wglGetProcAddress("glDisableVertexAttribArrayARB"));
  glValidateProgramARB = reinterpret_cast<PFNGLVALIDATEPROGRAMARBPROC>(
      wglGetProcAddress("glValidateProgramARB"));
#endif

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &textureImageUnitCount_);

  shadersAvailable = true;
}
#endif

#if OO_USE_VBO
void oo::OpenGLExtensionManager::checkVBOSupported() {
  vboSupported_ = false;

  if (versionIsAtLeastMajor(1, 5) ||
      haveExtension("GL_ARB_vertex_buffer_object")) {
    vboSupported_ = true;
  }

#if OOLITE_WINDOWS
  if (vboSupported_) {
    glGenBuffersARB = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(
        wglGetProcAddress("glGenBuffersARB"));
    glDeleteBuffersARB = reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(
        wglGetProcAddress("glDeleteBuffersARB"));
    glBindBufferARB = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(
        wglGetProcAddress("glBindBufferARB"));
    glBufferDataARB = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(
        wglGetProcAddress("glBufferDataARB"));
  }
#endif
}
#endif

#if OO_USE_FBO
void oo::OpenGLExtensionManager::checkFBOSupported() {
  fboSupported_ = false;

  if (haveExtension("GL_EXT_framebuffer_object")) {
    fboSupported_ = true;
  }

#if OOLITE_WINDOWS
  if (fboSupported_) {
    glGenFramebuffersEXT = reinterpret_cast<PFNGLGENFRAMEBUFFERSEXTPROC>(
        wglGetProcAddress("glGenFramebuffersEXT"));
    glBindFramebufferEXT = reinterpret_cast<PFNGLBINDFRAMEBUFFEREXTPROC>(
        wglGetProcAddress("glBindFramebufferEXT"));
    glGenRenderbuffersEXT = reinterpret_cast<PFNGLGENRENDERBUFFERSEXTPROC>(
        wglGetProcAddress("glGenRenderbuffersEXT"));
    glBindRenderbufferEXT = reinterpret_cast<PFNGLBINDRENDERBUFFEREXTPROC>(
        wglGetProcAddress("glBindRenderbufferEXT"));
    glRenderbufferStorageEXT =
        reinterpret_cast<PFNGLRENDERBUFFERSTORAGEEXTPROC>(
            wglGetProcAddress("glRenderbufferStorageEXT"));
    glFramebufferRenderbufferEXT =
        reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC>(
            wglGetProcAddress("glFramebufferRenderbufferEXT"));
    glFramebufferTexture2DEXT =
        reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DEXTPROC>(
            wglGetProcAddress("glFramebufferTexture2DEXT"));
    glCheckFramebufferStatusEXT =
        reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC>(
            wglGetProcAddress("glCheckFramebufferStatusEXT"));
    glDeleteFramebuffersEXT = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSEXTPROC>(
        wglGetProcAddress("glDeleteFramebuffersEXT"));
    glDeleteRenderbuffersEXT =
        reinterpret_cast<PFNGLDELETERENDERBUFFERSEXTPROC>(
            wglGetProcAddress("glDeleteRenderbuffersEXT"));
    glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(
        wglGetProcAddress("glGenRenderbuffers"));
    glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(
        wglGetProcAddress("glBindRenderbuffer"));
    glRenderbufferStorage = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(
        wglGetProcAddress("glRenderbufferStorage"));
    glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(
        wglGetProcAddress("glGenFramebuffers"));
    glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(
        wglGetProcAddress("glBindFramebuffer"));
    glFramebufferRenderbuffer =
        reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(
            wglGetProcAddress("glFramebufferRenderbuffer"));
    glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(
        wglGetProcAddress("glFramebufferTexture2D"));
    glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(
        wglGetProcAddress("glGenVertexArrays"));
    glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(
        wglGetProcAddress("glGenBuffers"));
    glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(
        wglGetProcAddress("glBindVertexArray"));
    glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(
        wglGetProcAddress("glBindBuffer"));
    glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(
        wglGetProcAddress("glBufferData"));
    glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(
        wglGetProcAddress("glVertexAttribPointer"));
    glEnableVertexAttribArray =
        reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(
            wglGetProcAddress("glEnableVertexAttribArray"));
    glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(
        wglGetProcAddress("glUseProgram"));
    glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(
        wglGetProcAddress("glGetUniformLocation"));
    glUniform1i =
        reinterpret_cast<PFNGLUNIFORM1IPROC>(wglGetProcAddress("glUniform1i"));
    glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(
        wglGetProcAddress("glActiveTexture"));
    glBlendFuncSeparate = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(
        wglGetProcAddress("glBlendFuncSeparate"));
    glUniform1f =
        reinterpret_cast<PFNGLUNIFORM1FPROC>(wglGetProcAddress("glUniform1f"));
    glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(
        wglGetProcAddress("glUniform2fv"));
    glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(
        wglGetProcAddress("glDeleteRenderbuffer"));
    glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(
        wglGetProcAddress("glDeleteFramebuffers"));
    glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(
        wglGetProcAddress("glDeleteVertexArrays"));
    glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(
        wglGetProcAddress("glDeleteBuffers"));
    glDrawBuffers = reinterpret_cast<PFNGLDRAWBUFFERSPROC>(
        wglGetProcAddress("glDrawBuffers"));
    glCheckFramebufferStatus =
        reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(
            wglGetProcAddress("glCheckFramebufferStatus"));
    glTexImage2DMultisample = reinterpret_cast<PFNGLTEXIMAGE2DMULTISAMPLEPROC>(
        wglGetProcAddress("glTexImage2DMultisample"));
    glRenderbufferStorageMultisample =
        reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(
            wglGetProcAddress("glRenderbufferStorageMultisample"));
    glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(
        wglGetProcAddress("glBlitFramebuffer"));
    glClampColor = reinterpret_cast<PFNGLCLAMPCOLORPROC>(
        wglGetProcAddress("glClampColor"));
  }
#endif
}
#endif

#if OO_MULTITEXTURE
void oo::OpenGLExtensionManager::checkTextureCombinersSupported() {
  textureCombinersSupported_ = haveExtension("GL_ARB_texture_env_combine");

  if (textureCombinersSupported_) {
    OOGL(glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureUnitCount_));

#if OOLITE_WINDOWS
    // Duplicated in checkShadersSupported. but that's not really a problem.
    glActiveTextureARB = reinterpret_cast<PFNGLACTIVETEXTUREARBPROC>(
        wglGetProcAddress("glActiveTextureARB"));

    glClientActiveTextureARB =
        reinterpret_cast<PFNGLCLIENTACTIVETEXTUREARBPROC>(
            wglGetProcAddress("glClientActiveTextureARB"));
#endif
  } else {
    textureUnitCount_ = 1;
  }
}
#endif

namespace {

// The regexp test sent to a string that may be nil (a nil string matched
// nothing). matcher is the one pass's matcher, taken on first use and held by
// the caller, so one pass compiles one tester.
// A null matcher (the tester did not compile) answers NO and is asked again.
bool MatchesRegExp(std::shared_ptr<::oo::RegExpMatcher>& matcher,
                   const std::optional<std::string>& string,
                   const std::string& regexp) {
  if (!string.has_value()) return false;
  if (matcher == nullptr) matcher = ::oo::RegExpMatcher::regExpMatcher();
  if (matcher == nullptr) return false;
  return matcher->string(*string, regexp);
}

// regexps may be a single string or an array of strings (in which case results
// are ANDed).
bool CheckRegExps(std::shared_ptr<::oo::RegExpMatcher>& matcher,
                  const std::optional<std::string>& string,
                  const oo::PList& regexps) {
  if (regexps.isNull()) return true;  // No restriction == match.
  if (const std::string* regexp = regexps.getIf<std::string>()) {
    return MatchesRegExp(matcher, string, *regexp);
  }
  if (const oo::PList::Array* array = regexps.getIf<oo::PList::Array>()) {
    for (const oo::PList& element : *array) {
      const std::string* regexp = element.getIf<std::string>();
      if (regexp == nullptr) [[unlikely]] {
        // Invalid type -- match fails.
        return false;
      }

      if (!MatchesRegExp(matcher, string, *regexp)) return false;
    }
    return true;
  }

  // Invalid type -- match fails.
  return false;
}

// oo_stringForKey: on a dictionary that may be nil: a string, a number's string
// value, or nil.
oo::PList StringForKey(const oo::PList* dict, std::string_view key) {
  const oo::PList* value = (dict != nullptr) ? dict->find(key) : nullptr;
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return oo::PList();
  return oo::PList(dict->get<std::string>(key));
}

}  // namespace

oo::PList oo::OpenGLExtensionManager::lookUpPerGPUSettingsWithVersionString(
    const std::optional<std::string>& versionStr,
    const std::optional<std::string>& extensionsStr) {
  const oo::PList configurations = ResourceManager::dictionaryFromFilesNamed(
      "gpu-settings.plist", "Config", true);

  // Highest precedence first, then case-insensitive name order (keys were taken
  // in hash order before sorting; ties between names equal but for case now
  // keep byte order).
  std::vector<std::string> keys;
  if (const oo::PList::Dict* dict = configurations.getIf<oo::PList::Dict>()) {
    for (const auto& entry : *dict) keys.push_back(entry.first);
  }
  std::stable_sort(
      keys.begin(), keys.end(),
      [&configurations](const std::string& keyA, const std::string& keyB) {
        const oo::PList* dictA = configurations.get<oo::PList::Dict>(keyA);
        const oo::PList* dictB = configurations.get<oo::PList::Dict>(keyB);
        const double precedenceA =
            dictA != nullptr ? dictA->get<double>("precedence", 1) : 0;
        const double precedenceB =
            dictB != nullptr ? dictB->get<double>("precedence", 1) : 0;

        if (precedenceA != precedenceB) return precedenceA > precedenceB;
        return oo::str::caseInsensitiveCompare(keyA, keyB) < 0;
      });

  std::shared_ptr<::oo::RegExpMatcher>
      matcher;  // one for the pass (see MatchesRegExp)
  for (const std::string& key : keys) {
    const oo::PList* config = configurations.get<oo::PList::Dict>(key);
    if (config == nullptr) [[unlikely]]
      continue;

    const oo::PList* match = config->get<oo::PList::Dict>("match");
    const oo::PList* vendorExpr =
        (match != nullptr) ? match->find("vendor") : nullptr;

    if (!CheckRegExps(matcher, vendor,
                      vendorExpr != nullptr ? *vendorExpr : oo::PList()))
      continue;

    if (!CheckRegExps(matcher, renderer, StringForKey(match, "renderer")))
      continue;

    if (!CheckRegExps(matcher, versionStr, StringForKey(match, "version")))
      continue;

    if (!CheckRegExps(matcher, extensionsStr,
                      StringForKey(match, "extensions")))
      continue;

    OO_LOG("rendering.opengl.gpuSpecific", "Matched GPU configuration \"{}\".",
           key);
    return *config;
  }

  return oo::PList(oo::PList::Dict{});
}

// The singleton category (+allocWithZone:, and -retain/-release/-autorelease
// doing nothing) is not translated: the one instance is made only by
// sharedManager() and never released.

#if OOLITE_WINDOWS

namespace oo {

static void BadOpenGLExtensionUsed() {
  OO_LOG("rendering.opengl.badExtension", "{}",
         "***** An uninitialized OpenGL extension function has been called, "
         "terminating. This is a serious error, please report it. *****");
  exit(EXIT_FAILURE);
}

}  // namespace oo

#endif
