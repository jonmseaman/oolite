/*	test_OOVisualEffectEntity.mm
	Unit tests for OOVisualEffectEntity (src/Core/Entities/OOVisualEffectEntity.h), the scripted
	visual effects (initialisers, flags, mesh, subentities and flashers, scaling, orientation
	vectors, drawing, update, the break pattern flag and the subentity relationship).

	The entity reads Universe and PLAYER, so the test links the whole game but main (['*']), starts
	the JavaScript engine (the effect names its script events with OOJSID) in a scratch folder with
	no scripts (so an effect has no script), and uses a Universe that was never initialised, which
	makes the standard subentities the effects ask it for, and a plain entity as PLAYER. No effect
	has a model, so nothing is drawn. The tests pin an effect's flags, key, class and status; its
	scales and the radii they scale; its flasher and standard subentities (made from the definition,
	owned, listed, counted, scaled with it, removed and cleared); its orientation vectors; and the
	break pattern flag. An effect is the global C++ class, made with oo::NewVisualEffectObject()
	as a C++ entity, and the cases call its members. Run: bash
	tools/check-core-tests.sh test_OOVisualEffectEntity
*/

#include <gtest/gtest.h>

#include "VisualEffectEntity.h"
#include "PlayerEntity.h"	// PLAYER is C++
#include "OOJavaScriptEngine.h"
#include "Universe.h"
#include "Color.h"
#include "OOJSPropID.h"
#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSVisualEffect.h"

#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/FileSystem.hpp"

#include <cmath>
#include <cstdlib>  // IWYU pragma: keep
#include <filesystem>
#include <process.h>
#include <string>
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
	HPVector viewpointPosition() override	{ return kZeroHPVector; }
};

// NewTestPlayer<TestPlayer>(): a C++ player, as PlayerEntity::sharedPlayer() makes the game's,
// retained (+1).
template <class T>
T *NewTestPlayer()
{
	std::shared_ptr<T> player = oo::makeShared<T>();
	{
		oo::RetainEntity(oo::MadeEntity(player));
	}
	return player.get();
}


// UNIVERSE: never initialised; makes the standard subentities, an effect with no definition, by
// key ("missing" has none).
class TestUniverse : public Universe
{
public:
	std::shared_ptr<Entity> newVisualEffectWithName(const std::string &effectKey) override	// the effect, owned by the caller
	{
		if (effectKey == "missing")  return nullptr;
		return oo::NewVisualEffect(effectKey, oo::PList());
	}
};


namespace {

namespace stdfs = std::filesystem;

void SetUp()
{
	static stdfs::path root;
	if (root.empty())
	{
		root = stdfs::temp_directory_path() / ("oo-test-visualeffect-" + std::to_string(static_cast<unsigned long>(::_getpid())));
		stdfs::remove_all(root);
		stdfs::create_directories(root / "Resources");
		EXPECT_TRUE(::_putenv_s("HOMEPATH", root.string().c_str()) == 0);
		stdfs::current_path(root);
		EXPECT_TRUE(oo::fs::writeFile(root / "Resources" / "Info-gnustep.plist", oo::Data("{ CFBundleVersion = \"9.9.9-test\"; }", 35), oo::fs::WriteMode::kDirect).has_value());
		{
			(void)OOJavaScriptEngine::sharedEngine();
		}
	}
	static Universe *universe = nullptr;
	if (universe == nullptr)
	{
		universe = new TestUniverse();	// never released
	}
	gSharedUniverse = universe;
	static TestPlayer *player = nullptr;
	if (player == nullptr)
	{
		{	// what the stand-in player's -init autoreleases is freed
       // while the engine is alive
			player = NewTestPlayer<TestPlayer>();
		}
	}
	gOOPlayer = player;
}


bool Near(double a, double b)
{
	return std::fabs(a - b) < 1e-4;
}


oo::PList Dict(oo::PList::Dict entries)
{
	return oo::PList(std::move(entries));
}


oo::PList Numbers(double x, double y, double z)
{
	return oo::PList(oo::PList::Array{ oo::PList(x), oo::PList(y), oo::PList(z) });
}


// A flasher subentity of diameter 4 (collision radius 2) at (x, 0, 0).
oo::PList Flasher(double x)
{
	return Dict({ { "type", oo::PList(std::string("flasher")) }, { "size", oo::PList(4.0) }, { "position", Numbers(x, 0, 0) } });
}


// An effect, as the universe makes it (oo::RetainedEntity(oo::NewVisualEffect()),
// An effect, as the universe makes it (oo::RetainedEntity(oo::NewVisualEffect())): the C++ effect,
// autoreleased.
oo::VisualEffectEntity *Effect(const std::string &key, const oo::PList &definition)
{
	return oo::ToEffect(oo::autorelease(oo::NewVisualEffect(key, definition)));
}


GLfloat NoDrawDistance(Entity *e)	{ return e->no_draw_distance; }

}	// namespace


TEST(OOVisualEffectEntity, defaults)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("test-effect", Dict({}));
		EXPECT_TRUE(effect != nullptr);
		EXPECT_TRUE(effect->isEffect());
		EXPECT_TRUE(effect->getIsVisualEffect());
		EXPECT_TRUE(!effect->canCollide());
		EXPECT_TRUE(effect->effectKey() == std::optional<std::string>("test-effect"));
		EXPECT_TRUE(effect->mesh() == nullptr);
		EXPECT_TRUE(effect->getScanClass() == oo::ScanClass::kVisualEffect);
		EXPECT_TRUE(effect->status() == oo::EntityStatus::kEffect);
		EXPECT_TRUE(!effect->isBreakPattern());
		EXPECT_TRUE(effect->effectInfoDictionary() == Dict({}));

		EXPECT_TRUE(effect->scaleX() == 1.0f && effect->scaleY() == 1.0f && effect->scaleZ() == 1.0f);
		EXPECT_TRUE(effect->scaleMax() == 1.0f);
		EXPECT_TRUE(effect->collisionRadius() == 0.0f);
		EXPECT_TRUE(effect->frustumRadius() == 0.0f);
		EXPECT_TRUE(NoDrawDistance(effect) == 0.0f);

		EXPECT_EQ(effect->subEntityCount(), 0u);
		EXPECT_TRUE(effect->subEntities().empty());
		EXPECT_TRUE(effect->subEntityEnumerator().empty());
		EXPECT_TRUE(!effect->visualEffectSubEntityEnumerator().has_value());
		EXPECT_TRUE(effect->effectSubEntityEnumerator().empty());
		EXPECT_TRUE(effect->flasherEnumerator().empty());

		// A null definition is an empty one; -init has the empty key.
		oo::VisualEffectEntity *plain = Effect(std::string{}, oo::PList());	// what -init made
		EXPECT_TRUE(plain != nullptr);
		EXPECT_TRUE(plain->effectKey() == std::optional<std::string>(""));
		EXPECT_TRUE(plain->effectInfoDictionary() == Dict({}));
	}
}


TEST(OOVisualEffectEntity, breakPatternFlag)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("bp", Dict({ { "is_break_pattern", oo::PList(true) } }));
		EXPECT_TRUE(effect->isBreakPattern());
		effect->setIsBreakPattern(false);
		EXPECT_TRUE(!effect->isBreakPattern());
		effect->setIsBreakPattern(true);
		EXPECT_TRUE(effect->isBreakPattern());
	}
}


TEST(OOVisualEffectEntity, flasherSubentities)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("lights", Dict({ { "subentities", oo::PList(oo::PList::Array{ Flasher(10), oo::PList(std::string("not a dictionary")) }) } }));
		EXPECT_EQ(effect->subEntityCount(), 1u);	// the string made no subentity
		std::vector<oo::EntityHold> flashers = effect->flasherEnumerator();	// the flashers (held C++ entities)
		EXPECT_EQ(flashers.size(), 1u);
		EXPECT_TRUE(effect->effectSubEntityEnumerator().empty());
		EXPECT_TRUE(effect->visualEffectSubEntityEnumerator().has_value() && effect->visualEffectSubEntityEnumerator()->empty());
		if (flashers.size() != 1)  return;

		Entity *flasher = flashers[0].get();
		EXPECT_TRUE(effect->subEntities().size() == 1 && effect->subEntities()[0].get() == flasher);
		EXPECT_TRUE(flasher->owner() == effect);
		EXPECT_TRUE(flasher->getIsSubEntity());
		EXPECT_TRUE(effect->hasSubEntity(flasher));
		EXPECT_TRUE(Near(flasher->getPosition().x, 10.0));

		// The profile radius reaches the flasher's far side: 10 + 2.
		EXPECT_TRUE(Near(effect->frustumRadius(), 12.0));
		EXPECT_TRUE(Near(NoDrawDistance(effect), 12.0 * 12.0 * NO_DRAW_DISTANCE_FACTOR * NO_DRAW_DISTANCE_FACTOR * 2.0));

		// Scaling one axis moves the flasher along it and resizes it by the cube root.
		const GLfloat flasherRadius = flasher->collisionRadius();
		effect->setScaleX(2.0f);
		EXPECT_TRUE(effect->scaleX() == 2.0f);
		EXPECT_TRUE(effect->scaleMax() == 2.0f);
		EXPECT_TRUE(Near(flasher->getPosition().x, 20.0));
		EXPECT_TRUE(Near(flasher->collisionRadius(), flasherRadius * std::pow(2.0, 1.0 / 3.0)));
		EXPECT_TRUE(Near(effect->frustumRadius(), 24.0));
		effect->setScaleY(3.0f);
		EXPECT_TRUE(effect->scaleMax() == 3.0f);
		effect->setScaleZ(0.5f);
		EXPECT_TRUE(effect->scaleZ() == 0.5f);
		EXPECT_TRUE(effect->scaleMax() == 3.0f);

		effect->removeSubEntity(flasher);
		EXPECT_EQ(effect->subEntityCount(), 0u);
		EXPECT_TRUE(flasher->owner() == nullptr);
		EXPECT_TRUE(!effect->hasSubEntity(flasher));
	}
}


TEST(OOVisualEffectEntity, standardSubentities)
{
	::SetUp();
	{
		oo::PList::Array subs;
		subs.push_back(Dict({ { "subentity_key", oo::PList(std::string("child")) }, { "position", Numbers(0, 5, 0) } }));
		subs.push_back(Dict({ { "subentity_key", oo::PList(std::string("missing")) } }));	// the universe has none
		subs.push_back(Dict({ { "position", Numbers(1, 1, 1) } }));							// no key
		subs.push_back(Flasher(-3));
		// Made, and the snapshot taken, in a pool of their own: what is left after it drains is what
		// owns the child (pending autoreleases are not ownership).
		oo::VisualEffectEntity *effect = nullptr;
		std::vector<oo::EntityHold> effects;	// the subentities (held C++ entities)
		Entity *effectObject = nullptr;
		{
			effectObject = oo::RetainedEntity(oo::NewVisualEffect("parent", Dict({ { "subentities", oo::PList(std::move(subs)) } })));
			effect = oo::ToEffect(effectObject);
			effects = effect->effectSubEntityEnumerator();
		}
		oo::AutoreleaseEntity(effectObject);

		EXPECT_EQ(effect->subEntityCount(), 2u);
		EXPECT_EQ(effects.size(), 1u);
		EXPECT_TRUE(effect->visualEffectSubEntityEnumerator().has_value() && effect->visualEffectSubEntityEnumerator()->size() == 1);
		EXPECT_EQ(effect->flasherEnumerator().size(), 1u);
		if (effects.size() != 1)  return;

		Entity *childObject = effects[0].get();	// the child, +0 and not autoreleased
		oo::VisualEffectEntity *child = oo::ToEffect(childObject);
		EXPECT_TRUE(child->effectKey() == std::optional<std::string>("child"));
		EXPECT_TRUE(Near(child->getPosition().y, 5.0));
		EXPECT_TRUE(child->owner() == effect);
		EXPECT_TRUE(child->getIsSubEntity());
		EXPECT_TRUE(childObject->weak_from_this().use_count() == 2);	// the parent's list and the enumerator's snapshot (effects)
		EXPECT_TRUE(effect->isShipWithSubEntityShip(child));
		EXPECT_TRUE(child->parentEntity() == effect);
		EXPECT_TRUE(!child->isShipWithSubEntityShip(effect));

		// Rescaling the whole effect moves and rescales its subentities.
		effect->rescaleBy(2.0f);
		EXPECT_TRUE(Near(child->getPosition().y, 10.0));
		EXPECT_EQ(effect->subEntityCount(), 2u);

		// A scale of one axis is passed on to a visual-effect subentity.
		effect->setScaleY(4.0f);
		EXPECT_TRUE(child->scaleY() == 4.0f);
		EXPECT_TRUE(Near(child->getPosition().y, 40.0));

		effect->clearSubEntities();
		EXPECT_EQ(effect->subEntityCount(), 0u);
		EXPECT_TRUE(effect->subEntities().empty());
		EXPECT_TRUE(!effect->visualEffectSubEntityEnumerator().has_value());
		EXPECT_TRUE(effect->frustumRadius() == 0.0f);
	}
}


TEST(OOVisualEffectEntity, orientationVectors)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("turned", Dict({}));
		Quaternion q = kIdentityQuaternion;
		quaternion_rotate_about_y(&q, 0.5);
		quaternion_rotate_about_x(&q, 0.25);
		effect->setOrientation(q);
		const Quaternion o = effect->getOrientation();
		const Vector f = vector_forward_from_quaternion(o), u = vector_up_from_quaternion(o), r = vector_right_from_quaternion(o);
		EXPECT_TRUE(Near(effect->forwardVector().x, f.x) && Near(effect->forwardVector().y, f.y) && Near(effect->forwardVector().z, f.z));
		EXPECT_TRUE(Near(effect->upVector().x, u.x) && Near(effect->upVector().y, u.y) && Near(effect->upVector().z, u.z));
		EXPECT_TRUE(Near(effect->rightVector().x, r.x) && Near(effect->rightVector().y, r.y) && Near(effect->rightVector().z, r.z));
		EXPECT_TRUE(!Near(effect->forwardVector().z, 1.0));
	}
}


// The scanner colours, the script, the beacons and the shader uniforms.
TEST(OOVisualEffectEntity, scannerColours)
{
	::SetUp();
	{
		oo::VisualEffectEntity *plain = Effect("plain", Dict({}));
		EXPECT_TRUE(plain->scannerDisplayColor1() == nullptr);
		EXPECT_TRUE(plain->scannerDisplayColor2() == nullptr);
		EXPECT_TRUE(plain->scannerDisplayColorForShip(true, nullptr, nullptr)[3] == 0.0f);	// transparent black

		oo::VisualEffectEntity *effect = Effect("coloured", Dict({ { "scanner_display_color1", oo::PList(std::string("redColor")) }, { "scanner_display_color2", oo::PList(std::string("blueColor")) } }));
		oo::Color *c1 = effect->scannerDisplayColor1();
		oo::Color *c2 = effect->scannerDisplayColor2();
		EXPECT_TRUE(c1 != nullptr && c2 != nullptr);
		float r, g, b, a;
		c1->getRed(&r, &g, &b, &a);
		EXPECT_TRUE(r == 1.0f && g == 0.0f && b == 0.0f && a == 1.0f);

		GLfloat *flashOn = effect->scannerDisplayColorForShip(true, c1, c2);
		EXPECT_TRUE(flashOn[0] == 1.0f && flashOn[2] == 0.0f);
		GLfloat *flashOff = effect->scannerDisplayColorForShip(false, c1, c2);
		EXPECT_TRUE(flashOff[0] == 0.0f && flashOff[2] == 1.0f);
		EXPECT_TRUE(effect->scannerDisplayColorForShip(false, c1, nullptr)[0] == 1.0f);
		EXPECT_TRUE(effect->scannerDisplayColorForShip(true, nullptr, c2)[2] == 1.0f);

		// A new colour replaces it; nil takes the definition's again.
		effect->setScannerDisplayColor1(oo::Color::greenColor().get());
		effect->scannerDisplayColor1()->getRed(&r, &g, &b, &a);
		EXPECT_TRUE(r == 0.0f && g == 1.0f && b == 0.0f);
		effect->setScannerDisplayColor1(nullptr);
		effect->scannerDisplayColor1()->getRed(&r, &g, &b, &a);
		EXPECT_TRUE(r == 1.0f && g == 0.0f && b == 0.0f);
	}
}


TEST(OOVisualEffectEntity, scriptAndScriptInfo)
{
	::SetUp();
	{
		// No scripts in the scratch folder: neither the named script nor the default one is found.
		oo::VisualEffectEntity *effect = Effect("scripted", Dict({ { "script", oo::PList(std::string("no-such-script.js")) }, { "script_info", Dict({ { "k", oo::PList(std::string("v")) } }) } }));
		EXPECT_TRUE(effect->script() == nullptr);
		EXPECT_TRUE(effect->scriptInfo() == Dict({ { "k", oo::PList(std::string("v")) } }));
		EXPECT_TRUE(Effect("bare", Dict({}))->scriptInfo() == Dict({}));
		effect->doScriptEvent(OOJSID("anEvent"));	// no script: nothing happens
	}
}


TEST(OOVisualEffectEntity, beacons)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("unlit", Dict({}));
		EXPECT_TRUE(!effect->isBeacon());
		EXPECT_TRUE(!effect->beaconCode().has_value());
		EXPECT_TRUE(!effect->beaconLabel().has_value());
		EXPECT_TRUE(!effect->isJammingScanning());
		EXPECT_TRUE(effect->prevBeacon() == nullptr && effect->nextBeacon() == nullptr);

		oo::VisualEffectEntity *other = Effect("other", Dict({}));
		effect->setPrevBeacon(other);
		effect->setNextBeacon(other);
		EXPECT_TRUE(effect->prevBeacon() == other && effect->nextBeacon() == other);
		effect->setNextBeacon(nullptr);
		EXPECT_TRUE(effect->nextBeacon() == nullptr);
		effect->setPrevBeacon(nullptr);

		// An empty code is none.
		effect->setBeaconCode(std::optional<std::string>(""));
		EXPECT_TRUE(!effect->beaconCode().has_value());
		EXPECT_TRUE(effect->compareBeaconCodeWith(other) == oo::ComparisonResult::kSame);
	}
}


TEST(OOVisualEffectEntity, shaderUniforms)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("shaded", Dict({}));
		EXPECT_TRUE(Near(effect->hullHeatLevel(), 60.0 / 256.0));
		effect->setHullHeatLevel(2.0f);
		EXPECT_TRUE(effect->hullHeatLevel() == 1.0f);	// clamped
		effect->setHullHeatLevel(-1.0f);
		EXPECT_TRUE(effect->hullHeatLevel() == 0.0f);
		EXPECT_TRUE(effect->shaderFloat1() == 0.0f && effect->shaderFloat2() == 0.0f);
		EXPECT_TRUE(effect->shaderInt1() == 0 && effect->shaderInt2() == 0);
		effect->setShaderFloat1(1.5f);
		effect->setShaderFloat2(2.5f);
		effect->setShaderInt1(3);
		effect->setShaderInt2(4);
		effect->setShaderVector1(make_vector(1, 2, 3));
		effect->setShaderVector2(make_vector(4, 5, 6));
		EXPECT_TRUE(effect->shaderFloat1() == 1.5f && effect->shaderFloat2() == 2.5f);
		EXPECT_TRUE(effect->shaderInt1() == 3 && effect->shaderInt2() == 4);
		EXPECT_TRUE(effect->shaderVector1().y == 2.0f && effect->shaderVector2().z == 6.0f);
	}
}


// An effect is a C++ entity made by the initialiser the universe sends; the C++
// members give its answers, and the selectors still answer on the entity.
TEST(OOVisualEffectEntity, facadeContract)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("facade", Dict({ { "subentities", oo::PList(oo::PList::Array{ Flasher(1) }) } }));
		oo::VisualEffectEntity *cxxEffect = effect;
		EXPECT_TRUE(cxxEffect != nullptr);
		EXPECT_TRUE(oo::ToEffect(cxxEffect) == effect);
		EXPECT_TRUE(oo::ToEffect(static_cast<Entity *>(nullptr)) == nullptr);
		EXPECT_TRUE(static_cast<oo::VisualEffectEntity *>(nullptr) == nullptr);

		EXPECT_TRUE(cxxEffect->effectKey() == effect->effectKey());
		EXPECT_TRUE(cxxEffect->subEntityCount() == effect->subEntityCount());
		EXPECT_TRUE(cxxEffect->getIsVisualEffect() && cxxEffect->isEffect() && !cxxEffect->canCollide());
		EXPECT_TRUE(Near(cxxEffect->frustumRadius(), effect->frustumRadius()));
		cxxEffect->setScaleZ(5.0f);
		EXPECT_TRUE(effect->scaleZ() == 5.0f);
		EXPECT_TRUE(effect->scaleMax() == 5.0f);

		// The flasher's owner is the façade the C++ member handed it.
		std::vector<oo::EntityHold> flashers = cxxEffect->flasherEnumerator();
		EXPECT_TRUE(flashers.size() == 1 && flashers[0].get()->owner() == effect);

		// The members, on the façade, read the state the class keeps.
		EXPECT_TRUE(Near(effect->hullHeatLevel(), 60.0 / 256.0));
		effect->setShaderFloat1(0.5f);
		EXPECT_TRUE(cxxEffect->_shaderFloat1 == 0.5f);
	}
}


// The scanner-colour, script, beacon and shader-uniform members answer as the
// façade does.
TEST(OOVisualEffectEntity, slice2Members)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("members", Dict({ { "scanner_display_color1", oo::PList(std::string("redColor")) } }));
		oo::VisualEffectEntity *cxxEffect = effect;
		EXPECT_TRUE(cxxEffect->scannerDisplayColor1() == effect->scannerDisplayColor1());
		EXPECT_TRUE(cxxEffect->scannerDisplayColor2() == nullptr);
		cxxEffect->setShaderInt2(7);
		EXPECT_TRUE(effect->shaderInt2() == 7);
		cxxEffect->setHullHeatLevel(0.25f);
		EXPECT_TRUE(effect->hullHeatLevel() == 0.25f);
		EXPECT_TRUE(cxxEffect->script() == effect->script());
		EXPECT_TRUE(!cxxEffect->isBeacon() && !cxxEffect->isJammingScanning());
		EXPECT_TRUE(cxxEffect->scannerDisplayColorForShip(true, cxxEffect->scannerDisplayColor1(), nullptr)[0] == 1.0f);
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


// The binding's category: what the engine asks an effect's object for by
// selector is what OOJSVisualEffect.mm answers.
TEST(OOVisualEffectEntity, jsExtensions)
{
	::SetUp();
	{
		oo::VisualEffectEntity *effect = Effect("js-effect", Dict({}));
		RegisterJSClasses(InitOOJSVisualEffect);
		Entity *object = effect;	// the engine asks the object
		const JSClassID jsClass = OOJSEntityJSClassID(object);
		const JSClassID expectedClass = JSVisualEffectClassID();
		EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
		EXPECT_TRUE(OOJSEntityJSClassName(object) == std::optional<std::string>("VisualEffect"));
		EXPECT_TRUE(OOJSEntityIsVisibleToScripts(object) == true);
	}
}
