#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/PatchDeformer.h>
#include <TinyVisualizer/Drawer.h>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  MeshVisualizer patch2D("data_for_L1opt/3_UV.obj",true);
  MeshVisualizer patch3D("data_for_L1opt/3.obj",true);
  patch2D.printInfo();
  patch3D.printInfo();

  mpfr_float::default_precision(1000);
  PatchDeformer deform(patch2D,patch3D);
  deform.debugL1(1e-2,1e-8);
  deform.debugConvex(1e-8);
  deform.debugArea(1e-8);
  deform.debugArap(1e-8);
  return 0;
}
