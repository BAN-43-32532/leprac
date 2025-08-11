#ifndef USER_CUSTOM_STYLE_HPP
#define USER_CUSTOM_STYLE_HPP
#include <imgui.h>
#include <string>
#include <unordered_map>

namespace leprac {
inline std::unordered_map<std::string, void (*)()> customStyles;

/*
 * Add your custom style cpp code here, and build leprac on your own!
 * Please note that ImGui Style API has changed over time.
 */

// https://github.com/ocornut/imgui/issues/707#issuecomment-252413954
inline void ExampleStyle() {
  ImGuiStyle& style       = ImGui::GetStyle();
  style.WindowRounding    = 5.3f;
  style.FrameRounding     = 2.3f;
  style.ScrollbarRounding = 0;
  // clang-format off
  style.Colors[ImGuiCol_Text]                  = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
  // ImGuiCol_ChildWindowBg              -> use ImGuiCol_ChildBg
  // style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
  style.Colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
  style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
  style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
  style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
  style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
  style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
  style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
  // Style: Removed `ImGuiCol_ComboBg` in favor of combo boxes using `ImGuiCol_PopupBg` for consistency. Combo are normal pop-ups.
  // style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.1f, 0.1f, 0.1f, 0.99f);
  style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
  style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
  style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
  style.Colors[ImGuiCol_Button]                = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
  style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
  style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_Header]                = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
  style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
  //  - 2017/08/13 (1.51) - renamed ImGuiCol_Column to ImGuiCol_Separator, ImGuiCol_ColumnHovered to ImGuiCol_SeparatorHovered, ImGuiCol_ColumnActive to ImGuiCol_SeparatorActive.
  //  Kept redirection enums (will obsolete).
  // style.Colors[ImGuiCol_Column]                = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  // style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
  // style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
  style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
  style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
  // Removed ImGuiCol_CloseButton, ImGuiCol_CloseButtonActive, ImGuiCol_CloseButtonHovered style colors as the closing cross uses regular button colors now.
  // style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
  // style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
  // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
  style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
  // ImGuiCol_ModalWindowDarkening               -> use ImGuiCol_ModalWindowDimBg
  // style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
  // clang-format on

  style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_Separator]        = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive]  = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

inline void ExampleStyle2() {
  ImGuiStyle& style = ImGui::GetStyle();

  // light style from Pacôme Danhiez (user itamago)
  // https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
  style.Alpha                             = 1.0f;
  style.FrameRounding                     = 3.0f;
  style.Colors[ImGuiCol_Text]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]     = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
  style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg]          = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_Border]           = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_BorderShadow]     = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  style.Colors[ImGuiCol_FrameBg]          = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
  style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
    ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
    ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  style.Colors[ImGuiCol_CheckMark]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]        = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive]  = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Button]            = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_ButtonHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive]      = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Header]            = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  style.Colors[ImGuiCol_HeaderHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  style.Colors[ImGuiCol_HeaderActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Separator]         = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered]  = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  style.Colors[ImGuiCol_SeparatorActive]   = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  style.Colors[ImGuiCol_PlotLines]         = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]  = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
    ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]   = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

  bool  bStyleDark_ = false;
  float alpha_      = .3f;
  if (bStyleDark_) {
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      float   H, S, V;
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

      if (S < 0.1f) { V = 1.0f - V; }
      ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
      if (col.w < 1.00f) { col.w *= alpha_; }
    }
  } else {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      if (col.w < 1.00f) {
        col.x *= alpha_;
        col.y *= alpha_;
        col.z *= alpha_;
        col.w *= alpha_;
      }
    }
  }
}

inline void addCustomStyles() {
  // customStyles["Name to show in GUI"] = function;
  customStyles["Example Style"]  = ExampleStyle;
  customStyles["Example Style2"] = ExampleStyle2;
}
}  // namespace leprac
#endif  // USER_CUSTOM_STYLE_HPP
