#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/VisibilityScore.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <TinyVisualizer/imgui/ImGuiHelpers.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  DRAWER::ImGuiPlugin *menup = new DRAWER::ImGuiPlugin;

  // Draw additional windows
  menup->callbackDrawCustomWindow = [&]()
  {
      // Define next window position + size
      ImGui::SetNextWindowPos(ImVec2(180.f * menup->menuScaling(), 0), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSizeConstraints(ImVec2(180, -1.0f), ImVec2(180, -1.0f));

      ImGui::Begin("LOD", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);

      ImGui::PushItemWidth(-80);

      float w = ImGui::GetContentRegionAvailWidth();
      float p = ImGui::GetStyle().FramePadding.x;
      if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
      {
          ;
      }
      ImGui::PopItemWidth();
      ImGui::End();
  };

  std::shared_ptr < DRAWER::Plugin> menu = std::shared_ptr < DRAWER::ImGuiPlugin> (menup);

  drawer.addPlugin(menu);


  drawer.mainLoop();

  VisibilityScore score(10);
  score.debugVisibility();

  return 0;
}
