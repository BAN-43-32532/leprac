#ifndef GAME_H
#define GAME_H
#include <libmem/libmem.hpp>
#include <string>
#include <vector>

#include "common.hpp"
#include "logger.hpp"

namespace leprac {
class Game {
 public:
  void init();

  [[nodiscard]] auto getID() const;
  [[nodiscard]] auto process() const;

  static std::vector<GameID> detectRunningGames();

 private:
  bool                           completeGameInfo();
  std::optional<std::wstring>    getGameTitle(libmem::Process const &process);
  std::optional<libmem::Address> getStackAddress(
    libmem::Process const      &process,
    libmem::Address             address,
    std::vector<uint8_t> const &pattern
  );

  std::vector<uint8_t> getStackSignature(libmem::Process const &process);

  GameID gameId_{};
  // libmem::Process gameProcess_;
};
}  // namespace leprac
#endif
