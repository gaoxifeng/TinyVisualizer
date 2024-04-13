#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Povray.h>
#include <TinyVisualizer/ArrowShape.h>
#include <TinyVisualizer/LowDimensionalMeshShape.h>
#include <TinyVisualizer/ShadowAndLight.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<ArrowShape> arrow(new ArrowShape(60,0.1,0.2));
  arrow->setArrow(Eigen::Matrix<GLfloat,3,1>(-0.5,-0.5,-0.5),Eigen::Matrix<GLfloat,3,1>(0.5,0.5,0.5));
  arrow->setColorAmbient(GL_TRIANGLES,1,1,1);

  Eigen::Matrix<GLfloat,-1,-1> DHDL;
  DHDL.resize(arrow->nrVertex()*3,3);
  for(int i=0; i<arrow->nrVertex(); i++)
    DHDL.block<3,3>(i*3,0).setIdentity();
  std::shared_ptr<LowDimensionalMeshShape> lowDimArrow(new LowDimensionalMeshShape(arrow));
  lowDimArrow->setLowToHighDimensionalMapping(DHDL);
  drawer.addShape(lowDimArrow);

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

  bool sim=false;
  GLfloat theta=0;
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>) {
    if(sim) {
      theta=std::fmod(theta+M_PI*2/360.0f,M_PI*2);
      lowDimArrow->updateHighDimensionalMapping(Eigen::Matrix<GLfloat,3,1>(std::sin(theta),0,0));
    }
  });
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
    else if(key==GLFW_KEY_F && action==GLFW_PRESS) {
      Povray pov("pov");
      drawer.drawPovray(pov);
    }
  });
  drawer.mainLoop();
  return 0;
}
