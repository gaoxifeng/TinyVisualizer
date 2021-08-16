#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/VisibilityScore.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<VisibilityScore> score(new VisibilityScore(10));
  score->debugVisibility();
  return 0;
}
