#include "RayCaster.h"
#include "MeshVisualizer.h"
#include <TinyVisualizer/MakeMesh.h>
#include <iostream>
#include <stack>

namespace DRAWER {

RayCaster::RayCaster(const MeshVisualizer& mesh) {
  for(const auto& component:mesh.getComponents()) {
    const MeshVisualizer::MeshComponent& comp=component.second;
    int off=(int)_vss.size();
    for(int i=0; i<comp._mesh->nrVertex(); i++) {
      _vss.push_back(comp._mesh->getVertex(i).cast<GLdouble>());
      _tcss.push_back(comp._mesh->getTexcoord(i).cast<GLdouble>());
    }
    for(int i=0; i<comp._mesh->nrIndex(); i+=3) {
      int tid=comp._texture ? (int)_textures.size() : -1;
      _triss.push_back(Triangle(comp._mesh->getIndex(i+0)+off,
                                comp._mesh->getIndex(i+1)+off,
                                comp._mesh->getIndex(i+2)+off,
                                tid));
      Node n;
      n._nrCell=1;
      n._cell=(int)_triss.size()-1;
      n._bb=resetBBD();
      n._bb=unionBB(n._bb,_vss[_triss.back()._vid[0]]);
      n._bb=unionBB(n._bb,_vss[_triss.back()._vid[1]]);
      n._bb=unionBB(n._bb,_vss[_triss.back()._vid[2]]);
      _bvh.push_back(n);
    }
    if(comp._texture)
      _textures.push_back(comp._texture);
  }
  BVHBuilder().buildBVH(_bvh);
  for(int i=(int)_triss.size(); i<(int)_bvh.size(); i++)
    _bvh[i]._cell=-1;
  std::cout << "BVH optimal depth=" << BVHBuilder::depthOptimal(_bvh) << " depth=" << BVHBuilder::depth(_bvh) << std::endl;
}
RayCaster::RayIntersect RayCaster::castRay(Eigen::Matrix<GLdouble,6,1>& ray) const {
  std::stack<int> ss;
  ss.push((int)_bvh.size()-1);
  RayIntersect ret=RayIntersectNone;
  while(!ss.empty()) {
    int id=ss.top();
    ss.pop();
    if(!intersectBB(_bvh[id]._bb,ray))
      continue;
    else if(_bvh[id]._cell>=0) {
      if(intersect(_triss[_bvh[id]._cell],ray,ret))
        ret.first=_bvh[id]._cell;
    } else {
      ss.push(_bvh[id]._l);
      ss.push(_bvh[id]._r);
    }
  }
  return ret;
}
std::vector<RayCaster::RayIntersect> RayCaster::castRayBatched(std::vector<Eigen::Matrix<GLdouble,6,1>>& ray) const {
  std::vector<int> indices(ray.size());
  for(int i=0; i<(int)ray.size(); i++)
    indices[i]=i;
  std::stack<std::pair<int,std::vector<int>>> ss;
  ss.push(std::make_pair((int)_bvh.size()-1,indices));
  std::vector<RayCaster::RayIntersect> ret(ray.size(),RayIntersectNone);
  while(!ss.empty()) {
    int id=ss.top().first;
    indices=ss.top().second;
    ss.pop();
    //batched intersectBB/intersect
    #pragma omp parallel for
    for(int i=0; i<(int)indices.size(); i++) {
      if(!intersectBB(_bvh[id]._bb,ray[indices[i]]))
        indices[i]=-1;
      else if(_bvh[id]._cell>=0) {
        if(intersect(_triss[_bvh[id]._cell],ray[indices[i]],ret[indices[i]]))
          ret[indices[i]].first=_bvh[id]._cell;
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
      ss.push(std::make_pair(_bvh[id]._l,indices));
      ss.push(std::make_pair(_bvh[id]._r,indices));
    }
  }
  return ret;
}
Eigen::Matrix<GLdouble,3,1> RayCaster::getIntersect(const RayIntersect& I) const {
  ASSERT(I.first>=0)
  return _vss[_triss[I.first]._vid[0]]*I.second[0]+
         _vss[_triss[I.first]._vid[1]]*I.second[1]+
         _vss[_triss[I.first]._vid[2]]*I.second[2];
}
bool RayCaster::intersect(const Triangle& tri,Eigen::Matrix<GLdouble,6,1>& ray,RayIntersect& ret) const {
  Eigen::Matrix<GLdouble,3,1> abt,a=_vss[tri._vid[0]],b=_vss[tri._vid[1]],c=_vss[tri._vid[2]];
  Eigen::Matrix<GLdouble,3,3> mat;
  mat.col(0)=a-c;
  mat.col(1)=b-c;
  mat.col(2)=-(maxCorner(ray)-minCorner(ray));
  if(std::abs(mat.determinant())<std::numeric_limits<GLdouble>::min())
    return false;

  abt=mat.inverse()*(minCorner(ray)-c);
  bool succ=(abt.x()>=0 && abt.y()>=0 && (abt.x()+abt.y())<=1) && //in triangle
            (abt.z()>=0 && abt.z()<1);  //in segment
  if(succ) {
    ret.second.x()=abt.x();
    ret.second.y()=abt.y();
    ret.second.z()=1-abt.x()-abt.y();
    maxCorner(ray)=minCorner(ray)-abt.z()*mat.col(2);
    return true;
  }
  return false;
}
std::vector<Eigen::Matrix<GLdouble,3,1>> RayCaster::sampleDir(int res,const Eigen::Matrix<GLdouble,3,1>& g) const {
  std::vector<Eigen::Matrix<GLdouble,3,1>> dirs;
  auto mesh=makeSphere(res,false,1);
  for(int i=0; i<mesh->nrVertex(); i++) {
    Eigen::Matrix<GLdouble,3,1> v=mesh->getVertex(i).cast<GLdouble>();
    if(g.isZero() || v.dot(g)<=0)
      dirs.push_back(-v);
  }
  return dirs;
}
std::vector<Eigen::Matrix<GLdouble,6,1>> RayCaster::sampleRay(int res,const Eigen::Matrix<GLdouble,3,1>& g) const {
  Eigen::Matrix<GLdouble,3,1> ext=(maxCorner(_bvh.back()._bb)-minCorner(_bvh.back()._bb));
  Eigen::Matrix<GLdouble,3,1> ctr=(maxCorner(_bvh.back()._bb)+minCorner(_bvh.back()._bb))/2;
  GLfloat rad=(GLfloat)ext.norm()/2;

  Eigen::Matrix<GLdouble,6,1> ray;
  std::vector<Eigen::Matrix<GLdouble,6,1>> rays;
  auto mesh=makeSphere(res,false,rad);
  for(int i=0; i<mesh->nrVertex(); i++) {
    Eigen::Matrix<GLdouble,3,1> v=mesh->getVertex(i).cast<GLdouble>();
    if(g.isZero() || v.dot(g)<=0) {
      maxCorner(ray)=ctr;
      minCorner(ray)=ctr+v;
      rays.push_back(ray);
    }
  }
  return rays;
}
std::shared_ptr<MeshShape> RayCaster::drawRay(int res,const Eigen::Matrix<GLdouble,3,1>& g,bool batched) const {
  std::shared_ptr<MeshShape> mesh(new MeshShape);
  mesh->setMode(GL_LINES);
  mesh->setColor(GL_LINES,1,0,0);
  mesh->setUseLight(false);
  mesh->setLineWidth(2);

  std::vector<Eigen::Matrix<GLdouble,6,1>> ray=sampleRay(res,g);
  std::vector<RayIntersect> rayI;
  if(batched)
    rayI=castRayBatched(ray);
  else {
    for(int i=0; i<(int)ray.size(); i++)
      rayI.push_back(castRay(ray[i]));
  }

  for(int i=0; i<(int)ray.size(); i++) {
    Eigen::Matrix<GLdouble,6,1> r=ray[i];
    if(rayI[i].first<0)
      continue;
    maxCorner(r)=getIntersect(rayI[i]);
    mesh->addIndexSingle(mesh->nrVertex());
    mesh->addVertex(minCorner(r).cast<GLfloat>());
    mesh->addIndexSingle(mesh->nrVertex());
    mesh->addVertex(maxCorner(r).cast<GLfloat>());
  }
  return mesh;
}
RayCaster::RayIntersect RayCaster::RayIntersectNone=RayCaster::RayIntersect(-1,Eigen::Matrix<GLdouble,3,1>());
}