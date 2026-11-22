/*	test_OOFlashEffectEntity.mm
        Unit tests for OOFlashEffectEntity
   (src/Core/Entities/OOFlashEffectEntity.h), the white flash of an explosion
   and the coloured flash where a laser hits.

        The entity reads Universe and PLAYER, so the test links the whole game
   but main (['*']) and uses a Universe that was never initialised, subclassed
   to answer the time the test sets and to record -removeEntity:, and a plain
   entity as PLAYER. The tests cover: an explosion flash sits where its entity
   is, moves with it, starts at its collision radius, is white and grows at 150
   times its size a second (at least 600) to half opacity; a laser flash starts
   at diameter 1 in the given colour and grows at 150 a second to full opacity;
   both fade in over two thirds of their life (0.4 and 0.3 seconds) and out over
   the rest, and remove themselves when their time is up. The diameter and
   colour components, ivars of OOLightParticleEntity, are read through the one
   block of helpers below. Its texture is loaded once, by name (the loader
   replaced as in test_OOParticleSystem). The flashes are made by the C++
   factories the callers call and wrapped as the callers do: the class's own
   selectors are member calls, the root's are still sent to the object, and the
   last test pins that the object is a C++ entity's (the root Entity's). Run:
   bash tools/check-core-tests.sh
*/

#include <cmath>
#include <optional>
#include <string>

#include <gtest/gtest.h>

#include "Color.h"
#include "FlashEffectEntity.h"
#include "Texture.h"
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

// UNIVERSE: never initialised; answers the test's time and records what is
// removed.
class TestUniverse : public Universe {
 public:
  oo::TimeAbsolute& _time = universal_time;  // the universe's own state, which
                                           // its getter answers
  Entity* _removed = {};

  bool removeEntity(Entity* entity) override {
    _removed = entity;
    return true;
  }
};

namespace {

TestUniverse* sUniverse = nullptr;

// The texture loader's stand-in (as in test_OOParticleSystem).
std::optional<std::string> sTextureName;
oo::Texture* sTexture = nullptr;  // never released

// OOTexture::nameFactory, the test-only loader hook.
std::shared_ptr<oo::Texture> LoadTexture(const std::optional<std::string>& name,
                               const std::optional<std::string>&,
                               oo::TextureFlags, GLfloat, GLfloat) {
  sTextureName = name;
  return oo::share(sTexture);
}

void SetUp(oo::TimeAbsolute time) {
  if (sUniverse == nullptr) {
    sUniverse = new TestUniverse();                 // never released
    sTexture = oo::test::NewRetained<oo::Texture>();  // never released
    oo::Texture::nameFactory = &LoadTexture;
  }
  sUniverse->_time = time;
  sUniverse->_removed = nullptr;
  gSharedUniverse = sUniverse;
  static TestPlayer* player = nullptr;
  if (player == nullptr) player = NewTestPlayer<TestPlayer>();
  gOOPlayer = player;
}

// --- Ivars of OOLightParticleEntity the test reads, and nothing else
// ------------------------------

const GLfloat* ColorComponents(Entity* e) {
  return static_cast<oo::LightParticleEntity*>(e)->_colorComponents;
}
float Diameter(Entity* e) {
  return static_cast<oo::LightParticleEntity*>(e)->_diameter;
}

// --------------------------------------------------------------------------------------------------

bool Near(double a, double b) { return std::fabs(a - b) < 1e-4; }

// The flash's object, as the ship and the universe hand it to Objective-C.
Entity* FlashObject(const std::shared_ptr<oo::FlashEffectEntity>& flash) {
  return oo::MadeEntity(flash);
}
oo::FlashEffectEntity* Part(Entity* flash) {
  return dynamic_cast<oo::FlashEffectEntity*>(flash);
}

Entity* Exploding() {
  Entity* e = oo::MadeEntity(oo::makeShared<Entity>());
  e->setPosition(make_HPvector(10, 20, 30));
  e->setVelocity(make_vector(1, 2, 3));
  e->setCollisionRadius(2.0f);
  return e;
}

}  // namespace

TEST(OOFlashEffectEntity, explosionFlash) {
  {
    ::SetUp(5.0);
    Entity* flash =
        FlashObject(oo::FlashEffectEntity::explosionFlashFromEntity(Exploding()));
    EXPECT_TRUE(flash != nullptr && Part(flash) != nullptr);
    EXPECT_TRUE(HPvector_equal(flash->getPosition(), make_HPvector(10, 20, 30)));
    Vector v = flash->getVelocity();
    EXPECT_TRUE(v.x == 1 && v.y == 2 && v.z == 3);
    EXPECT_TRUE(Part(flash)->diameter() == 2.0f && Diameter(flash) == 2.0f);
    EXPECT_TRUE(flash->collisionRadius() == 0.0f && flash->getEnergy() == 0.0f);
    EXPECT_TRUE(flash->status() == oo::EntityStatus::kEffect &&
             flash->getScanClass() == oo::ScanClass::kNoDraw && flash->isEffect() &&
             !flash->canCollide());
    const GLfloat* c = ColorComponents(flash);
    EXPECT_TRUE(c[0] == 1.0f && c[1] == 1.0f && c[2] == 1.0f && c[3] == 1.0f);

    // Growth max(150 * 2, 600) = 600 a second; alpha 0.5, duration 0.4: fade in
    // for 0.2668 s.
    sUniverse->_time = 5.1;
    flash->update(0.1);
    EXPECT_TRUE(Near(Diameter(flash), 62.0));
    EXPECT_TRUE(Near(c[3], 0.5 * 0.1 / (0.4 * 0.667)));
    EXPECT_TRUE(sUniverse->_removed == nullptr);

    sUniverse->_time = 5.35;
    flash->update(0.25);
    EXPECT_TRUE(Near(Diameter(flash), 212.0));
    EXPECT_TRUE(Near(c[3], 0.5 * (0.4 - 0.35) / (0.4 - 0.4 * 0.667)));
    EXPECT_TRUE(sUniverse->_removed == nullptr);

    sUniverse->_time = 5.41;
    flash->update(0.06);
    EXPECT_TRUE(sUniverse->_removed == flash);
  }
}

TEST(OOFlashEffectEntity, bigExplosionGrowsFaster) {
  {
    ::SetUp(0.0);
    Entity* big = Exploding();
    big->setCollisionRadius(10.0f);
    Entity* flash =
        FlashObject(oo::FlashEffectEntity::explosionFlashFromEntity(big));
    EXPECT_TRUE(Part(flash)->diameter() == 10.0f);
    sUniverse->_time = 0.1;
    flash->update(0.1);
    EXPECT_TRUE(Near(Diameter(flash), 10.0 + 0.1 * 1500.0));  // 150 * 10 > 600
  }
}

TEST(OOFlashEffectEntity, laserFlash) {
  {
    ::SetUp(1.0);
    Entity* flash = FlashObject(oo::FlashEffectEntity::laserFlashWithPosition(
        make_HPvector(-1, -2, -3), make_vector(4, 5, 6),
        oo::Color::colorWithRed(1.0f, 0.5f, 0.25f, 0.125f).get()));
    EXPECT_TRUE(flash != nullptr && Part(flash) != nullptr);
    EXPECT_TRUE(HPvector_equal(flash->getPosition(), make_HPvector(-1, -2, -3)));
    Vector v = flash->getVelocity();
    EXPECT_TRUE(v.x == 4 && v.y == 5 && v.z == 6);
    EXPECT_TRUE(Part(flash)->diameter() == 1.0f);
    const GLfloat* c = ColorComponents(flash);
    EXPECT_TRUE(c[0] == 1.0f && c[1] == 0.5f && c[2] == 0.25f &&
             c[3] == 1.0f);  // opaque

    // Growth 150 a second; alpha 1, duration 0.3.
    sUniverse->_time = 1.1;
    flash->update(0.1);
    EXPECT_TRUE(Near(Diameter(flash), 16.0));
    EXPECT_TRUE(Near(c[3], 0.1 / (0.3 * 0.667)));

    sUniverse->_time = 1.25;
    flash->update(0.15);
    EXPECT_TRUE(Near(c[3], (0.3 - 0.25) / (0.3 - 0.3 * 0.667)));
    EXPECT_TRUE(sUniverse->_removed == nullptr);

    sUniverse->_time = 1.31;
    flash->update(0.06);
    EXPECT_TRUE(sUniverse->_removed == flash);
  }
}

TEST(OOFlashEffectEntity, nilColourKeepsWhite) {
  {
    ::SetUp(0.0);
    Entity* flash = FlashObject(oo::FlashEffectEntity::laserFlashWithPosition(
        kZeroHPVector, kZeroVector, nullptr));
    const GLfloat* c = ColorComponents(flash);
    EXPECT_TRUE(c[0] == 1.0f && c[1] == 1.0f && c[2] == 1.0f && c[3] == 1.0f);
  }
}

TEST(OOFlashEffectEntity, texture) {
  {
    ::SetUp(0.0);
    Entity* flash =
        FlashObject(oo::FlashEffectEntity::explosionFlashFromEntity(Exploding()));
    sTextureName.reset();
    EXPECT_TRUE(Part(flash)->texture() == sTexture);
    EXPECT_TRUE(sTextureName ==
             std::optional<std::string>("oolite-particle-flash.png"));
    sTextureName.reset();
    EXPECT_TRUE(Part(flash)->texture() == sTexture &&
             !sTextureName.has_value());  // loaded once
    oo::FlashEffectEntity::setUpTexture();
    EXPECT_TRUE(!sTextureName.has_value());
  }
}

TEST(OOFlashEffectEntity, facade) {
  {
    ::SetUp(0.0);
    Entity* flash =
        FlashObject(oo::FlashEffectEntity::explosionFlashFromEntity(Exploding()));
    // A C++ entity.
    EXPECT_TRUE(dynamic_cast<oo::FlashEffectEntity*>(flash) != nullptr);
    EXPECT_TRUE(flash == flash);
  }
}
