/*	tests/component-cpp/component_census.cpp
        Every ship definition and every equipment item in the core data is
   tried, and each gets exactly one outcome
   (specs/011-headless-component-tests/contracts/census.md).

        The definitions come from the data at run time, through the game's own
   loaders: every top-level key of the merged shipdata.plist and every entry
   of equipment.plist. No name is written here. A ship is spawned by key near
   the player in an empty system, run for 10 s of game time, checked and
   removed; an equipment item is awarded to the player's ship, activated if
   it has a script, run for 5 s and removed. Each try is reseeded from the
   base seed and the key, so no result depends on the order; the player's
   energy is made full before it, and a try after which the player is not in
   flight ends the child like an unclean universe.

        The whole run is a parent that starts this program again as a child;
   the child writes each index and key to a progress file before trying it,
   so a definition that crashes the child is recorded as failed and the next
   child starts after it. census-expectations.tsv lists the definitions
   expected to fail or to leave, each with a reason, checked both ways.

            component_census                    the whole census
            component_census --list             the definitions and classes
            component_census --only ship:adder  one try, in this process
   Run: tests/run.sh component --cpp -k census
*/

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "OOEquipmentType.h"
#include "OOPListParsing.h"
#include "OOShipRegistry.h"
#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "ShipEntity.h"
#include "Universe.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

namespace fs = std::filesystem;
using oo::component::Game;
using oo::component::GameTime;

namespace {

constexpr std::uint32_t kDefaultSeed = 20260910;
constexpr double kShipSpanSeconds = 10.0;       // research R8
constexpr double kEquipmentSpanSeconds = 5.0;   // research R8
// From the player, away from the planet, beyond the player's scanner range
// (25.6 km), so that a hostile definition does not take the player on and
// nothing spawns inside the planet.
constexpr double kSpawnDistanceMetres = 40'000.0;
constexpr int kMaxRestarts = 50;
constexpr int kExitUnclean = 3;  // a child stopped: the universe was left unclean
constexpr std::size_t kTailLines = 12;

// --- arguments ----------------------------------------------------------------

struct Options {
  bool child = false;
  bool list = false;
  std::optional<std::string> only;  // "<kind>:<key>"
  std::uint32_t seed = kDefaultSeed;
  std::size_t from = 0;
  std::string progress;
  std::string report;
  std::string expectations;
  std::string overlay;
  bool bad = false;
};

Options ParseOptions() {
  Options o;
  const std::vector<std::string>& args = oo::component::programArguments();
  for (std::size_t i = 0; i < args.size(); ++i) {
    const std::string& a = args[i];
    const bool hasValue = i + 1 < args.size();
    if (a == "--child") {
      o.child = true;
    } else if (a == "--list") {
      o.list = true;
    } else if (a == "--only" && hasValue) {
      o.only = args[++i];
    } else if (a == "--seed" && hasValue) {
      o.seed = static_cast<std::uint32_t>(std::stoul(args[++i]));
    } else if (a == "--from" && hasValue) {
      o.from = static_cast<std::size_t>(std::stoul(args[++i]));
    } else if (a == "--progress" && hasValue) {
      o.progress = args[++i];
    } else if (a == "--report" && hasValue) {
      o.report = args[++i];
    } else if (a == "--expectations" && hasValue) {
      o.expectations = args[++i];
    } else if (a == "--data-overlay" && hasValue) {
      // Absolute: the children run in the application directory.
      o.overlay = fs::absolute(args[++i]).string();
    } else {
      std::fprintf(stderr, "component_census: unknown or incomplete argument %s\n",
                   a.c_str());
      o.bad = true;
    }
  }
  return o;
}

// --- definitions and rows -----------------------------------------------------

struct Definition {
  std::string kind;  // "ship" or "equipment"
  std::string key;
  std::string cls;   // template, part, service, candidate
  std::vector<std::string> parts;  // a ship's sub-entity ship keys
  std::size_t attachments = 0;     // everything its definition attaches
};

struct Row {
  std::string kind, key, cls, outcome;  // outcome: passed, left, failed, not-applicable
  std::uint32_t seed = 0;
  double gameTime = 0.0;
  std::string evidence;
};

std::string Clean(std::string text) {  // one TSV field
  for (char& c : text) {
    if (c == '\t' || c == '\n' || c == '\r') c = ' ';
  }
  return text;
}

std::string RowLine(const Row& r) {
  return std::format("{}\t{}\t{}\t{}\t{}\t{:.3f}\t{}", r.kind, r.key, r.cls, r.outcome,
                     r.seed, r.gameTime, Clean(r.evidence));
}

std::vector<std::string> Split(const std::string& line, char separator) {
  std::vector<std::string> fields;
  std::string field;
  std::istringstream in(line);
  while (std::getline(in, field, separator)) fields.push_back(field);
  if (!line.empty() && line.back() == separator) fields.emplace_back();
  return fields;
}

std::optional<Row> ParseRow(const std::string& line) {
  const std::vector<std::string> f = Split(line, '\t');
  if (f.size() < 6) return std::nullopt;
  Row r{f[0], f[1], f[2], f[3], static_cast<std::uint32_t>(std::stoul(f[4])),
        std::stod(f[5]), f.size() > 6 ? f[6] : ""};
  return r;
}

// The seed a definition is tried with: the base seed and the key (FNV-1a).
std::uint32_t SeedFor(std::uint32_t base, const std::string& kind, const std::string& key) {
  std::uint32_t h = 2166136261U ^ base;
  for (char c : kind + ":" + key) {
    h ^= static_cast<unsigned char>(c);
    h *= 16777619U;
  }
  return h;
}

// The overlay's shipdata.plist (a test fixture, not expansion content).
oo::PList OverlayShips(const std::string& overlay) {
  if (overlay.empty()) return oo::PList();
  return OOPropertyListFromFile((fs::path(overlay) / "shipdata.plist").string());
}

// The ship keys of the merged core data plus the overlay's, sorted.
std::vector<Definition> ShipDefinitions(const std::string& overlay) {
  const oo::PList core =
      ResourceManager::dictionaryFromFilesNamed("shipdata.plist", "Config", true);
  const oo::PList extra = OverlayShips(overlay);
  std::map<std::string, oo::PList> raw;
  for (const oo::PList* data : {&core, &extra}) {
    if (const oo::PList::Dict* d = data->getIf<oo::PList::Dict>()) {
      for (const auto& [key, value] : *d) raw[key] = value;
    }
  }
  OOShipRegistry* registry = OOShipRegistry::sharedRegistry();
  std::vector<Definition> ships;
  for (const auto& [key, value] : raw) {
    Definition d{"ship", key, "candidate", {}, 0};
    const oo::PList info = registry->shipInfoForKey(key);
    // Attached at spawn: each sub-entity (ship or flasher) and each exhaust.
    for (const char* attached : {"subentities", "exhaust"}) {
      if (const oo::PList* list = info.find(attached)) d.attachments += list->count();
    }
    if (const oo::PList* subs = info.find("subentities")) {
      for (std::size_t i = 0; i < subs->count(); ++i) {
        const oo::PList* sub = subs->at(i);
        const oo::PList* subKey = sub != nullptr ? sub->find("subentity_key") : nullptr;
        if (subKey != nullptr && subKey->isString()) {
          d.parts.push_back(*subKey->getIf<std::string>());
        }
      }
    }
    if (value.get<bool>("is_template", false)) {
      d.cls = "template";
    } else if (info.get<bool>("_oo_is_subentity", false) &&
               info.get<std::string>("roles", "").empty()) {
      d.cls = "part";
    }
    ships.push_back(std::move(d));
  }
  return ships;
}

// --- trying -------------------------------------------------------------------

std::string JsQuote(const std::string& text) {
  std::string q = "\"";
  for (char c : text) {
    if (c == '\\' || c == '"') q += '\\';
    q += c;
  }
  return q + "\"";
}

// Whether the player's ship reports the item fitted: among its equipment or
// its missiles and mines (the ship's own queries), or on a weapon mount.
bool Fitted(Game& game, const std::string& key) {
  PlayerEntity* player = game.player();
  if (player->hasOneEquipmentItemIncludingMissiles(key, true, false)) return true;
  if (player->countEquipmentItem(key) > 0) return true;
  return game.evalBool(std::format(
      "(function(){{ var s = player.ship, k = {0};"
      " return !!(s.forwardWeapon && s.forwardWeapon.equipmentKey == k); }})()",
      JsQuote(key)));
}

// What the item's own data says it needs before it can be awarded, done:
// fuel not full (or full), the items it requires. Answers why a need cannot be
// met, or "".
std::string Prepare(Game& game, OOEquipmentType* type) {
  if (type == nullptr) return "not an equipment type";
  if (!type->isAvailableToPlayer()) {
    return "not available to the player, and the census fits equipment to the player's ship";
  }
  if (type->requiresNonFullFuel()) game.eval("player.ship.fuel = 0");
  if (type->requiresFullFuel()) game.eval("player.ship.fuel = 7");
  if (std::optional<std::vector<std::string>> all = type->requiresEquipment()) {
    for (const std::string& k : *all) {
      game.eval(std::format("player.ship.awardEquipment({})", JsQuote(k)));
    }
  }
  if (std::optional<std::vector<std::string>> any = type->requiresAnyEquipment()) {
    if (!any->empty()) {
      game.eval(std::format("player.ship.awardEquipment({})", JsQuote(any->front())));
    }
  }
  return "";
}

// Awards the item as the game's scripts do: a primary weapon onto the forward
// mount, anything else through awardEquipment.
bool Award(Game& game, OOEquipmentType* type, const std::string& key) {
  if (type != nullptr && type->isPrimaryWeapon()) {
    game.eval(std::format("player.ship.forwardWeapon = {}", JsQuote(key)));
    return game.evalBool(std::format(
        "player.ship.forwardWeapon && player.ship.forwardWeapon.equipmentKey == {}",
        JsQuote(key)));
  }
  return game.evalBool(std::format("player.ship.awardEquipment({})", JsQuote(key)));
}

std::string FirstError(const std::vector<oo::component::Event>& errors) {
  return errors.empty() ? std::string() : errors.front().text;
}

// One equipment item, on the player's ship. Answers its row; cls is set to
// "service" when the item is applied and gone (no fitted state after award).
Row TryEquipment(Game& game, const Definition& d, std::uint32_t baseSeed) {
  Row r{d.kind, d.key, d.cls, "passed", SeedFor(baseSeed, d.kind, d.key), 0.0, ""};
  game.reseed(r.seed);
  const std::size_t mark = game.events().size();
  const double started = game.time();
  const std::string forward = game.evalString(
      "player.ship.forwardWeapon ? player.ship.forwardWeapon.equipmentKey : 'EQ_WEAPON_NONE'");

  std::shared_ptr<OOEquipmentType> type = OOEquipmentType::equipmentTypeWithIdentifier(d.key);
  // The standard save's ship already carries some items, and an item cannot
  // be awarded twice: take it off for the try and put it back afterwards.
  const bool hadIt = Fitted(game, d.key) && !(type != nullptr && type->isPrimaryWeapon());
  if (hadIt) game.eval(std::format("player.ship.removeEquipment({})", JsQuote(d.key)));
  const std::string unmet = Prepare(game, type.get());
  const bool awarded = unmet.empty() && Award(game, type.get(), d.key);
  if (!unmet.empty()) {
    r.outcome = "not-applicable";
    r.evidence = unmet;
  } else if (!awarded) {
    r.outcome = "failed";
    r.evidence = "award refused";
  } else if (!Fitted(game, d.key)) {
    r.cls = "service";
    r.outcome = "not-applicable";
    r.evidence = "service: no fitted state after award";
  } else {
    if (type != nullptr && type->scriptName().has_value()) {
      PlayerEntity* player = game.player();
      const std::size_t index = player->eqScriptIndexForKey(d.key);
      if (index < player->primedEquipmentCount()) {
        player->activatePrimableEquipment(index, oo::PrimedEquipmentMode::kActivated);
      }
    }
    game.simulate(GameTime{kEquipmentSpanSeconds});
    game.eval(std::format("player.ship.removeEquipment({})", JsQuote(d.key)));
    if (type != nullptr && type->isPrimaryWeapon()) {
      game.eval(std::format("player.ship.forwardWeapon = {}", JsQuote(forward)));
    }
    if (Fitted(game, d.key) && !(type != nullptr && type->isPrimaryWeapon() &&
                                 forward == d.key)) {
      r.outcome = "failed";
      r.evidence = "still fitted after removal";
    }
  }
  const std::vector<oo::component::Event> errors = game.errorsSince(mark);
  if (r.outcome == "passed" && !errors.empty()) {
    r.outcome = "failed";
    r.evidence = FirstError(errors);
  }
  if (hadIt) game.eval(std::format("player.ship.awardEquipment({})", JsQuote(d.key)));
  r.gameTime = game.time() - started;
  return r;
}

// One ship definition, spawned by key 10 km ahead of the player. Answers its
// row and whether the universe is clean afterwards.
Row TryShip(Game& game, const Definition& d, std::uint32_t baseSeed, bool* clean) {
  Row r{d.kind, d.key, d.cls, "passed", SeedFor(baseSeed, d.kind, d.key), 0.0, ""};
  *clean = true;
  game.reseed(r.seed);
  const std::size_t countBefore = game.entityCount();
  std::set<const Entity*> before;
  for (const oo::EntityHold& e : game.universe()->entityList()) before.insert(e.get());
  const std::size_t mark = game.events().size();
  const double started = game.time();

  PlayerEntity* player = game.player();
  HPVector away = vectorToHPVector(player->forwardVector());
  if (Entity* planet = game.universe()->planet()) {
    const HPVector d = HPvector_subtract(player->getPosition(), planet->getPosition());
    if (HPmagnitude(d) > 0.0) away = HPvector_normal(d);
  }
  const HPVector at = HPvector_add(player->getPosition(),
                                   HPvector_multiply_scalar(away, kSpawnDistanceMetres));
  std::shared_ptr<ShipEntity> ship = game.spawnKey(d.key, at);
  if (ship == nullptr) {
    r.outcome = "failed";
    r.evidence = "spawn returned nothing";
    const std::vector<oo::component::Event> errors = game.errorsSince(mark);
    if (!errors.empty()) r.evidence += ": " + FirstError(errors);
    return r;
  }
  oo::component::ShipHandle handle(ship);
  ship.reset();  // the universe owns it; the census watches

  game.simulate(GameTime{kShipSpanSeconds});

  if (!handle.alive()) {
    r.outcome = "left";
    r.evidence = "it left the universe within the span";
  } else {
    ShipEntity* live = handle;
    if (live->subEntityCount() != d.attachments) {
      r.outcome = "failed";
      r.evidence = std::format("{} attached parts, {} declared", live->subEntityCount(),
                               d.attachments);
    }
  }
  const std::vector<oo::component::Event> errors = game.errorsSince(mark);
  if (r.outcome == "passed" && !errors.empty()) {
    r.outcome = "failed";
    r.evidence = FirstError(errors);
  }

  // Take out the ship and every ship that came with it or from it (escorts,
  // launched ships), then the universe must be as it was.
  std::vector<ShipEntity*> brought;
  for (const oo::EntityHold& e : game.universe()->entityList()) {
    if (before.contains(e.get())) continue;
    if (ShipEntity* s = dynamic_cast<ShipEntity*>(e.get())) {
      if (!s->getIsPlayer() && !s->getIsSubEntity()) brought.push_back(s);
    }
  }
  std::vector<oo::component::ShipHandle> leaving;
  for (ShipEntity* s : brought) leaving.emplace_back(oo::share(s));
  for (const oo::component::ShipHandle& s : leaving) game.remove(s);
  game.step();
  if (game.entityCount() != countBefore) {
    *clean = false;
    r.outcome = "failed";
    r.evidence = std::format("{} entities after removal, {} before", game.entityCount(),
                             countBefore);
  }
  r.gameTime = game.time() - started;
  return r;
}

// Every definition, classified. Equipment is classified by behaviour, so the
// list needs the game and a trial award of each item.
std::vector<Definition> Definitions(Game& game, const Options& o) {
  std::vector<Definition> all = ShipDefinitions(o.overlay);
  const oo::PList equipment =
      ResourceManager::arrayFromFilesNamed("equipment.plist", "Config", true);
  std::vector<Definition> items;
  for (std::size_t i = 0; i < equipment.count(); ++i) {
    const oo::PList* entry = equipment.at(i);
    const oo::PList* key = entry != nullptr ? entry->at(3) : nullptr;
    if (key == nullptr || !key->isString()) continue;
    items.push_back(Definition{"equipment", *key->getIf<std::string>(), "candidate", {}, 0});
  }
  std::sort(items.begin(), items.end(),
            [](const Definition& a, const Definition& b) { return a.key < b.key; });
  for (Definition& d : items) {
    // Applied and gone: awarded (its needs met first), and then not fitted.
    std::shared_ptr<OOEquipmentType> type = OOEquipmentType::equipmentTypeWithIdentifier(d.key);
    const std::string forward = game.evalString(
        "player.ship.forwardWeapon ? player.ship.forwardWeapon.equipmentKey : 'EQ_WEAPON_NONE'");
    const bool hadIt = Fitted(game, d.key) && !(type != nullptr && type->isPrimaryWeapon());
    if (hadIt) game.eval(std::format("player.ship.removeEquipment({})", JsQuote(d.key)));
    const bool awarded = Prepare(game, type.get()).empty() && Award(game, type.get(), d.key);
    if (awarded && !Fitted(game, d.key)) d.cls = "service";
    if (awarded) game.eval(std::format("player.ship.removeEquipment({})", JsQuote(d.key)));
    if (hadIt) game.eval(std::format("player.ship.awardEquipment({})", JsQuote(d.key)));
    game.eval(std::format("player.ship.forwardWeapon = {}", JsQuote(forward)));
  }
  all.insert(all.end(), items.begin(), items.end());
  return all;
}

void ApplyOverlay(const std::string& overlay) {
  const oo::PList extra = OverlayShips(overlay);
  if (const oo::PList::Dict* d = extra.getIf<oo::PList::Dict>()) {
    for (const auto& [key, value] : *d) {
      OOShipRegistry::sharedRegistry()->setShipInfoForKey(key, value);
    }
  }
}

Row Try(Game& game, const Definition& d, std::uint32_t seed, bool* clean) {
  *clean = true;
  if (d.cls == "template" || d.cls == "part") {
    // A part's outcome is decided by its parents, in the parent process.
    return Row{d.kind, d.key, d.cls, "not-applicable", SeedFor(seed, d.kind, d.key), 0.0,
               d.cls};
  }
  PlayerEntity* player = game.player();
  // Every try starts from the same player: stopped (a census is about 2,000 s
  // of game time, and a player still flying at launch speed would reach the
  // planet), and at full energy (the script API's maxEnergy: the player's
  // own field is not its maximum).
  player->setSpeed(0.0);
  game.eval("player.ship.energy = player.ship.maxEnergy");
  Row r = d.kind == "ship" ? TryShip(game, d, seed, clean) : TryEquipment(game, d, seed);
  if (player->status() != oo::EntityStatus::kInFlight) {
    *clean = false;  // the next try would not start where this one did
    r.outcome = "failed";
    r.evidence = std::format("the player was {} after it",
                             oo::StringFromEntityStatus(player->status()));
  }
  return r;
}

// --- the parent ---------------------------------------------------------------

struct Expectation {
  std::string expect, reason;
};

std::vector<std::string> ReadLines(const std::string& path) {
  std::vector<std::string> lines;
  std::ifstream in(path);
  for (std::string line; std::getline(in, line);) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    lines.push_back(line);
  }
  return lines;
}

std::string Tail(const std::string& path) {
  const std::vector<std::string> lines = ReadLines(path);
  std::string tail;
  const std::size_t from = lines.size() > kTailLines ? lines.size() - kTailLines : 0;
  for (std::size_t i = from; i < lines.size(); ++i) tail += lines[i] + " | ";
  return tail;
}

int RunParent(const Options& o) {
  const fs::path scratch = Game::buildRoot() / "component";
  fs::create_directories(scratch);
  const std::string self = oo::component::programPath();
  std::vector<std::string> common = {"--seed", std::to_string(o.seed)};
  if (!o.overlay.empty()) {
    common.push_back("--data-overlay");
    common.push_back(o.overlay);
  }

  // The definitions, from a child's --list.
  const std::string listFile = (scratch / "census-list.tsv").string();
  std::vector<std::string> listArgs = common;
  listArgs.push_back("--list");
  if (oo::component::runChild(self, listArgs, listFile, "") != 0) {
    std::printf("component_census: --list failed\n");
    return 2;
  }
  std::vector<Definition> defs;
  for (const std::string& line : ReadLines(listFile)) {
    const std::vector<std::string> f = Split(line, '\t');
    if (f.size() < 3 || (f[0] != "ship" && f[0] != "equipment")) continue;
    Definition d{f[0], f[1], f[2], {}, 0};
    if (f.size() > 3 && !f[3].empty()) d.parts = Split(f[3], ',');
    if (f.size() > 4 && !f[4].empty()) d.attachments = std::stoul(f[4]);
    defs.push_back(std::move(d));
  }
  if (defs.empty()) {
    std::printf("component_census: no definitions found\n");
    return 2;
  }

  // The children, restarted after each one that dies.
  const std::string progress = (scratch / "census-progress.txt").string();
  const std::string rowsFile = progress + ".rows";
  const std::string childLog = (scratch / "census-child.log").string();
  fs::remove(rowsFile);
  std::map<std::pair<std::string, std::string>, Row> rows;
  std::size_t from = 0;
  int restarts = 0;
  while (from < defs.size()) {
    fs::remove(progress);
    std::vector<std::string> args = common;
    for (const char* a : {"--child", "--from"}) args.emplace_back(a);
    args.push_back(std::to_string(from));
    args.push_back("--progress");
    args.push_back(progress);
    const int status = oo::component::runChild(self, args, childLog, childLog + ".err");
    for (const std::string& line : ReadLines(rowsFile)) {
      if (std::optional<Row> r = ParseRow(line)) rows[{r->kind, r->key}] = *r;
    }
    if (status == 0) break;
    // The definition the child was on: recorded failed (unless the child
    // recorded it, as it does when it stops for an unclean universe).
    const std::vector<std::string> p = ReadLines(progress);
    const std::vector<std::string> f = p.empty() ? std::vector<std::string>() : Split(p.front(), '\t');
    if (f.size() < 2) {
      std::printf("component_census: a child exited %d before trying anything\n", status);
      return 2;
    }
    const std::size_t index = std::stoul(f[0]);
    const Definition& d = defs[index];
    if (!rows.contains({d.kind, d.key})) {
      rows[{d.kind, d.key}] = Row{d.kind, d.key, d.cls, "failed", SeedFor(o.seed, d.kind, d.key),
                                  0.0, std::format("child exited {}: {}{}", status,
                                                   Tail(childLog), Tail(childLog + ".err"))};
    }
    from = index + 1;
    if (++restarts > kMaxRestarts) {
      std::printf("component_census: more than %d restarts\n", kMaxRestarts);
      return 2;
    }
  }

  // Parts: not-applicable if a ship that carries one passed.
  for (const Definition& d : defs) {
    if (d.cls != "part") continue;
    bool parentPassed = false;
    for (const Definition& parent : defs) {
      if (std::find(parent.parts.begin(), parent.parts.end(), d.key) == parent.parts.end())
        continue;
      auto it = rows.find({parent.kind, parent.key});
      if (it != rows.end() && it->second.outcome == "passed") parentPassed = true;
    }
    Row& r = rows[{d.kind, d.key}];
    r = Row{d.kind, d.key, d.cls, parentPassed ? "not-applicable" : "failed",
            SeedFor(o.seed, d.kind, d.key), 0.0, parentPassed ? "part" : "no passing parent"};
  }

  // The expectations, checked both ways.
  const std::string expectationsPath =
      !o.expectations.empty()
          ? o.expectations
          : (Game::sourceRoot() / "tests" / "component-cpp" / "census-expectations.tsv").string();
  std::map<std::pair<std::string, std::string>, Expectation> expected;
  bool unsound = false;
  for (const std::string& line : ReadLines(expectationsPath)) {
    if (line.empty() || line[0] == '#') continue;
    const std::vector<std::string> f = Split(line, '\t');
    if (f.size() >= 1 && f[0] == "kind") continue;  // the header
    if (f.size() < 4 || f[3].find_first_not_of(' ') == std::string::npos) {
      std::printf("component_census: %s: a row with no reason: %s\n", expectationsPath.c_str(),
                  line.c_str());
      unsound = true;
      continue;
    }
    if (!rows.contains({f[0], f[1]})) {
      std::printf("component_census: %s: %s %s is not in the data\n", expectationsPath.c_str(),
                  f[0].c_str(), f[1].c_str());
      unsound = true;
      continue;
    }
    expected[{f[0], f[1]}] = Expectation{f[2], f[3]};
  }

  int passed = 0, left = 0, na = 0, failed = 0, matched = 0;
  std::vector<std::string> unexpected;
  for (auto& [id, r] : rows) {
    auto e = expected.find(id);
    if (e != expected.end()) {
      if (e->second.expect == "not-applicable" && r.outcome != "passed") {
        r.outcome = "not-applicable";
        r.evidence = e->second.reason;
      }
      if (r.outcome == e->second.expect) {
        ++matched;
      } else {
        unexpected.push_back(std::format("{} {}: listed as {}, was {} ({})", r.kind, r.key,
                                         e->second.expect, r.outcome, r.evidence));
      }
    } else if (r.outcome == "left") {
      r.outcome = "failed";  // leaving is an outcome only where it is expected
    }
    if (e == expected.end() && r.outcome == "failed") {
      unexpected.push_back(std::format("{} {}: failed: {}", r.kind, r.key, r.evidence));
    }
    if (r.outcome == "passed") ++passed;
    if (r.outcome == "left") ++left;
    if (r.outcome == "not-applicable") ++na;
    if (r.outcome == "failed") ++failed;
  }

  // The report, sorted by kind and key (the map's order).
  const std::string reportPath =
      !o.report.empty() ? o.report : (scratch / "census-report.tsv").string();
  {
    std::ofstream out(reportPath, std::ios::out | std::ios::trunc);
    out << "kind\tkey\tclass\toutcome\tseed\tgameTime\tevidence\n";
    for (const auto& [id, r] : rows) out << RowLine(r) << '\n';
  }
  for (const std::string& line : unexpected) std::printf("unexpected: %s\n", line.c_str());

  std::size_t ships = 0, equipment = 0;
  for (const Definition& d : defs) (d.kind == "ship" ? ships : equipment)++;
  std::printf("component_census: %zu ships, %zu equipment: %d passed, %d left, %d not-applicable, "
              "%d failed, %d expected\n",
              ships, equipment, passed, left, na, failed, matched);
  std::printf("component_census: report at %s\n", reportPath.c_str());
  if (rows.size() != defs.size()) {
    std::printf("component_census: %zu outcomes for %zu definitions\n", rows.size(), defs.size());
    return 2;
  }
  if (unsound) return 2;
  return unexpected.empty() ? 0 : 1;
}

// --- the child modes ----------------------------------------------------------

int RunInGame(const Options& o) {
  Game game = Game::scenario(o.seed);
  ApplyOverlay(o.overlay);
  const std::vector<Definition> defs = Definitions(game, o);

  if (o.list) {
    std::size_t ships = 0, equipment = 0;
    for (const Definition& d : defs) {
      std::string parts;
      for (const std::string& p : d.parts) parts += (parts.empty() ? "" : ",") + p;
      std::printf("%s\t%s\t%s\t%s\t%zu\n", d.kind.c_str(), d.key.c_str(), d.cls.c_str(),
                  parts.c_str(), d.attachments);
      (d.kind == "ship" ? ships : equipment)++;
    }
    std::printf("%zu ships, %zu equipment\n", ships, equipment);
    return 0;
  }

  if (o.only.has_value()) {
    const std::size_t colon = o.only->find(':');
    const std::string kind = colon == std::string::npos ? "" : o.only->substr(0, colon);
    const std::string key = colon == std::string::npos ? "" : o.only->substr(colon + 1);
    for (const Definition& d : defs) {
      if (d.kind != kind || d.key != key) continue;
      bool clean = true;
      const Row r = Try(game, d, o.seed, &clean);
      std::printf("%s\n", RowLine(r).c_str());
      return r.outcome == "failed" ? 1 : 0;
    }
    std::printf("component_census: no definition %s\n", o.only->c_str());
    return 2;
  }

  // --child: from the start index, each definition in turn.
  std::ofstream rows(o.progress + ".rows", std::ios::out | std::ios::app);
  for (std::size_t i = o.from; i < defs.size(); ++i) {
    {
      std::ofstream progress(o.progress, std::ios::out | std::ios::trunc);
      progress << i << '\t' << defs[i].kind << ':' << defs[i].key << '\n';
    }
    bool clean = true;
    const Row r = Try(game, defs[i], o.seed, &clean);
    rows << RowLine(r) << '\n';
    rows.flush();
    if (!clean) return kExitUnclean;
  }
  return 0;
}

}  // namespace

OO_TEST(census) {
  const Options o = ParseOptions();
  int code = 0;
  if (o.bad) {
    code = 2;
  } else if (o.child || o.list || o.only.has_value()) {
    code = RunInGame(o);
  } else {
    code = RunParent(o);
  }
  // The census's own output and exit codes, not oo_test's summary.
  oo::component::exitProgram(code);
}

OO_COMPONENT_MAIN()
