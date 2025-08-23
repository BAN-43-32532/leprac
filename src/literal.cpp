module;
#include <toml.hpp>
module literal;
namespace leprac {
void Literal::cacheClear() {
  cacheLiteral.clear();
  cacheLabel.clear();
}
}  // namespace leprac
