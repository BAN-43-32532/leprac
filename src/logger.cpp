module;
#include <algorithm>
#include <chrono>
#include <filesystem>
module leprac.logger;
import leprac.common;
import magic_enum;
import leprac.config;

namespace leprac {
void Logger::init(fs::path log_file) {
  if (ofs_.is_open()) { ofs_.close(); }
  ofs_.open(log_file, std::ios::app);
  if (!ofs_) {
    throw std::runtime_error("Failed to open log file: " + log_file.string());
  }
}

void Logger::syncDebug() { debug_ = Config::debug(); }

std::string Logger::prefix(Level lvl) {
  using std::chrono::system_clock;
  if (lvl == Level::Debug && !debug_) return {};
  std::string tag = std::format("[{}]", me::enum_name(lvl));
  ranges::transform(tag, tag.begin(), [](auto c) { return std::toupper(c); });
  return std::format("{:<7} {:%Y-%m-%d %H:%M:%S} - ", tag, system_clock::now());
}
}  // namespace leprac
