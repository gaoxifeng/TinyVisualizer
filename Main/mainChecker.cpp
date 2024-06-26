#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Povray.h>
#include <TinyVisualizer/MakeTexture.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<Texture> grid=drawGrid();
  drawer.addCamera2D(10);
  drawer.setDrawFunc([&]() {
    glActiveTexture(GL_TEXTURE0);
    grid->begin();
    glActiveTexture(GL_TEXTURE1);
    FBO::screenQuad(grid);
    grid->end();
    glActiveTexture(GL_TEXTURE0);
  });
  Povray pov("pov");
  drawer.drawPovray(pov);
  drawer.mainLoop();
  return 0;
}
