#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <toml.hpp>
#include <vector>

#include "common.h"

namespace leprac {
class Config {
 public:
  Config() = delete;
  static void init();
  static void deinit();

  static void sync();
  static void load();
  static void save();

  static auto const& lang() { return lang_; }

  static void setLang(Lang lang) {
    lang_ = lang;
    syncLang();
  }

  static auto const& style() { return style_; }

  static void setStyle(Style style) {
    style_ = style;
    syncStyle();
  }

  static auto const& pathGame() { return pathGame_; }

  static void setPathGame(GameId game, std::string const& path) {
    pathGame_[game] = path;
    syncPathGame();
  }

  static auto const& pathFonts() { return pathFonts_; }

  static void setPathFonts(std::vector<std::string> const& fonts) {
    pathFonts_ = fonts;
    syncPathFonts();
  }

  static bool debug() { return debug_; }

  static void setDebug(bool value) {
    debug_ = value;
    syncDebug();
  }

 private:
  static inline toml::value tomlValue_;

  static inline Lang                                    lang_{Lang::en};
  static inline Style                                   style_{Style::dark};
  static inline std::unordered_map<GameId, std::string> pathGame_{};
  static inline std::vector<std::string>                pathFonts_{};
  static inline bool                                    debug_{};

  static void syncLang();
  static void syncStyle();
  static void syncPathGame();
  static void syncPathFonts();
  static void syncDebug();
};
}  // namespace leprac
#endif
