#include "logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Windows.h>

#include "config.h"

namespace leprac {
namespace {
std::string const pathLog{"leprac-log.txt"};
std::string const divLine(41, '=');
auto const        logHeader =
  std::format("\n{}\n leprac v{} log \n{}", divLine, VERSION, divLine);
auto const logFooter =
  std::format("{}\n{:^41}\n{}", divLine, "log terminates", divLine);
}  // namespace

void Logger::init() {
  std::atexit(deinit);
  Config::warmup();
  logger_->set_level(Config::logLevel());
  switch (Config::logMode()) {
  case Mode::Console: initConsole(); break;
  case Mode::Basic  : initBasic(); break;
  case Mode::Ring   : initRing();
  }
  logger_->log(spdlog::level::off, logHeader);
}

void Logger::deinit() {
  logger_->log(spdlog::level::off, logFooter);
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
  logger_ = spdlog::stdout_color_st("console_logger");
}

void Logger::initBasic() {
  logger_ = spdlog::basic_logger_st("basic_logger", pathLog);
}

void Logger::initRing() {
  auto ringSink =
    std::make_shared<spdlog::sinks::ringbuffer_sink_st>(Config::logRingLines());
  logger_ = std::make_shared<spdlog::logger>("ringLogger", ringSink);
}

void Logger::flush() { logger_->flush(); }
}  // namespace leprac

// namespace {
// fs::path const    pathLog{"leprac-log.txt"};

// }  // namespace
