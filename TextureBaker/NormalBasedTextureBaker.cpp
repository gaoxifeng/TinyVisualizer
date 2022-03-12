#include "NormalBasedTextureBaker.h"
#include "TextureExtender.h"
#include "MeshVisualizer.h"
#include <stack>
#include <TinyVisualizer/Texture.h>

namespace DRAWER {
NormalBasedTextureBaker::NormalBasedTextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res)
  :_low(low),_high(high),_lowRay(low,true),_highRay(high,false) {
  _texture.reset(new Texture(res,res,GL_RGBA));
  _low.setTexture(_texture,true,2.0/res);
}
Eigen::Matrix<GLdouble,2,1> NormalBasedTextureBaker::texelPos(int id,int w,int h) const {
  Eigen::Matrix<GLdouble,2,1> pos;
  pos.x()=(int(id%w)+0.5)/w;
  pos.y()=(int(id/w)+0.5)/h;
  return pos;
}
bool NormalBasedTextureBaker::inside(const RayCaster::Triangle& tri,const Eigen::Matrix<GLdouble,2,1>& p,Eigen::Matrix<GLdouble,3,1>* bary) const {
  Eigen::Matrix<GLdouble,2,1> a=_lowRay._tcss[tri._vid[0]];
  Eigen::Matrix<GLdouble,2,1> b=_lowRay._tcss[tri._vid[1]];
  Eigen::Matrix<GLdouble,2,1> c=_lowRay._tcss[tri._vid[2]];

  Eigen::Matrix<GLdouble,2,2> LHS;
  Eigen::Matrix<GLdouble,2,1> RHS=p-c;
  LHS.col(0)=a-c;
  LHS.col(1)=b-c;
  Eigen::Matrix<GLdouble,2,1> ab=LHS.inverse()*RHS;
  if(bary)
    *bary=Eigen::Matrix<GLdouble,3,1>(ab[0],ab[1],1-ab[0]-ab[1]);
  return ab[0]>=0 && ab[1]>=0 && ab[0]+ab[1]<=1;
}
std::vector<NormalBasedTextureBaker::TexelIntersect> NormalBasedTextureBaker::getInteriorTexel(bool compact) const {
  int w=_texture->width(),h=_texture->height();
  std::vector<TexelIntersect> texelI(w*h,TexelIntersectNone);
  //main loop
  std::vector<int> indices(w*h);
  for(int i=0; i<(int)indices.size(); i++)
    indices[i]=i;
  std::stack<std::pair<int,std::vector<int>>> ss;
  ss.push(std::make_pair((int)_lowRay._bvh.size()-1,indices));
  while(!ss.empty()) {
    int id=ss.top().first;
    indices=ss.top().second;
    ss.pop();
    //batched intersectBB/intersect
    #pragma omp parallel for
    for(int i=0; i<(int)indices.size(); i++) {
      if(!intersectBB2D(_lowRay._bvh[id]._bb,texelPos(indices[i],w,h)))
        indices[i]=-1;
      else if(_lowRay._bvh[id]._cell>=0) {
        Eigen::Matrix<GLdouble,3,1> bary;
        if(inside(_lowRay._triss[_lowRay._bvh[id]._cell],texelPos(indices[i],w,h),&bary))
          texelI[indices[i]]=std::make_tuple(indices[i],_lowRay._bvh[id]._cell,bary);
      }
    }
    //compact
    int newSz=0;
    for(int i=0; i<(int)indices.size(); i++)
      if(indices[i]>=0)
        indices[newSz++]=indices[i];
    indices.resize(newSz);
    //descend
    if(!indices.empty()) {
      ss.push(std::make_pair(_lowRay._bvh[id]._l,indices));
      ss.push(std::make_pair(_lowRay._bvh[id]._r,indices));
    }
  }
  if(compact) {
    int newSz=0;
    for(int i=0; i<(int)texelI.size(); i++)
      if(std::get<0>(texelI[i])>=0)
        texelI[newSz++]=texelI[i];
    texelI.resize(newSz);
  }
  return texelI;
}
void NormalBasedTextureBaker::setNearestTextureExtender() {
  _extender.reset(new NearestTextureExtender(_texture));
}
void NormalBasedTextureBaker::setLaplaceTextureExtender() {
  _extender.reset(new LaplaceTextureExtender(_texture));
}
void NormalBasedTextureBaker::bakeTexture() {
  //GPU->CPU
  Texture::TextureCPUData& data=_texture->loadCPUData();
  memset(data._data,0,data._width*data._height*4);  //clear to black

  //find texels
  std::vector<TexelIntersect> texels=getInteriorTexel(true);

  //find rays: we need two rays per texel, one facing forward and one facing bacward
  std::vector<Eigen::Matrix<GLdouble,6,1>> rays(texels.size()*2);
  GLdouble extLen=(maxCorner(_highRay._bvh.back()._bb)-minCorner(_highRay._bvh.back()._bb)).norm();
  #pragma omp parallel for
  for(int i=0; i<(int)texels.size(); i++) {
    const RayCaster::Triangle& tri=_lowRay._triss[std::get<1>(texels[i])];
    const auto& a=_lowRay._vss[tri._vid[0]];
    const auto& b=_lowRay._vss[tri._vid[1]];
    const auto& c=_lowRay._vss[tri._vid[2]];
    Eigen::Matrix<GLdouble,3,1> normal=(b-a).cross(c-a).normalized();

    Eigen::Matrix<GLdouble,6,1>& rayF=rays[i];
    minCorner(rayF)=a*std::get<2>(texels[i])[0]+b*std::get<2>(texels[i])[1]+c*std::get<2>(texels[i])[2];
    maxCorner(rayF)=minCorner(rayF)+normal*extLen;

    Eigen::Matrix<GLdouble,6,1>& rayB=rays[i+(int)texels.size()];
    minCorner(rayB)=minCorner(rayF);
    maxCorner(rayB)=minCorner(rayB)-normal*extLen;
  }
  std::vector<RayIntersect> rayI=_highRay.castRayBatched(rays);

  //assign color
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
  if(_extender)
    _extender->extend(texels);

  //CPU->GPU
  _texture->syncGPUData();
}
NormalBasedTextureBaker::TexelIntersect NormalBasedTextureBaker::TexelIntersectNone=std::make_tuple(-1,-1,Eigen::Matrix<GLdouble,3,1>());
}
