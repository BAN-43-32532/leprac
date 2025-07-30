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

#include "../src/logger.h"
#include "../src/UI.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "battery/embed.hpp"
#include "imgui.h"
#include "libmem/libmem.hpp"
#include "module/launcher.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "util.h"

namespace leprac {
struct SDL_State {
  SDL_Window   *window   = nullptr;
  SDL_Renderer *renderer = nullptr;

  float main_scale = 1.;
};

struct AppState {
  SDL_State                                     sdl;
  Launcher                                      launcher{};
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
}  // namespace leprac

using namespace leprac;

SDL_AppResult SDL_AppInit(void **appstate, int, char **) {
  auto state = new AppState();
  *appstate  = state;
  SetConsoleOutputCP(CP_UTF8);

  if (!SDL_init(state)) return SDL_APP_FAILURE;
  if (!ImGuiInit(state)) return SDL_APP_FAILURE;

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

  // state->launcher.UI();

  auto &m                   = state->imgui;
  auto &show_demo_window    = m.show_demo_window;
  auto &show_another_window = m.show_another_window;
  auto &clear_color         = m.clear_color;
  auto &io                  = ImGui::GetIO();
  auto  renderer            = state->sdl.renderer;
  //
  // 1. Show the big demo window (Most of the sample code is in
  // // ImGui::ShowDemoWindow()! You can browse its code to learn more about
  // Dear
  // // ImGui!).
  // if (show_demo_window)
  ImGui::ShowDemoWindow(&show_demo_window);
  //
  // // 2. Show a simple window that we create ourselves. We use a Begin/End
  // pair
  // // to create a named window.
  // {
  //   static float f       = 0.0f;
  //   static int   counter = 0;
  //
  //   ImGui::Begin("Hello, world!");
  //   ImGui::Text("This is some useful text.");
  //   ImGui::Checkbox("Demo Window", &show_demo_window);
  //   ImGui::Checkbox("Another Window", &show_another_window);
  //   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  //   ImGui::ColorEdit3("clear color", reinterpret_cast<float
  //   *>(&clear_color)); if (ImGui::Button("Button")) counter++;
  //   ImGui::SameLine();
  //   ImGui::Text("counter = %d", counter);
  //   ImGui::Text(
  //     "Application average %.3f ms/frame (%.1f FPS)",
  //     1000.0f / io.Framerate,
  //     io.Framerate
  //   );
  //   ImGui::End();
  // }
  //
  // if (show_another_window) {
  //   ImGui::Begin("Another Window", &show_another_window);
  //   // Pass a pointer to our bool variable (the window will have a closing
  //   // button that will clear the bool when clicked)
  //   ImGui::Text("Hello from another window!");
  //   if (ImGui::Button("Close Me")) show_another_window = false;
  //   ImGui::End();
  // }
  //
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
