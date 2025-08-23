// #ifndef CUSTOM_STYLE_HPP
// #define CUSTOM_STYLE_HPP
// #include <imgui.h>
// #include <string>
// #include <typeindex>
//
// #include "aho_corasick.hpp"
// #include "common.hpp"
// #include "logger.hpp"
//
// namespace leprac {
// struct MemberInfo {
//   std::type_index type;
//   size_t          offset;
// };
//
// // asset/style/macro.txt
// #define REG_STYLE(type, name)                           \
//   {                                                     \
//     #name, { typeid(type), offsetof(ImGuiStyle, name) } \
//   }
//
// // file -> string without inline comments
// std::string readRemoveComment(std::ifstream &file);
//
// // Example:
// // keyEnd:                           v
// // style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
// // Return: " ImVec4(0.80f, 0.50f, 0.50f, 1.00f)"
// std::string_view getStrValue(std::string_view data, size_t keyEnd);
//
// // Example:
// // Input: " ImVec4(0.80f, 0.50f, 0.50f, 1.00f)"
// // Return: {.8f, .5f, .5f, 1.f}
// static std::vector<float> parseNumbers(std::string_view str);
//
// template<class T>
// T *stylePtr(size_t offset) {
//   return reinterpret_cast<T *>(reinterpret_cast<char *>(&ImGui::GetStyle()) + offset);
// }
//
// template<class T>
// int parseEnum(std::string_view strValue, bool isOR = true) {
//   static auto trie = [] {
//     aho_corasick::trie trie;
//     for (auto name: me::enum_names<T>()) { trie.insert(std::string(name)); }
//     return std::move(trie.only_whole_words());
//   }();
//   auto entries = trie.parse_text(std::string(strValue));
//   if (isOR) {
//     int enumOR{};
//     for (auto entry: entries) {
//       auto enumValue = me::enum_cast<T>(entry.get_keyword());
//       if (!enumValue.has_value()) { throw; }
//       enumOR |= *enumValue;
//     }
//     return enumOR;
//   }
//   if (entries.size() != 1) { throw; }
//   auto enumValue = me::enum_cast<T>(entries.at(0).get_keyword());
//   if (!enumValue.has_value()) { throw; }
//   return static_cast<int>(*enumValue);
// }
//
// void parseCustomStyle(std::string const &path);
// }  // namespace leprac
//
// #endif  // CUSTOM_STYLE_HPP
