#ifndef RAY_CASTER_H
#define RAY_CASTER_H

#include "BVHBuilder.h"
#include <memory>

namespace DRAWER {

class Texture;
class MeshShape;
class MeshVisualizer;
class RayCaster {
 public:
  struct Triangle {
    Triangle() {}
    Triangle(int v0,int v1,int v2,int t):_vid(v0,v1,v2),_tid(t) {}
    Eigen::Matrix<GLuint,3,1> _vid;
    int _tid;
  };
  friend class NormalBasedTextureBaker;
  typedef std::pair<int,Eigen::Matrix<GLdouble,3,1>> RayIntersect;
  RayCaster(const MeshVisualizer& mesh,bool useTC=false);
  RayIntersect castRay(Eigen::Matrix<GLdouble,6,1>& ray) const;
  std::vector<RayIntersect> castRayBatched(std::vector<Eigen::Matrix<GLdouble,6,1>>& ray) const;
  Eigen::Matrix<GLdouble,3,1> getIntersectVert(const RayIntersect& I) const;
  Eigen::Matrix<GLdouble,2,1> getIntersectTexcoord(const RayIntersect& I) const;
  Eigen::Matrix<GLdouble,4,1> getIntersectColor(const RayIntersect& I) const;
  bool intersect(const Triangle& tri,Eigen::Matrix<GLdouble,6,1>& ray,RayIntersect& ret) const;
  void castRayImage(const std::string& path,int resw,Eigen::Matrix<GLdouble,3,1> dir,
                    const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ(),
                    const Eigen::Matrix<GLdouble,4,1>& bkg=Eigen::Matrix<GLdouble,4,1>::Ones()) const;
  std::vector<Eigen::Matrix<GLdouble,3,1>> sampleDir(int res,const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ()) const;
  std::vector<Eigen::Matrix<GLdouble,6,1>> sampleRay(int res,const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ()) const;
  std::shared_ptr<MeshShape> drawRay(int res,const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ(),bool batched=true) const; //for debug of castRay(Batched)
 private:
  static RayIntersect RayIntersectNone;
  std::vector<Eigen::Matrix<GLdouble,3,1>> _vss;
  std::vector<Eigen::Matrix<GLdouble,2,1>> _tcss;
  std::vector<std::shared_ptr<Texture>> _textures;
  std::vector<Triangle> _triss;
  std::vector<Node> _bvh;
};

}

#endif
