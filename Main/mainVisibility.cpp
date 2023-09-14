#include <glad/gl.h>
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/VisibilityScore.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<VisibilityScore> score(new VisibilityScore(10,GL_R32F));
  score->debugVisibility(drawer);
  //score->debugVisibility();
  return 0;
}
