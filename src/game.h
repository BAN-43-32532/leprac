#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "libmem/libmem.hpp"
#include "logger.h"
#include "util.h"

namespace leprac {
enum class GameId {
  Le01,
  Le02,
  Le03,
  Le04,
  Uso,  // 束方 嘘時空　～ Fools_rush_in
  COUNT,
};

struct GameInfo {
  GameId          id = GameId::COUNT;
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
