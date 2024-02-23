#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/TerrainShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/CameraExportPlugin.h>
#include <TinyVisualizer/CaptureGIFPlugin.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <iostream>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  drawer.addPlugin(std::shared_ptr<Plugin>(new CameraExportPlugin(GLFW_KEY_2,GLFW_KEY_3,"camera.dat")));
  drawer.addPlugin(std::shared_ptr<Plugin>(new CaptureGIFPlugin(GLFW_KEY_1,"record.gif",drawer.FPS())));
#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(2048,20);
  drawer.getLight()->lightSz(10);
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(10,10,10),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  
  std::shared_ptr<Texture> checker=drawChecker();
  for(int x=0;x<5;x++)
    for(int y=0;y<5;y++)
      for(int z=0;z<5;z++) {
        std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
        std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
        box->setTextureDiffuse(checker);
        box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,z));
        shapeTB->addShape(box);
        drawer.addShape(shapeTB);
      }
  
  std::shared_ptr<Shape> IShape;
  drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
  drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
  drawer.addPlugin(std::shared_ptr<Plugin>(new ImGuiPlugin([&]() {
    drawer.getCamera3D()->getManipulator()->imGuiCallback();
  })));
  drawer.setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
      double x=0,y=0;
      GLfloat IAlpha=1;
      glfwGetCursorPos(wnd,&x,&y);
      Eigen::Matrix<GLfloat,6,1> ray=drawer.getCameraRay(x,y);
      if(IShape)
        IShape->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
      if(drawer.rayIntersect(ray,IShape,IAlpha)) {
        std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
        IShape->setColorDiffuse(GL_TRIANGLES,1,0,0);
      } else std::cout << "No intersection!" << std::endl;
    }
  });
  drawer.mainLoop();
  return 0;
}
