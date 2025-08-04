#ifndef GAME_H
#define GAME_H
#include <libmem/libmem.hpp>
#include <string>
#include <vector>

#include "common.h"
#include "logger.h"

namespace leprac {
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

  GameId                         gameId_{};
  std::optional<libmem::Process> gameProcess_;
};
}  // namespace leprac
#endif
