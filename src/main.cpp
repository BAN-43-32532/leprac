#define SDL_MAIN_USE_CALLBACKS

// clang-format off
#include <windows.h>
#include <tlhelp32.h>
// clang-format on

#include <any>
#include <iostream>
#include <print>
#include <typeindex>
#include <unordered_map>
#include <winternl.h>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "imgui.h"
#include "libmem/libmem.hpp"
#include "logBuffer.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

namespace leprac {
struct SDL_State {
  SDL_Window   *window   = nullptr;
  SDL_Renderer *renderer = nullptr;

  float main_scale = 1.;
};

struct ImGuiState {
  bool   show_demo_window    = true;
  bool   show_another_window = false;
  ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

struct AppState {
  SDL_State                                     sdl;
  ImGuiState                                    imgui;
  std::unordered_map<std::type_index, std::any> components;

  template<typename T>
  T &get() {
    auto &map = components;
    auto  id  = std::type_index(typeid(T));
    auto  it  = map.find(id);
    if (it == map.end()) {
      map[id] = T{};
      it      = map.find(id);
    }
    return std::any_cast<T &>(it->second);
  }
};

bool SDL_init(AppState *state) {
  auto &window   = state->sdl.window;
  auto &renderer = state->sdl.renderer;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    logBuffer.println("Error: SDL_Init(): {}", SDL_GetError());
    return false;
  }

  auto &main_scale = state->sdl.main_scale =
    SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags =
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  if (!SDL_CreateWindowAndRenderer(
        "Hello World",
        static_cast<int>(1280 * main_scale),
        static_cast<int>(720 * main_scale),
        window_flags,
        &window,
        &renderer
      )) {
    logBuffer.println(
      "Couldn't create window and renderer: {}", SDL_GetError()
    );
    return false;
  }
  SDL_SetRenderVSync(renderer, 1);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);
  return true;
}

bool ImGuiInit(AppState *state) {
  auto &window     = state->sdl.window;
  auto &renderer   = state->sdl.renderer;
  auto &main_scale = state->sdl.main_scale;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  auto &io        = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;
  return true;
}

struct THREAD_BASIC_INFORMATION {
  NTSTATUS  ExitStatus;
  PVOID     TebBaseAddress;
  CLIENT_ID ClientId;
  KAFFINITY AffinityMask;
  KPRIORITY Priority;
  KPRIORITY BasePriority;
};

struct TEB64 {
  uint64_t Reserved1;
  uint64_t StackBase;
  uint64_t StackLimit;
};

bool checkIf32Bits(libmem::Process const &process) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  BOOL isWow64{};
  IsWow64Process(hProcess, &isWow64);
  CloseHandle(hProcess);
  return isWow64;
}

std::string to_utf8(std::wstring const &wstr) {
  if (wstr.empty()) return "";
  int size = WideCharToMultiByte(
    CP_UTF8,
    0,
    wstr.c_str(),
    static_cast<int>(wstr.size()),
    nullptr,
    0,
    nullptr,
    nullptr
  );
  std::string utf8_str(size, 0);
  WideCharToMultiByte(
    CP_UTF8,
    0,
    wstr.c_str(),
    static_cast<int>(wstr.size()),
    &utf8_str[0],
    size,
    nullptr,
    nullptr
  );
  return utf8_str;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  auto &pidList =
    *reinterpret_cast<std::vector<std::pair<DWORD, std::wstring>> *>(lParam);

  DWORD processId = 0;
  GetWindowThreadProcessId(hwnd, &processId);

  int titleLength = GetWindowTextLengthW(hwnd);
  if (titleLength <= 0) { return TRUE; }

  std::wstring title(titleLength + 1, L'\0');
  GetWindowTextW(hwnd, &title[0], titleLength + 1);
  title.resize(titleLength);

  pidList.emplace_back(processId, title);
  return TRUE;
}

std::vector<std::wstring> GetWindowTitles(libmem::Process const &process) {
  std::vector<std::pair<libmem::Pid, std::wstring>> allWindows;
  EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&allWindows));

  std::vector<std::wstring> titles;
  for (auto const &[pid, title]: allWindows) {
    if (pid == process.pid) { titles.emplace_back(title); }
  }
  logBuffer.println("Found {} windows:", titles.size());
  for (auto const &title: titles) { logBuffer.println("{}", to_utf8(title)); }
  return titles;
}

void print_hex(std::vector<uint8_t> hex) {
  logBuffer.println("Data size: {} bytes:", hex.size());
  logBuffer << "Hex: ";
  for (size_t i = 0; i < hex.size(); ++i) {
    logBuffer.append("{:02X} ", hex[i]);
    if ((i + 1) % 16 == 0 && i < hex.size() - 1) {
      logBuffer.flush();
      logBuffer.append("     ");
    }
  }
  logBuffer.flush();
}

std::optional<std::wstring> getGameTitle(libmem::Process const &process) {
  auto titles = GetWindowTitles(process);
  if (titles.empty()) {
    logBuffer.println("No windows found for PID: {}", process.pid);
    return std::nullopt;
  }
  for (auto const &title: titles) {
    if (title.contains(L".ver")) { return title; }
  }
  logBuffer.println("No window seems to be the game");
  return std::nullopt;
}

std::optional<libmem::Address> getThreadStack0(
  libmem::Process const      &process,
  libmem::Address             address,
  std::vector<uint8_t> const &pattern
) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  MEMORY_BASIC_INFORMATION mbi;

  auto     base   = address;
  uint64_t offset = 0;
  while (offset <= 0x10000000) {
    if (VirtualQueryEx(
          hProcess, reinterpret_cast<LPCVOID>(base), &mbi, sizeof(mbi)
        )) {
      // logBuffer.println(
      //   "Region Base: {}, Size: {:#x}, Type: {:#x}, Protect: {:#x}",
      //   mbi.BaseAddress,
      //   mbi.RegionSize,
      //   mbi.Type,
      //   mbi.Protect
      // );
      if (mbi.Protect == 0x04) {
        if (auto result =
              libmem::DataScan(&process, pattern, base, mbi.RegionSize)) {
          return *result - 0x04;
        }
      }
      base    = reinterpret_cast<uint64_t>(mbi.BaseAddress) + mbi.RegionSize;
      offset += mbi.RegionSize;
    } else {
      logBuffer.println("VirtualQueryEx failed: {}", GetLastError());
      CloseHandle(hProcess);
      return std::nullopt;
    }
  }

  return std::nullopt;
}
}  // namespace leprac

using namespace leprac;

SDL_AppResult SDL_AppInit(void **appstate, int, char **) {
  auto state = new AppState();
  *appstate  = state;

  if (!SDL_init(state)) return SDL_APP_FAILURE;
  if (!ImGuiInit(state)) return SDL_APP_FAILURE;

  SetConsoleOutputCP(CP_UTF8);

  auto process = libmem::FindProcess("Le03.exe");
  if (!process) {
    logBuffer.println("Process not found");
    return SDL_APP_FAILURE;
  }
  logBuffer.println("Pid: {}", process->pid);
  logBuffer.println("Process is {}", checkIf32Bits(*process) ? "x32" : "x64");

  auto thread = libmem::GetThread(&*process);
  logBuffer.println("Tid: {}", thread->tid);
  THREAD_BASIC_INFORMATION tbi{};

  auto hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, thread->tid);
  if (!hThread) {
    logBuffer.println("OpenThread failed, error: {}", GetLastError());
    return SDL_APP_CONTINUE;
  }
  auto status = NtQueryInformationThread(
    hThread, static_cast<THREADINFOCLASS>(0), &tbi, sizeof(tbi), nullptr
  );
  CloseHandle(hThread);
  if (status != 0) {
    logBuffer.println(
      "NtQueryInformationThread failed with status: {:#x}", status
    );
    return SDL_APP_CONTINUE;
  }
  logBuffer.println("TEB Base Address: {}", tbi.TebBaseAddress);

  auto tebData = libmem::ReadMemory<TEB64>(
    &*process, reinterpret_cast<libmem::Address>(tbi.TebBaseAddress)
  );
  if (!tebData) {
    logBuffer.println("ReadMemory failed, error: {}", GetLastError());
    return SDL_APP_CONTINUE;
  }
  uint64_t stackBase  = tebData->StackBase;
  uint64_t stackLimit = tebData->StackLimit;
  logBuffer.println("StackBase: {:#x}", stackBase);
  logBuffer.println("StackLimit: {:#x}", stackLimit);

  std::vector<uint8_t> literal_prefix(20, 0);
  literal_prefix[0] = 0x0C;

  auto targetTitle    = getGameTitle(*process);
  auto titleBytes     = reinterpret_cast<uint8_t const *>(targetTitle->c_str());
  auto titleByteCount = targetTitle->size() * sizeof(wchar_t);

  std::vector<uint8_t> combined;
  combined.reserve(literal_prefix.size() + titleByteCount);
  combined.insert(combined.end(), literal_prefix.begin(), literal_prefix.end());
  combined.insert(combined.end(), titleBytes, titleBytes + titleByteCount);

  print_hex(combined);

  if (auto result = getThreadStack0(*process, tebData->StackLimit, combined)) {
    logBuffer.println("Target address: {:#x}", *result);
  } else {
    logBuffer.println("Failed to get thread stack");
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto state = static_cast<AppState *>(appstate);
  ImGui_ImplSDL3_ProcessEvent(event);
  switch (event->type) {
  case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    if (event->window.windowID == SDL_GetWindowID(state->sdl.window))
      return SDL_APP_SUCCESS;
    break;
  default: break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto state = static_cast<AppState *>(appstate);

  if (SDL_GetWindowFlags(state->sdl.window) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return SDL_APP_CONTINUE;
  }

  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  auto &m                   = state->imgui;
  auto &show_demo_window    = m.show_demo_window;
  auto &show_another_window = m.show_another_window;
  auto &clear_color         = m.clear_color;
  auto &io                  = ImGui::GetIO();
  auto  renderer            = state->sdl.renderer;

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to create a named window.
  {
    static float f       = 0.0f;
    static int   counter = 0;

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &show_demo_window);
    ImGui::Checkbox("Another Window", &show_another_window);
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", reinterpret_cast<float *>(&clear_color));
    if (ImGui::Button("Button")) counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    ImGui::Text(
      "Application average %.3f ms/frame (%.1f FPS)",
      1000.0f / io.Framerate,
      io.Framerate
    );
    ImGui::End();
  }

  if (show_another_window) {
    ImGui::Begin("Another Window", &show_another_window);
    // Pass a pointer to our bool variable (the window will have a closing
    // button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) show_another_window = false;
    ImGui::End();
  }

  ImGui::Render();
  SDL_SetRenderScale(
    renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y
  );
  SDL_SetRenderDrawColorFloat(
    renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w
  );
  SDL_RenderClear(renderer);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
