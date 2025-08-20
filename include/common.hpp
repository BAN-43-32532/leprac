#ifndef COMMON_HPP
#define COMMON_HPP
#include <filesystem>
#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum_all.hpp>
#include <ranges>
#include <Windows.h>

#include "aho_corasick.hpp"
#include "spdlog/common.h"

namespace leprac {
using namespace std::string_literals;
namespace fs     = std::filesystem;
namespace ranges = std::ranges;
namespace views  = std::views;
namespace me     = magic_enum;
using level      = spdlog::level::level_enum;
using ParseMode  = aho_corasick::trie::ParseMode;

template<typename T>
concept CvtSV = std::convertible_to<T, std::string_view>;

class Asset;
class Config;
class Game;
class Launcher;
class Literal;
class Logger;
class UI;
class Update;

enum class GameID {
  Le01,
  Le02,
  Le03,
  Le04,
  Uso,  // JynX call it Uso, while westerners may call it FRI
};

// To add more languages, simply add the language code to Literal::Lang.
// It is recommended to use a 2-letter lowercase language code.
// Provide the translations in the asset/literal.toml file.
// At minimum, specify the language name under the [lang_name] section:
// [lang_name]
// en = "English"
// zh = ...
// ja = ...
// your_lang_code = "a UTF-8 name"
// If the translation for a tag is missing in the new language,
// it will fallback to English, with a warning logged.
// Note: you have to load a font that supports the language.
enum class Lang {
  en,
  zh,
  ja,
  // add more language codes here if needed
};

enum class LogMode {
  // Append logs to a file.
  // If logLines is positive, only keep the latest #logLines lines.
  // Otherwise, no line limits.
  // You can always claer the log file manually by clicking a button.
  file,
  console,  // Open a console and display logs there; no file output.
};

enum class Style {
  dark,
  light,
  classic,
};

constexpr inline auto VERSION = "0.1.0";

constexpr inline uint32_t minLogLines = 10;
constexpr inline uint32_t maxLogLines = 1000;

std::string  toString(std::wstring const &wstr);
std::wstring toWstring(std::string const &str);

std::string toUTF16(std::string const &str);
std::string toUTF8(std::string const &str);

// Safe when sv contains non-ascii characters.
std::string toLower(std::string_view sv);
std::string toUpper(std::string_view sv);
std::string capitalize(std::string_view sv);

void errorBox(std::string const &text, std::string const &caption = "ERROR");

bool is32bit(libmem::Process const &process);

// Use joinZeros for ImGui::Combo. There have to be two '\0' at the end.

template<ranges::input_range R>
requires ranges::common_range<R> && std::convertible_to<ranges::range_value_t<R>, std::string_view>
std::string join(std::string_view sep, R &&range) {
  return range | views::join_with(sep) | ranges::to<std::string>();
}

template<ranges::input_range R>
requires ranges::common_range<R> && std::convertible_to<ranges::range_value_t<R>, std::string_view>
std::string joinZeros(R &&range) {
  return join({"\0", 1}, range).append(std::string_view("\0", 1));
}

template<class... T>
requires(std::convertible_to<T, std::string_view> && ...) std::string join(std::string_view sep, T &&...args) {
  std::array<std::string_view, sizeof...(T)> arr{std::forward<T>(args)...};
  return join(sep, arr);
}

template<class... T>
requires(std::convertible_to<T, std::string_view> && ...) std::string joinZeros(T &&...args) {
  std::array<std::string_view, sizeof...(T)> arr{std::forward<T>(args)...};
  return joinZeros(arr);
}

// Previous implementation
// std::string join(std::string_view sep, std::string arg, auto const &...args)
// requires(std::convertible_to<decltype(args), std::string_view> && ...) {
//   if constexpr (sizeof...(args) > 0) {
//     size_t size = arg.size()
//                 + (std::string_view(args).size() + ...)
//                 + sizeof...(args) * sep.size();
//     arg.reserve(size);
//     (arg.append(sep).append(args), ...);
//   }
//   return arg;
// }
//
// std::string joinZeros(std::string arg, auto const &...args)
// requires(std::convertible_to<decltype(args), std::string_view> && ...) {
//   return join({"\0", 1}, arg, args...).append(std::string_view("\0", 1));
// }
}  // namespace leprac
#endif  // COMMON_HPP
