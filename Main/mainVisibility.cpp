#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/VisibilityScore.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  VisibilityScore score(10);
  score.debugVisibility();
  return 0;
}
