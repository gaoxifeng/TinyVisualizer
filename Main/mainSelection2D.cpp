#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <iostream>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  drawer.addCamera2D(10);
  drawer.clearLight();
  std::shared_ptr<Shape> IShape;
  std::shared_ptr<Texture> checker=drawChecker();
  for(int x=0; x<5; x++)
    for(int y=0; y<5; y++) {
      std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
      std::shared_ptr<MeshShape> box=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(0.25,0.25,0.25));
      box->setTextureDiffuse(checker);
      box->setColorDiffuse(GL_TRIANGLES,.5,.5,.5);
      shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(x,y,0));
      shapeTB->addShape(box);
      drawer.addShape(shapeTB);
    }
  drawer.setMouseFunc([&](GLFWwindowPtr wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
      double x=0,y=0;
      GLfloat IAlpha=1;
      glfwGetCursorPos(wnd._ptr,&x,&y);
      Eigen::Matrix<GLfloat,2,1> pos=drawer.getCameraRay(x,y);
      Eigen::Matrix<GLfloat,6,1> ray;
      ray << pos[0],pos[1],1,0,0,-2;
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
