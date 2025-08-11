#include "launcher.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>
#include <Windows.h>

#include "config.hpp"
#include "logger.hpp"
#include "UI.hpp"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT
  ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace leprac {
// Forward declarations of helper functions
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Launcher::init() {
  Logger::info("Launcher init.");
  std::atexit(deinit);

  initWindow();
  initImGui();
  UI::init();
  // game_.init();
}

void Launcher::deinit() {
  Logger::info("Launcher deinit.");
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  cleanupDeviceD3D();
  DestroyWindow(hwnd_);
  UnregisterClass(wc_.lpszClassName, wc_.hInstance);
  Logger::info("Launcher deinit done.");
}

void Launcher::run() {
  while (true) {
    // Poll and handle messages (inputs, window resize, etc.)
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) return;
    }

    // Handle window being minimized or screen locked
    if (swapChainOccluded
        && swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
      continue;
    }
    swapChainOccluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    handleResize();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI::mainMenu();

    // Rendering
    ImGui::Render();
    constexpr float clear_color_with_alpha[4]{1, 1, 1, 0};
    d3dDeviceContext->ClearRenderTargetView(
      mainRenderTargetView, clear_color_with_alpha
    );
    d3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr        = swapChain->Present(1, 0);  // Present with vsync
    swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
  }
}

// void a() {
//   auto process = libmem::FindProcess("Le03.exe");
//
//   if (!process) {
//     logBuffer.println("Process not found");
//     return SDL_APP_CONTINUE;
//   }
//
//   if (auto result = getStackAddress(*process, tebData->StackLimit, combined))
//   {
//     logBuffer.println("Target address: {:#x}", *result);
//   } else {
//     logBuffer.println("Failed to get thread stack");
//   }
// }

void Launcher::initWindow() {
  // Make process DPI aware and obtain main monitor scale
  ImGui_ImplWin32_EnableDpiAwareness();
  mainScale_ = ImGui_ImplWin32_GetDpiScaleForMonitor(
    MonitorFromPoint({}, MONITOR_DEFAULTTOPRIMARY)
  );

  auto hInstance = GetModuleHandle(nullptr);
  wc_            = {
    sizeof(wc_),
    CS_CLASSDC,
    WndProc,
    0,
    0,
    hInstance,
    LoadIcon(hInstance, MAKEINTRESOURCE(101)),
    nullptr,
    nullptr,
    nullptr,
    L"leprac",
    LoadIcon(hInstance, MAKEINTRESOURCE(101))
  };
  RegisterClassEx(&wc_);

  // auto windowStyle = WS_POPUP | WS_THICKFRAME;
  auto windowStyle = WS_TILEDWINDOW & ~WS_MAXIMIZEBOX;
  RECT workArea{};
  SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
  auto windowWidth =
    static_cast<uint32_t>(static_cast<float>(Config::width()) * mainScale_);
  auto windowHeight =
    static_cast<uint32_t>(static_cast<float>(Config::height()) * mainScale_);
  hwnd_ = CreateWindowEx(
    WS_EX_LAYERED,
    wc_.lpszClassName,
    L"leprac",
    windowStyle,
    (workArea.left + workArea.right - windowWidth) / 2,
    (workArea.top + workArea.bottom - windowHeight) / 2,
    windowWidth,
    windowHeight,
    nullptr,
    nullptr,
    wc_.hInstance,
    nullptr
  );

  uint8_t alpha = 255;
  // SetLayeredWindowAttributes(hwnd_, 0, alpha, LWA_ALPHA);
  SetLayeredWindowAttributes(hwnd_, 0, alpha, LWA_ALPHA);

  // Initialize Direct3D
  if (!createDeviceD3D()) {
    cleanupDeviceD3D();
    UnregisterClass(wc_.lpszClassName, wc_.hInstance);
    Logger::critical("Failed to create device");
  }

  // Show the window
  ShowWindow(hwnd_, SW_SHOWDEFAULT);
  UpdateWindow(hwnd_);
}

void Launcher::initImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io        = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename  = nullptr;

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(hwnd_);
  ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);
}

// Helper functions

bool Launcher::createDeviceD3D() {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount                        = 2;
  sd.BufferDesc.Width                   = 0;
  sd.BufferDesc.Height                  = 0;
  sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator   = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow       = hwnd_;
  sd.SampleDesc.Count   = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed           = TRUE;
  sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

  UINT                        createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL           featureLevel;
  constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_0,
  };
  HRESULT res = D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    createDeviceFlags,
    featureLevelArray,
    2,
    D3D11_SDK_VERSION,
    &sd,
    &swapChain,
    &d3dDevice,
    &featureLevel,
    &d3dDeviceContext
  );
  if (res == DXGI_ERROR_UNSUPPORTED)
    res = D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_WARP,
      nullptr,
      createDeviceFlags,
      featureLevelArray,
      2,
      D3D11_SDK_VERSION,
      &sd,
      &swapChain,
      &d3dDevice,
      &featureLevel,
      &d3dDeviceContext
    );
  if (res != S_OK) return false;
  createRenderTarget();
  return true;
}

void Launcher::cleanupDeviceD3D() {
  cleanupRenderTarget();
  if (swapChain) {
    swapChain->Release();
    swapChain = nullptr;
  }
  if (d3dDeviceContext) {
    d3dDeviceContext->Release();
    d3dDeviceContext = nullptr;
  }
  if (d3dDevice) {
    d3dDevice->Release();
    d3dDevice = nullptr;
  }
}

void Launcher::createRenderTarget() {
  ID3D11Texture2D* pBackBuffer;
  swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  d3dDevice->CreateRenderTargetView(
    pBackBuffer, nullptr, &mainRenderTargetView
  );
  pBackBuffer->Release();
}

void Launcher::cleanupRenderTarget() {
  if (mainRenderTargetView) {
    mainRenderTargetView->Release();
    mainRenderTargetView = nullptr;
  }
}

void Launcher::handleResize() {
  if (resizeWidth_ != 0 && resizeHeight_ != 0) {
    cleanupRenderTarget();
    swapChain->ResizeBuffers(
      0, resizeWidth_, resizeHeight_, DXGI_FORMAT_UNKNOWN, 0
    );
    resizeWidth_ = resizeHeight_ = 0;
    createRenderTarget();

    Config::width()  = resizeWidth_;
    Config::height() = resizeHeight_;
  }
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
// dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
// your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from
// your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
  switch (msg) {
  // case WM_NCHITTEST: return HTCAPTION;
  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED) return 0;
    Launcher::resizeWidth()  = LOWORD(lParam);  // Queue resize
    Launcher::resizeHeight() = HIWORD(lParam);
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU)  // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY: PostQuitMessage(0); return 0;
  default        : Logger::trace("Unhandled message {}", msg);
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
}  // namespace leprac
