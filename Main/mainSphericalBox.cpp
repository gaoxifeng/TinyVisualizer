#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <iostream>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<Texture> checker=drawChecker();

  int off=0;
  std::vector<Eigen::Matrix<GLfloat,4,1>> params;
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0.1f,0.1f,0.1f,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0,0.1f,0.1f,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0.1f,0,0.1f,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0.1f,0.1f,0,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0,0,0.1f,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0,0.1f,0,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0.1f,0,0,0.2f));
  params.push_back(Eigen::Matrix<GLfloat,4,1>(0,0,0,0.2f));
  for(const Eigen::Matrix<GLfloat,4,1>& param:params) {
    std::shared_ptr<Bullet3DShape> shapeTA(new Bullet3DShape);
    std::shared_ptr<MeshShape> sphereA=makeSphericalBox(8,true,param[3],param.segment<3>(0));
    sphereA->setTexture(checker);
    shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(off,0,0));
    shapeTA->addShape(sphereA);
    drawer.addShape(shapeTA);
    off++;

    std::shared_ptr<Bullet3DShape> shapeTB(new Bullet3DShape);
    std::shared_ptr<MeshShape> sphereB=makeSphericalBox(8,false,param[3],param.segment<3>(0));
    shapeTB->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(off,0,0));
    shapeTB->addShape(sphereB);
    shapeTB->setColor(GL_LINES,.7,.7,.7);
    drawer.addShape(shapeTB);
    off++;
  }

#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(0);
  drawer.getLight()->lightSz(10);
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(off/2,3, 0.2f),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
  drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(off/2,3,-0.2f),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(1,1,1),
                              Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  drawer.mainLoop();
  return 0;
}
