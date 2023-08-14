#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/CaptureMPEG2Plugin.h>
#include <TinyVisualizer/BezierCurveShape.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <TinyVisualizer/Camera3D.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  drawer.addPlugin(std::shared_ptr<Plugin>(new CaptureMPEG2Plugin(GLFW_KEY_1,"record.mp2",drawer.FPS())));
  std::shared_ptr<Texture> checker=drawChecker();

  std::shared_ptr<Bullet3DShape> shapeTA(new Bullet3DShape);
  std::shared_ptr<MeshShape> sphere=makeSphere(8,true,0.25f);
  sphere->setTextureDiffuse(checker);
  shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(-0.5f,0,0));
  shapeTA->addShape(sphere);
  drawer.addShape(shapeTA);

  Eigen::Matrix<GLfloat,3,1> sz(0.1f,2,2);
  std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
  std::shared_ptr<MeshShape> box=makeBox(1,true,sz);
  box->setCastShadow(false);
  box->setTextureDiffuse(checker);
  shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(-2.0f,0,0));
  shapeTB->addShape(box);
  drawer.addShape(shapeTB);

  //points are not affected by lighting
  std::shared_ptr<MeshShape> points(new MeshShape);
  for(int x=0,off=0; x<5; x++)
    for(int y=0; y<5; y++)
      for(int z=0; z<5; z++) {
        points->addVertex(Eigen::Matrix<GLfloat,3,1>(x,y,z)*0.1f);
        points->addIndexSingle(off++);
      }
  points->setMode(GL_POINTS);
  points->setColorDiffuse(GL_POINTS,.6,.3,.3);
  points->setPointSize(5);
  drawer.addShape(points);

  //light
  drawer.addLightSystem(2048,20);
  drawer.getLight()->lightSz(10);
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(0,0,0),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(0, .2,0),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(0,-.2,0),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(1,0,0),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
  bool sim=false;
  GLfloat theta=0;
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>) {
    Eigen::Matrix<GLfloat,4,4> T;
    T.setIdentity();
    T(0,0)=std::cos(theta);
    T(0,1)=-std::sin(theta);
    T(1,0)=std::sin(theta);
    T(1,1)=std::cos(theta);
    T.block<3,1>(0,3)=T.block<3,3>(0,0)*Eigen::Matrix<GLfloat,3,1>(-0.5f,0,0);
    shapeTA->setLocalTransform(T);
    shapeTA->setColorAmbient(GL_TRIANGLES,0,.05,0);
    shapeTA->setColorDiffuse(GL_TRIANGLES,.2,.2,.2);
    shapeTA->setColorSpecular(GL_TRIANGLES,.5,.5,.5);
    shapeTA->setShininess(GL_TRIANGLES,10.);
    T.block<3,1>(0,3)=T.block<3,3>(0,0)*Eigen::Matrix<GLfloat,3,1>(-2.0f,0,0);
    shapeTB->setLocalTransform(T);
    shapeTB->setColorAmbient(GL_TRIANGLES,0,.05,0);
    shapeTB->setColorDiffuse(GL_TRIANGLES,.2,.2,.2);
    shapeTB->setColorSpecular(GL_TRIANGLES,.5,.5,.5);
    shapeTB->setShininess(GL_TRIANGLES,10.);
    if(sim)
      theta=std::fmod(theta+M_PI*2/360.0f,M_PI*2);
  });
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
  });
  drawer.addPlugin(std::shared_ptr<Plugin>(new ImGuiPlugin([&]() {
    drawer.getCamera3D()->getManipulator()->imGuiCallback();
  })));
  drawer.mainLoop();
  return 0;
}
