#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/CellShape.h>
#include <TinyVisualizer/ShadowAndLight.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    for(int x=0; x<3; x++)
      ids.push_back(Eigen::Matrix<int,3,1>(x,0,0));
    for(int y=0; y<3; y++)
      ids.push_back(Eigen::Matrix<int,3,1>(0,y,0));
    for(int z=0; z<3; z++)
      ids.push_back(Eigen::Matrix<int,3,1>(0,0,z));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("tripod.obj");
  }
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    for(int x=-2; x<=2; x++)
      ids.push_back(Eigen::Matrix<int,3,1>(x,0,0));
    for(int y=-2; y<=2; y++)
      ids.push_back(Eigen::Matrix<int,3,1>(0,y,0));
    for(int z=-2; z<=2; z++)
      ids.push_back(Eigen::Matrix<int,3,1>(0,0,z));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("octpod.obj");
  }
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    ids.push_back(Eigen::Matrix<int,3,1>(0,0,0));
    ids.push_back(Eigen::Matrix<int,3,1>(2,0,0));
    ids.push_back(Eigen::Matrix<int,3,1>(0,2,0));
    ids.push_back(Eigen::Matrix<int,3,1>(2,2,0));
    for(int x=0; x<=2; x++)
      for(int y=0; y<=2; y++)
        ids.push_back(Eigen::Matrix<int,3,1>(x,y,1));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("walker.obj");
  }
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    for(int z=0; z<=1; z++) {
      ids.push_back(Eigen::Matrix<int,3,1>(0,0,z));
      ids.push_back(Eigen::Matrix<int,3,1>(2,0,z));
      ids.push_back(Eigen::Matrix<int,3,1>(0,2,z));
      ids.push_back(Eigen::Matrix<int,3,1>(2,2,z));
    }
    for(int x=0; x<=2; x++)
      for(int y=0; y<=2; y++)
        ids.push_back(Eigen::Matrix<int,3,1>(x,y,2));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("walkerHigh.obj");
  }
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    ids.push_back(Eigen::Matrix<int,3,1>(0,0,0));
    ids.push_back(Eigen::Matrix<int,3,1>(3,0,0));
    ids.push_back(Eigen::Matrix<int,3,1>(0,3,0));
    ids.push_back(Eigen::Matrix<int,3,1>(3,3,0));
    for(int x=0; x<=3; x++)
      for(int y=0; y<=3; y++)
        ids.push_back(Eigen::Matrix<int,3,1>(x,y,1));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("walkerWide.obj");
  }
  {
    std::shared_ptr<CellShape> cells(new CellShape());
    std::vector<Eigen::Matrix<int,3,1>> ids;
    for(int z=0; z<=1; z++) {
      ids.push_back(Eigen::Matrix<int,3,1>(0,0,z));
      ids.push_back(Eigen::Matrix<int,3,1>(3,0,z));
      ids.push_back(Eigen::Matrix<int,3,1>(0,3,z));
      ids.push_back(Eigen::Matrix<int,3,1>(3,3,z));
    }
    for(int x=0; x<=3; x++)
      for(int y=0; y<=3; y++)
        ids.push_back(Eigen::Matrix<int,3,1>(x,y,2));
    cells->reset(ids,1);
    drawer.addShape(cells);
    cells->debugWriteObj("walkerHighWide.obj");
  }

#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(0);
  drawer.getLight()->lightSz(10);
  for(int x=-1; x<=1; x+=2)
    for(int y=-1; y<=1; y+=2)
      for(int z=-1; z<=1; z+=2)
        drawer.getLight()->addLight(Eigen::Matrix<GLfloat,3,1>((GLfloat)x,(GLfloat)y,(GLfloat)z)*2,
                                    Eigen::Matrix<GLfloat,3,1>(.2f,.2f,.2f),
                                    Eigen::Matrix<GLfloat,3,1>(.7f,.7f,.7f),
                                    Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  drawer.mainLoop();
  return 0;
}
