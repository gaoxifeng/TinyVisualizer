#include "VisualTextureBakerStaggered.h"
#include "TextureExtender.h"
#include <TinyVisualizer/Texture.h>
#include <iostream>

namespace DRAWER {
VisualTextureBakerStaggered::VisualTextureBakerStaggered
(const MeshVisualizer& high,MeshVisualizer& low,int res,int resSphere,
 const Eigen::Matrix<GLdouble,3,1>& g):TextureBaker(high,low,res),_LReg(1e-3),_tolLeastSquare(1e-5) {
  _dirs=_lowRay.sampleDir(resSphere,g);
}
void VisualTextureBakerStaggered::setLaplaceRegularization(GLdouble LReg) {
  ASSERT(LReg>0)
  _LReg=LReg;
}
void VisualTextureBakerStaggered::bakeTexture() {
  //GPU->CPU
  Texture::TextureCPUData& data=_texture->loadCPUData();
  memset(data._data,0,data._width*data._height*4);  //clear to black

  //find texels
  std::vector<TexelIntersect> texels=getInteriorTexel(true,Eigen::Matrix<GLdouble,2,1>(0.5,0.5)); //we add an offset so rays lie in the center of four texels
  Eigen::Matrix<GLdouble,4,1>* num=new Eigen::Matrix<GLdouble,4,1>[data._width*data._height];
  GLdouble* denom=new GLdouble[data._width*data._height];
  memset(num,0,sizeof(Eigen::Matrix<GLdouble,4,1>)*data._width*data._height);
  memset(denom,0,sizeof(GLdouble)*data._width*data._height);

  //loop over directions
  GLdouble extLen=(maxCorner(_highRay.getBVH().back()._bb)-minCorner(_highRay.getBVH().back()._bb)).norm();
  for(int d=0; d<(int)_dirs.size(); d++) {
    //find rays: one ray per texel-direction pair
    std::cout << "Constructing visual rays for " << d << "/" << _dirs.size() << " view!" << std::endl;
    std::vector<Eigen::Matrix<GLdouble,6,1>> rays(texels.size());
    #pragma omp parallel for
    for(int i=0; i<(int)texels.size(); i++) {
      const RayCaster::Triangle& tri=_lowRayTexture.getTriss()[std::get<1>(texels[i])];
      const auto& a=_lowRayTexture.getVss()[tri._vid[0]];
      const auto& b=_lowRayTexture.getVss()[tri._vid[1]];
      const auto& c=_lowRayTexture.getVss()[tri._vid[2]];
      //ray along direction
      Eigen::Matrix<GLdouble,6,1>& ray=rays[i];
      auto ctr=a*std::get<2>(texels[i])[0]+b*std::get<2>(texels[i])[1]+c*std::get<2>(texels[i])[2];
      minCorner(ray)=ctr-_dirs[d]*extLen;
      maxCorner(ray)=ctr+_dirs[d]*extLen;
    }
    std::cout << "Intersecting " << rays.size() << " visual rays!" << std::endl;
    std::vector<RayIntersect> rayIHigh=_highRay.castRayBatched(rays);
    std::vector<RayIntersect> rayILow=_lowRay.castRayBatched(rays);

    //compute averaged color values
    std::cout << "Accumulating " << texels.size() << " visual rays!" << std::endl;
    #pragma omp parallel for
    for(int i=0; i<(int)texels.size(); i++) {
      const RayIntersect& rayIHDT=rayIHigh[i];
      const RayIntersect& rayILDT=rayILow[i];
      if(rayIHDT.first>=0 && rayILDT.first==std::get<1>(texels[i])) {
        GLdouble weight=std::abs(_lowRay.getTriss()[std::get<1>(texels[i])]._normal.dot(_dirs[d]));
        num[std::get<0>(texels[i])]+=_highRay.getIntersectColor(rayIHDT)*weight;
        denom[std::get<0>(texels[i])]+=weight;
      }
    }
  }

  //solve linear system
  std::cout << "Solving for " << texels.size() << " color values, recovering other from Laplacian regularization!" << std::endl;
  solveLinearSystem(num,denom);
  delete [] num;
  delete [] denom;
  //we do not need to do any extension, which is taken care of in the same solve

  //CPU->GPU
  _texture->syncGPUData();
}
void VisualTextureBakerStaggered::solveLinearSystem(Eigen::Matrix<GLdouble,4,1>* num,GLdouble* denom) const {
  Texture::TextureCPUData& data=_texture->loadCPUData();
  Eigen::Matrix<GLdouble,4,1>* ret=new Eigen::Matrix<GLdouble,4,1>[data._width*data._height];
  memset(ret,0,sizeof(Eigen::Matrix<GLdouble,4,1>)*data._width*data._height);
  //rescale
  #pragma omp parallel for
  for(unsigned int h=0; h<data._height; h++)
    for(unsigned int w=0; w<data._width; w++)
      if(denom[w+data._width*h]>0) {
        num[w+data._width*h]/=denom[w+data._width*h];
        ret[w+data._width*h]=num[w+data._width*h];
        denom[w+data._width*h]=1;
      }
  //solve
  bool more=true;
  for(int iter=0; more; iter++) {
    more=false;
    for(unsigned int passX=0; passX<2; passX++)
      for(unsigned int passY=0; passY<2; passY++) {
        #pragma omp parallel for
        for(unsigned int h=0; h<data._height; h++)
          for(unsigned int w=0; w<data._width; w++)
            if((w%2)==passX && (h%2)==passY)
              more=more|applyGaussSeidel(w,h,data._width,data._height,ret,num,denom);
      }
    std::cout << "Staggered-Solve iter=" << iter << std::endl;
  }
  //assign
  #pragma omp parallel for
  for(unsigned int h=0; h<data._height; h++)
    for(unsigned int w=0; w<data._width; w++)
      reinterpret_cast<Eigen::Matrix<unsigned char,4,1>*>(data._data)[w+data._width*h]=(ret[w+data._width*h]*255).cast<unsigned char>();
  delete [] ret;
}
bool VisualTextureBakerStaggered::applyGaussSeidel
(int w,int h,int width,int height,Eigen::Matrix<GLdouble,4,1>* data,
 const Eigen::Matrix<GLdouble,4,1>* num,const GLdouble* denom) const {
#define ToD(X) X//(X.cast<GLdouble>()/255).eval()
#define FromD(X) X//(X*255).cast<unsigned char>()
  int off=w+width*h;
  //initialize
  Eigen::Matrix<GLdouble,4,1> NUM=Eigen::Matrix<GLdouble,4,1>::Zero();
  GLdouble DENOM=0;
  //laplace term
  if(w>0) {
    NUM+=ToD(data[off-1])*_LReg;
    DENOM+=_LReg;
  }
  if(w<width-1) {
    NUM+=ToD(data[off+1])*_LReg;
    DENOM+=_LReg;
  }
  if(h>0) {
    NUM+=ToD(data[off-width])*_LReg;
    DENOM+=_LReg;
  }
  if(h<height-1) {
    NUM+=ToD(data[off+width])*_LReg;
    DENOM+=_LReg;
  }
  //data term: bottom left
  if(w>0 && h>0) {
    NUM+=num[off-1-width]/4-(ToD(data[off-1-width])+ToD(data[off-width])+ToD(data[off-1]))*denom[off-1-width]/16;
    DENOM+=denom[off-1-width]/16;
  }
  //data term: bottom right
  if(w<width-1 && h>0) {
    NUM+=num[off  -width]/4-(ToD(data[off+1-width])+ToD(data[off-width])+ToD(data[off+1]))*denom[off  -width]/16;
    DENOM+=denom[off  -width]/16;
  }
  //data term: top left
  if(w>0 && h<height-1) {
    NUM+=num[off-1]/4-(ToD(data[off-1+width])+ToD(data[off+width])+ToD(data[off-1]))*denom[off-1]/16;
    DENOM+=denom[off-1]/16;
  }
  //data term: top right
  if(w<width-1 && h<height-1) {
    NUM+=num[off  ]/4-(ToD(data[off+1+width])+ToD(data[off+width])+ToD(data[off+1]))*denom[off  ]/16;
    DENOM+=denom[off  ]/16;
  }
  //solve
  NUM=(NUM/DENOM).cwiseMax(Eigen::Matrix<GLdouble,4,1>::Zero()).cwiseMin(Eigen::Matrix<GLdouble,4,1>::Ones());
  bool ret=(FromD(NUM)-data[off]).cwiseAbs().maxCoeff()>_tolLeastSquare;
  data[off]=FromD(NUM);
  return ret;
#undef ToD
}
}
