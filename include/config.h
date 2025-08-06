#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <toml.hpp>
#include <vector>

#include "game.h"
#include "literal.h"
#include "UI.h"

namespace leprac {
class Config {
 public:
  Config() = delete;
  static void warmup();  // Load config for Logger
  static void init();    // Prepare other config and detect if lang is specified
  static void deinit();  // Sync and save to leprac-cfg.toml (with comments)

  static void sync();
  static void load();
  static void save();

  [[nodiscard]] static auto& logMode() { return logMode_; }
  // assign only if minRingLines <= logRingLines <= maxRingLines
  [[nodiscard]] static auto& logRingLines() {
    static struct proxy {
      operator uint32_t() const { return logRingLines_; }
      uint32_t operator=(uint32_t logRingLines) const {
        if (logRingLines >= minRingLines && logRingLines <= maxRingLines) {
          logRingLines_ = logRingLines;
        }
        return logRingLines_;
      }
    } inst;
    return inst;
  }
  [[nodiscard]] static auto& logLevel() { return logLevel_; }
  [[nodiscard]] static auto& width() { return width_; }
  [[nodiscard]] static auto& height() { return height_; }
  [[nodiscard]] static auto& lang() { return lang_; }
  [[nodiscard]] static auto& style() { return style_; }
  [[nodiscard]] static auto& pathGames() { return pathGames_; }
  [[nodiscard]] static auto& pathFonts() { return pathFonts_; }

 private:
  static inline toml::value tomlValue_;

  static inline auto     logMode_{Logger::Mode::Basic};
  static inline uint32_t logRingLines_{100};
  static inline auto     logLevel_{spdlog::level::info};

  static inline uint32_t width_{600};
  static inline uint32_t height_{800};

  static inline auto lang_{Literal::Lang::en};
  static inline auto style_{UI::Style::dark};
  static inline std::unordered_map<Game::ID, std::string> pathGames_{};
  static inline std::vector<std::string>                  pathFonts_{};

  // static void syncLang();
  // static void syncStyle();
  // static void syncPathGame();
  // static void syncPathFonts();
  // static void syncDebug();
};
}  // namespace leprac
#endif
