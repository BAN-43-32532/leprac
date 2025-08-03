module;
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <magic_enum/magic_enum_all.hpp>
#include <ostream>
module leprac.logger;
import leprac.common;
// import magic_enum;
import leprac.config;

namespace leprac {
namespace {
fs::path const    pathLog{"leprac-log.txt"};
std::string const divLine(41, '=');
auto const        logHeader = std::format(
  "\n{}\n leprac v{} log - {} \n{}",
  divLine,
  VERSION,
  timestamp(),
  divLine
);
auto const logFooter =
  std::format("{}\n{:^41}\n{}", divLine, "log terminates", divLine);
}  // namespace

void Logger::init() {
  if (console_) {
    // TODO
  } else {
    if (ofs_.is_open()) { ofs_.close(); }
    ofs_.open(pathLog, std::ios::app);
    if (!ofs_) {
      throw std::runtime_error("Failed to open log file: " + pathLog.string());
    }
    std::println(ofs_, "{}", logHeader);
    flush();
  }
}

void Logger::deinit() {
  if (console_) {
    // TODO
  } else {
    std::println(ofs_, "{}", logFooter);
    ofs_.close();
  }
}

void Logger::syncDebug() { debug_ = Config::debug(); }

std::string Logger::prefix(Level lvl) {
  std::string tag = std::format("[{}]", me::enum_name(lvl));
  ranges::transform(tag, tag.begin(), [](auto c) { return std::toupper(c); });
  return std::format("{:<7} {} - ", tag, timestamp());
}

void Logger::flush() { ofs_.flush(); }
}  // namespace leprac
