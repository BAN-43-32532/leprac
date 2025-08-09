#ifndef CONFIG_H
#define CONFIG_H
#include <spdlog/spdlog.h>
#include <string>
#include <toml.hpp>
#include <vector>

#include "common.h"
#include "logger.h"

namespace leprac {
/**
 * Example of leprac-cfg.toml
 *
 * # Config file for leprac in TOML format
 * # Auto-generated alongside leprac on first launch
 * # You can modify settings in leprac or edit this file directly
 *
 * language = "en" # "en" / "zh" / "ja" (Prompts on launch if missing)
 * style = "dark" # ImGui style: "dark" (default) / "light" / "classic"
 * width = 600
 * height = 800
 *
 * [log]
 * mode = "file" # "file" (default) / "console"
 * level = "info" # spdlog valid level ("info", "debug", etc.)
 * lines = -1 # Max lines in log file. Valid: -1 (no limits), 10~1000
 *
 * [[font]]
 * path = ""
 * size = 20
 *
 * [[game]]
 * id = "Le01"
 * version = "1_20a"
 * path = "./Le01.exe"
 *
 * [[game]]
 * id = "Le02"
 * version = "1_20a"
 * path = "./Le02.exe"
 *
 * [[game]]
 * id = "Le03"
 * version = "1_21a"
 * path = "./Le03.exe"
 *
 * [[game]]
 * id = "Le04"
 * version = "1_20f"
 * path = "./Le04.exe"
 *
 * [[game]]
 * id = "Uso"
 * version = "0_20b"
 * path = "./Uso.exe"
 */
class Config {
 public:
  struct Item {
    std::string key;
    std::string comment;
  };

  struct GameInfo {
    GameID      id;
    std::string version;
    std::string path;
  };

  struct FontInfo {
    std::string path;
    uint32_t    size;
  };

  Config() = delete;
  static void init();    // Prepare config and detect if lang is specified
  static void deinit();  // Sync and save to leprac-cfg.toml (with comments)

  static void sync();
  // static void save();

  [[nodiscard]] static auto& lang() { return lang_; }
  [[nodiscard]] static auto& style() { return style_; }
  [[nodiscard]] static auto& width() { return width_; }
  [[nodiscard]] static auto& height() { return height_; }

  [[nodiscard]] static auto& logMode() { return logMode_; }
  [[nodiscard]] static auto& logLevel() { return logLevel_; }
  // assign only if logLines == 1 or minLogLines <= logLines <= maxLogLines
  [[nodiscard]] static auto& logLines() {
    static struct proxy {
      operator int() const { return logLines_; }
      int operator=(int logLines) const {
        if (logLines == -1
            || logLines >= minLogLines && logLines <= maxLogLines) {
          logLines_ = logLines;
        }
        return logLines_;
      }
    } inst;
    return inst;
  }

  [[nodiscard]] static auto& gameInfos() { return gameInfos_; }
  [[nodiscard]] static auto& fontInfos() { return fontInfos_; }

  static void syncLang();
  static void syncStyle();
  static void syncWidth();
  static void syncHeight();

  static void syncLogMode();
  static void syncLogLevel();
  static void syncLogLines();

  static void syncGameInfos();
  static void syncFontInfos();

 private:
  static inline toml::value config_{};
  static inline bool        parseSuccess_{};

  static inline auto     lang_{Lang::en};
  static inline auto     style_{Style::dark};
  static inline uint32_t width_{600};
  static inline uint32_t height_{800};

  static inline auto logMode_{LoggerMode::file};
  static inline auto logLevel_{spdlog::level::info};
  static inline int  logLines_{-1};

  static inline std::vector<GameInfo> gameInfos_{};
  static inline std::vector<FontInfo> fontInfos_{};
};
}  // namespace leprac
#endif
