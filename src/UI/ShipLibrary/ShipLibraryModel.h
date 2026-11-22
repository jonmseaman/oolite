// The ship library's data: the categories of demo ships and the sheet of the
// ship on show. Plain values, filled by the game (ShipLibraryGameAdapter) and
// read by the view-model's views.

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace oo::ui {

// One ship in a category: its shipdata key and its display name (absent where
// the library entry has none).
struct ShipLibraryEntry {
  std::string key;
  std::optional<std::string> name;
};

// One category of the library ("ship", "station", ...), its plural display
// name and its ships, in library order.
struct ShipLibraryCategory {
  std::string id;
  std::string plural_name;
  std::vector<ShipLibraryEntry> entries;
};

// What the library shows about the ship on show. An absent optional is a field
// the classic screen leaves off its row.
struct ShipSheet {
  std::string category_singular;
  std::optional<std::string> class_name;
  std::optional<std::string> summary;

  // ship_data: the three rows of figures are shown.
  bool has_stats = false;
  std::string speed;
  std::string turn_rate;
  std::string cargo;
  std::string generator;
  std::string shields;
  std::string witchspace;
  std::string weapons;
  std::string turrets;
  std::string size;

  // The entry has a description (its expansion may still be absent).
  bool has_description = false;
  std::optional<std::string> description;
};

struct ShipLibraryModel {
  std::vector<ShipLibraryCategory> categories;
  std::size_t category = 0;  // the category on show
  std::size_t ship = 0;      // the ship on show, within its category
  ShipSheet sheet;
};

}  // namespace oo::ui
