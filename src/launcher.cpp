#include "launcher.h"
#include "imgui.h"

void Launcher::UI() {
  // 1. 设置全局样式（可选，调整分割线、间距等）
  ImGuiStyle& style = ImGui::GetStyle();
  style.ItemSpacing = ImVec2(4, 4);               // 控件间距
  style.FrameBorderSize = 0.5f;                   // 边框宽度
  style.ItemInnerSpacing = ImVec2(8, 4);          // 内部间距
  style.SeparatorTextBorderSize = 0.5f;           // 分割线粗细

  // 2. 开始一个无边框的全屏窗口（或你自己的窗口）
  ImGui::Begin("##MainWindow", nullptr);

  // 3. 顶部栏目（也可以用 MenuBar，或自定义一个子区域）
  if (ImGui::BeginMenuBar())
  {
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
    for (int i = 0; i < 20; ++i)
    {
      ImGui::Text("这是第 %d 行内容，用于演示分割线", i + 1);

      // 如果不是最后一行，画条细的横线做分割
      if (i != 19)
      {
        // 默认 Separator 已经很细，如需更细可自定义绘制
        ImGui::Separator();
      }
    }
  }
  ImGui::EndChild();

  ImGui::End();
}
