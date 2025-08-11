#include "UI.hpp"

#include <imgui.h>
#include <magic_enum/magic_enum_all.hpp>
#include <portable-file-dialogs/portable-file-dialogs.h>
#include <typeindex>

#include "common.hpp"
#include "config.hpp"
#include "launcher.hpp"
#include "literal.hpp"
#include "user_custom_style.hpp"

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

  addCustomStyles();
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
  constexpr auto flags =
    ImGuiWindowFlags_NoDecoration & ~ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoNavFocus
    | ImGuiWindowFlags_NoBringToFrontOnFocus;
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
  static auto strSetting = [](auto const&... keys) {
    return str("UI", "setting", keys...);
  };
  static auto txtSetting = [](auto const&... keys) {
    return txt("UI", "setting", keys...);
  };
  static auto lblSetting = [](auto const&... keys) {
    return lbl("UI", "setting", keys...);
  };

  static bool test;
  ImGui::Checkbox(lblSetting("enable_SOCD"), &test);
  HelpMarker(txtSetting("enable_SOCD", "help"));
  ImGui::Checkbox(lblSetting("resize_enable"), &test);
  ImGui::Checkbox(lblSetting("check_update"), &test);
  mainMenu_Setting_StyleSelect();
  mainMenu_Setting_LangSelect();

  ImGui::TextUnformatted(txtSetting("after_game_launch"));
  ImGui::SameLine();
  static int  a               = 0;
  std::string afterGameLaunch = joinZeros(
    strSetting("after_game_launch", "minimize"),
    strSetting("after_game_launch", "close"),
    strSetting("after_game_launch", "remain")
  );
  if (ImGui::Combo("###ComboAfterGameLaunch", &a, afterGameLaunch.c_str())) {}

  ImGui::TextUnformatted(txtSetting("about"));
}
void UI::mainMenu_Game_Info(GameID gameID) {
  backButton();
  ImGui::SameLine();

  center_(ImGui::TextUnformatted(txt("UI", gameNameTag(gameID))));
  static int              idxSelected = -1;
  static Config::GameInfo infoSelected{};
  if (ImGui::BeginTable(
        "GameInfoTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders
      )) {
    for (auto const& [i, gameInfo]: views::enumerate(Config::gameInfos())) {
      if (gameInfo.id != gameID) { continue; }
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      bool selected{idxSelected == i};
      ImGui::Selectable(
        Game::versionToLiteral(gameInfo.version).c_str(),
        &selected,
        ImGuiSelectableFlags_SpanAllColumns
      );
      if (selected) {
        idxSelected  = i;
        infoSelected = gameInfo;
      } else if (idxSelected == i) {
        idxSelected = -1;
      }
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(gameInfo.path.c_str());
    }
    ImGui::EndTable();
  }
  if (ImGui::Button(lbl("UI", "game", "add"))) {
    auto a = pfd::open_file(
               "", "", {"EXE Files (.exe)", "*.exe"}, pfd::opt::multiselect
    )
               .result();
    if (!a.empty()) {
      Logger::debug("{}", a[0]);
      Game::searchIDFromEXE(a[0]);
      Game::searchVersionFromEXE(gameID, a[0]);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button(lbl("UI", "game", "scan"))) {
    auto a = pfd::select_folder("").result();
    if (!a.empty()) Logger::debug("{}", a);
  }
  ImGui::SameLine();
  static bool test{};
  bool        notSupported =
    idxSelected >= 0
    && !Game::isVersionSupported(infoSelected.id, infoSelected.version)
          .value_or(false);
  if (notSupported) {
    bool dummy{};
    ImGui::BeginDisabled();
    ImGui::Checkbox(lbl("UI", "game", "apply_leprac"), &dummy);
    itemTooltip(txt("UI", "game", "not_supported"));
    ImGui::EndDisabled();
  } else if (ImGui::Checkbox(lbl("UI", "game", "apply_leprac"), &test)) {
    ;
  }
  ImGui::BeginDisabled(idxSelected == -1);
  if (ImGui::Button(lbl("UI", "game", "modify"))) { ; }
  ImGui::SameLine();
  if (ImGui::Button(lbl("UI", "game", "open_folder"))) { ; }
  ImGui::SameLine();
  if (ImGui::Button(lbl("UI", "game", "delete"))) { ; }
  if (ImGui::Button(lbl("UI", "game", "launch"))) { ; }
  ImGui::EndDisabled();
}

void UI::mainMenu_Setting_StyleSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::style()));
  ImGui::TextUnformatted(txt("UI", "setting", "style"));
  ImGui::SameLine();
  if (ImGui::Combo("###ComboStyle", &idx, "Dark\0Light\0Classic\0")) {
    auto style      = *me::enum_cast<Style>(idx);
    Config::style() = style;
    setStyle(style);
  }
}

void UI::mainMenu_Setting_LangSelect() {
  static int idx = static_cast<int>(*me::enum_index(Config::lang()));

  ImGui::TextUnformatted(txt("UI", "setting", "language"));
  ImGui::SameLine();
  auto langNames = me::enum_names<Lang>() | views::transform([](auto sv) {
    return Asset::literal().at("lang_name").at(std::string(sv)).as_string();
  });

  if (ImGui::Combo("###ComboLang", &idx, joinZeros(langNames).c_str())) {
    Config::lang() = *me::enum_cast<Lang>(idx);
    Literal::cacheClear();
  }
}

void UI::backButton() {
  FrameRound fr(5);
  if (ImGui::Button(lbl("UI", "back"))) { stack_.pop(); }
}

void UI::itemTooltip(char const* text, float width) {
  auto minWidth = min(width, ImGui::GetWindowWidth());
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(minWidth);
    ImGui::TextUnformatted(text);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void UI::HelpMarker(char const* desc) {
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  itemTooltip(desc);
}

void UI::setStyle(Style style) {
  switch (style) {
  case Style::dark   : ImGui::StyleColorsDark(); break;
  case Style::light  : ImGui::StyleColorsLight(); break;
  case Style::classic: ImGui::StyleColorsClassic(); break;
  }
  // Logger::debug("{}", customStyles.contains("Example Style"));
  // customStyles["Example Style2"]();
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
