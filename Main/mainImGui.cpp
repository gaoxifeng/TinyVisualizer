#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/ArrowShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <TinyVisualizer/FileDialog.h>
#include <iostream>
#include <imgui.h>

using namespace DRAWER;

int main(int argc,char** argv) {
#ifdef IMGUI_SUPPORT
  Drawer drawer(argc,argv);
  std::shared_ptr<ArrowShape> arrow(new ArrowShape(60,0.1,0.2));
  arrow->setArrow(Eigen::Matrix<GLfloat,3,1>(-0.5,-0.5,-0.5),Eigen::Matrix<GLfloat,3,1>(0.5,0.5,0.5));
  arrow->setColorAmbient(GL_TRIANGLES,1,1,1);
  drawer.addShape(arrow);

#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem();
  drawer.getLight()->lightSz(10);
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z),
                                    Eigen::Matrix<GLfloat,3,1>(.05,.05,.05),
                                    Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                    Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  float from[3]= {-.5,-.5,-.5},to[3]= {.5,.5,.5};
  drawer.addPlugin(std::shared_ptr<Plugin>(new ImGuiPlugin([&]() {
    if(ImGui::BeginMainMenuBar()) {
      if(ImGui::BeginMenu("file")) {
        if(ImGui::MenuItem("open"))
          std::cout << fileDialogOpen() << std::endl;
        if(ImGui::MenuItem("save"))
          std::cout << fileDialogSave() << std::endl;
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
    ImGui::Begin("Arrow Direction");
    ImGui::DragFloat3("from",from,.01,-1,1);
    ImGui::DragFloat3("to",to,.01,-1,1);
    ImGui::End();
  })));
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>& root) {
    arrow->setArrow(Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(from),Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(to));
  });
  drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
  drawer.mainLoop();
#endif
  return 0;
}
