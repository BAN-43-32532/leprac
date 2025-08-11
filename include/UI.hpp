#ifndef UI_HPP
#define UI_HPP
#include <imgui.h>
#include <stack>

#include "common.hpp"

// Solution for widget centering, from
// https://github.com/ocornut/imgui/discussions/3862#discussioncomment-8907750
class centered_control_wrapper_t {
 public:
  explicit centered_control_wrapper_t(bool result): result_(result) {}
  explicit operator bool() const { return result_; }

 private:
  bool result_;
};

class centered_control_t {
 public:
  explicit centered_control_t(
    ImVec2 window_size,
    float  y = 0.f,
    float s  = 0.f,
    float su = 0.f
  ):
    window_size_(window_size) {
    y_offset      = y;
    spacing_below = s;
    spacing_up    = su;
  }

  template<typename func>
  centered_control_wrapper_t operator()(func control) const {
    auto   ccw          = centered_control_wrapper_t(false);
    ImVec2 original_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));
    control();
    ImVec2 control_size = ImGui::GetItemRectSize();
    ImGui::Dummy(ImVec2(0, spacing_up));
    ImGui::SetCursorPos(ImVec2(
      (window_size_.x - control_size.x) * 0.5f, original_pos.y + y_offset
    ));
    control();
    ImGui::Dummy(ImVec2(0, spacing_below));
    return ccw;
  }

 private:
  ImVec2 window_size_;
  float  y_offset;
  float  spacing_below;
  float  spacing_up;
};

/*
 * This macros centers in the better way possible, got other function but this
 * one may work for the 90% of widgets of ImGui so it's ok, because for some
 * reason with 'ImGui::BeginChild' doesn't work properly
 *
 * In the __VA_ARGS__ which is pointed to the '...' as the second parameter,
 * means that you can call the macro with more arguments. It may not look safe
 * but i'll explain briefly below
 *
 * First arg  : function ( No matter the type )
 * Second arg : vertical offset to center on
 * Third arg  : add spacing below the widget
 * Fourth arg : add spacing over the widget
 *
 * Cons : to add only spacing you'll need to still add a y_offset, you can do it
 * with 0.f on the second one, to avoid redundancy I made it like that
 */
#define center_(control, ...) \
  centered_control_t{ImGui::GetWindowSize(), __VA_ARGS__}([&]() { control; })

namespace leprac {
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

  static void mainMenu_Setting_StyleSelect();
  static void mainMenu_Setting_LangSelect();

  static void backButton();
  static void
  itemTooltip(char const* text, float width = ImGui::GetFontSize() * 35.0f);
  // ImGui demo implementation. I merged ImGui::SameLine() with it.
  static void HelpMarker(char const* desc);

  static void setStyle(Style style);
  static void setImGuiFont();

 private:
  static inline std::stack<Page> stack_{};
};
}  // namespace leprac
#endif  // UI_HPP
