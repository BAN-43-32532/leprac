#include "asset.hpp"

#include <battery/embed.hpp>
#include <toml.hpp>

#include "logger.hpp"

#define ASSET_PATH(rel)         "asset/" #rel ".toml"
#define ASSET_PATH2(rel1, rel2) "asset/" #rel1 "/" #rel2 ".toml"

namespace leprac {
void Asset::init() {
  Logger::info("Asset init");
  std::atexit(deinit);
  load();
}

void Asset::deinit() { Logger::info("Asset deinit (no work to do)."); }

void Asset::load() {
  address_ = toml::parse_str(b::embed<ASSET_PATH(address)>().str());
  literal_ = toml::parse_str(b::embed<ASSET_PATH(literal)>().str());
  version_ = toml::parse_str(b::embed<ASSET_PATH(version)>().str());
  link_    = toml::parse_str(b::embed<ASSET_PATH(link)>().str());

  frame_[GameID::Le01]   = toml::parse_str(b::embed<ASSET_PATH2(le01, frame)>().str());
  pattern_[GameID::Le01] = toml::parse_str(b::embed<ASSET_PATH2(le01, pattern)>().str());
  frame_[GameID::Le02]   = toml::parse_str(b::embed<ASSET_PATH2(le02, frame)>().str());
  pattern_[GameID::Le02] = toml::parse_str(b::embed<ASSET_PATH2(le02, pattern)>().str());
  frame_[GameID::Le03]   = toml::parse_str(b::embed<ASSET_PATH2(le03, frame)>().str());
  pattern_[GameID::Le03] = toml::parse_str(b::embed<ASSET_PATH2(le03, pattern)>().str());
}
}  // namespace leprac
