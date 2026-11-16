/*

OOWaypointEntity.m

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

#include "WaypointEntity.h"

#include <memory>
#include <string>
#include <string_view>

#include "Entity.h"
#include "HeadUpDisplay.h"
#include "MacroOpenGL.h"
#include "OOJSWaypoint.h"
#include "OpenGL.h"
#include "PListGameTypes.h"
#include "PlayerEntity.h"
#include "PolygonSprite.h"
#include "StringExpander.h"
#include "Universe.h"
#include "oofnd/PListGet.hpp"
#include "oofnd/String.hpp"

constexpr std::string_view kWaypointKeyPosition = "position";
constexpr std::string_view kWaypointKeyOrientation = "orientation";
constexpr std::string_view kWaypointKeySize = "size";
constexpr std::string_view kWaypointKeyCode = "beaconCode";
constexpr std::string_view kWaypointKeyLabel = "beaconLabel";

std::shared_ptr<oo::WaypointEntity> oo::WaypointEntity::waypointWithDictionary(
    const oo::PList& info) {
  const std::shared_ptr<oo::WaypointEntity> waypoint =
      oo::makeShared<oo::WaypointEntity>();
  waypoint->initWithDictionary(info);
  return waypoint;
}

void oo::WaypointEntity::initWithDictionary(const oo::PList& info) {
  // self = [super init]: the constructor ran Entity's -init body.

  // A nil dictionary read zero-filled values and nil strings (messaging nil),
  // not the defaults.
  oriented = true;
  position = info ? oo::HPVectorFromPList(info.find(kWaypointKeyPosition),
                                          kZeroHPVector)
                  : kZeroHPVector;
  Quaternion q =
      info ? oo::QuaternionFromPList(info.find(kWaypointKeyOrientation),
                                     kIdentityQuaternion)
           : Quaternion{0, 0, 0, 0};
  setOrientation(q);
  setSize(info.get<oo::NonNegative<float>>(kWaypointKeySize, 1000.0));
  setBeaconCode(info ? std::optional<std::string>(
                           info.get<std::string>(kWaypointKeyCode, "W"))
                     : std::nullopt);
  setBeaconLabel(info ? std::optional<std::string>(info.get<std::string>(
                            kWaypointKeyLabel, "Waypoint"))
                      : std::nullopt);

  setStatus(oo::EntityStatus::kEffect);
  setScanClass(oo::ScanClass::kNoDraw);
}

// override
void oo::WaypointEntity::setOrientation(Quaternion q) {
  if (quaternion_equal(q, kZeroQuaternion)) {
    q = kIdentityQuaternion;
    oriented = false;
  } else {
    oriented = true;
  }
  Entity::setOrientation(q);
}

bool oo::WaypointEntity::getOriented() { return oriented; }

oo::Scalar oo::WaypointEntity::size() { return _size; }

void oo::WaypointEntity::setSize(oo::Scalar newSize) {
  if (newSize > 0) {
    _size = newSize;
    no_draw_distance =
        static_cast<GLfloat>(newSize * newSize * NO_DRAW_DISTANCE_FACTOR *
                             NO_DRAW_DISTANCE_FACTOR * 2);
  }
}

bool oo::WaypointEntity::isEffect() { return true; }

bool oo::WaypointEntity::isWaypoint() { return true; }

void oo::WaypointEntity::drawImmediate(bool /*immediate*/, bool translucent) {
  if (!translucent || no_draw_distance < cam_zero_distance) {
    return;
  }

  if (!(PLAYER != nullptr
            ? PLAYER->hasEquipmentItemProviding("EQ_ADVANCED_COMPASS")
            : false)) {
    return;
  }

  int8_t i, j, k;

  GLfloat a = 0.75;
  if ((PLAYER != nullptr ? PLAYER->getCompassTargetEntity() : nullptr) !=
      this) {
    a *= static_cast<GLfloat>(0.25);
  }
  if (cam_zero_distance > _size * _size) {
    // dim out as gets further away; 2-D HUD display more
    // important at long range
    a -= 0.004f * (sqrtf(cam_zero_distance) / _size);
  }
  if (a < 0.01f) {
    return;
  }

  GLfloat s0 = _size;
  GLfloat s1 = _size * 0.75f;

  OO_ENTER_OPENGL();
  OOSetOpenGLState(oo::OpenGLStateID::kTranslucentPass);
  OOGL(glEnable(GL_BLEND));
  GLScaledLineWidth(1.0);

  OOGL(glColor4f(0.0, 0.0, 1.0, a));
  OOGLBEGIN(GL_LINES);
  for (i = -1; i <= 1; i += 2) {
    for (j = -1; j <= 1; j += 2) {
      for (k = -1; k <= 1; k += 2) {
        glVertex3f(i * s0, j * s0, k * s1);
        glVertex3f(i * s0, j * s1, k * s0);
        glVertex3f(i * s0, j * s1, k * s0);
        glVertex3f(i * s1, j * s0, k * s0);
        glVertex3f(i * s1, j * s0, k * s0);
        glVertex3f(i * s0, j * s0, k * s1);
      }
    }
  }
  if (oriented) {
    while (s1 > 20.0f) {
      glVertex3f(-20.0, 0, -s1 - 20.0f);
      glVertex3f(0, 0, -s1);
      glVertex3f(20.0, 0, -s1 - 20.0f);
      glVertex3f(0, 0, -s1);
      glVertex3f(-20.0, 0, s1 - 20.0f);
      glVertex3f(0, 0, s1);
      glVertex3f(20.0, 0, s1 - 20.0f);
      glVertex3f(0, 0, s1);
      s1 *= static_cast<GLfloat>(0.5);
    }
  }
  OOGLEND();

  OOGL(glDisable(GL_BLEND));
  OOVerifyOpenGLState();
}

/* beacons */

oo::ComparisonResult oo::WaypointEntity::compareBeaconCodeWith(Entity* other) {
  return static_cast<oo::ComparisonResult>(oo::str::caseInsensitiveCompare(
      beaconCode().value_or(""), oo::BeaconCode(other).value_or("")));
}

std::optional<std::string> oo::WaypointEntity::beaconCode() {
  return _beaconCode;
}

// bcode: optional string; empty is treated as none. The Foundation version
// compared the new string with the old by pointer, so any new string (every
// string this class hands out is new) replaced it.
void oo::WaypointEntity::setBeaconCode(
    const std::optional<std::string>& bcode) {
  std::optional<std::string> code = bcode;
  if (code.has_value() && code->empty()) code.reset();

  if (code.has_value() || _beaconCode.has_value()) {
    _beaconCode = code;

    _beaconDrawable = nullptr;
  }
  // if not blanking code and label is currently blank, default label to code
  if (code.has_value() &&
      (!_beaconLabel.has_value() || _beaconLabel->empty())) {
    setBeaconLabel(code);
  }
}

std::optional<std::string> oo::WaypointEntity::beaconLabel() {
  return _beaconLabel;
}

void oo::WaypointEntity::setBeaconLabel(
    const std::optional<std::string>& blabel) {
  std::optional<std::string> label = blabel;
  if (label.has_value() && label->empty()) label.reset();

  if (label.has_value() || _beaconLabel.has_value()) {
    _beaconLabel = label.has_value() ? OOExpand(*label) : std::nullopt;
  }
}

bool oo::WaypointEntity::isBeacon() { return beaconCode().has_value(); }

oo::HUDBeaconIcon* oo::WaypointEntity::beaconDrawable() {
  if (_beaconDrawable == nullptr) {
    const std::u16string beaconCode =
        oo::utf8ToUtf16(_beaconCode.value_or(std::string()));
    std::size_t length = beaconCode.size();  // -length: UTF-16 units

    if (length > 1) {
      const oo::PList* iconEntry = UNIVERSE->descriptions()->find(*_beaconCode);
      const oo::PList iconData =
          (iconEntry != nullptr) ? *iconEntry : oo::PList();
      if (iconData.isArray())
        _beaconDrawable = oo::PolygonSprite::initWithDataArray(
            iconData, 0.5, *_beaconCode);  // null where it answered nil
    }

    if (_beaconDrawable == nullptr) {
      if (length > 0)
        _beaconDrawable = oo::makeShared<oo::HUDBeaconCodeIcon>(
            oo::utf16ToUtf8(beaconCode.substr(0, 1)));  // -substringToIndex:1
      else
        _beaconDrawable = oo::makeShared<oo::HUDBeaconCodeIcon>(std::string());
    }
  }

  return _beaconDrawable.get();
}

Entity* oo::WaypointEntity::prevBeacon() { return oo::LiveEntity(_prevBeacon); }

Entity* oo::WaypointEntity::nextBeacon() { return oo::LiveEntity(_nextBeacon); }

void oo::WaypointEntity::setPrevBeacon(Entity* beaconShip) {
  if (beaconShip != prevBeacon()) {
    _prevBeacon = std::weak_ptr<Entity>(oo::share(beaconShip));
  }
}

void oo::WaypointEntity::setNextBeacon(Entity* beaconShip) {
  if (beaconShip != nextBeacon()) {
    _nextBeacon = std::weak_ptr<Entity>(oo::share(beaconShip));
  }
}

bool oo::WaypointEntity::isJammingScanning() { return false; }

// The binding's bodies.
JSClassID oo::WaypointEntity::jsClassID() { return ::JSWaypointClassID(); }

std::optional<std::string> oo::WaypointEntity::jsClassName() {
  return ::OOJSWaypointJSClassName();
}

bool oo::WaypointEntity::isVisibleToScripts() {
  return ::OOJSWaypointIsVisibleToScripts();
}
