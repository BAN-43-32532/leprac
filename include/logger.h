#ifndef LOGGER_H
#define LOGGER_H
#include <deque>
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

  // Log functions below can be called safely even if Logger hasn't init yet.

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

  struct LogEntry {
    spdlog::log_clock::time_point log_time;
    spdlog::level::level_enum     lvl{spdlog::level::off};
    std::string                   msg;
  };
  // Save messages here before Logger::init() and will output after that
  static inline std::deque<LogEntry> buffer_;

  template<class... Args>
  static void logBuffered(
    spdlog::level::level_enum        lvl,
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
  );
  static void drainBuffer();

  static void initConsole();
  static void initFile();
  static void initBasic();
  static void initRing();
};

template<class... Args>
void Logger::trace(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logBuffered(spdlog::level::trace, fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logBuffered(spdlog::level::debug, fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logBuffered(spdlog::level::info, fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logBuffered(spdlog::level::warn, fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  logBuffered(spdlog::level::err, fmt, std::forward<Args>(args)...);
}

template<class... Args>
void Logger::critical(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  if (logger_) {
    logger_->critical(msg);
    logger_->flush();
  }
  errorBox(msg);
  std::exit(EXIT_FAILURE);
}

template<class... Args>
void Logger::logBuffered(
  spdlog::level::level_enum        lvl,
  spdlog::format_string_t<Args...> fmt,
  Args &&...args
) {
  if (logger_ == nullptr) [[unlikely]] {
    buffer_.emplace_back(
      spdlog::log_clock::now(),
      lvl,
      fmt::format(fmt, std::forward<Args>(args)...)
    );
  } else {
    logger_->log(lvl, fmt, std::forward<Args>(args)...);
  }
}
}  // namespace leprac

#endif  // LOGGER_H
