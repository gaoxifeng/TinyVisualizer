#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Povray.h>
#include <TinyVisualizer/CellShape.h>
#include <TinyVisualizer/ShadowAndLight.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<CellShape> cells(new CellShape());
  std::vector<Eigen::Matrix<int,3,1>> ids;
  for(int x=-10; x<=10; x++)
    for(int y=-10; y<=10; y++)
      for(int z=-10; z<=10; z++)
        if(x*x+y*y+z*z<=100)
          ids.push_back(Eigen::Matrix<int,3,1>(x,y,z));
  cells->reset(ids,0.1f);
  drawer.addShape(cells);
  //cells->debugWriteObj("cells.obj");

#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(0);
  drawer.getLight()->lightSz(10);
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>(x,y,z)*2,
                                    Eigen::Matrix<GLfloat,3,1>(.2,.2,.2),
                                    Eigen::Matrix<GLfloat,3,1>(.7,.7,.7),
                                    Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  Povray pov("pov");
  drawer.drawPovray(pov);
  drawer.mainLoop();
  return 0;
}
