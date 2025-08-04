#ifndef LITERAL_H
#define LITERAL_H
#include <magic_enum/magic_enum_all.hpp>
#include <toml.hpp>

#include "asset.h"
#include "common.h"
#include "config.h"
#include "logger.h"

namespace leprac {
template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
char const* l(Keys const&... keys) {
  auto        p = &Asset::literal();
  std::string tag{((std::string(keys) + ".") + ...)};
  try {
    // clang-format off
    ((p = &p->at(keys)), ...);
    // clang-format on
  } catch (std::exception const& e) {
    Logger::error("Literal {} not found ({})", tag, e.what());
    return "error";
    // return tag.c_str();
  }
  std::string lang{"en"};
  if (Config::lang()) switch (Config::lang().value()) {
    case Lang::en: lang = "en"; break;
    case Lang::zh: lang = "zh"; break;
    case Lang::ja: lang = "ja";
    }
  // Logger::log(Logger::Level::Warn, "test3");
  // std::string lang{me::enum_name(Config::lang().value())};

  if (p->contains(lang)) {
    if (auto const& str = p->at(lang).as_string(); !str.empty())
      return str.c_str();
  }
  Logger::warn("Literal {} in \"{}\" not set. Fallback to English.", tag, lang);
  return p->at("en").as_string().c_str();
}
}  // namespace leprac
#endif
