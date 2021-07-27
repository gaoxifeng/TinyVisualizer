#include "CellShape.h"
#include <unordered_map>

namespace DRAWER {
struct Hash {
  size_t operator()(const Eigen::Matrix<int,3,1>& key) const {
    std::hash<int> h;
    return h(key[0])+h(key[1])+h(key[2]);
  }
  size_t operator()(const Eigen::Matrix<int,4,1>& key) const {
    std::hash<int> h;
    return h(key[0])+h(key[1])+h(key[2])+h(key[3]);
  }
};
CellShape::CellShape() {}
CellShape::CellShape(const std::vector<Eigen::Matrix<int,3,1>>& ids,GLfloat res,bool discreteNormal) {
  reset(ids,res,discreteNormal);
}
void CellShape::reset(const std::vector<Eigen::Matrix<int,3,1>>& idsV,GLfloat res,bool discreteNormal) {
#define ID(BASE,I) (BASE+Eigen::Matrix<int,3,1>(((I)&1)?1:0,((I)&2)?1:0,((I)&4)?1:0))
#define NEIGHBOR(BASE,D) (BASE+Eigen::Matrix<int,3,1>::Unit(D/2)*((D%2)?1:-1))
  std::unordered_map<Eigen::Matrix<int,3,1>,int,Hash> verts;
  std::unordered_set<Eigen::Matrix<int,4,1>,Hash> quads;
  std::unordered_set<Eigen::Matrix<int,3,1>,Hash> ids;
  std::map<int,Eigen::Matrix<int,3,1>> vertsInv;
  for(const Eigen::Matrix<int,3,1>& id:idsV)
    ids.insert(id);
  //insertVert
  int nrVert=0;
  std::function<void(Eigen::Matrix<int,3,1>)> insertVert=[&](const Eigen::Matrix<int,3,1>& id) {
    if(verts.find(id)==verts.end()) {
      verts[id]=nrVert;
      vertsInv[nrVert]=id;
      nrVert++;
    }
  };
  //vert
  for(const Eigen::Matrix<int,3,1>& id:ids)
    for(int d=0; d<6; d++)
      if(ids.find(NEIGHBOR(id,d))==ids.end()) {
        int mask=1<<(d/2);
        for(int i=0; i<8; i++)
          if(bool(i&mask)==bool(d%2))
            insertVert(ID(id,i));
      }
  //face
  for(const Eigen::Matrix<int,3,1>& id:ids)
    for(int d=0; d<6; d++)
      if(ids.find(NEIGHBOR(id,d))==ids.end()) {
        Eigen::Matrix<int,4,1> quad=FACE(d);
        for(int i=0; i<4; i++)
          quad[i]=verts.find(ID(id,quad[i]))->second;
        quads.insert(quad);
      }
  clear();
  if(discreteNormal) {
    int off=0;
    for(const Eigen::Matrix<int,4,1>& q:quads) {
      for(int d=0; d<4; d++)
        addVertex(vertsInv.find(q[d])->second.template cast<GLfloat>()*res-Eigen::Matrix<GLfloat,3,1>::Constant(res/2));
      addIndex(Eigen::Matrix<int,3,1>(off+0,off+1,off+2));
      addIndex(Eigen::Matrix<int,3,1>(off+0,off+2,off+3));
      off+=4;
    }
  } else {
    //insert vert
    for(const std::pair<int,Eigen::Matrix<int,3,1>>& vInv:vertsInv)
      addVertex(vInv.second.template cast<GLfloat>()*res-Eigen::Matrix<GLfloat,3,1>::Constant(res/2));
    //insert face
    for(const Eigen::Matrix<int,4,1>& q:quads) {
      addIndex(Eigen::Matrix<int,3,1>(q[0],q[1],q[2]));
      addIndex(Eigen::Matrix<int,3,1>(q[0],q[2],q[3]));
    }
  }
  setMode(GL_TRIANGLES);
  computeNormals();
#undef ID
#undef NEIGHBOR
}
Eigen::Matrix<int,4,1> CellShape::FACE(int d) {
  Eigen::Matrix<int,4,1> ret;
  switch(d) {
  case 0:
    ret=Eigen::Matrix<int,4,1>(0,4,6,2);
    break;
  case 1:
    ret=Eigen::Matrix<int,4,1>(1,3,7,5);
    break;
  case 2:
    ret=Eigen::Matrix<int,4,1>(0,1,5,4);
    break;
  case 3:
    ret=Eigen::Matrix<int,4,1>(2,6,7,3);
    break;
  case 4:
    ret=Eigen::Matrix<int,4,1>(0,2,3,1);
    break;
  case 5:
    ret=Eigen::Matrix<int,4,1>(4,5,7,6);
    break;
  default:
    ASSERT(false)
    break;
  }
  return ret;
}
}
