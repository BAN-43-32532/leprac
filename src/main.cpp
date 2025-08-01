#include <Windows.h>

import leprac.logger;
import leprac.config;
import leprac.launcher;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  // leprac::Logger::init();
  // leprac::Config::init();
  leprac::Launcher launcher;
  launcher.init();
  return 0;
}
