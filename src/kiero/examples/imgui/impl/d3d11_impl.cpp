#include "d3d11_impl.h"

#include <assert.h>
#include <d3d11.h>

#include "../../../kiero.h"

// clang-format off
#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_dx11.h"
#include "../imgui/examples/imgui_impl_win32.h"
// clang-format on

#include "win32_impl.h"

typedef long(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);
static Present oPresent = NULL;

long __stdcall hkPresent11(
  IDXGISwapChain* pSwapChain,
  UINT            SyncInterval,
  UINT            Flags
) {
  LogToFile("=== hkPresent11 called ===");

  static bool init = false;
  if (!init) {
    LogToFile("Initializing ImGui for D3D11");

    DXGI_SWAP_CHAIN_DESC desc;
    if (FAILED(pSwapChain->GetDesc(&desc))) {
      LogToFile("Failed to get swap chain description");
      return oPresent(pSwapChain, SyncInterval, Flags);
    }

    LogToFile(
      "SwapChain: Width={}, Height={}, Windowed={}",
      desc.BufferDesc.Width,
      desc.BufferDesc.Height,
      desc.Windowed
    );

    ID3D11Device* device = nullptr;
    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**) &device)
        )) {
      LogToFile("Failed to get D3D11 device from swap chain");
      return oPresent(pSwapChain, SyncInterval, Flags);
    }

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);

    impl::win32::init(desc.OutputWindow);

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(desc.OutputWindow);
    ImGui_ImplDX11_Init(device, context);

    LogToFile("ImGui initialized successfully");
    init = true;
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  impl::showExampleWindow("D3D11");

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return oPresent(pSwapChain, SyncInterval, Flags);
}

void impl::d3d11::init() {
  LogToFile("Attempting to bind Present hook");

  int const indices[] = {8, 22, 140};
  for (int index: indices) {
    auto status = kiero::bind(index, (void**) &oPresent, hkPresent11);
    LogToFile("Binding index {}: result {}", index, static_cast<int>(status));

    if (status == kiero::Status::Success) {
      LogToFile("Successfully bound to index {}", index);
      return;
    }
  }

  LogToFile("All binding attempts failed!");
}
