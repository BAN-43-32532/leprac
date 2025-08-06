#ifndef COMMON_H
#define COMMON_H
#include <filesystem>
#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum_all.hpp>
#include <Windows.h>

namespace leprac {
namespace fs     = std::filesystem;
namespace ranges = std::ranges;
namespace me     = magic_enum;

constexpr auto VERSION = "0.1.0";

std::string  toString(std::wstring const &wstr);
std::wstring toWstring(std::string const &str);

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
