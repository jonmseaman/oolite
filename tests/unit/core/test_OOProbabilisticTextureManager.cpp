/*	test_OOProbabilisticTextureManager.mm
        Unit tests for OOProbabilisticTextureManager
   (src/Core/OOProbabilisticTextureManager.h), C++.

        The manager reads a list of textures from a Config plist (a name, or a
   dictionary with a texture name, a probability and a galaxy), loads each with
   the options it was given, and then picks one at random, weighted, from those
   allowed in the player's galaxy, with its own seed. The test pins which
   entries it keeps (no name, probability 0, a texture that does not load and an
   entry of another type are dropped; a galaxy that is not a string makes the
   entry galaxy-wide), what it asks the resource manager and the texture class,
   its description, the seed (copied from the global one unless given, advanced
   by each pick), the sequence of picks for two seeds in two galaxies (including
   the "galaxy list requirements not met" and "last texture" fallbacks), that it
   fails (null) when nothing loads, and that it retains its textures until it
   goes. The game around it is replaced the resource manager answers the
   configuration below, OOTexture is a stand-in that records how it was asked
   and counts -ensureFinishedLoading, and the player answers a galaxy. The
   expectations are written against the C++ calls. Only the description's
   "<OOProbabilisticTextureManager 0x...>" wrapper, which OOObject's
   -description added, went with that API: the components are checked as they
   were. Run: tests/run.sh unit
*/

#include <cstdio>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "ProbabilisticTextureManager.h"
#include "Texture.h"
#include "Types.h"
#include "oofnd/String.hpp"

// --- The game around the class
// ------------------------------------------------------------------

static oo::PList gConfig;
static std::vector<std::string> gLog;

#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
oo::PList ResourceManager::arrayFromFilesNamed(
    const std::string& fileName, const std::optional<std::string>& folderName,
    bool mergeFiles) {
  gLog.push_back("config " + fileName + " in " + folderName.value_or("(nil)") +
                 (mergeFiles ? " merged" : " unmerged"));
  return gConfig;
}

// Every texture loads, but for "missing.png". The stand-in is C++: the factory the manager
// calls, the root's own members (a texture needs them), and TestTexture, which the factory answers.
class TestTexture final : public oo::Texture {
 public:
  void ensureFinishedLoading() override { _loaded++; }

  std::string _name;
  unsigned _loaded = 0;
};

std::shared_ptr<oo::Texture> oo::Texture::textureWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& directory, oo::TextureFlags options,
    GLfloat anisotropy, GLfloat lodBias) {
  char args[128];
  std::snprintf(args, sizeof args, " options=%#x anisotropy=%g lodBias=%g",
                options, anisotropy, lodBias);
  gLog.push_back("texture " + name.value_or("(nil)") + " in " +
                 directory.value_or("(nil)") + args);
  if (name == "missing.png") return nullptr;
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_name = *name;
  return texture;
}

oo::Texture::Texture() {}
namespace oo {

Texture::~Texture() {}

}  // namespace oo

void oo::Texture::apply() {}
void oo::Texture::ensureFinishedLoading() {}
bool oo::Texture::isFinishedLoading() { return true; }
std::optional<std::string> oo::Texture::cacheKey() { return std::nullopt; }
NSSize oo::Texture::dimensions() { return NSSize{}; }
NSSize oo::Texture::originalDimensions() { return dimensions(); }
bool oo::Texture::isMipMapped() { return false; }
oo::PixMap oo::Texture::copyPixMapRepresentation() { return oo::PixMap{}; }
bool oo::Texture::isRectangleTexture() { return false; }
bool oo::Texture::isCubeMap() { return false; }
NSSize oo::Texture::texCoordsScale() { return NSSize{1.0, 1.0}; }
GLint oo::Texture::glTextureName() { return 0; }
#ifndef NDEBUG
std::optional<std::string> oo::Texture::name() { return std::nullopt; }
#endif
std::optional<std::string> oo::Texture::descriptionComponents() const {
  return std::nullopt;
}
std::optional<std::string> oo::Texture::shortDescriptionComponents() const {
  return std::nullopt;
}
std::string oo::Texture::description() const {
  return oo::str::format("<OOTexture %s>",
                         oo::str::pointerDescription(this).c_str());
}
std::string oo::Texture::shortDescription() const { return description(); }
std::string oo::Texture::className() const { return "OOTexture"; }
void oo::Texture::forceRebind() {}

static oo::GalaxyID gGalaxy = 0;

// PLAYER: the members the code under test calls, declared as PlayerEntity.h
// declares them (the test imports no game header that defines the class), with
// the same answers.
class PlayerEntity {
 public:
  oo::GalaxyID currentGalaxyID();
};

oo::GalaxyID PlayerEntity::currentGalaxyID() { return gGalaxy; }

PlayerEntity* gOOPlayer = nullptr;

// --- Helpers
// ------------------------------------------------------------------------------------

namespace {

oo::PList Dict(
    std::initializer_list<std::pair<const char*, oo::PList>> entries) {
  oo::PList::Dict dict;
  for (const auto& [key, value] : entries) dict[key] = value;
  return oo::PList(std::move(dict));
}

// a and h everywhere, b three times as likely, g (four) only in galaxy 2; the
// rest are dropped.
oo::PList Config() {
  return oo::PList(oo::PList::Array{
      oo::PList("a.png"),
      Dict({{"texture", oo::PList("b.png")}, {"probability", oo::PList(3)}}),
      Dict({{"texture", oo::PList("c.png")}, {"probability", oo::PList(0)}}),
      Dict({{"texture", oo::PList("missing.png")}}),
      Dict({{"probability", oo::PList(2)}}),
      oo::PList(42),
      Dict({{"texture", oo::PList("g.png")},
            {"probability", oo::PList(4)},
            {"galaxy", oo::PList("2")}}),
      Dict({{"texture", oo::PList("h.png")}, {"galaxy", oo::PList(3)}}),
  });
}

void Reset(const oo::PList& config) {
  if (gOOPlayer == nullptr) gOOPlayer = new PlayerEntity;  // never deleted
  gConfig = config;
  gLog.clear();
  gGalaxy = 0;
}

std::string Picks(const std::shared_ptr<oo::ProbabilisticTextureManager>& manager,
                  int count) {
  std::string picks;
  for (int i = 0; i < count; i++) {
    TestTexture* texture = static_cast<TestTexture*>(manager->selectTexture());
    picks += (texture != nullptr ? texture->_name.substr(0, 1) : std::string("-"));
  }
  return picks;
}

bool SameSeed(RANROTSeed a, RANROTSeed b) {
  return a.high == b.high && a.low == b.low;
}

std::shared_ptr<oo::ProbabilisticTextureManager> Manager(RANROTSeed seed) {
  return oo::ProbabilisticTextureManager::createWithPListName("test.plist", static_cast<oo::TextureFlags>(0x803),
                                                            0.5f, -0.25f, seed);
}

bool CheckPicks(const char* what, const std::string& picks,
                const char* expected) {
  if (picks == expected) return true;
  std::fprintf(stderr, "%s: picked %s, expected %s\n", what, picks.c_str(),
               expected);
  return false;
}

}  // namespace

// --- Tests
// --------------------------------------------------------------------------------------

TEST(OOProbabilisticTextureManager, entriesKeptAndHowTheyAreLoaded) {
  {
    Reset(Config());
    std::shared_ptr<oo::ProbabilisticTextureManager> manager = Manager(MakeRanrotSeed(1));
    EXPECT_TRUE(manager != nullptr);
    EXPECT_TRUE(manager->textureCount() == 4);
    const std::vector<std::string> expected = {
        "config test.plist in Config merged",
        "texture a.png in Textures options=0x803 anisotropy=0.5 lodBias=-0.25",
        "texture b.png in Textures options=0x803 anisotropy=0.5 lodBias=-0.25",
        "texture missing.png in Textures options=0x803 anisotropy=0.5 "
        "lodBias=-0.25",
        "texture g.png in Textures options=0x803 anisotropy=0.5 lodBias=-0.25",
        "texture h.png in Textures options=0x803 anisotropy=0.5 lodBias=-0.25",
    };
    EXPECT_TRUE(gLog == expected);
    if (gLog != expected)
      for (const std::string& line : gLog)
        std::fprintf(stderr, "  %s\n", line.c_str());

    // The cumulative probability of the galaxy-wide entries: a 1, b 3, h 1.
    EXPECT_TRUE(
        manager->descriptionComponents() ==
        std::optional<std::string>("4 textures, cumulative probability=5"));
  }
}

TEST(OOProbabilisticTextureManager, nothingLoadedIsNil) {
  {
    Reset(oo::PList(oo::PList::Array{}));
    EXPECT_TRUE(Manager(MakeRanrotSeed(1)) == nullptr);
    Reset(oo::PList(oo::PList::Array{oo::PList("missing.png"),
                                     Dict({{"texture", oo::PList("x.png")},
                                           {"probability", oo::PList(-1)}})}));
    EXPECT_TRUE(Manager(MakeRanrotSeed(1)) == nullptr);
    Reset(oo::PList());  // no such plist
    EXPECT_TRUE(Manager(MakeRanrotSeed(1)) == nullptr);
    EXPECT_TRUE(gLog.size() == 1);  // asked for the config, and nothing else
  }
}

TEST(OOProbabilisticTextureManager, seed) {
  {
    Reset(Config());
    RANROTSeed global = MakeRanrotSeed(77);
    RANROTSetFullSeed(global);
    std::shared_ptr<oo::ProbabilisticTextureManager> manager =
        oo::ProbabilisticTextureManager::createWithPListName("test.plist", oo::TextureFlags::kNone, 0,
                                                           0);
    EXPECT_TRUE(SameSeed(manager->seed(), global));  // copied from the global seed
    EXPECT_TRUE(SameSeed(RANROTGetFullSeed(), global));  // and that is left alone

    manager->setSeed(MakeRanrotSeed(5));
    EXPECT_TRUE(SameSeed(manager->seed(), MakeRanrotSeed(5)));
    manager->selectTexture();
    EXPECT_TRUE(
        !SameSeed(manager->seed(), MakeRanrotSeed(5)));  // a pick advances it
    EXPECT_TRUE(SameSeed(RANROTGetFullSeed(), global));

    // Two managers with one seed pick alike.
    EXPECT_TRUE(Picks(Manager(MakeRanrotSeed(9)), 30) ==
             Picks(Manager(MakeRanrotSeed(9)), 30));
  }
}

TEST(OOProbabilisticTextureManager, picks) {
  {
    Reset(Config());
    EXPECT_TRUE(CheckPicks("galaxy 0, seed A",
                        Picks(Manager(MakeRanrotSeed(0x1234567)), 40),
                        "bhabhbabbhbhbbhahbahbbbaabbhbbbbbbabbabb"));
    EXPECT_TRUE(CheckPicks("galaxy 0, seed B",
                        Picks(Manager(MakeRanrotSeed(0x7654321)), 40),
                        "bhbabahhabbabbhbbhbhbahbbaabababaaabhbbb"));
    // In galaxy 2, h (added after g, cumulative 5) is never picked: g
    // (cumulative 8) comes first in the list and takes every pick in (4, 8],
    // and the picks only reach 9.
    gGalaxy = 2;
    EXPECT_TRUE(CheckPicks("galaxy 2, seed A",
                        Picks(Manager(MakeRanrotSeed(0x1234567)), 40),
                        "ggabgbbbggbggggaggaggggbagggbbggggagbbgg"));
    EXPECT_TRUE(CheckPicks("galaxy 2, seed B",
                        Picks(Manager(MakeRanrotSeed(0x7654321)), 40),
                        "ggbabbggabbagbggggggbbgbgabbagbbabbggbbg"));

    // g only in galaxy 0, before a galaxy-wide a: both have cumulative 1 of 2,
    // so a pick past g's share is not a's either, and falls back to the
    // galaxy's first texture (g). a is never picked.
    Reset(oo::PList(oo::PList::Array{
        Dict({{"texture", oo::PList("g.png")}, {"galaxy", oo::PList("0")}}),
        oo::PList("a.png"),
    }));
    EXPECT_TRUE(CheckPicks("fallback to the galaxy's first",
                        Picks(Manager(MakeRanrotSeed(0xBADC0DE)), 40),
                        "gggggggggggggggggggggggggggggggggggggggg"));

    // Nothing for galaxy 1: the last texture, every time.
    Reset(oo::PList(oo::PList::Array{
        Dict({{"texture", oo::PList("g.png")}, {"galaxy", oo::PList("0")}}),
        Dict({{"texture", oo::PList("k.png")}, {"galaxy", oo::PList("2")}}),
    }));
    gGalaxy = 1;
    EXPECT_TRUE(CheckPicks("nothing for the galaxy",
                        Picks(Manager(MakeRanrotSeed(3)), 10), "kkkkkkkkkk"));
  }
}

TEST(OOProbabilisticTextureManager, texturesAreLoadedAndKept) {
  std::vector<TestTexture*> textures;
  std::shared_ptr<oo::ProbabilisticTextureManager> manager;
  {
    Reset(Config());
    manager = Manager(MakeRanrotSeed(0x1234567));
    for (oo::GalaxyID galaxy :
         {0, 2})  // a, b and h in galaxy 0; g only in galaxy 2
    {
      gGalaxy = galaxy;
      for (int i = 0; i < 200; i++) {
        TestTexture* texture =
            static_cast<TestTexture*>(manager->selectTexture());
        bool seen = false;
        for (TestTexture* t : textures) seen = seen || t == texture;
        if (!seen)
          textures.push_back(
              oo::test::Retain(texture));  // the test's reference
      }
    }
  }
  EXPECT_TRUE(textures.size() == 4);
  manager->ensureTexturesLoaded();
  manager->ensureTexturesLoaded();
  for (TestTexture* texture : textures) {
    EXPECT_TRUE(texture->_loaded == 2);
    EXPECT_TRUE(texture->weak_from_this().use_count() == 2);  // the test's and the manager's
  }
  manager = nullptr;
  for (TestTexture* texture : textures) {
    EXPECT_TRUE(texture->weak_from_this().use_count() == 1);
    oo::test::Release(texture);
  }
}
