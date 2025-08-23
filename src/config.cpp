module;
#include <ranges>
#include <toml.hpp>
module config;
import logger;

namespace leprac {
void Config::init() {
  Logger::info("Config init.");
  std::atexit(deinit);

  config(itemLang)   = "en";
  config(itemStyle)  = "dark";
  config(itemWidth)  = 600;
  config(itemHeight) = 800;

  config(itemLog, itemLogMode)  = "file";
  config(itemLog, itemLogLevel) = "info";
  config(itemLog, itemLogLines) = -1;

  config(itemGame) = toml::array();
  config(itemFont) = toml::array();

  if (auto result = toml::try_parse<wo_comment_config>(pathConfig); result.is_err()) {
    Logger::warn(
      "\"{}\" failed to parse. It may not exist or contain invalid format. "
      "Leprac will auto-generate a correct config file upon exit.",
      pathConfig
    );
  } else {
    mergeTOML(root_, result.unwrap());
  }

  for (auto const& gameInfoValue: config(itemGame).as_array()) {
    GameInfo gameInfo;
    try {
      gameInfo.id      = *me::enum_cast<GameID>(gameInfoValue.at(itemGameID.key).as_string());
      gameInfo.version = gameInfoValue.at(itemGameVersion.key).as_string();
      gameInfo.path    = gameInfoValue.at(itemGamePath.key).as_string();
    } catch (std::exception const& e) { Logger::warn("Invalid [[game]] item omitted. Exception: {}", e.what()); }
    gameInfos_.emplace_back(gameInfo);
  }

  for (auto const& fontInfoValue: config(itemFont).as_array()) {
    FontInfo fontInfo;
    try {
      fontInfo.path = fontInfoValue.at(itemFontPath.key).as_string();
      fontInfo.size = fontInfoValue.at(itemFontSize.key).as_integer();
    } catch (std::exception const& e) { Logger::warn("Invalid [[font]] item omitted. Exception: {}", e.what()); }
    fontInfos_.emplace_back(fontInfo);
  }
}

void Config::deinit() {
  Logger::info("Config deinit.");
  save();
}

void Config::save() {
  Logger::info("Config save.");

  try {
    Logger::info("1");
    config(itemGame).as_array().clear();
    // config(itemGame).as_array().resize(gameInfos_.size());
    Logger::info("1");
    for (auto const& [idx, info]: gameInfos_ | views::enumerate) {
      auto const& [id, version, path] = info;
      auto& table                     = config(itemGame).emplace_back(toml::table());
      table[itemGameID.key]           = std::string(me::enum_name(id));
      table[itemGameVersion.key]      = version;
      table[itemGamePath.key]         = path;
    }
    Logger::info("1");
    if (config(itemGame).is_array_of_tables()) {
      config(itemGame).as_array_fmt().fmt = toml::array_format::array_of_tables;
    }
    Logger::info("1");
    config(itemFont).as_array().clear();
    // config(itemFont).as_array().resize(gameInfos_.size());
    for (auto const& [idx, info]: fontInfos_ | views::enumerate) {
      auto const& [path, size] = info;
      auto& table              = config(itemFont).emplace_back(toml::table());
      table[itemFontPath.key]  = path;
      table[itemFontSize.key]  = size;
    }
    Logger::info("1");
    if (config(itemFont).is_array_of_tables()) {
      config(itemFont).as_array_fmt().fmt = toml::array_format::array_of_tables;
    }
  } catch (std::exception const& e) { Logger::error("{}", e.what()); }

  setStringLiteral(root_);

  root_.comments().insert(root_.comments().end(), commentConfig.begin(), commentConfig.end());

  Logger::info("1");

  addComment(itemLang);
  addComment(itemStyle);
  addComment(itemWidth);
  addComment(itemHeight);

  Logger::info("1");

  addComment(itemLog, itemLogMode);
  addComment(itemLog, itemLogLevel);
  addComment(itemLog, itemLogLines);

  Logger::info("1");

  std::ofstream      ofs(pathConfig);
  std::istringstream iss(format(root_));
  std::string        line, prev_line;

  Logger::info("1");

  while (std::getline(iss, line)) {
    if (!prev_line.empty() && prev_line[0] != '#' && !line.empty() && line[0] == '#') { ofs << "\n"; }
    for (size_t pos = 0; pos < line.size(); ++pos) {
      if (line[pos] == '=') { break; }
      if (line[pos] == '"') { line.erase(pos, 1); }
    }
    // line.erase(ranges::remove(line, '\"').begin(), line.end());
    ofs << line + "\n";
    prev_line = line;
  }
  Logger::info("Config save done.");
}

Config::ProxyEnum<Lang>& Config::lang() {
  static ProxyEnum inst{config(itemLang), Lang::en};
  return inst;
}

Config::ProxyEnum<Style>& Config::style() {
  static ProxyEnum inst{config(itemStyle), Style::dark};
  return inst;
}

Config::ProxyInt<uint32_t>& Config::width() {
  static ProxyInt<uint32_t> inst{config(itemWidth), 600, [](uint32_t val) { return val <= 3000; }};
  return inst;
}

Config::ProxyInt<uint32_t>& Config::height() {
  static ProxyInt<uint32_t> inst{config(itemHeight), 800, [](uint32_t val) { return val <= 2000; }};
  return inst;
}

Config::ProxyEnum<LogMode>& Config::logMode() {
  static ProxyEnum inst{config(itemLog, itemLogMode), LogMode::file};
  return inst;
}

Config::ProxyEnum<level>& Config::logLevel() {
  static ProxyEnum inst{config(itemLog, itemLogLevel), level::info};
  return inst;
}

Config::ProxyInt<int>& Config::logLines() {
  static ProxyInt<int> inst{config(itemLog, itemLogLines), -1, [](int val) {
    return val == -1 || val >= minLogLines && val <= maxLogLines;
  }};
  return inst;
}

void Config::setStringLiteral(toml::value& v) {
  if (v.is_string()) {
    if (not v.as_string().contains('\'')) { v.as_string_fmt().fmt = toml::string_format::literal; }
  } else if (v.is_table()) {
    for (auto& sub: v.as_table() | std::views::values) { setStringLiteral(sub); }
  } else if (v.is_array()) {
    for (auto& sub: v.as_array()) { setStringLiteral(sub); }
  }
}

void Config::mergeTOML(toml::value& a, toml::value const& b) {
  if (a.is_table() && b.is_table()) {
    for (auto const& [k, vb]: b.as_table()) {
      if (a.contains(k)) {
        mergeTOML(a[k], vb);
      } else {
        a[k] = vb;
      }
    }
  } else if (a.is_array() && b.is_array()) {
    auto& arra = a.as_array();
    arra.insert_range(arra.end(), b.as_array());
  } else {
    a = b;
  }
}
}  // namespace leprac
