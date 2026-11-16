// clang-format off: a table, laid out by hand
/*

OOEntityCalledByNameTable.h

The by-name table for the ship, player, station, dock and visual effect entities. It was generated
from the last Objective-C++ build and is maintained by hand now: this file is the source. Edit a
function and its row together.

One row per method: the name, the applies-to flags, the encoding of the declared result
(oo::kValueType<>, the kind of the result type: ValueType.h),
the argument kind, the adaptor: the method's body, verbatim, with the entity as its C++
entity, and for a method with no argument the typed adaptor (its result by type).
Included by OOEntityCalledByName.cpp only, after the parts (ShipPart() ...) the bodies read.

*/

#pragma once

namespace {

// The category methods, as functions of the C++ entity.

bool ByName_0(Entity *entity, const oo::PList &inShipDict)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->setUpFromDictionary(inShipDict) : false; }
bool ByName_1(Entity *entity, const oo::PList &shipDict)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->setUpShipFromDictionary(shipDict) : false; }
std::size_t ByName_2(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->subIdx() : std::size_t{}; }
std::size_t ByName_3(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxShipSubEntities() : std::size_t{}; }
void ByName_4(Entity *entity, const std::string &string)
{ if (ShipEntity *ship = ShipPart(entity))  ship->deserializeShipSubEntitiesFrom(string); }
bool ByName_5(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->setUpSubEntities(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->setUpSubEntities() : false; }
bool ByName_6(Entity *entity, const oo::PList &subentDict)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->setUpOneSubentity(subentDict) : false; }
bool ByName_7(Entity *entity, const oo::PList &subentDict)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->setUpOneFlasher(subentDict) : false; }
bool ByName_8(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isTemplateCargoPod() : false; }
void ByName_9(Entity *entity, const std::string &cargoString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setUpCargoType(cargoString); }
void ByName_10(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->removeScript(); }
void ByName_11(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->clearSubEntities(); else if (oo::VisualEffectEntity *effect = EffectPart(entity))  effect->clearSubEntities(); }
Quaternion ByName_12(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->subEntityRotationalVelocity() : kIdentityQuaternion; }
GLfloat ByName_13(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getSunGlareFilter() : GLfloat{}; }
GLfloat ByName_14(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getAccuracy() : GLfloat{}; }
Vector ByName_15(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->forwardVector(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->forwardVector() : kZeroVector; }
Vector ByName_16(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->upVector(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->upVector() : kZeroVector; }
Vector ByName_17(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->rightVector(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->rightVector() : kZeroVector; }
bool ByName_18(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->scriptedMisjump() : false; }
GLfloat ByName_19(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->scriptedMisjumpRange() : GLfloat{}; }
std::size_t ByName_20(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->subEntityCount(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->subEntityCount() : std::size_t{}; }
Entity * ByName_21(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->subEntityTakingDamage() : nullptr; }
OOTimeAbsolute ByName_22(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shipAIScriptWakeTime() : OOTimeAbsolute{}; }
float ByName_23(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->volume() : float{}; }
HPVector ByName_24(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->absoluteTractorPosition() : kZeroHPVector; }
bool ByName_25(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isBoulder() : false; }
bool ByName_26(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isMinable() : false; }
bool ByName_27(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->countsAsKill() : false; }
void ByName_28(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setUpEscorts(); }
void ByName_29(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setUpMixedEscorts(); }
oo::PList ByName_30(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shipInfoDictionary() : oo::PList(); }
bool ByName_31(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getIsFrangible() : false; }
bool ByName_32(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->suppressFlightNotifications() : false; }
void ByName_33(Entity *entity, const std::string &context)
{ if (ShipEntity *ship = ShipPart(entity))  ship->noteFrustration(context); }
std::size_t ByName_34(Entity *entity, const std::string &eqkey)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->countEquipmentItem(eqkey) : std::size_t{}; }
bool ByName_35(Entity *entity, const oo::PList &equipmentKeys)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasEquipmentItem(equipmentKeys) : false; }
bool ByName_36(Entity *entity, const std::string &equipmentType)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasEquipmentItemProviding(equipmentType) : false; }
bool ByName_37(Entity *entity, const oo::PList &equipmentKeys)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasAllEquipment(equipmentKeys) : false; }
bool ByName_38(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasHyperspaceMotor() : false; }
float ByName_39(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hyperspaceSpinTime() : float{}; }
oo::PList ByName_40(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->passengerListForScripting() : oo::PList(); }
oo::PList ByName_41(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->parcelListForScripting() : oo::PList(); }
oo::PList ByName_42(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->contractListForScripting() : oo::PList(); }
std::size_t ByName_43(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->equipmentCount() : std::size_t{}; }
void ByName_44(Entity *entity, const std::string &equipmentKey)
{ if (ShipEntity *ship = ShipPart(entity))  ship->removeEquipmentItem(equipmentKey); }
void ByName_45(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->removeAllEquipment(); }
OOCreditsQuantity ByName_46(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->removeMissiles() : OOCreditsQuantity{}; }
std::size_t ByName_47(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->parcelCount() : std::size_t{}; }
std::size_t ByName_48(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->passengerCount() : std::size_t{}; }
std::size_t ByName_49(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->passengerCapacity() : std::size_t{}; }
std::size_t ByName_50(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->missileCount() : std::size_t{}; }
std::size_t ByName_51(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->missileCapacity() : std::size_t{}; }
std::size_t ByName_52(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->extraCargo() : std::size_t{}; }
bool ByName_53(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasScoop() : false; }
bool ByName_54(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasFuelScoop() : false; }
bool ByName_55(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasCargoScoop() : false; }
bool ByName_56(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasECM() : false; }
bool ByName_57(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasCloakingDevice() : false; }
bool ByName_58(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasMilitaryScannerFilter() : false; }
bool ByName_59(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasMilitaryJammer() : false; }
bool ByName_60(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasExpandedCargoBay() : false; }
bool ByName_61(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasShieldBooster() : false; }
bool ByName_62(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasMilitaryShieldEnhancer() : false; }
bool ByName_63(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasHeatShield() : false; }
bool ByName_64(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasFuelInjection() : false; }
bool ByName_65(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasCascadeMine() : false; }
bool ByName_66(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasEscapePod() : false; }
bool ByName_67(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasDockingComputer() : false; }
bool ByName_68(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasGalacticHyperdrive() : false; }
float ByName_69(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shieldBoostFactor() : float{}; }
float ByName_70(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxForwardShieldLevel() : float{}; }
float ByName_71(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxAftShieldLevel() : float{}; }
float ByName_72(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shieldRechargeRate() : float{}; }
double ByName_73(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxHyperspaceDistance() : double{}; }
float ByName_74(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->afterburnerFactor() : float{}; }
float ByName_75(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->afterburnerRate() : float{}; }
float ByName_76(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxThrust() : float{}; }
float ByName_77(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getThrust() : float{}; }
float ByName_78(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getReactionTime() : float{}; }
HPVector ByName_79(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->calculateTargetPosition() : kZeroHPVector; }
void ByName_80(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->startTrackingCurve(); }
void ByName_81(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->updateTrackingCurve(); }
void ByName_82(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->calculateTrackingCurve(); }
#ifdef OO_DEBUG_MONITOR_ENABLED
void ByName_83(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->drawDebugStuff(); }
#endif
bool ByName_84(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isCloaked() : false; }
bool ByName_85(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getCloakPassive() : false; }
bool ByName_86(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasAutoCloak() : false; }
void ByName_87(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->avoidCollision(); }
void ByName_88(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->resumePostProximityAlert(); }
double ByName_89(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getMessageTime() : double{}; }
bool ByName_90(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasEscorts() : false; }
uint8_t ByName_91(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->escortCount() : uint8_t{}; }
uint8_t ByName_92(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->pendingEscortCount() : uint8_t{}; }
uint8_t ByName_93(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxEscortCount() : uint8_t{}; }
std::size_t ByName_94(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->turretCount() : std::size_t{}; }
Entity * ByName_95(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->proximityAlert() : nullptr; }
bool ByName_96(Entity *entity, const std::string &role)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasRole(role) : false; }
void ByName_97(Entity *entity, const std::string &role)
{ if (ShipEntity *ship = ShipPart(entity))  ship->addRole(role); }
void ByName_98(Entity *entity, const std::string &role)
{ if (ShipEntity *ship = ShipPart(entity))  ship->removeRole(role); }
void ByName_99(Entity *entity, const std::string &role)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setPrimaryRole(role); }
bool ByName_100(Entity *entity, const std::string &role)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasPrimaryRole(role) : false; }
bool ByName_101(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isPolice() : false; }
bool ByName_102(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isThargoid() : false; }
bool ByName_103(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isTrader() : false; }
bool ByName_104(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isPirate() : false; }
bool ByName_105(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getIsMissile() : false; }
bool ByName_106(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isMine() : false; }
bool ByName_107(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isWeapon() : false; }
bool ByName_108(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isEscort() : false; }
bool ByName_109(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isShuttle() : false; }
bool ByName_110(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isTurret() : false; }
bool ByName_111(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isPirateVictim() : false; }
bool ByName_112(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isExplicitlyUnpiloted() : false; }
bool ByName_113(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isUnpiloted() : false; }
bool ByName_114(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasHostileTarget() : false; }
GLfloat ByName_115(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getWeaponRange() : GLfloat{}; }
float ByName_116(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->energyRechargeRate() : float{}; }
float ByName_117(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->weaponRechargeRate() : float{}; }
OOWeaponFacing ByName_118(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getCurrentWeaponFacing() : OOWeaponFacing{}; }
GLfloat ByName_119(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getScannerRange() : GLfloat{}; }
Vector ByName_120(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getReference() : kZeroVector; }
bool ByName_121(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getReportAIMessages() : false; }
void ByName_122(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->transitionToAegisNone(); }
Entity * ByName_123(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->findNearestStellarBody() : nullptr; }
OOAegisStatus ByName_124(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->checkForAegis() : OOAegisStatus{}; }
void ByName_125(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->forceAegisCheck(); }
bool ByName_126(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->withinStationAegis() : false; }
Entity * ByName_127(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->lastAegisLock() : nullptr; }
OOSystemID ByName_128(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->homeSystem() : OOSystemID{}; }
OOSystemID ByName_129(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->destinationSystem() : OOSystemID{}; }
void ByName_130(Entity *entity, const std::string &crewRole)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSingleCrewWithRole(crewRole); }
void ByName_131(Entity *entity, const std::string &smName)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setStateMachine(smName); }
bool ByName_132(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasAutoAI() : false; }
bool ByName_133(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasNewAI() : false; }
bool ByName_134(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hasAutoWeapons() : false; }
double ByName_135(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFrustration() : double{}; }
OOFuelQuantity ByName_136(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFuel() : OOFuelQuantity{}; }
OOFuelQuantity ByName_137(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->fuelCapacity() : OOFuelQuantity{}; }
GLfloat ByName_138(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->fuelChargeRate() : GLfloat{}; }
OOCreditsQuantity ByName_139(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getBounty() : OOCreditsQuantity{}; }
int ByName_140(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->legalStatus() : int{}; }
OOCargoQuantity ByName_141(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->commodityAmount() : OOCargoQuantity{}; }
OOCargoQuantity ByName_142(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxAvailableCargoSpace() : OOCargoQuantity{}; }
OOCargoQuantity ByName_143(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->availableCargoSpace() : OOCargoQuantity{}; }
OOCargoQuantity ByName_144(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->cargoQuantityOnBoard() : OOCargoQuantity{}; }
OOCargoType ByName_145(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->cargoType() : OOCargoType::kNotCargo; }
std::size_t ByName_146(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->cargoCount() : std::size_t{}; }
oo::PList ByName_147(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->cargoListForScripting() : oo::PList(); }
bool ByName_148(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->showScoopMessage() : false; }
double ByName_149(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->desiredSpeed() : double{}; }
double ByName_150(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->desiredRange() : double{}; }
double ByName_151(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getCruiseSpeed() : double{}; }
GLfloat ByName_152(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFlightRoll() : GLfloat{}; }
GLfloat ByName_153(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFlightPitch() : GLfloat{}; }
GLfloat ByName_154(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFlightYaw() : GLfloat{}; }
GLfloat ByName_155(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getFlightSpeed() : GLfloat{}; }
GLfloat ByName_156(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxFlightPitch() : GLfloat{}; }
GLfloat ByName_157(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getMaxFlightSpeed() : GLfloat{}; }
GLfloat ByName_158(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxFlightRoll() : GLfloat{}; }
GLfloat ByName_159(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->maxFlightYaw() : GLfloat{}; }
GLfloat ByName_160(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->speedFactor() : GLfloat{}; }
GLfloat ByName_161(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->temperature() : GLfloat{}; }
float ByName_162(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->randomEjectaTemperature() : float{}; }
GLfloat ByName_163(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->heatInsulation() : GLfloat{}; }
int ByName_164(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->damage() : int{}; }
void ByName_165(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->dealEnergyDamageWithinDesiredRange(); }
bool ByName_166(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getIsHulk() : false; }
void ByName_167(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->releaseCargoPodsDebris(); }
bool ByName_168(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->showDamage() : false; }
void ByName_169(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->becomeExplosion(); }
void ByName_170(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->becomeEnergyBlast(); }
void ByName_171(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->broadcastEnergyBlastImminent(); }
GLfloat ByName_172(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->weaponRecoveryTime() : GLfloat{}; }
GLfloat ByName_173(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->laserHeatLevel() : GLfloat{}; }
GLfloat ByName_174(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->laserHeatLevelAft() : GLfloat{}; }
GLfloat ByName_175(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->laserHeatLevelForward() : GLfloat{}; }
GLfloat ByName_176(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->laserHeatLevelPort() : GLfloat{}; }
GLfloat ByName_177(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->laserHeatLevelStarboard() : GLfloat{}; }
GLfloat ByName_178(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->hullHeatLevel(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->hullHeatLevel() : GLfloat{}; }
GLfloat ByName_179(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->entityPersonality() : GLfloat{}; }
GLint ByName_180(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->entityPersonalityInt() : GLint{}; }
uint32_t ByName_181(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->randomSeedForShaders() : uint32_t{}; }
void ByName_182(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->resetExhaustPlumes(); }
void ByName_183(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkScanner(); }
void ByName_184(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkScannerIgnoringUnpowered(); }
int ByName_185(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->numberOfScannedShips() : int{}; }
Entity * ByName_186(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->foundTarget() : nullptr; }
Entity * ByName_187(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->primaryAggressor() : nullptr; }
Entity * ByName_188(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->lastEscortTarget() : nullptr; }
Entity * ByName_189(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->thankedShip() : nullptr; }
Entity * ByName_190(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->rememberedShip() : nullptr; }
Entity * ByName_191(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->targetStation() : nullptr; }
bool ByName_192(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->canStillTrackPrimaryTarget() : false; }
Entity * ByName_193(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shipHitByLaser() : nullptr; }
void ByName_194(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->noteLostTarget(); }
void ByName_195(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->noteLostTargetAndGoIdle(); }
oo::Behaviour ByName_196(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getBehaviour() : oo::Behaviour{}; }
HPVector ByName_197(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->destination() : kZeroHPVector; }
HPVector ByName_198(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getCoordinates() : kZeroHPVector; }
GLfloat ByName_199(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->rangeToDestination() : GLfloat{}; }
std::size_t ByName_200(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->defenseTargetCount() : std::size_t{}; }
void ByName_201(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->validateDefenseTargets(); }
void ByName_202(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->removeAllDefenseTargets(); }
double ByName_203(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->rangeToPrimaryTarget() : double{}; }
double ByName_204(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->approachAspectToPrimaryTarget() : double{}; }
GLfloat ByName_205(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->currentAimTolerance() : GLfloat{}; }
OOTimeDelta ByName_206(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->shotTime() : OOTimeDelta{}; }
void ByName_207(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->resetShotTime(); }
bool ByName_208(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->fireDirectLaserDefensiveShot() : false; }
int ByName_209(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->missedShots() : int{}; }
Vector ByName_210(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->missileLaunchPosition() : kZeroVector; }
Entity * ByName_211(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->fireMissile() : nullptr; }
bool ByName_212(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isMissileFlagSet() : false; }
OOTimeDelta ByName_213(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->missileLoadTime() : OOTimeDelta{}; }
void ByName_214(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->noticeECM(); }
bool ByName_215(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->fireECM() : false; }
bool ByName_216(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->activateCloakingDevice() : false; }
void ByName_217(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->deactivateCloakingDevice(); }
bool ByName_218(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->launchCascadeMine() : false; }
Entity * ByName_219(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->launchEscapeCapsule() : nullptr; }
void ByName_220(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->dumpCargo(); }
void ByName_221(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->manageCollisions(); }
Vector ByName_222(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->thrustVector() : kZeroVector; }
void ByName_223(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->suppressTargetLost(); }
bool ByName_224(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->abandonShip() : false; }
void ByName_225(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->enterWitchspace(); }
void ByName_226(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->leaveWitchspace(); }
bool ByName_227(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->witchspaceLeavingEffects() : false; }
void ByName_228(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->switchLightsOn(); }
void ByName_229(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->switchLightsOff(); }
bool ByName_230(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->lightsActive() : false; }
void ByName_231(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->updateEscortFormation(); }
void ByName_232(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->refreshEscortPositions(); }
void ByName_233(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->deployEscorts(); }
void ByName_234(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->dockEscorts(); }
void ByName_235(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToNearestFriendlyStation(); }
void ByName_236(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToNearestStation(); }
void ByName_237(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToSystemStation(); }
void ByName_238(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->abortDocking(); }
oo::PList ByName_239(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getDockingInstructions() : oo::PList(); }
void ByName_240(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->broadcastThargoidDestroyed(); }
void ByName_241(Entity *entity, const std::string &ai_message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->broadcastAIMessage(ai_message); }
void ByName_242(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setCommsMessageColor(); }
bool ByName_243(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->markedForFines() : false; }
bool ByName_244(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->markForFines() : false; }
bool ByName_245(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->isMining() : false; }
void ByName_246(Entity *entity, const std::string &ms)
{ if (ShipEntity *ship = ShipPart(entity))  ship->interpretAIMessage(ms); }
void ByName_247(Entity *entity, const std::string &roles_number)
{ if (ShipEntity *ship = ShipPart(entity))  ship->spawn(roles_number); }
int ByName_248(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->checkShipsInVicinityForWitchJumpExit() : int{}; }
bool ByName_249(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getTrackCloseContacts() : false; }
oo::PList ByName_253(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  return effect->scriptInfo(); ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getScriptInfo() : oo::PList(); }
void ByName_254(Entity *entity, const oo::PList &override)
{ if (ShipEntity *ship = ShipPart(entity))  ship->overrideScriptInfo(override); }
Entity * ByName_255(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->entityForShaderProperties() : nullptr; }
bool ByName_256(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getIsDemoShip() : false; }
OOTimeAbsolute ByName_257(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->getDemoStartTime() : OOTimeAbsolute{}; }
void ByName_258(Entity *entity, const std::string &message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->sendAIMessage(message); }
oo::AlertCondition ByName_259(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->alertCondition() : oo::AlertCondition{}; }
oo::AlertCondition ByName_260(Entity *entity)
{ ShipEntity *ship = ShipPart(entity); return ship != nullptr ? ship->realAlertCondition() : oo::AlertCondition{}; }
void ByName_261(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->doNothing(); }
void ByName_262(Entity *entity, const std::string &aiString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setAITo(aiString); }
void ByName_263(Entity *entity, const std::string &aiString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setAIScript(aiString); }
void ByName_264(Entity *entity, const std::string &aiString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->switchAITo(aiString); }
void ByName_265(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForHostiles(); }
void ByName_266(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->groupAttackTarget(); }
void ByName_267(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performAttack(); }
void ByName_268(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performCollect(); }
void ByName_269(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performEscort(); }
void ByName_270(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performFaceDestination(); }
void ByName_271(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performFlee(); }
void ByName_272(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performFlyToRangeFromDestination(); }
void ByName_273(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performHold(); }
void ByName_274(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performIdle(); }
void ByName_275(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performIntercept(); }
void ByName_276(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performLandOnPlanet(); }
void ByName_277(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performMining(); }
void ByName_278(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performScriptedAI(); }
void ByName_279(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performScriptedAttackAI(); }
void ByName_280(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performBuoyTumble(); }
void ByName_281(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performStop(); }
void ByName_282(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performTumble(); }
void ByName_283(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->requestDockingCoordinates(); }
void ByName_284(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->recallDockingInstructions(); }
void ByName_285(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestIncomingMissile(); }
void ByName_286(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->enterPlayerWormhole(); }
void ByName_287(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->enterTargetWormhole(); }
void ByName_288(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->wormholeEscorts(); }
void ByName_289(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->wormholeEntireGroup(); }
void ByName_290(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->broadcastDistressMessage(); }
void ByName_291(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkFoundTarget(); }
void ByName_292(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return;
 if (StationEntity *station = StationPart(entity))  station->increaseAlertLevel(); else  ship->increaseAlertLevel(); 
}
void ByName_293(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return;
 if (StationEntity *station = StationPart(entity))  station->decreaseAlertLevel(); else  ship->decreaseAlertLevel(); 
}
oo::PList ByName_294(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return oo::PList();
	if (StationEntity *station = StationPart(entity))  return station->launchPolice();
	return ship->launchPolice(); 
}
Entity * ByName_295(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchDefenseShip();
	ship->launchDefenseShip(); return nullptr; 
}
Entity * ByName_296(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchScavenger();
	ship->launchScavenger(); return nullptr; 
}
Entity * ByName_297(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchMiner();
	ship->launchMiner(); return nullptr; 
}
Entity * ByName_298(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchPirateShip();
	ship->launchPirateShip(); return nullptr; 
}
Entity * ByName_299(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchShuttle();
	ship->launchShuttle(); return nullptr; 
}
void ByName_300(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->launchTrader(); }
Entity * ByName_301(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchEscort();
	ship->launchEscort(); return nullptr; 
}
Entity * ByName_302(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return nullptr;
	if (StationEntity *station = StationPart(entity))  return station->launchPatrol();
	(void)ship->launchPatrol(); return nullptr; 
}
void ByName_303(Entity *entity, const std::string &param)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return;
 if (StationEntity *station = StationPart(entity))  station->launchShipWithRole(param); else  ship->launchShipWithRole(param); 
}
void ByName_304(Entity *entity)
{
	ShipEntity *ship = ShipPart(entity);
	if (ship == nullptr)  return;
 if (StationEntity *station = StationPart(entity))  station->abortAllDockings(); else if (DockEntity *dock = DockPart(entity))  dock->abortAllDockings(); else  ship->abortAllDockings(); 
}
void ByName_305(Entity *entity, const std::string &state)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setStateTo(state); }
void ByName_306(Entity *entity, const std::string &intervalString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->pauseAI(intervalString); }
void ByName_307(Entity *entity, const std::string &intervalString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->randomPauseAI(intervalString); }
void ByName_308(Entity *entity, const std::string &messageString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->dropMessages(messageString); }
void ByName_309(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->debugDumpPendingMessages(); }
void ByName_310(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToCurrentLocation(); }
void ByName_311(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToJinkPosition(); }
void ByName_312(Entity *entity, const std::string &rangeString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDesiredRangeTo(rangeString); }
void ByName_313(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDesiredRangeForWaypoint(); }
void ByName_314(Entity *entity, const std::string &speedString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSpeedTo(speedString); }
void ByName_315(Entity *entity, const std::string &speedString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSpeedFactorTo(speedString); }
void ByName_316(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSpeedToCruiseSpeed(); }
void ByName_317(Entity *entity, const std::string &thrustFactorString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setThrustFactorTo(thrustFactorString); }
void ByName_318(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToPrimaryAggressor(); }
void ByName_319(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->addPrimaryAggressorAsDefenseTarget(); }
void ByName_320(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestMerchantman(); }
void ByName_321(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForRandomMerchantman(); }
void ByName_322(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForLoot(); }
void ByName_323(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForRandomLoot(); }
void ByName_324(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToFoundTarget(); }
void ByName_325(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->addFoundTargetAsDefenseTarget(); }
void ByName_326(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkForFullHold(); }
void ByName_327(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->getWitchspaceEntryCoordinates(); }
void ByName_328(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationFromCoordinates(); }
void ByName_329(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setCoordinatesFromPosition(); }
void ByName_330(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->fightOrFleeMissile(); }
void ByName_331(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setCourseToPlanet(); }
void ByName_332(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTakeOffFromPlanet(); }
void ByName_333(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->landOnPlanet(); }
void ByName_334(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkTargetLegalStatus(); }
void ByName_335(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkOwnLegalStatus(); }
void ByName_336(Entity *entity, const std::string &message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->exitAIWithMessage(message); }
void ByName_337(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToTarget(); }
void ByName_338(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationWithinTarget(); }
void ByName_339(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkCourseToDestination(); }
void ByName_340(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkAegis(); }
void ByName_341(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkEnergy(); }
void ByName_342(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkHeatInsulation(); }
void ByName_343(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->findNewDefenseTarget(); }
void ByName_344(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForOffenders(); }
void ByName_345(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setCourseToWitchpoint(); }
void ByName_346(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToWitchpoint(); }
void ByName_347(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToStationBeacon(); }
void ByName_348(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performHyperSpaceExit(); }
void ByName_349(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performHyperSpaceExitWithoutReplacing(); }
void ByName_350(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->disengageAutopilot(); }
void ByName_351(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->wormholeGroup(); }
void ByName_352(Entity *entity, const std::string &valueString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->commsMessage(valueString); }
void ByName_353(Entity *entity, const std::string &valueString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->commsMessageByUnpiloted(valueString); }
void ByName_354(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->ejectCargo(); }
void ByName_355(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForThargoid(); }
void ByName_356(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNonThargoid(); }
void ByName_357(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->thargonCheckMother(); }
void ByName_358(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->becomeUncontrolledThargon(); }
void ByName_359(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkDistanceTravelled(); }
void ByName_360(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->fightOrFleeHostiles(); }
void ByName_361(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->suggestEscort(); }
void ByName_362(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->escortCheckMother(); }
void ByName_363(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkGroupOddsVersusTarget(); }
void ByName_364(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForFormationLeader(); }
void ByName_365(Entity *entity, const std::string &msgString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->messageMother(msgString); }
void ByName_366(Entity *entity, const std::string &msgString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->messageSelf(msgString); }
void ByName_367(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setPlanetPatrolCoordinates(); }
void ByName_368(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSunSkimStartCoordinates(); }
void ByName_369(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSunSkimEndCoordinates(); }
void ByName_370(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setSunSkimExitCoordinates(); }
void ByName_371(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->patrolReportIn(); }
void ByName_372(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkForMotherStation(); }
void ByName_373(Entity *entity, const std::string &message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->sendTargetCommsMessage(message); }
void ByName_374(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->markTargetForFines(); }
void ByName_375(Entity *entity, const std::string &valueString)
{ if (ShipEntity *ship = ShipPart(entity))  ship->markTargetForOffence(valueString); }
void ByName_376(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->storeTarget(); }
void ByName_377(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->recallStoredTarget(); }
void ByName_378(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForRocks(); }
void ByName_379(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setDestinationToDockingAbort(); }
void ByName_380(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->requestNewTarget(); }
void ByName_381(Entity *entity, const std::string &die_number)
{ if (ShipEntity *ship = ShipPart(entity))  ship->rollD(die_number); }
void ByName_382(Entity *entity, const std::string &scanRole)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithPrimaryRole(scanRole); }
void ByName_383(Entity *entity, const std::string &scanRole)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipHavingRole(scanRole); }
void ByName_384(Entity *entity, const std::string &scanRoles)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithAnyPrimaryRole(scanRoles); }
void ByName_385(Entity *entity, const std::string &scanRoles)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipHavingAnyRole(scanRoles); }
void ByName_386(Entity *entity, const std::string &scanScanClass)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithScanClass(scanScanClass); }
void ByName_387(Entity *entity, const std::string &scanRole)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithoutPrimaryRole(scanRole); }
void ByName_388(Entity *entity, const std::string &scanRole)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipNotHavingRole(scanRole); }
void ByName_389(Entity *entity, const std::string &scanRoles)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithoutAnyPrimaryRole(scanRoles); }
void ByName_390(Entity *entity, const std::string &scanRoles)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipNotHavingAnyRole(scanRoles); }
void ByName_391(Entity *entity, const std::string &scanScanClass)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipWithoutScanClass(scanScanClass); }
void ByName_392(Entity *entity, const std::string &predicateExpression)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scanForNearestShipMatchingPredicate(predicateExpression); }
void ByName_393(Entity *entity, const std::string &system_x_y_z)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setCoordinates(system_x_y_z); }
void ByName_394(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->checkForNormalSpace(); }
void ByName_395(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToRandomStation(); }
void ByName_396(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setTargetToLastStation(); }
void ByName_397(Entity *entity, const std::string &fuel_number)
{ if (ShipEntity *ship = ShipPart(entity))  ship->addFuel(fuel_number); }
void ByName_398(Entity *entity, const std::string &action)
{ if (ShipEntity *ship = ShipPart(entity))  ship->scriptActionOnTarget(action); }
void ByName_399(Entity *entity, const std::string &action)
{ if (ShipEntity *ship = ShipPart(entity))  ship->safeScriptActionOnTarget(action); }
void ByName_400(Entity *entity, const std::string &message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->sendScriptMessage(message); }
void ByName_401(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->ai_throwSparks(); }
void ByName_402(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->explodeSelf(); }
void ByName_403(Entity *entity, const std::string &message)
{ if (ShipEntity *ship = ShipPart(entity))  ship->ai_debugMessage(message); }
void ByName_404(Entity *entity, const std::string &code)
{ if (ShipEntity *ship = ShipPart(entity))  ship->targetFirstBeaconWithCode(code); }
void ByName_405(Entity *entity, const std::string &code)
{ if (ShipEntity *ship = ShipPart(entity))  ship->targetNextBeaconWithCode(code); }
void ByName_406(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->setRacepointsFromTarget(); }
void ByName_407(Entity *entity)
{ if (ShipEntity *ship = ShipPart(entity))  ship->performFlyRacepoints(); }
GLfloat ByName_408(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->baseMass() : GLfloat{}; }
void ByName_409(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->unloadCargoPods(); }
void ByName_410(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->loadCargoPods(); }
OOCreditsQuantity ByName_411(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->deciCredits() : OOCreditsQuantity{}; }
int ByName_412(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->random_factor() : int{}; }
OOGalaxyID ByName_413(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->galaxyNumber() : OOGalaxyID{}; }
NSPoint ByName_414(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getGalaxy_coordinates() : NSMakePoint(0, 0); }
NSPoint ByName_415(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCursor_coordinates() : NSMakePoint(0, 0); }
NSPoint ByName_416(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getChart_centre_coordinates() : NSMakePoint(0, 0); }
OOScalar ByName_417(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getChart_zoom() : OOScalar{}; }
OOScalar ByName_418(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustom_chart_zoom() : OOScalar{}; }
NSPoint ByName_419(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustom_chart_centre_coordinates() : NSMakePoint(0, 0); }
NSPoint ByName_420(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->adjusted_chart_centre() : NSMakePoint(0, 0); }
OORouteType ByName_421(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->ANAMode() : OORouteType{}; }
OOSystemID ByName_422(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemID() : OOSystemID{}; }
OOSystemID ByName_423(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->previousSystemID() : OOSystemID{}; }
OOSystemID ByName_424(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->targetSystemID() : OOSystemID{}; }
OOSystemID ByName_425(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->nextHopTargetSystemID() : OOSystemID{}; }
OOSystemID ByName_426(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->infoSystemID() : OOSystemID{}; }
void ByName_427(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->nextInfoSystem(); }
void ByName_428(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->previousInfoSystem(); }
void ByName_429(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->homeInfoSystem(); }
void ByName_430(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->targetInfoSystem(); }
bool ByName_431(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->infoSystemOnRoute() : false; }
oo::PList ByName_432(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderDataDictionary() : oo::PList(); }
bool ByName_433(Entity *entity, const oo::PList &dict)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->setCommanderDataFromDictionary(dict) : false; }
void ByName_434(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->completeSetUp(); }
void ByName_435(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->startUpComplete(); }
GLfloat ByName_436(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->insideAtmosphereFraction() : GLfloat{}; }
void ByName_437(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateMovementFlags(); }
void ByName_438(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateAlertConditionForNearbyEntities(); }
void ByName_439(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateAlertCondition(); }
void ByName_440(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->checkScriptsIfAppropriate(); }
void ByName_441(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetAutopilotAI(); }
#if OO_VARIABLE_TORUS_SPEED
GLfloat ByName_442(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getHyperspeedFactor() : GLfloat{}; }
#endif
bool ByName_443(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->injectorsEngaged() : false; }
bool ByName_444(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->hyperspeedEngaged() : false; }
void ByName_445(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->gameOverFadeToBW(); }
void ByName_446(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showGameOver(); }
void ByName_447(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateTargeting(); }
HPVector ByName_448(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->breakPatternPosition() : kZeroHPVector; }
Vector ByName_449(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointOffset() : kZeroVector; }
Vector ByName_450(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointOffsetAft() : kZeroVector; }
Vector ByName_451(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointOffsetForward() : kZeroVector; }
Vector ByName_452(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointOffsetPort() : kZeroVector; }
Vector ByName_453(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointOffsetStarboard() : kZeroVector; }
HPVector ByName_454(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->viewpointPosition() : kZeroHPVector; }
bool ByName_455(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getMassLockable() : false; }
bool ByName_456(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->massLocked(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->massLocked() : false; }
bool ByName_457(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->atHyperspeed(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->atHyperspeed() : false; }
float ByName_458(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->occlusionLevel() : float{}; }
void ByName_459(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setDockedAtMainStation(); }
Entity * ByName_460(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dockedStation() : nullptr; }
Entity * ByName_461(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getTargetDockStation() : nullptr; }
void ByName_462(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetHud(); }
bool ByName_463(Entity *entity, const std::string &hudFileName)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->switchHudTo(hudFileName) : false; }
float ByName_464(Entity *entity, const std::string &dialKey)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialCustomFloat(dialKey) : float{}; }
bool ByName_465(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getShowDemoShips() : false; }
float ByName_466(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->forwardShieldRechargeRate() : float{}; }
float ByName_467(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->aftShieldRechargeRate() : float{}; }
GLfloat ByName_468(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->forwardShieldLevel() : GLfloat{}; }
GLfloat ByName_469(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->aftShieldLevel() : GLfloat{}; }
oo::PList ByName_470(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->keyConfig() : oo::PList(); }
bool ByName_471(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->isMouseControlOn() : false; }
GLfloat ByName_472(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialRoll() : GLfloat{}; }
GLfloat ByName_473(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialPitch() : GLfloat{}; }
GLfloat ByName_474(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialYaw() : GLfloat{}; }
GLfloat ByName_475(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialSpeed() : GLfloat{}; }
GLfloat ByName_476(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialHyperSpeed() : GLfloat{}; }
GLfloat ByName_477(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->dialForwardShield(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialForwardShield() : GLfloat{}; }
GLfloat ByName_478(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->dialAftShield(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialAftShield() : GLfloat{}; }
GLfloat ByName_479(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialEnergy() : GLfloat{}; }
GLfloat ByName_480(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialMaxEnergy() : GLfloat{}; }
GLfloat ByName_481(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialFuel() : GLfloat{}; }
GLfloat ByName_482(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialHyperRange() : GLfloat{}; }
GLfloat ByName_483(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialAltitude() : GLfloat{}; }
double ByName_484(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clockTime() : double{}; }
double ByName_485(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clockTimeAdjusted() : double{}; }
bool ByName_486(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clockAdjusting() : false; }
double ByName_487(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->escapePodRescueTime() : double{}; }
unsigned ByName_488(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->countMissiles() : unsigned{}; }
oo::MissileStatus ByName_489(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->dialMissileStatus(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialMissileStatus() : oo::MissileStatus{}; }
oo::FuelScoopStatus ByName_490(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->dialFuelScoopStatus(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialFuelScoopStatus() : oo::FuelScoopStatus{}; }
float ByName_491(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->fuelLeakRate(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fuelLeakRate() : float{}; }
void ByName_492(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addRoleForMining(); }
void ByName_493(Entity *entity, const std::string &role)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addRoleToPlayer(role); }
std::size_t ByName_494(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->maxPlayerRoles() : std::size_t{}; }
void ByName_495(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateSystemMemory(); }
Entity * ByName_496(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCompassTargetEntity() : nullptr; }
void ByName_497(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->validateCompassTarget(); }
oo::CompassMode ByName_498(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->compassMode(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCompassMode() : oo::CompassMode{}; }
void ByName_499(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setPrevCompassMode(); }
void ByName_500(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setNextCompassMode(); }
std::size_t ByName_501(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getActiveMissile() : std::size_t{}; }
std::size_t ByName_502(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialMaxMissiles() : std::size_t{}; }
bool ByName_503(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->dialIdentEngaged(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dialIdentEngaged() : false; }
void ByName_504(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->selectNextMultiFunctionDisplay(); }
void ByName_505(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->selectPreviousMultiFunctionDisplay(); }
std::size_t ByName_506(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getActiveMFD() : std::size_t{}; }
void ByName_507(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->safeAllMissiles(); }
void ByName_508(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->tidyMissilePylons(); }
void ByName_509(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->selectNextMissile(); }
void ByName_510(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearAlertFlags(); }
int ByName_511(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getAlertFlags() : int{}; }
oo::PlayerFleeingStatus ByName_512(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fleeingStatus() : oo::PlayerFleeingStatus{}; }
bool ByName_513(Entity *entity, const std::string &role)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->mountMissileWithRole(role) : false; }
bool ByName_514(Entity *entity, const std::string &equipmentKey)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->assignToActivePylon(equipmentKey) : false; }
double ByName_515(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scannerFuzziness() : double{}; }
OOEnergyUnitType ByName_516(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->installedEnergyUnitType() : OOEnergyUnitType{}; }
OOEnergyUnitType ByName_517(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->energyUnitType() : OOEnergyUnitType{}; }
void ByName_518(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->currentWeaponStats(); }
bool ByName_519(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->weaponsOnline() : false; }
bool ByName_520(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fireMainWeapon() : false; }
Entity * ByName_521(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->createDoppelganger() : nullptr; }
void ByName_522(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->rotateCargo(); }
bool ByName_523(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->takeInternalDamage() : false; }
void ByName_524(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->loseTargetStatus(); }
bool ByName_525(Entity *entity, const std::string &key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->endScenario(key) : false; }
void ByName_526(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->docked(); }
void ByName_527(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->witchStart(); }
void ByName_528(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->witchEnd(); }
double ByName_529(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->hyperspaceJumpDistance() : double{}; }
OOFuelQuantity ByName_530(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fuelRequiredForJump() : OOFuelQuantity{}; }
bool ByName_531(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->hasSufficientFuelForJump() : false; }
void ByName_532(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->noteCompassLostTarget(); }
void ByName_533(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->enterGalacticWitchspace(); }
void ByName_534(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToStatusScreen(); }
std::size_t ByName_535(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->primedEquipmentCount() : std::size_t{}; }
unsigned ByName_536(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->legalStatusOfCargoList() : unsigned{}; }
void ByName_537(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToSystemDataScreen(); }
void ByName_538(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToLongRangeChartScreen(); }
void ByName_539(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToShortRangeChartScreen(); }
void ByName_540(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToGameOptionsScreen(); }
void ByName_541(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToLoadSaveScreen(); }
void ByName_542(Entity *entity, const std::string &highlightKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->highlightEquipShipScreenKey(highlightKey); }
OOWeaponFacingSet ByName_543(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->availableFacings() : OOWeaponFacingSet{}; }
void ByName_544(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showInformationForSelectedUpgrade(); }
void ByName_545(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showInformationForSelectedInterface(); }
void ByName_546(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->activateSelectedInterface(); }
void ByName_547(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setupStartScreenGui(); }
void ByName_548(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToOXZManager(); }
void ByName_549(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->buySelectedItem(); }
bool ByName_550(Entity *entity, const std::string &eqKey)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->tryBuyingItem(eqKey) : false; }
OOCargoQuantity ByName_551(Entity *entity, const std::string &type)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->cargoQuantityForType(type) : OOCargoQuantity{}; }
void ByName_552(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->calculateCurrentCargo(); }
void ByName_553(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showMarketScreenHeaders(); }
void ByName_554(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToMarketScreen(); }
void ByName_555(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToMarketInfoScreen(); }
void ByName_556(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showMarketCashAndLoadLine(); }
oo::GUIScreenID ByName_557(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->guiScreen() : oo::GUIScreenID{}; }
oo::SpeechSettings ByName_558(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getIsSpeechOn() : oo::SpeechSettings{}; }
void ByName_559(Entity *entity, const std::string &equipmentKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addEquipmentWithScriptToCustomKeyArray(equipmentKey); }
void ByName_560(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->validateCustomEquipActivationArray(); }
void ByName_561(Entity *entity, const oo::PList &equipment)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addEquipmentFromCollection(equipment); }
void ByName_562(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->getFined(); }
int ByName_563(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->tradeInFactor(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->tradeInFactor() : int{}; }
double ByName_564(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->renovationCosts() : double{}; }
double ByName_565(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->renovationFactor() : double{}; }
void ByName_566(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setDefaultViewOffsets(); }
void ByName_567(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setDefaultCustomViews(); }
Vector ByName_568(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->weaponViewOffset() : kZeroVector; }
void ByName_569(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setUpTrumbles(); }
std::size_t ByName_570(Entity *entity)
{ if (ProxyPlayerEntity *proxy = ProxyPart(entity))  return proxy->trumbleCount(); PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getTrumbleCount() : std::size_t{}; }
oo::PList ByName_571(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->trumbleValue() : oo::PList(); }
void ByName_572(Entity *entity, const oo::PList &trumbleValue)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setTrumbleValueFrom(trumbleValue); }
float ByName_573(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->trumbleAppetiteAccumulator() : float{}; }
void ByName_574(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setScoopsActive(); }
void ByName_575(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearTargetMemory(); }
Quaternion ByName_576(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewQuaternion() : kZeroQuaternion; }
OOMatrix ByName_577(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewMatrix() : kZeroMatrix; }
Vector ByName_578(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewOffset() : kZeroVector; }
Vector ByName_579(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewRotationCenter() : kZeroVector; }
Vector ByName_580(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewForwardVector() : kZeroVector; }
Vector ByName_581(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewUpVector() : kZeroVector; }
Vector ByName_582(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomViewRightVector() : kZeroVector; }
void ByName_583(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetCustomView(); }
void ByName_584(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setCustomViewData(); }
bool ByName_585(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->showInfoFlag() : false; }
oo::PList ByName_586(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionOverlayDescriptor() : oo::PList(); }
oo::PList ByName_587(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionOverlayDescriptorOrDefault() : oo::PList(); }
void ByName_588(Entity *entity, const oo::PList &descriptor)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionOverlayDescriptor(descriptor); }
oo::PList ByName_589(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionBackgroundDescriptor() : oo::PList(); }
oo::PList ByName_590(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionBackgroundDescriptorOrDefault() : oo::PList(); }
void ByName_591(Entity *entity, const oo::PList &descriptor)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionBackgroundDescriptor(descriptor); }
OOGUIBackgroundSpecial ByName_592(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionBackgroundSpecial() : OOGUIBackgroundSpecial{}; }
void ByName_593(Entity *entity, const std::string &special)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionBackgroundSpecial(special); }
oo::GUIScreenID ByName_594(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionExitScreen() : oo::GUIScreenID{}; }
oo::PList ByName_595(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->equipScreenBackgroundDescriptor() : oo::PList(); }
void ByName_596(Entity *entity, const oo::PList &descriptor)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setEquipScreenBackgroundDescriptor(descriptor); }
bool ByName_597(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptsLoaded() : false; }
oo::GalacticHyperspaceBehaviour ByName_598(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getGalacticHyperspaceBehaviour() : oo::GalacticHyperspaceBehaviour{}; }
NSPoint ByName_599(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getGalacticHyperspaceFixedCoords() : NSMakePoint(0, 0); }
oo::LongRangeChartMode ByName_600(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getLongRangeChartMode() : oo::LongRangeChartMode{}; }
bool ByName_601(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getScoopOverride() : false; }
bool ByName_602(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->isDocked() : false; }
bool ByName_603(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clearedToDock() : false; }
OODockingClearanceStatus ByName_604(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getDockingClearanceStatus() : OODockingClearanceStatus{}; }
void ByName_605(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->penaltyForUnauthorizedDocking(); }
void ByName_606(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateWormholes(); }
void ByName_607(Entity *entity, const oo::PList &marker)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addMissionDestinationMarker(marker); }
bool ByName_608(Entity *entity, const oo::PList &marker)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->removeMissionDestinationMarker(marker) : false; }
oo::PList ByName_609(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getMissionDestinations() : oo::PList(); }
void ByName_610(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearExtraMissionKeys(); }
void ByName_611(Entity *entity, const oo::PList &keys)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setExtraMissionKeys(keys); }
unsigned ByName_612(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->score() : unsigned{}; }
double ByName_613(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->creditBalance() : double{}; }
bool ByName_614(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dockedAtMainStation() : false; }
void ByName_615(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetScannerZoom(); }
OOGalaxyID ByName_616(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->currentGalaxyID() : OOGalaxyID{}; }
OOSystemID ByName_617(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->currentSystemID() : OOSystemID{}; }
void ByName_618(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->allowMissionInterrupt(); }
OOTimeDelta ByName_619(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptTimer() : OOTimeDelta{}; }
unsigned ByName_620(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemPseudoRandom100() : unsigned{}; }
unsigned ByName_621(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemPseudoRandom256() : unsigned{}; }
double ByName_622(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemPseudoRandomFloat() : double{}; }
oo::PList ByName_623(Entity *entity, const oo::PList &marker)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->validatedMarker(marker) : oo::PList(); }
oo::PList ByName_624(Entity *entity)
{
	PlayerEntity *player = PlayerEntityPart(entity);
	if (player == nullptr)  return oo::PList();
 const auto result = player->commanderKillsAsString(); return result.has_value() ? oo::PList(*result) : oo::PList();
}
oo::PList ByName_625(Entity *entity)
{
	PlayerEntity *player = PlayerEntityPart(entity);
	if (player == nullptr)  return oo::PList();
 const auto result = player->commanderBountyAsString(); return result.has_value() ? oo::PList(*result) : oo::PList();
}
oo::PList ByName_626(Entity *entity)
{
	PlayerEntity *player = PlayerEntityPart(entity);
	if (player == nullptr)  return oo::PList();
 const auto result = player->creditsFormattedForSubstitution(); return result.has_value() ? oo::PList(*result) : oo::PList();
}
oo::PList ByName_627(Entity *entity)
{
	PlayerEntity *player = PlayerEntityPart(entity);
	if (player == nullptr)  return oo::PList();
 const auto result = player->creditsFormattedForLegacySubstitution(); return result.has_value() ? oo::PList(*result) : oo::PList();
}
void ByName_628(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setUpSound(); }
void ByName_629(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setUpWeaponSounds(); }
void ByName_630(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->destroySound(); }
bool ByName_631(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->isBeeping() : false; }
void ByName_632(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->boop(); }
void ByName_633(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playIdentOn(); }
void ByName_634(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playIdentOff(); }
void ByName_635(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playIdentLockedOn(); }
void ByName_636(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMissileArmed(); }
void ByName_637(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMineArmed(); }
void ByName_638(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMissileSafe(); }
void ByName_639(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMissileLockedOn(); }
void ByName_640(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playNextEquipmentSelected(); }
void ByName_641(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playNextMissileSelected(); }
void ByName_642(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWeaponsOnline(); }
void ByName_643(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWeaponsOffline(); }
void ByName_644(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCargoJettisioned(); }
void ByName_645(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAutopilotOn(); }
void ByName_646(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAutopilotOff(); }
void ByName_647(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAutopilotOutOfRange(); }
void ByName_648(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAutopilotCannotDockWithTarget(); }
void ByName_649(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playSaveOverwriteYes(); }
void ByName_650(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playSaveOverwriteNo(); }
void ByName_651(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHoldFull(); }
void ByName_652(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playJumpMassLocked(); }
void ByName_653(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playTargetLost(); }
void ByName_654(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playNoTargetInMemory(); }
void ByName_655(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playTargetSwitched(); }
void ByName_656(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHyperspaceNoTarget(); }
void ByName_657(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHyperspaceNoFuel(); }
void ByName_658(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHyperspaceBlocked(); }
void ByName_659(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHyperspaceDistanceTooGreat(); }
void ByName_660(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCloakingDeviceOn(); }
void ByName_661(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCloakingDeviceOff(); }
void ByName_662(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMenuNavigationUp(); }
void ByName_663(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMenuNavigationDown(); }
void ByName_664(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMenuNavigationNot(); }
void ByName_665(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMenuPagePrevious(); }
void ByName_666(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playMenuPageNext(); }
void ByName_667(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playDismissedReportScreen(); }
void ByName_668(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playDismissedMissionScreen(); }
void ByName_669(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playChangedOption(); }
void ByName_670(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateAfterburnerSound(); }
void ByName_671(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->startAfterburnerSound(); }
void ByName_672(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->stopAfterburnerSound(); }
void ByName_673(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCloakingDeviceInsufficientEnergy(); }
void ByName_674(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playBuyCommodity(); }
void ByName_675(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playBuyShip(); }
void ByName_676(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playSellCommodity(); }
void ByName_677(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCantBuyCommodity(); }
void ByName_678(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCantSellCommodity(); }
void ByName_679(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playCantBuyShip(); }
void ByName_680(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playStandardHyperspace(); }
void ByName_681(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playGalacticHyperspace(); }
void ByName_682(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHyperspaceAborted(); }
void ByName_683(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHitByECMSound(); }
void ByName_684(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playFiredECMSound(); }
void ByName_685(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playLaunchFromStation(); }
void ByName_686(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playDockWithStation(); }
void ByName_687(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playExitWitchspace(); }
void ByName_688(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playHostileWarning(); }
void ByName_689(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAlertConditionRed(); }
void ByName_690(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playEnergyLow(); }
void ByName_691(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playDockingDenied(); }
void ByName_692(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWitchjumpFailure(); }
void ByName_693(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWitchjumpMisjump(); }
void ByName_694(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWitchjumpBlocked(); }
void ByName_695(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWitchjumpDistanceTooGreat(); }
void ByName_696(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playWitchjumpInsufficientFuel(); }
void ByName_697(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playFuelLeak(); }
void ByName_698(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playEscapePodScooped(); }
void ByName_699(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAegisCloseToPlanet(); }
void ByName_700(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playAegisCloseToStation(); }
void ByName_701(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playGameOver(); }
void ByName_702(Entity *entity, const std::string &key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playLegacyScriptSound(key); }
void ByName_703(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  oo::ScheduleEntityCall(player, [](Entity *target) { if (PlayerEntity *part = PlayerEntityPart(target))  part->updateAfterburnerSound(); }, 1.25); }
void ByName_704(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetStickFunctions(); }
void ByName_705(Entity *entity, const oo::PList &hwDict)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->updateFunction(hwDict); }
oo::PList ByName_706(Entity *entity, const std::string &header)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->makeStickGuiDictHeader(header) : oo::PList(); }
void ByName_707(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->initControls(); }
void ByName_708(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->initKeyConfigSettings(); }
oo::PList ByName_709(Entity *entity, const oo::PList &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->processKeyCode(key_def) : oo::PList(); }
bool ByName_710(Entity *entity, const oo::PList &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->checkNavKeyPress(key_def) : false; }
bool ByName_711(Entity *entity, const oo::PList &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->checkKeyPress(key_def) : false; }
int ByName_712(Entity *entity, const oo::PList &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getFirstKeyCode(key_def) : int{}; }
bool ByName_713(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->handleGUIUpDownArrowKeys() : false; }
void ByName_714(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearPlanetSearchString(); }
void ByName_715(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->switchToMainView(); }
void ByName_716(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->beginWitchspaceCountdown(); }
void ByName_717(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->cancelWitchspaceCountdown(); }
void ByName_718(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollApplicationControls(); }
void ByName_719(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollMarketScreenControls(); }
void ByName_720(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleGameOptionsScreenKeys(); }
void ByName_721(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleKeyMapperScreenKeys(); }
void ByName_722(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleKeyboardLayoutKeys(); }
void ByName_723(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleStickMapperScreenKeys(); }
void ByName_724(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollCustomViewControls(); }
void ByName_725(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollViewControls(); }
void ByName_726(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollGuiScreenControls(); }
void ByName_727(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleUndockControl(); }
void ByName_728(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->pollMissionInterruptControls(); }
void ByName_729(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleMissionCallback(); }
void ByName_730(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToMissionEndScreen(); }
void ByName_731(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleButtonIdent(); }
void ByName_732(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->handleButtonTargetMissile(); }
void ByName_733(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->initCheckingDictionary(); }
void ByName_734(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetKeyFunctions(); }
bool ByName_735(Entity *entity, const oo::PList &dict)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->entryIsDictCustomEquip(dict) : false; }
bool ByName_736(Entity *entity, const std::string &entry)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->entryIsCustomEquip(entry) : false; }
oo::PList ByName_737(Entity *entity, const std::string &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomEquipArray(key_def) : oo::PList(); }
std::size_t ByName_738(Entity *entity, const std::string &key_def)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getCustomEquipIndex(key_def) : std::size_t{}; }
void ByName_739(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToKeyConfigScreen(); }
void ByName_740(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToKeyConfigEntryScreen(); }
void ByName_741(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToConfirmClearScreen(); }
oo::PList ByName_742(Entity *entity, const std::string &header)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->makeKeyGuiDictHeader(header) : oo::PList(); }
bool ByName_743(Entity *entity, const std::string &key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->entryIsEqualToDefault(key) : false; }
void ByName_744(Entity *entity, const std::string &key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->saveKeySetting(key); }
void ByName_745(Entity *entity, const std::string &key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->unsetKeySetting(key); }
void ByName_746(Entity *entity, const std::string &key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->deleteKeySetting(key); }
void ByName_747(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->deleteAllKeySettings(); }
oo::PList ByName_748(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->loadKeySettings() : oo::PList(); }
void ByName_749(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->reloadPage(); }
ShipEntity * ByName_750(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptTarget() : nullptr; }
void ByName_751(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->checkScript(); }
bool ByName_752(Entity *entity, const oo::PList &array)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptTestConditions(array) : false; }
bool ByName_753(Entity *entity, const oo::PList &scriptCondition)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptTestCondition(scriptCondition) : false; }
oo::PList ByName_754(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionVariables() : oo::PList(); }
oo::PList ByName_755(Entity *entity, const std::string &key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionVariableForKey(key) : oo::PList(); }
oo::PList ByName_756(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionsList() : oo::PList(); }
void ByName_757(Entity *entity, const std::string &textKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionDescription(textKey); }
void ByName_758(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearMissionDescription(); }
void ByName_759(Entity *entity, const std::string &key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearMissionDescriptionForMission(key); }
oo::PList ByName_760(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->mission_string() : oo::PList(); }
oo::PList ByName_761(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->status_string() : oo::PList(); }
oo::PList ByName_762(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->gui_screen_string() : oo::PList(); }
oo::PList ByName_763(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getGalaxy_number() : oo::PList(); }
oo::PList ByName_764(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->planet_number() : oo::PList(); }
oo::PList ByName_765(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->score_number() : oo::PList(); }
oo::PList ByName_766(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->credits_number() : oo::PList(); }
oo::PList ByName_767(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->scriptTimer_number() : oo::PList(); }
oo::PList ByName_768(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->shipsFound_number() : oo::PList(); }
oo::PList ByName_769(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderLegalStatus_number() : oo::PList(); }
void ByName_770(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setLegalStatus(valueString); }
oo::PList ByName_771(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderLegalStatus_string() : oo::PList(); }
oo::PList ByName_772(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->d100_number() : oo::PList(); }
oo::PList ByName_773(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->pseudoFixedD100_number() : oo::PList(); }
oo::PList ByName_774(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->d256_number() : oo::PList(); }
oo::PList ByName_775(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->pseudoFixedD256_number() : oo::PList(); }
oo::PList ByName_776(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clock_number() : oo::PList(); }
oo::PList ByName_777(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clock_secs_number() : oo::PList(); }
oo::PList ByName_778(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clock_mins_number() : oo::PList(); }
oo::PList ByName_779(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clock_hours_number() : oo::PList(); }
oo::PList ByName_780(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->clock_days_number() : oo::PList(); }
oo::PList ByName_781(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fuelLevel_number() : oo::PList(); }
oo::PList ByName_782(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dockedAtMainStation_bool() : oo::PList(); }
oo::PList ByName_783(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->foundEquipment_bool() : oo::PList(); }
oo::PList ByName_784(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->sunWillGoNova_bool() : oo::PList(); }
oo::PList ByName_785(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->sunGoneNova_bool() : oo::PList(); }
oo::PList ByName_786(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionChoice_string() : oo::PList(); }
oo::PList ByName_787(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missionKeyPress_string() : oo::PList(); }
oo::PList ByName_788(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dockedTechLevel_number() : oo::PList(); }
oo::PList ByName_789(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->dockedStationName_string() : oo::PList(); }
oo::PList ByName_790(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemGovernment_string() : oo::PList(); }
oo::PList ByName_791(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemGovernment_number() : oo::PList(); }
oo::PList ByName_792(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemEconomy_string() : oo::PList(); }
oo::PList ByName_793(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemEconomy_number() : oo::PList(); }
oo::PList ByName_794(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemTechLevel_number() : oo::PList(); }
oo::PList ByName_795(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemPopulation_number() : oo::PList(); }
oo::PList ByName_796(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->systemProductivity_number() : oo::PList(); }
oo::PList ByName_797(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderName_string() : oo::PList(); }
oo::PList ByName_798(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderRank_string() : oo::PList(); }
oo::PList ByName_799(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderShip_string() : oo::PList(); }
oo::PList ByName_800(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->commanderShipDisplayName_string() : oo::PList(); }
void ByName_801(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->consoleMessage3s(valueString); }
void ByName_802(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->consoleMessage6s(valueString); }
void ByName_803(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->awardCredits(valueString); }
void ByName_804(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->awardShipKills(valueString); }
void ByName_805(Entity *entity, const std::string &equipString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->awardEquipment(equipString); }
void ByName_806(Entity *entity, const std::string &equipString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->removeEquipment(equipString); }
void ByName_807(Entity *entity, const std::string &key_valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setPlanetinfo(key_valueString); }
void ByName_808(Entity *entity, const std::string &key_valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setSpecificPlanetInfo(key_valueString); }
void ByName_809(Entity *entity, const std::string &amount_typeString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->awardCargo(amount_typeString); }
void ByName_810(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->removeAllCargo(); }
void ByName_811(Entity *entity, const std::string &descriptionString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->useSpecialCargo(descriptionString); }
void ByName_812(Entity *entity, const std::string &equipString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->testForEquipment(equipString); }
void ByName_813(Entity *entity, const std::string &valueString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->awardFuel(valueString); }
void ByName_814(Entity *entity, const std::string &roles_message)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->messageShipAIs(roles_message); }
void ByName_815(Entity *entity, const std::string &itemKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->ejectItem(itemKey); }
void ByName_816(Entity *entity, const std::string &roles_number)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addShips(roles_number); }
void ByName_817(Entity *entity, const std::string &roles_number_position)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addSystemShips(roles_number_position); }
void ByName_818(Entity *entity, const std::string &roles_number_system_x_y_z)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addShipsAt(roles_number_system_x_y_z); }
void ByName_819(Entity *entity, const std::string &roles_number_system_x_y_z)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addShipsAtPrecisely(roles_number_system_x_y_z); }
void ByName_820(Entity *entity, const std::string &roles_number_system_x_y_z_r)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addShipsWithinRadius(roles_number_system_x_y_z_r); }
void ByName_821(Entity *entity, const std::string &ship_key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->spawnShip(ship_key); }
void ByName_822(Entity *entity, const std::string &missionvariable_value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->set(missionvariable_value); }
void ByName_823(Entity *entity, const std::string &missionvariable)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->reset(missionvariable); }
void ByName_824(Entity *entity, const std::string &missionVariableObject)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->increment(missionVariableObject); }
void ByName_825(Entity *entity, const std::string &missionVariableObject)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->decrement(missionVariableObject); }
void ByName_826(Entity *entity, const std::string &missionVariableString_value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->add(missionVariableString_value); }
void ByName_827(Entity *entity, const std::string &missionVariableString_value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->subtract(missionVariableString_value); }
void ByName_828(Entity *entity, const std::string &roleString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->checkForShips(roleString); }
void ByName_829(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetScriptTimer(); }
void ByName_830(Entity *entity, const std::string &textKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addMissionText(textKey); }
void ByName_831(Entity *entity, const std::string &text)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addLiteralMissionText(text); }
void ByName_832(Entity *entity, const std::string &choicesKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionChoices(choicesKey); }
void ByName_833(Entity *entity, const oo::PList &choicesDict)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionChoicesDictionary(choicesDict); }
void ByName_834(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->resetMissionChoice(); }
void ByName_835(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearMissionScreen(); }
void ByName_836(Entity *entity, const std::string &destinations)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addMissionDestination(destinations); }
void ByName_837(Entity *entity, const std::string &destinations)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->removeMissionDestination(destinations); }
void ByName_838(Entity *entity, const std::string &role)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showShipModel(role); }
void ByName_839(Entity *entity, const std::string &value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionMusic(value); }
void ByName_840(Entity *entity, const std::string &value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionImage(value); }
void ByName_841(Entity *entity, const std::string &value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setMissionBackground(value); }
void ByName_842(Entity *entity, const std::string &value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setFuelLeak(value); }
oo::PList ByName_843(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->fuelLeakRate_number() : oo::PList(); }
void ByName_844(Entity *entity, const std::string &time_value)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setSunNovaIn(time_value); }
void ByName_845(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->launchFromStation(); }
void ByName_846(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->blowUpStation(); }
void ByName_847(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->sendAllShipsAway(); }
void ByName_848(Entity *entity, const std::string &planetKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addPlanet(planetKey); }
void ByName_849(Entity *entity, const std::string &moonKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addMoon(moonKey); }
void ByName_850(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->debugOn(); }
void ByName_851(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->debugOff(); }
void ByName_852(Entity *entity, const std::string &args)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->debugMessage(args); }
void ByName_853(Entity *entity, const std::string &soundName)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->playSound(soundName); }
void ByName_854(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->doMissionCallback(); }
void ByName_855(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->clearMissionScreenID(); }
void ByName_856(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->endMissionScreenAndNoteOpportunity(); }
void ByName_857(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToMissionScreen(); }
void ByName_858(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->refreshMissionScreenTextEntry(); }
void ByName_859(Entity *entity, const std::string &d_key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setBackgroundFromDescriptionsKey(d_key); }
bool ByName_860(Entity *entity, const std::string &eq_key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->addEqScriptForKey(eq_key) : false; }
void ByName_861(Entity *entity, const std::string &eq_key)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->removeEqScriptForKey(eq_key); }
std::size_t ByName_862(Entity *entity, const std::string &eq_key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->eqScriptIndexForKey(eq_key) : std::size_t{}; }
void ByName_863(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->targetNearestHostile(); }
void ByName_864(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->targetNearestIncomingMissile(); }
void ByName_865(Entity *entity, const std::string &galacticHyperspaceBehaviourString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGalacticHyperspaceBehaviourTo(galacticHyperspaceBehaviourString); }
void ByName_866(Entity *entity, const std::string &galacticHyperspaceFixedCoordsString)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGalacticHyperspaceFixedCoordsTo(galacticHyperspaceFixedCoordsString); }
OOCargoQuantity ByName_867(Entity *entity, const std::string &good)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->contractedVolumeForGood(good) : OOCargoQuantity{}; }
void ByName_868(Entity *entity, const std::string &report)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addMessageToReport(report); }
oo::PList ByName_869(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->getReputation() : oo::PList(); }
int ByName_870(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->passengerReputation() : int{}; }
int ByName_871(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->parcelReputation() : int{}; }
int ByName_872(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->contractReputation() : int{}; }
void ByName_873(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->erodeReputation(); }
void ByName_874(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->normaliseReputation(); }
bool ByName_875(Entity *entity, const std::string &Name)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->removePassenger(Name) : false; }
bool ByName_876(Entity *entity, const std::string &Name)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->removeParcel(Name) : false; }
void ByName_877(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToManifestScreen(); }
void ByName_878(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToDockingReportScreen(); }
OOCreditsQuantity ByName_879(Entity *entity, const std::string &key)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->priceForShipKey(key) : OOCreditsQuantity{}; }
void ByName_880(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showShipyardInfoForSelection(); }
void ByName_881(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showTradeInInformationFooter(); }
std::ptrdiff_t ByName_882(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->missingSubEntitiesAdjustment() : std::ptrdiff_t{}; }
OOCreditsQuantity ByName_883(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->tradeInValue() : OOCreditsQuantity{}; }
bool ByName_884(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->buySelectedShip() : false; }
bool ByName_885(Entity *entity, const std::string &shipKey)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->replaceShipWithNamedShip(shipKey) : false; }
bool ByName_886(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->loadPlayer() : false; }
void ByName_887(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->savePlayer(); }
void ByName_888(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->autosavePlayer(); }
void ByName_889(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->quicksavePlayer(); }
void ByName_890(Entity *entity, const std::string &shipKey)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->addScenarioModel(shipKey); }
void ByName_891(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->showScenarioDetails(); }
bool ByName_892(Entity *entity)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->startScenario() : false; }
void ByName_893(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->saveCommanderInputHandler(); }
void ByName_894(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->overwriteCommanderInputHandler(); }
void ByName_895(Entity *entity, const std::string &path)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->writePlayerToPath(path); }
void ByName_896(Entity *entity, const std::string &cdrName)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->nativeSavePlayer(cdrName); }
void ByName_897(Entity *entity)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToLoadCommanderScreen(); }
void ByName_898(Entity *entity, const std::string &cdrName)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToSaveCommanderScreen(cdrName); }
void ByName_899(Entity *entity, const std::string &cdrName)
{ if (PlayerEntity *player = PlayerEntityPart(entity))  player->setGuiToOverwriteScreen(cdrName); }
bool ByName_900(Entity *entity, const std::string &cdrName)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->existingNativeSave(cdrName) : false; }
int ByName_901(Entity *entity, const std::string &cdrName)
{ PlayerEntity *player = PlayerEntityPart(entity); return player != nullptr ? player->findIndexOfCommander(cdrName) : int{}; }
OOTechLevelID ByName_902(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getEquivalentTechLevel() : OOTechLevelID{}; }
Vector ByName_903(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->virtualPortDimensions() : kZeroVector; }
Entity * ByName_904(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->playerReservedDock() : nullptr; }
HPVector ByName_905(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->beaconPosition() : kZeroHPVector; }
float ByName_906(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getEquipmentPriceFactor() : float{}; }
OOCargoQuantity ByName_907(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getMarketCapacity() : OOCargoQuantity{}; }
oo::PList ByName_908(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getMarketDefinition() : oo::PList(); }
bool ByName_909(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getMarketMonitored() : false; }
bool ByName_910(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getMarketBroadcast() : false; }
void ByName_911(Entity *entity, const oo::PList &market)
{ if (StationEntity *station = StationPart(entity))  station->setLocalMarket(market); }
oo::PList ByName_912(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->localMarketForScripting() : oo::PList(); }
unsigned ByName_913(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->countOfDockedContractors() : unsigned{}; }
unsigned ByName_914(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->countOfDockedPolice() : unsigned{}; }
unsigned ByName_915(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->countOfDockedDefenders() : unsigned{}; }
bool ByName_916(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getInterstellarUndockingAllowed() : false; }
bool ByName_917(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getHasNPCTraffic() : false; }
bool ByName_918(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getRequiresDockingClearance() : false; }
bool ByName_919(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getAllowsFastDocking() : false; }
bool ByName_920(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getAllowsAutoDocking() : false; }
bool ByName_921(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getAllowsSaving() : false; }
bool ByName_922(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->isRotatingStation() : false; }
bool ByName_923(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->hasShipyard() : false; }
void ByName_924(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->generateShipyard(); }
bool ByName_925(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->suppressArrivalReports() : false; }
bool ByName_926(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getHasBreakPattern() : false; }
void ByName_927(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->sanityCheckShipsOnApproach(); }
void ByName_928(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->autoDockShipsOnHold(); }
void ByName_929(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->autoDockShipsOnApproach(); else if (DockEntity *dock = DockPart(entity))  dock->autoDockShipsOnApproach(); }
bool ByName_930(Entity *entity)
{ if (DockEntity *dock = DockPart(entity))  return dock->dockingCorridorIsEmpty(); StationEntity *station = StationPart(entity); return station != nullptr ? station->dockingCorridorIsEmpty() : false; }
void ByName_931(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->clearDockingCorridor(); else if (DockEntity *dock = DockPart(entity))  dock->clearDockingCorridor(); }
void ByName_932(Entity *entity)
{ if (StationEntity *station = StationPart(entity))  station->clear(); else if (DockEntity *dock = DockPart(entity))  dock->clear(); }
bool ByName_933(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->hasMultipleDocks() : false; }
bool ByName_934(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->hasClearDock() : false; }
bool ByName_935(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->hasEligibleDock() : false; }
bool ByName_936(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->hasLaunchDock() : false; }
Entity * ByName_937(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->selectDockForDocking() : nullptr; }
unsigned ByName_938(Entity *entity, const std::string &role)
{ if (DockEntity *dock = DockPart(entity))  return static_cast<unsigned>(dock->countOfShipsInLaunchQueueWithPrimaryRole(role)); StationEntity *station = StationPart(entity); return station != nullptr ? station->countOfShipsInLaunchQueueWithPrimaryRole(role) : unsigned{}; }
oo::StationAlertLevel ByName_939(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->getAlertLevel() : oo::StationAlertLevel::kGreen; }
unsigned ByName_940(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->currentlyInDockingQueues() : unsigned{}; }
unsigned ByName_941(Entity *entity)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->currentlyInLaunchingQueues() : unsigned{}; }
oo::PList ByName_942(Entity *entity, const std::string &role)
{ StationEntity *station = StationPart(entity); return station != nullptr ? station->launchIndependentShip(role) : oo::PList(); }
bool ByName_943(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->allowsDocking() : false; }
bool ByName_944(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->disallowedDockingCollides() : false; }
std::size_t ByName_945(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->countOfShipsInDockingQueue() : std::size_t{}; }
bool ByName_946(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->allowsLaunching() : false; }
std::size_t ByName_947(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->countOfShipsInLaunchQueue() : std::size_t{}; }
bool ByName_948(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->isOffCentre() : false; }
void ByName_949(Entity *entity)
{ if (DockEntity *dock = DockPart(entity))  dock->setVirtual(); }
void ByName_950(Entity *entity)
{ if (DockEntity *dock = DockPart(entity))  dock->clearAllIdLocks(); }
std::size_t ByName_951(Entity *entity)
{ DockEntity *dock = DockPart(entity); return dock != nullptr ? dock->pruneAndCountShipsOnApproach() : std::size_t{}; }
void ByName_952(Entity *entity)
{ if (DockEntity *dock = DockPart(entity))  dock->abortAllLaunches(); }
bool ByName_953(Entity *entity, const oo::PList &effectDict)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->setUpVisualEffectFromDictionary(effectDict) : false; }
void ByName_954(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  effect->setNoDrawDistance(); }
GLfloat ByName_955(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->scaleMax() : GLfloat{}; }
GLfloat ByName_956(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->scaleX() : GLfloat{}; }
GLfloat ByName_957(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->scaleY() : GLfloat{}; }
GLfloat ByName_958(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->scaleZ() : GLfloat{}; }
bool ByName_959(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->isBreakPattern() : false; }
oo::PList ByName_960(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->effectInfoDictionary() : oo::PList(); }
void ByName_961(Entity *entity)
{ if (oo::VisualEffectEntity *effect = EffectPart(entity))  effect->remove(); }
GLfloat ByName_962(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderFloat1() : GLfloat{}; }
GLfloat ByName_963(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderFloat2() : GLfloat{}; }
int ByName_964(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderInt1() : 0; }
int ByName_965(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderInt2() : 0; }
Vector ByName_966(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderVector1() : kZeroVector; }
Vector ByName_967(Entity *entity)
{ oo::VisualEffectEntity *effect = EffectPart(entity); return effect != nullptr ? effect->shaderVector2() : kZeroVector; }

}	// namespace


// name, applies-to flags, the encoding of the result, argument kind, adaptor, typed adaptor (no argument)
namespace oo {

inline const EntityByNameRow kEntityByNameRows[] =
{
	{ "setUpFromDictionary:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_0>,	nullptr },
	{ "setUpShipFromDictionary:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_1>,	nullptr },
	{ "subIdx",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_2>,	&EntityByNameTyped<&ByName_2> },
	{ "maxShipSubEntities",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_3>,	&EntityByNameTyped<&ByName_3> },
	{ "deserializeShipSubEntitiesFrom:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_4>,	nullptr },
	{ "setUpSubEntities",	ByName::kShip | ByName::kEffect,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_5>,	&EntityByNameTyped<&ByName_5> },
	{ "setUpOneSubentity:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_6>,	nullptr },
	{ "setUpOneFlasher:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_7>,	nullptr },
	{ "isTemplateCargoPod",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_8>,	&EntityByNameTyped<&ByName_8> },
	{ "setUpCargoType:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_9>,	nullptr },
	{ "removeScript",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_10>,	&EntityByNameTyped<&ByName_10> },
	{ "clearSubEntities",	ByName::kShip | ByName::kEffect,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_11>,	&EntityByNameTyped<&ByName_11> },
	{ "subEntityRotationalVelocity",	ByName::kShip,	oo::kValueType<Quaternion>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_12>,	&EntityByNameTyped<&ByName_12> },
	{ "sunGlareFilter",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_13>,	&EntityByNameTyped<&ByName_13> },
	{ "accuracy",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_14>,	&EntityByNameTyped<&ByName_14> },
	{ "forwardVector",	ByName::kShip | ByName::kEffect,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_15>,	&EntityByNameTyped<&ByName_15> },
	{ "upVector",	ByName::kShip | ByName::kEffect,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_16>,	&EntityByNameTyped<&ByName_16> },
	{ "rightVector",	ByName::kShip | ByName::kEffect,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_17>,	&EntityByNameTyped<&ByName_17> },
	{ "scriptedMisjump",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_18>,	&EntityByNameTyped<&ByName_18> },
	{ "scriptedMisjumpRange",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_19>,	&EntityByNameTyped<&ByName_19> },
	{ "subEntityCount",	ByName::kShip | ByName::kEffect,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_20>,	&EntityByNameTyped<&ByName_20> },
	{ "subEntityTakingDamage",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_21>,	&EntityByNameTyped<&ByName_21> },
	{ "shipAIScriptWakeTime",	ByName::kShip,	oo::kValueType<OOTimeAbsolute>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_22>,	&EntityByNameTyped<&ByName_22> },
	{ "volume",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_23>,	&EntityByNameTyped<&ByName_23> },
	{ "absoluteTractorPosition",	ByName::kShip,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_24>,	&EntityByNameTyped<&ByName_24> },
	{ "isBoulder",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_25>,	&EntityByNameTyped<&ByName_25> },
	{ "isMinable",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_26>,	&EntityByNameTyped<&ByName_26> },
	{ "countsAsKill",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_27>,	&EntityByNameTyped<&ByName_27> },
	{ "setUpEscorts",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_28>,	&EntityByNameTyped<&ByName_28> },
	{ "setUpMixedEscorts",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_29>,	&EntityByNameTyped<&ByName_29> },
	{ "shipInfoDictionary",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_30>,	&EntityByNameTyped<&ByName_30> },
	{ "isFrangible",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_31>,	&EntityByNameTyped<&ByName_31> },
	{ "suppressFlightNotifications",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_32>,	&EntityByNameTyped<&ByName_32> },
	{ "noteFrustration:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_33>,	nullptr },
	{ "countEquipmentItem:",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_34>,	nullptr },
	{ "hasEquipmentItem:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_35>,	nullptr },
	{ "hasEquipmentItemProviding:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_36>,	nullptr },
	{ "hasAllEquipment:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_37>,	nullptr },
	{ "hasHyperspaceMotor",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_38>,	&EntityByNameTyped<&ByName_38> },
	{ "hyperspaceSpinTime",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_39>,	&EntityByNameTyped<&ByName_39> },
	{ "passengerListForScripting",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_40>,	&EntityByNameTyped<&ByName_40> },
	{ "parcelListForScripting",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_41>,	&EntityByNameTyped<&ByName_41> },
	{ "contractListForScripting",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_42>,	&EntityByNameTyped<&ByName_42> },
	{ "equipmentCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_43>,	&EntityByNameTyped<&ByName_43> },
	{ "removeEquipmentItem:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_44>,	nullptr },
	{ "removeAllEquipment",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_45>,	&EntityByNameTyped<&ByName_45> },
	{ "removeMissiles",	ByName::kShip,	oo::kValueType<OOCreditsQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_46>,	&EntityByNameTyped<&ByName_46> },
	{ "parcelCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_47>,	&EntityByNameTyped<&ByName_47> },
	{ "passengerCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_48>,	&EntityByNameTyped<&ByName_48> },
	{ "passengerCapacity",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_49>,	&EntityByNameTyped<&ByName_49> },
	{ "missileCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_50>,	&EntityByNameTyped<&ByName_50> },
	{ "missileCapacity",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_51>,	&EntityByNameTyped<&ByName_51> },
	{ "extraCargo",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_52>,	&EntityByNameTyped<&ByName_52> },
	{ "hasScoop",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_53>,	&EntityByNameTyped<&ByName_53> },
	{ "hasFuelScoop",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_54>,	&EntityByNameTyped<&ByName_54> },
	{ "hasCargoScoop",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_55>,	&EntityByNameTyped<&ByName_55> },
	{ "hasECM",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_56>,	&EntityByNameTyped<&ByName_56> },
	{ "hasCloakingDevice",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_57>,	&EntityByNameTyped<&ByName_57> },
	{ "hasMilitaryScannerFilter",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_58>,	&EntityByNameTyped<&ByName_58> },
	{ "hasMilitaryJammer",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_59>,	&EntityByNameTyped<&ByName_59> },
	{ "hasExpandedCargoBay",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_60>,	&EntityByNameTyped<&ByName_60> },
	{ "hasShieldBooster",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_61>,	&EntityByNameTyped<&ByName_61> },
	{ "hasMilitaryShieldEnhancer",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_62>,	&EntityByNameTyped<&ByName_62> },
	{ "hasHeatShield",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_63>,	&EntityByNameTyped<&ByName_63> },
	{ "hasFuelInjection",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_64>,	&EntityByNameTyped<&ByName_64> },
	{ "hasCascadeMine",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_65>,	&EntityByNameTyped<&ByName_65> },
	{ "hasEscapePod",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_66>,	&EntityByNameTyped<&ByName_66> },
	{ "hasDockingComputer",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_67>,	&EntityByNameTyped<&ByName_67> },
	{ "hasGalacticHyperdrive",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_68>,	&EntityByNameTyped<&ByName_68> },
	{ "shieldBoostFactor",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_69>,	&EntityByNameTyped<&ByName_69> },
	{ "maxForwardShieldLevel",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_70>,	&EntityByNameTyped<&ByName_70> },
	{ "maxAftShieldLevel",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_71>,	&EntityByNameTyped<&ByName_71> },
	{ "shieldRechargeRate",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_72>,	&EntityByNameTyped<&ByName_72> },
	{ "maxHyperspaceDistance",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_73>,	&EntityByNameTyped<&ByName_73> },
	{ "afterburnerFactor",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_74>,	&EntityByNameTyped<&ByName_74> },
	{ "afterburnerRate",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_75>,	&EntityByNameTyped<&ByName_75> },
	{ "maxThrust",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_76>,	&EntityByNameTyped<&ByName_76> },
	{ "thrust",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_77>,	&EntityByNameTyped<&ByName_77> },
	{ "reactionTime",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_78>,	&EntityByNameTyped<&ByName_78> },
	{ "calculateTargetPosition",	ByName::kShip,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_79>,	&EntityByNameTyped<&ByName_79> },
	{ "startTrackingCurve",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_80>,	&EntityByNameTyped<&ByName_80> },
	{ "updateTrackingCurve",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_81>,	&EntityByNameTyped<&ByName_81> },
	{ "calculateTrackingCurve",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_82>,	&EntityByNameTyped<&ByName_82> },
#ifdef OO_DEBUG_MONITOR_ENABLED
	{ "drawDebugStuff",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_83>,	&EntityByNameTyped<&ByName_83> },
#endif
	{ "isCloaked",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_84>,	&EntityByNameTyped<&ByName_84> },
	{ "cloakPassive",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_85>,	&EntityByNameTyped<&ByName_85> },
	{ "hasAutoCloak",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_86>,	&EntityByNameTyped<&ByName_86> },
	{ "avoidCollision",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_87>,	&EntityByNameTyped<&ByName_87> },
	{ "resumePostProximityAlert",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_88>,	&EntityByNameTyped<&ByName_88> },
	{ "messageTime",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_89>,	&EntityByNameTyped<&ByName_89> },
	{ "hasEscorts",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_90>,	&EntityByNameTyped<&ByName_90> },
	{ "escortCount",	ByName::kShip,	oo::kValueType<uint8_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_91>,	&EntityByNameTyped<&ByName_91> },
	{ "pendingEscortCount",	ByName::kShip,	oo::kValueType<uint8_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_92>,	&EntityByNameTyped<&ByName_92> },
	{ "maxEscortCount",	ByName::kShip,	oo::kValueType<uint8_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_93>,	&EntityByNameTyped<&ByName_93> },
	{ "turretCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_94>,	&EntityByNameTyped<&ByName_94> },
	{ "proximityAlert",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_95>,	&EntityByNameTyped<&ByName_95> },
	{ "hasRole:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_96>,	nullptr },
	{ "addRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_97>,	nullptr },
	{ "removeRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_98>,	nullptr },
	{ "setPrimaryRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_99>,	nullptr },
	{ "hasPrimaryRole:",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_100>,	nullptr },
	{ "isPolice",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_101>,	&EntityByNameTyped<&ByName_101> },
	{ "isThargoid",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_102>,	&EntityByNameTyped<&ByName_102> },
	{ "isTrader",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_103>,	&EntityByNameTyped<&ByName_103> },
	{ "isPirate",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_104>,	&EntityByNameTyped<&ByName_104> },
	{ "isMissile",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_105>,	&EntityByNameTyped<&ByName_105> },
	{ "isMine",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_106>,	&EntityByNameTyped<&ByName_106> },
	{ "isWeapon",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_107>,	&EntityByNameTyped<&ByName_107> },
	{ "isEscort",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_108>,	&EntityByNameTyped<&ByName_108> },
	{ "isShuttle",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_109>,	&EntityByNameTyped<&ByName_109> },
	{ "isTurret",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_110>,	&EntityByNameTyped<&ByName_110> },
	{ "isPirateVictim",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_111>,	&EntityByNameTyped<&ByName_111> },
	{ "isExplicitlyUnpiloted",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_112>,	&EntityByNameTyped<&ByName_112> },
	{ "isUnpiloted",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_113>,	&EntityByNameTyped<&ByName_113> },
	{ "hasHostileTarget",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_114>,	&EntityByNameTyped<&ByName_114> },
	{ "weaponRange",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_115>,	&EntityByNameTyped<&ByName_115> },
	{ "energyRechargeRate",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_116>,	&EntityByNameTyped<&ByName_116> },
	{ "weaponRechargeRate",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_117>,	&EntityByNameTyped<&ByName_117> },
	{ "currentWeaponFacing",	ByName::kShip,	oo::kValueType<OOWeaponFacing>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_118>,	&EntityByNameTyped<&ByName_118> },
	{ "scannerRange",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_119>,	&EntityByNameTyped<&ByName_119> },
	{ "reference",	ByName::kShip,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_120>,	&EntityByNameTyped<&ByName_120> },
	{ "reportAIMessages",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_121>,	&EntityByNameTyped<&ByName_121> },
	{ "transitionToAegisNone",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_122>,	&EntityByNameTyped<&ByName_122> },
	{ "findNearestStellarBody",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_123>,	&EntityByNameTyped<&ByName_123> },
	{ "checkForAegis",	ByName::kShip,	oo::kValueType<OOAegisStatus>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_124>,	&EntityByNameTyped<&ByName_124> },
	{ "forceAegisCheck",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_125>,	&EntityByNameTyped<&ByName_125> },
	{ "withinStationAegis",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_126>,	&EntityByNameTyped<&ByName_126> },
	{ "lastAegisLock",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_127>,	&EntityByNameTyped<&ByName_127> },
	{ "homeSystem",	ByName::kShip,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_128>,	&EntityByNameTyped<&ByName_128> },
	{ "destinationSystem",	ByName::kShip,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_129>,	&EntityByNameTyped<&ByName_129> },
	{ "setSingleCrewWithRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_130>,	nullptr },
	{ "setStateMachine:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_131>,	nullptr },
	{ "hasAutoAI",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_132>,	&EntityByNameTyped<&ByName_132> },
	{ "hasNewAI",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_133>,	&EntityByNameTyped<&ByName_133> },
	{ "hasAutoWeapons",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_134>,	&EntityByNameTyped<&ByName_134> },
	{ "frustration",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_135>,	&EntityByNameTyped<&ByName_135> },
	{ "fuel",	ByName::kShip,	oo::kValueType<OOFuelQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_136>,	&EntityByNameTyped<&ByName_136> },
	{ "fuelCapacity",	ByName::kShip,	oo::kValueType<OOFuelQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_137>,	&EntityByNameTyped<&ByName_137> },
	{ "fuelChargeRate",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_138>,	&EntityByNameTyped<&ByName_138> },
	{ "bounty",	ByName::kShip,	oo::kValueType<OOCreditsQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_139>,	&EntityByNameTyped<&ByName_139> },
	{ "legalStatus",	ByName::kShip,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_140>,	&EntityByNameTyped<&ByName_140> },
	{ "commodityAmount",	ByName::kShip,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_141>,	&EntityByNameTyped<&ByName_141> },
	{ "maxAvailableCargoSpace",	ByName::kShip,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_142>,	&EntityByNameTyped<&ByName_142> },
	{ "availableCargoSpace",	ByName::kShip,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_143>,	&EntityByNameTyped<&ByName_143> },
	{ "cargoQuantityOnBoard",	ByName::kShip,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_144>,	&EntityByNameTyped<&ByName_144> },
	{ "cargoType",	ByName::kShip,	oo::kValueType<OOCargoType>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_145>,	&EntityByNameTyped<&ByName_145> },
	{ "cargoCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_146>,	&EntityByNameTyped<&ByName_146> },
	{ "cargoListForScripting",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_147>,	&EntityByNameTyped<&ByName_147> },
	{ "showScoopMessage",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_148>,	&EntityByNameTyped<&ByName_148> },
	{ "desiredSpeed",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_149>,	&EntityByNameTyped<&ByName_149> },
	{ "desiredRange",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_150>,	&EntityByNameTyped<&ByName_150> },
	{ "cruiseSpeed",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_151>,	&EntityByNameTyped<&ByName_151> },
	{ "flightRoll",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_152>,	&EntityByNameTyped<&ByName_152> },
	{ "flightPitch",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_153>,	&EntityByNameTyped<&ByName_153> },
	{ "flightYaw",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_154>,	&EntityByNameTyped<&ByName_154> },
	{ "flightSpeed",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_155>,	&EntityByNameTyped<&ByName_155> },
	{ "maxFlightPitch",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_156>,	&EntityByNameTyped<&ByName_156> },
	{ "maxFlightSpeed",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_157>,	&EntityByNameTyped<&ByName_157> },
	{ "maxFlightRoll",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_158>,	&EntityByNameTyped<&ByName_158> },
	{ "maxFlightYaw",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_159>,	&EntityByNameTyped<&ByName_159> },
	{ "speedFactor",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_160>,	&EntityByNameTyped<&ByName_160> },
	{ "temperature",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_161>,	&EntityByNameTyped<&ByName_161> },
	{ "randomEjectaTemperature",	ByName::kShip,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_162>,	&EntityByNameTyped<&ByName_162> },
	{ "heatInsulation",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_163>,	&EntityByNameTyped<&ByName_163> },
	{ "damage",	ByName::kShip,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_164>,	&EntityByNameTyped<&ByName_164> },
	{ "dealEnergyDamageWithinDesiredRange",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_165>,	&EntityByNameTyped<&ByName_165> },
	{ "isHulk",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_166>,	&EntityByNameTyped<&ByName_166> },
	{ "releaseCargoPodsDebris",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_167>,	&EntityByNameTyped<&ByName_167> },
	{ "showDamage",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_168>,	&EntityByNameTyped<&ByName_168> },
	{ "becomeExplosion",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_169>,	&EntityByNameTyped<&ByName_169> },
	{ "becomeEnergyBlast",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_170>,	&EntityByNameTyped<&ByName_170> },
	{ "broadcastEnergyBlastImminent",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_171>,	&EntityByNameTyped<&ByName_171> },
	{ "weaponRecoveryTime",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_172>,	&EntityByNameTyped<&ByName_172> },
	{ "laserHeatLevel",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_173>,	&EntityByNameTyped<&ByName_173> },
	{ "laserHeatLevelAft",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_174>,	&EntityByNameTyped<&ByName_174> },
	{ "laserHeatLevelForward",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_175>,	&EntityByNameTyped<&ByName_175> },
	{ "laserHeatLevelPort",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_176>,	&EntityByNameTyped<&ByName_176> },
	{ "laserHeatLevelStarboard",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_177>,	&EntityByNameTyped<&ByName_177> },
	{ "hullHeatLevel",	ByName::kShip | ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_178>,	&EntityByNameTyped<&ByName_178> },
	{ "entityPersonality",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_179>,	&EntityByNameTyped<&ByName_179> },
	{ "entityPersonalityInt",	ByName::kShip,	oo::kValueType<GLint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_180>,	&EntityByNameTyped<&ByName_180> },
	{ "randomSeedForShaders",	ByName::kShip,	oo::kValueType<uint32_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_181>,	&EntityByNameTyped<&ByName_181> },
	{ "resetExhaustPlumes",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_182>,	&EntityByNameTyped<&ByName_182> },
	{ "checkScanner",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_183>,	&EntityByNameTyped<&ByName_183> },
	{ "checkScannerIgnoringUnpowered",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_184>,	&EntityByNameTyped<&ByName_184> },
	{ "numberOfScannedShips",	ByName::kShip,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_185>,	&EntityByNameTyped<&ByName_185> },
	{ "foundTarget",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_186>,	&EntityByNameTyped<&ByName_186> },
	{ "primaryAggressor",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_187>,	&EntityByNameTyped<&ByName_187> },
	{ "lastEscortTarget",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_188>,	&EntityByNameTyped<&ByName_188> },
	{ "thankedShip",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_189>,	&EntityByNameTyped<&ByName_189> },
	{ "rememberedShip",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_190>,	&EntityByNameTyped<&ByName_190> },
	{ "targetStation",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_191>,	&EntityByNameTyped<&ByName_191> },
	{ "canStillTrackPrimaryTarget",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_192>,	&EntityByNameTyped<&ByName_192> },
	{ "shipHitByLaser",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_193>,	&EntityByNameTyped<&ByName_193> },
	{ "noteLostTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_194>,	&EntityByNameTyped<&ByName_194> },
	{ "noteLostTargetAndGoIdle",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_195>,	&EntityByNameTyped<&ByName_195> },
	{ "behaviour",	ByName::kShip,	oo::kValueType<oo::Behaviour>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_196>,	&EntityByNameTyped<&ByName_196> },
	{ "destination",	ByName::kShip,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_197>,	&EntityByNameTyped<&ByName_197> },
	{ "coordinates",	ByName::kShip,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_198>,	&EntityByNameTyped<&ByName_198> },
	{ "rangeToDestination",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_199>,	&EntityByNameTyped<&ByName_199> },
	{ "defenseTargetCount",	ByName::kShip,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_200>,	&EntityByNameTyped<&ByName_200> },
	{ "validateDefenseTargets",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_201>,	&EntityByNameTyped<&ByName_201> },
	{ "removeAllDefenseTargets",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_202>,	&EntityByNameTyped<&ByName_202> },
	{ "rangeToPrimaryTarget",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_203>,	&EntityByNameTyped<&ByName_203> },
	{ "approachAspectToPrimaryTarget",	ByName::kShip,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_204>,	&EntityByNameTyped<&ByName_204> },
	{ "currentAimTolerance",	ByName::kShip,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_205>,	&EntityByNameTyped<&ByName_205> },
	{ "shotTime",	ByName::kShip,	oo::kValueType<OOTimeDelta>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_206>,	&EntityByNameTyped<&ByName_206> },
	{ "resetShotTime",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_207>,	&EntityByNameTyped<&ByName_207> },
	{ "fireDirectLaserDefensiveShot",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_208>,	&EntityByNameTyped<&ByName_208> },
	{ "missedShots",	ByName::kShip,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_209>,	&EntityByNameTyped<&ByName_209> },
	{ "missileLaunchPosition",	ByName::kShip,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_210>,	&EntityByNameTyped<&ByName_210> },
	{ "fireMissile",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_211>,	&EntityByNameTyped<&ByName_211> },
	{ "isMissileFlagSet",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_212>,	&EntityByNameTyped<&ByName_212> },
	{ "missileLoadTime",	ByName::kShip,	oo::kValueType<OOTimeDelta>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_213>,	&EntityByNameTyped<&ByName_213> },
	{ "noticeECM",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_214>,	&EntityByNameTyped<&ByName_214> },
	{ "fireECM",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_215>,	&EntityByNameTyped<&ByName_215> },
	{ "activateCloakingDevice",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_216>,	&EntityByNameTyped<&ByName_216> },
	{ "deactivateCloakingDevice",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_217>,	&EntityByNameTyped<&ByName_217> },
	{ "launchCascadeMine",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_218>,	&EntityByNameTyped<&ByName_218> },
	{ "launchEscapeCapsule",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_219>,	&EntityByNameTyped<&ByName_219> },
	{ "dumpCargo",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_220>,	&EntityByNameTyped<&ByName_220> },
	{ "manageCollisions",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_221>,	&EntityByNameTyped<&ByName_221> },
	{ "thrustVector",	ByName::kShip,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_222>,	&EntityByNameTyped<&ByName_222> },
	{ "suppressTargetLost",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_223>,	&EntityByNameTyped<&ByName_223> },
	{ "abandonShip",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_224>,	&EntityByNameTyped<&ByName_224> },
	{ "enterWitchspace",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_225>,	&EntityByNameTyped<&ByName_225> },
	{ "leaveWitchspace",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_226>,	&EntityByNameTyped<&ByName_226> },
	{ "witchspaceLeavingEffects",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_227>,	&EntityByNameTyped<&ByName_227> },
	{ "switchLightsOn",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_228>,	&EntityByNameTyped<&ByName_228> },
	{ "switchLightsOff",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_229>,	&EntityByNameTyped<&ByName_229> },
	{ "lightsActive",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_230>,	&EntityByNameTyped<&ByName_230> },
	{ "updateEscortFormation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_231>,	&EntityByNameTyped<&ByName_231> },
	{ "refreshEscortPositions",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_232>,	&EntityByNameTyped<&ByName_232> },
	{ "deployEscorts",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_233>,	&EntityByNameTyped<&ByName_233> },
	{ "dockEscorts",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_234>,	&EntityByNameTyped<&ByName_234> },
	{ "setTargetToNearestFriendlyStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_235>,	&EntityByNameTyped<&ByName_235> },
	{ "setTargetToNearestStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_236>,	&EntityByNameTyped<&ByName_236> },
	{ "setTargetToSystemStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_237>,	&EntityByNameTyped<&ByName_237> },
	{ "abortDocking",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_238>,	&EntityByNameTyped<&ByName_238> },
	{ "dockingInstructions",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_239>,	&EntityByNameTyped<&ByName_239> },
	{ "broadcastThargoidDestroyed",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_240>,	&EntityByNameTyped<&ByName_240> },
	{ "broadcastAIMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_241>,	nullptr },
	{ "setCommsMessageColor",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_242>,	&EntityByNameTyped<&ByName_242> },
	{ "markedForFines",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_243>,	&EntityByNameTyped<&ByName_243> },
	{ "markForFines",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_244>,	&EntityByNameTyped<&ByName_244> },
	{ "isMining",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_245>,	&EntityByNameTyped<&ByName_245> },
	{ "interpretAIMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_246>,	nullptr },
	{ "spawn:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_247>,	nullptr },
	{ "checkShipsInVicinityForWitchJumpExit",	ByName::kShip,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_248>,	&EntityByNameTyped<&ByName_248> },
	{ "trackCloseContacts",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_249>,	&EntityByNameTyped<&ByName_249> },
	{ "scriptInfo",	ByName::kShip | ByName::kEffect,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_253>,	&EntityByNameTyped<&ByName_253> },
	{ "overrideScriptInfo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_254>,	nullptr },
	{ "entityForShaderProperties",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_255>,	&EntityByNameTyped<&ByName_255> },
	{ "isDemoShip",	ByName::kShip,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_256>,	&EntityByNameTyped<&ByName_256> },
	{ "getDemoStartTime",	ByName::kShip,	oo::kValueType<OOTimeAbsolute>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_257>,	&EntityByNameTyped<&ByName_257> },
	{ "sendAIMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_258>,	nullptr },
	{ "alertCondition",	ByName::kShip,	oo::kValueType<AlertCondition>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_259>,	&EntityByNameTyped<&ByName_259> },
	{ "realAlertCondition",	ByName::kShip,	oo::kValueType<AlertCondition>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_260>,	&EntityByNameTyped<&ByName_260> },
	{ "doNothing",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_261>,	&EntityByNameTyped<&ByName_261> },
	{ "setAITo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_262>,	nullptr },
	{ "setAIScript:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_263>,	nullptr },
	{ "switchAITo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_264>,	nullptr },
	{ "scanForHostiles",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_265>,	&EntityByNameTyped<&ByName_265> },
	{ "groupAttackTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_266>,	&EntityByNameTyped<&ByName_266> },
	{ "performAttack",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_267>,	&EntityByNameTyped<&ByName_267> },
	{ "performCollect",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_268>,	&EntityByNameTyped<&ByName_268> },
	{ "performEscort",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_269>,	&EntityByNameTyped<&ByName_269> },
	{ "performFaceDestination",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_270>,	&EntityByNameTyped<&ByName_270> },
	{ "performFlee",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_271>,	&EntityByNameTyped<&ByName_271> },
	{ "performFlyToRangeFromDestination",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_272>,	&EntityByNameTyped<&ByName_272> },
	{ "performHold",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_273>,	&EntityByNameTyped<&ByName_273> },
	{ "performIdle",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_274>,	&EntityByNameTyped<&ByName_274> },
	{ "performIntercept",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_275>,	&EntityByNameTyped<&ByName_275> },
	{ "performLandOnPlanet",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_276>,	&EntityByNameTyped<&ByName_276> },
	{ "performMining",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_277>,	&EntityByNameTyped<&ByName_277> },
	{ "performScriptedAI",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_278>,	&EntityByNameTyped<&ByName_278> },
	{ "performScriptedAttackAI",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_279>,	&EntityByNameTyped<&ByName_279> },
	{ "performBuoyTumble",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_280>,	&EntityByNameTyped<&ByName_280> },
	{ "performStop",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_281>,	&EntityByNameTyped<&ByName_281> },
	{ "performTumble",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_282>,	&EntityByNameTyped<&ByName_282> },
	{ "requestDockingCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_283>,	&EntityByNameTyped<&ByName_283> },
	{ "recallDockingInstructions",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_284>,	&EntityByNameTyped<&ByName_284> },
	{ "scanForNearestIncomingMissile",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_285>,	&EntityByNameTyped<&ByName_285> },
	{ "enterPlayerWormhole",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_286>,	&EntityByNameTyped<&ByName_286> },
	{ "enterTargetWormhole",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_287>,	&EntityByNameTyped<&ByName_287> },
	{ "wormholeEscorts",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_288>,	&EntityByNameTyped<&ByName_288> },
	{ "wormholeEntireGroup",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_289>,	&EntityByNameTyped<&ByName_289> },
	{ "broadcastDistressMessage",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_290>,	&EntityByNameTyped<&ByName_290> },
	{ "checkFoundTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_291>,	&EntityByNameTyped<&ByName_291> },
	{ "increaseAlertLevel",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_292>,	&EntityByNameTyped<&ByName_292> },
	{ "decreaseAlertLevel",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_293>,	&EntityByNameTyped<&ByName_293> },
	{ "launchPolice",	ByName::kShip,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_294>,	&EntityByNameTyped<&ByName_294> },
	{ "launchDefenseShip",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_295>,	&EntityByNameTyped<&ByName_295> },
	{ "launchScavenger",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_296>,	&EntityByNameTyped<&ByName_296> },
	{ "launchMiner",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_297>,	&EntityByNameTyped<&ByName_297> },
	{ "launchPirateShip",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_298>,	&EntityByNameTyped<&ByName_298> },
	{ "launchShuttle",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_299>,	&EntityByNameTyped<&ByName_299> },
	{ "launchTrader",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_300>,	&EntityByNameTyped<&ByName_300> },
	{ "launchEscort",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_301>,	&EntityByNameTyped<&ByName_301> },
	{ "launchPatrol",	ByName::kShip,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_302>,	&EntityByNameTyped<&ByName_302> },
	{ "launchShipWithRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_303>,	nullptr },
	{ "abortAllDockings",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_304>,	&EntityByNameTyped<&ByName_304> },
	{ "setStateTo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_305>,	nullptr },
	{ "pauseAI:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_306>,	nullptr },
	{ "randomPauseAI:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_307>,	nullptr },
	{ "dropMessages:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_308>,	nullptr },
	{ "debugDumpPendingMessages",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_309>,	&EntityByNameTyped<&ByName_309> },
	{ "setDestinationToCurrentLocation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_310>,	&EntityByNameTyped<&ByName_310> },
	{ "setDestinationToJinkPosition",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_311>,	&EntityByNameTyped<&ByName_311> },
	{ "setDesiredRangeTo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_312>,	nullptr },
	{ "setDesiredRangeForWaypoint",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_313>,	&EntityByNameTyped<&ByName_313> },
	{ "setSpeedTo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_314>,	nullptr },
	{ "setSpeedFactorTo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_315>,	nullptr },
	{ "setSpeedToCruiseSpeed",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_316>,	&EntityByNameTyped<&ByName_316> },
	{ "setThrustFactorTo:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_317>,	nullptr },
	{ "setTargetToPrimaryAggressor",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_318>,	&EntityByNameTyped<&ByName_318> },
	{ "addPrimaryAggressorAsDefenseTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_319>,	&EntityByNameTyped<&ByName_319> },
	{ "scanForNearestMerchantman",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_320>,	&EntityByNameTyped<&ByName_320> },
	{ "scanForRandomMerchantman",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_321>,	&EntityByNameTyped<&ByName_321> },
	{ "scanForLoot",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_322>,	&EntityByNameTyped<&ByName_322> },
	{ "scanForRandomLoot",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_323>,	&EntityByNameTyped<&ByName_323> },
	{ "setTargetToFoundTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_324>,	&EntityByNameTyped<&ByName_324> },
	{ "addFoundTargetAsDefenseTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_325>,	&EntityByNameTyped<&ByName_325> },
	{ "checkForFullHold",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_326>,	&EntityByNameTyped<&ByName_326> },
	{ "getWitchspaceEntryCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_327>,	&EntityByNameTyped<&ByName_327> },
	{ "setDestinationFromCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_328>,	&EntityByNameTyped<&ByName_328> },
	{ "setCoordinatesFromPosition",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_329>,	&EntityByNameTyped<&ByName_329> },
	{ "fightOrFleeMissile",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_330>,	&EntityByNameTyped<&ByName_330> },
	{ "setCourseToPlanet",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_331>,	&EntityByNameTyped<&ByName_331> },
	{ "setTakeOffFromPlanet",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_332>,	&EntityByNameTyped<&ByName_332> },
	{ "landOnPlanet",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_333>,	&EntityByNameTyped<&ByName_333> },
	{ "checkTargetLegalStatus",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_334>,	&EntityByNameTyped<&ByName_334> },
	{ "checkOwnLegalStatus",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_335>,	&EntityByNameTyped<&ByName_335> },
	{ "exitAIWithMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_336>,	nullptr },
	{ "setDestinationToTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_337>,	&EntityByNameTyped<&ByName_337> },
	{ "setDestinationWithinTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_338>,	&EntityByNameTyped<&ByName_338> },
	{ "checkCourseToDestination",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_339>,	&EntityByNameTyped<&ByName_339> },
	{ "checkAegis",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_340>,	&EntityByNameTyped<&ByName_340> },
	{ "checkEnergy",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_341>,	&EntityByNameTyped<&ByName_341> },
	{ "checkHeatInsulation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_342>,	&EntityByNameTyped<&ByName_342> },
	{ "findNewDefenseTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_343>,	&EntityByNameTyped<&ByName_343> },
	{ "scanForOffenders",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_344>,	&EntityByNameTyped<&ByName_344> },
	{ "setCourseToWitchpoint",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_345>,	&EntityByNameTyped<&ByName_345> },
	{ "setDestinationToWitchpoint",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_346>,	&EntityByNameTyped<&ByName_346> },
	{ "setDestinationToStationBeacon",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_347>,	&EntityByNameTyped<&ByName_347> },
	{ "performHyperSpaceExit",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_348>,	&EntityByNameTyped<&ByName_348> },
	{ "performHyperSpaceExitWithoutReplacing",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_349>,	&EntityByNameTyped<&ByName_349> },
	{ "disengageAutopilot",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_350>,	&EntityByNameTyped<&ByName_350> },
	{ "wormholeGroup",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_351>,	&EntityByNameTyped<&ByName_351> },
	{ "commsMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_352>,	nullptr },
	{ "commsMessageByUnpiloted:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_353>,	nullptr },
	{ "ejectCargo",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_354>,	&EntityByNameTyped<&ByName_354> },
	{ "scanForThargoid",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_355>,	&EntityByNameTyped<&ByName_355> },
	{ "scanForNonThargoid",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_356>,	&EntityByNameTyped<&ByName_356> },
	{ "thargonCheckMother",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_357>,	&EntityByNameTyped<&ByName_357> },
	{ "becomeUncontrolledThargon",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_358>,	&EntityByNameTyped<&ByName_358> },
	{ "checkDistanceTravelled",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_359>,	&EntityByNameTyped<&ByName_359> },
	{ "fightOrFleeHostiles",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_360>,	&EntityByNameTyped<&ByName_360> },
	{ "suggestEscort",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_361>,	&EntityByNameTyped<&ByName_361> },
	{ "escortCheckMother",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_362>,	&EntityByNameTyped<&ByName_362> },
	{ "checkGroupOddsVersusTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_363>,	&EntityByNameTyped<&ByName_363> },
	{ "scanForFormationLeader",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_364>,	&EntityByNameTyped<&ByName_364> },
	{ "messageMother:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_365>,	nullptr },
	{ "messageSelf:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_366>,	nullptr },
	{ "setPlanetPatrolCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_367>,	&EntityByNameTyped<&ByName_367> },
	{ "setSunSkimStartCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_368>,	&EntityByNameTyped<&ByName_368> },
	{ "setSunSkimEndCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_369>,	&EntityByNameTyped<&ByName_369> },
	{ "setSunSkimExitCoordinates",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_370>,	&EntityByNameTyped<&ByName_370> },
	{ "patrolReportIn",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_371>,	&EntityByNameTyped<&ByName_371> },
	{ "checkForMotherStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_372>,	&EntityByNameTyped<&ByName_372> },
	{ "sendTargetCommsMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_373>,	nullptr },
	{ "markTargetForFines",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_374>,	&EntityByNameTyped<&ByName_374> },
	{ "markTargetForOffence:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_375>,	nullptr },
	{ "storeTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_376>,	&EntityByNameTyped<&ByName_376> },
	{ "recallStoredTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_377>,	&EntityByNameTyped<&ByName_377> },
	{ "scanForRocks",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_378>,	&EntityByNameTyped<&ByName_378> },
	{ "setDestinationToDockingAbort",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_379>,	&EntityByNameTyped<&ByName_379> },
	{ "requestNewTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_380>,	&EntityByNameTyped<&ByName_380> },
	{ "rollD:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_381>,	nullptr },
	{ "scanForNearestShipWithPrimaryRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_382>,	nullptr },
	{ "scanForNearestShipHavingRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_383>,	nullptr },
	{ "scanForNearestShipWithAnyPrimaryRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_384>,	nullptr },
	{ "scanForNearestShipHavingAnyRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_385>,	nullptr },
	{ "scanForNearestShipWithScanClass:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_386>,	nullptr },
	{ "scanForNearestShipWithoutPrimaryRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_387>,	nullptr },
	{ "scanForNearestShipNotHavingRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_388>,	nullptr },
	{ "scanForNearestShipWithoutAnyPrimaryRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_389>,	nullptr },
	{ "scanForNearestShipNotHavingAnyRole:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_390>,	nullptr },
	{ "scanForNearestShipWithoutScanClass:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_391>,	nullptr },
	{ "scanForNearestShipMatchingPredicate:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_392>,	nullptr },
	{ "setCoordinates:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_393>,	nullptr },
	{ "checkForNormalSpace",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_394>,	&EntityByNameTyped<&ByName_394> },
	{ "setTargetToRandomStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_395>,	&EntityByNameTyped<&ByName_395> },
	{ "setTargetToLastStation",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_396>,	&EntityByNameTyped<&ByName_396> },
	{ "addFuel:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_397>,	nullptr },
	{ "scriptActionOnTarget:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_398>,	nullptr },
	{ "safeScriptActionOnTarget:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_399>,	nullptr },
	{ "sendScriptMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_400>,	nullptr },
	{ "ai_throwSparks",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_401>,	&EntityByNameTyped<&ByName_401> },
	{ "explodeSelf",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_402>,	&EntityByNameTyped<&ByName_402> },
	{ "ai_debugMessage:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_403>,	nullptr },
	{ "targetFirstBeaconWithCode:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_404>,	nullptr },
	{ "targetNextBeaconWithCode:",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_405>,	nullptr },
	{ "setRacepointsFromTarget",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_406>,	&EntityByNameTyped<&ByName_406> },
	{ "performFlyRacepoints",	ByName::kShip,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_407>,	&EntityByNameTyped<&ByName_407> },
	{ "baseMass",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_408>,	&EntityByNameTyped<&ByName_408> },
	{ "unloadCargoPods",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_409>,	&EntityByNameTyped<&ByName_409> },
	{ "loadCargoPods",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_410>,	&EntityByNameTyped<&ByName_410> },
	{ "deciCredits",	ByName::kPlayer,	oo::kValueType<OOCreditsQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_411>,	&EntityByNameTyped<&ByName_411> },
	{ "random_factor",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_412>,	&EntityByNameTyped<&ByName_412> },
	{ "galaxyNumber",	ByName::kPlayer,	oo::kValueType<OOGalaxyID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_413>,	&EntityByNameTyped<&ByName_413> },
	{ "galaxy_coordinates",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_414>,	&EntityByNameTyped<&ByName_414> },
	{ "cursor_coordinates",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_415>,	&EntityByNameTyped<&ByName_415> },
	{ "chart_centre_coordinates",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_416>,	&EntityByNameTyped<&ByName_416> },
	{ "chart_zoom",	ByName::kPlayer,	oo::kValueType<OOScalar>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_417>,	&EntityByNameTyped<&ByName_417> },
	{ "custom_chart_zoom",	ByName::kPlayer,	oo::kValueType<OOScalar>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_418>,	&EntityByNameTyped<&ByName_418> },
	{ "custom_chart_centre_coordinates",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_419>,	&EntityByNameTyped<&ByName_419> },
	{ "adjusted_chart_centre",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_420>,	&EntityByNameTyped<&ByName_420> },
	{ "ANAMode",	ByName::kPlayer,	oo::kValueType<OORouteType>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_421>,	&EntityByNameTyped<&ByName_421> },
	{ "systemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_422>,	&EntityByNameTyped<&ByName_422> },
	{ "previousSystemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_423>,	&EntityByNameTyped<&ByName_423> },
	{ "targetSystemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_424>,	&EntityByNameTyped<&ByName_424> },
	{ "nextHopTargetSystemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_425>,	&EntityByNameTyped<&ByName_425> },
	{ "infoSystemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_426>,	&EntityByNameTyped<&ByName_426> },
	{ "nextInfoSystem",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_427>,	&EntityByNameTyped<&ByName_427> },
	{ "previousInfoSystem",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_428>,	&EntityByNameTyped<&ByName_428> },
	{ "homeInfoSystem",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_429>,	&EntityByNameTyped<&ByName_429> },
	{ "targetInfoSystem",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_430>,	&EntityByNameTyped<&ByName_430> },
	{ "infoSystemOnRoute",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_431>,	&EntityByNameTyped<&ByName_431> },
	{ "commanderDataDictionary",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_432>,	&EntityByNameTyped<&ByName_432> },
	{ "setCommanderDataFromDictionary:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_433>,	nullptr },
	{ "completeSetUp",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_434>,	&EntityByNameTyped<&ByName_434> },
	{ "startUpComplete",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_435>,	&EntityByNameTyped<&ByName_435> },
	{ "insideAtmosphereFraction",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_436>,	&EntityByNameTyped<&ByName_436> },
	{ "updateMovementFlags",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_437>,	&EntityByNameTyped<&ByName_437> },
	{ "updateAlertConditionForNearbyEntities",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_438>,	&EntityByNameTyped<&ByName_438> },
	{ "updateAlertCondition",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_439>,	&EntityByNameTyped<&ByName_439> },
	{ "checkScriptsIfAppropriate",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_440>,	&EntityByNameTyped<&ByName_440> },
	{ "resetAutopilotAI",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_441>,	&EntityByNameTyped<&ByName_441> },
#if OO_VARIABLE_TORUS_SPEED
	{ "hyperspeedFactor",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_442>,	&EntityByNameTyped<&ByName_442> },
#endif
	{ "injectorsEngaged",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_443>,	&EntityByNameTyped<&ByName_443> },
	{ "hyperspeedEngaged",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_444>,	&EntityByNameTyped<&ByName_444> },
	{ "gameOverFadeToBW",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_445>,	&EntityByNameTyped<&ByName_445> },
	{ "showGameOver",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_446>,	&EntityByNameTyped<&ByName_446> },
	{ "updateTargeting",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_447>,	&EntityByNameTyped<&ByName_447> },
	{ "breakPatternPosition",	ByName::kPlayer,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_448>,	&EntityByNameTyped<&ByName_448> },
	{ "viewpointOffset",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_449>,	&EntityByNameTyped<&ByName_449> },
	{ "viewpointOffsetAft",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_450>,	&EntityByNameTyped<&ByName_450> },
	{ "viewpointOffsetForward",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_451>,	&EntityByNameTyped<&ByName_451> },
	{ "viewpointOffsetPort",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_452>,	&EntityByNameTyped<&ByName_452> },
	{ "viewpointOffsetStarboard",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_453>,	&EntityByNameTyped<&ByName_453> },
	{ "viewpointPosition",	ByName::kPlayer,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_454>,	&EntityByNameTyped<&ByName_454> },
	{ "massLockable",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_455>,	&EntityByNameTyped<&ByName_455> },
	{ "massLocked",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_456>,	&EntityByNameTyped<&ByName_456> },
	{ "atHyperspeed",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_457>,	&EntityByNameTyped<&ByName_457> },
	{ "occlusionLevel",	ByName::kPlayer,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_458>,	&EntityByNameTyped<&ByName_458> },
	{ "setDockedAtMainStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_459>,	&EntityByNameTyped<&ByName_459> },
	{ "dockedStation",	ByName::kPlayer,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_460>,	&EntityByNameTyped<&ByName_460> },
	{ "getTargetDockStation",	ByName::kPlayer,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_461>,	&EntityByNameTyped<&ByName_461> },
	{ "resetHud",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_462>,	&EntityByNameTyped<&ByName_462> },
	{ "switchHudTo:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_463>,	nullptr },
	{ "dialCustomFloat:",	ByName::kPlayer,	oo::kValueType<float>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_464>,	nullptr },
	{ "showDemoShips",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_465>,	&EntityByNameTyped<&ByName_465> },
	{ "forwardShieldRechargeRate",	ByName::kPlayer,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_466>,	&EntityByNameTyped<&ByName_466> },
	{ "aftShieldRechargeRate",	ByName::kPlayer,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_467>,	&EntityByNameTyped<&ByName_467> },
	{ "forwardShieldLevel",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_468>,	&EntityByNameTyped<&ByName_468> },
	{ "aftShieldLevel",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_469>,	&EntityByNameTyped<&ByName_469> },
	{ "keyConfig",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_470>,	&EntityByNameTyped<&ByName_470> },
	{ "isMouseControlOn",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_471>,	&EntityByNameTyped<&ByName_471> },
	{ "dialRoll",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_472>,	&EntityByNameTyped<&ByName_472> },
	{ "dialPitch",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_473>,	&EntityByNameTyped<&ByName_473> },
	{ "dialYaw",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_474>,	&EntityByNameTyped<&ByName_474> },
	{ "dialSpeed",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_475>,	&EntityByNameTyped<&ByName_475> },
	{ "dialHyperSpeed",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_476>,	&EntityByNameTyped<&ByName_476> },
	{ "dialForwardShield",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_477>,	&EntityByNameTyped<&ByName_477> },
	{ "dialAftShield",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_478>,	&EntityByNameTyped<&ByName_478> },
	{ "dialEnergy",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_479>,	&EntityByNameTyped<&ByName_479> },
	{ "dialMaxEnergy",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_480>,	&EntityByNameTyped<&ByName_480> },
	{ "dialFuel",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_481>,	&EntityByNameTyped<&ByName_481> },
	{ "dialHyperRange",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_482>,	&EntityByNameTyped<&ByName_482> },
	{ "dialAltitude",	ByName::kPlayer,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_483>,	&EntityByNameTyped<&ByName_483> },
	{ "clockTime",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_484>,	&EntityByNameTyped<&ByName_484> },
	{ "clockTimeAdjusted",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_485>,	&EntityByNameTyped<&ByName_485> },
	{ "clockAdjusting",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_486>,	&EntityByNameTyped<&ByName_486> },
	{ "escapePodRescueTime",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_487>,	&EntityByNameTyped<&ByName_487> },
	{ "countMissiles",	ByName::kPlayer,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_488>,	&EntityByNameTyped<&ByName_488> },
	{ "dialMissileStatus",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<MissileStatus>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_489>,	&EntityByNameTyped<&ByName_489> },
	{ "dialFuelScoopStatus",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<FuelScoopStatus>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_490>,	&EntityByNameTyped<&ByName_490> },
	{ "fuelLeakRate",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_491>,	&EntityByNameTyped<&ByName_491> },
	{ "addRoleForMining",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_492>,	&EntityByNameTyped<&ByName_492> },
	{ "addRoleToPlayer:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_493>,	nullptr },
	{ "maxPlayerRoles",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_494>,	&EntityByNameTyped<&ByName_494> },
	{ "updateSystemMemory",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_495>,	&EntityByNameTyped<&ByName_495> },
	{ "compassTarget",	ByName::kPlayer,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_496>,	&EntityByNameTyped<&ByName_496> },
	{ "validateCompassTarget",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_497>,	&EntityByNameTyped<&ByName_497> },
	{ "compassMode",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<oo::CompassMode>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_498>,	&EntityByNameTyped<&ByName_498> },
	{ "setPrevCompassMode",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_499>,	&EntityByNameTyped<&ByName_499> },
	{ "setNextCompassMode",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_500>,	&EntityByNameTyped<&ByName_500> },
	{ "activeMissile",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_501>,	&EntityByNameTyped<&ByName_501> },
	{ "dialMaxMissiles",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_502>,	&EntityByNameTyped<&ByName_502> },
	{ "dialIdentEngaged",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_503>,	&EntityByNameTyped<&ByName_503> },
	{ "selectNextMultiFunctionDisplay",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_504>,	&EntityByNameTyped<&ByName_504> },
	{ "selectPreviousMultiFunctionDisplay",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_505>,	&EntityByNameTyped<&ByName_505> },
	{ "activeMFD",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_506>,	&EntityByNameTyped<&ByName_506> },
	{ "safeAllMissiles",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_507>,	&EntityByNameTyped<&ByName_507> },
	{ "tidyMissilePylons",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_508>,	&EntityByNameTyped<&ByName_508> },
	{ "selectNextMissile",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_509>,	&EntityByNameTyped<&ByName_509> },
	{ "clearAlertFlags",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_510>,	&EntityByNameTyped<&ByName_510> },
	{ "alertFlags",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_511>,	&EntityByNameTyped<&ByName_511> },
	{ "fleeingStatus",	ByName::kPlayer,	oo::kValueType<PlayerFleeingStatus>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_512>,	&EntityByNameTyped<&ByName_512> },
	{ "mountMissileWithRole:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_513>,	nullptr },
	{ "assignToActivePylon:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_514>,	nullptr },
	{ "scannerFuzziness",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_515>,	&EntityByNameTyped<&ByName_515> },
	{ "installedEnergyUnitType",	ByName::kPlayer,	oo::kValueType<OOEnergyUnitType>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_516>,	&EntityByNameTyped<&ByName_516> },
	{ "energyUnitType",	ByName::kPlayer,	oo::kValueType<OOEnergyUnitType>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_517>,	&EntityByNameTyped<&ByName_517> },
	{ "currentWeaponStats",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_518>,	&EntityByNameTyped<&ByName_518> },
	{ "weaponsOnline",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_519>,	&EntityByNameTyped<&ByName_519> },
	{ "fireMainWeapon",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_520>,	&EntityByNameTyped<&ByName_520> },
	{ "createDoppelganger",	ByName::kPlayer,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_521>,	&EntityByNameTyped<&ByName_521> },
	{ "rotateCargo",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_522>,	&EntityByNameTyped<&ByName_522> },
	{ "takeInternalDamage",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_523>,	&EntityByNameTyped<&ByName_523> },
	{ "loseTargetStatus",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_524>,	&EntityByNameTyped<&ByName_524> },
	{ "endScenario:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_525>,	nullptr },
	{ "docked",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_526>,	&EntityByNameTyped<&ByName_526> },
	{ "witchStart",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_527>,	&EntityByNameTyped<&ByName_527> },
	{ "witchEnd",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_528>,	&EntityByNameTyped<&ByName_528> },
	{ "hyperspaceJumpDistance",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_529>,	&EntityByNameTyped<&ByName_529> },
	{ "fuelRequiredForJump",	ByName::kPlayer,	oo::kValueType<OOFuelQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_530>,	&EntityByNameTyped<&ByName_530> },
	{ "hasSufficientFuelForJump",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_531>,	&EntityByNameTyped<&ByName_531> },
	{ "noteCompassLostTarget",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_532>,	&EntityByNameTyped<&ByName_532> },
	{ "enterGalacticWitchspace",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_533>,	&EntityByNameTyped<&ByName_533> },
	{ "setGuiToStatusScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_534>,	&EntityByNameTyped<&ByName_534> },
	{ "primedEquipmentCount",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_535>,	&EntityByNameTyped<&ByName_535> },
	{ "legalStatusOfCargoList",	ByName::kPlayer,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_536>,	&EntityByNameTyped<&ByName_536> },
	{ "setGuiToSystemDataScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_537>,	&EntityByNameTyped<&ByName_537> },
	{ "setGuiToLongRangeChartScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_538>,	&EntityByNameTyped<&ByName_538> },
	{ "setGuiToShortRangeChartScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_539>,	&EntityByNameTyped<&ByName_539> },
	{ "setGuiToGameOptionsScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_540>,	&EntityByNameTyped<&ByName_540> },
	{ "setGuiToLoadSaveScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_541>,	&EntityByNameTyped<&ByName_541> },
	{ "highlightEquipShipScreenKey:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_542>,	nullptr },
	{ "availableFacings",	ByName::kPlayer,	oo::kValueType<OOWeaponFacingSet>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_543>,	&EntityByNameTyped<&ByName_543> },
	{ "showInformationForSelectedUpgrade",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_544>,	&EntityByNameTyped<&ByName_544> },
	{ "showInformationForSelectedInterface",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_545>,	&EntityByNameTyped<&ByName_545> },
	{ "activateSelectedInterface",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_546>,	&EntityByNameTyped<&ByName_546> },
	{ "setupStartScreenGui",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_547>,	&EntityByNameTyped<&ByName_547> },
	{ "setGuiToOXZManager",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_548>,	&EntityByNameTyped<&ByName_548> },
	{ "buySelectedItem",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_549>,	&EntityByNameTyped<&ByName_549> },
	{ "tryBuyingItem:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_550>,	nullptr },
	{ "cargoQuantityForType:",	ByName::kPlayer,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_551>,	nullptr },
	{ "calculateCurrentCargo",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_552>,	&EntityByNameTyped<&ByName_552> },
	{ "showMarketScreenHeaders",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_553>,	&EntityByNameTyped<&ByName_553> },
	{ "setGuiToMarketScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_554>,	&EntityByNameTyped<&ByName_554> },
	{ "setGuiToMarketInfoScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_555>,	&EntityByNameTyped<&ByName_555> },
	{ "showMarketCashAndLoadLine",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_556>,	&EntityByNameTyped<&ByName_556> },
	{ "guiScreen",	ByName::kPlayer,	oo::kValueType<oo::GUIScreenID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_557>,	&EntityByNameTyped<&ByName_557> },
	{ "isSpeechOn",	ByName::kPlayer,	oo::kValueType<SpeechSettings>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_558>,	&EntityByNameTyped<&ByName_558> },
	{ "addEquipmentWithScriptToCustomKeyArray:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_559>,	nullptr },
	{ "validateCustomEquipActivationArray",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_560>,	&EntityByNameTyped<&ByName_560> },
	{ "addEquipmentFromCollection:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_561>,	nullptr },
	{ "getFined",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_562>,	&EntityByNameTyped<&ByName_562> },
	{ "tradeInFactor",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_563>,	&EntityByNameTyped<&ByName_563> },
	{ "renovationCosts",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_564>,	&EntityByNameTyped<&ByName_564> },
	{ "renovationFactor",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_565>,	&EntityByNameTyped<&ByName_565> },
	{ "setDefaultViewOffsets",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_566>,	&EntityByNameTyped<&ByName_566> },
	{ "setDefaultCustomViews",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_567>,	&EntityByNameTyped<&ByName_567> },
	{ "weaponViewOffset",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_568>,	&EntityByNameTyped<&ByName_568> },
	{ "setUpTrumbles",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_569>,	&EntityByNameTyped<&ByName_569> },
	{ "trumbleCount",	ByName::kPlayer | ByName::kProxy,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_570>,	&EntityByNameTyped<&ByName_570> },
	{ "trumbleValue",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_571>,	&EntityByNameTyped<&ByName_571> },
	{ "setTrumbleValueFrom:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_572>,	nullptr },
	{ "trumbleAppetiteAccumulator",	ByName::kPlayer,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_573>,	&EntityByNameTyped<&ByName_573> },
	{ "setScoopsActive",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_574>,	&EntityByNameTyped<&ByName_574> },
	{ "clearTargetMemory",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_575>,	&EntityByNameTyped<&ByName_575> },
	{ "customViewQuaternion",	ByName::kPlayer,	oo::kValueType<Quaternion>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_576>,	&EntityByNameTyped<&ByName_576> },
	{ "customViewMatrix",	ByName::kPlayer,	oo::kValueType<OOMatrix>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_577>,	&EntityByNameTyped<&ByName_577> },
	{ "customViewOffset",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_578>,	&EntityByNameTyped<&ByName_578> },
	{ "customViewRotationCenter",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_579>,	&EntityByNameTyped<&ByName_579> },
	{ "customViewForwardVector",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_580>,	&EntityByNameTyped<&ByName_580> },
	{ "customViewUpVector",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_581>,	&EntityByNameTyped<&ByName_581> },
	{ "customViewRightVector",	ByName::kPlayer,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_582>,	&EntityByNameTyped<&ByName_582> },
	{ "resetCustomView",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_583>,	&EntityByNameTyped<&ByName_583> },
	{ "setCustomViewData",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_584>,	&EntityByNameTyped<&ByName_584> },
	{ "showInfoFlag",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_585>,	&EntityByNameTyped<&ByName_585> },
	{ "missionOverlayDescriptor",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_586>,	&EntityByNameTyped<&ByName_586> },
	{ "missionOverlayDescriptorOrDefault",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_587>,	&EntityByNameTyped<&ByName_587> },
	{ "setMissionOverlayDescriptor:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_588>,	nullptr },
	{ "missionBackgroundDescriptor",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_589>,	&EntityByNameTyped<&ByName_589> },
	{ "missionBackgroundDescriptorOrDefault",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_590>,	&EntityByNameTyped<&ByName_590> },
	{ "setMissionBackgroundDescriptor:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_591>,	nullptr },
	{ "missionBackgroundSpecial",	ByName::kPlayer,	oo::kValueType<OOGUIBackgroundSpecial>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_592>,	&EntityByNameTyped<&ByName_592> },
	{ "setMissionBackgroundSpecial:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_593>,	nullptr },
	{ "missionExitScreen",	ByName::kPlayer,	oo::kValueType<oo::GUIScreenID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_594>,	&EntityByNameTyped<&ByName_594> },
	{ "equipScreenBackgroundDescriptor",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_595>,	&EntityByNameTyped<&ByName_595> },
	{ "setEquipScreenBackgroundDescriptor:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_596>,	nullptr },
	{ "scriptsLoaded",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_597>,	&EntityByNameTyped<&ByName_597> },
	{ "galacticHyperspaceBehaviour",	ByName::kPlayer,	oo::kValueType<oo::GalacticHyperspaceBehaviour>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_598>,	&EntityByNameTyped<&ByName_598> },
	{ "galacticHyperspaceFixedCoords",	ByName::kPlayer,	oo::kValueType<NSPoint>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_599>,	&EntityByNameTyped<&ByName_599> },
	{ "longRangeChartMode",	ByName::kPlayer,	oo::kValueType<oo::LongRangeChartMode>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_600>,	&EntityByNameTyped<&ByName_600> },
	{ "scoopOverride",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_601>,	&EntityByNameTyped<&ByName_601> },
	{ "isDocked",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_602>,	&EntityByNameTyped<&ByName_602> },
	{ "clearedToDock",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_603>,	&EntityByNameTyped<&ByName_603> },
	{ "getDockingClearanceStatus",	ByName::kPlayer,	oo::kValueType<OODockingClearanceStatus>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_604>,	&EntityByNameTyped<&ByName_604> },
	{ "penaltyForUnauthorizedDocking",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_605>,	&EntityByNameTyped<&ByName_605> },
	{ "updateWormholes",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_606>,	&EntityByNameTyped<&ByName_606> },
	{ "addMissionDestinationMarker:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_607>,	nullptr },
	{ "removeMissionDestinationMarker:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_608>,	nullptr },
	{ "getMissionDestinations",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_609>,	&EntityByNameTyped<&ByName_609> },
	{ "clearExtraMissionKeys",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_610>,	&EntityByNameTyped<&ByName_610> },
	{ "setExtraMissionKeys:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_611>,	nullptr },
	{ "score",	ByName::kPlayer,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_612>,	&EntityByNameTyped<&ByName_612> },
	{ "creditBalance",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_613>,	&EntityByNameTyped<&ByName_613> },
	{ "dockedAtMainStation",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_614>,	&EntityByNameTyped<&ByName_614> },
	{ "resetScannerZoom",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_615>,	&EntityByNameTyped<&ByName_615> },
	{ "currentGalaxyID",	ByName::kPlayer,	oo::kValueType<OOGalaxyID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_616>,	&EntityByNameTyped<&ByName_616> },
	{ "currentSystemID",	ByName::kPlayer,	oo::kValueType<OOSystemID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_617>,	&EntityByNameTyped<&ByName_617> },
	{ "allowMissionInterrupt",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_618>,	&EntityByNameTyped<&ByName_618> },
	{ "scriptTimer",	ByName::kPlayer,	oo::kValueType<OOTimeDelta>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_619>,	&EntityByNameTyped<&ByName_619> },
	{ "systemPseudoRandom100",	ByName::kPlayer,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_620>,	&EntityByNameTyped<&ByName_620> },
	{ "systemPseudoRandom256",	ByName::kPlayer,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_621>,	&EntityByNameTyped<&ByName_621> },
	{ "systemPseudoRandomFloat",	ByName::kPlayer,	oo::kValueType<double>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_622>,	&EntityByNameTyped<&ByName_622> },
	{ "validatedMarker:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_623>,	nullptr },
	{ "commanderKillsAsString",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_624>,	&EntityByNameTyped<&ByName_624> },
	{ "commanderBountyAsString",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_625>,	&EntityByNameTyped<&ByName_625> },
	{ "creditsFormattedForSubstitution",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_626>,	&EntityByNameTyped<&ByName_626> },
	{ "creditsFormattedForLegacySubstitution",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_627>,	&EntityByNameTyped<&ByName_627> },
	{ "setUpSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_628>,	&EntityByNameTyped<&ByName_628> },
	{ "setUpWeaponSounds",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_629>,	&EntityByNameTyped<&ByName_629> },
	{ "destroySound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_630>,	&EntityByNameTyped<&ByName_630> },
	{ "isBeeping",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_631>,	&EntityByNameTyped<&ByName_631> },
	{ "boop",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_632>,	&EntityByNameTyped<&ByName_632> },
	{ "playIdentOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_633>,	&EntityByNameTyped<&ByName_633> },
	{ "playIdentOff",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_634>,	&EntityByNameTyped<&ByName_634> },
	{ "playIdentLockedOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_635>,	&EntityByNameTyped<&ByName_635> },
	{ "playMissileArmed",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_636>,	&EntityByNameTyped<&ByName_636> },
	{ "playMineArmed",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_637>,	&EntityByNameTyped<&ByName_637> },
	{ "playMissileSafe",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_638>,	&EntityByNameTyped<&ByName_638> },
	{ "playMissileLockedOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_639>,	&EntityByNameTyped<&ByName_639> },
	{ "playNextEquipmentSelected",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_640>,	&EntityByNameTyped<&ByName_640> },
	{ "playNextMissileSelected",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_641>,	&EntityByNameTyped<&ByName_641> },
	{ "playWeaponsOnline",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_642>,	&EntityByNameTyped<&ByName_642> },
	{ "playWeaponsOffline",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_643>,	&EntityByNameTyped<&ByName_643> },
	{ "playCargoJettisioned",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_644>,	&EntityByNameTyped<&ByName_644> },
	{ "playAutopilotOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_645>,	&EntityByNameTyped<&ByName_645> },
	{ "playAutopilotOff",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_646>,	&EntityByNameTyped<&ByName_646> },
	{ "playAutopilotOutOfRange",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_647>,	&EntityByNameTyped<&ByName_647> },
	{ "playAutopilotCannotDockWithTarget",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_648>,	&EntityByNameTyped<&ByName_648> },
	{ "playSaveOverwriteYes",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_649>,	&EntityByNameTyped<&ByName_649> },
	{ "playSaveOverwriteNo",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_650>,	&EntityByNameTyped<&ByName_650> },
	{ "playHoldFull",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_651>,	&EntityByNameTyped<&ByName_651> },
	{ "playJumpMassLocked",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_652>,	&EntityByNameTyped<&ByName_652> },
	{ "playTargetLost",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_653>,	&EntityByNameTyped<&ByName_653> },
	{ "playNoTargetInMemory",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_654>,	&EntityByNameTyped<&ByName_654> },
	{ "playTargetSwitched",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_655>,	&EntityByNameTyped<&ByName_655> },
	{ "playHyperspaceNoTarget",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_656>,	&EntityByNameTyped<&ByName_656> },
	{ "playHyperspaceNoFuel",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_657>,	&EntityByNameTyped<&ByName_657> },
	{ "playHyperspaceBlocked",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_658>,	&EntityByNameTyped<&ByName_658> },
	{ "playHyperspaceDistanceTooGreat",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_659>,	&EntityByNameTyped<&ByName_659> },
	{ "playCloakingDeviceOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_660>,	&EntityByNameTyped<&ByName_660> },
	{ "playCloakingDeviceOff",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_661>,	&EntityByNameTyped<&ByName_661> },
	{ "playMenuNavigationUp",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_662>,	&EntityByNameTyped<&ByName_662> },
	{ "playMenuNavigationDown",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_663>,	&EntityByNameTyped<&ByName_663> },
	{ "playMenuNavigationNot",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_664>,	&EntityByNameTyped<&ByName_664> },
	{ "playMenuPagePrevious",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_665>,	&EntityByNameTyped<&ByName_665> },
	{ "playMenuPageNext",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_666>,	&EntityByNameTyped<&ByName_666> },
	{ "playDismissedReportScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_667>,	&EntityByNameTyped<&ByName_667> },
	{ "playDismissedMissionScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_668>,	&EntityByNameTyped<&ByName_668> },
	{ "playChangedOption",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_669>,	&EntityByNameTyped<&ByName_669> },
	{ "updateAfterburnerSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_670>,	&EntityByNameTyped<&ByName_670> },
	{ "startAfterburnerSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_671>,	&EntityByNameTyped<&ByName_671> },
	{ "stopAfterburnerSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_672>,	&EntityByNameTyped<&ByName_672> },
	{ "playCloakingDeviceInsufficientEnergy",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_673>,	&EntityByNameTyped<&ByName_673> },
	{ "playBuyCommodity",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_674>,	&EntityByNameTyped<&ByName_674> },
	{ "playBuyShip",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_675>,	&EntityByNameTyped<&ByName_675> },
	{ "playSellCommodity",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_676>,	&EntityByNameTyped<&ByName_676> },
	{ "playCantBuyCommodity",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_677>,	&EntityByNameTyped<&ByName_677> },
	{ "playCantSellCommodity",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_678>,	&EntityByNameTyped<&ByName_678> },
	{ "playCantBuyShip",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_679>,	&EntityByNameTyped<&ByName_679> },
	{ "playStandardHyperspace",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_680>,	&EntityByNameTyped<&ByName_680> },
	{ "playGalacticHyperspace",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_681>,	&EntityByNameTyped<&ByName_681> },
	{ "playHyperspaceAborted",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_682>,	&EntityByNameTyped<&ByName_682> },
	{ "playHitByECMSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_683>,	&EntityByNameTyped<&ByName_683> },
	{ "playFiredECMSound",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_684>,	&EntityByNameTyped<&ByName_684> },
	{ "playLaunchFromStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_685>,	&EntityByNameTyped<&ByName_685> },
	{ "playDockWithStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_686>,	&EntityByNameTyped<&ByName_686> },
	{ "playExitWitchspace",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_687>,	&EntityByNameTyped<&ByName_687> },
	{ "playHostileWarning",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_688>,	&EntityByNameTyped<&ByName_688> },
	{ "playAlertConditionRed",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_689>,	&EntityByNameTyped<&ByName_689> },
	{ "playEnergyLow",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_690>,	&EntityByNameTyped<&ByName_690> },
	{ "playDockingDenied",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_691>,	&EntityByNameTyped<&ByName_691> },
	{ "playWitchjumpFailure",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_692>,	&EntityByNameTyped<&ByName_692> },
	{ "playWitchjumpMisjump",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_693>,	&EntityByNameTyped<&ByName_693> },
	{ "playWitchjumpBlocked",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_694>,	&EntityByNameTyped<&ByName_694> },
	{ "playWitchjumpDistanceTooGreat",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_695>,	&EntityByNameTyped<&ByName_695> },
	{ "playWitchjumpInsufficientFuel",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_696>,	&EntityByNameTyped<&ByName_696> },
	{ "playFuelLeak",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_697>,	&EntityByNameTyped<&ByName_697> },
	{ "playEscapePodScooped",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_698>,	&EntityByNameTyped<&ByName_698> },
	{ "playAegisCloseToPlanet",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_699>,	&EntityByNameTyped<&ByName_699> },
	{ "playAegisCloseToStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_700>,	&EntityByNameTyped<&ByName_700> },
	{ "playGameOver",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_701>,	&EntityByNameTyped<&ByName_701> },
	{ "playLegacyScriptSound:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_702>,	nullptr },
	{ "scheduleAfterburnerSoundUpdate",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_703>,	&EntityByNameTyped<&ByName_703> },
	{ "resetStickFunctions",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_704>,	&EntityByNameTyped<&ByName_704> },
	{ "updateFunction:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_705>,	nullptr },
	{ "makeStickGuiDictHeader:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_706>,	nullptr },
	{ "initControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_707>,	&EntityByNameTyped<&ByName_707> },
	{ "initKeyConfigSettings",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_708>,	&EntityByNameTyped<&ByName_708> },
	{ "processKeyCode:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_709>,	nullptr },
	{ "checkNavKeyPress:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_710>,	nullptr },
	{ "checkKeyPress:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_711>,	nullptr },
	{ "getFirstKeyCode:",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_712>,	nullptr },
	{ "handleGUIUpDownArrowKeys",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_713>,	&EntityByNameTyped<&ByName_713> },
	{ "clearPlanetSearchString",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_714>,	&EntityByNameTyped<&ByName_714> },
	{ "switchToMainView",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_715>,	&EntityByNameTyped<&ByName_715> },
	{ "beginWitchspaceCountdown",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_716>,	&EntityByNameTyped<&ByName_716> },
	{ "cancelWitchspaceCountdown",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_717>,	&EntityByNameTyped<&ByName_717> },
	{ "pollApplicationControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_718>,	&EntityByNameTyped<&ByName_718> },
	{ "pollMarketScreenControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_719>,	&EntityByNameTyped<&ByName_719> },
	{ "handleGameOptionsScreenKeys",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_720>,	&EntityByNameTyped<&ByName_720> },
	{ "handleKeyMapperScreenKeys",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_721>,	&EntityByNameTyped<&ByName_721> },
	{ "handleKeyboardLayoutKeys",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_722>,	&EntityByNameTyped<&ByName_722> },
	{ "handleStickMapperScreenKeys",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_723>,	&EntityByNameTyped<&ByName_723> },
	{ "pollCustomViewControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_724>,	&EntityByNameTyped<&ByName_724> },
	{ "pollViewControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_725>,	&EntityByNameTyped<&ByName_725> },
	{ "pollGuiScreenControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_726>,	&EntityByNameTyped<&ByName_726> },
	{ "handleUndockControl",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_727>,	&EntityByNameTyped<&ByName_727> },
	{ "pollMissionInterruptControls",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_728>,	&EntityByNameTyped<&ByName_728> },
	{ "handleMissionCallback",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_729>,	&EntityByNameTyped<&ByName_729> },
	{ "setGuiToMissionEndScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_730>,	&EntityByNameTyped<&ByName_730> },
	{ "handleButtonIdent",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_731>,	&EntityByNameTyped<&ByName_731> },
	{ "handleButtonTargetMissile",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_732>,	&EntityByNameTyped<&ByName_732> },
	{ "initCheckingDictionary",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_733>,	&EntityByNameTyped<&ByName_733> },
	{ "resetKeyFunctions",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_734>,	&EntityByNameTyped<&ByName_734> },
	{ "entryIsDictCustomEquip:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_735>,	nullptr },
	{ "entryIsCustomEquip:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_736>,	nullptr },
	{ "getCustomEquipArray:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_737>,	nullptr },
	{ "getCustomEquipIndex:",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_738>,	nullptr },
	{ "setGuiToKeyConfigScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_739>,	&EntityByNameTyped<&ByName_739> },
	{ "setGuiToKeyConfigEntryScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_740>,	&EntityByNameTyped<&ByName_740> },
	{ "setGuiToConfirmClearScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_741>,	&EntityByNameTyped<&ByName_741> },
	{ "makeKeyGuiDictHeader:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_742>,	nullptr },
	{ "entryIsEqualToDefault:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_743>,	nullptr },
	{ "saveKeySetting:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_744>,	nullptr },
	{ "unsetKeySetting:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_745>,	nullptr },
	{ "deleteKeySetting:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_746>,	nullptr },
	{ "deleteAllKeySettings",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_747>,	&EntityByNameTyped<&ByName_747> },
	{ "loadKeySettings",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_748>,	&EntityByNameTyped<&ByName_748> },
	{ "reloadPage",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_749>,	&EntityByNameTyped<&ByName_749> },
	{ "scriptTarget",	ByName::kPlayer,	oo::kValueType<ShipEntity *>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_750>,	&EntityByNameTyped<&ByName_750> },
	{ "checkScript",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_751>,	&EntityByNameTyped<&ByName_751> },
	{ "scriptTestConditions:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_752>,	nullptr },
	{ "scriptTestCondition:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_753>,	nullptr },
	{ "missionVariables",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_754>,	&EntityByNameTyped<&ByName_754> },
	{ "missionVariableForKey:",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_755>,	nullptr },
	{ "missionsList",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_756>,	&EntityByNameTyped<&ByName_756> },
	{ "setMissionDescription:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_757>,	nullptr },
	{ "clearMissionDescription",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_758>,	&EntityByNameTyped<&ByName_758> },
	{ "clearMissionDescriptionForMission:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_759>,	nullptr },
	{ "mission_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_760>,	&EntityByNameTyped<&ByName_760> },
	{ "status_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_761>,	&EntityByNameTyped<&ByName_761> },
	{ "gui_screen_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_762>,	&EntityByNameTyped<&ByName_762> },
	{ "galaxy_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_763>,	&EntityByNameTyped<&ByName_763> },
	{ "planet_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_764>,	&EntityByNameTyped<&ByName_764> },
	{ "score_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_765>,	&EntityByNameTyped<&ByName_765> },
	{ "credits_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_766>,	&EntityByNameTyped<&ByName_766> },
	{ "scriptTimer_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_767>,	&EntityByNameTyped<&ByName_767> },
	{ "shipsFound_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_768>,	&EntityByNameTyped<&ByName_768> },
	{ "commanderLegalStatus_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_769>,	&EntityByNameTyped<&ByName_769> },
	{ "setLegalStatus:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_770>,	nullptr },
	{ "commanderLegalStatus_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_771>,	&EntityByNameTyped<&ByName_771> },
	{ "d100_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_772>,	&EntityByNameTyped<&ByName_772> },
	{ "pseudoFixedD100_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_773>,	&EntityByNameTyped<&ByName_773> },
	{ "d256_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_774>,	&EntityByNameTyped<&ByName_774> },
	{ "pseudoFixedD256_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_775>,	&EntityByNameTyped<&ByName_775> },
	{ "clock_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_776>,	&EntityByNameTyped<&ByName_776> },
	{ "clock_secs_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_777>,	&EntityByNameTyped<&ByName_777> },
	{ "clock_mins_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_778>,	&EntityByNameTyped<&ByName_778> },
	{ "clock_hours_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_779>,	&EntityByNameTyped<&ByName_779> },
	{ "clock_days_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_780>,	&EntityByNameTyped<&ByName_780> },
	{ "fuelLevel_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_781>,	&EntityByNameTyped<&ByName_781> },
	{ "dockedAtMainStation_bool",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_782>,	&EntityByNameTyped<&ByName_782> },
	{ "foundEquipment_bool",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_783>,	&EntityByNameTyped<&ByName_783> },
	{ "sunWillGoNova_bool",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_784>,	&EntityByNameTyped<&ByName_784> },
	{ "sunGoneNova_bool",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_785>,	&EntityByNameTyped<&ByName_785> },
	{ "missionChoice_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_786>,	&EntityByNameTyped<&ByName_786> },
	{ "missionKeyPress_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_787>,	&EntityByNameTyped<&ByName_787> },
	{ "dockedTechLevel_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_788>,	&EntityByNameTyped<&ByName_788> },
	{ "dockedStationName_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_789>,	&EntityByNameTyped<&ByName_789> },
	{ "systemGovernment_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_790>,	&EntityByNameTyped<&ByName_790> },
	{ "systemGovernment_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_791>,	&EntityByNameTyped<&ByName_791> },
	{ "systemEconomy_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_792>,	&EntityByNameTyped<&ByName_792> },
	{ "systemEconomy_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_793>,	&EntityByNameTyped<&ByName_793> },
	{ "systemTechLevel_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_794>,	&EntityByNameTyped<&ByName_794> },
	{ "systemPopulation_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_795>,	&EntityByNameTyped<&ByName_795> },
	{ "systemProductivity_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_796>,	&EntityByNameTyped<&ByName_796> },
	{ "commanderName_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_797>,	&EntityByNameTyped<&ByName_797> },
	{ "commanderRank_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_798>,	&EntityByNameTyped<&ByName_798> },
	{ "commanderShip_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_799>,	&EntityByNameTyped<&ByName_799> },
	{ "commanderShipDisplayName_string",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_800>,	&EntityByNameTyped<&ByName_800> },
	{ "consoleMessage3s:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_801>,	nullptr },
	{ "consoleMessage6s:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_802>,	nullptr },
	{ "awardCredits:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_803>,	nullptr },
	{ "awardShipKills:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_804>,	nullptr },
	{ "awardEquipment:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_805>,	nullptr },
	{ "removeEquipment:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_806>,	nullptr },
	{ "setPlanetinfo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_807>,	nullptr },
	{ "setSpecificPlanetInfo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_808>,	nullptr },
	{ "awardCargo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_809>,	nullptr },
	{ "removeAllCargo",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_810>,	&EntityByNameTyped<&ByName_810> },
	{ "useSpecialCargo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_811>,	nullptr },
	{ "testForEquipment:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_812>,	nullptr },
	{ "awardFuel:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_813>,	nullptr },
	{ "messageShipAIs:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_814>,	nullptr },
	{ "ejectItem:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_815>,	nullptr },
	{ "addShips:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_816>,	nullptr },
	{ "addSystemShips:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_817>,	nullptr },
	{ "addShipsAt:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_818>,	nullptr },
	{ "addShipsAtPrecisely:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_819>,	nullptr },
	{ "addShipsWithinRadius:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_820>,	nullptr },
	{ "spawnShip:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_821>,	nullptr },
	{ "set:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_822>,	nullptr },
	{ "reset:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_823>,	nullptr },
	{ "increment:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_824>,	nullptr },
	{ "decrement:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_825>,	nullptr },
	{ "add:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_826>,	nullptr },
	{ "subtract:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_827>,	nullptr },
	{ "checkForShips:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_828>,	nullptr },
	{ "resetScriptTimer",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_829>,	&EntityByNameTyped<&ByName_829> },
	{ "addMissionText:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_830>,	nullptr },
	{ "addLiteralMissionText:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_831>,	nullptr },
	{ "setMissionChoices:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_832>,	nullptr },
	{ "setMissionChoicesDictionary:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_833>,	nullptr },
	{ "resetMissionChoice",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_834>,	&EntityByNameTyped<&ByName_834> },
	{ "clearMissionScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_835>,	&EntityByNameTyped<&ByName_835> },
	{ "addMissionDestination:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_836>,	nullptr },
	{ "removeMissionDestination:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_837>,	nullptr },
	{ "showShipModel:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_838>,	nullptr },
	{ "setMissionMusic:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_839>,	nullptr },
	{ "setMissionImage:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_840>,	nullptr },
	{ "setMissionBackground:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_841>,	nullptr },
	{ "setFuelLeak:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_842>,	nullptr },
	{ "fuelLeakRate_number",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_843>,	&EntityByNameTyped<&ByName_843> },
	{ "setSunNovaIn:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_844>,	nullptr },
	{ "launchFromStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_845>,	&EntityByNameTyped<&ByName_845> },
	{ "blowUpStation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_846>,	&EntityByNameTyped<&ByName_846> },
	{ "sendAllShipsAway",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_847>,	&EntityByNameTyped<&ByName_847> },
	{ "addPlanet:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_848>,	nullptr },
	{ "addMoon:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_849>,	nullptr },
	{ "debugOn",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_850>,	&EntityByNameTyped<&ByName_850> },
	{ "debugOff",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_851>,	&EntityByNameTyped<&ByName_851> },
	{ "debugMessage:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_852>,	nullptr },
	{ "playSound:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_853>,	nullptr },
	{ "doMissionCallback",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_854>,	&EntityByNameTyped<&ByName_854> },
	{ "clearMissionScreenID",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_855>,	&EntityByNameTyped<&ByName_855> },
	{ "endMissionScreenAndNoteOpportunity",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_856>,	&EntityByNameTyped<&ByName_856> },
	{ "setGuiToMissionScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_857>,	&EntityByNameTyped<&ByName_857> },
	{ "refreshMissionScreenTextEntry",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_858>,	&EntityByNameTyped<&ByName_858> },
	{ "setBackgroundFromDescriptionsKey:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_859>,	nullptr },
	{ "addEqScriptForKey:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_860>,	nullptr },
	{ "removeEqScriptForKey:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_861>,	nullptr },
	{ "eqScriptIndexForKey:",	ByName::kPlayer,	oo::kValueType<std::size_t>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_862>,	nullptr },
	{ "targetNearestHostile",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_863>,	&EntityByNameTyped<&ByName_863> },
	{ "targetNearestIncomingMissile",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_864>,	&EntityByNameTyped<&ByName_864> },
	{ "setGalacticHyperspaceBehaviourTo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_865>,	nullptr },
	{ "setGalacticHyperspaceFixedCoordsTo:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_866>,	nullptr },
	{ "contractedVolumeForGood:",	ByName::kPlayer,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_867>,	nullptr },
	{ "addMessageToReport:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_868>,	nullptr },
	{ "reputation",	ByName::kPlayer,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_869>,	&EntityByNameTyped<&ByName_869> },
	{ "passengerReputation",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_870>,	&EntityByNameTyped<&ByName_870> },
	{ "parcelReputation",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_871>,	&EntityByNameTyped<&ByName_871> },
	{ "contractReputation",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_872>,	&EntityByNameTyped<&ByName_872> },
	{ "erodeReputation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_873>,	&EntityByNameTyped<&ByName_873> },
	{ "normaliseReputation",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_874>,	&EntityByNameTyped<&ByName_874> },
	{ "removePassenger:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_875>,	nullptr },
	{ "removeParcel:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_876>,	nullptr },
	{ "setGuiToManifestScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_877>,	&EntityByNameTyped<&ByName_877> },
	{ "setGuiToDockingReportScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_878>,	&EntityByNameTyped<&ByName_878> },
	{ "priceForShipKey:",	ByName::kPlayer,	oo::kValueType<OOCreditsQuantity>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_879>,	nullptr },
	{ "showShipyardInfoForSelection",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_880>,	&EntityByNameTyped<&ByName_880> },
	{ "showTradeInInformationFooter",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_881>,	&EntityByNameTyped<&ByName_881> },
	{ "missingSubEntitiesAdjustment",	ByName::kPlayer,	oo::kValueType<std::ptrdiff_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_882>,	&EntityByNameTyped<&ByName_882> },
	{ "tradeInValue",	ByName::kPlayer,	oo::kValueType<OOCreditsQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_883>,	&EntityByNameTyped<&ByName_883> },
	{ "buySelectedShip",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_884>,	&EntityByNameTyped<&ByName_884> },
	{ "replaceShipWithNamedShip:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_885>,	nullptr },
	{ "loadPlayer",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_886>,	&EntityByNameTyped<&ByName_886> },
	{ "savePlayer",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_887>,	&EntityByNameTyped<&ByName_887> },
	{ "autosavePlayer",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_888>,	&EntityByNameTyped<&ByName_888> },
	{ "quicksavePlayer",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_889>,	&EntityByNameTyped<&ByName_889> },
	{ "addScenarioModel:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_890>,	nullptr },
	{ "showScenarioDetails",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_891>,	&EntityByNameTyped<&ByName_891> },
	{ "startScenario",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_892>,	&EntityByNameTyped<&ByName_892> },
	{ "saveCommanderInputHandler",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_893>,	&EntityByNameTyped<&ByName_893> },
	{ "overwriteCommanderInputHandler",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_894>,	&EntityByNameTyped<&ByName_894> },
	{ "writePlayerToPath:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_895>,	nullptr },
	{ "nativeSavePlayer:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_896>,	nullptr },
	{ "setGuiToLoadCommanderScreen",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_897>,	&EntityByNameTyped<&ByName_897> },
	{ "setGuiToSaveCommanderScreen:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_898>,	nullptr },
	{ "setGuiToOverwriteScreen:",	ByName::kPlayer,	oo::kValueType<void>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_899>,	nullptr },
	{ "existingNativeSave:",	ByName::kPlayer,	oo::kValueType<bool>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_900>,	nullptr },
	{ "findIndexOfCommander:",	ByName::kPlayer,	oo::kValueType<int>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_901>,	nullptr },
	{ "equivalentTechLevel",	ByName::kStation,	oo::kValueType<OOTechLevelID>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_902>,	&EntityByNameTyped<&ByName_902> },
	{ "virtualPortDimensions",	ByName::kStation,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_903>,	&EntityByNameTyped<&ByName_903> },
	{ "playerReservedDock",	ByName::kStation,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_904>,	&EntityByNameTyped<&ByName_904> },
	{ "beaconPosition",	ByName::kStation,	oo::kValueType<HPVector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_905>,	&EntityByNameTyped<&ByName_905> },
	{ "equipmentPriceFactor",	ByName::kStation,	oo::kValueType<float>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_906>,	&EntityByNameTyped<&ByName_906> },
	{ "marketCapacity",	ByName::kStation,	oo::kValueType<OOCargoQuantity>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_907>,	&EntityByNameTyped<&ByName_907> },
	{ "marketDefinition",	ByName::kStation,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_908>,	&EntityByNameTyped<&ByName_908> },
	{ "marketMonitored",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_909>,	&EntityByNameTyped<&ByName_909> },
	{ "marketBroadcast",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_910>,	&EntityByNameTyped<&ByName_910> },
	{ "setLocalMarket:",	ByName::kStation,	oo::kValueType<void>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_911>,	nullptr },
	{ "localMarketForScripting",	ByName::kStation,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_912>,	&EntityByNameTyped<&ByName_912> },
	{ "countOfDockedContractors",	ByName::kStation,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_913>,	&EntityByNameTyped<&ByName_913> },
	{ "countOfDockedPolice",	ByName::kStation,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_914>,	&EntityByNameTyped<&ByName_914> },
	{ "countOfDockedDefenders",	ByName::kStation,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_915>,	&EntityByNameTyped<&ByName_915> },
	{ "interstellarUndockingAllowed",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_916>,	&EntityByNameTyped<&ByName_916> },
	{ "hasNPCTraffic",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_917>,	&EntityByNameTyped<&ByName_917> },
	{ "requiresDockingClearance",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_918>,	&EntityByNameTyped<&ByName_918> },
	{ "allowsFastDocking",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_919>,	&EntityByNameTyped<&ByName_919> },
	{ "allowsAutoDocking",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_920>,	&EntityByNameTyped<&ByName_920> },
	{ "allowsSaving",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_921>,	&EntityByNameTyped<&ByName_921> },
	{ "isRotatingStation",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_922>,	&EntityByNameTyped<&ByName_922> },
	{ "hasShipyard",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_923>,	&EntityByNameTyped<&ByName_923> },
	{ "generateShipyard",	ByName::kStation,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_924>,	&EntityByNameTyped<&ByName_924> },
	{ "suppressArrivalReports",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_925>,	&EntityByNameTyped<&ByName_925> },
	{ "hasBreakPattern",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_926>,	&EntityByNameTyped<&ByName_926> },
	{ "sanityCheckShipsOnApproach",	ByName::kStation,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_927>,	&EntityByNameTyped<&ByName_927> },
	{ "autoDockShipsOnHold",	ByName::kStation,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_928>,	&EntityByNameTyped<&ByName_928> },
	{ "autoDockShipsOnApproach",	ByName::kStation | ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_929>,	&EntityByNameTyped<&ByName_929> },
	{ "dockingCorridorIsEmpty",	ByName::kStation | ByName::kDock,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_930>,	&EntityByNameTyped<&ByName_930> },
	{ "clearDockingCorridor",	ByName::kStation | ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_931>,	&EntityByNameTyped<&ByName_931> },
	{ "clear",	ByName::kStation | ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_932>,	&EntityByNameTyped<&ByName_932> },
	{ "hasMultipleDocks",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_933>,	&EntityByNameTyped<&ByName_933> },
	{ "hasClearDock",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_934>,	&EntityByNameTyped<&ByName_934> },
	{ "hasEligibleDock",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_935>,	&EntityByNameTyped<&ByName_935> },
	{ "hasLaunchDock",	ByName::kStation,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_936>,	&EntityByNameTyped<&ByName_936> },
	{ "selectDockForDocking",	ByName::kStation,	oo::kValueType<oo::ObjectResult>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_937>,	&EntityByNameTyped<&ByName_937> },
	{ "countOfShipsInLaunchQueueWithPrimaryRole:",	ByName::kStation | ByName::kDock,	oo::kValueType<unsigned>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_938>,	nullptr },
	{ "alertLevel",	ByName::kStation,	oo::kValueType<StationAlertLevel>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_939>,	&EntityByNameTyped<&ByName_939> },
	{ "currentlyInDockingQueues",	ByName::kStation,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_940>,	&EntityByNameTyped<&ByName_940> },
	{ "currentlyInLaunchingQueues",	ByName::kStation,	oo::kValueType<unsigned>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_941>,	&EntityByNameTyped<&ByName_941> },
	{ "launchIndependentShip:",	ByName::kStation,	oo::kValueType<oo::PList>,	EntityByNameArgument::kString,	&EntityByNameAdaptString<&ByName_942>,	nullptr },
	{ "allowsDocking",	ByName::kDock,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_943>,	&EntityByNameTyped<&ByName_943> },
	{ "disallowedDockingCollides",	ByName::kDock,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_944>,	&EntityByNameTyped<&ByName_944> },
	{ "countOfShipsInDockingQueue",	ByName::kDock,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_945>,	&EntityByNameTyped<&ByName_945> },
	{ "allowsLaunching",	ByName::kDock,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_946>,	&EntityByNameTyped<&ByName_946> },
	{ "countOfShipsInLaunchQueue",	ByName::kDock,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_947>,	&EntityByNameTyped<&ByName_947> },
	{ "isOffCentre",	ByName::kDock,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_948>,	&EntityByNameTyped<&ByName_948> },
	{ "setVirtual",	ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_949>,	&EntityByNameTyped<&ByName_949> },
	{ "clearAllIdLocks",	ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_950>,	&EntityByNameTyped<&ByName_950> },
	{ "pruneAndCountShipsOnApproach",	ByName::kDock,	oo::kValueType<std::size_t>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_951>,	&EntityByNameTyped<&ByName_951> },
	{ "abortAllLaunches",	ByName::kDock,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_952>,	&EntityByNameTyped<&ByName_952> },
	{ "setUpVisualEffectFromDictionary:",	ByName::kEffect,	oo::kValueType<bool>,	EntityByNameArgument::kPList,	&EntityByNameAdaptPList<&ByName_953>,	nullptr },
	{ "setNoDrawDistance",	ByName::kEffect,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_954>,	&EntityByNameTyped<&ByName_954> },
	{ "scaleMax",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_955>,	&EntityByNameTyped<&ByName_955> },
	{ "scaleX",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_956>,	&EntityByNameTyped<&ByName_956> },
	{ "scaleY",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_957>,	&EntityByNameTyped<&ByName_957> },
	{ "scaleZ",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_958>,	&EntityByNameTyped<&ByName_958> },
	{ "isBreakPattern",	ByName::kEffect,	oo::kValueType<bool>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_959>,	&EntityByNameTyped<&ByName_959> },
	{ "effectInfoDictionary",	ByName::kEffect,	oo::kValueType<oo::PList>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_960>,	&EntityByNameTyped<&ByName_960> },
	{ "remove",	ByName::kEffect,	oo::kValueType<void>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_961>,	&EntityByNameTyped<&ByName_961> },
	{ "shaderFloat1",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_962>,	&EntityByNameTyped<&ByName_962> },
	{ "shaderFloat2",	ByName::kEffect,	oo::kValueType<GLfloat>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_963>,	&EntityByNameTyped<&ByName_963> },
	{ "shaderInt1",	ByName::kEffect,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_964>,	&EntityByNameTyped<&ByName_964> },
	{ "shaderInt2",	ByName::kEffect,	oo::kValueType<int>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_965>,	&EntityByNameTyped<&ByName_965> },
	{ "shaderVector1",	ByName::kEffect,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_966>,	&EntityByNameTyped<&ByName_966> },
	{ "shaderVector2",	ByName::kEffect,	oo::kValueType<Vector>,	EntityByNameArgument::kNone,	&EntityByNameAdaptNone<&ByName_967>,	&EntityByNameTyped<&ByName_967> },
};

}  // namespace oo

