#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Povray.h>
#include <TinyVisualizer/BezierCurveShape.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  int i=0;
  Drawer drawer(argc,argv);
  for(GLfloat RES: {
        16,0
      }) {
    std::shared_ptr<BezierCurveShape> curve(new BezierCurveShape(0.01f,true,RES));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>( 0, 0,0+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>(-1,-1,1+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>( 1,-1,2+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>( 1, 1,3+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>(-1, 1,4+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>(-1,-1,5+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>( 1,-1,6+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>( 1, 1,7+i));
    curve->addControlPoint(Eigen::Matrix<GLfloat,3,1>(-1, 1,8+i));
    curve->setLineWidth(2);
    curve->setColorDiffuse(GL_QUADS,.7,.7,.7);
    curve->setColorDiffuse(GL_LINE_STRIP,.7,.7,.7);
    drawer.addShape(curve);
    i+=1;
  }
  drawer.setKeyFunc([&](GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_F && action==GLFW_PRESS) {
      Povray pov("pov");
      drawer.drawPovray(pov);
    }
  });
  drawer.mainLoop();
  return 0;
}
