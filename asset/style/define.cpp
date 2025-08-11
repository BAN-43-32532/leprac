#include <imgui.h>
/*
 * This is the definition of struct ImGuiStyle copied from imgui.h (1.92.1)
 * Used regex (?m)//.*$ to remove all comments.
 * Then use macro.py to convert to REG_STYLE(type, name) macros for reflection.
 * Finally, add a underscore after ImGuiTreeNodeFlags & ImGuiHoveredFlags
 * to make them enum. Also, comment out ImVec4 Colors[ImGuiCol_COUNT]
 */

#define DEF_START
float              FontSizeBase;
float              FontScaleMain;
float              FontScaleDpi;
float              Alpha;
float              DisabledAlpha;
ImVec2             WindowPadding;
float              WindowRounding;
float              WindowBorderSize;
float              WindowBorderHoverPadding;
ImVec2             WindowMinSize;
ImVec2             WindowTitleAlign;
ImGuiDir           WindowMenuButtonPosition;
float              ChildRounding;
float              ChildBorderSize;
float              PopupRounding;
float              PopupBorderSize;
ImVec2             FramePadding;
float              FrameRounding;
float              FrameBorderSize;
ImVec2             ItemSpacing;
ImVec2             ItemInnerSpacing;
ImVec2             CellPadding;
ImVec2             TouchExtraPadding;
float              IndentSpacing;
float              ColumnsMinSpacing;
float              ScrollbarSize;
float              ScrollbarRounding;
float              GrabMinSize;
float              GrabRounding;
float              LogSliderDeadzone;
float              ImageBorderSize;
float              TabRounding;
float              TabBorderSize;
float              TabMinWidthBase;
float              TabMinWidthShrink;
float              TabCloseButtonMinWidthSelected;
float              TabCloseButtonMinWidthUnselected;
float              TabBarBorderSize;
float              TabBarOverlineSize;
float              TableAngledHeadersAngle;
ImVec2             TableAngledHeadersTextAlign;
ImGuiTreeNodeFlags TreeLinesFlags;
float              TreeLinesSize;
float              TreeLinesRounding;
ImGuiDir           ColorButtonPosition;
ImVec2             ButtonTextAlign;
ImVec2             SelectableTextAlign;
float              SeparatorTextBorderSize;
ImVec2             SeparatorTextAlign;
ImVec2             SeparatorTextPadding;
ImVec2             DisplayWindowPadding;
ImVec2             DisplaySafeAreaPadding;
float              MouseCursorScale;
bool               AntiAliasedLines;
bool               AntiAliasedLinesUseTex;
bool               AntiAliasedFill;
float              CurveTessellationTol;
float              CircleTessellationMaxError;
ImVec4             Colors[ImGuiCol_COUNT];
float              HoverStationaryDelay;
float              HoverDelayShort;
float              HoverDelayNormal;
ImGuiHoveredFlags  HoverFlagsForTooltipMouse;
ImGuiHoveredFlags  HoverFlagsForTooltipNav;
