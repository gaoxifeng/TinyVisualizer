#include <Drawer/Drawer.h>
#include <Drawer/MakeTexture.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<Texture> grid=drawGrid();
  drawer.addCamera2D(10);
  drawer.setDrawFunc([&]() {
    glActiveTexture(GL_TEXTURE0);
    grid->begin();
    glActiveTexture(GL_TEXTURE1);
    glColor3f(1,1,1);
    FBO::screenQuad();
    grid->end();
    glActiveTexture(GL_TEXTURE0);
  });
  drawer.mainLoop();
  return 0;
}
