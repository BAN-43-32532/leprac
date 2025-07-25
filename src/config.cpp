#include "config.h"

#include <battery/embed.hpp>
#include <ranges>

namespace ranges = std::ranges;
namespace me     = magic_enum;

namespace leprac {
static constexpr auto pathConfig = "leprac-cfg.toml";

static constexpr std::array commentConfig = {
  R"( This is the configuration file for leprac, written in TOML format.)",
  R"( It is auto-generated alongside leprac.exe on first launch.)",
  R"( You can modify settings either within leprac or by editing this file directly.)"
};

static constexpr std::array commentLanguage = {
  R"( Language setting for launcher and overlay GUI ("en", "zh", "ja").)",
  R"( If omitted, leprac prompts on launch.)"
};

static constexpr std::array commentStyle = {
  R"( ImGui style: "dark", "light", "classic", "custom" (not supported yet).)",
  R"( Default: "dark")"
};

static constexpr std::array commentGamePath = {
  R"( Paths to game executables or directories.)",
  R"( You can configure them in leprac.)"
};

static constexpr std::array commentFonts = {
  R"( Fonts list for GUI (default []).)"
};

static constexpr std::array commentDebug = {
  R"( Enable debug mode: true or false (default false).)"
};

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

    auto lang = toml::find_or(tomlValue_, "language", "");
    lang_     = me::enum_cast<Lang>(lang).value_or(lang_);

    auto style = toml::find_or(tomlValue_, "style", "dark");
    style_     = me::enum_cast<Style>(style).value_or(style_);

    magic_enum::enum_for_each<GameId>([&](auto val) {
      constexpr GameId gameId = val;
      auto key          = std::format("path{}", me::enum_name(gameId));
      pathGame_[gameId] = toml::find_or(tomlValue_, key, "");
    });

    pathFonts_ =
      toml::find_or(tomlValue_, "pathFonts", std::vector<std::string>{});
    debug_ = toml::find_or(tomlValue_, "debug", false);
  }
}

void Config::save() {
  sync();
  ranges::copy(commentConfig, std::back_inserter(tomlValue_.comments()));
  ranges::copy(commentLanguage, std::back_inserter(tomlValue_["language"].comments()));
  ranges::copy(commentStyle, std::back_inserter(tomlValue_["style"].comments()));
  ranges::copy(commentGamePath, std::back_inserter(tomlValue_["pathLe01"].comments()));
  ranges::copy(commentFonts, std::back_inserter(tomlValue_["pathFonts"].comments()));
  ranges::copy(commentDebug, std::back_inserter(tomlValue_["debug"].comments()));
  std::ofstream fileConfig(pathConfig);
  fileConfig << format(tomlValue_);
}

void Config::syncLang() { tomlValue_["language"] = me::enum_name(lang_); }

void Config::syncStyle() { tomlValue_["style"] = me::enum_name(style_); }

void Config::syncPathGame() {
  me::enum_for_each<GameId>([&](auto val) {
    constexpr GameId gameId = val;
    auto             key = std::format("path{}", me::enum_name(gameId));
    tomlValue_[key]      = pathGame_[gameId];
  });
}

void Config::syncPathFonts() {
  tomlValue_["pathFonts"] = pathFonts_;
}

void Config::syncDebug() {
  tomlValue_["debug"] = debug_;
}
}  // namespace leprac
