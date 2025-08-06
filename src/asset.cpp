#include "asset.h"

#include <battery/embed.hpp>
#include <toml.hpp>

#define ASSET_PATH(rel)         "asset/" #rel ".toml"
#define ASSET_PATH2(rel1, rel2) "asset/" #rel1 "/" #rel2 ".toml"

namespace leprac {
void Asset::init() {
  Logger::info("Asset init");
  load();
}

void Asset::load() {
  address_ = toml::parse_str(b::embed<ASSET_PATH(address)>().str());
  literal_ = toml::parse_str(b::embed<ASSET_PATH(literal)>().str());
  version_ = toml::parse_str(b::embed<ASSET_PATH(version)>().str());

  frame_[Game::ID::Le01] =
    toml::parse_str(b::embed<ASSET_PATH2(le01, frame)>().str());
  pattern_[Game::ID::Le01] =
    toml::parse_str(b::embed<ASSET_PATH2(le01, pattern)>().str());
  frame_[Game::ID::Le02] =
    toml::parse_str(b::embed<ASSET_PATH2(le02, frame)>().str());
  pattern_[Game::ID::Le02] =
    toml::parse_str(b::embed<ASSET_PATH2(le02, pattern)>().str());
  frame_[Game::ID::Le03] =
    toml::parse_str(b::embed<ASSET_PATH2(le03, frame)>().str());
  pattern_[Game::ID::Le03] =
    toml::parse_str(b::embed<ASSET_PATH2(le03, pattern)>().str());
}
}  // namespace leprac
