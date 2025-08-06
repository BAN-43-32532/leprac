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
  enum class ID {
    Le01,
    Le02,
    Le03,
    Le04,
    Uso,  // JynX call it Uso, while westerners may call it FRI
  };
  void init();

  [[nodiscard]] auto getID() const;
  [[nodiscard]] auto process() const;

  static std::vector<ID> detectRunningGames();

 private:
  bool                           completeGameInfo();
  std::optional<std::wstring>    getGameTitle(libmem::Process const &process);
  std::optional<libmem::Address> getStackAddress(
    libmem::Process const      &process,
    libmem::Address             address,
    std::vector<uint8_t> const &pattern
  );

  std::vector<uint8_t> getStackSignature(libmem::Process const &process);

  ID              gameId_{};
  libmem::Process gameProcess_;
};
}  // namespace leprac
#endif
