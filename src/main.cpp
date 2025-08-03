#include <Windows.h>

import leprac.logger;
import leprac.asset;
import leprac.config;
import leprac.launcher;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  leprac::Logger::init();
  leprac::Asset::init();
  leprac::Config::init();
  leprac::Launcher::init();
  leprac::Launcher::run();
  leprac::Launcher::deinit();
  leprac::Config::deinit();
  leprac::Logger::deinit();
  return 0;
}
