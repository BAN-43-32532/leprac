#include "literal.h"

namespace leprac {
void Literal::cacheClear() {
  cacheLiteral.clear();
  cacheLabel.clear();
}
}  // namespace leprac
