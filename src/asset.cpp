module;
#include <battery/embed.hpp>
module leprac.asset;

namespace leprac {
bool Asset::load() {
  auto address = b::embed<"asset/address.toml">();
  auto literal = b::embed<"asset/literal.toml">();
  auto version = b::embed<"asset/version.toml">();

  auto le01_frame   = b::embed<"asset/le01/frame.toml">();
  auto le01_pattern = b::embed<"asset/le01/pattern.toml">();
  auto le02_frame   = b::embed<"asset/le02/frame.toml">();
  auto le02_pattern = b::embed<"asset/le02/pattern.toml">();
  auto le03_frame   = b::embed<"asset/le03/frame.toml">();
  auto le03_pattern = b::embed<"asset/le03/pattern.toml">();
  return true;
}
}  // namespace leprac
