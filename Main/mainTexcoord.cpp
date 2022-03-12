#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TextureBaker/MeshVisualizer.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  MeshVisualizer vis("high-poly/OBJ/SM_M2_Build_Apartment_01.obj");
  //MeshVisualizer vis("low-poly/SM_M2_Build_Apartment_01.obj");

  drawer.addCamera2D(10);
  drawer.addLightSystem(0);
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(0,0,100));
  drawer.addShape(vis.getTextureCoordShape());
  drawer.mainLoop();
  return 0;
}
