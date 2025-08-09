#include <Windows.h>

#include "asset.hpp"
#include "config.hpp"
#include "launcher.hpp"
#include "logger.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  using namespace leprac;
  Logger::info("leprac v{} starts.", VERSION);
  std::atexit(Logger::deinit);
  Config::init();
  Logger::init();
  Asset::init();
  Launcher::init();
  Launcher::run();
  Logger::info("leprac v{} terminates.", VERSION);
  return 0;
}
