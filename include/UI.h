#ifndef UI_H
#define UI_H

namespace leprac {
class UI {
 public:
  enum class Style {
    dark,
    light,
    classic,
    custom,  // TODO: Not supported yet. Currently fallback to dark.
  };
  static void setImGuiFont();

  static void StyleSelect();
  static void LangSelect();
};
}  // namespace leprac
#endif
