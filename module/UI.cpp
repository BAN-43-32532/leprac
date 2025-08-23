module;
#include <imgui.h>
#include <stack>
export module UI;

import common;

export namespace leprac {
class UI {
 public:
  enum class Page {
    mainMenu,
    mainMenu_Game_Info,
  };

  class Wrapper {
   public:
    explicit operator bool() const { return true; }
  };

  // Usage (example):
  // {
  //   FrameRound fr(5);
  //   if (ImGui::Button("label")) { ... }
  // }
  class FrameRound: public Wrapper {
   public:
    explicit FrameRound(float radius): radius_(radius) {
      radiusOld_                      = ImGui::GetStyle().FrameRounding;
      ImGui::GetStyle().FrameRounding = radius;
    }

    ~FrameRound() { ImGui::GetStyle().FrameRounding = radiusOld_; }

   private:
    float radiusOld_;
    float radius_;
  };

  UI() = delete;
  static void init();
  static void deinit();

  static void mainMenu();

  static void mainMenu_Game();
  static void mainMenu_Tool();
  static void mainMenu_Setting();

  static void mainMenu_Game_Info(GameID gameID);

  static void mainMenu_Game_Add();
  static void mainMenu_Game_Scan();

  static void mainMenu_Game_Launch(size_t idx);
  static void mainMenu_Game_ApplyLeprac(bool disabled);

  static void mainMenu_Game_Modify();
  static void mainMenu_Game_OpenFolder(size_t idx);
  static void mainMenu_Game_Delete();

  static void mainMenu_Setting_StyleSelect();
  static void mainMenu_Setting_LangSelect();

  static void backButton();
  static void itemTooltip(char const* text, float width = ImGui::GetFontSize() * 35.0f);
  // ImGui demo implementation. I merged ImGui::SameLine() with it.
  static void helpMarker(char const* desc);

  static void setStyle(Style style);
  static void setImGuiFont();

 private:
  static inline std::stack<Page> stack_{};
};
}  // namespace leprac
