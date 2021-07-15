#include <Drawer/Drawer.h>
#include <Drawer/ArrowShape.h>
#include <Drawer/ShadowAndLight.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
  std::shared_ptr<ArrowShape> arrow(new ArrowShape(60,0.1,0.2));
  arrow->setArrow(Eigen::Matrix<GLfloat,3,1>(-0.5,-0.5,-0.5),Eigen::Matrix<GLfloat,3,1>(0.5,0.5,0.5));
  arrow->setColorAmbient(GL_TRIANGLES,1,1,1);
  drawer.addShape(arrow);

#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem();
  drawer.getLight().lightSz()=10;
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight().addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z),
                                   Eigen::Matrix<GLfloat,3,1>(.05,.05,.05),
                                   Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                   Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  drawer.mainLoop();
  return 0;
}
