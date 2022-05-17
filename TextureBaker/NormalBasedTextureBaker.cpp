#include "NormalBasedTextureBaker.h"
#include "TextureExtender.h"
#include <TinyVisualizer/Texture.h>
#include <iostream>

namespace DRAWER {
NormalBasedTextureBaker::NormalBasedTextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res)
  :TextureBaker(high,low,res) {}
void NormalBasedTextureBaker::bakeTexture() {
  //GPU->CPU
  Texture::TextureCPUData& data=_texture->loadCPUData();
  memset(data._data,0,data._width*data._height*4);  //clear to black

  //find texels
  std::vector<TexelIntersect> texels=getInteriorTexel(true);

  //find rays: we need two rays per texel, one facing forward and one facing bacward
  std::vector<Eigen::Matrix<GLdouble,6,1>> rays(texels.size()*2);
  GLdouble extLen=(maxCorner(_highRay.getBVH().back()._bb)-minCorner(_highRay.getBVH().back()._bb)).norm();
  std::cout << "Constructing normal rays!" << std::endl;
  #pragma omp parallel for
  for(int i=0; i<(int)texels.size(); i++) {
    const RayCaster::Triangle& tri=_lowRayTexture.getTriss()[std::get<1>(texels[i])];
    const auto& a=_lowRayTexture.getVss()[tri._vid[0]];
    const auto& b=_lowRayTexture.getVss()[tri._vid[1]];
    const auto& c=_lowRayTexture.getVss()[tri._vid[2]];
    //forward ray
    Eigen::Matrix<GLdouble,6,1>& rayF=rays[i];
    minCorner(rayF)=a*std::get<2>(texels[i])[0]+b*std::get<2>(texels[i])[1]+c*std::get<2>(texels[i])[2];
    maxCorner(rayF)=minCorner(rayF)+tri._normal*extLen;
    //backward ray
    Eigen::Matrix<GLdouble,6,1>& rayB=rays[i+(int)texels.size()];
    minCorner(rayB)=minCorner(rayF);
    maxCorner(rayB)=minCorner(rayB)-tri._normal*extLen;
  }
  std::cout << "Intersecting " << rays.size() << " normal rays!" << std::endl;
  std::vector<RayIntersect> rayI=_highRay.castRayBatched(rays);

  //assign color
  std::cout << "Computing " << texels.size() << " color values!" << std::endl;
  #pragma omp parallel for
  for(int i=0; i<(int)texels.size(); i++) {
    int w=std::get<0>(texels[i])%data._width;
    int h=std::get<0>(texels[i])/data._width;
    const RayIntersect& rayIF=rayI[i];
    const RayIntersect& rayIB=rayI[i+(int)texels.size()];
    if(rayIF.first>=0 && rayIB.first<0)
      Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+4*(w+data._width*h))=(_highRay.getIntersectColor(rayIF)*255).cast<unsigned char>();
    else if(rayIF.first<0 && rayIB.first>=0)
      Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+4*(w+data._width*h))=(_highRay.getIntersectColor(rayIB)*255).cast<unsigned char>();
    else if(rayIF.first>=0 && rayIB.first>=0) {
      const Eigen::Matrix<GLdouble,6,1>& rayF=rays[i];
      const Eigen::Matrix<GLdouble,6,1>& rayB=rays[i+(int)texels.size()];
      GLdouble distF=(maxCorner(rayF)-minCorner(rayF)).squaredNorm();
      GLdouble distB=(maxCorner(rayB)-minCorner(rayB)).squaredNorm();
      if(distF<distB)
        Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+4*(w+data._width*h))=(_highRay.getIntersectColor(rayIF)*255).cast<unsigned char>();
      else Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+4*(w+data._width*h))=(_highRay.getIntersectColor(rayIB)*255).cast<unsigned char>();
    } else std::get<0>(texels[i])=-1;
  }

  //extend texture
  if(_extender) {
    std::cout << "Extending textures!" << std::endl;
    _extender->extend(texels);
  }

  //CPU->GPU
  _texture->syncGPUData();
}
}
