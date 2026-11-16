/*

EntityShaderBindings.m

Extra methods exposed for shader bindings.

The category Entity (ShaderBindings) is members of Entity (Entity.h), defined
here. The shader uniforms bind them through the entities' member table.


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

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "Entity.h"
#include "PlanetEntity.h"
#include "PlayerEntity.h"
#include "ProxyPlayerEntity.h"
#include "ShaderUniformMethodType.h"  // the shader binding member table
#include "ShipEntity.h"
#include "ValueType.h"
#include "VisualEffectEntity.h"
#include "WaypointEntity.h"

// -unsignedIntValue of the number a system-data query returned (nil: 0).
static unsigned UnsignedIntValueOf(const oo::PList& value) {
  return value.isNumber() ? static_cast<unsigned>(value.int64Value()) : 0;
}

// Clock time.
GLfloat Entity::clock() {
  return (PLAYER != nullptr ? static_cast<GLfloat>(PLAYER->clockTime()) : 0.0);
}

// System "flavour" numbers.
unsigned Entity::pseudoFixedD100() {
  return (PLAYER != nullptr ? PLAYER->systemPseudoRandom100() : unsigned{});
}

unsigned Entity::pseudoFixedD256() {
  return (PLAYER != nullptr ? PLAYER->systemPseudoRandom256() : unsigned{});
}

// System attributes.
unsigned Entity::systemGovernment() {
  return UnsignedIntValueOf(
      (PLAYER != nullptr ? PLAYER->systemGovernment_number() : oo::PList()));
}

unsigned Entity::systemEconomy() {
  return UnsignedIntValueOf(
      (PLAYER != nullptr ? PLAYER->systemEconomy_number() : oo::PList()));
}

unsigned Entity::systemTechLevel() {
  return UnsignedIntValueOf(
      (PLAYER != nullptr ? PLAYER->systemTechLevel_number() : oo::PList()));
}

unsigned Entity::systemPopulation() {
  return UnsignedIntValueOf(
      (PLAYER != nullptr ? PLAYER->systemPopulation_number() : oo::PList()));
}

unsigned Entity::systemProductivity() {
  return UnsignedIntValueOf(
      (PLAYER != nullptr ? PLAYER->systemProductivity_number() : oo::PList()));
}

#if OO_SHADERS || defined(OO_DEBUG_MONITOR_ENABLED)

/*	The entities' shader binding member table, its rows receiving the C++
   entity: one row per name a shader may bind on an entity
   (shader-uniform-bindings.plist and every other name whitelist.plist permits),
   each with its uniform type, whether it applies to this entity and its getter.
   laserColor has no method and no row.
*/
namespace {

// The parts the rows ask for.
ShipEntity* ShipPart(Entity* entity) {
  return dynamic_cast<ShipEntity*>(entity);
}

oo::VisualEffectEntity* EffectPart(Entity* entity) {
  return dynamic_cast<oo::VisualEffectEntity*>(entity);
}

PlayerEntity* PlayerEntityPart(Entity* entity) {
  return dynamic_cast<PlayerEntity*>(entity);
}

ProxyPlayerEntity* ProxyPart(Entity* entity) {
  return dynamic_cast<ProxyPlayerEntity*>(entity);
}

template <typename R>
void StoreShaderBindingValue(oo::ShaderBindingValue& out, R value) {
  if constexpr (std::is_same_v<R, Vector>)
    out.v = value;
  else if constexpr (std::is_same_v<R, HPVector>)
    out.hpv = value;
  else if constexpr (std::is_same_v<R, Quaternion>)
    out.q = value;
  else if constexpr (std::is_same_v<R, OOMatrix>)
    out.m = value;
  else if constexpr (std::is_same_v<R, NSPoint>)
    out.p = value;
  else if constexpr (std::is_same_v<R, OOColor*>)
    out.color = value;  // borrowed (fogUniform)
  else if constexpr (std::is_floating_point_v<R>)
    out.f = value;
  // bool (signed char): sign-extended, as the selector path's call returning
  // signed char was.
  else if constexpr (std::is_same_v<R, signed char>)
    out.i =
        value < 0
            ? static_cast<std::int64_t>(static_cast<unsigned char>(value)) - 256
            : static_cast<std::int64_t>(static_cast<unsigned char>(value));
  else
    out.i = static_cast<std::int64_t>(value);  // integers and enums
}

struct EntityShaderBindingRow {
  const char* name;
  oo::ValueType type;  // the method's return type's kind
  bool (*answers)(Entity* part);
  void (*get)(Entity* part, oo::ShaderBindingValue& out);
};

constexpr EntityShaderBindingRow kEntityShaderBindings[] = {
    {"position", oo::kValueType<HPVector>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> HPVector { return part->getPosition(); }());
     }},
    {"orientation", oo::kValueType<Quaternion>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> Quaternion { return part->getOrientation(); }());
     }},
    {"relativePosition", oo::kValueType<Vector>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> Vector { return part->relativePosition(); }());
     }},
    {"viewpointOffset", oo::kValueType<Vector>,
     [](Entity* part) -> bool { return PlayerEntityPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->viewpointOffset() : kZeroVector;
       }());
     }},
    {"collisionRadius", oo::kValueType<GLfloat>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->collisionRadius(); }());
     }},
    {"mass", oo::kValueType<GLfloat>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->getMass(); }());
     }},
    {"energy", oo::kValueType<GLfloat>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->getEnergy(); }());
     }},
    {"maxEnergy", oo::kValueType<GLfloat>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->getMaxEnergy(); }());
     }},
    {"universalTime", oo::kValueType<GLfloat>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->universalTime(); }());
     }},
    {"spawnTime", oo::kValueType<GLfloat>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> GLfloat { return part->getSpawnTime(); }());
     }},
    {"timeElapsedSinceSpawn", oo::kValueType<GLfloat>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         return part->timeElapsedSinceSpawn();
       }());
     }},
    {"throwingSparks", oo::kValueType<bool>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> bool { return part->throwingSparks(); }());
     }},
    {"clock", oo::kValueType<GLfloat>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out,
                               [part]() -> GLfloat { return part->clock(); }());
     }},
    {"pseudoFixedD100", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->pseudoFixedD100(); }());
     }},
    {"pseudoFixedD256", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->pseudoFixedD256(); }());
     }},
    {"systemGovernment", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->systemGovernment(); }());
     }},
    {"systemEconomy", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->systemEconomy(); }());
     }},
    {"systemTechLevel", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->systemTechLevel(); }());
     }},
    {"systemPopulation", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->systemPopulation(); }());
     }},
    {"systemProductivity", oo::kValueType<unsigned>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> unsigned { return part->systemProductivity(); }());
     }},
    {"isBeacon", oo::kValueType<bool>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::WaypointEntity*>(part) != nullptr ||
              ShipPart(part) != nullptr || EffectPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         if (ShipEntity* ship = ShipPart(part)) return ship->isBeacon();
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->isBeacon();
         oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(part);
         return waypoint != nullptr ? waypoint->isBeacon() : false;
       }());
     }},
    {"isFrangible", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getIsFrangible() : false;
       }());
     }},
    {"isCloaked", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->isCloaked() : false;
       }());
     }},
    {"isJammingScanning", oo::kValueType<bool>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::WaypointEntity*>(part) != nullptr ||
              ShipPart(part) != nullptr || EffectPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         if (ShipEntity* ship = ShipPart(part))
           return ship->isJammingScanning();
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->isJammingScanning();
         oo::WaypointEntity* waypoint = dynamic_cast<oo::WaypointEntity*>(part);
         return waypoint != nullptr ? waypoint->isJammingScanning() : false;
       }());
     }},
    {"hasMilitaryScannerFilter", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->hasMilitaryScannerFilter() : false;
       }());
     }},
    {"messageTime", oo::kValueType<double>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> double {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getMessageTime() : double{};
       }());
     }},
    {"escortCount", oo::kValueType<uint8_t>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> uint8_t {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->escortCount() : uint8_t{};
       }());
     }},
    {"hasHostileTarget", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->hasHostileTarget() : false;
       }());
     }},
    {"weaponRange", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getWeaponRange() : GLfloat{};
       }());
     }},
    {"scannerRange", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getScannerRange() : GLfloat{};
       }());
     }},
    {"withinStationAegis", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->withinStationAegis() : false;
       }());
     }},
    {"fuel", oo::kValueType<OOFuelQuantity>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> OOFuelQuantity {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getFuel() : OOFuelQuantity{};
       }());
     }},
    {"flightPitch", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getFlightPitch() : GLfloat{};
       }());
     }},
    {"flightRoll", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getFlightRoll() : GLfloat{};
       }());
     }},
    {"flightYaw", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getFlightYaw() : GLfloat{};
       }());
     }},
    {"flightSpeed", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getFlightSpeed() : GLfloat{};
       }());
     }},
    {"maxFlightSpeed", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getMaxFlightSpeed() : GLfloat{};
       }());
     }},
    {"speedFactor", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->speedFactor() : GLfloat{};
       }());
     }},
    {"damage", oo::kValueType<int>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->damage() : int{};
       }());
     }},
    {"laserHeatLevel", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->laserHeatLevel() : GLfloat{};
       }());
     }},
    {"hullHeatLevel", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool {
       return EffectPart(part) != nullptr || ShipPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->hullHeatLevel();
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->hullHeatLevel() : GLfloat{};
       }());
     }},
    {"entityPersonality", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->entityPersonality() : GLfloat{};
       }());
     }},
    {"entityPersonalityInt", oo::kValueType<GLint>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLint {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->entityPersonalityInt() : GLint{};
       }());
     }},
    {"numberOfScannedShips", oo::kValueType<int>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->numberOfScannedShips() : int{};
       }());
     }},
    {"destination", oo::kValueType<HPVector>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> HPVector {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->destination() : kZeroHPVector;
       }());
     }},
    {"rangeToDestination", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->rangeToDestination() : GLfloat{};
       }());
     }},
    {"rangeToPrimaryTarget", oo::kValueType<double>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> double {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->rangeToPrimaryTarget() : double{};
       }());
     }},
    {"isHulk", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->getIsHulk() : false;
       }());
     }},
    {"lightsActive", oo::kValueType<bool>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->lightsActive() : false;
       }());
     }},
    {"legalStatus", oo::kValueType<int>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->legalStatus() : int{};
       }());
     }},
    {"velocity", oo::kValueType<Vector>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> Vector { return part->getVelocity(); }());
     }},
    {"missileCapacity", oo::kValueType<std::size_t>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> std::size_t {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->missileCapacity() : std::size_t{};
       }());
     }},
    {"missileCount", oo::kValueType<std::size_t>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> std::size_t {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->missileCount() : std::size_t{};
       }());
     }},
    {"fuelLeakRate", oo::kValueType<float>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> float {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->fuelLeakRate();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->fuelLeakRate() : float{};
       }());
     }},
    {"massLocked", oo::kValueType<bool>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->massLocked();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->massLocked() : false;
       }());
     }},
    {"atHyperspeed", oo::kValueType<bool>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->atHyperspeed();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->atHyperspeed() : false;
       }());
     }},
    {"dialForwardShield", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->dialForwardShield();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->dialForwardShield() : GLfloat{};
       }());
     }},
    {"dialAftShield", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->dialAftShield();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->dialAftShield() : GLfloat{};
       }());
     }},
    {"dialMissileStatus", oo::kValueType<oo::MissileStatus>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> oo::MissileStatus {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->dialMissileStatus();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->dialMissileStatus()
                                  : oo::MissileStatus{};
       }());
     }},
    {"dialFuelScoopStatus", oo::kValueType<oo::FuelScoopStatus>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> oo::FuelScoopStatus {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->dialFuelScoopStatus();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->dialFuelScoopStatus()
                                  : oo::FuelScoopStatus{};
       }());
     }},
    {"compassMode", oo::kValueType<oo::CompassMode>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> oo::CompassMode {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->compassMode();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->getCompassMode()
                                  : oo::CompassMode{};
       }());
     }},
    {"dialIdentEngaged", oo::kValueType<bool>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> bool {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->dialIdentEngaged();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->dialIdentEngaged() : false;
       }());
     }},
    {"alertCondition", oo::kValueType<oo::AlertCondition>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> oo::AlertCondition {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->alertCondition() : oo::AlertCondition{};
       }());
     }},
    {"trumbleCount", oo::kValueType<std::size_t>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> std::size_t {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->trumbleCount();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->getTrumbleCount() : std::size_t{};
       }());
     }},
    // The other names whitelist.plist permits, each
    // with its return type, whether it applies and its getter.
    // The planet's five:
    {"airColorAsVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::PlanetEntity*>(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(part);
         return planet != nullptr ? planet->airColorAsVector() : kZeroVector;
       }());
     }},
    {"illuminationColorAsVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::PlanetEntity*>(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(part);
         return planet != nullptr ? planet->illuminationColorAsVector()
                                  : kZeroVector;
       }());
     }},
    {"airColorMixRatio", oo::kValueType<float>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::PlanetEntity*>(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> float {
         oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(part);
         return planet != nullptr ? planet->airColorMixRatio() : 0.0f;
       }());
     }},
    {"airDensity", oo::kValueType<float>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::PlanetEntity*>(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> float {
         oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(part);
         return planet != nullptr ? planet->airDensity() : 0.0f;
       }());
     }},
    {"terminatorThresholdVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return dynamic_cast<oo::PlanetEntity*>(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         oo::PlanetEntity* planet = dynamic_cast<oo::PlanetEntity*>(part);
         return planet != nullptr ? planet->terminatorThresholdVector()
                                  : kZeroVector;
       }());
     }},
    // The root's own:
    {"cameraRelativePosition", oo::kValueType<Vector>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         return part->getCameraRelativePosition();
       }());
     }},
    {"fogUniform", oo::kValueType<OOColor*>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(
           out, [part]() -> OOColor* { return part->fogUniform().get(); }());
     }},  // borrowed: the entity holds it
    // The ship family's, found by name (an effect's part answers the three
    // vectors too):
    {"forwardVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return EffectPart(part) != nullptr || ShipPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->forwardVector();
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->forwardVector() : kZeroVector;
       }());
     }},
    {"upVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return EffectPart(part) != nullptr || ShipPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->upVector();
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->upVector() : kZeroVector;
       }());
     }},
    {"rightVector", oo::kValueType<Vector>,
     [](Entity* part) -> bool {
       return EffectPart(part) != nullptr || ShipPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         if (oo::VisualEffectEntity* effect = EffectPart(part))
           return effect->rightVector();
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->rightVector() : kZeroVector;
       }());
     }},
    {"weaponRecoveryTime", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->weaponRecoveryTime() : GLfloat{};
       }());
     }},
    {"laserHeatLevelAft", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->laserHeatLevelAft() : GLfloat{};
       }());
     }},
    {"laserHeatLevelForward", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->laserHeatLevelForward() : GLfloat{};
       }());
     }},
    {"laserHeatLevelPort", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->laserHeatLevelPort() : GLfloat{};
       }());
     }},
    {"laserHeatLevelStarboard", oo::kValueType<GLfloat>,
     [](Entity* part) -> bool { return ShipPart(part) != nullptr; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         ShipEntity* ship = ShipPart(part);
         return ship != nullptr ? ship->laserHeatLevelStarboard() : GLfloat{};
       }());
     }},
    {"tradeInFactor", oo::kValueType<int>,
     [](Entity* part) -> bool {
       return ProxyPart(part) != nullptr || PlayerEntityPart(part) != nullptr;
     },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         if (ProxyPlayerEntity* proxy = ProxyPart(part))
           return proxy->tradeInFactor();
         PlayerEntity* player = PlayerEntityPart(part);
         return player != nullptr ? player->tradeInFactor() : int{};
       }());
     }},
    // The visual effect's, found by name: any entity answers these, 0 for any
    // but an effect's part.
    {"shaderFloat1", oo::kValueType<GLfloat>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderFloat1() : GLfloat{};
       }());
     }},
    {"shaderFloat2", oo::kValueType<GLfloat>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> GLfloat {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderFloat2() : GLfloat{};
       }());
     }},
    {"shaderInt1", oo::kValueType<int>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderInt1() : 0;
       }());
     }},
    {"shaderInt2", oo::kValueType<int>, [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> int {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderInt2() : 0;
       }());
     }},
    {"shaderVector1", oo::kValueType<Vector>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderVector1() : kZeroVector;
       }());
     }},
    {"shaderVector2", oo::kValueType<Vector>,
     [](Entity*) -> bool { return true; },
     [](Entity* part, oo::ShaderBindingValue& out) {
       StoreShaderBindingValue(out, [part]() -> Vector {
         oo::VisualEffectEntity* effect = EffectPart(part);
         return effect != nullptr ? effect->shaderVector2() : kZeroVector;
       }());
     }},
};

// The row's getter, given the uniform's target (the entity the lookup
// answered).
template <size_t I>
void GetShaderBinding(oo::Object* target, oo::ShaderBindingValue& out) {
  kEntityShaderBindings[I].get(static_cast<Entity*>(target), out);
}

template <size_t... I>
std::array<oo::ShaderMemberGetter, sizeof...(I)> ShaderBindingGetters(
    std::index_sequence<I...>) {
  return {&GetShaderBinding<I>...};
}

}  // namespace

bool EntityShaderMemberBindingFor(Entity* entity, std::string_view property,
                                  oo::ShaderMemberBinding* outBinding) {
  if (entity == nullptr || outBinding == nullptr) return false;

  static const std::unordered_map<std::string, size_t> rows = [] {
    std::unordered_map<std::string, size_t> result;
    for (size_t i = 0; i != std::size(kEntityShaderBindings); ++i)
      result.emplace(kEntityShaderBindings[i].name, i);
    return result;
  }();
  static const std::array<oo::ShaderMemberGetter,
                          std::size(kEntityShaderBindings)>
      getters = ShaderBindingGetters(
          std::make_index_sequence<std::size(kEntityShaderBindings)>());

  const auto found = rows.find(std::string(property));
  if (found == rows.end()) return false;
  const EntityShaderBindingRow& row = kEntityShaderBindings[found->second];
  if (!row.answers(entity)) return false;  // the selector path fails as it did
  const oo::ShaderUniformType type = oo::ShaderUniformTypeOf(row.type);
  if (type == oo::kShaderUniformTypeInvalid)
    return false;  // likewise ("unsupported type")
  *outBinding = {type, getters[found->second], entity};
  return true;
}

/*	What the uniforms and the materials ask of a binding target that is an
   entity, registered so that the materials link without the entities.
*/
namespace {

// The
// entity's own members, for an entity that has its object; nil's answers for
// one without.
Entity* ShaderSuperTarget(Entity* entity) {
  // The parent entity; nil's for an entity without its object.
  return (oo::WithObject(entity) != nullptr) ? entity->parentEntity() : nullptr;
}

std::string ShaderClassName(Entity* entity) {
  return (oo::WithObject(entity) != nullptr) ? std::string("Entity")
                                             : std::string("(null)");
}

std::string ShaderDescription(Entity* entity) {
  return oo::EntityDescription(entity);
}

bool ShaderMemberBinding(Entity* entity, const char* property,
                         oo::ShaderMemberBinding* outBinding) {
  if (oo::WithObject(entity) == nullptr) return false;  // nil bound nothing
  if (property == nullptr) return false;
  return EntityShaderMemberBindingFor(entity, property, outBinding);
}

bool ShaderRandomSeed(Entity* entity, uint32_t* outSeed) {
  // The ship family's, found by name (the by-name table's row), and a ship's
  // part's where there was no object to ask.
  ShipEntity* ship = ShipPart(entity);
  if (ship == nullptr) return false;
  *outSeed = ship->randomSeedForShaders();
  return true;
}

const bool sShaderEntityHooksRegistered =
    (oo::SetShaderEntityHooks({&ShaderSuperTarget, &ShaderClassName,
                               &ShaderDescription, &ShaderMemberBinding,
                               &ShaderRandomSeed}),
     true);

}  // namespace

#endif  // OO_SHADERS || defined(OO_DEBUG_MONITOR_ENABLED)
