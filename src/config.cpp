#include "config.h"

#include <magic_enum/magic_enum_all.hpp>
#include <ranges>
#include <toml.hpp>

#include "logger.h"

namespace leprac {
namespace {
constexpr std::string pathConfig = "leprac-cfg.toml";
constexpr std::array  commentConfig{
  R"(Config file for leprac in TOML format)",
  R"(Auto-generated alongside leprac on first launch)",
  R"(You can modify settings in leprac or edit this file directly)"
};

Config::Item const itemLang{
  "language",
  R"("en" / "zh" / "ja" (Prompts on launch if missing))"
};
Config::Item const itemStyle{
  "style",
  R"(ImGui style: "dark" (default) / "light" / "classic")"
};
Config::Item const itemWidth{"width"};
Config::Item const itemHeight{"height"};

Config::Item const itemLog{"log"};
Config::Item const itemLogMode{"mode", R"("file" (default) / "console")"};
Config::Item const itemLogLevel{
  "level",
  R"(spdlog valid level ("info", "debug", etc.))"
};
Config::Item const itemLogLines{
  "lines",
  R"(Max lines in log file. Valid: -1 (no limits), 10~1000)"
};
}  // namespace

void Config::warmup() {
  auto res      = toml::try_parse(pathConfig);
  parseSuccess_ = res.is_ok();
  if (!parseSuccess_) { return; }
  config_ = res.unwrap();

  if (!config_.contains(itemLog.key)) { return; }
  auto log = config_[itemLog.key];

  auto strLogMode  = toml::find_or(log, itemLogMode.key, "");
  logMode()        = me::enum_cast<LoggerMode>(strLogMode).value_or(logMode_);
  auto strLogLevel = toml::find_or(log, itemLogLevel.key, "");
  logLevel() =
    me::enum_cast<spdlog::level::level_enum>(strLogLevel).value_or(logLevel_);
  logLines() = toml::find_or(log, itemLogLines.key, logLines_);
}

void Config::init() {
  Logger::info("Config init start.");
  std::atexit(deinit);
  if (!parseSuccess_) {
    Logger::warn(
      "\"{}\" failed to parse. It may not exist or contain invalid formatting. "
      "Leprac will auto-generate a correct config file upon exit.",
      pathConfig
    );
    return;
  }

  auto strLang = toml::find_or(config_, itemLang.key, "");
  if (auto res = me::enum_cast<Lang>(strLang)) {
    lang() = *res;
  } else {
    Logger::info("Language config not found. Prompt language selection window");
    // TODO: Prompt language selection window
  }
  auto strStyle = toml::find_or(config_, itemStyle.key, "");
  style()       = me::enum_cast<Style>(strStyle).value_or(style_);
  width()       = toml::find_or(config_, itemWidth.key, width_);
  height()      = toml::find_or(config_, itemHeight.key, height_);

  // TODO: GameInfo and FontInfo
  Logger::info("Config init done.");
}

void Config::deinit() {
  Logger::info("Config deinit.");
  sync();
  Logger::info("Config deinit done.");
}

void Config::sync() {
  Logger::info("Config sync all items.");
  syncLang();
  syncStyle();
  syncWidth();
  syncHeight();

  syncLogMode();
  syncLogLevel();
  syncLogLines();

  syncGameInfos();
  syncFontInfos();
  Logger::info("Config sync done.");
}

// void Config::save() {
//   sync();
//   config_.comments().assign(commentConfig.begin(), commentConfig.end());
//   config_[keyGamePath].comments().assign(
//     commentGamePath.begin(), commentGamePath.end()
//   );
//   config_[keyStyle].comments().assign(commentStyle.begin(),
//   commentStyle.end());
//   config_[keyLang].comments().assign(commentLang.begin(), commentLang.end());
//   config_[keyFonts].comments().assign(commentFonts.begin(),
//   commentFonts.end());
//   config_[keyDebug].comments().assign(commentDebug.begin(),
//   commentDebug.end());
//
//   std::ofstream      ofs(pathConfig);
//   std::istringstream iss(format(config_));
//   std::string        line, prev_line;
//
//   while (std::getline(iss, line)) {
//     if (!prev_line.empty()
//         && prev_line[0] != '#'
//         && !line.empty()
//         && line[0] == '#') {
//       ofs << "\n";
//     }
//     ofs << line + "\n";
//     prev_line = line;
//   }
// }

void Config::syncLang() { config_[itemLang.key] = me::enum_name(lang_); }
void Config::syncStyle() { config_[itemStyle.key] = me::enum_name(style_); }
void Config::syncWidth() { config_[itemWidth.key] = width_; }
void Config::syncHeight() { config_[itemHeight.key] = height_; }

void Config::syncLogMode() {
  config_[itemLogMode.key] = me::enum_name(logMode_);
}
void Config::syncLogLevel() {
  config_[itemLogLevel.key] = me::enum_name(logLevel_);
}
void Config::syncLogLines() { config_[itemLogLines.key] = logLines_; }
void Config::syncGameInfos() {}
void Config::syncFontInfos() {}
}  // namespace leprac
