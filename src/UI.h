#ifndef UI_H
#define UI_H
#include <imgui.h>

#include "literal.h"
#include "logger.h"

namespace leprac {


//
// inline auto toString(Lang lang) {
//   static char const* lang2string[]{"en", "zh", "ja"};
//   return lang2string[static_cast<int>(lang)];
// }

class UI {
 public:
  // private:
  static void setImGuiFont() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig font_cfg;
    font_cfg.MergeMode  = false;
    font_cfg.PixelSnapH = true;

    auto pathFonts = "C:/Windows/Fonts/";

    std::pair<Lang, char const*> fonts[]{
      {Lang::en, "segoeui.ttf"},
      {Lang::zh,   "msyhl.ttc"},
      {Lang::ja,  "meiryo.ttc"}
    };

    auto font_en_default = io.Fonts->AddFontFromFileTTF(
      "C:/Windows/Fonts/segoeui.ttf", 16.0f, &font_cfg
    );
    if (!font_en_default) {
      logBuffer.println("Failed to load font {}", fonts[0].second);
    }

    font_cfg.MergeMode   = true;
    auto font_zh_default = io.Fonts->AddFontFromFileTTF(
      "C:/Windows/Fonts/msyhl.ttc",
      16.0f,
      &font_cfg,
      io.Fonts->GetGlyphRangesChineseFull()
    );
    if (!font_zh_default) {
      logBuffer.println("Failed to load font {}", fonts[1].second);
    }

    auto font_ja_default = io.Fonts->AddFontFromFileTTF(
      "C:/Windows/Fonts/meiryo.ttc",
      16.0f,
      &font_cfg,
      io.Fonts->GetGlyphRangesJapanese()
    );
    if (!font_ja_default) {
      logBuffer.println("Failed to load font {}", fonts[2].second);
    }

    io.Fonts->Build();
  }
};

// bool ImGui::ShowStyleSelector(const char* label)
// {
//   static int style_idx = -1;
//   if (ImGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
//   {
//     switch (style_idx)
//     {
//     case 0: ImGui::StyleColorsDark(); break;
//     case 1: ImGui::StyleColorsLight(); break;
//     case 2: ImGui::StyleColorsClassic(); break;
//     }
//     return true;
//   }
//   return false;
// }
}  // namespace leprac

#endif  // UI_H
