module;
#include <toml.hpp>
export module literal;

import common;
import asset;
import logger;
import config;

export namespace leprac {
class Literal {
 public:
  // Returns "key1.key2" style tag for cache index
  template<CvtSV... T>
  friend std::string tag(T&&... keys);

  // Returns literal[key1][key2] as std::string
  template<CvtSV... T>
  friend std::string const& str(T&&... keys);

  // Returns literal[key1][key2] as c_str
  template<CvtSV... T>
  friend char const* txt(T&&... keys);

  // Returns "literal###key1.key2" style c_str for ImGui label
  // Should use triple #, so that ImGui only hashes the tag part
  // Double # only hides the tag while the whole str is hashed
  template<CvtSV... T>
  friend char const* lbl(T&&... keys);

  // Call this after language change
  static void cacheClear();

 protected:
  static inline std::unordered_map<std::string, std::string> cacheLiteral{};
  static inline std::unordered_map<std::string, std::string> cacheLabel{};

  template<CvtSV... T>
  static void cachingLiteral(T&&... keys);
  template<CvtSV... T>
  static void cachingLabel(T&&... keys);
};

template<CvtSV... T>
std::string tag(T&&... keys) {
  return join(".", std::forward<T>(keys)...);
}

template<CvtSV... T>
std::string const& str(T&&... keys) {
  auto t = tag(std::forward<T>(keys)...);
  if (auto it = Literal::cacheLiteral.find(t); it == Literal::cacheLiteral.end() || it->second.empty()) {
    Literal::cachingLiteral(keys...);
  }
  return Literal::cacheLiteral[t];
}

template<CvtSV... T>
char const* txt(T&&... keys) {
  return str(std::forward<T>(keys)...).c_str();
}

template<CvtSV... T>
char const* lbl(T&&... keys) {
  auto t = tag(std::forward<T>(keys)...);
  if (auto it = Literal::cacheLabel.find(t); it == Literal::cacheLabel.end() || it->second.empty()) {
    Literal::cachingLabel(keys...);
  }
  return Literal::cacheLabel[t].c_str();
}

template<CvtSV... T>
void Literal::cachingLiteral(T&&... keys) {
  auto p = &Asset::literal();
  auto t = tag(std::forward<T>(keys)...);
  try {
    // Leave it be to suppress warning and stop clang-format being fool.
    // clang-format off
    ((p->contains(keys) && ((p = &p->at(keys)))), ...);
    // clang-format on
  } catch (std::exception const& e) {
    Logger::error("Literal {} not found. Exception: {}", t, e.what());
    cacheLiteral[t] = t;
  }
  std::string lang{me::enum_name<Lang>(Config::lang())};
  if (!p->contains(lang) || p->at(lang).as_string().empty()) {
    Logger::warn("Literal {} in \"{}\" not set. Fallback to English.", t, lang);
    lang = "en";
  }
  cacheLiteral[t] = p->at(lang).as_string();
}

template<CvtSV... T>
void Literal::cachingLabel(T&&... keys) {
  auto t = tag(std::forward<T>(keys)...);
  if (auto it = cacheLiteral.find(t); it == cacheLiteral.end() || it->second.empty()) { cachingLiteral(keys...); }
  cacheLabel[t] = std::format("{}###{}", str(keys...), t);
}
}  // namespace leprac
