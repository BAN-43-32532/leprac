#include <Windows.h>

#include "asset.h"
#include "config.h"
#include "launcher.h"
#include "logger.h"

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
