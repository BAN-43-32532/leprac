#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum_all.hpp>
#include <string>
#include <vector>

#include "game.h"
#include "literal.h"
#include "toml.hpp"

namespace leprac {
class Config {
  auto commentLanguage =
    R"(A string ("en" / "zh" / "ja") defining which language the launcher and the in-game overlay gui use)";
  auto commentPath  = R"(path)";
  auto commentFonts = R"(font)";
  auto commentDebug = R"(debug)";

 public:
  static void init() {
    auto        pathConfig = "leprac-cfg.toml";
    toml::value config;
    if (auto parseResult = toml::try_parse(pathConfig); parseResult.is_ok()) {
      config    = parseResult.unwrap();
      auto lang = toml::find_or(config, "language", "unk");
      lang_ = *magic_enum::enum_cast<Lang>(lang);
      magic_enum::enum_for_each<GameId>([config](auto val) {
        constexpr GameId gameId = val;
        pathGame_[gameId]       = toml::find_or(
          config, std::format("path{}", magic_enum::enum_name(gameId)), ""
        );
      });
      pathFonts_ =
        toml::find_or(config, "pathFonts", std::vector<std::string>{});
      debug_ = toml::find_or(config, "debug", false);
    } else {
    }

    std::ofstream fileConfig(pathConfig);
  }

  // Getters
  static auto const& lang() { return lang_; }

  static void setLanguage(Lang lang) {
    lang_ = lang;
  }

 private:
  static inline Lang                                    lang_{};
  static inline std::unordered_map<GameId, std::string> pathGame_;
  static inline std::vector<std::string>                pathFonts_{};
  static inline bool                                    debug_{};
};
}  // namespace leprac

#endif  // CONFIG_H
