module;
#include <d3d11.h>
#include <Windows.h>
export module leprac.launcher;

export namespace leprac {
class Launcher {
 public:
  void init();
  void UI();

 private:
  bool           CreateDeviceD3D(HWND hWnd);
  void           CleanupDeviceD3D();
  void           CreateRenderTarget();
  void           CleanupRenderTarget();
  LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  ID3D11Device*           d3dDevice{};
  ID3D11DeviceContext*    d3dDeviceContext{};
  IDXGISwapChain*         SwapChain{};
  bool                    SwapChainOccluded{};
  UINT                    ResizeWidth{};
  UINT                    ResizeHeight{};
  ID3D11RenderTargetView* mainRenderTargetView{};

  float mainScale{1};
};
}  // namespace leprac
