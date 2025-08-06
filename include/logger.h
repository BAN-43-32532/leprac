#ifndef LOGGER_H
#define LOGGER_H
#include <spdlog/spdlog.h>

#include "common.h"

namespace leprac {
constexpr uint32_t minLogLines = 10;
constexpr uint32_t maxLogLines = 1000;

class Logger {
 public:
  Logger() = delete;
  static void init();
  static void deinit();

  // Intermediate states for dev; avoid high-freq output.
  template<class... Args>
  static void trace(spdlog::format_string_t<Args...> fmt, Args &&...args);
  // General debug messages.
  template<class... Args>
  static void debug(spdlog::format_string_t<Args...> fmt, Args &&...args);
  // General info for users, e.g. Config init; Le01 attached.
  template<class... Args>
  static void info(spdlog::format_string_t<Args...> fmt, Args &&...args);
  // Recoverable mismatches, e.g. some literal in ja missing, fallback to en.
  template<class... Args>
  static void warn(spdlog::format_string_t<Args...> fmt, Args &&...args);
  // Functional errors that don’t halt leprac.
  template<class... Args>
  static void error(spdlog::format_string_t<Args...> fmt, Args &&...args);
  // Fatal errors: log -> errorBox -> std::atexit() -> std::exit().
  template<class... Args>
  static void critical(spdlog::format_string_t<Args...> fmt, Args &&...args);

 private:
  static inline std::shared_ptr<spdlog::logger> logger_;

  static void initConsole();
  static void initBasic();
  static void initRing();

  static void flush();
};

template<class... Args>
void Logger::trace(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logger_->trace(fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logger_->debug(fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logger_->info(fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logger_->warn(fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logger_->error(fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::critical(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  logger_->critical(msg);
  flush();
  errorBox(msg);
  std::exit(EXIT_FAILURE);
}
}  // namespace leprac

#endif  // LOGGER_H
