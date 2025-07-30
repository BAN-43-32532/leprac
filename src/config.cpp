module;
#include <ranges>
#include <toml.hpp>
module leprac.config;

import magic_enum;

namespace leprac {
namespace {
constexpr auto       pathConfig    = "leprac-cfg.toml";
constexpr std::array commentConfig = {
  R"(# Config file for leprac in TOML format)",
  R"(# Auto-generated alongside leprac on first launch)",
  R"(# You can either modify settings in leprac or edit this file directly)"
};

constexpr auto       keyLang     = "language";
constexpr std::array commentLang = {
  R"(# Language for launcher and overlay GUI: "en" / "zh" / "ja")",
  R"(# If omitted, leprac prompts on launch)"
};

constexpr auto       keyStyle     = "style";
constexpr std::array commentStyle = {
  R"(# ImGui style: "dark" / "light" / "classic" / "custom" (not supported yet))",
  R"(# Default: "dark")"
};

constexpr auto       keyGamePath     = "path";
constexpr std::array commentGamePath = {
  R"(# Paths to game executables or directories)",
  R"(# You can configure them in leprac)"
};

constexpr auto       keyFonts     = "fonts";
constexpr std::array commentFonts = {R"(# Paths to custom font ttf/ttc for GUI)"
                                     R"(# You can configure them in leprac)"};

constexpr auto       keyDebug     = "debug";
constexpr std::array commentDebug = {
  R"(# Enable debug mode: true / false (default))"
};
}  // namespace

void Config::sync() {
  syncLang();
  syncStyle();
  syncPathGame();
  syncPathFonts();
  syncDebug();
}

void Config::load() {
  if (auto res = toml::try_parse(pathConfig); res.is_ok()) {
    tomlValue_ = res.unwrap();

    // If lang is not specified of is illegal, lang_ remains unchanged. lang_ is
    // initialized std::nullopt. In this case, leprac prompts to ask which
    // language user chooses
    auto lang = toml::find_or(tomlValue_, keyLang, "");
    lang_     = me::enum_cast<Lang>(lang);

    auto style = toml::find_or(tomlValue_, keyStyle, "dark");
    style_     = me::enum_cast<Style>(style).value_or(style_);

    debug_ = toml::find_or(tomlValue_, keyDebug, false);

    pathFonts_ =
      toml::find_or(tomlValue_, keyFonts, std::vector<std::string>{});

    auto pathValue = tomlValue_[keyGamePath];
    magic_enum::enum_for_each<GameId>([&](auto val) {
      constexpr GameId gameId = val;
      pathGame_[gameId] = toml::find_or(pathValue, me::enum_name(gameId), "");
    });
  }
}

void Config::save() {
  sync();
  tomlValue_.comments().assign(commentConfig.begin(), commentConfig.end());
  tomlValue_[keyGamePath].comments().assign(
    commentGamePath.begin(), commentGamePath.end()
  );
  tomlValue_[keyStyle].comments().assign(
    commentStyle.begin(), commentStyle.end()
  );
  tomlValue_[keyLang].comments().assign(commentLang.begin(), commentLang.end());
  tomlValue_[keyFonts].comments().assign(
    commentFonts.begin(), commentFonts.end()
  );
  tomlValue_[keyDebug].comments().assign(
    commentDebug.begin(), commentDebug.end()
  );

  std::ofstream      ofs(pathConfig);
  std::istringstream iss(format(tomlValue_));
  std::string        line, prev_line;

  while (std::getline(iss, line)) {
    if (!prev_line.empty()
        && prev_line[0] != '#'
        && !line.empty()
        && line[0] == '#') {
      ofs << "\n";
    }
    ofs << line + "\n";
    prev_line = line;
  }
}

void Config::syncLang() {
  if (lang_) {
    tomlValue_[keyLang] = me::enum_name(*lang_);
  } else {
    tomlValue_[keyLang] = "";
  }
}

void Config::syncStyle() { tomlValue_[keyStyle] = me::enum_name(style_); }

void Config::syncPathGame() {
  auto& pathValue = tomlValue_[keyGamePath];
  me::enum_for_each<GameId>([&](auto val) {
    constexpr GameId gameId                       = val;
    pathValue[std::string{me::enum_name(gameId)}] = pathGame_[gameId];
  });
}

void Config::syncPathFonts() { tomlValue_[keyFonts] = pathFonts_; }

void Config::syncDebug() { tomlValue_[keyDebug] = debug_; }
}  // namespace leprac
