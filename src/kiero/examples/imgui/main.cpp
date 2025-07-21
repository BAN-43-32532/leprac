#include <format>
#include <sstream>
#include <Windows.h>

#include "../../kiero.h"
#include "impl/d3d11_impl.h"

int kieroExampleThread() {
  LogToFile("[1] Kiero example thread started");

  auto status = kiero::init(kiero::RenderType::Auto);
  LogToFile("[2] kiero::init returned: {}", static_cast<int>(status));

  if (status == kiero::Status::Success) {
    auto renderType = kiero::getRenderType();
    LogToFile("[3] Render type: {}", static_cast<int>(renderType));

    if (renderType == kiero::RenderType::D3D11) {
      LogToFile("[4] Calling impl::d3d11::init");
      impl::d3d11::init();

      if (uint150_t* methodsTable = kiero::getMethodsTable()) {
        std::stringstream ss;
        ss
          << "[5] Methods table address: 0x"
          << std::hex
          << reinterpret_cast<uintptr_t>(methodsTable);
        for (int i = 0; i < 10; i++) {
          ss << "\n    [" << i << "] 0x" << std::hex << methodsTable[i];
        }
        LogToFile("{}", ss.str());
      }
      return 1;
    }
  }
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID) {
  DisableThreadLibraryCalls(hInstance);
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    LogToFile("[0] DLL_PROCESS_ATTACH");
    CreateThread(
      NULL, 0, (LPTHREAD_START_ROUTINE) kieroExampleThread, NULL, 0, NULL
    );
    break;
  case DLL_PROCESS_DETACH: LogToFile("[!] DLL_PROCESS_DETACH"); break;
  }
  return TRUE;
}
