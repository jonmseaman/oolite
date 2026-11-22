/*	test_Octree.mm
        Unit tests for Octree and OOOctreeBuilder (src/Core/Octree.h).

        The tests pin what the builder builds (and folds), the cache
   representation and its round trip, the volume, the random point, line hits
   and octree-octree hits. Run: tests/run.sh unit
*/

#include <cmath>

#include <gtest/gtest.h>

#include "Octree.h"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

bool Same(Vector a, Vector b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

bool Near(float a, float b) { return std::fabs(a - b) < 1e-4f; }

const Triangle kIdentityIJK = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

// The node values of an octree, from its cache representation.
std::vector<int> Nodes(const oo::PList& representation) {
  const oo::Data* data = representation.find("octree")->getIf<oo::Data>();
  const int* ints = (const int*)data->bytes();
  return std::vector<int>(ints, ints + data->length() / sizeof(int));
}

}  // namespace

// An octree built by an OOOctreeBuilder.
#define BUILD(radius, ...)                                             \
  ([&]() {                                                             \
    std::shared_ptr<oo::OctreeBuilder> builder = oo::makeShared<oo::OctreeBuilder>(); \
    __VA_ARGS__;                                                       \
    return builder->buildOctreeWithRadius(radius);                     \
  }())

TEST(Octree, builderAndRepresentation) {
  {
    std::shared_ptr<Octree> solid = BUILD(2.0f, builder->writeSolid());
    EXPECT_TRUE(solid);
    oo::PList representation = solid->dictionaryRepresentation();
    EXPECT_TRUE(Nodes(representation) == std::vector<int>({-1}));
    EXPECT_TRUE(representation.get<float>("radius") == 2.0f);

    std::shared_ptr<Octree> empty = BUILD(2.0f, builder->writeEmpty());
    EXPECT_TRUE(Nodes(empty->dictionaryRepresentation()) == std::vector<int>({0}));

    // One inner node with its first child solid: the root holds the offset to
    // its children.
    std::shared_ptr<Octree> corner =
        BUILD(2.0f, builder->beginInnerNode(); builder->writeSolid();
              for (int i = 0; i < 7; i++) builder->writeEmpty();
              builder->endInnerNode());
    EXPECT_TRUE(Nodes(corner->dictionaryRepresentation()) ==
             std::vector<int>({1, -1, 0, 0, 0, 0, 0, 0, 0}));

    // Eight solid children fold into one solid node.
    std::shared_ptr<Octree> folded =
        BUILD(2.0f, builder->beginInnerNode();
              for (int i = 0; i < 8; i++) builder->writeSolid();
              builder->endInnerNode());
    EXPECT_TRUE(Nodes(folded->dictionaryRepresentation()) ==
             std::vector<int>({-1}));

    // The representation comes back as an equal octree.
    std::shared_ptr<Octree> copy =
        Octree::initWithDictionary(corner->dictionaryRepresentation());
    EXPECT_TRUE(copy && copy != corner);
    EXPECT_TRUE(Nodes(copy->dictionaryRepresentation()) ==
             Nodes(corner->dictionaryRepresentation()));
    EXPECT_TRUE(copy->volume() == corner->volume());
  }
}

TEST(Octree, invalidRepresentations) {
  {
    EXPECT_TRUE(Octree::initWithDictionary(oo::PList(oo::PList::Dict())).get() ==
             nullptr);  // no octree

    oo::PList::Dict ragged;
    ragged["octree"] =
        oo::PList(oo::Data("abc", 3));  // not a whole number of nodes
    ragged["radius"] = oo::PList(1.0);
    EXPECT_TRUE(Octree::initWithDictionary(oo::PList(std::move(ragged))).get() ==
             nullptr);

    oo::PList::Dict empty;
    empty["octree"] = oo::PList(oo::Data());  // no nodes at all
    empty["radius"] = oo::PList(1.0);
    EXPECT_TRUE(Octree::initWithDictionary(oo::PList(std::move(empty))).get() ==
             nullptr);
  }
}

TEST(Octree, volumeAndRandomPoint) {
  {
    EXPECT_TRUE(BUILD(2.0f, builder->writeSolid())->volume() == 8.0f);
    EXPECT_TRUE(BUILD(2.0f, builder->writeEmpty())->volume() == 0.0f);

    std::shared_ptr<Octree> corner =
        BUILD(2.0f, builder->beginInnerNode(); builder->writeSolid();
              for (int i = 0; i < 7; i++) builder->writeEmpty();
              builder->endInnerNode());
    EXPECT_TRUE(corner->volume() == 1.0f);
    EXPECT_TRUE(Same(corner->randomPoint(),
                  make_vector(1, 1, 1)));  // the one full octant's centre
    EXPECT_TRUE(corner->octreeScaledBy(2.0f)->volume() == 8.0f);
    EXPECT_TRUE(Same(corner->octreeScaledBy(2.0f)->randomPoint(),
                  make_vector(2, 2, 2)));
    EXPECT_TRUE(
        Same(BUILD(2.0f, builder->writeSolid())->randomPoint(), kZeroVector));
  }
}

TEST(Octree, lineHits) {
  {
    std::shared_ptr<Octree> solid = BUILD(2.0f, builder->writeSolid());
    EXPECT_TRUE(
        solid->isHitByLine(make_vector(-10, 0, 0), make_vector(10, 0, 0)) ==
        10.0f);  // solid root: distance to v0

    std::shared_ptr<Octree> empty = BUILD(2.0f, builder->writeEmpty());
    EXPECT_TRUE(empty->isHitByLine(make_vector(-10, 0, 0),
                                make_vector(10, 0, 0)) == 0.0f);

    std::shared_ptr<Octree> corner =
        BUILD(2.0f, builder->beginInnerNode(); builder->writeSolid();
              for (int i = 0; i < 7; i++) builder->writeEmpty();
              builder->endInnerNode());
    // (the line walk offsets octants
    // with the opposite sign to isHitByOctree(), so each of these lines finds
    // the solid octant)
    EXPECT_TRUE(corner->isHitByLine(make_vector(-1, -1, 10),
                                 make_vector(-1, -1, -10)) == 11.0f);
    EXPECT_TRUE(Near(
        corner->isHitByLine(make_vector(-1, 1, 10), make_vector(-1, 1, -10)),
        11.1803f));
    EXPECT_TRUE(
        Near(corner->isHitByLine(make_vector(1, 1, 10), make_vector(1, 1, -10)),
             11.3578f));
    EXPECT_TRUE(
        Near(corner->isHitByLine(make_vector(1, 1, 10), make_vector(1, 1, -10)),
             11.3578f));  // repeatable
    EXPECT_TRUE(
        corner->isHitByLine(make_vector(10, 10, 10), make_vector(20, 20, 20)) ==
        0.0f);  // misses the cube
  }
}

TEST(Octree, octreeHits) {
  {
    std::shared_ptr<Octree> a = BUILD(2.0f, builder->writeSolid());
    std::shared_ptr<Octree> b = BUILD(1.0f, builder->writeSolid());
    std::shared_ptr<Octree> empty = BUILD(1.0f, builder->writeEmpty());
    std::shared_ptr<Octree> corner =
        BUILD(2.0f, builder->beginInnerNode(); builder->writeSolid();
              for (int i = 0; i < 7; i++) builder->writeEmpty();
              builder->endInnerNode());

    EXPECT_TRUE(a->isHitByOctree(b.get(), make_vector(0, 0, 0), kIdentityIJK));
    EXPECT_TRUE(!a->isHitByOctree(b.get(), make_vector(10, 0, 0), kIdentityIJK));
    EXPECT_TRUE(
        !a->isHitByOctree(empty.get(), make_vector(0, 0, 0), kIdentityIJK));
    EXPECT_TRUE(!a->isHitByOctree(nullptr, make_vector(0, 0, 0), kIdentityIJK));

    // Child 0 is the low-coordinate octant.
    EXPECT_TRUE(corner->isHitByOctree(b.get(), make_vector(-1.5f, -1.5f, -1.5f),
                                   kIdentityIJK));
    EXPECT_TRUE(!corner->isHitByOctree(b.get(), make_vector(1.5f, 1.5f, 1.5f),
                                    kIdentityIJK));
    EXPECT_TRUE(!corner->isHitByOctree(b.get(), make_vector(-1.5f, -1.5f, 1.5f),
                                    kIdentityIJK));

    // Scaled: b at 3 units misses unscaled, hits once b is scaled up.
    EXPECT_TRUE(!a->isHitByOctree(b.get(), make_vector(3.5f, 0, 0), kIdentityIJK,
                               1.0f, 1.0f));
    EXPECT_TRUE(a->isHitByOctree(b.get(), make_vector(3.5f, 0, 0), kIdentityIJK,
                              1.0f, 2.0f));
    EXPECT_TRUE(!a->isHitByOctree(nullptr, make_vector(0, 0, 0), kIdentityIJK,
                               1.0f, 1.0f));
  }
}

TEST(Octree, cxxOctree) {
  std::shared_ptr<oo::OctreeBuilder> builder = oo::makeShared<oo::OctreeBuilder>();
  builder->beginInnerNode();
  builder->writeSolid();
  for (int i = 0; i < 7; i++) builder->writeEmpty();
  builder->endInnerNode();
  std::shared_ptr<Octree> corner = builder->buildOctreeWithRadius(2.0f);
  EXPECT_TRUE(corner.get() != nullptr);
  EXPECT_TRUE(corner->volume() == 1.0f);
  EXPECT_TRUE(Nodes(corner->dictionaryRepresentation()) ==
           std::vector<int>({1, -1, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_TRUE(corner->octreeScaledBy(2.0f)->volume() == 8.0f);

  std::shared_ptr<Octree> copy =
      Octree::initWithDictionary(corner->dictionaryRepresentation());
  EXPECT_TRUE(copy.get() != nullptr && copy.get() != corner.get() &&
           copy->volume() == 1.0f);
  EXPECT_TRUE(Octree::initWithDictionary(oo::PList(oo::PList::Dict())).get() ==
           nullptr);

  std::shared_ptr<oo::OctreeBuilder> small = oo::makeShared<oo::OctreeBuilder>();
  small->writeSolid();
  std::shared_ptr<Octree> b = small->buildOctreeWithRadius(1.0f);
  EXPECT_TRUE(corner->isHitByOctree(b.get(), make_vector(-1.5f, -1.5f, -1.5f),
                                 kIdentityIJK));
  EXPECT_TRUE(!corner->isHitByOctree(b.get(), make_vector(1.5f, 1.5f, 1.5f),
                                  kIdentityIJK));
  EXPECT_TRUE(!corner->isHitByOctree(nullptr, kZeroVector, kIdentityIJK));
  EXPECT_TRUE(
      !corner->isHitByOctree(nullptr, kZeroVector, kIdentityIJK, 1.0f, 1.0f));
  EXPECT_TRUE(corner->isHitByLine(make_vector(-1, -1, 10),
                               make_vector(-1, -1, -10)) == 11.0f);
}
