#include "UI.h"

#include <imgui.h>
#include <magic_enum/magic_enum_all.hpp>

#include "common.h"
#include "config.h"
#include "launcher.h"
#include "literal.h"

namespace leprac {
// Use ImGui::TextUnformatted(txt(keys)) to get faster while suppressing
// Clangd: Format string is not a string literal (potentially insecure)

namespace {
constexpr auto pathSystemFonts = "C:/Windows/Fonts/";
;
}  // namespace

std::string gameNameTag(GameID id) {
  return std::format("{}_name", toLower(me::enum_name<GameID>(id)));
}

void UI::init() {
  Logger::info("UI init.");
  std::atexit(deinit);

  setStyle(Config::style());
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(Launcher::mainScale());
  style.FontScaleDpi = Launcher::mainScale();

  // If no fonts are loaded, dear imgui will use the default font.
  // You can also load multiple fonts
  // and use ImGui::PushFont()/PopFont() to select them.
  // AddFontFromFileTTF() will return the ImFont*
  // so you can store it if you need to select the font among multiple.
  // If the file cannot be loaded, the function will return a nullptr.
  // Please handle those errors in your application (e.g. use an assertion,
  // or display an error and quit).
  // Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file
  // to use Freetype for higher quality font rendering.
  // Read 'docs/FONTS.md' for more instructions and details.
  style.FontSizeBase = 20.0f;
  auto& io           = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/ArialUni.ttf");
  // IM_ASSERT(font != nullptr);
  setImGuiFont();

  stack_.emplace(Page::mainMenu);
}

void UI::deinit() { Logger::info("UI deinit."); }

void UI::mainMenu() {
  ImGui::SetNextWindowPos({}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
  auto flags = ImGuiWindowFlags_NoDecoration
             | ImGuiWindowFlags_NoMove
             | ImGuiWindowFlags_NoBringToFrontOnFocus
             | ImGuiWindowFlags_NoSavedSettings;
  ImGui::Begin("###MainMenu", nullptr, flags);

  if (ImGui::BeginTabBar("MainTabBar")) {
    if (ImGui::BeginTabItem(lbl("UI", "game"))) {
      mainMenu_Game();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(lbl("UI", "tool"))) {
      mainMenu_Tool();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(lbl("UI", "setting"))) {
      mainMenu_Setting();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}

void UI::mainMenu_Game() {
  static GameID gameSelected;

  switch (stack_.top()) {
  case Page::mainMenu:
    for (auto gameID: me::enum_values<GameID>()) {
      bool selected{};
      ImGui::Selectable(lbl("UI", gameNameTag(gameID)), &selected);
      if (selected) {
        gameSelected = gameID;
        stack_.emplace(Page::mainMenu_Game_Info);
        break;
      }
    }
    break;
  case Page::mainMenu_Game_Info: mainMenu_Game_Info(gameSelected);
  }
}

void UI::mainMenu_Tool() { center_(ImGui::Text("Tool")); }

void UI::mainMenu_Setting() {
  mainMenu_Setting_StyleSelect();
  mainMenu_Setting_LangSelect();
}
void UI::mainMenu_Game_Info(GameID gameID) {
  backButton();
  ImGui::SameLine();
  center_(ImGui::TextUnformatted(txt("UI", gameNameTag(gameID))));
  static bool selected{};
  if (ImGui::BeginTable(
        "GameInfoTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders
      )) {
    for (auto const& gameInfo: Config::gameInfos()) {
      if (gameInfo.id != gameID) { continue; }
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Selectable(
        gameInfo.version.c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns
      );
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(gameInfo.path.c_str());
    }
    ImGui::EndTable();
  }
}

void UI::mainMenu_Setting_StyleSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::style()));
  ImGui::TextUnformatted(txt("UI", "setting", "style"));
  ImGui::SameLine();
  if (ImGui::Combo("###ComboStyle", &idx, "Dark\0Light\0Classic")) {
    auto style      = *me::enum_cast<Style>(idx);
    Config::style() = style;
    setStyle(style);
  }
}

void UI::mainMenu_Setting_LangSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::lang()));
  static constexpr size_t numLang = me::enum_count<Lang>();
  static auto const       strLang = [] {
    std::array<std::string, numLang> tmp;
    size_t                           i = 0;
    for (auto name: me::enum_names<Lang>()) {
      tmp[i] =
        Asset::literal().at("lang_name").at(std::string(name)).as_string();
      ++i;
    }
    return tmp;
  }();
  static auto const itemLang = [] {
    std::array<char const*, numLang> arr{};
    for (size_t i = 0; i < numLang; ++i) { arr[i] = strLang[i].c_str(); }
    return arr;
  }();

  ImGui::TextUnformatted(txt("UI", "setting", "language"));
  ImGui::SameLine();
  if (ImGui::Combo("###ComboLang", &idx, itemLang.data(), numLang)) {
    Config::lang() = *me::enum_cast<Lang>(idx);
    Literal::cacheClear();
  }
}

void UI::backButton() {
  FrameRound fr(5);
  if (ImGui::Button(lbl("UI", "back"))) { stack_.pop(); }
}

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
}  // namespace leprac
