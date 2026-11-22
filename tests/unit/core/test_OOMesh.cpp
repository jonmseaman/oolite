/*	test_OOMesh.mm
        Unit tests for OOMesh (src/Core/OOMesh.h/.mm), the drawable of a static
   mesh from a DAT file, a subclass of the root OODrawable.

        The mesh loads its model through the resource manager, caches its data
   and octree in the cache manager, makes its materials and registers with the
   graphics reset manager, so the test links every game object but main's
   (tests/unit/core/meson.build entry ['*']) and runs on the hidden GL context
   of oo_gl_test_context.hpp. The models are DAT files the test writes to a
   scratch Resources/Models folder, which is the only resource root: the current
   directory, the home and the add-on folders all point into the scratch folder,
   so no add-on is ever scanned. The tetrahedron has no TEXTURES section, so its
   one material is the placeholder material and no texture is loaded.

        The expectations cover: loading (counts, name, radius, draw distance,
   bounding box, parts, the material and shader dictionaries, the description),
   scaling, a missing model, the mesh and octree caches, the placeholder
   material, -copy and -mutableCopy, -meshRescaledBy:, the subentity bounding
   box, the debug state and size, and a graphics reset after a mesh is gone (it
   unregistered itself). The cases call the C++ class (global OOMesh);
   OctreeForModel and SetOctreeForModel below call the OOCacheManager octree
   functions. facadeContract keeps the C++ members' answers, the root's C++
   pointer and the factory. geometryMembers checks the geometry members,
   renderingMembers the rendering members, and loadingMembers the loading and
   copying members and the mesh as its own graphics reset client. Run: bash
   tests/run.sh unit
*/

#include <process.h>

#include <cmath>
#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <string>

#include <gtest/gtest.h>

#include "Entity.h"
#include "BasicMaterial.h"
#include "CacheManager.h"
#include "GraphicsResetManager.h"
#include "Mesh.h"
#include "Octree.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/FileSystem.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

namespace stdfs = std::filesystem;

stdfs::path sRoot;

void WriteText(const stdfs::path& path, const std::string& text) {
  stdfs::create_directories(path.parent_path());
  EXPECT_TRUE(oo::fs::writeFile(path, oo::Data(text.data(), text.size()),
                             oo::fs::WriteMode::kDirect)
               .has_value());
}

// A tetrahedron with its right angle at the origin and edges of 10 m, and no
// TEXTURES section.
const char* const kTetrahedron =
    "NVERTS 4\n"
    "NFACES 4\n"
    "VERTEX\n"
    "0 0 0\n"
    "10 0 0\n"
    "0 10 0\n"
    "0 0 10\n"
    "FACES\n"
    "0 0 0  0 0 -1  3 0 2 1\n"
    "0 0 0  0 -1 0  3 0 1 3\n"
    "0 0 0  -1 0 0  3 0 3 2\n"
    "0 0 0  1 1 1  3 1 2 3\n"
    "END\n";

// The same tetrahedron with explicit vertex normals.
const char* const kTetrahedronWithNormals =
    "NVERTS 4\n"
    "NFACES 4\n"
    "VERTEX\n"
    "0 0 0\n"
    "10 0 0\n"
    "0 10 0\n"
    "0 0 10\n"
    "FACES\n"
    "0 0 0  0 0 -1  3 0 2 1\n"
    "0 0 0  0 -1 0  3 0 1 3\n"
    "0 0 0  -1 0 0  3 0 3 2\n"
    "0 0 0  1 1 1  3 1 2 3\n"
    "NORMALS\n"
    "-1 -1 -1\n"
    "1 0 0\n"
    "0 1 0\n"
    "0 0 1\n"
    "END\n";

// The scratch game folder, made once, before the resource and cache managers
// exist.
void SetUp() {
  if (!sRoot.empty()) return;
  sRoot = stdfs::temp_directory_path() /
          ("oo-test-mesh-" +
           std::to_string(static_cast<unsigned long>(::_getpid())));
  stdfs::remove_all(sRoot);
  stdfs::create_directories(sRoot / "game");
  EXPECT_TRUE(::_putenv_s("HOMEPATH", (sRoot / "home").string().c_str()) == 0);
  EXPECT_TRUE(::_putenv_s("OO_MANAGEDADDONSDIR",
                       (sRoot / "managed").string().c_str()) == 0);
  EXPECT_TRUE(::_putenv_s("OO_ADDONSEXTRACTDIR",
                       (sRoot / "extract").string().c_str()) == 0);
  stdfs::current_path(sRoot / "game");
  WriteText(sRoot / "game" / "Resources" / "Info-gnustep.plist",
            "{ CFBundleVersion = \"9.9.9-test\"; }");
  WriteText(sRoot / "game" / "Resources" / "Models" / "tetra.dat",
            kTetrahedron);
  WriteText(sRoot / "game" / "Resources" / "Models" / "tetra2.dat",
            kTetrahedron);
  WriteText(sRoot / "game" / "Resources" / "Models" / "tetra3.dat",
            kTetrahedron);
  WriteText(sRoot / "game" / "Resources" / "Models" / "normals.dat",
            kTetrahedronWithNormals);
  EXPECT_TRUE(OOTestGLContext());
}

std::shared_ptr<oo::Mesh> Mesh(const std::string& name, float scale = 1.0f,
                     bool cacheWriteable = true, bool smooth = false,
                     const oo::PList& materials = oo::PList(),
                     const oo::PList& shaders = oo::PList()) {
  return oo::Mesh::meshWithName(name, std::nullopt, materials, shaders, smooth,
                              oo::PList(), nullptr, scale, cacheWriteable);
}

oo::PList CachedPList(const std::string& key, const std::string& cache) {
  return oo::CacheManager::sharedCache()->pListForKey(key, cache);
}

// The OOCacheManager (Octree) category: free functions on the C++ octree.
std::shared_ptr<Octree> OctreeForModel(const std::string& key) {
  return oo::CacheManagerOctreeForModel(key);
}

// The mesh's octree.
Octree* MeshOctree(oo::Mesh* mesh) { return mesh->getOctree().get(); }

void SetOctreeForModel(Octree* octree, const std::string& key) {
  oo::CacheManagerSetOctree(octree, key);
}

bool Near(double a, double b) {
  return std::fabs(a - b) < 1e-4 * std::fmax(1.0, std::fabs(b));
}

bool SameVector(Vector a, Vector b) {
  return Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z);
}

}  // namespace

TEST(OOMesh, loadsModel) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    EXPECT_TRUE(mesh != nullptr);
    if (mesh == nullptr) return;
    EXPECT_TRUE(dynamic_cast<oo::Mesh*>(mesh.get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<oo::Drawable*>(mesh.get()) != nullptr);
    EXPECT_EQ(mesh->modelName().value_or("<none>"), "tetra.dat");
    EXPECT_EQ(mesh->getVertexCount(), 4u);
    EXPECT_EQ(mesh->getFaceCount(), 4u);
    EXPECT_TRUE(Near(mesh->collisionRadius(), 10.0));
    // The average of the longest and shortest sides, squared, times the no-draw
    // factor squared.
    EXPECT_TRUE(Near(mesh->maxDrawDistance(),
                  100.0 * NO_DRAW_DISTANCE_FACTOR * NO_DRAW_DISTANCE_FACTOR));
    const BoundingBox box = mesh->boundingBox();
    EXPECT_TRUE(SameVector(box.min, make_vector(0, 0, 0)));
    EXPECT_TRUE(SameVector(box.max, make_vector(10, 10, 10)));
    EXPECT_TRUE(mesh->hasOpaqueParts());
    EXPECT_TRUE(!mesh->hasTranslucentParts());
    EXPECT_TRUE(!mesh->getMaterials());
    EXPECT_TRUE(!mesh->shaders());

    const std::string description = mesh->description();
    EXPECT_TRUE(description.starts_with("<OOMesh "));
    EXPECT_TRUE(description.find("{\"tetra.dat\", 4 vertices, 4 faces, radius: 10 "
                              "m normals: per-face}") != std::string::npos);

    // Binding targets go to the materials; dumping the state only logs.
    mesh->setBindingTarget(nullptr);
    mesh->dumpSelfState();

#ifndef NDEBUG
    // The placeholder material has no textures; the size counts the vertex and
    // face buffers.
    EXPECT_TRUE(mesh->allTextures().empty());
    EXPECT_TRUE(mesh->totalSize() >= 4 * sizeof(Vector) + 4 * sizeof(oo::MeshFace));
#endif
  }
}

TEST(OOMesh, dictionariesAndSmoothing) {
  ::SetUp();
  {
    oo::PList::Dict materials;
    materials["other.png"] = oo::PList(oo::PList::Dict{});
    oo::PList::Dict shaders;
    shaders["other.png"] = oo::PList(oo::PList::Dict{});
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra2.dat", 1.0f, true, true,
                                      oo::PList(materials), oo::PList(shaders));
    EXPECT_TRUE(mesh != nullptr);
    if (mesh == nullptr) return;
    EXPECT_TRUE(mesh->getMaterials() == oo::PList(materials));
    EXPECT_TRUE(mesh->shaders() == oo::PList(shaders));
    EXPECT_TRUE(mesh->description().find("normals: smooth}") != std::string::npos);
  }
}

TEST(OOMesh, scaleAndMissingModel) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra3.dat", 2.0f, false);
    EXPECT_TRUE(mesh != nullptr);
    if (mesh == nullptr) return;
    EXPECT_TRUE(Near(mesh->collisionRadius(), 20.0));
    EXPECT_TRUE(SameVector(mesh->boundingBox().max, make_vector(20, 20, 20)));
    // Not cache-writeable: neither the mesh data nor its octree is cached.
    EXPECT_TRUE(!CachedPList("tetra3.dat:0:2.000", "OOMesh"));
    EXPECT_TRUE(MeshOctree(mesh.get()) != nullptr);
    EXPECT_TRUE(!CachedPList("tetra3.dat-2.000", "octrees"));

    EXPECT_TRUE(Mesh("no such model.dat") == nullptr);
  }
}

TEST(OOMesh, caches) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    EXPECT_TRUE(mesh != nullptr);
    if (mesh == nullptr) return;

    // The OOCacheManager (OOMesh) category: the mesh data, under name:normal
    // mode:scale.
    const oo::PList meshData = CachedPList("tetra.dat:0:1.000", "OOMesh");
    EXPECT_TRUE(meshData.isDict());
    EXPECT_EQ(meshData.get<unsigned int>("vertex count"), 4u);
    EXPECT_EQ(meshData.get<unsigned int>("face count"), 4u);

    // The octree is made once, and cached under name-scale.
    Octree* octree = MeshOctree(mesh.get());
    EXPECT_TRUE(octree != nullptr);
    EXPECT_TRUE(MeshOctree(mesh.get()) == octree);
    const oo::PList octreeData = CachedPList("tetra.dat-1.000", "octrees");
    EXPECT_TRUE(octreeData.isDict());
    EXPECT_TRUE(octreeData == octree->dictionaryRepresentation());

    // The OOCacheManager (Octree) category.
    std::shared_ptr<Octree> cached = OctreeForModel("tetra.dat-1.000");
    EXPECT_TRUE(cached.get() != nullptr);
    EXPECT_TRUE(cached.get() != octree);
    EXPECT_TRUE(cached->dictionaryRepresentation() == octreeData);
    EXPECT_TRUE(OctreeForModel("no such model-1.000").get() == nullptr);
    SetOctreeForModel(octree, "copied-1.000");
    EXPECT_TRUE(CachedPList("copied-1.000", "octrees") == octreeData);
    SetOctreeForModel(nullptr, "nil-1.000");  // does nothing
    EXPECT_TRUE(!CachedPList("nil-1.000", "octrees"));
  }
}

TEST(OOMesh, placeholderMaterial) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Material> placeholder = oo::Mesh::placeholderMaterial();
    EXPECT_TRUE(placeholder != nullptr);
    EXPECT_TRUE(dynamic_cast<oo::BasicMaterial*>(placeholder.get()) != nullptr);
    EXPECT_EQ(placeholder->name().value_or("<none>"), "/placeholder/");
    EXPECT_TRUE(oo::Mesh::placeholderMaterial() == placeholder);
  }
  {
    const std::shared_ptr<oo::Material> again = oo::Mesh::placeholderMaterial();
    EXPECT_TRUE(again != nullptr);
    EXPECT_EQ(again->name().value_or("<none>"), "/placeholder/");
  }
}

TEST(OOMesh, copies) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    EXPECT_TRUE(mesh != nullptr);
    if (mesh == nullptr) return;

    // Immutable seen from outside: a copy is the mesh itself, retained.
    const std::shared_ptr<oo::Mesh> copy = mesh->copyWithZone(nullptr);
    EXPECT_TRUE(copy == mesh);

    // A mutable copy is a new mesh with the same model, sharing nothing it can
    // change.
    const std::shared_ptr<oo::Mesh> mutableCopy = mesh->mutableCopyWithZone(nullptr);
    EXPECT_TRUE(mutableCopy != nullptr);
    EXPECT_TRUE(mutableCopy != mesh);
    EXPECT_TRUE(dynamic_cast<oo::Mesh*>(mutableCopy.get()) != nullptr);
    EXPECT_EQ(mutableCopy->modelName().value_or("<none>"), "tetra.dat");
    EXPECT_EQ(mutableCopy->getVertexCount(), 4u);
    EXPECT_EQ(mutableCopy->getFaceCount(), 4u);
    EXPECT_TRUE(Near(mutableCopy->collisionRadius(), 10.0));
    EXPECT_TRUE(MeshOctree(mutableCopy.get()) != nullptr);

    // The subentity bounding box: the vertices moved by the position and
    // rotated.
    const BoundingBox moved = mesh->findSubentityBoundingBoxWithPosition(
        make_vector(1, 2, 3), kIdentityMatrix);
    EXPECT_TRUE(SameVector(moved.min, make_vector(1, 2, 3)));
    EXPECT_TRUE(SameVector(moved.max, make_vector(11, 12, 13)));

    // Rescaled: a mutable copy, scaled, with no name (so its octree is not
    // cached).
    const std::shared_ptr<oo::Mesh> rescaled = mesh->meshRescaledBy(3.0f);
    EXPECT_TRUE(rescaled != nullptr && rescaled != mesh);
    EXPECT_TRUE(Near(rescaled->collisionRadius(), 30.0));
    EXPECT_TRUE(!rescaled->modelName().has_value());
    EXPECT_TRUE(SameVector(rescaled->boundingBox().max, make_vector(30, 30, 30)));
    EXPECT_TRUE(Near(mesh->collisionRadius(), 10.0));
    EXPECT_TRUE(SameVector(mesh->boundingBox().max, make_vector(10, 10, 10)));
    // The copy shares the original's vertex buffer, so the original's vertices
    // were scaled too (its radius and bounding box were not recalculated).
    const BoundingBox shared = mesh->findSubentityBoundingBoxWithPosition(
        make_vector(0, 0, 0), kIdentityMatrix);
    EXPECT_TRUE(SameVector(shared.max, make_vector(30, 30, 30)));
  }
}

TEST(OOMesh, graphicsResetAfterRelease) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    EXPECT_TRUE(mesh != nullptr);
    std::shared_ptr<oo::Mesh> mutableCopy = mesh->mutableCopyWithZone(nullptr);
    // Both are registered: the reset rebinds their materials.
    oo::GraphicsResetManager::sharedManager()->resetGraphicsState();
    EXPECT_EQ(mesh->getVertexCount(), 4u);
    EXPECT_EQ(mutableCopy->getVertexCount(), 4u);
    mutableCopy = nullptr;
  }
  // Both are gone and unregistered themselves, so the reset does not reach
  // them.
  oo::GraphicsResetManager::sharedManager()->resetGraphicsState();
  EXPECT_TRUE(true);
}

// Pins for loading, geometry and rendering.
TEST(OOMesh, laterSlices) {
  ::SetUp();
  {
    // Loading: a smooth mesh caches its normals and tangents with
    // normal mode 1.
    const std::shared_ptr<oo::Mesh> smooth = Mesh("tetra2.dat", 1.0f, true, true);
    EXPECT_TRUE(smooth != nullptr);
    const oo::PList smoothData = CachedPList("tetra2.dat:1:1.000", "OOMesh");
    EXPECT_TRUE(smoothData.isDict());
    EXPECT_EQ(smoothData.get<unsigned int>("normal mode"), 1u);
    EXPECT_TRUE(smoothData.find("normal data") != nullptr);
    EXPECT_TRUE(smoothData.find("tangent data") != nullptr);
    const oo::PList* keys = smoothData.find("material keys");
    EXPECT_TRUE(keys != nullptr && keys->isArray() &&
             keys->getIf<oo::PList::Array>()->size() == 1);

    // Explicit normals: the normal mode is explicit, cached under the per-face
    // key.
    const std::shared_ptr<oo::Mesh> normals = Mesh("normals.dat");
    EXPECT_TRUE(normals != nullptr);
    if (normals == nullptr) return;
    EXPECT_TRUE(normals->description().find("normals: explicit}") !=
             std::string::npos);
    const oo::PList normalsData = CachedPList("normals.dat:0:1.000", "OOMesh");
    EXPECT_EQ(normalsData.get<unsigned int>("normal mode"), 2u);
    EXPECT_TRUE(normalsData.find("normal data") != nullptr);
    // A per-face mesh caches no normals.
    EXPECT_TRUE(CachedPList("tetra.dat:0:1.000", "OOMesh").find("normal data") ==
             nullptr);

    // Geometry: the bounding box relative to a position and basis.
    const Vector i = make_vector(1, 0, 0), j = make_vector(0, 1, 0),
                 k = make_vector(0, 0, 1);
    const BoundingBox relative = normals->findBoundingBoxRelativeToPosition(
        make_vector(0, 0, 0), i, j, k, make_vector(1, 2, 3), i, j, k);
    EXPECT_TRUE(SameVector(relative.min, make_vector(1, 2, 3)));
    EXPECT_TRUE(SameVector(relative.max, make_vector(11, 12, 13)));
    // Seen along -x from (5, 0, 0): x is flipped and offset.
    const BoundingBox flipped = normals->findBoundingBoxRelativeToPosition(
        make_vector(5, 0, 0), make_vector(-1, 0, 0), j, k, make_vector(0, 0, 0),
        i, j, k);
    EXPECT_TRUE(SameVector(flipped.min, make_vector(-5, 0, 0)));
    EXPECT_TRUE(SameVector(flipped.max, make_vector(5, 10, 10)));
    EXPECT_TRUE(MeshOctree(normals.get()) != nullptr);

    // Rendering: rebinding the materials keeps the placeholder.
    normals->rebindMaterials();
    EXPECT_TRUE(normals->hasOpaqueParts());
#ifndef NDEBUG
    EXPECT_TRUE(normals->allTextures().empty());
#endif
  }
}

TEST(OOMesh, facadeContract) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    oo::Mesh* cxxMesh = mesh.get();
    EXPECT_TRUE(cxxMesh != nullptr);
    if (cxxMesh == nullptr) return;

    // The C++ members' answers.
    EXPECT_TRUE(cxxMesh->hasOpaqueParts());
    EXPECT_TRUE(cxxMesh->descriptionComponents() ==
             std::optional<std::string>("\"tetra.dat\", 4 vertices, 4 faces, "
                                        "radius: 10 m normals: per-face"));
    EXPECT_TRUE(cxxMesh->copyWithZone(nullptr).get() ==
             cxxMesh);  // a copy is the mesh itself

    // Through the root's C++ pointer, the overrides answer (the bounding box is
    // geometry's).
    oo::Drawable* drawable = cxxMesh;
    EXPECT_TRUE(Near(drawable->collisionRadius(), 10.0));
    EXPECT_TRUE(Near(drawable->maxDrawDistance(), cxxMesh->maxDrawDistance()));
    EXPECT_TRUE(SameVector(drawable->boundingBox().max, make_vector(10, 10, 10)));
    EXPECT_TRUE(!drawable->hasTranslucentParts());

    // The C++ factory: a mesh; null for a missing model.
    const std::shared_ptr<oo::Mesh> made =
        oo::Mesh::meshWithName("tetra.dat", std::nullopt, oo::PList(),
                             oo::PList(), false, oo::PList(), nullptr, 2.0f, false);
    EXPECT_TRUE(made.get() != nullptr);
    if (made.get() == nullptr) return;
    EXPECT_TRUE(Near(made->collisionRadius(), 20.0));
    EXPECT_TRUE(made->description().starts_with("<OOMesh "));
    EXPECT_TRUE(oo::Mesh::meshWithName("no such model.dat", std::nullopt,
                                  oo::PList(), oo::PList(), false, oo::PList(),
                                  nullptr)
                 .get() == nullptr);

    // A C++ mesh made bare is the old -init's: no model yet.
    const std::shared_ptr<oo::Mesh> bare = oo::makeShared<oo::Mesh>();
    EXPECT_EQ(bare->modelName().value_or("<none>"), "No Model");
    EXPECT_EQ(bare->getVertexCount(), 0u);
  }
}

// The geometry members.
TEST(OOMesh, geometryMembers) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    oo::Mesh* cxxMesh = mesh.get();
    EXPECT_TRUE(cxxMesh != nullptr);
    if (cxxMesh == nullptr) return;

    const std::shared_ptr<Octree> octree = cxxMesh->getOctree();
    EXPECT_TRUE(octree.get() != nullptr);
    EXPECT_TRUE(cxxMesh->getOctree().get() == octree.get());  // made once
    EXPECT_TRUE(MeshOctree(mesh.get()) == octree.get());

    EXPECT_TRUE(SameVector(cxxMesh->boundingBox().max, make_vector(10, 10, 10)));
    const Vector i = make_vector(1, 0, 0), j = make_vector(0, 1, 0),
                 k = make_vector(0, 0, 1);
    const BoundingBox relative = cxxMesh->findBoundingBoxRelativeToPosition(
        make_vector(0, 0, 0), i, j, k, make_vector(1, 2, 3), i, j, k);
    EXPECT_TRUE(SameVector(relative.max, make_vector(11, 12, 13)));
    const BoundingBox moved = cxxMesh->findSubentityBoundingBoxWithPosition(
        make_vector(2, 2, 2), kIdentityMatrix);
    EXPECT_TRUE(SameVector(moved.min, make_vector(2, 2, 2)));
    EXPECT_TRUE(SameVector(moved.max, make_vector(12, 12, 12)));

    // Rescaled through the C++ member: a new mesh, with no name.
    const std::shared_ptr<oo::Mesh> rescaled = cxxMesh->meshRescaledBy(0.5f);
    EXPECT_TRUE(rescaled.get() != nullptr && rescaled.get() != cxxMesh);
    if (rescaled.get() == nullptr) return;
    EXPECT_TRUE(Near(rescaled->collisionRadius(), 5.0));
    EXPECT_TRUE(!rescaled->modelName().has_value());
  }
}

// Drawing on the hidden GL context, the display lists it makes, and a graphics
// reset between draws.
TEST(OOMesh, rendering) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    const std::shared_ptr<oo::Mesh> smooth = Mesh("tetra2.dat", 1.0f, true, true);
    EXPECT_TRUE(mesh != nullptr && smooth != nullptr);
    if (mesh == nullptr || smooth == nullptr) return;
    while (glGetError() != GL_NO_ERROR) {
    }

    mesh->renderOpaqueParts();  // makes the display lists
    mesh->renderOpaqueParts();
    smooth->renderOpaqueParts();
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));

    // A reset deletes the display lists and rebinds the materials; the next
    // draw remakes them.
    oo::GraphicsResetManager::sharedManager()->resetGraphicsState();
    mesh->renderOpaqueParts();
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(mesh->getVertexCount(), 4u);
    EXPECT_TRUE(Near(mesh->collisionRadius(), 10.0));

    // A mutable copy draws on its own lists.
    const std::shared_ptr<oo::Mesh> copy = mesh->mutableCopyWithZone(nullptr);
    copy->renderOpaqueParts();
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
  }
}

// The rendering members, through the C++ mesh.
TEST(OOMesh, renderingMembers) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh = Mesh("tetra.dat");
    oo::Mesh* cxxMesh = mesh.get();
    EXPECT_TRUE(cxxMesh != nullptr);
    if (cxxMesh == nullptr) return;
    while (glGetError() != GL_NO_ERROR) {
    }

    // Through the root's C++ pointer, as the entities draw.
    oo::Drawable* drawable = cxxMesh;
    drawable->renderOpaqueParts();
    EXPECT_TRUE(cxxMesh->listsReady);
    cxxMesh->deleteDisplayLists();
    EXPECT_TRUE(!cxxMesh->listsReady);
    drawable->renderOpaqueParts();
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));

    // The materials: the placeholder, rebound; a reset.
    cxxMesh->rebindMaterials();
    EXPECT_TRUE(cxxMesh->materials[0].get() == oo::Mesh::placeholderMaterial().get());
    cxxMesh->resetGraphicsState();
    EXPECT_TRUE(!cxxMesh->listsReady);
    EXPECT_TRUE(cxxMesh->materials[0].get() == oo::Mesh::placeholderMaterial().get());

    // The buffers: a renamed key, and bytes kept by key.
    cxxMesh->renameTexturesFrom("_oo_placeholder_material",
                                "_oo_placeholder_material");
    EXPECT_EQ(cxxMesh->materialKeys[0], "_oo_placeholder_material");
    void* bytes = cxxMesh->allocateBytesWithSize(4, 3, "test bytes");
    EXPECT_TRUE(bytes != nullptr);
    EXPECT_TRUE(cxxMesh->_retainedObjects.find("test bytes") !=
             cxxMesh->_retainedObjects.end());
  }
}

// Loading and copying are C++ members, and the mesh itself is the graphics
// reset client.
TEST(OOMesh, loadingMembers) {
  ::SetUp();
  {
    const std::shared_ptr<oo::Mesh> mesh =
        oo::Mesh::meshWithName("tetra.dat", std::nullopt, oo::PList(),
                             oo::PList(), false, oo::PList(), nullptr);
    EXPECT_TRUE(mesh.get() != nullptr);
    if (mesh.get() == nullptr) return;
    EXPECT_EQ(mesh->getVertexCount(), 4u);
    EXPECT_EQ(mesh->modelName().value_or("<none>"), "tetra.dat");

    // A mutable copy shares the buffers and is a new mesh.
    const std::shared_ptr<oo::Mesh> copy = mesh->mutableCopyWithZone(nullptr);
    EXPECT_TRUE(copy.get() != nullptr && copy.get() != mesh.get());
    if (copy.get() == nullptr) return;
    EXPECT_TRUE(copy->_vertices == mesh->_vertices);
    EXPECT_TRUE(copy->octree == mesh->octree);
    EXPECT_TRUE(!copy->listsReady);

    // Both are registered with no other owner for either: a reset deletes
    // their display lists.
    while (glGetError() != GL_NO_ERROR) {
    }
    mesh->renderOpaqueParts();
    copy->renderOpaqueParts();
    EXPECT_TRUE(mesh->listsReady && copy->listsReady);
    oo::GraphicsResetManager::sharedManager()->resetGraphicsState();
    EXPECT_TRUE(!mesh->listsReady && !copy->listsReady);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
  }
}

TEST(OOMesh, cleanUp) {
  stdfs::current_path(stdfs::temp_directory_path());
  std::error_code ignored;
  stdfs::remove_all(sRoot, ignored);
  EXPECT_TRUE(true);
}
