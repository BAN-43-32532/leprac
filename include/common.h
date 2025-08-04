#ifndef COMMON_H
#define COMMON_H
#include <filesystem>
#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum_all.hpp>

namespace leprac {
namespace fs     = std::filesystem;
namespace ranges = std::ranges;
namespace me     = magic_enum;

constexpr auto VERSION = "0.1.0";

enum class Style {
  dark,
  light,
  classic,
  custom,
};
enum class Lang {
  en,
  zh,
  ja,
};
enum class GameId {
  Le01,
  Le02,
  Le03,
  Le04,
  Uso,  // 束方 嘘時空　～ Fools_rush_in
};

std::string to_utf8(std::wstring const &wstr);
bool        is32bit(libmem::Process const &process);
std::string timestamp();

// void print_hex(std::vector<uint8_t> hex) {
//   logBuffer.println("Data size: {} bytes:", hex.size());
//   logBuffer << "Hex: ";
//   for (size_t i = 0; i < hex.size(); ++i) {
//     logBuffer.append("{:02X} ", hex[i]);
//     if ((i + 1) % 16 == 0 && i < hex.size() - 1) {
//       logBuffer.flush();
//       logBuffer.append("     ");
//     }
//   }
//   logBuffer.flush();
// }
}  // namespace leprac
#endif
