#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/TerrainShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/CapturePlugin.h>
#include <iostream>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  drawer.addPlugin(std::shared_ptr<Plugin>(new CapturePlugin(GLFW_KEY_1,"record.mp2",drawer.FPS())));
#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(2048,20);
  drawer.getLight().lightSz()=10;
  drawer.getLight().addLight(Eigen::Matrix<GLfloat,3,1>(2,2,2),
                             Eigen::Matrix<GLfloat,3,1>(1,1,1),
                             Eigen::Matrix<GLfloat,3,1>(1,1,1),
                             Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  {
    Eigen::Matrix<GLfloat,-1,-1> height;
    height.resize(64,128);
    for(int x=0; x<height.rows(); x++)
      for(int y=0; y<height.cols(); y++)
        height(x,y)=std::sin(x*0.25f)*std::sin(y*0.25f);
    Eigen::Matrix<GLfloat,2,1> tcMult(1/16.,1/16.);
    Eigen::Matrix<GLfloat,3,1> scale(1/16.,1/8.,1/16.);
    std::shared_ptr<MeshShape> terrain(new TerrainShape(height,1,scale,tcMult));
    terrain->setTexture(drawGrid());
    drawer.addShape(terrain);
  }
  {
    Eigen::Matrix<GLfloat,2,1> tcMult(1/8.,1/8.);
    Eigen::Matrix<GLfloat,6,1> aabb(4,4,4,12,8,8);
    std::shared_ptr<MeshShape> terrain(new TerrainShape([&](GLfloat x,GLfloat y) {
      return std::sin(x*4.)*std::sin(y*4.)/8.;
    },1,aabb,0.1f,tcMult));
    terrain->setTexture(drawGrid());
    drawer.addShape(terrain);
  }
  std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
  std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
  box->setTexture(drawChecker());
  shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(2,0.5,2));
  shapeTB->addShape(box);
  drawer.addShape(shapeTB);

  drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
  drawer.getCamera3D().setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
  drawer.mainLoop();
  return 0;
}
