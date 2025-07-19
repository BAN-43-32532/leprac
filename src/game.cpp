#include "game.h"

#include <libmem/libmem.hpp>

namespace leprac {

void Game::getCurrentlyRunningGame() {
  for (auto game: {"Le01.exe", "Le02.exe", "Le03.exe", "Le04.exe"}) {
    auto process = libmem::FindProcess(game);
    if (process) {

    }
  }
}
}  // namespace leprac
