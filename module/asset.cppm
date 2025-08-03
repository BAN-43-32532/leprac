module;
#include <toml.hpp>
export module leprac.asset;
import leprac.common;

export namespace leprac {
class Asset {
 public:
  Asset() = delete;
  static void init();
  static void load();

  [[nodiscard]] static auto const& address() { return address_; }

  [[nodiscard]] static auto& literal() { return literal_; }

  [[nodiscard]] static auto const& version() { return version_; }

  [[nodiscard]] static auto const& frame(GameId const gameId) {
    return frame_[gameId];
  }

  [[nodiscard]] static auto const& pattern(GameId const gameId) {
    return pattern_[gameId];
  }

 private:
  static inline toml::value address_;
  static inline toml::value literal_;
  static inline toml::value version_;

  static inline std::unordered_map<GameId, toml::value> frame_;
  static inline std::unordered_map<GameId, toml::value> pattern_;
};
}  // namespace leprac
