#include "launcher.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>
#include <unordered_map>
#include <Windows.h>

#include "asset.h"
#include "config.h"
#include "literal.h"
#include "logger.h"
#include "UI.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT
  ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace leprac {
namespace {
UINT ResizeWidth{};
UINT ResizeHeight{};
}  // namespace

// Forward declarations of helper functions
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Launcher::init() {
  initWindow();
  initImGui();
  game_.init();
}

void Launcher::deinit() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  DestroyWindow(hwnd);
  UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void Launcher::run() {
  bool done = false;
  while (!done) {
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32
    // backend.
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) done = true;
    }
    if (done) break;

    // Handle window being minimized or screen locked
    if (SwapChainOccluded
        && SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      Sleep(10);
      continue;
    }
    SwapChainOccluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (ResizeWidth != 0 && ResizeHeight != 0) {
      CleanupRenderTarget();
      SwapChain->ResizeBuffers(
        0, ResizeWidth, ResizeHeight, DXGI_FORMAT_UNKNOWN, 0
      );
      ResizeWidth = ResizeHeight = 0;
      CreateRenderTarget();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI();

    // Rendering
    ImGui::Render();
    ImVec4      clear_color               = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float const clear_color_with_alpha[4] = {
      clear_color.x * clear_color.w,
      clear_color.y * clear_color.w,
      clear_color.z * clear_color.w,
      clear_color.w
    };
    d3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    d3dDeviceContext->ClearRenderTargetView(
      mainRenderTargetView, clear_color_with_alpha
    );
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr        = SwapChain->Present(1, 0);  // Present with vsync
    SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
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

void Launcher::UI() {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos({}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
  auto flags = ImGuiWindowFlags_NoDecoration
             | ImGuiWindowFlags_NoMove
             | ImGuiWindowFlags_NoBringToFrontOnFocus
             | ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("###Fullscreen", nullptr, flags);
  ImGui::ShowDemoWindow();

  if (ImGui::BeginTabBar("Main tab bar")) {
    if (ImGui::BeginTabItem(lbl("UI", "game"))) {
      std::unordered_map<Game::ID, bool> gameSelected;

      ImGui::Selectable(lbl("UI", "le01_name"), &gameSelected[Game::ID::Le01]);
      ImGui::Selectable(lbl("UI", "le02_name"), &gameSelected[Game::ID::Le02]);
      ImGui::Selectable(lbl("UI", "le03_name"), &gameSelected[Game::ID::Le03]);
      ImGui::Selectable(lbl("UI", "le04_name"), &gameSelected[Game::ID::Le04]);
      ImGui::Selectable(lbl("UI", "uso_name"), &gameSelected[Game::ID::Uso]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(lbl("UI", "tool"))) {
      ImGui::Text("Tool");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(lbl("UI", "setting"))) {
      UI::StyleSelect();
      UI::LangSelect();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}

void Launcher::initWindow() {
  // Make process DPI aware and obtain main monitor scale
  ImGui_ImplWin32_EnableDpiAwareness();
  mainScale = ImGui_ImplWin32_GetDpiScaleForMonitor(
    MonitorFromPoint({}, MONITOR_DEFAULTTOPRIMARY)
  );

  auto hInstance = GetModuleHandle(nullptr);
  wc             = {
    sizeof(wc),
    CS_CLASSDC,
    WndProc,
    0,
    0,
    hInstance,
    LoadIconW(hInstance, MAKEINTRESOURCEW(101)),
    nullptr,
    nullptr,
    nullptr,
    L"leprac",
    LoadIcon(hInstance, MAKEINTRESOURCE(101))
  };
  RegisterClassExW(&wc);

  // auto windowStyle = WS_POPUP | WS_THICKFRAME;
  auto windowStyle = WS_TILEDWINDOW & ~WS_MAXIMIZEBOX;
  RECT workArea{};
  SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
  int screen_width  = workArea.right - workArea.left;
  int screen_height = workArea.bottom - workArea.top;
  int window_width  = static_cast<int>(720 * mainScale);
  int window_height = static_cast<int>(960 * mainScale);
  int pos_x         = workArea.left + (screen_width - window_width) / 2;
  int pos_y         = workArea.top + (screen_height - window_height) / 2;
  hwnd              = CreateWindowW(
    wc.lpszClassName,
    L"leprac",
    windowStyle,
    pos_x,
    pos_y,
    window_width,
    window_height,
    nullptr,
    nullptr,
    wc.hInstance,
    nullptr
  );

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    Logger::critical("Failed to create device");
  }

  // Show the window
  ShowWindow(hwnd, SW_SHOWDEFAULT);
  UpdateWindow(hwnd);
}

void Launcher::initImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io        = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename  = nullptr;

  // Setup Dear ImGui style
  switch (Config::style()) {
  case UI::Style::dark   : ImGui::StyleColorsDark(); break;
  case UI::Style::light  : ImGui::StyleColorsLight(); break;
  case UI::Style::classic: ImGui::StyleColorsClassic(); break;
  case UI::Style::custom:
    Logger::warn("Custom is currently not supported. Fallback to Dark Mode.");
    ImGui::StyleColorsDark();
  }

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(mainScale);
  style.FontScaleDpi = mainScale;

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);

  // If no fonts are loaded, dear imgui will use the default font.
  // You can also load multiple fonts
  // and use ImGui::PushFont()/PopFont() to select them.
  // AddFontFromFileTTF() will return the ImFont*
  // so you can store it if you need to select the font among multiple.
  // If the file cannot be loaded, the function will return a nullptr.
  // Please handle those errors in your application (e.g. use an assertion,
  // or display an error and quit).
  // Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file
  // to use Freetype for higher quality font rendering.
  // Read 'docs/FONTS.md' for more instructions and details.
  style.FontSizeBase = 20.0f;
  io.Fonts->AddFontDefault();
  io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/ArialUni.ttf");
  IM_ASSERT(font != nullptr);
  UI::setImGuiFont();
}

// Helper functions

bool Launcher::CreateDeviceD3D(HWND hWnd) {
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
  sd.OutputWindow       = hWnd;
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
    &SwapChain,
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
      &SwapChain,
      &d3dDevice,
      &featureLevel,
      &d3dDeviceContext
    );
  if (res != S_OK) return false;
  CreateRenderTarget();
  return true;
}

void Launcher::CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (SwapChain) {
    SwapChain->Release();
    SwapChain = nullptr;
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

void Launcher::CreateRenderTarget() {
  ID3D11Texture2D* pBackBuffer;
  SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  d3dDevice->CreateRenderTargetView(
    pBackBuffer, nullptr, &mainRenderTargetView
  );
  pBackBuffer->Release();
}

void Launcher::CleanupRenderTarget() {
  if (mainRenderTargetView) {
    mainRenderTargetView->Release();
    mainRenderTargetView = nullptr;
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
    ResizeWidth  = static_cast<UINT>(LOWORD(lParam));  // Queue resize
    ResizeHeight = static_cast<UINT>(HIWORD(lParam));
    return 0;
  case WM_SYSCOMMAND:
    if (wParam & 0xfff0 == SC_KEYMENU)  // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY: PostQuitMessage(0); return 0;
  default        : Logger::trace("Unhandled message {}", msg);
  }
  return DefWindowProcW(hWnd, msg, wParam, lParam);
}
}  // namespace leprac
