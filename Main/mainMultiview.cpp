#include <glad/gl.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MultiDrawer.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <iostream>
#include <imgui.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  MultiDrawer drawer(argc,argv);
  drawer.setViewportLayout(2,2);
  std::shared_ptr<Shape> IShape;
  std::shared_ptr<Texture> grid=drawGrid();
  std::shared_ptr<Texture> checker=drawChecker();
  //view00
  {
    std::shared_ptr<Drawer> view00=drawer.getDrawer(0,0);
    view00->addCamera2D(10);
    view00->clearLight();
    for(int x=0;x<5;x++)
      for(int y=0;y<5;y++) {
        std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
        std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
        box->setTextureDiffuse(checker);
        box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,0));
        shapeTB->addShape(box);
        view00->addShape(shapeTB);
      }
    view00->setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
      if(captured)
        return;
      else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
        double x=0,y=0;
        GLfloat IAlpha=1;
        glfwGetCursorPos(wnd,&x,&y);
        Eigen::Matrix<GLfloat,2,1> pos=view00->getCameraRay(x,y);
        Eigen::Matrix<GLfloat,6,1> ray;
        ray << pos[0],pos[1],1,0,0,-2;
        if(IShape)
          IShape->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        if(view00->rayIntersect(ray,IShape,IAlpha)) {
          std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
          IShape->setColorDiffuse(GL_TRIANGLES,1,0,0);
        } else std::cout << "No intersection!" << std::endl;
      }
    });
  }
  //view11
  {
    std::shared_ptr<Drawer> view11=drawer.getDrawer(1,1);
    view11->addCamera2D(10);
    view11->clearLight();
    for(int x=0;x<5;x++)
      for(int y=0;y<5;y++) {
        std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
        std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
        box->setTextureDiffuse(checker);
        box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,0));
        shapeTB->addShape(box);
        view11->addShape(shapeTB);
      }
    view11->setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
      if(captured)
        return;
      else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
        double x=0,y=0;
        GLfloat IAlpha=1;
        glfwGetCursorPos(wnd,&x,&y);
        Eigen::Matrix<GLfloat,2,1> pos=view11->getCameraRay(x,y);
        Eigen::Matrix<GLfloat,6,1> ray;
        ray << pos[0],pos[1],1,0,0,-2;
        if(IShape)
          IShape->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        if(view11->rayIntersect(ray,IShape,IAlpha)) {
          std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
          IShape->setColorDiffuse(GL_TRIANGLES,1,0,0);
        } else std::cout << "No intersection!" << std::endl;
      }
    });
  }
  //view10
  {
    std::shared_ptr<Drawer> view10=drawer.getDrawer(1,0);
#define USE_LIGHT
#ifdef USE_LIGHT
    view10->addLightSystem(2048,20);
    view10->getLight()->lightSz(10);
    view10->getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(10,10,10),
                                 Eigen::Matrix<GLfloat,3,1>(1,1,1),
                                 Eigen::Matrix<GLfloat,3,1>(1,1,1),
                                 Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  
    for(int x=0;x<5;x++)
      for(int y=0;y<5;y++)
        for(int z=0;z<5;z++) {
          std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
          std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
          box->setTextureDiffuse(checker);
          box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
          shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,z));
          shapeTB->addShape(box);
          view10->addShape(shapeTB);
        }
  
    view10->addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
    view10->getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(view10->getCamera3D())));
    view10->setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
      if(captured)
        return;
      else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
        double x=0,y=0;
        GLfloat IAlpha=1;
        glfwGetCursorPos(wnd,&x,&y);
        Eigen::Matrix<GLfloat,6,1> ray=view10->getCameraRay(x,y);
        if(IShape)
          IShape->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        if(view10->rayIntersect(ray,IShape,IAlpha)) {
          std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
          IShape->setColorDiffuse(GL_TRIANGLES,1,0,0);
        } else std::cout << "No intersection!" << std::endl;
      }
    });
  }
  //view01
  {
    std::shared_ptr<Drawer> view01=drawer.getDrawer(0,1);
#define USE_LIGHT
#ifdef USE_LIGHT
    view01->addLightSystem(2048,20);
    view01->getLight()->lightSz(10);
    view01->getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(10,10,10),
                                 Eigen::Matrix<GLfloat,3,1>(1,1,1),
                                 Eigen::Matrix<GLfloat,3,1>(1,1,1),
                                 Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  
    for(int x=0;x<5;x++)
      for(int y=0;y<5;y++)
        for(int z=0;z<5;z++) {
          std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
          std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
          box->setTextureDiffuse(checker);
          box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
          shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,z));
          shapeTB->addShape(box);
          view01->addShape(shapeTB);
        }
  
    view01->addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
    view01->getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(view01->getCamera3D())));
    view01->setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
      if(captured)
        return;
      else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
        double x=0,y=0;
        GLfloat IAlpha=1;
        glfwGetCursorPos(wnd,&x,&y);
        Eigen::Matrix<GLfloat,6,1> ray=view01->getCameraRay(x,y);
        if(IShape)
          IShape->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
        if(view01->rayIntersect(ray,IShape,IAlpha)) {
          std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
          IShape->setColorDiffuse(GL_TRIANGLES,1,0,0);
        } else std::cout << "No intersection!" << std::endl;
      }
    });
  }
  drawer.mainLoop();
  return 0;
}
