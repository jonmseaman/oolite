/*	test_OOShipGroup.mm
	Unit tests for OOShipGroup and OOShipGroupCursor (src/Core/OOShipGroup.h).

	A group holds weak references to its ships (and to its leader), so a ship that dies leaves it.
	The group's own objects are linked; ShipEntity, which reaches the whole game, is replaced by a
	minimal weak-referenceable class of the same name that records what the group asks of it; the
	ship is C++, so the stand-in is a C++ ship under a stand-in root object, and the cases hold the
	ships. Naming, adding (no duplicates, growth past the initial capacity), the leader (added
	as a member, dropped when it dies), removing (the ship is told to leave), dead members leaving,
	the two member arrays, the cursor (and its mutation check), and the description text. A dying
	group lets go of its weak reference to its leader as it does of its members'. The cases ask the
	C++ group, holding the factory's std::shared_ptr, and objectNode pins how a group travels in plist data.
	Run: bash tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "ShipGroup.h"
#include "EntityManualRefs.h"  // the tests' manual references

#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Exception.hpp"

#include <algorithm>
#include <string>
#include <vector>


// The JS glue of OOShipGroup, which OOJSShipGroup.mm defines and this test
// does not link: the vtable names them. Nothing here reaches the JS engine.
JSValue oo::ShipGroup::jsValueInContext(JSContext*)  { return JS_NULL; }
void oo::ShipGroup::clearJSSelf(JSValueConst)  {}
std::optional<std::string> oo::ShipGroup::jsDescription()  { return std::nullopt; }


// --- A stand-in for ShipEntity (see the banner)
// ---------------------------------------------------

/*	The ship is C++: the group holds weak references to the ships' objects
   (the root's, which hold their C++ parts, oo::ToCxx reads them, and which
   oo::ToObjC answers for them) and calls the ships' members. Declared as
   Entity.h and ShipEntity.h declare them (the test imports neither); every
   object the test makes holds a ship that records what the group asks of it.
   The object is C++, rooted on oo::Object.
*/
class TestEntityObject;

class Entity : public oo::Object
{
public:
	virtual ~Entity();

	::TestEntityObject *_object = nullptr;	// its object (not retained)
};

class ShipEntity : public Entity
{
public:
	void setGroup(::oo::ShipGroup *group);
	void setOwner(Entity *who_owns_entity);

	::oo::ShipGroup *lastGroup = nullptr;
	Entity *lastOwner = nullptr;
	int setGroupCalls = 0;
};

// The group holds its ships' C++ parts weakly.
class TestEntityObject final : public oo::Object
{
public:
	// Every object the test makes holds its ship.
	TestEntityObject() : _cxxEntity(oo::makeShared<ShipEntity>())  { _cxxEntity->_object = this; }
	const char *className() const  { return "Entity"; }	// the class's runtime name

	std::shared_ptr<Entity> _cxxEntity;
};

Entity::~Entity() = default;

void ShipEntity::setGroup(::oo::ShipGroup *group)
{
	setGroupCalls++;
	lastGroup = group;
}


void ShipEntity::setOwner(Entity *who_owns_entity)
{
	lastOwner = who_owns_entity;
}

// The test's object of an entity.
::TestEntityObject *TestObjectOf(Entity *entity)  { return entity != nullptr ? entity->_object : nullptr; }

#define OO_ENTITY_HELPERS_STAND_IN_CXX
#include "EntityHelpersStandIn.h"	// EntityHelpers.mm's, on the test's objects


namespace {

ShipEntity *Ship(::TestEntityObject *object)  { return static_cast<ShipEntity *>(object->_cxxEntity.get()); }


// A ship, its object autoreleased (as [[[ShipEntity alloc] init] autorelease] was).
ShipEntity *NewShip()
{
	return Ship(oo::autorelease(oo::makeShared<::TestEntityObject>()));
}


bool HasMembers(const std::vector<oo::EntityHold> &members, std::vector<ShipEntity *> expected)
{
	std::vector<ShipEntity *> got;
	for (const oo::EntityHold &object : members)  got.push_back(Ship(TestObjectOf(object.get())));
	std::sort(got.begin(), got.end());
	std::sort(expected.begin(), expected.end());
	return got == expected;
}

}	// namespace


TEST(OOShipGroup, namesAndEmptyGroups)
{
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> unnamed = oo::ShipGroup::groupWithName(std::nullopt);	// -init
		EXPECT_TRUE(unnamed != nullptr && !unnamed->name().has_value());
		EXPECT_TRUE(unnamed->count() == 0 && unnamed->isEmpty() && unnamed->leader() == nullptr);
		EXPECT_TRUE(unnamed->memberArray().empty() && unnamed->memberArrayExcludingLeader().empty());

		std::shared_ptr<oo::ShipGroup> named = oo::ShipGroup::groupWithName(std::string("escort group"));
		EXPECT_TRUE(named->name() == std::optional<std::string>("escort group"));
		named->setName(std::string("renamed"));
		EXPECT_TRUE(named->name() == std::optional<std::string>("renamed"));
		named->setName(std::nullopt);
		EXPECT_TRUE(!named->name().has_value());

		std::shared_ptr<oo::ShipGroup> made = oo::ShipGroup::groupWithName(std::string("ship group"));	// -initWithName:
		EXPECT_TRUE(made->name() == std::optional<std::string>("ship group") && made->isEmpty());
	}
}


TEST(OOShipGroup, addingAndGrowing)
{
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::nullopt);
		std::vector<ShipEntity *> ships;
		for (int i = 0; i < 20; i++)  ships.push_back(NewShip());	// past kMinSize (4): the array grows

		for (ShipEntity *ship : ships)  EXPECT_TRUE(group->addShip(ship));
		EXPECT_TRUE(group->count() == 20 && !group->isEmpty());
		EXPECT_TRUE(group->addShip(ships[3]));	// already in: YES, and not added twice
		EXPECT_TRUE(group->count() == 20);
		EXPECT_TRUE(group->containsShip(ships[19]) && !group->containsShip(NewShip()));
		EXPECT_TRUE(HasMembers(group->memberArray(), ships));
		EXPECT_TRUE(HasMembers(group->memberArrayExcludingLeader(), ships));	// no leader
	}
}


TEST(OOShipGroup, leader)
{
	{
		oo::AutoreleaseScope pool;
		ShipEntity *leader = NewShip();
		ShipEntity *wingman = NewShip();
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::string("g"), leader);
		EXPECT_TRUE(group->leader() == leader && group->containsShip(leader) && group->count() == 1);
		group->addShip(wingman);
		EXPECT_TRUE(HasMembers(group->memberArrayExcludingLeader(), { wingman }));
		EXPECT_TRUE(HasMembers(group->memberArray(), { leader, wingman }));

		group->setLeader(wingman);
		EXPECT_TRUE(group->leader() == wingman && group->count() == 2);
		group->setLeader(nullptr);
		EXPECT_TRUE(group->leader() == nullptr && group->count() == 2);	// still members
	}
}


TEST(OOShipGroup, removing)
{
	{
		oo::AutoreleaseScope pool;
		ShipEntity *a = NewShip(), *b = NewShip(), *c = NewShip();
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::nullopt, a);
		group->addShip(b);
		group->addShip(c);

		EXPECT_TRUE(group->removeShip(b));
		EXPECT_TRUE(!group->containsShip(b) && group->count() == 2);
		EXPECT_TRUE(b->setGroupCalls == 1 && b->lastGroup == nullptr && b->lastOwner == b);	// told to leave
		EXPECT_TRUE(!group->removeShip(b));	// not a member

		EXPECT_TRUE(group->removeShip(a));	// the leader: no longer leads
		EXPECT_TRUE(group->leader() == nullptr && group->count() == 1 && group->containsShip(c));
	}
}


TEST(OOShipGroup, deadShipsLeave)
{
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::nullopt);
		ShipEntity *survivor = NewShip();
		::TestEntityObject *doomedObject = oo::test::NewRetained<::TestEntityObject>();
		::TestEntityObject *doomedLeaderObject = oo::test::NewRetained<::TestEntityObject>();
		ShipEntity *doomed = Ship(doomedObject);
		ShipEntity *doomedLeader = Ship(doomedLeaderObject);
		group->addShip(survivor);
		group->addShip(doomed);
		group->setLeader(doomedLeader);
		EXPECT_TRUE(group->count() == 3);

		oo::test::Release(doomedObject);
		oo::test::Release(doomedLeaderObject);
		EXPECT_TRUE(group->leader() == nullptr);
		EXPECT_TRUE(group->count() == 1 && !group->isEmpty());
		EXPECT_TRUE(HasMembers(group->memberArray(), { survivor }));
	}
}


TEST(OOShipGroup, cursor)
{
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::nullopt);
		std::vector<ShipEntity *> ships = { NewShip(), NewShip(), NewShip() };
		for (ShipEntity *ship : ships)  group->addShip(ship);

		std::vector<ShipEntity *> seen;
		oo::ShipGroupCursor cursor(group.get());
		while (ShipEntity *ship = cursor.next())  seen.push_back(ship);
		EXPECT_TRUE(seen == ships);	// internal order: the order added
		EXPECT_TRUE(cursor.index() == 3);

		oo::ShipGroupCursor mutated(group.get());
		EXPECT_TRUE(mutated.next() == ships[0]);
		group->addShip(NewShip());
		bool raised = false;
		try
		{
			mutated.next();
		}
		catch (const oo::Exception &)
		{
			raised = true;
		}
		EXPECT_TRUE(raised);
	}
}


TEST(OOShipGroup, description)
{
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::string("pirates"));
		group->addShip(NewShip());
		EXPECT_TRUE(group->descriptionComponents() == std::optional<std::string>("\"pirates\", 1 ships"));
		ShipEntity *leader = NewShip();
		group->setLeader(leader);
		std::optional<std::string> text = group->descriptionComponents();
		EXPECT_TRUE(text.has_value() && *text == "\"pirates\", 2 ships, leader: " + TestEntityObjectDescription(TestObjectOf(leader)));
		EXPECT_TRUE(oo::ShipGroup::groupWithName(std::nullopt)->descriptionComponents() == std::optional<std::string>("0 ships"));
	}
}


TEST(OOShipGroup, cxxGroup)
{
	{
		oo::AutoreleaseScope pool;
		ShipEntity *leader = NewShip(), *wingman = NewShip();
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::string("g"), leader);
		EXPECT_TRUE(group != nullptr && group->name() == std::optional<std::string>("g"));
		EXPECT_TRUE(group->leader() == leader && group->count() == 1);
		EXPECT_TRUE(group->addShip(wingman) && group->addShip(wingman) && group->count() == 2);
		EXPECT_TRUE(HasMembers(group->memberArray(), { leader, wingman }));
		EXPECT_TRUE(HasMembers(group->memberArrayExcludingLeader(), { wingman }));
		EXPECT_TRUE(group->descriptionComponents() == std::optional<std::string>("\"g\", 2 ships, leader: " + TestEntityObjectDescription(TestObjectOf(leader))));

		std::vector<ShipEntity *> seen;
		oo::ShipGroupCursor cursor(group.get());
		while (ShipEntity *ship = cursor.next())  seen.push_back(ship);
		EXPECT_TRUE(seen == std::vector<ShipEntity *>({ leader, wingman }));

		EXPECT_TRUE(group->removeShip(leader) && group->leader() == nullptr && wingman->setGroupCalls == 0);
		EXPECT_TRUE(leader->setGroupCalls == 1 && leader->lastOwner == leader);
		group->setName(std::nullopt);
		EXPECT_TRUE(!group->name().has_value() && !group->isEmpty());
	}
}


TEST(OOShipGroup, objectNode)
{
	// A group is its own PList Object node payload: the node gives the same group
  // back, and describes it as "%@" printed it.
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::string("node"));
		const oo::PList node = oo::ShipGroupObjectNode(group.get());
		EXPECT_TRUE(oo::ShipGroupInObjectNode(node) == group.get());
		EXPECT_TRUE(oo::ShipGroupObjectNode(nullptr).isNull() && oo::ShipGroupInObjectNode(oo::PList("node")) == nullptr);
		EXPECT_TRUE(group->className() == "OOShipGroup");
		const std::string text = group->description();
		EXPECT_TRUE(text.rfind("<OOShipGroup 0x", 0) == 0 && text.find(">{\"node\", 0 ships}") != std::string::npos);
		EXPECT_TRUE(oo::ShipGroup::groupWithName(std::nullopt) != oo::ShipGroup::groupWithName(std::nullopt));	// distinct groups, as before
	}
}


TEST(OOShipGroup, dyingGroupReleasesLeaderReference)
{
	// The group weak-retains its leader (setLeader) as it does each member
  // (addShip); its destructor must balance both, or every group that dies with
  // a leader leaks one weak-reference proxy. The references are std::weak_ptr
  // members, released with the group.
	::TestEntityObject *leaderObject = oo::test::NewRetained<::TestEntityObject>();
	::TestEntityObject *memberObject = oo::test::NewRetained<::TestEntityObject>();
	ShipEntity *leader = Ship(leaderObject), *member = Ship(memberObject);
	{
		oo::AutoreleaseScope pool;
		std::shared_ptr<oo::ShipGroup> group = oo::ShipGroup::groupWithName(std::string("doomed"));
		group->addShip(member);
		group->setLeader(leader);
		EXPECT_TRUE(group->leader() == leader && group->count() == 2);
	}
	oo::test::Release(leaderObject);
	oo::test::Release(memberObject);
}
