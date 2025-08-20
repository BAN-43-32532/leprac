#ifndef GAME_HPP
#define GAME_HPP
#include <libmem/libmem.hpp>
#include <string>
#include <vector>

#include "common.hpp"

namespace leprac {
class Game {
 public:
  Game(GameID);

  [[nodiscard]] auto getID() const;
  [[nodiscard]] auto process() const;

  static std::vector<GameID> detectRunningGames();

  // Replace the first '_' with '.'
  static std::string versionToLiteral(std::string version);
  // Replace the first '.' with '_'
  static std::string versionToTag(std::string version);

  // std::nullopt, invalid; true, supported; false, not supported.
  static std::optional<bool> isVersionSupported(GameID id, std::string const &version);

  static std::string           loadBinary(std::string const &path);
  // If this exe seems not a len'en game (no ID matches), return std::nullopt.
  static std::optional<GameID> searchIDFromEXE(std::string const &path);
  static std::string           searchVersionFromEXE(GameID id, std::string const &path);

  static std::vector<fs::path> scanGameEXE(fs::path const &path);

 private:
  bool                        completeGameInfo();
  std::optional<std::wstring> getGameTitle(libmem::Process const &process);
  std::optional<libmem::Address>
  getStackAddress(libmem::Process const &process, libmem::Address address, std::vector<uint8_t> const &pattern);

  std::vector<uint8_t> getStackSignature(libmem::Process const &process);

  GameID gameId_{};
  // libmem::Process gameProcess_;
};
}  // namespace leprac
#endif  // GAME_HPP
