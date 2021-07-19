#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/VisibilityScore.h>
#include <TinyVisualizer/ImGuiPlugin.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  std::shared_ptr < DRAWER::Plugin> menu = std::make_shared<DRAWER::imgui::ImGuiPlugin>();
  drawer.addPlugin(menu);
  drawer.mainLoop();

  VisibilityScore score(10);
  score.debugVisibility();

  return 0;
}
