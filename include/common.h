#ifndef COMMON_H
#define COMMON_H
#include <filesystem>
#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum_all.hpp>
#include <ranges>
#include <Windows.h>

namespace leprac {
namespace fs     = std::filesystem;
namespace ranges = std::ranges;
namespace views  = std::views;
namespace me     = magic_enum;

class Asset;
class Config;
class Game;
class Launcher;
class Literal;
class Logger;
class UI;

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

enum class LoggerMode {
  // Append losgs to a file.
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

constexpr auto VERSION = "0.1.0";

std::string  toString(std::wstring const &wstr);
std::wstring toWstring(std::string const &str);

// Safe when sv contains non-ascii characters.
std::string toLower(std::string_view sv);
std::string toUpper(std::string_view sv);
std::string capitalize(std::string_view sv);

void errorBox(std::string const &text, std::string const &caption = "ERROR");

bool is32bit(libmem::Process const &process);

template<class... Args>
requires(
  sizeof...(Args) >= 1 && (std::convertible_to<Args, std::string_view> && ...)
)
std::string join(std::string_view sep, Args const &...args) {
  size_t total_size = (std::string_view(args).size() + ... + 0)
                    + (sizeof...(args) - 1) * sep.size();
  std::string result;
  result.reserve(total_size);
  bool first = true;
  (((first ? result : result.append(sep)).append(args), first = false), ...);
  return result;
}
template<typename T, size_t N>
std::string join(std::string_view sep, std::array<T, N> const &arr) {
  return [&]<size_t... I>(std::index_sequence<I...>) {
    return join(sep, arr[I]...);
  }(std::make_index_sequence<N>());
}
}  // namespace leprac
#endif
