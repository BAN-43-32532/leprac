#include <cstdlib>
#include <Windows.h>
import logger;
import common;
import config;
import asset;
import launcher;

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
