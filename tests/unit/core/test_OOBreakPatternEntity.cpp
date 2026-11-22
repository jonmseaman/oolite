/*	test_OOBreakPatternEntity.mm
        Unit tests for OOBreakPatternEntity
   (src/Core/Entities/OOBreakPatternEntity.h), one ring of the hyperspace and
   docking tunnel.

        Its object needs the game graph, so the test links the whole game but
   main (['*']) and uses a Universe that was never initialised, of a test
   subclass that records what is removed, and a plain entity as PLAYER. The
   tests cover the polygon (sides clamped to 3...128, the
   aspect ratio, the closing pair of vertices), the default and the set inner
   and outer colours, the status, scan class and break-pattern flags, and
   removal once its lifetime has run out at the ring speed. Its vertex arrays
   are private; the test reads them through the access struct below. Run: bash
   tests/run.sh unit
*/

#include <cmath>

#include <gtest/gtest.h>

#include "BreakPatternEntity.h"
#include "Color.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "EntityManualRefs.h"  // the tests' manual entity references

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity* gOOPlayer;
extern Universe* gSharedUniverse;

class TestPlayer : public PlayerEntity {
 public:
  HPVector viewpointPosition() override { return kZeroHPVector; }
};

// A C++ player under the ship's facade, as
// PlayerEntity::sharedPlayer() makes the game's, retained (+1) as +alloc's
// object was.
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

static Entity* sRemoved = nullptr;

// UNIVERSE: never initialised; records what is removed.
class TestUniverse : public Universe {
 public:
  bool removeEntity(Entity* entity) override {
    sRemoved = entity;
    return true;
  }
};

// --- The private vertex arrays, and nothing else
// ---------------------------------------------------

struct OOBreakPatternEntityTestAccess {
  static std::size_t VertexCount(const std::shared_ptr<oo::BreakPatternEntity>& r) {
    oo::BreakPatternEntity* e = r.get();
    return e->_vertexCount;
  }
  static Vector VertexPosition(const std::shared_ptr<oo::BreakPatternEntity>& r,
                               std::size_t i) {
    oo::BreakPatternEntity* e = r.get();
    return e->_vertexPosition[i];
  }
  static const GLfloat* VertexColor(const std::shared_ptr<oo::BreakPatternEntity>& r,
                                    std::size_t i) {
    oo::BreakPatternEntity* e = r.get();
    return e->_vertexColor[i];
  }
};

// --------------------------------------------------------------------------------------------------

namespace {

using Access = OOBreakPatternEntityTestAccess;

void SetUp() {
  static Universe* universe = nullptr;
  if (universe == nullptr) {
    universe = new TestUniverse();  // never released
  }
  gSharedUniverse = universe;
  static TestPlayer* player = nullptr;
  if (player == nullptr) player = NewTestPlayer<TestPlayer>();
  gOOPlayer = player;
  sRemoved = nullptr;
}

bool Near(float a, float b) { return fabs(a - b) < 1e-4; }

bool ColorIs(const std::shared_ptr<oo::BreakPatternEntity>& e, std::size_t i, GLfloat r,
             GLfloat g, GLfloat b, GLfloat a) {
  const GLfloat* c = Access::VertexColor(e, i);
  return c[0] == r && c[1] == g && c[2] == b && c[3] == a;
}

}  // namespace

TEST(OOBreakPatternEntity, polygon) {
  {
    ::SetUp();
    std::shared_ptr<oo::BreakPatternEntity> ring =
        oo::BreakPatternEntity::breakPatternWithPolygonSides(4, 0.0f, 2.0f);
    EXPECT_TRUE(ring != nullptr);
    EXPECT_TRUE(Access::VertexCount(ring) == 10);

    // The first side starts at the top; the aspect ratio narrows the other axis
    // (here y).
    Vector v0 = Access::VertexPosition(ring, 0),
           v1 = Access::VertexPosition(ring, 1);
    EXPECT_TRUE(Near(v0.x, 0) && Near(v0.y, 25) && v0.z == -40);
    EXPECT_TRUE(Near(v1.x, 0) && Near(v1.y, 20) && v1.z == 0);
    Vector v2 = Access::VertexPosition(ring, 2);
    EXPECT_TRUE(Near(v2.x, 50) && Near(v2.y, 0) && v2.z == -40);
    // The strip closes on the first pair.
    EXPECT_TRUE(vector_equal(Access::VertexPosition(ring, 8), v0) &&
             vector_equal(Access::VertexPosition(ring, 9), v1));

    // Sides are clamped to 3...128.
    EXPECT_TRUE(
        Access::VertexCount(oo::BreakPatternEntity::breakPatternWithPolygonSides(
            1, 0.0f, 1.0f)) == 8);
    EXPECT_TRUE(
        Access::VertexCount(oo::BreakPatternEntity::breakPatternWithPolygonSides(
            1000, 0.0f, 1.0f)) == oo::kBreakPatternMaxVertices);
  }
}

TEST(OOBreakPatternEntity, flagsAndColors) {
  {
    ::SetUp();
    std::shared_ptr<oo::BreakPatternEntity> ring =
        oo::BreakPatternEntity::breakPatternWithPolygonSides(3, 45.0f, 1.0f);
    EXPECT_TRUE(ring->status() == oo::EntityStatus::kEffect &&
             ring->scanClass == oo::ScanClass::kNoDraw);
    EXPECT_TRUE(ring->isImmuneToBreakPatternHide && !ring->canCollide());
    std::shared_ptr<Entity> other = oo::makeShared<Entity>();
    EXPECT_TRUE(dynamic_cast<oo::BreakPatternEntity*>(ring.get()) != nullptr &&
             dynamic_cast<oo::BreakPatternEntity*>(other.get()) == nullptr);

    // Default: red inside, blue outside.
    EXPECT_TRUE(ColorIs(ring, 0, 1.0f, 0.0f, 0.0f, 0.5f) &&
             ColorIs(ring, 1, 0.0f, 0.0f, 1.0f, 0.25f));
    EXPECT_TRUE(ColorIs(ring, 7, 0.0f, 0.0f, 1.0f, 0.25f));

    ring->setInnerColor(oo::Color::colorWithRed(0.0f, 1.0f, 0.0f, 1.0f).get(),
                        oo::Color::colorWithRed(1.0f, 1.0f, 1.0f, 0.5f).get());
    EXPECT_TRUE(ColorIs(ring, 0, 0.0f, 1.0f, 0.0f, 1.0f) &&
             ColorIs(ring, 1, 1.0f, 1.0f, 1.0f, 0.5f));
    EXPECT_TRUE(ColorIs(ring, 6, 0.0f, 1.0f, 0.0f, 1.0f) &&
             ColorIs(ring, 7, 1.0f, 1.0f, 1.0f, 0.5f));

    // Hidden (no longer immune): nothing is drawn.
    ring->isImmuneToBreakPatternHide = false;
    ring->drawImmediate(false, true);
  }
}

TEST(OOBreakPatternEntity, lifetime) {
  {
    ::SetUp();
    std::shared_ptr<oo::BreakPatternEntity> ring =
        oo::BreakPatternEntity::breakPatternWithPolygonSides(4, 0.0f, 1.0f);
    Entity* facade = oo::MadeEntity(ring);  // what removeEntity: is sent
    ring->setStatus(oo::EntityStatus::kEffect);
    ring->setVelocity(make_vector(0, 0, 10));
    ring->setLifetime(100.0);

    // The lifetime runs down at the ring speed (200 a second); the ring moves
    // meanwhile.
    ring->update(0.25);
    EXPECT_TRUE(sRemoved == nullptr &&
             HPvector_equal(ring->getPosition(), make_HPvector(0, 0, 2.5)));
    ring->update(0.25);
    EXPECT_TRUE(sRemoved == nullptr);
    ring->update(0.01);
    EXPECT_TRUE(sRemoved == facade);
  }
}
