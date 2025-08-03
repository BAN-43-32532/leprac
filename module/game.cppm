module;

#include <libmem/libmem.hpp>
#include <string>
#include <vector>

export module leprac.game;

import leprac.logger;
import leprac.common;

export namespace leprac {
// struct GameInfo {
//   GameId          gameId_;
//   libmem::Process gameProcess_;
//   libmem::Thread  main_thread;
// };

class Game {
 public:
  void init();

  [[nodiscard]] auto getGameId() const;
  [[nodiscard]] auto getProcess() const;

  static std::vector<GameId> detectRunningGame();

 private:
  bool                           completeGameInfo();
  std::optional<std::wstring>    getGameTitle(libmem::Process const &process);
  std::optional<libmem::Address> getStackAddress(
    libmem::Process const      &process,
    libmem::Address             address,
    std::vector<uint8_t> const &pattern
  );

  std::vector<uint8_t> getStackSignature(libmem::Process const &process);

  GameId          gameId_{};
  std::optional<libmem::Process> gameProcess_;
};
}  // namespace leprac
