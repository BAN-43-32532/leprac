#ifndef CONFIG_H
#define CONFIG_H

#include "type.h"
#include <magic_enum/magic_enum_all.hpp>
#include <string>
#include <vector>

#include "toml.hpp"

namespace leprac {
class Config {
 public:
  static void sync();
  static void load();
  static void save();

  static auto const& lang() { return lang_; }

  static void setLanguage(Lang lang) { lang_ = lang; }

 private:
  static inline toml::value tomlValue_;

  static inline Lang        lang_  = Lang::en;
  static inline Style       style_ = Style::dark;
  static inline std::unordered_map<GameId, std::string> pathGame_{};
  static inline std::vector<std::string> pathFonts_{};

  static inline bool debug_ = false;

  static void syncLang();
  static void syncStyle();
  static void syncPathGame();
  static void syncPathFonts();
  static void syncDebug();
};
}  // namespace leprac

#endif  // CONFIG_H
