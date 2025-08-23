module;
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <toml.hpp>
#include <Windows.h>
module logger;
import config;
import common;

namespace leprac {
namespace {
std::string const pathLog{"leprac-log.txt"};
std::string const loggerName{"leprac"};
}  // namespace

void Logger::init() {
  info("Logger init.");
  // std::atexit(deinit); is called at main() so as to be before other deinits
  switch (Config::logMode()) {
  case LogMode::file   : initFile(); break;
  case LogMode::console: initConsole();
  }
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%n] %^[%l]%$ %v");
  spdlog::set_level(Config::logLevel());
  drainBuffer();
}

void Logger::deinit() {
  info("Logger deinit.\n");
  if (logger_) { logger_->flush(); }
  spdlog::shutdown();
}

void Logger::drainBuffer() {
  if (!logger_) {
    debug("Tried drainBuffer when logger is null.");
    return;
  }
  while (!buffer_.empty()) {
    auto entry = buffer_.front();
    buffer_.pop_front();
    logger_->log(entry.log_time, {}, entry.lvl, entry.msg);
  }
}

void Logger::initConsole() {
  SetConsoleOutputCP(CP_UTF8);
  if (!AllocConsole()) {
    initFile();
    error("Failed to allocate console. Fallback to file logger.");
    return;
  }

  FILE* stream;
  freopen_s(&stream, "CONOUT$", "w", stdout);
  freopen_s(&stream, "CONOUT$", "w", stderr);
  logger_ = spdlog::stdout_color_st(loggerName);
}

void Logger::initFile() {
  if (Config::logLines() == -1) {
    initBasic();
  } else {
    initRing();
  }
}

void Logger::initBasic() { logger_ = spdlog::basic_logger_st(loggerName, pathLog); }

void Logger::initRing() {
  auto ringSink = std::make_shared<spdlog::sinks::ringbuffer_sink_st>(Config::logLines());
  logger_       = std::make_shared<spdlog::logger>(loggerName, ringSink);
}
}  // namespace leprac
