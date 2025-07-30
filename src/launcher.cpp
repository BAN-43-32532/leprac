module;
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <exception>
#include <imgui.h>
#include <string>
#include <Windows.h>
module leprac.launcher;

namespace leprac {
void Launcher::init() {
  ImGui_ImplWin32_EnableDpiAwareness();
  float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
    MonitorFromPoint({}, MONITOR_DEFAULTTOPRIMARY)
  );
  WNDCLASSEXW wc = {
    sizeof(wc),
    CS_CLASSDC,
    WndProc,
    0L,
    0L,
    GetModuleHandle(nullptr),
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    L"leprac",
    nullptr
  };
  RegisterClassExW(&wc);
  auto windowStyle = WS_POPUP | WS_THICKFRAME;
  RECT workArea{};
  SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
  int screen_width  = workArea.right - workArea.left;
  int screen_height = workArea.bottom - workArea.top;

  int window_width  = static_cast<int>(1280 * main_scale);
  int window_height = static_cast<int>(800 * main_scale);

  int pos_x = workArea.left + (screen_width - window_width) / 2;
  int pos_y = workArea.top + (screen_height - window_height) / 2;

  // 4. 创建窗口
  HWND hwnd = CreateWindowW(
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

  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    throw std::exception("Failed to create device");
  }

  ShowWindow(hwnd, SW_SHOWDEFAULT);
  UpdateWindow(hwnd);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  std::string strstyle = "dark";
  if (strstyle == "dark") ImGui::StyleColorsDark();
  else if (strstyle == "light") ImGui::StyleColorsLight();
  else if (strstyle == "classic") ImGui::StyleColorsClassic();

  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype
  // for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // style.FontSizeBase = 20.0f;
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
  // IM_ASSERT(font != nullptr);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  bool done = false;
  while (!done) {
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT) done = true;
    }
    if (done) break;

    if (SwapChainOccluded
        && SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      Sleep(10);
      continue;
    }
    SwapChainOccluded = false;

    if (ResizeWidth != 0 && ResizeHeight != 0) {
      CleanupRenderTarget();
      SwapChain->ResizeBuffers(
        0, ResizeWidth, ResizeHeight, DXGI_FORMAT_UNKNOWN, 0
      );
      ResizeWidth = ResizeHeight = 0;
      CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                           | ImGuiWindowFlags_NoMove
                           | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##Fullscreen", nullptr, flags);
    ImGui::Text("Overlay!");
    ImGui::ShowDemoWindow();
    ImGui::End();

    ImGui::Render();

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

    HRESULT hr        = SwapChain->Present(1, 0);  // Present with vsync
    // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
    SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
  }

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  DestroyWindow(hwnd);
  UnregisterClassW(wc.lpszClassName, wc.hInstance);






  ///////////////////////////////
  ///
  ///
  ///////
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(1.5f);

  UI::setImGuiFont();

  auto process = libmem::FindProcess("Le03.exe");


  if (!process) {
    logBuffer.println("Process not found");
    return SDL_APP_CONTINUE;
  }

  if (auto result = getStackAddress(*process, tebData->StackLimit, combined)) {
    logBuffer.println("Target address: {:#x}", *result);
  } else {
    logBuffer.println("Failed to get thread stack");
  }
}

void Launcher::UI() {
  // 1. 设置全局样式（可选，调整分割线、间距等）
  ImGuiStyle& style             = ImGui::GetStyle();
  style.ItemSpacing             = ImVec2(4, 4);  // 控件间距
  style.FrameBorderSize         = 0.5f;          // 边框宽度
  style.ItemInnerSpacing        = ImVec2(8, 4);  // 内部间距
  style.SeparatorTextBorderSize = 0.5f;          // 分割线粗细

  // 2. 开始一个无边框的全屏窗口（或你自己的窗口）
  ImGui::Begin("##MainWindow", nullptr);

  // 3. 顶部栏目（也可以用 MenuBar，或自定义一个子区域）
  if (ImGui::BeginMenuBar()) {
    ImGui::Text("🏠 首页");
    ImGui::SameLine();
    ImGui::Text("⚙️ 设置");
    ImGui::SameLine();
    ImGui::Text("❓ 帮助");
    ImGui::EndMenuBar();
  }

  // 4. 留出一些垂直间距
  ImGui::Dummy(ImVec2(0, 8));

  // 5. 内容区域：一行行文本 + 细分割线
  ImGui::BeginChild("ContentRegion", ImVec2(0, 0), false);
  {
    for (int i = 0; i < 20; ++i) {
      ImGui::Text("这是第 %d 行内容，用于演示分割线", i + 1);

      // 如果不是最后一行，画条细的横线做分割
      if (i != 19) {
        // 默认 Separator 已经很细，如需更细可自定义绘制
        ImGui::Separator();
      }
    }
  }
  ImGui::EndChild();

  ImGui::End();
}

bool Launcher::CreateDeviceD3D(HWND hWnd) {
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

  UINT                    createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL       featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
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
extern IMGUI_IMPL_API LRESULT
  ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

LRESULT WINAPI
Launcher::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
  switch (msg) {
  // case WM_NCHITTEST: return HTCAPTION;
  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED) return 0;
    ResizeWidth  = static_cast<UINT>(LOWORD(lParam));
    ResizeHeight = static_cast<UINT>(HIWORD(lParam));
    return 0;
  case WM_SYSCOMMAND:
    if (wParam & 0xfff0 == SC_KEYMENU) return 0;
    break;
  case WM_DESTROY: PostQuitMessage(0); return 0;
  }
  return DefWindowProcW(hWnd, msg, wParam, lParam);
}
}  // namespace leprac
