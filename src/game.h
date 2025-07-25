#ifndef GAME_H
#define GAME_H

#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum.hpp>
#include <string>
#include <vector>

#include "logger.h"
#include "type.h"
#include "util.h"

namespace leprac {

struct GameInfo {
  GameId          id;
  libmem::Process process;
  libmem::Thread  main_thread;
};

std::string toProcessName(GameId game);

class Game {
 public:
  void init();

  [[nodiscard]] auto getGameId() const { return gameInfo_.id; }

  [[nodiscard]] auto getProcess() const { return gameInfo_.process; }

  static std::vector<GameInfo> detectRunningGame();

 private:
  bool                           completeGameInfo();
  std::optional<std::wstring>    getGameTitle(libmem::Process const &process);
  std::optional<libmem::Address> getStackAddress(
    libmem::Process const      &process,
    libmem::Address             address,
    std::vector<uint8_t> const &pattern
  );

  std::vector<uint8_t> getStackSignature(libmem::Process const &process);

  GameInfo gameInfo_;
};
}  // namespace leprac

#endif  // GAME_H
