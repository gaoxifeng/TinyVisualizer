#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/RayCaster.h>
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/CompositeShape.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  MeshVisualizer vis("high-poly/OBJ/SM_M2_Build_Apartment_01.obj");
  //MeshVisualizer vis("low-poly/SM_M2_Build_Apartment_01.obj");
  RayCaster ray(vis);

  drawer.addLightSystem(1024,10,true);
  drawer.addShape(vis.getShape());
  drawer.addShape(ray.drawRay(8,-Eigen::Matrix<GLdouble,3,1>::UnitY()));
  drawer.mainLoop();
  return 0;
}
