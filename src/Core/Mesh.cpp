/*

OOMesh.m

A note on memory management:
The dynamically-sized buffers used by OOMesh (_vertex etc) are the byte arrays
of refcounted OOMeshBuffers (one oo::Data each), which are tracked using the
_retainedObjects map. This simplifies the implementation of -dealloc, but more
importantly, it means bytes are refcounted and shared by a mesh and its mutable
copies. (Bytes read from the cache are copied into a buffer of their own.)

Since _retainedObjects is a map its members can be replaced,
potentially allowing mutable meshes, although we have no use for this at
present.

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

#include "Mesh.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "BasicMaterial.h"
#include "CacheManager.h"
#include "DebugFlags.h"
#include "DebugGLDrawing.h"
#include "DebugStandards.h"
#include "Entity.h"  // for NO_DRAW_DISTANCE_FACTOR.
#include "GraphicsResetManager.h"
#include "MacroOpenGL.h"
#include "MeshToOctreeConverter.h"
#include "OOJavaScriptEngine.h"
#include "Octree.h"
#include "OpenGLExtensionManager.h"
#include "ResourceManager.h"
#include "ShaderMaterial.h"
#include "Universe.h"
#include "oofnd/Log.hpp"
#include "oofnd/Scanner.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/Assert.h"
#include "oofnd/objc/Exception.h"

// If set, collision octree depth varies depending on the size of the mesh.
#define ADAPTIVE_OCTREE_DEPTH 1

// If set, cachable memory is scribbled with FEEDFACE to identify junk in cache.
#define SCRIBBLE 0

constexpr int kBaseOctreeDepth = 5;   // 32x32x32
                                      //	kMaxOctreeDepth declared in Octree.h.
constexpr int kSmallOctreeDepth = 4;  // 16x16x16
constexpr int kOctreeSizeThreshold =
    900;  // Size at which we start increasing octree depth
constexpr int kOctreeSmallSizeThreshold = 20;

namespace oo {

enum class MeshNormalMode : int { kPerFace, kSmooth, kExplicit };

constexpr std::string_view kLogMeshDataNotFound =
    "mesh.load.failed.fileNotFound";  // an OO_LOG message class
constexpr std::string_view kLogMeshTooManyMaterials =
    "mesh.load.failed.tooManyMaterials";

}  // namespace oo

#if OOMESH_PROFILE
#define PROFILE(tag)                                                         \
  do {                                                                       \
    _stopwatchLastTime = Profile(tag, _stopwatch.get(), _stopwatchLastTime); \
  } while (0)

static oo::TimeDelta Profile(std::string_view tag,
                             oo::ProfilingStopwatch* stopwatch,
                             oo::TimeDelta lastTime) {
  oo::TimeDelta now = stopwatch->currentTime();
  OO_LOG("mesh.profile", "Mesh profile: stage {}, {:g} seconds (delta {:g})",
         tag, now, now - lastTime);
  return now;
}
#else
#define PROFILE(tag) \
  do {               \
  } while (0)
#endif

/*	VertexFaceRef
        List of indices of faces used by a given vertex.
        Always access using the provided functions.

        The overflow list is a std::vector owned by the VertexFaceRef, which
   lives in a std::vector for the duration of -loadData:scaleFactor:.
*/

constexpr int kVertexFaceDefInternalCount = 11;  // sizeof (VertexFaceRef) = 32

struct VertexFaceRef {
  uint16_t internCount;
  uint16_t internFaces[kVertexFaceDefInternalCount];
  std::vector<std::size_t> extra;
};

static void VFRAddFace(VertexFaceRef* vfr, std::size_t index);
static std::size_t VFRGetCount(VertexFaceRef* vfr);
static std::size_t VFRGetFaceAtIndex(VertexFaceRef* vfr, std::size_t index);

// The OOCacheManager (OOMesh) category, as free functions next to the cache
// (defined below).
namespace {
oo::PList CacheManagerMeshDataForName(const std::string& inShipName);
void CacheManagerSetMeshData(const oo::PList& inData,
                             const std::string& inShipName);
}  // namespace

// One mesh buffer: the bytes _vertices & co. point into, shared (refcounted) by
// a mesh and its mutable copies as the retained data object was.
namespace oo {

class MeshBuffer : public oo::Object {
 public:
  explicit MeshBuffer(oo::Data bytes) : data_(std::move(bytes)) {}

  oo::Data& data() noexcept { return data_; }

 private:
  oo::Data data_;
};

}  // namespace oo

static bool IsLegacyNormalMode(oo::MeshNormalMode mode) {
  /*	True for modes that predate the "normal mode" concept, i.e. per-face
          and smooth. These modes require automatic winding correction.
  */
  switch (mode) {
    case oo::MeshNormalMode::kPerFace:
    case oo::MeshNormalMode::kSmooth:
      return true;

    case oo::MeshNormalMode::kExplicit:
      return false;
  }

#ifndef NDEBUG
  oo::RaiseException(oo::InvalidArgumentException,
                     "Unexpected normal mode in %s", __PRETTY_FUNCTION__);
#endif
  return false;
}

static bool IsPerVertexNormalMode(oo::MeshNormalMode mode) {
  /*	True for modes that have per-vertex normals, i.e. not per-face mode.
   */
  switch (mode) {
    case oo::MeshNormalMode::kPerFace:
      return false;

    case oo::MeshNormalMode::kSmooth:
    case oo::MeshNormalMode::kExplicit:
      return true;
  }

#ifndef NDEBUG
  oo::RaiseException(oo::InvalidArgumentException,
                     "Unexpected normal mode in %s", __PRETTY_FUNCTION__);
#endif
  return false;
}

std::shared_ptr<oo::Mesh> oo::Mesh::meshWithName(
    const std::string& name, const std::optional<std::string>& cacheKey,
    const oo::PList& materialDict, const oo::PList& shadersDict, bool smooth,
    const oo::PList& macros, Entity* object) {
  // [[self alloc] initWithName:...] autoreleased: a new mesh, loaded, or null
  // when it did not load.
  std::shared_ptr<oo::Mesh> mesh = oo::makeShared<oo::Mesh>();
  if (!mesh->initWithName(name, cacheKey, materialDict, shadersDict, smooth,
                          macros, object, 1.0f, true))
    return nullptr;
  return mesh;
}

std::shared_ptr<oo::Mesh> oo::Mesh::meshWithName(
    const std::string& name, const std::optional<std::string>& cacheKey,
    const oo::PList& materialDict, const oo::PList& shadersDict, bool smooth,
    const oo::PList& macros, Entity* object, float scale, bool cacheWriteable) {
  std::shared_ptr<oo::Mesh> mesh = oo::makeShared<oo::Mesh>();
  if (!mesh->initWithName(name, cacheKey, materialDict, shadersDict, smooth,
                          macros, object, scale, cacheWriteable))
    return nullptr;
  return mesh;
}

std::shared_ptr<oo::Material> oo::Mesh::placeholderMaterial() {
  static oo::BasicMaterial* placeholderMaterial =
      nullptr;  // never released, as before

  if (placeholderMaterial == nullptr) {
    // +materialDefaults answers a copy: keep it alive while noTextures
    // points into it.
    const oo::PList materialDefaults = ResourceManager::materialDefaults();
    const oo::PList* noTextures = materialDefaults.find("no-textures-material");
    // Never released: a heap std::shared_ptr that is never deleted.
    static std::shared_ptr<oo::BasicMaterial>* placeholderMaterialHolder =
        nullptr;  // never deleted
    placeholderMaterialHolder = new std::shared_ptr<oo::BasicMaterial>(
        oo::BasicMaterial::materialWithName(
            std::string("/placeholder/"),
            (noTextures != nullptr ? *noTextures : oo::PList())));
    placeholderMaterial = placeholderMaterialHolder->get();
  }

  return oo::share(placeholderMaterial);
}

oo::Mesh::Mesh() {
  baseFile = "No Model";
  baseFileOctreeCacheRef = "No Model-0.000";
  _cacheWriteable = true;
#if OO_MULTITEXTURE
  _textureUnitCount = oo::kNotFound;
#endif

  _lastPosition = kZeroVector;
  _lastRotMatrix = kZeroMatrix;  // not identity
  _lastBoundingBox = kZeroBoundingBox;
}

namespace oo {

Mesh::~Mesh() {
  unsigned i;

  deleteDisplayLists();

  for (i = 0; i != kMeshMaxMaterials; ++i) {
    // DESTROY(): cleared before the release
    const std::shared_ptr<oo::Material> material = std::move(materials[i]);
  }

  GraphicsResetManager::sharedManager()->unregisterCxxClient(this);

#if OOMESH_PROFILE
  _stopwatch = nullptr;
#endif
}

}  // namespace oo

namespace {

std::string_view NormalModeDescription(oo::MeshNormalMode mode) {
  switch (mode) {
    case oo::MeshNormalMode::kPerFace:
      return "per-face";
    case oo::MeshNormalMode::kSmooth:
      return "smooth";
    case oo::MeshNormalMode::kExplicit:
      return "explicit";
  }

  return "unknown";
}

}  // namespace

std::optional<std::string> oo::Mesh::descriptionComponents() const {
  oo::Mesh* mesh = const_cast<oo::Mesh*>(this);  // the getters are not const
  const std::optional<std::string> modelName = mesh->modelName();
  return std::format(
      "\"{}\", {} vertices, {} faces, radius: {:g} m normals: {}",
      modelName ? modelName->c_str() : "(null)", mesh->getVertexCount(),
      mesh->getFaceCount(), mesh->collisionRadius(),
      NormalModeDescription(static_cast<oo::MeshNormalMode>(_normalMode)));
}

std::shared_ptr<oo::Mesh> oo::Mesh::copyWithZone() {
  return oo::share(this);  // OK because we're immutable seen from the outside
}

std::optional<std::string> oo::Mesh::modelName() { return baseFile; }

size_t oo::Mesh::getVertexCount() { return vertexCount; }

size_t oo::Mesh::getFaceCount() { return faceCount; }

void oo::Mesh::renderOpaqueParts() {
  OO_ENTER_OPENGL();

  Entity* bindingEntity =
      _shaderBindingTarget.lock()
          .get();  // a message to a gone target's weak reference answered NO
  bool meshBelongsToVisualEffect =
      bindingEntity != nullptr && bindingEntity->getIsVisualEffect();

  OOSetOpenGLState(oo::OpenGLStateID::kOpaque);

  OOGL(glVertexPointer(3, GL_FLOAT, 0, _displayLists.vertexArray));
  OOGL(glNormalPointer(GL_FLOAT, 0, _displayLists.normalArray));

  // for visual effects enable blending. This will allow use of alpha
  // channel in shaders - note, this is a bit of cheating the system,
  // which expects blending to be disabled at this point
  if (meshBelongsToVisualEffect) {
    OOGL(glEnable(GL_BLEND));
    OOGL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  }

#if OO_SHADERS
  if (oo::OpenGLExtensionManager::sharedManager()->shadersSupported()) {
    OOGL(glEnableVertexAttribArrayARB(kTangentAttributeIndex));
    OOGL(glVertexAttribPointerARB(kTangentAttributeIndex, 3, GL_FLOAT, GL_FALSE,
                                  0, _displayLists.tangentArray));
  }
#endif

  bool usingNormalsAsTexCoords = false;
  oo::MeshMaterialIndex ti;

  /*	FIXME: really, really horrible hack to set up texture coordinates for
          each texture unit. Very messy and still fails to handle some possibly-
          basic stuff, like switching usingNormalsAsTexCoords per texture unit.
          The right way to do this is probably to move attribute setup into the
          material model.
          -- Ahruman 2010-04-12
  */
#if OO_MULTITEXTURE
  if (_textureUnitCount == oo::kNotFound) {
    _textureUnitCount = 0;
    for (ti = 0; ti < materialCount; ti++) {
      std::size_t count =
          materials[ti] != nullptr
              ? materials[ti]->countOfTextureUnitsWithBaseCoordinates()
              : 0;
      if (_textureUnitCount < count) _textureUnitCount = count;
    }
  }

  std::size_t unit;
  if (_textureUnitCount <= 1) {
    OOGL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
  } else {
    /*	It should not be possible to have multiple texture units if
            texture combiners are not available.
    */
    OOCAssert(
        oo::OpenGLExtensionManager::sharedManager()
            ->textureCombinersSupported(),
        "Mesh %s uses %zu texture units, but multitexturing is not available.",
        std::format("<OOMesh {}>", oo::str::pointerDescription(this)).c_str(),
        _textureUnitCount);

    for (unit = 0; unit < _textureUnitCount; unit++) {
      OOGL(glClientActiveTextureARB(
          static_cast<GLenum>(GL_TEXTURE0_ARB + unit)));
      OOGL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    }
  }
#else
  OOGL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
#endif

  try {
    if (!listsReady) {
      OOGL(displayList0 = glGenLists(materialCount));

      // Ensure all textures are loaded
      for (ti = 0; ti < materialCount; ti++) {
        if (materials[ti] != nullptr) materials[ti]->ensureFinishedLoading();
      }
    }

    for (ti = 0; ti < materialCount; ti++) {
      bool wantsNormalsAsTextureCoordinates =
          materials[ti] != nullptr &&
          materials[ti]->wantsNormalsAsTextureCoordinates();
      if (ti == 0 ||
          wantsNormalsAsTextureCoordinates != usingNormalsAsTexCoords) {
        // FIXME: enabling/disabling texturing should be handled by the
        // material.
#if OO_MULTITEXTURE
        for (unit = 0; unit < _textureUnitCount; unit++) {
          if (_textureUnitCount > 1) {
            OOGL(glClientActiveTextureARB(
                static_cast<GLenum>(GL_TEXTURE0_ARB + unit)));
            OOGL(glActiveTextureARB(
                static_cast<GLenum>(GL_TEXTURE0_ARB + unit)));
          }
#endif
          if (!wantsNormalsAsTextureCoordinates) {
            OOGL(glDisable(GL_TEXTURE_CUBE_MAP));
            OOGL(glTexCoordPointer(2, GL_FLOAT, 0,
                                   _displayLists.textureUVArray));
            /*	FIXME: Not including the line below breaks multitexturing in
               no-shaders mode. However, the OpenGL state manager should
               probably be handling this; TEXTURE_2D is part of
               OPENGL_STATE_OPAQUE, which has already been set.
                    - Nikos 20130103
            */
            OOGL(glEnable(GL_TEXTURE_2D));
          } else {
            OOGL(glDisable(GL_TEXTURE_2D));
            OOGL(glTexCoordPointer(3, GL_FLOAT, 0, _displayLists.vertexArray));
            OOGL(glEnable(GL_TEXTURE_CUBE_MAP));
          }
#if OO_MULTITEXTURE
        }
#endif
        usingNormalsAsTexCoords = wantsNormalsAsTextureCoordinates;
      }

      if (materials[ti] != nullptr) materials[ti]->apply();
      OOGL(glDrawArrays(GL_TRIANGLES,
                        static_cast<GLint>(triangle_range[ti].location),
                        static_cast<GLsizei>(triangle_range[ti].length)));
    }

    listsReady = true;
    brokenInRender = false;
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    if (!brokenInRender) {
      OO_LOG(oo::kLogException,
             "***** {} for {} encountered exception: {} : {} *****",
             __PRETTY_FUNCTION__, description(), exception->name(),
             exception->reason());
      brokenInRender = true;
    }
    if (std::string_view(exception->name()).starts_with("Oolite")) {
      if (UNIVERSE != nullptr)
        UNIVERSE->handleOoliteException(exception->object());
    }  // handle these ourself
    else
      throw;  // pass these on
  }

#if OO_SHADERS
  if (oo::OpenGLExtensionManager::sharedManager()->shadersSupported()) {
    OOGL(glDisableVertexAttribArrayARB(kTangentAttributeIndex));
  }
#endif

  oo::Material::applyNone();
  oo::CheckOpenGLErrors(
      [&]() -> std::string { return "OOMesh after drawing " + description(); });

#if OO_MULTITEXTURE
  if (_textureUnitCount <= 1) {
    OOGL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  } else {
    for (unit = 0; unit < _textureUnitCount; unit++) {
      OOGL(glClientActiveTextureARB(
          static_cast<GLenum>(GL_TEXTURE0_ARB + unit)));
      OOGL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
    }

    OOGL(glClientActiveTextureARB(GL_TEXTURE0_ARB));
    OOGL(glActiveTextureARB(GL_TEXTURE0_ARB));
  }
#else
  OOGL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
  if (oo::DebugFlagIsOn(oo::DebugFlags::kDrawNormals)) debugDrawNormals();
  if (oo::DebugFlagIsOn(oo::DebugFlags::kOctreeDraw)) {
    if (Octree* cxxOctree = getOctree().get()) cxxOctree->drawOctree();
  }  // a nil octree did nothing
#endif

  // visual effect - disable previously enabled blending
  if (meshBelongsToVisualEffect) OOGL(glDisable(GL_BLEND));

  OOVerifyOpenGLState();
}

void oo::Mesh::rebindMaterials() {
  oo::MeshMaterialCount i;
  std::shared_ptr<oo::Material> material;

  if (materialCount != 0) {
    for (i = 0; i != materialCount; ++i) {
      std::shared_ptr<oo::Material> oldMaterial = materials[i];

      if (materialKeys[i] != "_oo_placeholder_material") {
        material = oo::Material::materialWithName(
            materialKeys[i], _cacheKey, _materialDict, _shadersDict,
            _shaderMacros,
            _shaderBindingTarget.lock().get(),  // Windows DEP fix.
            IsPerVertexNormalMode(
                static_cast<oo::MeshNormalMode>(_normalMode)));
      } else {
        material = nullptr;
      }

      // The member holds the material, retained, as before.
      if (material != nullptr) {
        materials[i] = material;
      } else {
        materials[i] = placeholderMaterial();
      }

      /*	Release is deferred to here to ensure we don't end up releasing
              a texture that's not in the recent-cache and then reloading it.
      */
      oldMaterial.reset();
    }
  }
}

oo::PList oo::Mesh::getMaterials() { return _materialDict; }

oo::PList oo::Mesh::shaders() { return _shadersDict; }

bool oo::Mesh::hasOpaqueParts() { return true; }

GLfloat oo::Mesh::collisionRadius() { return _collisionRadius; }

GLfloat oo::Mesh::maxDrawDistance() { return _maxDrawDistance; }

#if ADAPTIVE_OCTREE_DEPTH
unsigned oo::Mesh::octreeDepth() {
  float threshold = kOctreeSizeThreshold;
  unsigned result = kBaseOctreeDepth;
  GLfloat xs, ys, zs, t, size;

  bounding_box_get_dimensions(_boundingBox, &xs, &ys, &zs);
  // Shuffle dimensions around so zs is smallest
  if (xs < zs) {
    t = zs;
    zs = xs;
    xs = t;
  }
  if (ys < zs) {
    t = zs;
    zs = ys;
    ys = t;
  }
  size = (xs + ys) / 2.0f;  // Use average of two largest

  if (size < static_cast<GLfloat>(kOctreeSmallSizeThreshold)) {
    result = kSmallOctreeDepth;
  } else {
    while (result < kMaxOctreeDepth) {
      if (size < threshold) break;
      threshold *= 2.0f;
      result++;
    }
  }

  OO_LOG("mesh.load.octree.size",
         "Selected octree depth {} for size {:g} for {}", result, size,
         baseFile.value_or("(null)"));
  return result;
}
#else
unsigned oo::Mesh::octreeDepth() { return kBaseOctreeDepth; }
#endif

std::shared_ptr<Octree> oo::Mesh::getOctree() {
  if (octree == nullptr) {
    octree = baseFileOctreeCacheRef
                 ? oo::CacheManagerOctreeForModel(*baseFileOctreeCacheRef)
                 : std::shared_ptr<Octree>();
    if (octree == nullptr) {
      {
        std::shared_ptr<oo::MeshToOctreeConverter> converter =
            oo::MeshToOctreeConverter::converterWithCapacity(faceCount);
        oo::MeshFaceCount i;
        for (i = 0; i < faceCount; i++) {
          // Somewhat surprisingly, this method doesn't even show up in
          // profiles. -- Ahruman 2012-09-22
          Triangle tri;
          tri.v[0] = _vertices[_faces[i].vertex[0]];
          tri.v[1] = _vertices[_faces[i].vertex[1]];
          tri.v[2] = _vertices[_faces[i].vertex[2]];
          converter->addTriangle(tri);
        }

        octree = converter->findOctreeToDepth(octreeDepth());
        if ((_cacheWriteable) && baseFileOctreeCacheRef) {
          oo::CacheManagerSetOctree(octree.get(), *baseFileOctreeCacheRef);
        }
      }
    } else {
      OO_LOG("mesh.load.octreeCached", "Retrieved octree \"{}\" from cache.",
             baseFileOctreeCacheRef.value_or("(null)"));
    }
  }

  return octree;
}

BoundingBox oo::Mesh::findBoundingBoxRelativeToPosition(Vector opv, Vector ri,
                                                        Vector rj, Vector rk,
                                                        Vector position,
                                                        Vector si, Vector sj,
                                                        Vector sk) {
  BoundingBox result;
  Vector pv, rv;

  // FIXME: rewrite with matrices
  Vector rpos = vector_subtract(position, opv);  // model origin relative to opv

  rv.x = dot_product(ri, rpos);
  rv.y = dot_product(rj, rpos);
  rv.z = dot_product(rk, rpos);  // model origin rel to opv in ijk

  if (vertexCount < 1) [[unlikely]] {
    bounding_box_reset_to_vector(&result, rv);
  } else {
    pv.x = rpos.x + si.x * _vertices[0].x + sj.x * _vertices[0].y +
           sk.x * _vertices[0].z;
    pv.y = rpos.y + si.y * _vertices[0].x + sj.y * _vertices[0].y +
           sk.y * _vertices[0].z;
    pv.z = rpos.z + si.z * _vertices[0].x + sj.z * _vertices[0].y +
           sk.z * _vertices[0].z;  // _vertices[0] position rel to opv
    rv.x = dot_product(ri, pv);
    rv.y = dot_product(rj, pv);
    rv.z = dot_product(rk, pv);  // _vertices[0] position rel to opv in ijk
    bounding_box_reset_to_vector(&result, rv);
  }

  oo::MeshVertexCount i;
  for (i = 1; i < vertexCount; i++) {
    pv.x = rpos.x + si.x * _vertices[i].x + sj.x * _vertices[i].y +
           sk.x * _vertices[i].z;
    pv.y = rpos.y + si.y * _vertices[i].x + sj.y * _vertices[i].y +
           sk.y * _vertices[i].z;
    pv.z = rpos.z + si.z * _vertices[i].x + sj.z * _vertices[i].y +
           sk.z * _vertices[i].z;
    rv.x = dot_product(ri, pv);
    rv.y = dot_product(rj, pv);
    rv.z = dot_product(rk, pv);
    bounding_box_add_vector(&result, rv);
  }

  return result;
}

BoundingBox oo::Mesh::findSubentityBoundingBoxWithPosition(
    Vector position, oo::Matrix rotMatrix) {
  // HACK! Should work out what the various bounding box things do and make it
  // neat and consistent.
  // FIXME: this is a bottleneck.
  // Try to fix bottleneck by caching for common case where subentity
  // pos+rot is constant from frame to frame. - CIM

  if (vector_equal(position, _lastPosition) &&
      oo::MatrixEqual(rotMatrix, _lastRotMatrix)) {
    return _lastBoundingBox;
  }

  BoundingBox result;
  Vector v;

  v = vector_add(position, oo::VectorMultiplyMatrix(_vertices[0], rotMatrix));
  bounding_box_reset_to_vector(&result, v);

  oo::MeshVertexCount i;
  for (i = 1; i < vertexCount; i++) {
    v = vector_add(position, oo::VectorMultiplyMatrix(_vertices[i], rotMatrix));
    bounding_box_add_vector(&result, v);
  }

  _lastBoundingBox = result;
  _lastPosition = position;
  _lastRotMatrix = rotMatrix;

  return result;
}

std::shared_ptr<oo::Mesh> oo::Mesh::meshRescaledBy(GLfloat scaleFactor) {
  std::shared_ptr<oo::Mesh> result = mutableCopyWithZone();
  result->rescaleByFactor(scaleFactor);
  return result;
}

void oo::Mesh::setBindingTarget(Entity* target) {
  unsigned i;

  for (i = 0; i != oo::kMeshMaxMaterials; ++i) {
    // A nil material did nothing.
    if (oo::Material* material = materials[i].get())
      material->setBindingTarget(target);
  }
}

#ifdef OO_DEBUG_MONITOR_ENABLED
void oo::Mesh::dumpSelfState() {
  oo::Drawable::dumpSelfState();

  if (baseFile) OO_LOG("dumpState.mesh", "Model file: {}", *baseFile);
  OO_LOG("dumpState.mesh", "Vertex count: {}, face count: {}",
         static_cast<unsigned>(vertexCount), static_cast<unsigned>(faceCount));
  OO_LOG("dumpState.mesh", "Normals: {}",
         NormalModeDescription(static_cast<oo::MeshNormalMode>(_normalMode)));
}
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::Mesh::allTextures() {
  // Every material's textures; duplicates are dropped.
  std::vector<std::shared_ptr<oo::Texture>> result;
  oo::MeshMaterialCount i;
  for (i = 0; i != materialCount; i++) {
    // A nil material answered no textures.
    oo::Material* material = materials[i].get();
    if (material == nullptr) continue;
    for (const std::shared_ptr<oo::Texture>& texture : material->allTextures())
      result.push_back(texture);
  }

  return result;
}

size_t oo::Mesh::totalSize() {
  size_t result = oo::Drawable::totalSize();
  if (_vertices != nullptr) result += sizeof *_vertices * vertexCount;
  if (_normals != nullptr) result += sizeof *_normals * vertexCount;
  if (_tangents != nullptr) result += sizeof *_tangents * vertexCount;
  if (_faces != nullptr) result += sizeof *_faces * faceCount;

  result += _displayLists.count *
            (sizeof(GLint) + sizeof(GLfloat) + sizeof(Vector) * 3);

  oo::MeshMaterialCount i;
  for (i = 0; i != materialCount; i++) {
    // The object's own size, 0 for nil.
    if (materials[i] != nullptr) result += materials[i]->objectSize();
  }

  // A nil octree answered 0.
  if (octree != nullptr) result += octree->totalSize();
  return result;
}
#endif

/*	This method exists purely to suppress Clang static analyzer warnings
   that these ivars are unused (but may be used by categories, which they are).
        FIXME: there must be a feature macro we can use to avoid actually
   building this into the app, but I can't find it in docs.
*/
bool oo::Mesh::suppressClangStuff() {
  return _normals && _tangents && _faces &&
         static_cast<bool>(_boundingBox.min.x);
}

bool oo::Mesh::initWithName(const std::string& name,
                            const std::optional<std::string>& cacheKey,
                            const oo::PList& materialDict,
                            const oo::PList& shadersDict, bool smooth,
                            const oo::PList& macros, Entity* target,
                            float scale, bool cacheWriteable) {
  OOJS_PROFILE_SCOPE();

  // The mesh is new (the factory made it; -init's defaults, which loading
  // overwrites). It answers whether it loaded: the factory drops it when not,
  // as [self release] did.
  bool loaded = false;
  {
    _normalMode = static_cast<uint8_t>(oo::ToUnderlying(
        smooth ? oo::MeshNormalMode::kSmooth : oo::MeshNormalMode::kPerFace));
    _cacheWriteable = cacheWriteable;

#if OOMESH_PROFILE
    _stopwatch = oo::makeShared<oo::ProfilingStopwatch>();
#endif

    if (loadData(name, scale)) {
      calculateBoundingVolumes();
      PROFILE("finished calculateBoundingVolumes (again\?\?)");

      baseFile = name;
      baseFileOctreeCacheRef = std::format("{}-{:.3f}", name, scale);

      /*	New in r3033: save the material-defining parameters here so we
              can rebind the materials at any time.
              -- Ahruman 2010-02-17
      */
      _materialDict = materialDict;
      _shadersDict = shadersDict;
      _cacheKey = cacheKey;
      _shaderMacros = macros;
      _shaderBindingTarget = oo::share(target);

      rebindMaterials();
      PROFILE("finished material setup");

      oo::GraphicsResetManager::sharedManager()->registerCxxClient(this);
      loaded = true;
    }
#if OOMESH_PROFILE
    _stopwatch = nullptr;
#endif
#if OO_MULTITEXTURE
    if (loaded) [[likely]] {
      _textureUnitCount = oo::kNotFound;
    }
#endif
  }
  return loaded;
}

std::shared_ptr<oo::Mesh> oo::Mesh::mutableCopyWithZone() {
  // A new mesh whose members are copied one by one (the copy constructor),
  // so the buffers are shared.
  std::shared_ptr<oo::Mesh> result = oo::makeShared<oo::Mesh>(*this);

  // (The binding target is a weak reference, copied; the octree is a C++
  // reference, copied; the materials are shared, copied.)

  // Reset unsharable GL state
  result->listsReady = false;

  oo::GraphicsResetManager::sharedManager()->registerCxxClient(result.get());

  return result;
}

oo::PList oo::Mesh::modelData() {
  OOJS_PROFILE_SCOPE();

  bool includeNormals =
      IsPerVertexNormalMode(static_cast<oo::MeshNormalMode>(_normalMode));

  // Prepare cache data elements.
  const auto vertData = _retainedObjects.find("vertices");
  const auto faceData = _retainedObjects.find("faces");
  const auto normData = _retainedObjects.find("normals");
  const auto tanData = _retainedObjects.find("tangents");

  // Ensure we have all the required data elements.
  if (vertData == _retainedObjects.end() ||
      faceData == _retainedObjects.end()) {
    return oo::PList();
  }

  if (includeNormals) {
    if (normData == _retainedObjects.end() || tanData == _retainedObjects.end())
      return oo::PList();
  }

  // All OK; stick 'em in a dictionary. The counts are unsigned
  // (+numberWithUnsignedInt:, and +numberWithUnsignedChar: for the normal
  // mode); the normals are only included when used.
  oo::PList::Array mtlKeys;
  for (oo::MeshMaterialCount i = 0; i != materialCount; ++i)
    mtlKeys.emplace_back(materialKeys[i]);

  oo::PList::Dict result;
  result["vertex count"] = oo::PList(vertexCount);
  result["vertex data"] = oo::PList(vertData->second->data());
  result["face count"] = oo::PList(faceCount);
  result["face data"] = oo::PList(faceData->second->data());
  result["material keys"] = oo::PList(std::move(mtlKeys));
  result["normal mode"] = oo::PList::unsignedInteger(_normalMode);
  if (includeNormals) {
    result["normal data"] = oo::PList(normData->second->data());
    result["tangent data"] = oo::PList(tanData->second->data());
  }
  return oo::PList(std::move(result));
}

bool oo::Mesh::setModelFromModelData(const oo::PList& dict,
                                     const std::string& fileName) {
  OOJS_PROFILE_SCOPE();

  unsigned i;

  if (!dict.isDict()) return false;

  vertexCount = dict.get<unsigned int>("vertex count");
  faceCount = dict.get<unsigned int>("face count");

  if (vertexCount == 0 || faceCount == 0) return false;

  // Read data elements from dictionary.
  const oo::PList* vertData = dict.get<oo::PList::Data>("vertex data");
  const oo::PList* faceData = dict.get<oo::PList::Data>("face data");
  const oo::PList* normData = nullptr;
  const oo::PList* tanData = nullptr;

  const oo::PList* mtlKeys = dict.get<oo::PList::Array>("material keys");
  _normalMode = dict.get<unsigned char>("normal mode");
  bool includeNormals =
      IsPerVertexNormalMode(static_cast<oo::MeshNormalMode>(_normalMode));

  // Ensure we have all the required data elements.
  if (vertData == nullptr || faceData == nullptr || mtlKeys == nullptr) {
    OO_LOG("mesh.load.error.badCacheData",
           "Ignoring bad cache data for mesh \"{}\".", fileName);
    return false;
  }

  if (includeNormals) {
    normData = dict.get<oo::PList::Data>("normal data");
    tanData = dict.get<oo::PList::Data>("tangent data");
    if (normData == nullptr || tanData == nullptr) {
      OO_LOG("mesh.load.error.badCacheData",
             "Ignoring bad normal/tangent cache data for mesh \"{}\".",
             fileName);
      return false;
    }
  }

  // Ensure data objects are of correct size.
  if (vertData->getIf<oo::PList::Data>()->length() !=
      sizeof *_vertices * vertexCount)
    return false;
  if (faceData->getIf<oo::PList::Data>()->length() !=
      sizeof *_faces * faceCount)
    return false;
  if (includeNormals) {
    if (normData->getIf<oo::PList::Data>()->length() !=
        sizeof *_normals * vertexCount)
      return false;
    if (tanData->getIf<oo::PList::Data>()->length() !=
        sizeof *_tangents * vertexCount)
      return false;
  }

  // Retain data: each is copied into a buffer of this mesh's, and the pointers
  // taken from it.
  setRetainedObject(*vertData->getIf<oo::PList::Data>(), "vertices");
  _vertices = reinterpret_cast<Vector*>(
      _retainedObjects.find("vertices")->second->data().mutableBytes());
  setRetainedObject(*faceData->getIf<oo::PList::Data>(), "faces");
  _faces = reinterpret_cast<oo::MeshFace*>(
      _retainedObjects.find("faces")->second->data().mutableBytes());
  if (includeNormals) {
    setRetainedObject(*normData->getIf<oo::PList::Data>(), "normals");
    _normals = reinterpret_cast<Vector*>(
        _retainedObjects.find("normals")->second->data().mutableBytes());
    setRetainedObject(*tanData->getIf<oo::PList::Data>(), "tangents");
    _tangents = reinterpret_cast<Vector*>(
        _retainedObjects.find("tangents")->second->data().mutableBytes());
  } else {
    _normals = nullptr;
    _tangents = nullptr;
  }

  // Copy material keys (oo_stringAtIndex: a string, or a number's
  // -stringValue).
  const oo::PList::Array& keys = *mtlKeys->getIf<oo::PList::Array>();
  materialCount = static_cast<oo::MeshMaterialCount>(keys.size());
  for (i = 0; i != materialCount; ++i) {
    const oo::PList& key = keys[i];
    if (key.isString() || key.isNumber()) {
      materialKeys[i] = oo::PListGet<std::string>::from(&key, std::string());
    } else {
      OO_LOG("mesh.load.error.badCacheData",
             "Ignoring bad cache data for mesh \"{}\".", fileName);
      return false;
    }
  }

  return true;
}

bool oo::Mesh::loadData(const std::string& filename, float scale) {
  OOJS_PROFILE_SCOPE();

  std::optional<oo::str::Scanner>
      scanner;  // made from the preprocessed text below
  bool failFlag = false;
  std::string failString = "***** ";
  unsigned i, j;
  std::map<std::string, unsigned, std::less<>> texFileName2Idx;
  bool using_preloaded = false;

  const std::string cacheKey = std::format(
      "{}:{}:{:.3f}", filename, static_cast<unsigned>(_normalMode), scale);
  const oo::PList cacheData = CacheManagerMeshDataForName(cacheKey);
  if (cacheData) {
    if (setModelFromModelData(cacheData, filename)) {
      using_preloaded = true;
      PROFILE("loaded from cache");
      OO_LOG("mesh.load.cached", "Retrieved mesh \"{}\" from cache.", filename);
    }
  }

  if (!using_preloaded) {
    OO_LOG("mesh.load.uncached", "Mesh \"{}\" is not in cache, loading.",
           cacheKey);

    const oo::str::CharacterSet whitespaceCharSet =
        oo::str::CharacterSet::whitespace();
    const oo::str::CharacterSet whitespaceAndNewlineCharSet =
        oo::str::CharacterSet::whitespaceAndNewline();
    // The newline set. The non-Mac build made it as whitespace-and-newline
    // minus whitespace, which is the same set (U+000A-U+000D and U+0085:
    // Scanner.hpp's tables).
    const oo::str::CharacterSet newlineCharSet =
        oo::str::CharacterSet::newline();

    {
      const std::optional<std::string> dataOpt =
          ResourceManager::stringFromFilesNamed(filename, "Models", false);
      if (!dataOpt) {
        // Model not found
        OO_LOG(oo::kLogMeshDataNotFound, "***** ERROR: could not find {}",
               filename);
        oo::StandardsError("Model file not found");
        return false;
      }

      // strip out comments and commas between values
      std::vector<std::string> lines = oo::str::split(*dataOpt, "\n");
      for (i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        std::vector<std::string> parts;
        //
        // comments
        //
        parts = oo::str::split(line, "#");
        line = parts.empty() ? std::string() : parts[0];
        parts = oo::str::split(line, "//");
        line = parts.empty() ? std::string() : parts[0];
        //
        // commas
        //
        parts = oo::str::split(line, ",");
        line.clear();
        for (std::size_t p = 0; p < parts.size(); ++p) {
          if (p) line += ' ';
          line += parts[p];
        }
        //
        lines[i] = std::move(line);
      }

      std::string data;
      for (std::size_t li = 0; li < lines.size(); ++li) {
        if (li) data += '\n';
        data += lines[li];
      }
      scanner.emplace(data);
    }

    PROFILE("finished preprocessing");

    // get number of vertices
    //
    scanner->setScanLocation(0);  // reset
    if (scanner->scanString("NVERTS")) {
      int n_v;
      if (scanner->scanInt(&n_v)) {
        vertexCount = static_cast<oo::MeshVertexCount>(n_v);
      } else {
        failFlag = true;
        failString += "Failed to read value of NVERTS\n";
      }
    } else {
      failFlag = true;
      failString += "Failed to read NVERTS\n";
    }

    if (!allocateVertexBuffersWithCount(vertexCount)) {
      OO_LOG(
          oo::kLogAllocationFailure,
          "***** ERROR: failed to allocate memory for model {} ({} vertices).",
          filename, static_cast<unsigned>(vertexCount));
      return false;
    }

    // get number of faces
    if (scanner->scanString("NFACES")) {
      int n_f;
      if (scanner->scanInt(&n_f)) {
        faceCount = static_cast<oo::MeshFaceCount>(n_f);
      } else {
        failFlag = true;
        failString += "Failed to read value of NFACES\n";
      }
    } else {
      failFlag = true;
      failString += "Failed to read NFACES\n";
    }

    // Allocate face->vertex table.
    std::vector<VertexFaceRef> faceRefTable(
        vertexCount);  // zeroed, freed when loading ends
    VertexFaceRef* faceRefs = faceRefTable.data();

    if (!allocateFaceBuffersWithCount(faceCount)) {
      OO_LOG(oo::kLogAllocationFailure,
             "***** ERROR: failed to allocate memory for model {} ({} "
             "vertices, {} faces).",
             filename, static_cast<unsigned>(vertexCount),
             static_cast<unsigned>(faceCount));
      return false;
    }

    // get vertex data
    if (scanner->scanString("VERTEX")) {
      for (j = 0; j < vertexCount; j++) {
        float x = 0, y = 0, z = 0;  // read only after every scan succeeded (GCC
                                    // -Wmaybe-uninitialized)
        if (!failFlag) {
          if (!scanner->scanFloat(&x)) failFlag = true;
          if (!scanner->scanFloat(&y)) failFlag = true;
          if (!scanner->scanFloat(&z)) failFlag = true;
          if (!failFlag) {
            _vertices[j] = make_vector(x * scale, y * scale, z * scale);
          } else {
            failString +=
                std::format("Failed to read a value for vertex[{}] in {}\n",
                            static_cast<int>(j), "VERTEX");
          }
        }
      }
    } else {
      failFlag = true;
      failString += "Failed to find VERTEX data\n";
    }

    // get face data
    if (scanner->scanString("FACES")) {
      for (j = 0; j < faceCount; j++) {
        int r, g, b;
        float nx = 0, ny = 0,
              nz = 0;  // read only after every scan succeeded (GCC
                       // -Wmaybe-uninitialized)
        int n_v;
        if (!failFlag) {
          // colors
          if (!scanner->scanInt(&r)) failFlag = true;
          if (!scanner->scanInt(&g)) failFlag = true;
          if (!scanner->scanInt(&b)) failFlag = true;
          if (!failFlag) {
            _faces[j].smoothGroup = static_cast<oo::MeshSmoothGroup>(r);
          } else {
            failString +=
                std::format("Failed to read a color for face[{}] in FACES\n",
                            static_cast<int>(j));
          }

          // normal
          if (!scanner->scanFloat(&nx)) failFlag = true;
          if (!scanner->scanFloat(&ny)) failFlag = true;
          if (!scanner->scanFloat(&nz)) failFlag = true;
          if (!failFlag) {
            _faces[j].normal = vector_normal(make_vector(nx, ny, nz));
          } else {
            failString +=
                std::format("Failed to read a normal for face[{}] in FACES\n",
                            static_cast<int>(j));
          }

          // vertices
          if (scanner->scanInt(&n_v)) {
            if (n_v < 3) {
              failFlag = true;
              failString +=
                  std::format("Face[{}] has fewer than three vertices.\n", j);
            } else if (n_v > 3) {
              OO_LOG_WARN("mesh.load.warning.nonTriangular",
                          "Face[{}] of {} has {} vertices specified. Only the "
                          "first three will be used.",
                          static_cast<unsigned>(j), baseFile.value_or("(null)"),
                          static_cast<unsigned>(n_v));
              n_v = 3;
            }
          } else {
            failFlag = true;
            failString += std::format(
                "Failed to read number of vertices for face[{}] in FACES\n",
                static_cast<int>(j));
          }

          if (!failFlag) {
            int vi;
            for (i = 0; static_cast<int>(i) < n_v; i++) {
              if (scanner->scanInt(&vi)) {
                _faces[j].vertex[i] = static_cast<GLuint>(vi);
                if (faceRefs != nullptr) VFRAddFace(&faceRefs[vi], j);
              } else {
                failFlag = true;
                failString += std::format(
                    "Failed to read vertex[{}] for face[{}] in FACES\n",
                    static_cast<int>(i), static_cast<int>(j));
              }
            }
          }
        }
      }
    } else {
      failFlag = true;
      failString += "Failed to find FACES data\n";
    }

    // Get textures data.
    if (scanner->scanString("TEXTURES")) {
      for (j = 0; j < faceCount; j++) {
        std::string materialKey;
        float max_x, max_y;
        float s, t;
        if (!failFlag) {
          // materialKey
          //
          scanner->scanCharactersFromSet(whitespaceAndNewlineCharSet);
          std::string scannedKey;
          if (!scanner->scanUpToCharactersFromSet(whitespaceCharSet,
                                                  &scannedKey)) {
            failFlag = true;
            failString += std::format(
                "Failed to read texture filename for face[{}] in TEXTURES\n",
                static_cast<int>(j));
          } else {
            materialKey = std::move(scannedKey);
            const auto indexIt = texFileName2Idx.find(materialKey);
            if (indexIt != texFileName2Idx.end()) {
              _faces[j].materialIndex =
                  static_cast<oo::MeshMaterialIndex>(indexIt->second);
            } else {
              if (materialCount == oo::kMeshMaxMaterials) {
                OO_LOG(oo::kLogMeshTooManyMaterials,
                       "***** ERROR: model {} has too many materials (maximum "
                       "is {})",
                       filename, static_cast<int>(oo::kMeshMaxMaterials));
                return false;
              }
              _faces[j].materialIndex = materialCount;
              materialKeys[materialCount] = materialKey;
              texFileName2Idx.emplace(materialKey, materialCount);
              ++materialCount;
            }
          }

          // texture size
          //
          if (!failFlag) {
            if (!scanner->scanFloat(&max_x)) failFlag = true;
            if (!scanner->scanFloat(&max_y)) failFlag = true;
            if (failFlag)
              failString += std::format(
                  "Failed to read texture size for max_x and max_y in face[{}] "
                  "in TEXTURES\n",
                  static_cast<int>(j));
          }

          // vertices
          //
          if (!failFlag) {
            for (i = 0; i < 3; i++) {
              if (!scanner->scanFloat(&s)) failFlag = true;
              if (!scanner->scanFloat(&t)) failFlag = true;
              if (!failFlag) {
                _faces[j].s[i] = s / max_x;
                _faces[j].t[i] = t / max_y;
              } else {
                failString += std::format(
                    "Failed to read s t coordinates for vertex[{}] in face[{}] "
                    "in TEXTURES\n",
                    static_cast<int>(i), static_cast<int>(j));
              }
            }
          }
        }
      }
    } else {
      failFlag = true;
      failString +=
          "Failed to find TEXTURES data (will use placeholder material)\n";
      materialKeys[0] = "_oo_placeholder_material";
      materialCount = 1;

      for (j = 0; j < faceCount; j++) {
        _faces[j].materialIndex = 0;
      }
    }

    if (scanner->scanString("NAMES")) {
      unsigned int count;
      if (!scanner->scanInt(reinterpret_cast<int*>(&count))) {
        failFlag = true;
        failString += "Expected count after NAMES\n";
      } else {
        for (j = 0; j < count; j++) {
          scanner->scanCharactersFromSet(whitespaceAndNewlineCharSet);
          std::string scannedName;
          if (!scanner->scanUpToCharactersFromSet(newlineCharSet,
                                                  &scannedName)) {
            failFlag = true;
            failString += "Expected file name\n";
          } else {
            renameTexturesFrom(std::format("{}", j), scannedName);
          }
        }
      }
    }

    bool explicitTangents = false;

    // Get explicit normals.
    if (scanner->scanString("NORMALS")) {
      _normalMode = oo::ToUnderlying(oo::MeshNormalMode::kExplicit);
      if (!allocateNormalBuffersWithCount(vertexCount)) {
        OO_LOG(oo::kLogAllocationFailure,
               "***** ERROR: failed to allocate memory for model {} ({} "
               "vertices).",
               filename, static_cast<unsigned>(vertexCount));
        return false;
      }

      for (j = 0; j < vertexCount; j++) {
        float x = 0, y = 0, z = 0;  // read only after every scan succeeded (GCC
                                    // -Wmaybe-uninitialized)
        if (!failFlag) {
          if (!scanner->scanFloat(&x)) failFlag = true;
          if (!scanner->scanFloat(&y)) failFlag = true;
          if (!scanner->scanFloat(&z)) failFlag = true;
          if (!failFlag) {
            _normals[j] = vector_normal(make_vector(x, y, z));
          } else {
            failString +=
                std::format("Failed to read a value for vertex[{}] in {}\n",
                            static_cast<int>(j), "NORMALS");
          }
        }
      }

      // Get explicit tangents (only together with vertices).
      if (scanner->scanString("TANGENTS")) {
        for (j = 0; j < vertexCount; j++) {
          float x, y, z;
          if (!failFlag) {
            if (!scanner->scanFloat(&x)) failFlag = true;
            if (!scanner->scanFloat(&y)) failFlag = true;
            if (!scanner->scanFloat(&z)) failFlag = true;
            if (!failFlag) {
              _tangents[j] = vector_normal(make_vector(x, y, z));
            } else {
              failString +=
                  std::format("Failed to read a value for vertex[{}] in {}\n",
                              static_cast<int>(j), "TANGENTS");
            }
          }
        }
      }
    }

    PROFILE("finished parsing");

    if (IsLegacyNormalMode(static_cast<oo::MeshNormalMode>(_normalMode))) {
      checkNormalsAndAdjustWinding();
      PROFILE("finished checkNormalsAndAdjustWinding");
    }
    if (!explicitTangents) {
      generateFaceTangents();
      PROFILE("finished generateFaceTangents");
    }

    // check for smooth shading and recalculate normals
    if (_normalMode == oo::ToUnderlying(oo::MeshNormalMode::kSmooth)) {
      if (!allocateNormalBuffersWithCount(vertexCount)) {
        OO_LOG(oo::kLogAllocationFailure,
               "***** ERROR: failed to allocate memory for model {} ({} "
               "vertices).",
               filename, static_cast<unsigned>(vertexCount));
        return false;
      }
      calculateVertexNormalsAndTangentsWithFaceRefs(faceRefs);
      PROFILE("finished calculateVertexNormalsAndTangents");
    } else if (IsPerVertexNormalMode(
                   static_cast<oo::MeshNormalMode>(_normalMode)) &&
               !explicitTangents) {
      calculateVertexTangentsWithFaceRefs(faceRefs);
      PROFILE("finished calculateVertexTangents");
    }

    // save the resulting data for possible reuse
    if (_cacheWriteable) [[likely]] {
      CacheManagerSetMeshData(modelData(), cacheKey);
      PROFILE("saved to cache");
    }

    if (failFlag) {
      OO_LOG("mesh.error", "{} ..... from {} {}", failString, filename,
             (using_preloaded) ? "(from preloaded data)" : "(from file)");
    }
  }

  calculateBoundingVolumes();
  PROFILE("finished calculateBoundingVolumes");

  // set up vertex arrays for drawing
  if (!setUpVertexArrays()) return false;
  PROFILE("finished setUpVertexArrays");

  return true;
}

#if SCRIBBLE
static void Scribble(void* bytes, size_t size) {
#if OOLITE_BIG_ENDIAN
  constexpr uint32_t kScribble = 0xFEEDFACE;
#else
  constexpr uint32_t kScribble = 0xCEFAEDFE;
#endif

  size /= sizeof(uint32_t);
  uint32_t* mem = bytes;
  while (size--) *mem++ = kScribble;
}
#else
#define Scribble(bytes, size) \
  do {                        \
  } while (0)
#endif

void oo::Mesh::deleteDisplayLists() {
  if (listsReady) {
    OO_ENTER_OPENGL();

    OOGL(glDeleteLists(displayList0, materialCount));
    listsReady = false;
  }
}

void oo::Mesh::resetGraphicsState() {
  deleteDisplayLists();
  rebindMaterials();
  _textureUnitCount = oo::kNotFound;
}

bool oo::Mesh::setUpVertexArrays() {
  OOJS_PROFILE_SCOPE();

  std::size_t fi, vi, mi;

  if (!allocateVertexArrayBuffersWithCount(faceCount)) return false;

  // if smoothed, find any vertices that are between faces of different
  // smoothing groups and mark them as being on an edge and therefore NOT
  // smooth shaded
  std::vector<bool> is_edge_vertex(vertexCount);
  std::vector<GLfloat> smoothGroup(vertexCount);
  for (vi = 0; vi < vertexCount; vi++) {
    is_edge_vertex[vi] = false;
    smoothGroup[vi] = -1;
  }
  if (_normalMode == oo::ToUnderlying(oo::MeshNormalMode::kSmooth)) {
    for (fi = 0; fi < faceCount; fi++) {
      GLfloat rv = _faces[fi].smoothGroup;
      int i;
      for (i = 0; i < 3; i++) {
        vi = _faces[fi].vertex[i];
        if (smoothGroup[vi] < 0.0)  // unassigned
          smoothGroup[vi] = rv;
        else if (smoothGroup[vi] != rv)  // a different colour
          is_edge_vertex[vi] = true;
      }
    }
  }

  // base model, flat or smooth shaded, all triangles
  int tri_index = 0;
  int uv_index = 0;
  int vertex_index = 0;

  // Iterate over material names
  for (mi = 0; mi != materialCount; ++mi) {
    triangle_range[mi].location = static_cast<std::size_t>(tri_index);

    for (fi = 0; fi < faceCount; fi++) {
      Vector normal, tangent;

      if (_faces[fi].materialIndex == mi) {
        for (vi = 0; vi < 3; vi++) {
          int v = static_cast<int>(_faces[fi].vertex[vi]);
          if (IsPerVertexNormalMode(
                  static_cast<oo::MeshNormalMode>(_normalMode))) {
            if (is_edge_vertex[static_cast<std::size_t>(v)]) {
              const NormalAndTangent smoothed = getNormal(
                  static_cast<oo::MeshVertexCount>(v), _faces[fi].smoothGroup);
              normal = smoothed.normal;
              tangent = smoothed.tangent;
            } else {
              OOCAssert(_normals != nullptr && _tangents != nullptr,
                        "Normal/tangent buffers not allocated in %s",
                        __PRETTY_FUNCTION__);

              normal = _normals[v];
              tangent = _tangents[v];
            }
          } else {
            normal = _faces[fi].normal;
            tangent = _faces[fi].tangent;
          }

          // FIXME: avoid redundant vertices so index array is actually useful.
          _displayLists.indexArray[tri_index++] = vertex_index;
          _displayLists.normalArray[vertex_index] = normal;
          _displayLists.tangentArray[vertex_index] = tangent;
          _displayLists.vertexArray[vertex_index++] = _vertices[v];
          _displayLists.textureUVArray[uv_index++] = _faces[fi].s[vi];
          _displayLists.textureUVArray[uv_index++] = _faces[fi].t[vi];
        }
      }
    }
    triangle_range[mi].length =
        static_cast<std::size_t>(tri_index) - triangle_range[mi].location;
  }

  _displayLists.count =
      static_cast<GLuint>(tri_index);  // total number of triangle vertices
  return true;
}

#ifdef OO_DEBUG_MONITOR_ENABLED
void oo::Mesh::debugDrawNormals() {
  GLuint i;
  Vector v, n, t, b;
  float length, blend;
  GLfloat color[3];
  oo::DebugWFState state;

  OO_ENTER_OPENGL();

  state = oo::DebugBeginWireframe(false);

  // Draw
  OOGLBEGIN(GL_LINES);
  for (i = 0; i < _displayLists.count; ++i) {
    v = _displayLists.vertexArray[i];
    n = _displayLists.normalArray[i];
    t = _displayLists.tangentArray[i];
    b = true_cross_product(n, t);

    // Draw normal
    length = magnitude2(n);
    blend = static_cast<float>(fabs(length - 1) * 5.0);
    color[0] = std::min(blend, 1.0f);
    color[1] = 1.0f - color[0];
    color[2] = color[1];
    glColor3fv(color);

    glVertex3f(v.x, v.y, v.z);
    scale_vector(&n, 5.0f);
    n = vector_add(n, v);
    glVertex3f(n.x, n.y, n.z);

    // Draw tangent
    glColor3f(1.0f, 1.0f, 0.0f);
    t = vector_add(v, vector_multiply_scalar(t, 3.0f));
    glVertex3f(v.x, v.y, v.z);
    glVertex3f(t.x, t.y, t.z);

    // Draw bitangent
    glColor3f(0.0f, 1.0f, 0.0f);
    b = vector_add(v, vector_multiply_scalar(b, 3.0f));
    glVertex3f(v.x, v.y, v.z);
    glVertex3f(b.x, b.y, b.z);
  }
  OOGLEND();

  oo::DebugEndWireframe(state);
}
#endif

void oo::Mesh::setRetainedObject(oo::Data object, const std::string& key) {
  _retainedObjects.insert_or_assign(
      key, oo::makeShared<oo::MeshBuffer>(std::move(object)));
}

/* valgrind complains that the memory allocated here isn't initialised
         at the time OOCacheManager::writeDict is pushing it to the
         cache. Not sure if that's a problem or not. - CIM */
void* oo::Mesh::allocateBytesWithSize(size_t size, std::size_t count,
                                      const std::string& key) {
  if (count == 0) {
    count = 1;
  }
  size *= count;
  oo::Data holder;
  holder.setLength(size);  // zero-filled (malloc left it uninitialised)
  setRetainedObject(std::move(holder), key);
  void* bytes = _retainedObjects.find(key)->second->data().mutableBytes();
  if (bytes != nullptr) {
    Scribble(bytes, size);
  }
  return bytes;
}

bool oo::Mesh::allocateVertexBuffersWithCount(std::size_t /*count*/) {
  _vertices = static_cast<Vector*>(
      allocateBytesWithSize(sizeof *_vertices, vertexCount, "vertices"));
  return _vertices != nullptr;
}

bool oo::Mesh::allocateNormalBuffersWithCount(std::size_t /*count*/) {
  _normals = static_cast<Vector*>(
      allocateBytesWithSize(sizeof *_normals, vertexCount, "normals"));
  _tangents = static_cast<Vector*>(
      allocateBytesWithSize(sizeof *_tangents, vertexCount, "tangents"));
  return _normals != nullptr && _tangents != nullptr;
}

bool oo::Mesh::allocateFaceBuffersWithCount(std::size_t /*count*/) {
  _faces = static_cast<oo::MeshFace*>(
      allocateBytesWithSize(sizeof *_faces, faceCount, "faces"));
  return _faces != nullptr;
}

bool oo::Mesh::allocateVertexArrayBuffersWithCount(std::size_t count) {
  _displayLists.indexArray = static_cast<GLint*>(allocateBytesWithSize(
      sizeof *_displayLists.indexArray, count * 3, "indexArray"));
  _displayLists.textureUVArray = static_cast<GLfloat*>(allocateBytesWithSize(
      sizeof *_displayLists.textureUVArray, count * 6, "textureUVArray"));
  _displayLists.vertexArray = static_cast<Vector*>(allocateBytesWithSize(
      sizeof *_displayLists.vertexArray, count * 3, "vertexArray"));
  _displayLists.normalArray = static_cast<Vector*>(allocateBytesWithSize(
      sizeof *_displayLists.normalArray, count * 3, "normalArray"));
  _displayLists.tangentArray = static_cast<Vector*>(allocateBytesWithSize(
      sizeof *_displayLists.tangentArray, count * 3, "tangentArray"));

  return _faces != nullptr && _displayLists.indexArray != nullptr &&
         _displayLists.textureUVArray != nullptr &&
         _displayLists.vertexArray != nullptr &&
         _displayLists.normalArray != nullptr &&
         _displayLists.tangentArray != nullptr;
}

void oo::Mesh::renameTexturesFrom(const std::string& from,
                                  const std::string& to) {
  /*	IMPORTANT: this has to be called before setUpMaterials..., so it can
          only be used during loading.
  */
  oo::MeshMaterialCount i;
  for (i = 0; i != materialCount; i++) {
    if (materialKeys[i] == from) {
      materialKeys[i] = to;
    }
  }
}

static float FaceArea(GLuint* vertIndices, Vector* vertices) {
  /*	Calculate areas using Heron's formula.	*/
  float a2 = distance2(vertices[vertIndices[0]], vertices[vertIndices[1]]);
  float b2 = distance2(vertices[vertIndices[1]], vertices[vertIndices[2]]);
  float c2 = distance2(vertices[vertIndices[2]], vertices[vertIndices[0]]);
  return sqrt(
      (2.0f * (a2 * b2 + b2 * c2 + c2 * a2) - (a2 * a2 + b2 * b2 + c2 * c2)) *
      0.0625f);
}

static float FaceAreaCorrect(GLuint* vertIndices, Vector* vertices) {
  /*	Calculate area of triangle.
          The magnitude of the cross product of two vectors is the area of
          the parallelogram they span. The area of a triangle is half the
          area of a parallelogram sharing two of its sides.
          Since we only use the area of the triangle as a weight factor,
          constant terms are irrelevant, so we don't bother halving the
          value.
  */
  Vector AB =
      vector_subtract(vertices[vertIndices[1]], vertices[vertIndices[0]]);
  Vector AC =
      vector_subtract(vertices[vertIndices[2]], vertices[vertIndices[0]]);
  return magnitude(true_cross_product(AB, AC));
}

void oo::Mesh::checkNormalsAndAdjustWinding() {
  OOJS_PROFILE_SCOPE();

  Vector calculatedNormal;
  oo::MeshFaceCount i;

  OOCParameterAssert(_normalMode !=
                     oo::ToUnderlying(oo::MeshNormalMode::kExplicit));

  for (i = 0; i < faceCount; i++) {
    Vector v0, v1, v2, norm;
    v0 = _vertices[_faces[i].vertex[0]];
    v1 = _vertices[_faces[i].vertex[1]];
    v2 = _vertices[_faces[i].vertex[2]];
    norm = _faces[i].normal;

    calculatedNormal = normal_to_surface(v2, v1, v0);
    if (vector_equal(norm, kZeroVector)) {
      norm = vector_flip(calculatedNormal);
      _faces[i].normal = norm;
    }

    /*	FIXME: for 2.0, either require explicit normals for every model
            or change to: if (dot_product(norm, calculatedNormal) < 0.0f)
            -- Ahruman 2010-01-23
    */
    if (norm.x * calculatedNormal.x < 0 || norm.y * calculatedNormal.y < 0 ||
        norm.z * calculatedNormal.z < 0) {
      // normal lies in the WRONG direction!
      // reverse the winding
      int firstVertex = static_cast<int>(_faces[i].vertex[0]);
      _faces[i].vertex[0] = _faces[i].vertex[2];
      _faces[i].vertex[2] = static_cast<GLuint>(firstVertex);

      GLfloat f0 = _faces[i].s[0];
      _faces[i].s[0] = _faces[i].s[2];
      _faces[i].s[2] = f0;

      f0 = _faces[i].t[0];
      _faces[i].t[0] = _faces[i].t[2];
      _faces[i].t[2] = f0;
    }
  }
}

void oo::Mesh::generateFaceTangents() {
  OOJS_PROFILE_SCOPE();

  oo::MeshFaceCount i;
  for (i = 0; i < faceCount; i++) {
    oo::MeshFace* face = _faces + i;

    /*	Generate tangents, i.e. vectors that run in the direction of the s
            texture coordinate. Based on code I found in a forum somewhere and
            then lost track of. Sorry to whomever I should be crediting.
            -- Ahruman 2008-11-23
    */
    Vector vAB =
        vector_subtract(_vertices[face->vertex[1]], _vertices[face->vertex[0]]);
    Vector vAC =
        vector_subtract(_vertices[face->vertex[2]], _vertices[face->vertex[0]]);
    Vector nA = face->normal;

    // projAB = aB - (nA . vAB) * nA
    Vector vProjAB =
        vector_subtract(vAB, vector_multiply_scalar(nA, dot_product(nA, vAB)));
    Vector vProjAC =
        vector_subtract(vAC, vector_multiply_scalar(nA, dot_product(nA, vAC)));

    // delta s/t
    GLfloat dsAB = face->s[1] - face->s[0];
    GLfloat dsAC = face->s[2] - face->s[0];
    GLfloat dtAB = face->t[1] - face->t[0];
    GLfloat dtAC = face->t[2] - face->t[0];

    if (dsAC * dtAB > dsAB * dtAC) {
      face->tangent =
          vector_normal(vector_subtract(vector_multiply_scalar(vProjAC, dtAB),
                                        vector_multiply_scalar(vProjAB, dtAC)));
    } else {
      face->tangent =
          vector_normal(vector_subtract(vector_multiply_scalar(vProjAB, dtAC),
                                        vector_multiply_scalar(vProjAC, dtAB)));
    }
  }
}

void oo::Mesh::calculateVertexNormalsAndTangentsWithFaceRefs(
    VertexFaceRef* faceRefs) {
  OOJS_PROFILE_SCOPE();

  OOCParameterAssert(faceRefs != nullptr);

  std::size_t i, j;
  std::vector<float> triangle_area(faceCount);

  OOCAssert(_normals != nullptr && _tangents != nullptr,
            "Normal/tangent buffers not allocated in %s", __PRETTY_FUNCTION__);

  for (i = 0; i < faceCount; i++) {
    triangle_area[i] = FaceArea(_faces[i].vertex, _vertices);
  }
  for (i = 0; i < vertexCount; i++) {
    Vector normal_sum = kZeroVector;
    Vector tangent_sum = kZeroVector;

    VertexFaceRef* vfr = &faceRefs[i];
    std::size_t fIter, fCount = VFRGetCount(vfr);
    for (fIter = 0; fIter < fCount; fIter++) {
      j = VFRGetFaceAtIndex(vfr, fIter);

      float t = triangle_area[j];  // weight sum by area
      normal_sum =
          vector_add(normal_sum, vector_multiply_scalar(_faces[j].normal, t));
      tangent_sum =
          vector_add(tangent_sum, vector_multiply_scalar(_faces[j].tangent, t));
    }

    normal_sum = vector_normal_or_fallback(normal_sum, kBasisZVector);
    tangent_sum = vector_subtract(
        tangent_sum, vector_multiply_scalar(
                         normal_sum, dot_product(tangent_sum, normal_sum)));
    tangent_sum = vector_normal_or_fallback(tangent_sum, kBasisXVector);

    _normals[i] = normal_sum;
    _tangents[i] = tangent_sum;
  }
}

void oo::Mesh::calculateVertexTangentsWithFaceRefs(VertexFaceRef* faceRefs) {
  OOJS_PROFILE_SCOPE();

  OOCParameterAssert(faceRefs != nullptr);

  /*	This is conceptually broken.
          At the moment, it's calculating one tangent per "input" vertex. It
     should be calculating one tangent per "real" vertex, where a "real" vertex
     is defined as a combination of position, normal, material and texture
          coordinates.
          Currently, we don't have a format with unique "real" vertices.
          This basically means explicit-normal models without explicit tangents
          can't usefully be normal mapped.
          I don't intend to do anything about this pre-MSNR, although it might
     be possible to fix it by moving tangent generation to the same stage as
          smooth-grouped normal generation.
          -- Ahruman 2010-05-22
  */
  std::size_t i, j;
  std::vector<float> triangle_area(faceCount);
  for (i = 0; i < faceCount; i++) {
    triangle_area[i] = FaceAreaCorrect(_faces[i].vertex, _vertices);
  }
  for (i = 0; i < vertexCount; i++) {
    Vector tangent_sum = kZeroVector;

    VertexFaceRef* vfr = &faceRefs[i];
    std::size_t fIter, fCount = VFRGetCount(vfr);
    for (fIter = 0; fIter < fCount; fIter++) {
      j = VFRGetFaceAtIndex(vfr, fIter);

      float t = triangle_area[j];  // weight sum by area
      tangent_sum =
          vector_add(tangent_sum, vector_multiply_scalar(_faces[j].tangent, t));
    }

    tangent_sum = vector_subtract(
        tangent_sum, vector_multiply_scalar(
                         _normals[i], dot_product(_normals[i], tangent_sum)));
    tangent_sum = vector_normal_or_fallback(tangent_sum, kBasisXVector);

    _tangents[i] = tangent_sum;
  }
}

/* profiling suggests this function takes a lot of time - almost all
 * the overhead of setting up a new ship is here. - CIM */
oo::Mesh::NormalAndTangent oo::Mesh::getNormal(
    oo::MeshVertexCount v_index, oo::MeshSmoothGroup smoothGroup) {
  OOJS_PROFILE_SCOPE();

  std::size_t j;
  Vector normal_sum = kZeroVector;
  Vector tangent_sum = kZeroVector;
  for (j = 0; j < faceCount; j++) {
    if (_faces[j].smoothGroup == smoothGroup) {
      if ((_faces[j].vertex[0] == v_index) ||
          (_faces[j].vertex[1] == v_index) ||
          (_faces[j].vertex[2] == v_index)) {
        float area = FaceArea(_faces[j].vertex, _vertices);
        normal_sum = vector_add(normal_sum,
                                vector_multiply_scalar(_faces[j].normal, area));
        tangent_sum = vector_add(
            tangent_sum, vector_multiply_scalar(_faces[j].tangent, area));
      }
    }
  }

  return {vector_normal_or_fallback(normal_sum, kBasisZVector),
          vector_normal_or_fallback(tangent_sum, kBasisXVector)};
}

void oo::Mesh::calculateBoundingVolumes() {
  OOJS_PROFILE_SCOPE();

  oo::MeshVertexCount i;
  float d_squared, length_longest_axis, length_shortest_axis;
  GLfloat result;

  result = 0.0f;
  if (vertexCount)
    bounding_box_reset_to_vector(&_boundingBox, _vertices[0]);
  else
    bounding_box_reset(&_boundingBox);

  for (i = 0; i < vertexCount; i++) {
    d_squared = magnitude2(_vertices[i]);
    if (d_squared > result) result = d_squared;
    bounding_box_add_vector(&_boundingBox, _vertices[i]);
  }

  length_longest_axis = _boundingBox.max.x - _boundingBox.min.x;
  if (_boundingBox.max.y - _boundingBox.min.y > length_longest_axis)
    length_longest_axis = _boundingBox.max.y - _boundingBox.min.y;
  if (_boundingBox.max.z - _boundingBox.min.z > length_longest_axis)
    length_longest_axis = _boundingBox.max.z - _boundingBox.min.z;

  length_shortest_axis = _boundingBox.max.x - _boundingBox.min.x;
  if (_boundingBox.max.y - _boundingBox.min.y < length_shortest_axis)
    length_shortest_axis = _boundingBox.max.y - _boundingBox.min.y;
  if (_boundingBox.max.z - _boundingBox.min.z < length_shortest_axis)
    length_shortest_axis = _boundingBox.max.z - _boundingBox.min.z;

  d_squared = static_cast<float>((length_longest_axis + length_shortest_axis) *
                                 (length_longest_axis + length_shortest_axis) *
                                 0.25);  // square of average length
  _maxDrawDistance = static_cast<GLfloat>(
      d_squared * NO_DRAW_DISTANCE_FACTOR *
      NO_DRAW_DISTANCE_FACTOR);  // no longer based on the collision radius

  _collisionRadius = sqrtf(result);
}

void oo::Mesh::rescaleByFactor(GLfloat factor) {
  // Rescale base vertices used for geometry calculations.
  oo::MeshVertexCount i;
  Vector* vertex = nullptr;

  for (i = 0; i < vertexCount; i++) {
    vertex = &_vertices[i];
    *vertex = vector_multiply_scalar(*vertex, factor);
  }

  // Rescale actual display vertices.
  for (i = 0; i < _displayLists.count; i++) {
    vertex = &_displayLists.vertexArray[i];
    *vertex = vector_multiply_scalar(*vertex, factor);
  }

  calculateBoundingVolumes();
  octree = nullptr;
  baseFile.reset();  // Avoid octree cache.
  baseFileOctreeCacheRef.reset();
}

BoundingBox oo::Mesh::boundingBox() { return _boundingBox; }

namespace oo {

static const char* const kCacheMeshes = "OOMesh";

}  // namespace oo

// The OOCacheManager (OOMesh) category, as free functions next to the cache.

namespace {

oo::PList CacheManagerMeshDataForName(const std::string& inShipName) {
  return oo::CacheManager::sharedCache()->pListForKey(inShipName,
                                                      oo::kCacheMeshes);
}

void CacheManagerSetMeshData(const oo::PList& inData,
                             const std::string& inShipName) {
  if (inData) {
    oo::CacheManager::sharedCache()->setPList(inData, inShipName,
                                              oo::kCacheMeshes);
  }
}

}  // namespace

namespace oo {

static const char* const kCacheOctrees = "octrees";

// The OOCacheManager (Octree) category, as free functions next to the cache
// (OOMesh.h).

std::shared_ptr<Octree> CacheManagerOctreeForModel(const std::string& inKey) {
  std::shared_ptr<Octree> result;
  CacheManager* cache = CacheManager::sharedCache();

  const oo::PList data =
      cache->pListForKey(inKey, kCacheOctrees);  // null: absent
  if (data) {
    result = Octree::initWithDictionary(data);
  }

  return result;
}

void CacheManagerSetOctree(Octree* inOctree, const std::string& inKey) {
  if (inOctree != nullptr) {
    CacheManager::sharedCache()->setPList(inOctree->dictionaryRepresentation(),
                                          inKey, kCacheOctrees);
  }
}

}  // namespace oo

static void VFRAddFace(VertexFaceRef* vfr, std::size_t index) {
  OOCParameterAssert(vfr != nullptr);

  if (index < UINT16_MAX && vfr->internCount < kVertexFaceDefInternalCount) {
    vfr->internFaces[vfr->internCount++] = static_cast<uint16_t>(index);
  } else {
    vfr->extra.push_back(index);
  }
}

static std::size_t VFRGetCount(VertexFaceRef* vfr) {
  OOCParameterAssert(vfr != nullptr);

  return vfr->internCount + vfr->extra.size();
}

static std::size_t VFRGetFaceAtIndex(VertexFaceRef* vfr, std::size_t index) {
  OOCParameterAssert(vfr != nullptr && index < VFRGetCount(vfr));

  if (index < vfr->internCount)
    return vfr->internFaces[index];
  else
    return vfr->extra[index - vfr->internCount];
}
