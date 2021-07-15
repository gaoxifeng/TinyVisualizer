#include <Drawer/Drawer.h>
#include <Drawer/VisibilityScore.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  VisibilityScore score(10);
  score.debugVisibility();
  return 0;
}
