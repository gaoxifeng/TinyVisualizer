#include <TextureBaker/MeshVisualizer.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  MeshVisualizer vis("high-poly/OBJ/SM_M2_Build_Apartment_01.obj");
  drawer.addShape(vis.getShape());
  drawer.mainLoop();
  return 0;
}
