#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/SkinnedMesh.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  Eigen::Matrix<GLfloat,4,4> scale=Eigen::Matrix<GLfloat,4,4>::Identity();
  std::shared_ptr<SkinnedMeshShape> shapeM(new SkinnedMeshShape(argv[1]));
  Eigen::Matrix<GLfloat,6,1> bb=shapeM->getBB();
  scale.diagonal().segment<3>(0).array()=1/(bb.segment<3>(3)-bb.segment<3>(0)).norm();
  shapeM->setLocalTransform(scale*shapeM->getLocalTransform());
  shapeM->setColor(GL_TRIANGLES,1,1,1);
  drawer.addShape(shapeM);

  //intersect
  std::shared_ptr<Bullet3DShape> shapeI(new Bullet3DShape);
  shapeI->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(0,0,0));
  shapeI->addShape(makeSphere(8,true,0.01f));
  shapeI->setColor(GL_TRIANGLES,0,0,1);
  shapeI->setUseLight(false);
  drawer.addShape(shapeI);

  //light
#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(0);
  drawer.getLight()->lightSz(10);
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z)*2,
                                    Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                    Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                    Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif

  //camera
  drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
  drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
  drawer.setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
      double x=0,y=0;
      GLfloat IAlpha=1;
      std::shared_ptr<Shape> IShape;
      glfwGetCursorPos(wnd,&x,&y);
      Eigen::Matrix<GLfloat,6,1> ray=drawer.getCameraRay(x,y);
      if(drawer.rayIntersect(ray,IShape,IAlpha)) {
        std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
        shapeI->setLocalTranslate(ray.segment<3>(0)+ray.segment<3>(3)*IAlpha);
      } else std::cout << "No intersection!" << std::endl;
    }
  });
  drawer.mainLoop();
  return 0;
}
