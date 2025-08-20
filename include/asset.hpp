#ifndef ASSET_HPP
#define ASSET_HPP
#include <toml.hpp>

#include "common.hpp"

namespace leprac {
class Asset {
 public:
  // No need to parse comments when reading config
  struct wo_comment_config: toml::type_config {
    using comment_type = toml::discard_comments;
  };

  Asset() = delete;
  static void init();
  static void deinit();
  static void load();

  [[nodiscard]] static auto const& address() { return address_; }

  [[nodiscard]] static auto const& literal() { return literal_; }

  [[nodiscard]] static auto const& version() { return version_; }

  [[nodiscard]] static auto const& link() { return link_; }

  [[nodiscard]] static auto const& frame(GameID const gameId) { return frame_[gameId]; }

  [[nodiscard]] static auto const& pattern(GameID const gameId) { return pattern_[gameId]; }

 private:
  static inline toml::basic_value<wo_comment_config> address_;
  static inline toml::basic_value<wo_comment_config> literal_;
  static inline toml::basic_value<wo_comment_config> version_;
  static inline toml::basic_value<wo_comment_config> link_;

  static inline std::unordered_map<GameID, toml::value> frame_;
  static inline std::unordered_map<GameID, toml::value> pattern_;
};
}  // namespace leprac
#endif  // ASSET_HPP
