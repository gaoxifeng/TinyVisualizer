#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/TextureExtender.h>
#include <TextureBaker/NormalBasedTextureBaker.h>
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/CompositeShape.h>

using namespace DRAWER;

enum VIS_MODE {
  TEXTURE,
  LOW_MESH,
  HIGH_MESH,
};

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  MeshVisualizer visHigh("high-poly/OBJ/SM_M2_Build_Apartment_02.obj");
  MeshVisualizer visLow("low-poly/SM_M2_Build_Apartment_02.obj");
  NormalBasedTextureBaker baker(visHigh,visLow,2048);
  baker.setLaplaceTextureExtender();
  baker.bakeTexture();

  VIS_MODE mode=LOW_MESH;
  if(mode==TEXTURE) {
    std::shared_ptr<Texture> grid=visLow.getComponents().begin()->second._texture;
    drawer.addCamera2D(10);
    drawer.setDrawFunc([&]() {
      glActiveTexture(GL_TEXTURE0);
      grid->begin();
      glActiveTexture(GL_TEXTURE1);
      FBO::screenQuad(grid);
      grid->end();
      glActiveTexture(GL_TEXTURE0);
    });
  } else if(mode==LOW_MESH) {
    drawer.addLightSystem(1024,10,true);
    drawer.addShape(visLow.getShape());
  } else if(mode==HIGH_MESH) {
    drawer.addLightSystem(1024,10,true);
    drawer.addShape(visHigh.getShape());
  }
  drawer.mainLoop();
  return 0;
}
