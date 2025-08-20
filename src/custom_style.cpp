#include "custom_style.hpp"

#include <fstream>

#include "aho_corasick.hpp"

namespace leprac {
namespace {
std::unordered_map<std::string, MemberInfo> const reflectStyle{
  REG_STYLE(float, FontSizeBase),
  REG_STYLE(float, FontScaleMain),
  REG_STYLE(float, FontScaleDpi),
  REG_STYLE(float, Alpha),
  REG_STYLE(float, DisabledAlpha),
  REG_STYLE(ImVec2, WindowPadding),
  REG_STYLE(float, WindowRounding),
  REG_STYLE(float, WindowBorderSize),
  REG_STYLE(float, WindowBorderHoverPadding),
  REG_STYLE(ImVec2, WindowMinSize),
  REG_STYLE(ImVec2, WindowTitleAlign),
  REG_STYLE(ImGuiDir, WindowMenuButtonPosition),
  REG_STYLE(float, ChildRounding),
  REG_STYLE(float, ChildBorderSize),
  REG_STYLE(float, PopupRounding),
  REG_STYLE(float, PopupBorderSize),
  REG_STYLE(ImVec2, FramePadding),
  REG_STYLE(float, FrameRounding),
  REG_STYLE(float, FrameBorderSize),
  REG_STYLE(ImVec2, ItemSpacing),
  REG_STYLE(ImVec2, ItemInnerSpacing),
  REG_STYLE(ImVec2, CellPadding),
  REG_STYLE(ImVec2, TouchExtraPadding),
  REG_STYLE(float, IndentSpacing),
  REG_STYLE(float, ColumnsMinSpacing),
  REG_STYLE(float, ScrollbarSize),
  REG_STYLE(float, ScrollbarRounding),
  REG_STYLE(float, GrabMinSize),
  REG_STYLE(float, GrabRounding),
  REG_STYLE(float, LogSliderDeadzone),
  REG_STYLE(float, ImageBorderSize),
  REG_STYLE(float, TabRounding),
  REG_STYLE(float, TabBorderSize),
  REG_STYLE(float, TabMinWidthBase),
  REG_STYLE(float, TabMinWidthShrink),
  REG_STYLE(float, TabCloseButtonMinWidthSelected),
  REG_STYLE(float, TabCloseButtonMinWidthUnselected),
  REG_STYLE(float, TabBarBorderSize),
  REG_STYLE(float, TabBarOverlineSize),
  REG_STYLE(float, TableAngledHeadersAngle),
  REG_STYLE(ImVec2, TableAngledHeadersTextAlign),
  REG_STYLE(ImGuiTreeNodeFlags_, TreeLinesFlags),  // Xx_ is enum type
  REG_STYLE(float, TreeLinesSize),
  REG_STYLE(float, TreeLinesRounding),
  REG_STYLE(ImGuiDir, ColorButtonPosition),
  REG_STYLE(ImVec2, ButtonTextAlign),
  REG_STYLE(ImVec2, SelectableTextAlign),
  REG_STYLE(float, SeparatorTextBorderSize),
  REG_STYLE(ImVec2, SeparatorTextAlign),
  REG_STYLE(ImVec2, SeparatorTextPadding),
  REG_STYLE(ImVec2, DisplayWindowPadding),
  REG_STYLE(ImVec2, DisplaySafeAreaPadding),
  REG_STYLE(float, MouseCursorScale),
  REG_STYLE(bool, AntiAliasedLines),
  REG_STYLE(bool, AntiAliasedLinesUseTex),
  REG_STYLE(bool, AntiAliasedFill),
  REG_STYLE(float, CurveTessellationTol),
  REG_STYLE(float, CircleTessellationMaxError),
  // REG_STYLE(ImVec4, Colors), Colors does not use reflection
  REG_STYLE(float, HoverStationaryDelay),
  REG_STYLE(float, HoverDelayShort),
  REG_STYLE(float, HoverDelayNormal),
  REG_STYLE(ImGuiHoveredFlags_, HoverFlagsForTooltipMouse),  // Xx_ is enum type
  REG_STYLE(ImGuiHoveredFlags_, HoverFlagsForTooltipNav),    // Xx_ is enum type
};

auto trieMember = [] {
  aho_corasick::trie trie;
  for (auto const &key: reflectStyle | views::keys) { trie.insert(key); }
  return std::move(trie.only_whole_words());
}();

auto trieColorEnum = [] {
  aho_corasick::trie trie;
  for (auto name: me::enum_names<ImGuiCol_>()) { trie.insert(std::string(name)); }
  return std::move(trie.only_whole_words());
}();

// auto trieColorName = [] {
//   aho_corasick::trie trie;
//   for (auto value: me::enum_values<ImGuiCol_>()) {
//     trie.insert(ImGui::GetStyleColorName(value));
//   }
//   return std::move(trie.only_whole_words());
// }();
}  // namespace

std::string readRemoveComment(std::ifstream &file) {
  std::string data;
  std::string line;
  while (std::getline(file, line)) {
    auto it = ranges::search(line, "//");
    data.append(line.substr(0, std::distance(line.begin(), it.begin())));
    data.push_back('\n');
  }
  return data;
}

std::string_view getStrValue(std::string_view data, size_t keyEnd) {
  auto pos = keyEnd + 1;
  for (; pos < data.size() && data[pos] != '='; ++pos) {}
  if (pos >= data.size()) { return {}; }
  ++pos;
  for (; pos < data.size() && std::isspace(data[pos]); ++pos) {}
  size_t start = pos;
  for (; pos < data.size() && data[pos] != ';'; ++pos) {}
  return data.substr(start, pos - start);
}

std::vector<float> parseNumbers(std::string_view str) {
  std::vector<float> nums;
  size_t             i = 0;
  while (i < str.size()) {
    for (static std::string ch("-."); i < str.size() && !(std::isdigit(str[i]) || ch.contains(str[i])); ++i) {}
    if (i >= str.size()) { break; }

    size_t start = i;
    for (static std::string ch(".eE+-"); i < str.size() && (std::isdigit(str[i]) || ch.contains(str[i])); ++i) {}
    if (i < str.size() && (str[i] == 'f' || str[i] == 'F')) i++;

    std::string strNum(str.substr(start, i - start));
    try {
      nums.emplace_back(std::stof(strNum));
    } catch (...) { Logger::warn("Invalid number format {} in custom style", strNum); }
  }
  return nums;
}

void parseCustomStyle(std::string const &path) {
  std::ifstream file(path);
  if (!file) {
    Logger::error("Failed to read custom style file. Fallback to ImGui built-in style.");
    return;
  }
  std::string data = readRemoveComment(file);

  for (auto &member: trieMember.parse_text(data)) {
    auto key  = member.get_keyword();
    auto info = reflectStyle.at(key);

    try {
      auto strValue = getStrValue(data, member.get_end());
      if (info.type == typeid(float)) {
        auto numbers = parseNumbers(strValue);
        if (numbers.size() != 1) { throw; }
        *stylePtr<float>(info.offset) = numbers[0];
      } else if (info.type == typeid(ImVec2)) {
        auto numbers = parseNumbers(strValue);
        if (numbers.size() != 2) { throw; }
        *stylePtr<ImVec2>(info.offset) = {numbers[0], numbers[1]};
      } else if (info.type == typeid(ImGuiDir)) {
        auto ptr = stylePtr<ImGuiDir>(info.offset);
        *ptr     = static_cast<ImGuiDir>(parseEnum<ImGuiDir>(strValue, false));
      } else if (info.type == typeid(ImGuiTreeNodeFlags_)) {
        auto ptr = stylePtr<ImGuiTreeNodeFlags>(info.offset);
        *ptr     = parseEnum<ImGuiTreeNodeFlags_>(strValue);
      } else if (info.type == typeid(ImGuiHoveredFlags_)) {
        auto ptr = stylePtr<ImGuiHoveredFlags>(info.offset);
        *ptr     = parseEnum<ImGuiHoveredFlags_>(strValue);
      }
    } catch (std::exception const &error) { Logger::error("Custom style exception: {}", error.what()); }
  }

  for (auto const &colorEnum: trieColorEnum.parse_text(data)) {
    auto key = colorEnum.get_keyword();

    try {
      auto strValue = getStrValue(data, colorEnum.get_end());
      auto numbers  = parseNumbers(strValue);
      if (numbers.size() != 4) { throw; }
      ImGui::GetStyle().Colors[*me::enum_cast<ImGuiCol_>(key)] = {numbers[0], numbers[1], numbers[2], numbers[3]};
    } catch (std::exception const &error) { Logger::error("Custom style exception: {}", error.what()); }
  }

  // for (auto const colorName: trieColorName.parse_text(data)) {
  //   auto key      = colorName.get_keyword();
  //
  //   try {
  //     auto strValue = getStrValue(data, colorName.get_end());
  //     auto numbers  = parseNumbers(strValue);
  //     if (numbers.size() != 4) { throw; }
  //     ImGui::GetStyle().Colors[*me::enum_cast<ImGuiCol_>(key)] = {
  //       numbers[0], numbers[1], numbers[2], numbers[3]
  //     };
  //   } catch (std::exception const &error) {
  //     Logger::error("Custom style exception: {}", error.what());
  //   }
  // }
}
}  // namespace leprac
