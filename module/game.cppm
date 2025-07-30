module;

#include <libmem/libmem.hpp>
#include <string>
#include <vector>

export module leprac.game;

import leprac.logger;
import leprac.common;

export namespace leprac {
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
