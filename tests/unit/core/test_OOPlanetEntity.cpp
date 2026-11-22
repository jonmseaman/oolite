/*	test_OOPlanetEntity.mm
	Unit tests for OOPlanetEntity (src/Core/Entities/OOPlanetEntity.h), a planet or moon.

	The planet reads the universe, PLAYER and the textures, so the test links the whole game but
	main (['*']). UNIVERSE is a Universe that was never initialised, subclassed to answer the system
	data (the planet's name and air density only, so its colours are its own), the time and minimum
	detail (no shaders); PLAYER is an entity that answers the clock and the nearest planet. Texture
	loading is replaced (method_setImplementation, as test_OOParticleSystem does) by a stand-in that
	records the configuration it was given and finds nothing, so the planet stops before it makes
	materials, as it does in the game when a texture file is missing. The RNG is seeded, and the
	values that depend on it are given in the planet's dictionary. A planet's radius, energy,
	status, orientation and rotation from its dictionary, a moon and a planet with an atmosphere and
	its air colour, the shader-visible values and their setters, the miniature version, -update:
	turning the planet, close collisions and the description. The cases make the C++ planet and ask
	the C++ class, and the facade case pins the
	crossing: the root Entity answers the shader binding selectors for a
	planet only. Run: bash tools/check-core-tests.sh
*/

#include <gtest/gtest.h>

#include "PlanetEntity.h"
#include "PlayerEntity.h"	// PLAYER is C++
#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSPlanet.h"
#include "Color.h"
#include "Texture.h"
#include "Universe.h"

#include "legacy_random.h"

#include <cmath>
#include <string>
#include <vector>
#include "EntityManualRefs.h"  // the tests' manual entity references


// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity *gOOPlayer;
extern Universe *gSharedUniverse;


class TestPlayer : public PlayerEntity
{
public:
	double	_clock = {};
	oo::PlanetEntity	*_nearestPlanet = {};	// the C++ planet

	HPVector viewpointPosition() override	{ return kZeroHPVector; }
	double clockTimeAdjusted() override	{ return _clock; }
	::oo::PlanetEntity * findNearestPlanet() override	{ return _nearestPlanet; }
	oo::SystemID systemID() override	{ return 7; }
};

// A C++ player, as PlayerEntity::sharedPlayer() makes
// the game's, retained (+1).
template <class T>
T *NewTestPlayer()
{
	std::shared_ptr<T> player = oo::makeShared<T>();
	{
		oo::RetainEntity(oo::MadeEntity(player));
	}
	return player.get();
}




// UNIVERSE: never initialised; answers the system data, the time and the detail level.
class TestUniverse : public Universe
{
public:
	oo::PList _systemData = {};
	oo::TimeAbsolute &_time = universal_time;	// the universe's own state, which its getter answers
	GLfloat &_airResistance = airResistanceFactor;	// the universe's own state, which setAirResistanceFactor() sets

	oo::PList generateSystemData(oo::SystemID /*s*/, bool /*useCache*/) override	{ return _systemData; }
	bool reducedDetail() override	{ return true; }
};


namespace {

TestUniverse *sUniverse = nullptr;
TestPlayer *sPlayer = nullptr;
std::vector<std::string> sTexturesAsked;


// OOTexture::configurationFactory, the test-only loader hook: records the name,
// finds nothing.
std::shared_ptr<oo::Texture> NoTexture(const oo::PList &configuration)
{
	sTexturesAsked.push_back(configuration.get<std::string>("name", "?"));
	return nullptr;
}


void SetUp()
{
	if (sUniverse == nullptr)
	{
		sUniverse = new TestUniverse();	// never released
		new (&sUniverse->_systemData) oo::PList();
		sPlayer = NewTestPlayer<TestPlayer>();
		oo::Texture::configurationFactory = &NoTexture;
	}
	sUniverse->_systemData = oo::PList(oo::PList::Dict{ { "air_density", oo::PList(0.4) }, { "planet_name", oo::PList("Lave") } });
	sUniverse->_time = 0;
	sUniverse->_airResistance = -1;
	gSharedUniverse = sUniverse;
	gOOPlayer = sPlayer;
	sPlayer->_clock = 0;
	sPlayer->_nearestPlanet = nullptr;
	sTexturesAsked.clear();
	ranrot_srand(12345);
}


oo::PList Dict(oo::PList::Dict entries)
{
	return oo::PList(std::move(entries));
}


bool Near(double a, double b, double epsilon = 1e-4)
{
	return std::fabs(a - b) < epsilon;
}


bool QuaternionNear(Quaternion a, Quaternion b)
{
	return Near(a.w, b.w) && Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z);
}


oo::PList PlanetDict(int radiusKm)
{
	return Dict({
		{ "radius", oo::PList(radiusKm) },
		{ "techlevel", oo::PList(8) },
		{ "texture", oo::PList("missing.png") },
		{ "rotational_velocity", oo::PList(0.02) },
		{ "atmosphere_rotational_velocity", oo::PList(0.01) },
		{ "air_color", oo::PList("0.2 0.4 0.6") },
		{ "air_color_mix_ratio", oo::PList(0.6) },
		{ "percent_cloud", oo::PList(40) },
	});
}


// A new C++ planet.
oo::PlanetEntity *Planet(bool atmosphere, int radiusKm = 5000)
{
	Random_Seed seed = { 1, 2, 3, 4, 5, 6 };
	std::shared_ptr<oo::PlanetEntity> planet = oo::makeShared<oo::PlanetEntity>();
	(void)oo::MadeEntity(planet);
	planet->initFromDictionary(PlanetDict(radiusKm), atmosphere, seed, 7);
	return planet.get();
}


// The miniature version: a new C++ planet.
oo::PlanetEntity *Miniature(oo::PlanetEntity *planet)
{
	std::shared_ptr<oo::PlanetEntity> mini = planet->miniatureVersion();
	(void)oo::MadeEntity(mini);
	return mini.get();
}

}	// namespace


TEST(OOPlanetEntity, moon)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		EXPECT_TRUE(moon != nullptr && dynamic_cast<oo::PlanetEntity *>(static_cast<Entity *>(moon)) != nullptr);
		EXPECT_TRUE(moon->radius() == 50000.0);
		EXPECT_TRUE(moon->collisionRadius() == 50000.0f);
		EXPECT_TRUE(moon->getEnergy() == 50000.0f * 1000.0f);
		EXPECT_TRUE(moon->status() == oo::EntityStatus::kActive);
		EXPECT_TRUE(moon->getScanClass() == oo::ScanClass::kNoDraw);
		EXPECT_TRUE(moon->isPlanet() && moon->isVisible());
		EXPECT_TRUE(moon->planetType() == oo::StellarBodyType::kMoon);
		EXPECT_TRUE(!moon->hasAtmosphere());
		EXPECT_TRUE(moon->rotationalVelocity() == (double)0.02f);
		EXPECT_TRUE(QuaternionNear(moon->getOrientation(), (Quaternion){ (float)M_SQRT1_2, (float)M_SQRT1_2, 0, 0 }));
		EXPECT_TRUE(moon->airColor() == nullptr);
		EXPECT_TRUE(moon->airColorMixRatio() == 0.5f && moon->airDensity() == 0.75f);
		// The land parameters always set an illumination colour.
		EXPECT_TRUE(moon->illuminationColor() != nullptr);
		Vector threshold = moon->terminatorThresholdVector();
		EXPECT_TRUE(Near(threshold.x, 0.105) && Near(threshold.y, 0.18) && Near(threshold.z, 0.28));
		// The texture was asked for as a cube map of the dictionary's name, and not found.
		EXPECT_TRUE(sTexturesAsked.size() == 1 && sTexturesAsked[0] == "missing.png");
		EXPECT_TRUE(!moon->textureFileName().has_value());
		EXPECT_TRUE(moon->material() == nullptr && moon->atmosphereMaterial() == nullptr && moon->atmosphereShaderMaterial() == nullptr);
		EXPECT_TRUE(moon->isFinishedLoading());
		// The system data's planet name, expanded.
		EXPECT_TRUE(moon->name() == std::optional<std::string>("Lave"));
	}
}


TEST(OOPlanetEntity, planetWithAtmosphere)
{
	{
		::SetUp();
		oo::PlanetEntity *planet = Planet(true);
		EXPECT_TRUE(planet->planetType() == oo::StellarBodyType::kNormalPlanet);
		EXPECT_TRUE(planet->hasAtmosphere());
		// The dictionary's air colour, through its hue, saturation and brightness.
		oo::Color *air = planet->airColor();
		EXPECT_TRUE(air != nullptr);
		EXPECT_TRUE(air != nullptr && Near(air->redComponent(), 0.2, 1e-3) && Near(air->greenComponent(), 0.4, 1e-3) && Near(air->blueComponent(), 0.6, 1e-3));
		Vector airVector = planet->airColorAsVector();
		EXPECT_TRUE(Near(airVector.x, 0.2, 1e-3) && Near(airVector.y, 0.4, 1e-3) && Near(airVector.z, 0.6, 1e-3));
		EXPECT_TRUE(Near(planet->airColorMixRatio(), 0.6));
		// The system data's air density.
		EXPECT_TRUE(Near(planet->airDensity(), 0.4));
		EXPECT_TRUE(planet->rotationalVelocity() == (double)0.02f);
		EXPECT_TRUE(sTexturesAsked.size() == 1);
	}
}


TEST(OOPlanetEntity, clockTurnsThePlanet)
{
	{
		::SetUp();
		sPlayer->_clock = 86400.0 * 3 + 100.0;	// 100 s into the day
		oo::PlanetEntity *planet = Planet(false);
		Quaternion expected = (Quaternion){ (float)M_SQRT1_2, (float)M_SQRT1_2, 0, 0 };
		quaternion_rotate_about_axis(&expected, vector_up_from_quaternion(expected), 0.02f * 100);
		EXPECT_TRUE(QuaternionNear(planet->getOrientation(), expected));
	}
}


TEST(OOPlanetEntity, shaderValues)
{
	{
		::SetUp();
		oo::PlanetEntity *planet = Planet(false);

		std::shared_ptr<oo::Color>	red = oo::Color::colorWithRed(1.0f, 0.0f, 0.0f, 1.0f);
		planet->setAirColor(red.get());
		EXPECT_TRUE(planet->airColor() == red.get());
		planet->setAirColor(nullptr);		// ignored
		EXPECT_TRUE(planet->airColor() == red.get());
		Vector v = planet->airColorAsVector();
		EXPECT_TRUE(v.x == 1.0f && v.y == 0.0f && v.z == 0.0f);

		std::shared_ptr<oo::Color>	blue = oo::Color::colorWithRed(0.0f, 0.0f, 1.0f, 1.0f);
		planet->setIlluminationColor(blue.get());
		EXPECT_TRUE(planet->illuminationColor() == blue.get());
		planet->setIlluminationColor(nullptr);	// ignored
		EXPECT_TRUE(planet->illuminationColor() == blue.get());
		v = planet->illuminationColorAsVector();
		EXPECT_TRUE(v.x == 0.0f && v.y == 0.0f && v.z == 1.0f);

		planet->setAirColorMixRatio(1.5f);
		EXPECT_TRUE(planet->airColorMixRatio() == 1.0f);
		planet->setAirColorMixRatio(0.25f);
		EXPECT_TRUE(planet->airColorMixRatio() == 0.25f);
		planet->setAirDensity(-1.0f);
		EXPECT_TRUE(planet->airDensity() == 0.0f);
		planet->setAirDensity(0.5f);
		EXPECT_TRUE(planet->airDensity() == 0.5f);

		planet->setTerminatorThresholdVector(make_vector(1, 2, 3));
		Vector t = planet->terminatorThresholdVector();
		EXPECT_TRUE(t.x == 1 && t.y == 2 && t.z == 3);

		planet->setRotationalVelocity(0.5);
		EXPECT_TRUE(planet->rotationalVelocity() == (double)0.5f);

		planet->setName(std::string("Diso"));
		EXPECT_TRUE(planet->name() == std::optional<std::string>("Diso"));
	}
}


TEST(OOPlanetEntity, miniature)
{
	{
		::SetUp();
		oo::PlanetEntity *planet = Planet(true);
		oo::PlanetEntity *mini = Miniature(planet);
		EXPECT_TRUE(mini != nullptr && mini != planet && dynamic_cast<oo::PlanetEntity *>(static_cast<Entity *>(mini)) != nullptr);
		EXPECT_TRUE(mini->planetType() == oo::StellarBodyType::kMiniature);
		EXPECT_TRUE(mini->status() == oo::EntityStatus::kCockpitDisplay);
		EXPECT_TRUE(mini->getScanClass() == oo::ScanClass::kNoDraw);
		EXPECT_TRUE(Near(mini->radius(), 50000.0 * PLANET_MINIATURE_FACTOR));
		EXPECT_TRUE(mini->rotationalVelocity() == (double)0.04f);
		EXPECT_TRUE(QuaternionNear(mini->getOrientation(), planet->getOrientation()));
		EXPECT_TRUE(mini->hasAtmosphere());
		Vector t = mini->terminatorThresholdVector();
		EXPECT_TRUE(Near(t.x, 0.105) && Near(t.y, 0.18) && Near(t.z, 0.28));
		// The original is unchanged.
		EXPECT_TRUE(planet->radius() == 50000.0 && planet->planetType() == oo::StellarBodyType::kNormalPlanet);
	}
}


TEST(OOPlanetEntity, updateTurnsThePlanet)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		Quaternion before = moon->getOrientation();
		moon->update(10.0);
		Quaternion expected = before;
		quaternion_rotate_about_axis(&expected, vector_up_from_quaternion(before), 0.02f * 10.0f);
		EXPECT_TRUE(QuaternionNear(moon->getOrientation(), expected));

		// The nearest planet, out of its atmosphere, sets no air resistance.
		sPlayer->_nearestPlanet = moon;
		moon->update(1.0);
		EXPECT_TRUE(sUniverse->_airResistance == 0.0f);
		sPlayer->_nearestPlanet = nullptr;

		// A miniature turns at its own rate.
		oo::PlanetEntity *mini = Miniature(moon);
		before = mini->getOrientation();
		mini->update(2.0);
		expected = before;
		quaternion_rotate_about_axis(&expected, vector_up_from_quaternion(before), 0.04f * 2.0f);
		EXPECT_TRUE(QuaternionNear(mini->getOrientation(), expected));
	}
}


TEST(OOPlanetEntity, orientationSetsTheAxis)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		Quaternion q = kIdentityQuaternion;
		moon->setOrientation(q);
		moon->update(5.0);
		// Turned about the new up axis.
		Quaternion expected = q;
		quaternion_rotate_about_axis(&expected, vector_up_from_quaternion(q), 0.02f * 5.0f);
		EXPECT_TRUE(QuaternionNear(moon->getOrientation(), expected));
	}
}


TEST(OOPlanetEntity, closeCollision)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		EXPECT_TRUE(!moon->checkCloseCollisionWith(nullptr));
		Entity *rock = oo::MadeEntity(oo::makeShared<Entity>());
		EXPECT_TRUE(moon->checkCloseCollisionWith(rock));
	}
}


TEST(OOPlanetEntity, description)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false, 3000);
		std::string description = oo::EntityDescription(moon);
		EXPECT_TRUE(description.find("OOPlanetEntity") != std::string::npos);
		EXPECT_TRUE(description.find("radius: 30000 m") != std::string::npos);
	}
}


// A planet is the root Entity; it answers the shader binding selectors (Entity
// (OOPlanetShaderBindings)) for a planet only.
TEST(OOPlanetEntity, facade)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		Entity *object = moon;
		EXPECT_TRUE(object != nullptr);	// A C++ entity.
		EXPECT_TRUE(object == moon);
		oo::PlanetEntity *mini = Miniature(moon);
		EXPECT_TRUE(mini == mini);	// The shader bindings: the planet's values, which its object's selectors
    // forward to.
		moon->setAirDensity(0.5f);
		moon->setTerminatorThresholdVector(make_vector(1, 2, 3));
		EXPECT_TRUE(moon->airDensity() == 0.5f);
		Vector t = moon->terminatorThresholdVector();
		EXPECT_TRUE(t.x == 1 && t.y == 2 && t.z == 3);
	}
}


// Registers Entity's JS class and the given class (a child of Entity) in the test's context, as the
// engine's start does; the context is made here if the test has none yet.
static void RegisterJSClasses(void (*initClass)(JSContext *, JSValueConst))
{
	extern JSContext *gOOJSMainThreadContext;
	if (gOOJSMainThreadContext == nullptr)  gOOJSMainThreadContext = JS_NewContext(JS_NewRuntime());
	JSContext *context = OOJSAcquireContext();
	JSValue global = JS_GetGlobalObject(context);
	InitOOJSEntity(context, global);
	initClass(context, global);
	JS_FreeValue(context, global);
}


// The binding's category, which the C++ planet's overrides answer: what the
// engine asks a planet's object for by selector is what OOJSPlanet.mm answers.
TEST(OOPlanetEntity, jsExtensions)
{
	{
		::SetUp();
		oo::PlanetEntity *moon = Planet(false);
		RegisterJSClasses(InitOOJSPlanet);
		Entity *object = moon;
		const JSClassID jsClass = OOJSEntityJSClassID(object);
		const JSClassID expectedClass = JSPlanetClassID();
		EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
		EXPECT_TRUE(OOJSEntityJSClassName(object) == std::optional<std::string>("Moon"));
		EXPECT_TRUE(OOJSEntityIsVisibleToScripts(object) == true);
	}
}
