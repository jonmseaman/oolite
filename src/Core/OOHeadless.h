/*

OOHeadless.h

How a game is started, and what a game with no window owns instead of its
devices: the options a test passes, the clock only the game's own step
advances, the observer told of script events, the record of what the game
would have shown and played, and the generator a headless game's Math.random
draws from.

A default-constructed OOHeadlessOptions is the windowed game exactly as it
was. The header includes nothing from the game, so anything may include it.


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

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ShipEntity;

// How a game is started. A default-constructed value is the windowed game,
// exactly as today.
struct OOHeadlessOptions {
  enum class AddOns : std::uint8_t { kAll, kNone };

  bool headless = false;              // no window, no GL, no sound device
  std::optional<std::uint32_t> seed;  // every generator; unset: the clock or
                                      // OO_RANDOM_SEED, as today
  AddOns addOns = AddOns::kAll;       // kNone: the built-in Resources only
  bool populate = true;               // fill the system with traffic
  bool startInFlight = false;    // the player in flight in the empty system
  bool inputView = false;        // a view with no window, for key events
  double step = 1.0 / 60.0;      // the headless stepper's fixed dt, s
  double scriptTimeLimit = 0.0;  // the script limiter's limit, s; 0: the
                                 // engine's own

  // Raises OOInvalidArgumentException, naming the rule, if the options are
  // inconsistent: a headless game needs a seed, and reads no add-ons.
  void validate() const;
};

// The game's clock: what the simulation reads as "now". The system clock
// reads the machine's; a manual clock starts at 0 and moves only when the
// game's step advances it, so a headless game never depends on how fast the
// machine is.
class OOGameClock {
 public:
  OOGameClock() = default;  // the system clock

  static OOGameClock system();
  // Monotonic seconds start at 0; the time of day starts at a value derived
  // from the seed (manualTimeOfDayStart) and advances with them.
  static OOGameClock manual(std::uint32_t seed);

  // The time of day a manual clock seeded so starts at, in [0, 86400).
  static double manualTimeOfDayStart(std::uint32_t seed);

  bool isManual() const { return _manual; }

  // Seconds from an arbitrary origin, for intervals and deadlines.
  double monotonicSeconds() const;
  // Seconds since local midnight, in [0, 86400).
  double timeOfDay() const;

  // The manual clock moves on by dt, rounded to a whole nanosecond so that a
  // run of fixed steps sums exactly; the system clock ignores it.
  void advance(double dt);

 private:
  bool _manual = false;
  std::int64_t _elapsedNanoseconds = 0;
  double _timeOfDayStart = 0.0;
};

// Told of each script event as it is dispatched, before the scripts run, on
// the main thread. It must not change game state. The windowed game has none.
class OOGameObserver {
 public:
  virtual ~OOGameObserver() = default;
  virtual void shipEvent(double time, ShipEntity* ship,
                         std::string_view name) = 0;
  virtual void worldEvent(double time, std::string_view name) = 0;
};

// What a game with no window and no sound device would have shown and played.
struct OOSoundRequest {
  double time = 0.0;  // game time, s
  std::string name;
};

struct OOPresentationRecord {
  std::uint64_t framesRequested = 0;
  std::vector<OOSoundRequest> soundsRequested;
};

// The generator a headless game's Math.random draws from: SplitMix64 over the
// seed, each draw a double in [0, 1) from the top 53 bits. Defined here so a
// test can compute the same sequence.
class OOSeededRandom {
 public:
  explicit OOSeededRandom(std::uint32_t seed) : _state(seed) {}

  double nextDouble();

 private:
  std::uint64_t _state;
};

// False in a headless game: no gl* function may be called while it is false.
bool OOGraphicsAvailable();
