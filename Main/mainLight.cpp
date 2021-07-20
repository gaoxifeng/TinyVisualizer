#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/BezierCurveShape.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<Texture> checker=drawChecker();

  std::shared_ptr<Bullet3DShape> shapeTA(new Bullet3DShape);
  std::shared_ptr<MeshShape> sphere=makeSphere(8,true,0.25f);
  sphere->setTexture(checker);
  shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(-0.5f,0,0));
  shapeTA->addShape(sphere);
  drawer.addShape(shapeTA);

  Eigen::Matrix<GLfloat,3,1> sz(0.1f,2,2);
  std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
  std::shared_ptr<MeshShape> box=makeBox(1,true,sz);
  box->setCastShadow(false);
  box->setTexture(checker);
  shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(-2.0f,0,0));
  shapeTB->addShape(box);
  drawer.addShape(shapeTB);

  bool sim=false;
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>) {
    Eigen::Matrix<GLfloat,4,4> T;
    GLfloat theta=0;
    T.setIdentity();
    T(0,0)=std::cos(theta);
    T(0,1)=-std::sin(theta);
    T(1,0)=std::sin(theta);
    T(1,1)=std::cos(theta);
    T.block<3,1>(0,3)=T.block<3,3>(0,0)*Eigen::Matrix<GLfloat,3,1>(-0.5f,0,0);
    shapeTA->setLocalTransform(T);
    shapeTA->setColorAmbient(GL_TRIANGLES,0,.05,0);
    shapeTA->setColor(GL_TRIANGLES,.2,.2,.2);
    shapeTA->setColorSpecular(GL_TRIANGLES,.5,.5,.5);
    shapeTA->setShininess(GL_TRIANGLES,10.);
    T.block<3,1>(0,3)=T.block<3,3>(0,0)*Eigen::Matrix<GLfloat,3,1>(-2.0f,0,0);
    shapeTB->setLocalTransform(T);
    shapeTB->setColorAmbient(GL_TRIANGLES,0,.05,0);
    shapeTB->setColor(GL_TRIANGLES,.2,.2,.2);
    shapeTB->setColorSpecular(GL_TRIANGLES,.5,.5,.5);
    shapeTB->setShininess(GL_TRIANGLES,10.);
    if(sim)
      theta=std::fmod(theta+M_PI*2/360.0f,M_PI*2);
  });
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods) {
    if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
  });
  drawer.mainLoop();
  return 0;
}
