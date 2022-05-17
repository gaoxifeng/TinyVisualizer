#include "TextureBaker.h"
#include "TextureExtender.h"
#include "MeshVisualizer.h"
#include <stack>
#include <TinyVisualizer/Texture.h>

namespace DRAWER {
TextureBaker::TextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res)
  :_low(low),_high(high),_lowRay(low,false),_lowRayTexture(low,true),_highRay(high,false) {
  _texture.reset(new Texture(res,res,GL_RGBA));
  _low.setTexture(_texture,true,2.0/res);
}
const RayCaster& TextureBaker::getHighRayCaster() const {
  return _highRay;
}
const RayCaster& TextureBaker::getLowRayCaster() const {
  return _lowRay;
}
void TextureBaker::setNearestTextureExtender() {
  _extender.reset(new NearestTextureExtender(_texture));
}
void TextureBaker::setLaplaceTextureExtender() {
  _extender.reset(new LaplaceTextureExtender(_texture));
}
void TextureBaker::save(const std::string& path) {
  _low.getComponents().begin()->second._texture->save(path);
}
//helper
Eigen::Matrix<GLdouble,2,1> TextureBaker::texelPos(int id,int w,int h,const Eigen::Matrix<GLdouble,2,1>& offset) const {
  Eigen::Matrix<GLdouble,2,1> pos;
  pos.x()=(int(id%w)+0.5+offset[0])/w;
  pos.y()=(int(id/w)+0.5+offset[1])/h;
  return pos;
}
bool TextureBaker::inside(const RayCaster::Triangle& tri,const Eigen::Matrix<GLdouble,2,1>& p,Eigen::Matrix<GLdouble,3,1>* bary) const {
  Eigen::Matrix<GLdouble,2,1> a=_lowRayTexture.getTcss()[tri._vid[0]];
  Eigen::Matrix<GLdouble,2,1> b=_lowRayTexture.getTcss()[tri._vid[1]];
  Eigen::Matrix<GLdouble,2,1> c=_lowRayTexture.getTcss()[tri._vid[2]];

  Eigen::Matrix<GLdouble,2,2> LHS;
  Eigen::Matrix<GLdouble,2,1> RHS=p-c;
  LHS.col(0)=a-c;
  LHS.col(1)=b-c;
  Eigen::Matrix<GLdouble,2,1> ab=LHS.inverse()*RHS;
  if(bary)
    *bary=Eigen::Matrix<GLdouble,3,1>(ab[0],ab[1],1-ab[0]-ab[1]);
  return ab[0]>=0 && ab[1]>=0 && ab[0]+ab[1]<=1;
}
std::vector<TextureBaker::TexelIntersect> TextureBaker::getInteriorTexel(bool compact,const Eigen::Matrix<GLdouble,2,1>& offset) const {
  int w=_texture->width(),h=_texture->height();
  std::vector<TexelIntersect> texelI(w*h,TexelIntersectNone);
  //main loop
  std::vector<int> indices(w*h);
  for(int i=0; i<(int)indices.size(); i++)
    indices[i]=i;
  std::stack<std::pair<int,std::vector<int>>> ss;
  ss.push(std::make_pair((int)_lowRayTexture.getBVH().size()-1,indices));
  while(!ss.empty()) {
    int id=ss.top().first;
    indices=ss.top().second;
    ss.pop();
    //batched intersectBB/intersect
    #pragma omp parallel for
    for(int i=0; i<(int)indices.size(); i++) {
      Eigen::Matrix<GLdouble,2,1>  tpos=texelPos(indices[i],w,h,offset);
      if(!intersectBB2D(_lowRayTexture.getBVH()[id]._bb,tpos))
        indices[i]=-1;
      else if(_lowRayTexture.getBVH()[id]._cell>=0) {
        Eigen::Matrix<GLdouble,3,1> bary;
        if(inside(_lowRayTexture.getTriss()[_lowRayTexture.getBVH()[id]._cell],tpos,&bary))
          texelI[indices[i]]=std::make_tuple(indices[i],_lowRayTexture.getBVH()[id]._cell,bary);
      }
    }
    //compact
    int newSz=0;
    for(int i=0; i<(int)indices.size(); i++)
      if(indices[i]>=0)
        indices[newSz++]=indices[i];
    indices.resize(newSz);
    //descend
    if(!indices.empty() && _lowRayTexture.getBVH()[id]._l>=0 && _lowRayTexture.getBVH()[id]._r >=0) {
      ss.push(std::make_pair(_lowRayTexture.getBVH()[id]._l,indices));
      ss.push(std::make_pair(_lowRayTexture.getBVH()[id]._r,indices));
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
TextureBaker::TexelIntersect TextureBaker::TexelIntersectNone=std::make_tuple(-1,-1,Eigen::Matrix<GLdouble,3,1>());
}
