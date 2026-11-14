/*

OOHeadless.cpp


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

#include "OOHeadless.h"

#include <chrono>
#include <cstdint>

#include "oofnd/Date.hpp"
#include "oofnd/Exception.hpp"

namespace {
constexpr double kSecondsPerDay = 86400.0;
}  // namespace

void OOHeadlessOptions::validate() const {
  if (headless && !seed.has_value()) {
    OORaiseException(OOInvalidArgumentException, "%s",
                     "headless true requires seed to be given");
  }
  if (headless && addOns == AddOns::kAll) {
    OORaiseException(OOInvalidArgumentException, "%s",
                     "addOns is never all when headless is true");
  }
}

OOGameClock OOGameClock::system() { return OOGameClock(); }

OOGameClock OOGameClock::manual(std::uint32_t seed) {
  OOGameClock clock;
  clock._manual = true;
  clock._timeOfDayStart = manualTimeOfDayStart(seed);
  return clock;
}

double OOGameClock::manualTimeOfDayStart(std::uint32_t seed) {
  return static_cast<double>(seed % 86400U);
}

double OOGameClock::monotonicSeconds() const {
  if (_manual) return _elapsed;
  return oo::date::monotonicSeconds();
}

double OOGameClock::timeOfDay() const {
  if (_manual) {
    const double t = _timeOfDayStart + _elapsed;
    return t - kSecondsPerDay * static_cast<double>(static_cast<std::uint64_t>(
                                    t / kSecondsPerDay));
  }
  // Local time of day, as the ship clock's start has always read it.
  const oo::date::Clock::time_point now = oo::date::Clock::now();
  long long secondOfDay =
      (static_cast<long long>(
           std::chrono::floor<std::chrono::seconds>(now.time_since_epoch())
               .count()) +
       static_cast<long long>(oo::date::localUTCOffsetMinutes(now)) * 60) %
      86400;
  if (secondOfDay < 0) secondOfDay += 86400;
  return static_cast<double>(secondOfDay);
}

void OOGameClock::advance(double dt) {
  if (_manual) _elapsed += dt;
}

double OOSeededRandom::nextDouble() {
  // SplitMix64 (Steele, Lea and Flood, 2014).
  _state += 0x9e3779b97f4a7c15ULL;
  std::uint64_t z = _state;
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  z ^= z >> 31;
  return static_cast<double>(z >> 11) * 0x1.0p-53;
}

bool OOGraphicsAvailable() { return true; }
