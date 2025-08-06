#include "UI.h"

#include <imgui.h>
#include <magic_enum/magic_enum_all.hpp>

#include "common.h"
#include "config.h"
#include "literal.h"

namespace leprac {
void UI::setStyle(Style style) {
  switch (style) {
  case Style::dark   : ImGui::StyleColorsDark(); break;
  case Style::light  : ImGui::StyleColorsLight(); break;
  case Style::classic: ImGui::StyleColorsClassic(); break;
  }
}
void UI::setImGuiFont() {
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
    Logger::error("Failed to load font {}", fonts[0].second);
  }

  font_cfg.MergeMode   = true;
  auto font_zh_default = io.Fonts->AddFontFromFileTTF(
    "C:/Windows/Fonts/msyhl.ttc",
    16.0f,
    &font_cfg,
    io.Fonts->GetGlyphRangesChineseFull()
  );
  if (!font_zh_default) {
    Logger::error("Failed to load font {}", fonts[1].second);
  }

  auto font_ja_default = io.Fonts->AddFontFromFileTTF(
    "C:/Windows/Fonts/meiryo.ttc",
    16.0f,
    &font_cfg,
    io.Fonts->GetGlyphRangesJapanese()
  );
  if (!font_ja_default) {
    Logger::error("Failed to load font {}", fonts[2].second);
  }

  io.Fonts->Build();
}

void UI::StyleSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::style()));
  ImGui::Text(l("UI", "setting", "style"));
  ImGui::SameLine();
  if (ImGui::Combo("###ComboStyle", &idx, "Dark\0Light\0Classic")) {
    auto style      = *me::enum_cast<Style>(idx);
    Config::style() = style;
    setStyle(style);
  }
}

void UI::LangSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::lang()));
  static constexpr size_t numLang = me::enum_count<Lang>() - 1;
  static auto const       strLang = [] {
    std::array<std::string, numLang> tmp;
    size_t                           i = 0;
    for (auto name: me::enum_names<Lang>()) {
      if (name != "unk") {
        tmp[i] =
          Asset::literal().at("lang_name").at(std::string(name)).as_string();
        ++i;
      }
    }
    return tmp;
  }();
  static auto const itemLang = [] {
    std::array<char const*, numLang> arr{};
    for (size_t i = 0; i < numLang; ++i) { arr[i] = strLang[i].c_str(); }
    return arr;
  }();

  ImGui::Text(l("UI", "setting", "language"));
  ImGui::SameLine();
  if (ImGui::Combo("###ComboLang", &idx, itemLang.data(), numLang)) {
    Config::lang() = *me::enum_cast<Lang>(idx);
    Literal::cacheClear();
  }
}
}  // namespace leprac
