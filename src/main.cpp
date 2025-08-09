#include <Windows.h>

#include "asset.h"
#include "config.h"
#include "launcher.h"
#include "logger.h"

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
