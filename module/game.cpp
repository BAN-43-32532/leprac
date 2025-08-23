module;
#include <expected>
#include <filesystem>
#include <libmem/libmem.hpp>
#include <string>
#include <vector>
export module game;

import common;

export namespace leprac {
class Game {
 public:
  Game(GameID id);

  [[nodiscard]] auto gameID() const;
  [[nodiscard]] auto process() const;

  static std::vector<GameID> detectRunningGames() noexcept;

  // Replace the first '_' with '.'
  static std::string versionToLiteral(std::string version) noexcept;
  // Replace the first '.' with '_'
  static std::string versionToTag(std::string version) noexcept;

  // std::nullopt, invalid; true, supported; false, not supported.
  static std::optional<bool> isVersionSupported(GameID id, std::string const &version) noexcept;

  static std::string           loadBinary(std::string_view path, size_t size = 0) noexcept;
  // If this exe seems not a len'en game (no ID matches), return std::nullopt.
  static std::optional<GameID> searchIDFromEXE(std::string const &path) noexcept;

  enum class [[nodiscard]] ErrorSearchVersionFromEXE {
    FoundMultipleVersions,
    FoundNoVersion,
    FailLoadBinary,
  };
  static std::expected<std::string, ErrorSearchVersionFromEXE>
  searchVersionFromEXE(GameID id, std::string const &path) noexcept;

  static bool isLe01prac(std::string_view path) noexcept;

  static std::vector<fs::path> scanGameEXE(fs::path const &path) noexcept;

 private:
  bool                           completeGameInfo() noexcept;
  std::optional<std::wstring>    getGameTitle(libmem::Process const &process) noexcept;
  std::optional<libmem::Address> getStackAddress(
    libmem::Process const      &process,
    libmem::Address             address,
    std::vector<uint8_t> const &pattern
  ) noexcept;

  std::vector<uint8_t> getStackSignature(libmem::Process const &process) noexcept;

  GameID                         gameId_;
  std::optional<libmem::Process> gameProcess_;
};
}  // namespace leprac
