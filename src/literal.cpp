#include "literal.h"

#include <ranges>

namespace leprac {
void Literal::cacheClear() {
  cacheLiteral.clear();
  cacheLabel.clear();
}
}  // namespace leprac
