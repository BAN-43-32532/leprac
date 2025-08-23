module;
#include <spdlog/spdlog.h>
#include <string>
#include <toml.hpp>
export module config;

import common;

export namespace leprac {
class Logger;

template<class T>
concept Enum = std::is_enum_v<T>;

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
  // No need to parse comments when reading config
  struct wo_comment_config: toml::type_config {
    using comment_type = toml::discard_comments;
  };

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

  template<Enum T>
  class ProxyEnum {
   public:
    explicit ProxyEnum(toml::value& value, T dflt): value_(value), dflt_(dflt) {}

    [[nodiscard]] explicit(false) operator T() const { return me::enum_cast<T>(value_.as_string()).value_or(dflt_); }

    auto& operator=(T value) {
      value_ = me::enum_name(value);
      return *this;
    }

   private:
    toml::value& value_;
    T            dflt_;
  };

  template<std::integral T>
  class ProxyInt {
   public:
    explicit ProxyInt(toml::value& value, T dflt, std::function<bool(T)> pred):
      value_(value), dflt_(dflt), pred_(pred) {}

    [[nodiscard]] explicit(false) operator T() const {
      if (auto value = value_.as_integer(); std::in_range<T>(value)) {
        auto val = static_cast<T>(value_.as_integer());
        return pred_(val) ? val : dflt_;
      }
      return dflt_;
    }

    auto& operator=(T value) {
      if (pred_(value)) { value_ = value; }
      return *this;
    }

   private:
    toml::value&           value_;
    T                      dflt_;
    std::function<bool(T)> pred_;
  };

  Config() = delete;
  static void init();    // Prepare config and detect if lang is specified
  static void deinit();  // Sync and save to leprac-cfg.toml (with comments)

  static void save();

  static ProxyEnum<Lang>&    lang();
  static ProxyEnum<Style>&   style();
  static ProxyInt<uint32_t>& width();
  static ProxyInt<uint32_t>& height();

  static ProxyEnum<LogMode>& logMode();
  static ProxyEnum<level>&   logLevel();
  static ProxyInt<int>&      logLines();

  static auto& gameInfos() { return gameInfos_; }

  static auto& fontInfos() { return fontInfos_; }

 private:
  static inline toml::value root_{};

  static inline std::vector<GameInfo> gameInfos_{};
  static inline std::vector<FontInfo> fontInfos_{};

  static inline std::string pathConfig = "leprac-cfg.toml";
  static inline std::array  commentConfig{
    R"( Config file for leprac in TOML format)",
    R"( Auto-generated alongside leprac on first launch)",
    R"( You can modify settings in leprac or edit this file directly)"
  };

  static inline Item const itemLang{"language", R"('en' / 'zh' / 'ja' (Prompts on launch if missing))"};
  static inline Item const itemStyle{"style", R"(ImGui style: 'dark' (default) / 'light' / 'classic')"};
  static inline Item const itemWidth{"width"};
  static inline Item const itemHeight{"height"};

  static inline Item const itemLog{"log"};
  static inline Item const itemLogMode{"mode", R"('file' (default) / 'console')"};
  static inline Item const itemLogLevel{"level", R"(spdlog valid level ('info', 'debug', etc.))"};
  static inline Item const itemLogLines{"lines", R"(Max lines in log file. Valid: -1 (no limits), 10~1000)"};

  static inline Item const itemGame{"game"};
  static inline Item const itemGameID{"id"};
  static inline Item const itemGameVersion{"version"};
  static inline Item const itemGamePath{"path"};

  static inline Item const itemFont{"font"};
  static inline Item const itemFontPath{"path"};
  static inline Item const itemFontSize{"size"};

  template<class... T>
  // requires(std::same_as<std::decay_t<T>, Item> && ...)
  static toml::value& config(T&&... item);
  static void         setStringLiteral(toml::value& v);
  // Merge toml::value b into a.
  static void         mergeTOML(toml::value& a, toml::value const& b);
  template<class... T>
  // requires(std::same_as<std::decay_t<T>, Item> && ...)
  static void addComment(T&&... item);
};

template<class... T>
// requires(std::same_as<std::decay_t<T>, Config::Item> && ...)
toml::value& Config::config(T&&... item) {
  auto p = &root_;
  // clang-format off
  ((p = &(*p)[item.key]), ...);
  // clang-format on
  return *p;
}

template<class... T>
// requires(std::same_as<std::decay_t<T>, Config::Item> && ...)
void Config::addComment(T&&... item) {
  auto const& lastComment = (..., item.comment);
  if (!lastComment.empty()) { config(std::forward<T>(item)...).comments().emplace_back(" " + lastComment); }
}
}  // namespace leprac
