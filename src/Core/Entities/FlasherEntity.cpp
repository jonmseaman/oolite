/*

OOFlasherEntity.m


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

#include "FlasherEntity.h"

#include <memory>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

#include "Color.h"
#include "OOJSFlasher.h"

std::shared_ptr<oo::FlasherEntity> oo::FlasherEntity::flasherWithDictionary(
    const oo::PList& dictionary) {
  const std::shared_ptr<oo::FlasherEntity> flasher =
      oo::makeShared<oo::FlasherEntity>();
  flasher->initWithDictionary(dictionary);
  return flasher;
}

void oo::FlasherEntity::initWithDictionary(const oo::PList& dictionary) {
  float size = dictionary.get<float>("size", 1.0f);

  oo::LightParticleEntity::initWithDiameter(size);
  // [super initWithDiameter:] could not fail.
  {
    _frequency = dictionary.get<float>("frequency", 1.0f) * 2.0f;
    _phase = dictionary.get<float>("phase", 0.0f);
    _brightfraction = dictionary.get<float>("bright_fraction", 0.5f);

    setUpColors(dictionary.get<oo::PList::Array>("colors"));
    getCurrentColorComponents();

    setActive(dictionary.get<bool>("initially_on", true));
  }
}

void oo::FlasherEntity::setUpColors(const oo::PList* colorSpecifiers) {
  std::vector<std::shared_ptr<oo::Color>> colors;
  if (colorSpecifiers != nullptr) {
    for (const oo::PList& specifier :
         *colorSpecifiers->getIf<oo::PList::Array>()) {
      colors.emplace_back(oo::Color::colorWithDescription(specifier, 0.75f));
    }
  }

  _colors = std::move(colors);
}

// The colour at index; null past the end (-objectAtIndex: raised there).
oo::Color* oo::FlasherEntity::flasherColorAtIndex(std::size_t index) {
  return index < _colors.size() ? _colors[index].get() : nullptr;
}

void oo::FlasherEntity::getCurrentColorComponents() {
  setColor(flasherColorAtIndex(_activeColor), _colorComponents[3]);
}

bool oo::FlasherEntity::isActive() { return _active; }

void oo::FlasherEntity::setActive(bool active) { _active = !!active; }

std::shared_ptr<oo::Color> oo::FlasherEntity::color() {
  return oo::Color::colorWithRed(_colorComponents[0], _colorComponents[1],
                                 _colorComponents[2], _colorComponents[3]);
}

float oo::FlasherEntity::frequency() { return _frequency; }

void oo::FlasherEntity::setFrequency(float frequency) {
  _frequency = frequency;
}

float oo::FlasherEntity::phase() { return _phase; }

void oo::FlasherEntity::setPhase(float phase) { _phase = phase; }

float oo::FlasherEntity::fraction() { return _brightfraction; }

void oo::FlasherEntity::setFraction(float fraction) {
  _brightfraction = fraction;
}

void oo::FlasherEntity::update(oo::TimeDelta delta_t) {
  oo::LightParticleEntity::update(delta_t);

  _time += delta_t;

  if (_frequency != 0) {
    float wave = sinf(
        static_cast<float>(_frequency * std::numbers::pi * (_time + _phase)));
    std::size_t count = _colors.size();
    if (count > 1 && wave < 0) {
      if (!_justSwitched && wave > _wave) {
        // don't test for wave >= _wave - could give wrong
        // results with very low frequencies
        _justSwitched = true;
        ++_activeColor;
        // _activeColor = ++_activeColor % count; is potentially undefined
        // operation
        _activeColor %= count;
        setColor(flasherColorAtIndex(_activeColor));
      }
    } else if (_justSwitched) {
      _justSwitched = false;
    }

    float threshold =
        cosf(static_cast<float>(_brightfraction * std::numbers::pi));

    float brightness = _brightfraction;
    if (wave > threshold) {
      brightness =
          _brightfraction +
          (((1 - _brightfraction) / (1 - threshold)) * (wave - threshold));
    } else if (wave < threshold) {
      brightness = _brightfraction +
                   ((_brightfraction / (threshold + 1)) * (wave - threshold));
    }

    _colorComponents[3] = brightness;

    _wave = wave;
  } else {
    _colorComponents[3] = 1.0;
  }
}

void oo::FlasherEntity::drawImmediate(bool immediate, bool translucent) {
  if (_active) {
    oo::LightParticleEntity::drawImmediate(immediate, translucent);
  }
}

void oo::FlasherEntity::drawSubEntityImmediate(bool immediate,
                                               bool translucent) {
  if (_active) {
    oo::LightParticleEntity::drawSubEntityImmediate(immediate, translucent);
  }
}

bool oo::FlasherEntity::isFlasher() { return true; }

double oo::FlasherEntity::findCollisionRadius() { return diameter() / 2.0; }

void oo::FlasherEntity::rescaleBy(GLfloat factor) {
  setDiameter(diameter() * factor);
}

void oo::FlasherEntity::rescaleBy(GLfloat /*factor*/, bool /*writeToCache*/) {
  /* Do nothing; this is only needed because of OOEntityWithDrawable
     implementation requirements */
}

// The binding's bodies (OOJSFlasher.cpp).
JSClassID oo::FlasherEntity::jsClassID() { return ::JSFlasherClassID(); }

std::optional<std::string> oo::FlasherEntity::jsClassName() {
  return ::OOJSFlasherJSClassName();
}

bool oo::FlasherEntity::isVisibleToScripts() {
  return ::OOJSFlasherIsVisibleToScripts();
}
