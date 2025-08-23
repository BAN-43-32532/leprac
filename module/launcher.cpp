module;
#include <d3d11.h>
#include <Windows.h>
export module launcher;
import common;

class Logger;

export namespace leprac {
class Launcher {
 public:
  Launcher() = delete;
  static void init();
  static void deinit();

  static void run();

  [[nodiscard]] static auto mainScale() { return mainScale_; }

  [[nodiscard]] static auto& resizeWidth() { return resizeWidth_; }

  [[nodiscard]] static auto& resizeHeight() { return resizeHeight_; }

 private:
  static void initWindow();
  static void initImGui();

  static bool createDeviceD3D();
  static void cleanupDeviceD3D();
  static void createRenderTarget();
  static void cleanupRenderTarget();

  static void handleResize();

  // static inline Game game_{};

  static inline ID3D11Device*           d3dDevice{};
  static inline ID3D11DeviceContext*    d3dDeviceContext{};
  static inline IDXGISwapChain*         swapChain{};
  static inline bool                    swapChainOccluded{};
  static inline ID3D11RenderTargetView* mainRenderTargetView{};

  static inline int resizeWidth_{};
  static inline int resizeHeight_{};

  static inline WNDCLASSEX wc_{};
  static inline HWND       hwnd_{};

  static inline float mainScale_{1};
};
}  // namespace leprac
