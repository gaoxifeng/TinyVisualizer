#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/SkinnedMeshShape.h>
#include <TinyVisualizer/TrackballCameraManipulator.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <imgui.h>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace DRAWER;

int main(int argc, char **argv) {
#ifdef ASSIMP_SUPPORT
#ifdef IMGUI_SUPPORT
  Drawer drawer(argc,argv);
  Eigen::Matrix<GLfloat,4,4> scale=Eigen::Matrix<GLfloat,4,4>::Identity();
  Eigen::Matrix<GLfloat,4,4> translate=Eigen::Matrix<GLfloat,4,4>::Identity();
  std::shared_ptr<SkinnedMeshShape> shapeM(new SkinnedMeshShape(argv[1]));
  std::cout << "#animation=" << shapeM->nrAnimation() << " duration=";
  for(GLuint i=0; i<shapeM->nrAnimation(); i++)
    std::cout << shapeM->duration(i) << " ";
  std::cout << std::endl;

  //mesh
  Eigen::Matrix<GLfloat,6,1> bb=shapeM->getBB();
  translate.col(3).segment<3>(0)=-(bb.segment<3>(3)+bb.segment<3>(0))/2;
  scale.diagonal().segment<3>(0).array()=1/(bb.segment<3>(3)-bb.segment<3>(0)).norm();
  shapeM->setLocalTransform(scale*translate*shapeM->getLocalTransform());
  shapeM->setColorDiffuse(GL_TRIANGLES,1,1,1);
  shapeM->setColorSpecular(GL_TRIANGLES,1,1,1);
  shapeM->setShininess(GL_TRIANGLES,50);
  drawer.addShape(shapeM);

  //intersect
  std::shared_ptr<Bullet3DShape> shapeI(new Bullet3DShape);
  shapeI->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(0,0,0));
  shapeI->addShape(makeSphere(8,true,0.01f));
  shapeI->setColorDiffuse(GL_TRIANGLES,0,0,1);
  shapeI->setUseLight(false);
  drawer.addShape(shapeI);

  //light
#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.setBackground(0,0,0);
  drawer.addLightSystem(0);
  drawer.getLight()->lightSz(10);
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z)*2,
                                    Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                    Eigen::Matrix<GLfloat,3,1>(.5,.5,.5),
                                    Eigen::Matrix<GLfloat,3,1>(.5,.5,.5));
#endif

  //camera
  drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
  drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new TrackballCameraManipulator(drawer.getCamera3D())));
  drawer.setMouseFunc([&](GLFWwindowPtr wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
      double x=0,y=0;
      GLfloat IAlpha=1;
      std::shared_ptr<Shape> IShape;
      glfwGetCursorPos(wnd._ptr,&x,&y);
      Eigen::Matrix<GLfloat,6,1> ray=drawer.getCameraRay(x,y);
      if(drawer.rayIntersect(ray,IShape,IAlpha)) {
        std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
        shapeI->setLocalTranslate(ray.segment<3>(0)+ray.segment<3>(3)*IAlpha);
      } else std::cout << "No intersection!" << std::endl;
    }
  });
  GLint index=0;
  GLfloat time=0.;
  bool play=false;
  if(shapeM->nrAnimation()>0) {
    drawer.addPlugin(std::shared_ptr<Plugin>(new ImGuiPlugin([&]() {
      ImGui::Begin("Skinned Animation");
      bool changedIndex=ImGui::SliderInt("animation index",&index,0,shapeM->nrAnimation()-1);
      bool changedTime=ImGui::SliderFloat("time",&time,0,shapeM->duration(index));
      ImGui::Checkbox("play",&play);
      if(play)
        time+=1.0f/drawer.FPS();
      if(changedIndex || changedTime || play)
        shapeM->setAnimatedFrame(index,time);
      ImGui::End();
    })));
  }
  drawer.mainLoop();
#endif
#endif
  return 0;
}
