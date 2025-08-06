#ifndef UI_H
#define UI_H
#include "common.h"

namespace leprac {
class UI {
 public:
  static void setStyle(Style style);
  static void setImGuiFont();

  static void StyleSelect();
  static void LangSelect();
};
}  // namespace leprac
#endif
