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
    //view1->setBackground(1,0,0);
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
    //view2->setBackground(0,0,1);
    view2->addCamera2D(10);
    view2->setDrawFunc([&]() {
      glActiveTexture(GL_TEXTURE0);
      grid->begin();
      glActiveTexture(GL_TEXTURE1);
      FBO::screenQuad(grid);
      grid->end();
      glActiveTexture(GL_TEXTURE0);
    });
  }
  drawer.mainLoop();
  return 0;
}
