#ifndef TEXTURE_BAKER_H
#define TEXTURE_BAKER_H

#include "RayCaster.h"

namespace DRAWER {

class TextureExtender;
class TextureBaker {
 public:
  typedef RayCaster::RayIntersect RayIntersect;
  typedef std::tuple<int,int,Eigen::Matrix<GLdouble,3,1>> TexelIntersect; //texelId,triId,bary
  TextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res);
  void setNearestTextureExtender();
  void setLaplaceTextureExtender();
  virtual void bakeTexture()=0;
 protected:
  Eigen::Matrix<GLdouble,2,1> texelPos(int id,int w,int h) const;
  bool inside(const RayCaster::Triangle& tri,const Eigen::Matrix<GLdouble,2,1>& texelPos,Eigen::Matrix<GLdouble,3,1>* bary) const;
  std::vector<TexelIntersect> getInteriorTexel(bool compact) const;
  //data
  MeshVisualizer& _low;
  const MeshVisualizer& _high;
  std::shared_ptr<Texture> _texture;
  RayCaster _lowRay,_lowRayTexture,_highRay;
  std::shared_ptr<TextureExtender> _extender;
  static TexelIntersect TexelIntersectNone;
};

}

#endif
