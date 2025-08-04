#ifndef LAUNCHER_H
#define LAUNCHER_H
#include <d3d11.h>
#include <Windows.h>

#include "game.h"

namespace leprac {
class Launcher {
 public:
  static void init();
  static void deinit();

  static void run();
  static void UI();

 private:
  static void initWindow();
  static void initImGui();

  static bool CreateDeviceD3D(HWND hWnd);
  static void CleanupDeviceD3D();
  static void CreateRenderTarget();
  static void CleanupRenderTarget();

  static inline Game game_{};

  static inline ID3D11Device*           d3dDevice{};
  static inline ID3D11DeviceContext*    d3dDeviceContext{};
  static inline IDXGISwapChain*         SwapChain{};
  static inline bool                    SwapChainOccluded{};
  static inline ID3D11RenderTargetView* mainRenderTargetView{};

  static inline WNDCLASSEX wc{};
  static inline HWND       hwnd{};

  static inline float mainScale{1};
};
}  // namespace leprac
#endif
