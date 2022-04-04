#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/PatchDeformer.h>
#include <TinyVisualizer/ArrowShape.h>
#include <TinyVisualizer/Drawer.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  MeshVisualizer patch2D("data_for_L1opt/7_UV.obj",true);
  MeshVisualizer patch3D("data_for_L1opt/7.obj",true);
  patch2D.printInfo();
  patch3D.printInfo();

  mpfr_float::default_precision(1000);
  PatchDeformer deform(patch2D,patch3D);
  deform.optimize(0.0001,10,0.01,0.01,0.01);
  //deform.debugL1(1e-2,1e-8);
  //deform.debugConvex(1e-8);
  //deform.debugArea(1e-8);
  //deform.debugArap(1e-8);

  int id=0;
  std::shared_ptr<Shape> last;
  if(last)
    drawer.removeShape(last);
  drawer.addShape(last=deform.getOptimizeHistory()[id]);
  //main loop
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_E) {
      id=std::min<int>(id+1,deform.getOptimizeHistory().size()-1);
      if(last)
        drawer.removeShape(last);
      drawer.addShape(last=deform.getOptimizeHistory()[id]);
    } else if(key==GLFW_KEY_Q) {
      id=std::max<int>(id-1,0);
      if(last)
        drawer.removeShape(last);
      drawer.addShape(last=deform.getOptimizeHistory()[id]);
    }
  });
  drawer.mainLoop();
  return 0;
}
