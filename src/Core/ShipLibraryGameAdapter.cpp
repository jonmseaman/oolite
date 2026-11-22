#include "ShipLibraryGameAdapter.h"

#include "EntityEnums.h"  // EntityStatus
#include "PlayerEntity.h"
#include "Universe.h"

namespace oo {

ShipLibraryGameAdapter::ShipLibraryGameAdapter(Universe& universe)
    : universe_(universe) {}

void ShipLibraryGameAdapter::ShowShip(std::size_t category, std::size_t index) {
  universe_.showDemoShip(category, index);
}

void ShipLibraryGameAdapter::Exit() { exit_requested_ = true; }

void ShipLibraryGameAdapter::RunPendingExit() {
  if (!exit_requested_) return;
  exit_requested_ = false;
  PlayerEntity* player = PLAYER;
  if (player == nullptr) return;
  if (player->status() == EntityStatus::kDocked) {
    player->setGuiToInterfacesScreen(0);
  } else {
    player->setGuiToIntroFirstGo(true);
  }
}

}  // namespace oo
