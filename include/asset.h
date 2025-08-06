#ifndef ASSET_H
#define ASSET_H
#include <toml.hpp>

#include "game.h"

namespace leprac {
class Asset {
 public:
  Asset() = delete;
  static void init();
  static void load();

  [[nodiscard]] static auto const& address() { return address_; }
  [[nodiscard]] static auto const& literal() { return literal_; }
  [[nodiscard]] static auto const& version() { return version_; }
  [[nodiscard]] static auto const& frame(Game::ID const gameId) {
    return frame_[gameId];
  }
  [[nodiscard]] static auto const& pattern(Game::ID const gameId) {
    return pattern_[gameId];
  }

 private:
  static inline toml::value address_;
  static inline toml::value literal_;
  static inline toml::value version_;

  static inline std::unordered_map<Game::ID, toml::value> frame_;
  static inline std::unordered_map<Game::ID, toml::value> pattern_;
};
}  // namespace leprac
#endif
