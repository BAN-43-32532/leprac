#include "logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Windows.h>

#include "config.h"

namespace leprac {
namespace {
std::string const pathLog{"leprac-log.txt"};
std::string const loggerName{"leprac"};
std::string const divLine(41, '=');
auto const        logHeader =
  std::format("\n{}\n leprac v{} log \n{}", divLine, VERSION, divLine);
auto const logFooter =
  std::format("{}\n{:^41}\n{}", divLine, "log terminates", divLine);
}  // namespace

void Logger::init() {
  std::atexit(deinit);
  Config::warmup();
  switch (Config::logMode()) {
  case LoggerMode::file:
    if (Config::logLines() == -1) {
      initBasic();
    } else {
      initRing();
    }
    break;
  case LoggerMode::console: initConsole();
  }
  logger_->set_pattern("[%Y-%m-%d %H:%M:%S] %^[%l]%$ [%n] %v");
  logger_->set_level(Config::logLevel());
  logger_->log(spdlog::level::off, logHeader);
}

void Logger::deinit() {
  info("Logger deinit.");
  logger_->log(spdlog::level::off, logFooter);
  info("Logger deinit done.");
  spdlog::shutdown();
}

void Logger::initConsole() {
  SetConsoleOutputCP(CP_UTF8);
  if (!AllocConsole()) {
    initBasic();
    error(
      "[Logger] Failed to allocate console. Fallback to basic logger (single "
      "infinite log file)."
    );
    return;
  }

  FILE* stream;
  freopen_s(&stream, "CONOUT$", "w", stdout);
  freopen_s(&stream, "CONOUT$", "w", stderr);
  logger_ = spdlog::stdout_color_st(loggerName);
}

void Logger::initBasic() {
  logger_ = spdlog::basic_logger_st(loggerName, pathLog);
}

void Logger::initRing() {
  auto ringSink =
    std::make_shared<spdlog::sinks::ringbuffer_sink_st>(Config::logLines());
  logger_ = std::make_shared<spdlog::logger>(loggerName, ringSink);
}

void Logger::flush() { logger_->flush(); }
}  // namespace leprac
