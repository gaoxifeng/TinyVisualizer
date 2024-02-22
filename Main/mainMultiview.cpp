#include <glad/gl.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/MultiDrawer.h>
#include <TinyVisualizer/MakeTexture.h>
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
  MultiDrawer drawer(argc,argv);
  drawer.setViewportLayout(1,2);
  std::shared_ptr<Texture> grid=drawGrid();
  //first view
  {
    std::shared_ptr<Drawer> view1=drawer.getDrawer(0,0);
    view1->setBackground(drawChecker());
    view1->setBackground(.5,.5,.5);
    view1->addCamera2D(10);
    view1->setDrawFunc([&]() {
      glActiveTexture(GL_TEXTURE0);
      grid->begin();
      glActiveTexture(GL_TEXTURE1);
      FBO::screenQuad(grid);
      grid->end();
      glActiveTexture(GL_TEXTURE0);
    });
  }
  //second view
  {
#ifdef IMGUI_SUPPORT
    std::shared_ptr<Drawer> view2=drawer.getDrawer(0,1);
    std::shared_ptr<ArrowShape> arrow(new ArrowShape(60,0.1,0.2));
    arrow->setArrow(Eigen::Matrix<GLfloat,3,1>(-0.5,-0.5,-0.5),Eigen::Matrix<GLfloat,3,1>(0.5,0.5,0.5));
    arrow->setColorAmbient(GL_TRIANGLES,1,1,1);
    view2->addShape(arrow);

#define USE_LIGHT
#ifdef USE_LIGHT
    view2->addLightSystem();
    view2->getLight()->lightSz(10);
    for(int x=-1; x<=1; x+=2)
      for(int y=-1; y<=1; y+=2)
        for(int z=-1; z<=1; z+=2)
          view2->getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z),
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
    view2->setFrameFunc([&](std::shared_ptr<SceneNode>& root) {
      arrow->setArrow(Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(from),Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(to));
    });
    view2->getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(view2->getCamera3D())));
#endif
  }
  drawer.mainLoop();
  return 0;
}
