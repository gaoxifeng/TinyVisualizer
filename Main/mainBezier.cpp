#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/BezierCurveShape.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  GLfloat i=0;
  Drawer drawer(argc,argv);
  for(int RES: {
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
    curve->setColor(GL_QUADS,.7f,.7f,.7f);
    curve->setColor(GL_LINE_STRIP,.7f,.7f,.7f);
    drawer.addShape(curve);
    i+=1.f;
  }
  drawer.mainLoop();
  return 0;
}
