#include <Windows.h>

#include "asset.h"
#include "config.h"
#include "launcher.h"
#include "logger.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  using namespace leprac;
  Logger::init();
  Asset::init();
  Config::init();
  Launcher::init();
  Launcher::run();
  return 0;
}
