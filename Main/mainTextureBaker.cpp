#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/VisualTextureBaker.h>
#include <TextureBaker/NormalBasedTextureBaker.h>
#include <TextureBaker/VisualTextureBakerStaggered.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/Drawer.h>

using namespace DRAWER;

enum BAKER_MODE {
  NORMAL,
  VISUAL,
  VISUAL_STAGGERED,
  NONE,
};

enum VIS_MODE {
  TEXTURE,
  LOW_MESH,
  HIGH_MESH,
  SAMPLED_RAY,
};

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  int res=512;
  int resSphere=8;
  VIS_MODE mode=LOW_MESH;
  BAKER_MODE baker= NORMAL;
  auto g=-Eigen::Matrix<GLdouble,3,1>::UnitY();
  MeshVisualizer visHigh("D:/data/siggraphasia2022/input/high-poly/OBJ/SM_M2_Build_Apartment_01.obj");
  MeshVisualizer visLow("D:/data/siggraphasia2022/input/low-poly/SM_M2_Build_Apartment_01.obj");

  if(baker==NORMAL) {
    NormalBasedTextureBaker baker(visHigh,visLow,res);
    baker.setNearestTextureExtender();
    baker.bakeTexture();
  } else if(baker==VISUAL) {
    VisualTextureBaker baker(visHigh,visLow,res,resSphere,g);
    baker.setNearestTextureExtender();
    baker.bakeTexture();
  } else if(baker==VISUAL_STAGGERED) {
    VisualTextureBakerStaggered baker(visHigh,visLow,res,resSphere,g);
    baker.setNearestTextureExtender();
    baker.bakeTexture();
  }

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
  } else if(mode==SAMPLED_RAY) {
    drawer.addLightSystem(1024,10,true);
    drawer.addShape(visHigh.getShape());
    RayCaster ray(visHigh);
    drawer.addShape(ray.drawRay(resSphere,g));
  }
  drawer.mainLoop();
  return 0;
}
