/*	test_EntityOOJavaScriptExtensions.mm
        Unit tests for the categories Entity (OOJavaScriptExtensions) and
   ShipEntity (OOJavaScriptExtensions)
   (src/Core/Scripting/EntityOOJavaScriptExtensions.h/.mm): what the script
   engine asks an entity by selector (is it visible to scripts, its JS class
        name, its JS class, its JS object) and what scripts ask a ship (its
   subentities, and setting its target, which climbs from a subentity to its
   root ship on both sides).

        The categories reach the entity's C++ part (_jsSelf), so the test links
   the whole game but main
        (['*']) and uses the real Entity. A ship is the test's subclass of
   ShipEntity made with class_createInstance (never initialised, never
   released): it answers the ShipEntity selectors the category sends (its
   subentities, its owner, being a subentity, its targets) and records what it
   was told. The JS object the entity already has is a plain object of a
   QuickJS-ng context the test makes, the engine's main thread context, with
   Entity's and Ship's JS classes registered by their Init functions; the test
   owns that object and frees it. On QuickJS-ng (upstream #768's binding) a JS
   class is its JSClassID, so the class checks compare IDs and the prototype
   checks are gone (the prototype is per context, JS_GetClassProto). Run:
   tests/run.sh unit
*/

#include <quickjs.h>

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSShip.h"
#include "ShipEntity.h"
#include "EntityManualRefs.h"  // the tests' manual entity references

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

// A ship that answers what the functions ask a ship, and records the targets it
// was told. Its subentities, owner and being a subentity are the ship's own
// state, set by the test; its primary target and the targets it is told are
// its overrides (removeTarget() is virtual for it, a test seam).
class TestShip : public ShipEntity {
 public:
  Entity* _testPrimaryTarget = nullptr;
  std::vector<std::string> _told;  // "add <name>" / "remove <name>"
  const char* _name = "";

  Entity* primaryTarget() override { return _testPrimaryTarget; }

  void addTarget(Entity* targetEntity) override {
    _told.push_back(
        std::string("add ") +
        (targetEntity != nullptr
             ? static_cast<TestShip*>(oo::ToShip(targetEntity))->_name
             : "nil"));
  }

  void removeTarget(Entity* targetEntity) override {
    _told.push_back(
        std::string("remove ") +
        (targetEntity != nullptr
             ? static_cast<TestShip*>(oo::ToShip(targetEntity))->_name
             : "nil"));
  }
};

// An entity that scripts can see but that keeps Entity's JS class (a C++
// subclass overriding the member its selector became).
class TestVisibleEntity : public Entity {
 public:
  bool isVisibleToScripts() override { return true; }
};

// A C++ entity made as the game makes one (its object made and autoreleased,
// oo::MadeEntity).
static TestVisibleEntity* NewVisibleEntity() {
  return static_cast<TestVisibleEntity*>(
      oo::MadeEntity(oo::makeShared<TestVisibleEntity>()));
}

extern JSContext* gOOJSMainThreadContext;  // the engine's

namespace {

JSContext* sContext = nullptr;

// The test's context is the engine's main thread context (the JS classes
// register with its runtime), with Entity's and Ship's classes registered by
// their Init functions, as the engine's start registers them.
void SetUpContext() {
  if (sContext != nullptr) return;
  sContext = JS_NewContext(JS_NewRuntime());
  gOOJSMainThreadContext = sContext;
  JSValue global = JS_GetGlobalObject(sContext);
  InitOOJSEntity(sContext, global);
  InitOOJSShip(sContext, global);
  JS_FreeValue(sContext, global);
}

// Whether two JS values are the same object.
bool SameObject(JSValueConst a, JSValueConst b) {
  return JS_IsObject(a) && JS_IsObject(b) &&
         JS_VALUE_GET_PTR(a) == JS_VALUE_GET_PTR(b);
}

// A ship made the way the test can: the C++ ship under its object (retained and
// never released), never set up from a definition, a ship (it answers -isShip,
// as the Objective-C class did, so shipSubEntities() keeps it).
TestShip* Ship(const char* name) {
  std::shared_ptr<TestShip> ship = oo::makeShared<TestShip>();
  {
    oo::RetainEntity(oo::MadeEntity(ship));
  }
  ship->isShip = true;
  ship->_name = name;
  return ship.get();
}

// A subentity's owner, and that it is one (the Objective-C ship answered -owner
// and -isSubEntity).
void SetSubEntityOf(TestShip* ship, Entity* owner) {
  ship->isSubEntity = true;
  ship->Entity::setOwner(owner);
}

std::string Told(TestShip* ship) {
  std::string result;
  for (const std::string& line : ship->_told)
    result += (result.empty() ? "" : "; ") + line;
  return result;
}

}  // namespace

TEST(EntityOOJavaScriptExtensions, entityIsNotVisible) {
  {
    Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
    EXPECT_TRUE(!OOJSEntityIsVisibleToScripts(entity));
    EXPECT_TRUE(OOJSEntityJSClassName(entity) ==
             std::optional<std::string>("Entity"));

    SetUpContext();
    EXPECT_TRUE(OOJSEntityJSClassID(entity) == JSEntityClassID());

    // Not visible: no JS object is made, and the value is null.
    JSValue value = OOJSEntityJSValue(entity, sContext);
    EXPECT_TRUE(JS_IsNull(value));
    EXPECT_TRUE(!JS_IsObject(entity->_jsSelf));

    // With no JS object, -deleteJSSelf does nothing.
    entity->deleteJSSelf();
    EXPECT_TRUE(!JS_IsObject(entity->_jsSelf));
  }
}

TEST(EntityOOJavaScriptExtensions, existingJSObject) {
  {
    SetUpContext();
    JSValue object = JS_NewObject(sContext);
    EXPECT_TRUE(JS_IsObject(object));

    // An entity that already has its JS object answers it, visible or not.
    Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
    entity->_jsSelf = object;
    JSValue value = OOJSEntityJSValue(entity, sContext);
    EXPECT_TRUE(JS_IsObject(value));
    EXPECT_TRUE(SameObject(value, object));
    JS_FreeValue(sContext, value);

    TestVisibleEntity* visible = NewVisibleEntity();
    visible->_jsSelf = object;
    value = OOJSEntityJSValue(visible, sContext);
    EXPECT_TRUE(SameObject(value, object));
    JS_FreeValue(sContext, value);

    // The test owns the object: the entities let go of it before they go.
    entity->_jsSelf = JS_UNDEFINED;
    visible->_jsSelf = JS_UNDEFINED;
    JS_FreeValue(sContext, object);
  }
}

TEST(EntityOOJavaScriptExtensions, visibleEntityKeepsEntityClass) {
  {
    TestVisibleEntity* visible = NewVisibleEntity();
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(visible));
    EXPECT_TRUE(OOJSEntityJSClassName(visible) ==
             std::optional<std::string>("Entity"));
    SetUpContext();
    EXPECT_TRUE(OOJSEntityJSClassID(visible) == JSEntityClassID());
  }
}

TEST(EntityOOJavaScriptExtensions, shipIsVisible) {
  {
    TestShip* ship = Ship("ship");
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(ship));
    EXPECT_TRUE(OOJSEntityJSClassName(ship) == std::optional<std::string>("Ship"));

    SetUpContext();
    EXPECT_TRUE(OOJSEntityJSClassID(ship) == JSShipClassID());
  }
}

TEST(EntityOOJavaScriptExtensions, subEntitiesForScript) {
  {
    TestShip* ship = Ship("ship");
    EXPECT_TRUE(ShipEntityJSSubEntitiesForScript(ship).empty());

    TestShip* turret = Ship("turret");
    TestShip* hull = Ship("hull");
    ship->subEntities = {oo::Hold(turret), oo::Hold(hull)};
    std::vector<oo::EntityHold> subs = ShipEntityJSSubEntitiesForScript(ship);
    EXPECT_TRUE(subs.size() == 2);
    EXPECT_TRUE(subs.size() == 2 && subs[0].get() == turret &&
             subs[1].get() == hull);
    ship->subEntities.clear();
  }
}

TEST(EntityOOJavaScriptExtensions, setTarget) {
  {
    TestShip* ship = Ship("ship");
    TestShip* other = Ship("other");
    ShipEntityJSSetTargetForScript(ship, other);
    EXPECT_TRUE(Told(ship) == "add other");
  }
}

TEST(EntityOOJavaScriptExtensions, setTargetNilRemovesPrimaryTarget) {
  {
    TestShip* ship = Ship("ship");
    TestShip* current = Ship("current");
    ship->_testPrimaryTarget = current;
    ShipEntityJSSetTargetForScript(ship, nullptr);
    EXPECT_TRUE(Told(ship) == "remove current");

    // No primary target: remove nil.
    TestShip* idle = Ship("idle");
    ShipEntityJSSetTargetForScript(idle, nullptr);
    EXPECT_TRUE(Told(idle) == "remove nil");
  }
}

TEST(EntityOOJavaScriptExtensions, setTargetClimbsToRootShips) {
  {
    // A subentity's target is set on its root ship, and a subentity target is
    // replaced by its root ship, however deep.
    TestShip* root = Ship("root");
    TestShip* middle = Ship("middle");
    TestShip* turret = Ship("turret");
    SetSubEntityOf(middle, root);
    SetSubEntityOf(turret, middle);

    TestShip* enemy = Ship("enemy");
    TestShip* enemyTurret = Ship("enemyTurret");
    SetSubEntityOf(enemyTurret, enemy);

    ShipEntityJSSetTargetForScript(turret, enemyTurret);
    EXPECT_TRUE(Told(root) == "add enemy");
    EXPECT_TRUE(Told(turret).empty() && Told(middle).empty());
  }
}

TEST(EntityOOJavaScriptExtensions, setTargetStopsAtSelfOwnedOrOwnerless) {
  {
    // A subentity that owns itself, or has no owner, is where the climb stops.
    TestShip* loop = Ship("loop");
    SetSubEntityOf(loop, loop);
    TestShip* orphan = Ship("orphan");
    orphan->isSubEntity = true;
    ShipEntityJSSetTargetForScript(loop, orphan);
    EXPECT_TRUE(Told(loop) == "add orphan");
  }
}

TEST(EntityOOJavaScriptExtensions, setTargetOnANonShipRootDoesNothing) {
  {
    // A subentity whose owner is not a ship: nothing is told.
    Entity* notAShip = oo::MadeEntity(oo::makeShared<Entity>());
    TestShip* sub = Ship("sub");
    SetSubEntityOf(sub, notAShip);
    TestShip* target = Ship("target");
    ShipEntityJSSetTargetForScript(sub, target);
    EXPECT_TRUE(Told(sub).empty() && Told(target).empty());
  }
}

// The root category asks the entity's C++ part, so a C++ entity answers the
// engine through its overrides, the defaults are the category's bodies, and an
// Objective-C class's own selectors still answer first.
namespace {

JSClassDef sTestScriptedClass = {.class_name = "TestScripted"};
JSClassID sTestScriptedClassID = 0;

class TestScriptedEntity : public Entity {
 public:
  JSClassID jsClassID() override { return sTestScriptedClassID; }
  std::optional<std::string> jsClassName() override {
    return std::string("TestScripted");
  }
  bool isVisibleToScripts() override { return true; }
};

}  // namespace

TEST(EntityOOJavaScriptExtensions, cxxEntityAnswersThroughItsJSMembers) {
  {
    SetUpContext();
    OOJSRegisterClass(&sTestScriptedClassID, &sTestScriptedClass);

    Entity* plain = oo::MadeEntity(oo::makeShared<Entity>());
    EXPECT_TRUE(plain != nullptr);
    EXPECT_TRUE(!OOJSEntityIsVisibleToScripts(plain) &&
             !plain->isVisibleToScripts());
    EXPECT_TRUE(OOJSEntityJSClassName(plain) ==
             std::optional<std::string>("Entity"));
    EXPECT_TRUE(OOJSEntityJSClassID(plain) == JSEntityClassID());

    Entity* scripted = oo::MadeEntity(oo::makeShared<TestScriptedEntity>());
    EXPECT_TRUE(scripted != nullptr);
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(scripted));
    EXPECT_TRUE(OOJSEntityJSClassName(scripted) ==
             std::optional<std::string>("TestScripted"));
    EXPECT_TRUE(OOJSEntityJSClassID(scripted) == sTestScriptedClassID &&
             sTestScriptedClassID != 0);
  }
}

// An entity's Object node holds the object, answers the C++ part, and its JS
// value is the part's JS object.
TEST(EntityOOJavaScriptExtensions, entityObjectNodeHoldsTheEntity) {
  {
    SetUpContext();
    JSValue object = JS_NewObject(sContext);
    EXPECT_TRUE(JS_IsObject(object));
    Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
    Entity* part = entity;
    part->_jsSelf = object;

    const oo::PList node = oo::EntityObjectNode(part);
    EXPECT_TRUE(node.type() == oo::PList::Type::kObject);
    EXPECT_TRUE(oo::EntityIn(node) == part);
    const oo::PList::Object* payload = node.getIf<oo::PList::Object>();
    EXPECT_TRUE(payload != nullptr && (*payload)->className() == "Entity");
    EXPECT_TRUE(payload != nullptr &&
             (*payload)->description() == oo::EntityDescription(entity));
    JSValue value = OOJSValueFromPList(sContext, node);
    EXPECT_TRUE(SameObject(value, object));
    JS_FreeValue(sContext, value);

    // Anything else answers none.
    EXPECT_TRUE(oo::EntityIn(oo::PList(1)) == nullptr);
    EXPECT_TRUE(oo::EntityIn(oo::PList()) == nullptr);
    EXPECT_TRUE(oo::EntityObjectNode(static_cast<Entity*>(nullptr)).isNull());

    // Arrays: one node per entity, in order.
    const std::vector<oo::EntityHold> refs{oo::Hold(entity),
                                           oo::Hold(entity)};
    const std::vector<Entity*> parts =
        oo::EntitiesIn(oo::EntityNodesFrom(refs));
    EXPECT_TRUE(parts.size() == 2 && parts[0] == part && parts[1] == part);

    part->_jsSelf = JS_UNDEFINED;  // the test owns the object
    JS_FreeValue(sContext, object);
  }
}
