#include <glad/gl.h>
#include <TinyVisualizer/MultiDrawer.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/BezierCurveShape.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  MultiDrawer drawer(argc,argv);
  drawer.setViewportLayout(1,2);
  std::shared_ptr<Texture> grid=drawGrid();
  //first view
  {
    std::shared_ptr<Drawer> view1=drawer.getDrawer(0,0);
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
    int i=0;
    std::shared_ptr<Drawer> view2=drawer.getDrawer(0,1);
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
      view2->addShape(curve);
      i+=1;
    }
  }
  drawer.mainLoop();
  return 0;
}
