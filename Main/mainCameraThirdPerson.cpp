#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <TinyVisualizer/imgui/ImGuiHelpers.h>
#include <TinyVisualizer/CameraThirdPerson.h>
#include <iostream>

using namespace DRAWER;

int main() {

    int argc = 9;
    char* argv[] = { "", "--headless","0", "--width", "1024", "--height", "800","--title ","CameraTest" };
    DRAWER::Drawer drawer(argc, argv);
    std::shared_ptr<DRAWER::ImGuiPlugin> menup(new DRAWER::ImGuiPlugin);
    menup->callbackDrawCustomWindow = [&]() {
        // Define next window position + size
        ImGui::SetNextWindowPos(ImVec2(180.f * menup->menuScaling(), 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(180, -1.0f), ImVec2(180, -1.0f));
        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::PushItemWidth(-80);

        float w = ImGui::GetContentRegionAvailWidth();
        float p = ImGui::GetStyle().FramePadding.x;
        if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0))) {
            drawer.addCamera3D(90, Eigen::Matrix<GLfloat, 3, 1>(0, 1, 0));

            std::shared_ptr<DRAWER::Bullet3DShape> shapeTA(new DRAWER::Bullet3DShape);
            std::shared_ptr<DRAWER::MeshShape> sphere = DRAWER::makeSphere(8, true, 0.25f);
            shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat, 3, 1>(-0.5f, 0, 0));
            shapeTA->addShape(sphere);
            drawer.addShape(shapeTA);

            Eigen::Matrix<GLfloat, 3, 1> sz(0.1f, 2, 2);
            std::shared_ptr<DRAWER::Bullet3DShape> shapeTB(new DRAWER::Bullet3DShape);
            std::shared_ptr<DRAWER::MeshShape> box = DRAWER::makeBox(1, true, sz);
            shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat, 3, 1>(-2.0f, 0, 0));
            shapeTB->addShape(box);
            drawer.addShape(shapeTB);
        }
        if (ImGui::Button("Second", ImVec2(-1, 0))) {

            std::shared_ptr<DRAWER::Bullet3DShape> shapeTA(new DRAWER::Bullet3DShape);
            std::shared_ptr<DRAWER::MeshShape> sphere = DRAWER::makeSphere(8, true, 0.25f);
            shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat, 3, 1>(-0.5f, 0, 0));
            shapeTA->addShape(sphere);
            drawer.addShape(shapeTA);

            Eigen::Matrix<GLfloat, 3, 1> sz(0.1f, 2, 2);
            std::shared_ptr<DRAWER::Bullet3DShape> shapeTB(new DRAWER::Bullet3DShape);
            std::shared_ptr<DRAWER::MeshShape> box = DRAWER::makeBox(1, true, sz);
            shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat, 3, 1>(-2.0f, 0, 0));
            shapeTB->addShape(box);
            drawer.addShape(shapeTB);
        }
 
        ImGui::PopItemWidth();
        ImGui::End();
    };
#define USE_LIGHT
#ifdef USE_LIGHT
    GLfloat theta = 0;
    drawer.addLightSystem();
    Eigen::Matrix<GLfloat, 4, 4> T;
    drawer.getLight().lightSz() = 10;
    drawer.getLight().addLight(Eigen::Matrix<GLfloat, 3, 1>(0, 0, 0.2f),
        Eigen::Matrix<GLfloat, 3, 1>(1, 1, 1),
        Eigen::Matrix<GLfloat, 3, 1>(1, 1, 1),
        Eigen::Matrix<GLfloat, 3, 1>(0, 0, 1));
    drawer.getLight().addLight(Eigen::Matrix<GLfloat, 3, 1>(0, 0, -0.2f),
        Eigen::Matrix<GLfloat, 3, 1>(1, 1, 1),
        Eigen::Matrix<GLfloat, 3, 1>(1, 1, 1),
        Eigen::Matrix<GLfloat, 3, 1>(1, 0, 0));
#endif
    drawer.addPlugin(menup);
    drawer.mainLoop();
    return 0;
}
