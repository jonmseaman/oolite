/*	test_OOMeshToOctreeConverter.mm
        Unit tests for OOMeshToOctreeConverter
   (src/Core/OOMeshToOctreeConverter.h).

        It links the whole game but main (tests/unit/core/meson.build entry
   ['*']), because the octrees it builds are Octree objects. The expectations
   cover: what octree a set of triangles gives (nodes, radius and volume) at
   several depths, that degenerate triangles are dropped, the description, and
   that the triangle store grows past its sixteen inline slots. Run: bash
   tests/run.sh unit
*/

#include <cmath>

#include <gtest/gtest.h>

#include "MeshToOctreeConverter.h"
#include "Octree.h"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

Triangle Tri(Vector a, Vector b, Vector c) {
  Triangle t;
  t.v[0] = a;
  t.v[1] = b;
  t.v[2] = c;
  return t;
}

// The twelve triangles of a cube centred on the origin.
void AddCube(oo::MeshToOctreeConverter& converter, float h) {
  Vector p[8];
  for (int i = 0; i < 8; i++)
    p[i] = make_vector((i & 4) ? h : -h, (i & 2) ? h : -h, (i & 1) ? h : -h);
  const int faces[6][4] = {{0, 1, 3, 2}, {4, 6, 7, 5}, {0, 4, 5, 1},
                           {2, 3, 7, 6}, {0, 2, 6, 4}, {1, 5, 7, 3}};
  for (const auto& f : faces) {
    converter.addTriangle(Tri(p[f[0]], p[f[1]], p[f[2]]));
    converter.addTriangle(Tri(p[f[0]], p[f[2]], p[f[3]]));
  }
}

std::vector<int> Nodes(const std::shared_ptr<Octree>& octree) {
  oo::PList representation = octree->dictionaryRepresentation();
  const oo::Data* data = representation.find("octree")->getIf<oo::Data>();
  const int* ints = (const int*)data->bytes();
  return std::vector<int>(ints, ints + data->length() / sizeof(int));
}

float Radius(const std::shared_ptr<Octree>& octree) {
  return octree->dictionaryRepresentation().get<float>("radius");
}

// The octree at a depth (Nodes and Radius read its representation).
std::shared_ptr<Octree> At(const std::shared_ptr<oo::MeshToOctreeConverter>& converter,
                   std::size_t depth) {
  return converter->findOctreeToDepth(depth);
}

std::optional<std::string> Description(
    const std::shared_ptr<oo::MeshToOctreeConverter>& converter) {
  return converter->descriptionComponents();
}

bool Near(float a, float b) {
  return std::fabs(a - b) < 1e-3f * std::fmax(1.0f, std::fabs(b));
}

std::shared_ptr<oo::MeshToOctreeConverter> OneTriangle() {
  std::shared_ptr<oo::MeshToOctreeConverter> one =
      oo::MeshToOctreeConverter::converterWithCapacity(1);
  one->addTriangle(
      Tri(make_vector(0, 0, 0), make_vector(4, 0, 0), make_vector(0, 4, 0)));
  one->addTriangle(Tri(make_vector(1, 1, 1), make_vector(1, 1, 1),
                       make_vector(2, 2, 2)));  // degenerate: dropped
  return one;
}

}  // namespace

TEST(OOMeshToOctreeConverter, emptyGivesAnEmptyOctree) {
  {
    std::shared_ptr<oo::MeshToOctreeConverter> empty =
        oo::MeshToOctreeConverter::converterWithCapacity(0);
    std::shared_ptr<Octree> octree = At(empty, 3);
    EXPECT_TRUE(Nodes(octree) == std::vector<int>({0}));
    EXPECT_TRUE(Radius(octree) == 0.5f);  // the half-metre pad
    EXPECT_TRUE(octree->volume() == 0.0f);
    EXPECT_TRUE(Description(empty) == std::optional<std::string>("0 triangles"));
  }
}

TEST(OOMeshToOctreeConverter, oneTriangle) {
  {
    std::shared_ptr<oo::MeshToOctreeConverter> one = OneTriangle();
    EXPECT_TRUE(Description(one) == std::optional<std::string>("1 triangles"));

    std::shared_ptr<Octree> d0 = At(one, 0);
    EXPECT_TRUE(Nodes(d0) == std::vector<int>({-1}));
    EXPECT_TRUE(Radius(d0) == 4.5f);
    EXPECT_TRUE(Near(d0->volume(), 91.125f));

    EXPECT_TRUE(Nodes(At(one, 1)) ==
             std::vector<int>({1, 0, 0, 0, 0, 0, 0, 0, -1}));
    EXPECT_TRUE(Near(At(one, 1)->volume(), 11.3906f));
    EXPECT_TRUE(Nodes(At(one, 2)) ==
             std::vector<int>(
                 {1, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, -1, 0, -1, 0, 0, 0}));
    EXPECT_TRUE(Near(At(one, 2)->volume(), 4.27148f));
    EXPECT_TRUE(Nodes(At(one, 3)) ==
             std::vector<int>({1, 0,  0, 0,  0, 0,  0, 0,  1, 8,  0, 14, 0, 20,
                               0, 0,  0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1,
                               0, -1, 0, 0,  0, -1, 0, -1, 0, -1, 0, 0,  0}));
    EXPECT_TRUE(Near(At(one, 3)->volume(), 1.77979f));
  }
}

TEST(OOMeshToOctreeConverter, cubeGrowsPastInlineStorage) {
  {
    std::shared_ptr<oo::MeshToOctreeConverter> cube =
        oo::makeShared<oo::MeshToOctreeConverter>(12);
    AddCube(*cube, 10);
    AddCube(*cube, 5);  // 24 triangles: past the sixteen inline slots
    EXPECT_TRUE(Description(cube) == std::optional<std::string>("24 triangles"));

    for (unsigned d = 0; d <= 2; d++) {
      std::shared_ptr<Octree> octree = At(cube, d);
      EXPECT_TRUE(Nodes(octree) == std::vector<int>({-1}) &&
               Radius(octree) == 10.5f);
    }
    std::shared_ptr<Octree> d3 = At(cube, 3);
    std::vector<int> nodes = Nodes(d3);
    EXPECT_TRUE(nodes.size() == 585);
    EXPECT_TRUE(std::vector<int>(nodes.begin(), nodes.begin() + 9) ==
             std::vector<int>({1, 8, 79, 150, 221, 292, 363, 434, 505}));
    EXPECT_TRUE(Near(d3->volume(), 795.867f));
    EXPECT_TRUE(Nodes(At(cube, 4)).size() == 3401);
    EXPECT_TRUE(Near(At(cube, 4)->volume(), 465.763f));
  }
}

TEST(OOMeshToOctreeConverter, cxxConverter) {
  std::shared_ptr<oo::MeshToOctreeConverter> one =
      oo::MeshToOctreeConverter::converterWithCapacity(1);
  one->addTriangle(
      Tri(make_vector(0, 0, 0), make_vector(4, 0, 0), make_vector(0, 4, 0)));
  one->addTriangle(Tri(make_vector(1, 1, 1), make_vector(1, 1, 1),
                       make_vector(2, 2, 2)));  // degenerate: dropped
  EXPECT_TRUE(one->descriptionComponents() ==
           std::optional<std::string>("1 triangles"));
  std::shared_ptr<Octree> octree = one->findOctreeToDepth(1);
  EXPECT_TRUE(octree.get() != nullptr && Near(octree->volume(), 11.3906f));
  EXPECT_TRUE(Nodes(octree) == std::vector<int>({1, 0, 0, 0, 0, 0, 0, 0, -1}));

  std::shared_ptr<oo::MeshToOctreeConverter> empty =
      oo::makeShared<oo::MeshToOctreeConverter>(0);
  EXPECT_TRUE(empty->findOctreeToDepth(2)->volume() == 0.0f);
}
