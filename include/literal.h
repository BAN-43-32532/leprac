#ifndef LITERAL_H
#define LITERAL_H
#include <magic_enum/magic_enum_all.hpp>
#include <toml.hpp>

#include "asset.h"
#include "common.h"
#include "config.h"
#include "logger.h"

namespace leprac {
class Literal {
 public:
  // Returns "key1.key2" style tag for cache index
  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  friend std::string tag(Keys const&... keys);

  // Returns literal[key1][key2] as std::string
  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  friend std::string str(Keys const&... keys);

  // Returns literal[key1][key2] as c_str
  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  friend char const* l(Keys const&... keys);

  // Returns "literal##key1.key2" style c_str for ImGui label
  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  friend char const* lbl(Keys const&... keys);

  // Call this after language change
  static void cacheClear();

 protected:
  static inline std::unordered_map<std::string, std::string> cacheLiteral{};
  static inline std::unordered_map<std::string, std::string> cacheLabel{};

  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  static void cachingLiteral(Keys const&... keys);

  template<class... Keys>
  requires(std::convertible_to<Keys, std::string_view> && ...)
  static void cachingLabel(Keys const&... keys);
};

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
std::string tag(Keys const&... keys) {
  return join(".", keys...);
}

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
std::string str(Keys const&... keys) {
  auto t = tag(keys...);
  if (auto it = Literal::cacheLiteral.find(t);
      it == Literal::cacheLiteral.end() || it->second.empty()) {
    Literal::cachingLiteral(keys...);
  }
  return Literal::cacheLiteral[t];
}

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
char const* l(Keys const&... keys) {
  auto t = tag(keys...);
  if (auto it = Literal::cacheLiteral.find(t);
      it == Literal::cacheLiteral.end() || it->second.empty()) {
    Literal::cachingLiteral(keys...);
  }
  return Literal::cacheLiteral[t].c_str();
}

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
char const* lbl(Keys const&... keys) {
  auto t = tag(keys...);
  if (auto it = Literal::cacheLabel.find(t);
      it == Literal::cacheLabel.end() || it->second.empty()) {
    Literal::cachingLabel(keys...);
  }
  return Literal::cacheLabel[t].c_str();
}

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
void Literal::cachingLiteral(Keys const&... keys) {
  auto p = &Asset::literal();
  auto t = tag(keys...);
  try {
    // clang-format off
    ((p = &p->at(keys)), ...);
    // clang-format on
  } catch (std::exception const& e) {
    Logger::error("Literal {} not found ({})", t, e.what());
    cacheLiteral[t] = t;
  }
  std::string lang{me::enum_name<Lang>(Config::lang())};
  if (p->contains(lang) && !p->at(lang).as_string().empty()) {
    cacheLiteral[t] = p->at(lang).as_string();
  } else {
    Logger::warn("Literal {} in \"{}\" not set. Fallback to English.", t, lang);
    cacheLiteral[t] = p->at("en").as_string();
  }
}

template<class... Keys>
requires(std::convertible_to<Keys, std::string_view> && ...)
void Literal::cachingLabel(Keys const&... keys) {
  auto t = tag(keys...);
  if (auto it = cacheLiteral.find(t);
      it == cacheLiteral.end() || it->second.empty()) {
    cachingLiteral(keys...);
  }
  // str###tag, so that ImGui only hash the tag part
  // str##tag is not correct since the whole str is hashed
  cacheLabel[t] = std::format("{}###{}", str(keys...), t);
}
}  // namespace leprac
#endif
