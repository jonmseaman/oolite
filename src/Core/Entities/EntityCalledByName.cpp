/*

OOEntityCalledByName.cpp

An entity's method called by name, on its C++ part (declared in OOCallByName.h).
The by-name table is OOEntityCalledByNameTable.h: a tabled name is answered for
the C++ part it applies to, and called as OOCallByName calls the method (the
signature rules read from the declared types' encodings: @encode()'s, measured
literals). Any other name, and a tabled name that does not apply, are answered
from OOEntityObjectNamesTable.h. callObjC() reads a typed result through
oo::CallEntityByNameTyped. The table is the source: a tabled name that applies
but whose signature the call cannot use is logged here, and
oo::ScheduleEntityCall holds the deferred calls.

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#include "EntityCalledByName.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "AI.h"
#include "CallByName.h"
#include "DockEntity.h"
#include "Entity.h"
#include "GameController.h"  // OOScheduleDeferredCall (oo::ScheduleEntityCall, the player's selectors called by name)
#include "PlanetEntity.h"    // the root-entity rows
#include "PlayerEntity.h"
#include "ProxyPlayerEntity.h"
#include "QuiriumCascadeEntity.h"
#include "ShipEntity.h"
#include "ShipEntityAI.h"
#include "StationEntity.h"
#include "Universe.h"
#include "ValueType.h"
#include "VisualEffectEntity.h"
#include "WaypointEntity.h"
#include "oofnd/Log.hpp"

namespace {

// The parts the table's method bodies read.
ShipEntity* ShipPart(Entity* entity) {
  return dynamic_cast<ShipEntity*>(entity);
}

StationEntity* StationPart(Entity* entity) {
  return dynamic_cast<StationEntity*>(entity);
}

DockEntity* DockPart(Entity* entity) {
  return dynamic_cast<DockEntity*>(entity);
}

PlayerEntity* PlayerEntityPart(Entity* entity) {
  return dynamic_cast<PlayerEntity*>(entity);
}

ProxyPlayerEntity* ProxyPart(Entity* entity) {
  return dynamic_cast<ProxyPlayerEntity*>(entity);
}

oo::VisualEffectEntity* EffectPart(Entity* entity) {
  return dynamic_cast<oo::VisualEffectEntity*>(entity);
}

}  // namespace

// The generated table's row (OOEntityCalledByName.h declares the flags and the
// argument kinds).
namespace {

template <auto Function>
oo::PList EntityByNameAdaptNone(Entity* entity, const std::string*,
                                const oo::PList*) {
  if constexpr (std::is_same_v<decltype(Function(entity)), oo::PList>) {
    return Function(entity);
  } else {
    (void)Function(entity);
    return oo::PList();
  }
}

template <auto Function>
oo::PList EntityByNameAdaptString(Entity* entity, const std::string* string,
                                  const oo::PList*) {
  if constexpr (std::is_same_v<decltype(Function(entity, *string)),
                               oo::PList>) {
    return Function(entity, *string);
  } else {
    (void)Function(entity, *string);
    return oo::PList();
  }
}

template <auto Function>
oo::PList EntityByNameAdaptPList(Entity* entity, const std::string*,
                                 const oo::PList* plist) {
  if constexpr (std::is_same_v<decltype(Function(entity, *plist)), oo::PList>) {
    return Function(entity, *plist);
  } else {
    (void)Function(entity, *plist);
    return oo::PList();
  }
}

template <auto Function>
void EntityByNameTyped(Entity* entity, oo::EntityByNameValue& value) {
  using Result = decltype(Function(entity));
  if constexpr (std::is_void_v<Result>) {
    Function(entity);
  } else if constexpr (std::is_same_v<Result, oo::PList>) {
    value.plist = Function(entity);
  } else if constexpr (std::is_integral_v<Result> && std::is_signed_v<Result> &&
                       sizeof(Result) == 1) {
    // a signed char (bool): its value, sign kept, as OOCallIntegerMethod()
    // converted it
    const std::int64_t bits = static_cast<unsigned char>(Function(entity));
    value.integer = (bits > 127) ? bits - 256 : bits;
  } else if constexpr (std::is_integral_v<Result> || std::is_enum_v<Result>) {
    value.integer = static_cast<std::int64_t>(Function(entity));
  } else if constexpr (std::is_floating_point_v<Result>) {
    value.real = static_cast<double>(Function(entity));
  } else if constexpr (std::is_same_v<Result, Vector>) {
    value.vector = Function(entity);
  } else if constexpr (std::is_same_v<Result, Quaternion>) {
    value.quaternion = Function(entity);
  } else if constexpr (std::is_same_v<Result, oo::Color*>) {
    value.color = Function(entity);  // borrowed
  } else if constexpr (std::is_convertible_v<Result, Entity*>) {
    value.entity = Function(entity);  // the C++ entity
  } else {
    (void)Function(entity);  // a result callObjC() cannot read (it does not
                             // call such a method)
  }
}

}  // namespace

#include "EntityCalledByNameTable.h"

/*	The root entity's own names that callObjC() lists and the by-name table
   does not, so callObjC() asks and calls an entity's C++ part for every name of
   its table. Each function is the method's body on the C++ entity, after a
   guard for no entity (zero; the calls never pass one); a name whose result
        callObjC() cannot read (a string, a vector of objects, a matrix, a
   high-precision vector, a bounding box: "cannot be called", or no result) has
   no function. Every entity answers them but for the planet's shader selectors
   (a planet's part) and the beacon protocol's (a ship's, an effect's or a
   waypoint's part). The entity results (nextBeacon, prevBeacon, parentEntity,
   rootShipEntity) are the C++ entity: callObjC() makes its Object node.
*/
namespace {

bool AnswersPlanet(Entity* entity) {
  return dynamic_cast<oo::PlanetEntity*>(entity) != nullptr;
}

bool AnswersBeacon(Entity* entity) {
  return dynamic_cast<oo::WaypointEntity*>(entity) != nullptr ||
         ShipPart(entity) != nullptr || EffectPart(entity) != nullptr;
}

void Facade_addToLinkedLists(Entity* entity) {
  if (entity == nullptr) return;
  entity->addToLinkedLists();
}

Vector Facade_airColorAsVector(Entity* entity) {
  if (entity == nullptr) return {};
  oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(entity);
  return planet != nullptr ? planet->airColorAsVector() : kZeroVector;
}

float Facade_airColorMixRatio(Entity* entity) {
  if (entity == nullptr) return {};
  oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(entity);
  return planet != nullptr ? planet->airColorMixRatio() : 0.0f;
}

float Facade_airDensity(Entity* entity) {
  if (entity == nullptr) return {};
  oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(entity);
  return planet != nullptr ? planet->airDensity() : 0.0f;
}

oo::HUDBeaconIcon* Facade_beaconDrawable(Entity* entity) {
  if (entity == nullptr) return {};
  if (ShipEntity* ship = ShipPart(entity)) return ship->beaconDrawable();
  if (oo::VisualEffectEntity* effect = EffectPart(entity))
    return effect->beaconDrawable();
  oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(entity);
  return waypoint != nullptr ? waypoint->beaconDrawable() : nullptr;
}

double Facade_camZeroDistance(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->camZeroDistance();
}

GLfloat Facade_cameraRangeBack(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->cameraRangeBack();
}

GLfloat Facade_cameraRangeFront(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->cameraRangeFront();
}

Vector Facade_cameraRelativePosition(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getCameraRelativePosition();
}

bool Facade_canCollide(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->canCollide();
}

GLfloat Facade_collisionRadius(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->collisionRadius();
}

void Facade_deleteJSSelf(Entity* entity) {
  if (entity == nullptr) return;
  entity->deleteJSSelf();
}

GLfloat Facade_distanceTravelled(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getDistanceTravelled();
}

void Facade_dumpSelfState(Entity* entity) {
  if (entity == nullptr) return;
  entity->dumpSelfState();
}

void Facade_dumpState(Entity* entity) {
  if (entity == nullptr) return;
  entity->dumpState();
}

GLfloat Facade_energy(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getEnergy();
}

double Facade_findCollisionRadius(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->findCollisionRadius();
}

oo::Color* Facade_fogUniform(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->fogUniform().get();
}

GLfloat Facade_frustumRadius(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->frustumRadius();
}

Vector Facade_illuminationColorAsVector(Entity* entity) {
  if (entity == nullptr) return {};
  oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(entity);
  return planet != nullptr ? planet->illuminationColorAsVector() : kZeroVector;
}

bool Facade_isBeacon(Entity* entity) {
  if (entity == nullptr) return {};
  if (ShipEntity* ship = ShipPart(entity)) return ship->isBeacon();
  if (oo::VisualEffectEntity* effect = EffectPart(entity))
    return effect->isBeacon();
  oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(entity);
  return waypoint != nullptr ? waypoint->isBeacon() : false;
}

bool Facade_isCascadeWeapon(Entity* entity) {
  if (entity == nullptr) return {};
  oo::QuiriumCascadeEntity* cascade =
      dynamic_cast<oo::QuiriumCascadeEntity*>(entity);
  return cascade != nullptr ? cascade->isCascadeWeapon() : false;
}

bool Facade_isDock(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isDock();
}

bool Facade_isEffect(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isEffect();
}

bool Facade_isImmuneToBreakPatternHide(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsImmuneToBreakPatternHide();
}

bool Facade_isInSpace(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isInSpace();
}

bool Facade_isJammingScanning(Entity* entity) {
  if (entity == nullptr) return {};
  if (ShipEntity* ship = ShipPart(entity)) return ship->isJammingScanning();
  if (oo::VisualEffectEntity* effect = EffectPart(entity))
    return effect->isJammingScanning();
  oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(entity);
  return waypoint != nullptr ? waypoint->isJammingScanning() : false;
}

bool Facade_isPlanet(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isPlanet();
}

bool Facade_isPlayer(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsPlayer();
}

bool Facade_isShip(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsShip();
}

bool Facade_isSky(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isSky();
}

bool Facade_isStation(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsStation();
}

bool Facade_isStellarObject(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isStellarObject();
}

bool Facade_isSubEntity(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsSubEntity();
}

bool Facade_isSun(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isSun();
}

bool Facade_isSunlit(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsSunlit();
}

bool Facade_isVisible(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isVisible();
}

bool Facade_isVisibleToScripts(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isVisibleToScripts();
}

bool Facade_isVisualEffect(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsVisualEffect();
}

bool Facade_isWaypoint(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->isWaypoint();
}

bool Facade_isWormhole(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getIsWormhole();
}

std::size_t Facade_lastDrawCounter(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getLastDrawCounter();
}

GLfloat Facade_mass(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getMass();
}

GLfloat Facade_maxEnergy(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getMaxEnergy();
}

Entity* Facade_nextBeacon(Entity* entity) {
  if (entity == nullptr) return {};
  if (ShipEntity* ship = ShipPart(entity)) return ship->nextBeacon();
  if (oo::VisualEffectEntity* effect = EffectPart(entity))
    return effect->nextBeacon();
  oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(entity);
  return waypoint != nullptr ? waypoint->nextBeacon() : nullptr;
}

Quaternion Facade_normalOrientation(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->normalOrientation();
}

Quaternion Facade_orientation(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getOrientation();
}

void Facade_orientationChanged(Entity* entity) {
  if (entity == nullptr) return;
  entity->orientationChanged();
}

Entity* Facade_owner(Entity* entity) {
  // the owner's C++ entity
  if (entity == nullptr) return {};
  return entity->owner();
}

Entity* Facade_parentEntity(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->parentEntity();
}

Entity* Facade_prevBeacon(Entity* entity) {
  if (entity == nullptr) return {};
  if (ShipEntity* ship = ShipPart(entity)) return ship->prevBeacon();
  if (oo::VisualEffectEntity* effect = EffectPart(entity))
    return effect->prevBeacon();
  oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(entity);
  return waypoint != nullptr ? waypoint->prevBeacon() : nullptr;
}

Vector Facade_relativePosition(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->relativePosition();
}

void Facade_removeFromLinkedLists(Entity* entity) {
  if (entity == nullptr) return;
  entity->removeFromLinkedLists();
}

Entity* Facade_rootShipEntity(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->rootShipEntity();
}

oo::ScanClass Facade_scanClass(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getScanClass();
}

std::size_t Facade_sessionID(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->sessionID();
}

GLfloat Facade_spawnTime(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getSpawnTime();
}

double Facade_speed(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->speed();
}

oo::EntityStatus Facade_status(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->status();
}

Entity* Facade_superShaderBindingTarget(Entity* entity) {
  // the parent's C++ entity
  if (entity == nullptr) return {};
  return entity->superShaderBindingTarget();
}

Vector Facade_terminatorThresholdVector(Entity* entity) {
  if (entity == nullptr) return {};
  oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(entity);
  return planet != nullptr ? planet->terminatorThresholdVector() : kZeroVector;
}

void Facade_throwSparks(Entity* entity) {
  if (entity == nullptr) return;
  entity->throwSparks();
}

bool Facade_throwingSparks(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->throwingSparks();
}

GLfloat Facade_timeElapsedSinceSpawn(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->timeElapsedSinceSpawn();
}

oo::UniversalID Facade_universalID(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getUniversalID();
}

GLfloat Facade_universalTime(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->universalTime();
}

void Facade_updateCameraRelativePosition(Entity* entity) {
  if (entity == nullptr) return;
  entity->updateCameraRelativePosition();
}

void Facade_updateLinkedLists(Entity* entity) {
  if (entity == nullptr) return;
  entity->updateLinkedLists();
}

bool Facade_validForAddToUniverse(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->validForAddToUniverse();
}

Vector Facade_velocity(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->getVelocity();
}

void Facade_warnAboutHostiles(Entity* entity) {
  if (entity == nullptr) return;
  entity->warnAboutHostiles();
}

void Facade_wasAddedToUniverse(Entity* entity) {
  if (entity == nullptr) return;
  entity->wasAddedToUniverse();
}

void Facade_wasRemovedFromUniverse(Entity* entity) {
  if (entity == nullptr) return;
  entity->wasRemovedFromUniverse();
}

double Facade_zeroDistance(Entity* entity) {
  if (entity == nullptr) return {};
  return entity->zeroDistance();
}

const oo::EntityFacadeRow kEntityFacadeRows[] = {
    {"absolutePositionForSubentity", nullptr, nullptr},
    {"addToLinkedLists", nullptr, &EntityByNameTyped<&Facade_addToLinkedLists>},
    {"airColorAsVector", &AnswersPlanet,
     &EntityByNameTyped<&Facade_airColorAsVector>},
    {"airColorMixRatio", &AnswersPlanet,
     &EntityByNameTyped<&Facade_airColorMixRatio>},
    {"airDensity", &AnswersPlanet, &EntityByNameTyped<&Facade_airDensity>},
    {"beaconCode", &AnswersBeacon, nullptr},
    {"beaconDrawable", &AnswersBeacon,
     &EntityByNameTyped<&Facade_beaconDrawable>},
    {"beaconLabel", &AnswersBeacon, nullptr},
    {"boundingBox", nullptr, nullptr},
    {"camZeroDistance", nullptr, &EntityByNameTyped<&Facade_camZeroDistance>},
    {"cameraRangeBack", nullptr, &EntityByNameTyped<&Facade_cameraRangeBack>},
    {"cameraRangeFront", nullptr, &EntityByNameTyped<&Facade_cameraRangeFront>},
    {"cameraRelativePosition", nullptr,
     &EntityByNameTyped<&Facade_cameraRelativePosition>},
    {"canCollide", nullptr, &EntityByNameTyped<&Facade_canCollide>},
    {"collisionRadius", nullptr, &EntityByNameTyped<&Facade_collisionRadius>},
#ifdef OO_DEBUG_MONITOR_ENABLED
    {"allTextures", nullptr, nullptr},
#endif
    {"collidingEntities", nullptr, nullptr},
    {"descriptionComponents", nullptr, nullptr},
    {"oo_jsClassName", nullptr, nullptr},
    {"deleteJSSelf", nullptr, &EntityByNameTyped<&Facade_deleteJSSelf>},
#ifdef OO_DEBUG_MONITOR_ENABLED
    {"descriptionForObjDump", nullptr, nullptr},
#endif
#ifdef OO_DEBUG_MONITOR_ENABLED
    {"descriptionForObjDumpBasic", nullptr, nullptr},
#endif
    {"distanceTravelled", nullptr,
     &EntityByNameTyped<&Facade_distanceTravelled>},
    {"drawRotationMatrix", nullptr, nullptr},
    {"drawTransformationMatrix", nullptr, nullptr},
    {"dumpSelfState", nullptr, &EntityByNameTyped<&Facade_dumpSelfState>},
    {"dumpState", nullptr, &EntityByNameTyped<&Facade_dumpState>},
    {"energy", nullptr, &EntityByNameTyped<&Facade_energy>},
    {"findCollisionRadius", nullptr,
     &EntityByNameTyped<&Facade_findCollisionRadius>},
    {"fogUniform", nullptr, &EntityByNameTyped<&Facade_fogUniform>},
    {"frustumRadius", nullptr, &EntityByNameTyped<&Facade_frustumRadius>},
    {"illuminationColorAsVector", &AnswersPlanet,
     &EntityByNameTyped<&Facade_illuminationColorAsVector>},
    {"isBeacon", &AnswersBeacon, &EntityByNameTyped<&Facade_isBeacon>},
    {"isCascadeWeapon", nullptr, &EntityByNameTyped<&Facade_isCascadeWeapon>},
    {"isDock", nullptr, &EntityByNameTyped<&Facade_isDock>},
    {"isEffect", nullptr, &EntityByNameTyped<&Facade_isEffect>},
    {"isImmuneToBreakPatternHide", nullptr,
     &EntityByNameTyped<&Facade_isImmuneToBreakPatternHide>},
    {"isInSpace", nullptr, &EntityByNameTyped<&Facade_isInSpace>},
    {"isJammingScanning", &AnswersBeacon,
     &EntityByNameTyped<&Facade_isJammingScanning>},
    {"isPlanet", nullptr, &EntityByNameTyped<&Facade_isPlanet>},
    {"isPlayer", nullptr, &EntityByNameTyped<&Facade_isPlayer>},
    {"isShip", nullptr, &EntityByNameTyped<&Facade_isShip>},
    {"isSky", nullptr, &EntityByNameTyped<&Facade_isSky>},
    {"isStation", nullptr, &EntityByNameTyped<&Facade_isStation>},
    {"isStellarObject", nullptr, &EntityByNameTyped<&Facade_isStellarObject>},
    {"isSubEntity", nullptr, &EntityByNameTyped<&Facade_isSubEntity>},
    {"isSun", nullptr, &EntityByNameTyped<&Facade_isSun>},
    {"isSunlit", nullptr, &EntityByNameTyped<&Facade_isSunlit>},
    {"isVisible", nullptr, &EntityByNameTyped<&Facade_isVisible>},
    {"isVisibleToScripts", nullptr,
     &EntityByNameTyped<&Facade_isVisibleToScripts>},
    {"isVisualEffect", nullptr, &EntityByNameTyped<&Facade_isVisualEffect>},
    {"isWaypoint", nullptr, &EntityByNameTyped<&Facade_isWaypoint>},
    {"isWormhole", nullptr, &EntityByNameTyped<&Facade_isWormhole>},
    {"lastDrawCounter", nullptr, &EntityByNameTyped<&Facade_lastDrawCounter>},
    {"mass", nullptr, &EntityByNameTyped<&Facade_mass>},
    {"maxEnergy", nullptr, &EntityByNameTyped<&Facade_maxEnergy>},
    {"nextBeacon", &AnswersBeacon, &EntityByNameTyped<&Facade_nextBeacon>},
    {"normalOrientation", nullptr,
     &EntityByNameTyped<&Facade_normalOrientation>},
    {"orientation", nullptr, &EntityByNameTyped<&Facade_orientation>},
    {"orientationChanged", nullptr,
     &EntityByNameTyped<&Facade_orientationChanged>},
    {"owner", nullptr, &EntityByNameTyped<&Facade_owner>},
    {"parentEntity", nullptr, &EntityByNameTyped<&Facade_parentEntity>},
    {"position", nullptr, nullptr},
    {"prevBeacon", &AnswersBeacon, &EntityByNameTyped<&Facade_prevBeacon>},
    {"relativePosition", nullptr, &EntityByNameTyped<&Facade_relativePosition>},
    {"removeFromLinkedLists", nullptr,
     &EntityByNameTyped<&Facade_removeFromLinkedLists>},
    {"rootShipEntity", nullptr, &EntityByNameTyped<&Facade_rootShipEntity>},
    {"rotationMatrix", nullptr, nullptr},
    {"scanClass", nullptr, &EntityByNameTyped<&Facade_scanClass>},
    {"sessionID", nullptr, &EntityByNameTyped<&Facade_sessionID>},
    {"spawnTime", nullptr, &EntityByNameTyped<&Facade_spawnTime>},
    {"speed", nullptr, &EntityByNameTyped<&Facade_speed>},
    {"status", nullptr, &EntityByNameTyped<&Facade_status>},
    {"superShaderBindingTarget", nullptr,
     &EntityByNameTyped<&Facade_superShaderBindingTarget>},
    {"terminatorThresholdVector", &AnswersPlanet,
     &EntityByNameTyped<&Facade_terminatorThresholdVector>},
    {"throwSparks", nullptr, &EntityByNameTyped<&Facade_throwSparks>},
    {"throwingSparks", nullptr, &EntityByNameTyped<&Facade_throwingSparks>},
    {"timeElapsedSinceSpawn", nullptr,
     &EntityByNameTyped<&Facade_timeElapsedSinceSpawn>},
    {"transformationMatrix", nullptr, nullptr},
    {"universalID", nullptr, &EntityByNameTyped<&Facade_universalID>},
    {"universalTime", nullptr, &EntityByNameTyped<&Facade_universalTime>},
    {"updateCameraRelativePosition", nullptr,
     &EntityByNameTyped<&Facade_updateCameraRelativePosition>},
    {"updateLinkedLists", nullptr,
     &EntityByNameTyped<&Facade_updateLinkedLists>},
    {"validForAddToUniverse", nullptr,
     &EntityByNameTyped<&Facade_validForAddToUniverse>},
    {"velocity", nullptr, &EntityByNameTyped<&Facade_velocity>},
    {"warnAboutHostiles", nullptr,
     &EntityByNameTyped<&Facade_warnAboutHostiles>},
    {"wasAddedToUniverse", nullptr,
     &EntityByNameTyped<&Facade_wasAddedToUniverse>},
    {"wasRemovedFromUniverse", nullptr,
     &EntityByNameTyped<&Facade_wasRemovedFromUniverse>},
    {"zeroDistance", nullptr, &EntityByNameTyped<&Facade_zeroDistance>},
};

#include "EntityObjectNamesTable.h"

}  // namespace

std::span<const oo::EntityObjectName> oo::EntityObjectNames() {
  return kEntityObjectNames;
}

const oo::EntityObjectName* oo::EntityObjectNameNamed(const std::string& name) {
  static const std::unordered_map<std::string_view, const oo::EntityObjectName*>
      names = [] {
        std::unordered_map<std::string_view, const oo::EntityObjectName*>
            result;
        for (const oo::EntityObjectName& row : kEntityObjectNames)
          result.emplace(row.name, &row);
        return result;
      }();
  auto found = names.find(name);
  return found != names.end() ? found->second : nullptr;
}

std::span<const oo::EntityFacadeRow> oo::EntityFacadeRows() {
  return kEntityFacadeRows;
}

const oo::EntityFacadeRow* oo::EntityFacadeRowNamed(const std::string& name) {
  static const std::unordered_map<std::string_view, const oo::EntityFacadeRow*>
      rows = [] {
        std::unordered_map<std::string_view, const oo::EntityFacadeRow*> result;
        for (const oo::EntityFacadeRow& row : kEntityFacadeRows)
          result.emplace(row.name, &row);
        return result;
      }();
  auto found = rows.find(name);
  return found != rows.end() ? found->second : nullptr;
}

std::span<const oo::EntityByNameRow> oo::EntityByNameRows() {
  return oo::kEntityByNameRows;
}

const oo::EntityByNameRow* oo::EntityByNameRowNamed(const std::string& name) {
  static const std::unordered_map<std::string_view, const oo::EntityByNameRow*>
      rows = [] {
        std::unordered_map<std::string_view, const oo::EntityByNameRow*> result;
        for (const oo::EntityByNameRow& row : oo::kEntityByNameRows)
          result.emplace(row.name, &row);
        return result;
      }();
  auto found = rows.find(name);
  return found != rows.end() ? found->second : nullptr;
}

// The name sets (the visual
// effect's first). A name only the effect's set holds is answered by the root
// for any entity; its body answers zero for any part
// but an effect's.
bool oo::EntityByNameRowApplies(const oo::EntityByNameRow& row,
                                Entity* entity) {
  if (entity == nullptr) return false;
  const oo::ByName flags = row.flags;
  if (oo::Any(flags & oo::ByName::kEffect) && EffectPart(entity) != nullptr)
    return true;
  if (!oo::Any(flags & (oo::ByName::kShip | oo::ByName::kPlayer |
                        oo::ByName::kStation | oo::ByName::kDock)))
    return true;
  if (oo::Any(flags & oo::ByName::kShip) && ShipPart(entity) != nullptr)
    return true;
  if (oo::Any(flags & oo::ByName::kPlayer) &&
      oo::Any(flags & oo::ByName::kProxy) && ProxyPart(entity) != nullptr)
    return true;
  if (oo::Any(flags & oo::ByName::kPlayer) &&
      PlayerEntityPart(entity) != nullptr)
    return true;
  return (oo::Any(flags & oo::ByName::kStation) &&
          StationPart(entity) != nullptr) ||
         (oo::Any(flags & oo::ByName::kDock) && DockPart(entity) != nullptr);
}

bool oo::CallEntityByNameTyped(Entity* entity, const std::string& name,
                               oo::EntityByNameValue& value) {
  if (entity == nullptr) return false;
  if (const oo::EntityByNameRow* row = EntityByNameRowNamed(name)) {
    if (row->typed == nullptr || !EntityByNameRowApplies(*row, entity))
      return false;
    row->typed(entity, value);
    return true;
  }
  // A root-entity row: the method's body, for an entity that answers it.
  const oo::EntityFacadeRow* facade = EntityFacadeRowNamed(name);
  if (facade == nullptr || facade->typed == nullptr ||
      (facade->answers != nullptr && !facade->answers(entity)))
    return false;
  facade->typed(entity, value);
  return true;
}

namespace {

enum class Kind : std::uint8_t {
  kVoid,    // a void result, or a scalar result the call ignores
  kPList,   // an oo::PList result
  kObject,  // an object result, ignored
  kUnsupported
};

// Classifies a method's return type as the Objective-C dispatcher did: a
// scalar result (bool to double, enums included) is ignored like a void one.
Kind ResultKind(oo::ValueType type) {
  switch (type) {
    case oo::ValueType::kVoid:
    case oo::ValueType::kBool:
    case oo::ValueType::kChar:
    case oo::ValueType::kUChar:
    case oo::ValueType::kShort:
    case oo::ValueType::kUShort:
    case oo::ValueType::kInt:
    case oo::ValueType::kUInt:
    case oo::ValueType::kLong:
    case oo::ValueType::kULong:
    case oo::ValueType::kLongLong:
    case oo::ValueType::kULongLong:
    case oo::ValueType::kFloat:
    case oo::ValueType::kDouble:
      return Kind::kVoid;
    case oo::ValueType::kObject:
      return Kind::kObject;
    case oo::ValueType::kPList:
      return Kind::kPList;
    case oo::ValueType::kVector:
    case oo::ValueType::kHPVector:
    case oo::ValueType::kQuaternion:
    case oo::ValueType::kMatrix:
    case oo::ValueType::kPoint:
    case oo::ValueType::kRect:
    case oo::ValueType::kBoundingBox:
    case oo::ValueType::kColor:
    case oo::ValueType::kString:
    case oo::ValueType::kOptionalString:
    case oo::ValueType::kOther:
      return Kind::kUnsupported;
  }
  return Kind::kUnsupported;
}

// The tabled row the name names, if it applies to the entity; null for any
// other name.
const oo::EntityByNameRow* ApplyingRow(Entity* entity,
                                       const std::string& name) {
  const oo::EntityByNameRow* row = oo::EntityByNameRowNamed(name);
  return (row != nullptr && oo::EntityByNameRowApplies(*row, entity)) ? row
                                                                      : nullptr;
}

// The row's call with the argument, if OOCallByName would make it (its usable
// signatures); none if the signature is unusable.
std::optional<oo::PList> CallRow(const oo::EntityByNameRow& row, Entity* entity,
                                 const std::string* string,
                                 const oo::PList* plist) {
  const Kind result = ResultKind(row.result);
  const bool hasArgument = (string != nullptr || plist != nullptr);
  if (hasArgument != (row.argument != oo::EntityByNameArgument::kNone) ||
      result == Kind::kUnsupported)
    return std::nullopt;

  // A string reaches a PList parameter as a string PList; a PList reaches a
  // string parameter only when it is a string.
  oo::PList stringAsPList;
  if (row.argument == oo::EntityByNameArgument::kString && plist != nullptr) {
    string = plist->getIf<std::string>();
    if (string == nullptr) return std::nullopt;
  }
  if (row.argument == oo::EntityByNameArgument::kPList) {
    if (result != Kind::kVoid && result != Kind::kObject) return std::nullopt;
    if (string != nullptr) {
      stringAsPList = oo::PList(*string);
      plist = &stringAsPList;
    }
  }

  oo::PList answer = row.call(entity, string, plist);
  // only a PList result is the call's result; a void, scalar or object result
  // is ignored
  return (result == Kind::kPList) ? std::move(answer) : oo::PList();
}

/*	The class OOCallByName's lines name: the entity's object's, of class
 * Entity (not asked).
 */
std::string_view ObjectClassName(Entity*) { return "Entity"; }

/*	The line OOCallByName logged for a method whose signature the call
   cannot use (LogBadSignature), for a tabled name that applies:
   the class of the entity's object, as the category method was its, and the
   kind of the row's result.
*/
void LogBadSignature(Entity* entity, const oo::EntityByNameRow& row) {
  OO_LOG_ERR("callByName.badSignature",
             "-[{} {}] has no signature a by-name call can use (returns {}).",
             ObjectClassName(entity), row.name, oo::ValueTypeName(row.result));
}

// A tabled name that applies, called or logged; none for any other name (the
// object's path).
std::optional<oo::PList> CallTabled(Entity* entity, const std::string& name,
                                    const std::string* string,
                                    const oo::PList* plist) {
  const oo::EntityByNameRow* row = ApplyingRow(entity, name);
  if (row == nullptr) return std::nullopt;
  if (std::optional<oo::PList> result = CallRow(*row, entity, string, plist))
    return result;
  LogBadSignature(entity, *row);
  return oo::PList();
}

// The name, if the entity's object answered it for this entity; else null.
const oo::EntityObjectName* AnsweredObjectName(Entity* entity,
                                               const std::string& name) {
  const oo::EntityObjectName* row = oo::EntityObjectNameNamed(name);
  return (row != nullptr && (row->answers == nullptr || row->answers(entity)))
             ? row
             : nullptr;
}

/*	A C++ entity's name outside the by-name table: a root-entity row's
   method with no argument that the call called is the row's body (its result
   ignored); any other name the object answers is logged (the method's type
   encoding), a root-class or lifetime method included, which is never called;
   any other name does not respond. The texts are the Objective-C dispatcher's
   (its unknown-selector and bad-signature lines).
*/
oo::PList CallObjectName(Entity* entity, const std::string& name,
                         bool hasArgument) {
  const oo::EntityObjectName* row = AnsweredObjectName(entity, name);
  if (row == nullptr) {
    OO_LOG_ERR("callByName.unknownSelector", "{} does not respond to {}.",
               ObjectClassName(entity), name);
    return oo::PList();
  }
  if (row->kind == oo::EntityObjectNameKind::kCalled && !hasArgument) {
    const oo::EntityFacadeRow* facade = oo::EntityFacadeRowNamed(name);
    if (facade != nullptr && facade->typed != nullptr) {
      oo::EntityByNameValue ignored;
      facade->typed(entity, ignored);
      return oo::PList();
    }
  }
  OO_LOG_ERR(
      "callByName.badSignature",
      "-[{} {}] has no signature a by-name call can use (type encoding {}).",
      ObjectClassName(entity), name, row->encoding);
  return oo::PList();
}

}  // namespace

bool oo::EntityRespondsToName(Entity* entity, const std::string& name) {
  if (entity == nullptr) return false;
  if (const oo::EntityByNameRow* row = EntityByNameRowNamed(name))
    return EntityByNameRowApplies(*row, entity);
  // A root-entity row's answer.
  if (const oo::EntityFacadeRow* facade = EntityFacadeRowNamed(name))
    return facade->answers == nullptr || facade->answers(entity);
  // Any other name the entity's object answered: the object is not asked.
  return AnsweredObjectName(entity, name) != nullptr;
}

oo::PList oo::CallEntityByName(Entity* entity, const std::string& name) {
  if (entity == nullptr) return PList();
  if (std::optional<PList> result = CallTabled(entity, name, nullptr, nullptr))
    return *result;
  return CallObjectName(entity, name, false);
}

oo::PList oo::CallEntityByName(Entity* entity, const std::string& name,
                               const std::string& argument) {
  if (entity == nullptr) return PList();
  if (std::optional<PList> result =
          CallTabled(entity, name, &argument, nullptr))
    return *result;
  return CallObjectName(entity, name, true);
}

oo::PList oo::CallEntityByName(Entity* entity, const std::string& name,
                               const PList& argument) {
  if (entity == nullptr) return PList();
  if (std::optional<PList> result =
          CallTabled(entity, name, nullptr, &argument))
    return *result;
  return CallObjectName(entity, name, true);
}

void oo::ScheduleEntityCall(Entity* entity, void (*call)(Entity* entity),
                            double delay) {
  if (entity == nullptr || call == nullptr) return;
  // The call holds the entity, so it keeps the entity alive until it has run.
  std::shared_ptr<Entity> held = oo::share(entity);
  oo::ScheduleDeferredCall([held, call]() { call(held.get()); }, delay);
}
