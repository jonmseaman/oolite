/*

OOMesh.h

Standard OODrawable for static meshes from DAT files. OOMeshes are immutable
(and can therefore be shared). Avoid the temptation to add externally-visible
mutator methods as it will break such sharing. (Sharing will be implemented
when ship types are turned into objects instead of dictionaries; this is
currently slated for post-1.70. -- Ahruman)

Hmm. On further consideration, sharing will be problematic because of material
bindings. Two possible solutions: separate mesh data into shared object with
each mesh instance having its own set of materials but shared data, or
retarget bindings each frame. -- Ahruman


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

#include "Drawable.h"
#include "GraphicsResetManager.h"
#include "OpenGL.h"
#include "OpenGLExtensionManager.h"
class Entity;  // Entity.h: the shader binding target

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

namespace oo {

class Material;

}  // namespace oo

// OOMesh.cpp: the faces that use one vertex, while loading
struct VertexFaceRef;

namespace oo {

class MeshBuffer;

}  // namespace oo

// OOMesh.cpp: one refcounted buffer (an oo::Data), shared by a mesh and its
// mutable copies

#define OOMESH_PROFILE 0
#if OOMESH_PROFILE
#include "ProfilingStopwatch.h"
#endif

namespace oo {

inline constexpr int kMeshMaxMaterials = 8;

using MeshSmoothGroup = uint16_t;
using MeshMaterialIndex = uint8_t;
using MeshMaterialCount = uint8_t;
using MeshVertexCount = uint32_t;
using MeshFaceCount = uint32_t;
using MeshFaceVertexCount = uint8_t;

struct MeshFace {
  MeshSmoothGroup smoothGroup;
  MeshMaterialIndex materialIndex;
  GLuint vertex[3];

  Vector normal;
  Vector tangent;
  GLfloat s[3];
  GLfloat t[3];
};

struct MeshDisplayLists {
  GLint* indexArray;
  GLfloat* textureUVArray;
  Vector* vertexArray;
  Vector* normalArray;
  Vector* tangentArray;

  GLuint count;
};

}  // namespace oo

class Octree;

/*	A mesh: the class shell, its factories, lifecycle and accessors,
   loading, geometry and rendering. Its callers hold std::shared_ptr<OOMesh>.
*/
namespace oo {

class Mesh : public Drawable, public GraphicsResetClient {
 public:
  static std::shared_ptr<Mesh> meshWithName(
      const std::string& name, const std::optional<std::string>& cacheKey,
      const oo::PList& materialDict, const oo::PList& shadersDict, bool smooth,
      const oo::PList& macros, Entity* object);

  static std::shared_ptr<Mesh> meshWithName(
      const std::string& name, const std::optional<std::string>& cacheKey,
      const oo::PList& materialDict, const oo::PList& shadersDict, bool smooth,
      const oo::PList& macros, Entity* object, float factor,
      bool cacheWriteable);

  static std::shared_ptr<oo::Material> placeholderMaterial();

  Mesh();
  // -mutableCopyWithZone:'s copy: every member, the buffers shared
  Mesh(const Mesh&) = default;
  ~Mesh() override;

  // The nullptr_t parameter keeps the old -copyWithZone:nil call shape.
  std::shared_ptr<Mesh> copyWithZone();
  std::shared_ptr<Mesh> mutableCopyWithZone();

  std::optional<std::string> modelName();

  // null: none (-materials; the member materials is the materials)
  oo::PList getMaterials();
  oo::PList shaders();

  size_t getVertexCount();  // -vertexCount
  size_t getFaceCount();    // -faceCount

  // OODrawable
  bool hasOpaqueParts() override;
  GLfloat collisionRadius() override;
  GLfloat maxDrawDistance() override;
  void setBindingTarget(Entity* target) override;
  std::optional<std::string> descriptionComponents() const override;
#ifdef OO_DEBUG_MONITOR_ENABLED
  void dumpSelfState() override;
  std::vector<std::shared_ptr<oo::Texture>> allTextures() override;
  size_t totalSize() override;

  size_t objectSize() const override { return sizeof *this; }
#endif

  std::shared_ptr<Octree>
  getOctree();  // -octree (the member octree is the octree)

  // This needs a better name.
  BoundingBox findBoundingBoxRelativeToPosition(Vector opv, Vector ri,
                                                Vector rj, Vector rk,
                                                Vector position, Vector si,
                                                Vector sj, Vector sk);
  BoundingBox findSubentityBoundingBoxWithPosition(Vector position,
                                                   Matrix rotMatrix);

  std::shared_ptr<Mesh> meshRescaledBy(GLfloat scaleFactor);

  BoundingBox boundingBox() override;

  void renderOpaqueParts() override;

  void rebindMaterials();

  // OOGraphicsResetClient: the mesh registers itself once loaded, and
  // unregisters when destroyed.
  void resetGraphicsState() override;

  // Internal: the state, public while the test reads it.
  // Zero, as class_createInstance left the ivars.
  uint8_t _normalMode : 2 = 0, brokenInRender : 1 = 0, listsReady : 1 = 0;

  MeshMaterialCount materialCount = {};
  MeshVertexCount vertexCount = {};
  MeshFaceCount faceCount = {};

  std::optional<std::string>
      // "No Model" until loaded; nullopt after -rescaleByFactor: (no octree
      // cache)
      baseFile;
  std::optional<std::string> baseFileOctreeCacheRef;
  bool _cacheWriteable = {};

  Vector* _vertices = {};
  Vector* _normals = {};
  Vector* _tangents = {};
  MeshFace* _faces = {};

  // Redundancy! Needs fixing.
  MeshDisplayLists _displayLists = {};

  NSRange triangle_range[kMeshMaxMaterials] = {};
  std::string materialKeys[kMeshMaxMaterials];
  std::shared_ptr<oo::Material> materials[kMeshMaxMaterials];  // shared
  GLuint displayList0 = {};

  // (named like the overridden getters, which a C++ member cannot be)
  GLfloat _collisionRadius = {};
  GLfloat _maxDrawDistance = {};
  BoundingBox _boundingBox = {};

  std::shared_ptr<Octree> octree;

  // the buffers _vertices & co. point into, by key
  std::map<std::string, std::shared_ptr<MeshBuffer>, std::less<>>
      _retainedObjects;

  oo::PList _materialDict;  // mixed configurations; null = nil
  oo::PList _shadersDict;
  std::optional<std::string> _cacheKey;  // nil and @"" differ for OOMaterial
  oo::PList _shaderMacros;
  // the entity, held weakly as its object's weak reference was
  std::weak_ptr<Entity> _shaderBindingTarget = {};

  Vector _lastPosition = {};
  Matrix _lastRotMatrix = {};
  BoundingBox _lastBoundingBox = {};

#if OO_MULTITEXTURE
  std::size_t _textureUnitCount = {};
#endif

#if OOMESH_PROFILE
  std::shared_ptr<ProfilingStopwatch> _stopwatch;
  double _stopwatchLastTime = {};
#endif

  // Internal: geometry, which the loading calls.
  void checkNormalsAndAdjustWinding();
  void generateFaceTangents();
  void calculateVertexNormalsAndTangentsWithFaceRefs(VertexFaceRef* faceRefs);
  void calculateVertexTangentsWithFaceRefs(VertexFaceRef* faceRefs);

  struct NormalAndTangent {
    Vector normal;
    Vector tangent;
  };

  NormalAndTangent getNormal(MeshVertexCount v_index,
                             MeshSmoothGroup smoothGroup);
  void calculateBoundingVolumes();
  void rescaleByFactor(GLfloat factor);

  // Internal: buffers and display lists, which the loading and the destructor
  // call.
  void deleteDisplayLists();
  bool setUpVertexArrays();

  // Manage the set of refcounted buffers we need to hang on to.
  void setRetainedObject(oo::Data object, const std::string& key);
  void* allocateBytesWithSize(size_t size, std::size_t count,
                              const std::string& key);

  // Allocate all per-vertex/per-face buffers.
  bool allocateVertexBuffersWithCount(std::size_t count);
  bool allocateNormalBuffersWithCount(std::size_t count);
  bool allocateFaceBuffersWithCount(std::size_t count);
  bool allocateVertexArrayBuffersWithCount(std::size_t count);

  void renameTexturesFrom(const std::string& from, const std::string& to);

 private:
  // Loading.
  bool initWithName(const std::string& name,
                    const std::optional<std::string>& cacheKey,
                    const oo::PList& materialDict, const oo::PList& shadersDict,
                    bool smooth, const oo::PList& macros, Entity* target,
                    float scale, bool cacheWriteable);
  bool loadData(const std::string& filename, float scale);
  oo::PList modelData();  // null: incomplete
  bool setModelFromModelData(const oo::PList& dict,
                             const std::string& fileName);

#ifdef OO_DEBUG_MONITOR_ENABLED
  void debugDrawNormals();
#endif
  unsigned octreeDepth();
  bool suppressClangStuff();
};

// The OOCacheManager (Octree) category, as free functions next to the cache.
// The octree cached for a model key, made afresh from its representation; null
// when none.
std::shared_ptr<Octree> CacheManagerOctreeForModel(const std::string& inKey);
// Caches the octree's representation under the key; a null octree does nothing.
void CacheManagerSetOctree(Octree* inOctree, const std::string& inKey);

}  // namespace oo
