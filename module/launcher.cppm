module;
#include <d3d11.h>
#include <Windows.h>
export module leprac.launcher;

export namespace leprac {
class Launcher {
 public:
  void init();
  void deinit();
  void UI();

 private:
  bool           CreateDeviceD3D(HWND hWnd);
  void           CleanupDeviceD3D();
  void           CreateRenderTarget();
  void           CleanupRenderTarget();

  ID3D11Device*           d3dDevice{};
  ID3D11DeviceContext*    d3dDeviceContext{};
  IDXGISwapChain*         SwapChain{};
  bool                    SwapChainOccluded{};
  ID3D11RenderTargetView* mainRenderTargetView{};

  WNDCLASSEXW wc{};
  HWND hwnd{};

  float mainScale{1};
};
}  // namespace leprac
