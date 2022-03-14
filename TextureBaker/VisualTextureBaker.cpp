#include "VisualTextureBaker.h"
#include "TextureExtender.h"
#include <TinyVisualizer/Texture.h>
#include <iostream>

namespace DRAWER {
VisualTextureBaker::VisualTextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res,int resSphere,
                                       const Eigen::Matrix<GLdouble,3,1>& g):TextureBaker(high,low,res) {
  _dirs=_lowRay.sampleDir(resSphere,g);
}
void VisualTextureBaker::bakeTexture() {
  //GPU->CPU
  Texture::TextureCPUData& data=_texture->loadCPUData();
  memset(data._data,0,data._width*data._height*4);  //clear to black

  //find texels
  std::vector<TexelIntersect> texels=getInteriorTexel(true);
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
      GLdouble weight=tri._normal.dot(_dirs[d]);
      if(weight<=0) //back face culling
        ray.setConstant(std::numeric_limits<GLdouble>::infinity());
      else {
        auto ctr=a*std::get<2>(texels[i])[0]+b*std::get<2>(texels[i])[1]+c*std::get<2>(texels[i])[2];
        minCorner(ray)=ctr+_dirs[d]*extLen;
        maxCorner(ray)=ctr-_dirs[d]*extLen;
      }
    }
    std::cout << "Intersecting " << rays.size() << " visual rays!" << std::endl;
    std::vector<RayIntersect> rayI=_highRay.castRayBatched(rays);

    //compute averaged color values
    std::cout << "Accumulating " << texels.size() << " visual rays!" << std::endl;
    #pragma omp parallel for
    for(int i=0; i<(int)texels.size(); i++) {
      const RayIntersect& rayIDT=rayI[i];
      if(rayIDT.first>=0) {
        GLdouble weight=_lowRay.getTriss()[std::get<1>(texels[i])]._normal.dot(_dirs[d]);
        num[i]+=_highRay.getIntersectColor(rayIDT)*weight;
        denom[i]+=weight;
      }
    }
  }

  std::cout << "Computing " << texels.size() << " color values!" << std::endl;
  #pragma omp parallel for
  for(int i=0; i<(int)texels.size(); i++)
    if(denom[std::get<0>(texels[i])]>0) {
      int w=std::get<0>(texels[i])%data._width;
      int h=std::get<0>(texels[i])/data._width;
      Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+4*(w+data._width*h))=
            (num[std::get<0>(texels[i])]*255/denom[std::get<0>(texels[i])]).cast<unsigned char>();
    } else std::get<0>(texels[i])=-1;
  delete [] num;
  delete [] denom;

  //extend texture
  if(_extender) {
    std::cout << "Extending textures!" << std::endl;
    _extender->extend(texels);
  }

  //CPU->GPU
  _texture->syncGPUData();
}
}
