#include "BVHBuilder.h"
#include <type_traits>
#include <deque>
#include <stack>

namespace DRAWER {
GLdouble area(const Eigen::Matrix<GLdouble,6,1>& bb) {
  for(int d=0; d<3; d++)
    if(bb[d]>=bb[d+3])
      return 0;
  Eigen::Matrix<GLdouble,3,1> ext=bb.segment<3>(3)-bb.segment<3>(0);
  return ext[0]*ext[1]+ext[0]*ext[2]+ext[1]*ext[2];
}
Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> minCorner(const Eigen::Matrix<GLdouble,6,1>& bb) {
  return Eigen::Map<const Eigen::Matrix<GLdouble,3,1>>(bb.data());
}
Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> maxCorner(const Eigen::Matrix<GLdouble,6,1>& bb) {
  return Eigen::Map<const Eigen::Matrix<GLdouble,3,1>>(bb.data()+3);
}
Eigen::Map<Eigen::Matrix<GLdouble,3,1>> minCorner(Eigen::Matrix<GLdouble,6,1>& bb) {
  return Eigen::Map<Eigen::Matrix<GLdouble,3,1>>(bb.data());
}
Eigen::Map<Eigen::Matrix<GLdouble,3,1>> maxCorner(Eigen::Matrix<GLdouble,6,1>& bb) {
  return Eigen::Map<Eigen::Matrix<GLdouble,3,1>>(bb.data()+3);
}
bool intersectBB(const Eigen::Matrix<GLdouble,6,1>& bb,const Eigen::Matrix<GLdouble,6,1>& ray) {
  Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> p=minCorner(ray);
  Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> q=maxCorner(ray);
  GLdouble lo=1;
  GLdouble hi=1;

  GLdouble s=0;
  GLdouble t=1;
  for(int i=0; i<3; ++i) {
    GLdouble D=q[i]-p[i];
    if(p[i]<q[i]) {
      GLdouble s0=lo*(minCorner(bb)[i]-p[i])/D, t0=hi*(maxCorner(bb)[i]-p[i])/D;
      if(s0>s) s=s0;
      if(t0<t) t=t0;
    } else if(p[i]>q[i]) {
      GLdouble s0=lo*(maxCorner(bb)[i]-p[i])/D, t0=hi*(minCorner(bb)[i]-p[i])/D;
      if(s0>s) s=s0;
      if(t0<t) t=t0;
    } else {
      if(p[i]<minCorner(bb)[i] || p[i]>maxCorner(bb)[i])
        return false;
    }
    if(s>t)
      return false;
  }
  return true;
}
bool intersectBB2D(const Eigen::Matrix<GLdouble,6,1>& bb,const Eigen::Matrix<GLdouble,2,1>& b) {
  return (bb.segment<2>(0).array()<=b.array()).all() && (bb.segment<2>(3).array()>=b.array()).all();
}
Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,6,1>& b) {
  Eigen::Matrix<GLdouble,6,1> ret;
  ret.segment<3>(0)=minCorner(a).cwiseMin(minCorner(b));
  ret.segment<3>(3)=maxCorner(a).cwiseMax(maxCorner(b));
  return ret;
}
Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,3,1>& b) {
  Eigen::Matrix<GLdouble,6,1> ret;
  ret.segment<3>(0)=minCorner(a).cwiseMin(b);
  ret.segment<3>(3)=maxCorner(a).cwiseMax(b);
  return ret;
}
Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,2,1>& b) {
  return unionBB(a,Eigen::Matrix<GLdouble,3,1>(b[0],b[1],0));
}
Eigen::Matrix<GLdouble,6,1> resetBBD() {
  Eigen::Matrix<GLdouble,6,1> bb;
  bb.segment<3>(0).setConstant(std::numeric_limits<GLdouble>::max());
  bb.segment<3>(3).setConstant(-std::numeric_limits<GLdouble>::max());
  return bb;
}
int BVHBuilder::buildBVH(std::vector<Node>& bvh,int f,int t) {
  if(bvh.empty())
    return -1;
  //find roots
  _roots.clear();
  for(int i=f; i<t; i++) {
    ASSERT(bvh[i]._parent == -1)
    _roots.push_back(i);
  }
  return buildBVHInner(bvh);
}
int BVHBuilder::buildBVH(std::vector<Node>& bvh) {
  if(bvh.empty())
    return -1;
  //find roots
  _roots.clear();
  for(int i=0; i<(int)bvh.size(); i++)
    if(bvh[i]._parent == -1)
      _roots.push_back(i);
  return buildBVHInner(bvh);
}
int BVHBuilder::depth(const std::vector<Node>& bvh,int root) {
  if(root==-1)
    root=(int)bvh.size()-1;
  if(bvh[root]._l==-1)
    return 1;
  return std::max(depth(bvh,bvh[root]._l),depth(bvh,bvh[root]._r))+1;
}
GLdouble BVHBuilder::depthOptimal(const std::vector<Node>& bvh) {
  return std::log((GLdouble)bvh.size());
}
int BVHBuilder::buildBVHInner(std::vector<Node>& bvh) {
  //short bounding box alone each axis
  for(int d=0; d<3; d++) {
    _hdls[d].clear();
    for(int i=0; i<(int)_roots.size(); i++) {
      _hdls[d].push_back(BVHHandle(minCorner(bvh[_roots[i]]._bb)[d],_roots[i],true));
      _hdls[d].push_back(BVHHandle(maxCorner(bvh[_roots[i]]._bb)[d],_roots[i],false));
    }
    std::sort(_hdls[d].begin(),_hdls[d].end());
  }
  //build BVH
  Eigen::Matrix<int,3,1> f(0,0,0);
  Eigen::Matrix<int,3,1> t((int)_hdls[0].size(),(int)_hdls[1].size(),(int)_hdls[2].size());
  bvh.reserve(bvh.size()+_roots.size()-1);
  int ret=buildBVH(bvh,0,(int)_roots.size(),f,t);
  bvh.back()._parent=-1;
  return ret;
}
//#define DEBUG_BVH
int BVHBuilder::buildBVH(std::vector<Node>& bvh,int fr,int tr,Eigen::Matrix<int,3,1> f,Eigen::Matrix<int,3,1> t) {
  if(tr == fr+1)
    return _roots[fr];
  //find split location
  int lt=fr,rf=tr;
  Eigen::Matrix<int,3,1> ltd=f,rfd=t;
  {
    //calculate split cost
    int minD=-1,minId=-1;
    GLdouble minCost=std::numeric_limits<GLdouble>::max();
    for(int d=0; d<3; d++) {
#ifdef DEBUG_BVH
      {
        std::vector<BVHHandle> hdls;
        for(int i=fr; i<tr; i++) {
          hdls.push_back(BVHHandle(minCorner(bvh[_roots[i]]._bb)[d],_roots[i],true));
          hdls.push_back(BVHHandle(maxCorner(bvh[_roots[i]]._bb)[d],_roots[i],false));
        }
        std::sort(hdls.begin(),hdls.end());

        ASSERT((int)hdls.size() == t[d]-f[d])
        ASSERT((int)hdls.size() == (tr-fr)*2)
        for(int i=0; i<(int)hdls.size(); i++)
          ASSERT(hdls[i] == _hdls[d][i+f[d]]);
      }
#endif
      calcCost(bvh,_hdls[d],f[d],t[d]);
      for(int i=f[d]; i<(int)t[d]; i++) {
#ifdef DEBUG_BVH
        GLdouble costSlow=debugCost(bvh,_hdls[d],f[d],t[d],i);
        GLdouble costFast=_hdls[d][i]._cost;
        ASSERT_MSGV(costSlow == costFast,"%f!=%f",costSlow,costFast);
#endif
        if(_hdls[d][i]._cost < minCost) {
          minCost=_hdls[d][i]._cost;
          minD=d;
          minId=i;
        }
      }
    }
    //find split set
    for(int i=fr; i<tr; i++)
      bvh[_roots[i]]._parent=-1;
    const BVHHandle& minHdl=_hdls[minD][minId];
    for(int i=f[minD]; i<t[minD]; i++) {
      const BVHHandle& hdl=_hdls[minD][i];
      if(bvh[hdl._rid]._parent != -1)
        continue;
      if(hdl._left && hdl <= minHdl) {
        _roots[lt++]=hdl._rid;
        bvh[hdl._rid]._parent=1;
      }
      if(!hdl._left && hdl >= minHdl) {
        _roots[--rf]=hdl._rid;
        bvh[hdl._rid]._parent=2;
      }
    }
    ASSERT_MSG(lt == rf,"Strange Error!");
    if(lt == tr || rf == fr) {
      lt=rf=(fr+tr)/2;
      for(int i=fr; i<lt; i++)
        bvh[_roots[i]]._parent=1;
      for(int i=rf; i<tr; i++)
        bvh[_roots[i]]._parent=2;
    }
    for(int d=0; d<3; d++)
      split(bvh,_hdls[d],ltd[d],rfd[d]);
  }

  //split
  Node n;
  n._l=buildBVH(bvh,fr,lt,f,ltd);
  n._r=buildBVH(bvh,rf,tr,rfd,t);
  bvh[n._l]._parent=(int)bvh.size();
  bvh[n._r]._parent=(int)bvh.size();

  //add new root
  n._bb=bvh[n._l]._bb;
  n._bb=unionBB(n._bb,bvh[n._r]._bb);
  n._nrCell=bvh[n._l]._nrCell+bvh[n._r]._nrCell;
  bvh.push_back(n);
  return bvh.size()-1;
}
void BVHBuilder::calcCost(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int f,int t) {
  int nrC=0;
  Eigen::Matrix<GLdouble,6,1> bb=resetBBD();
  for(int i=f; i<t; i++) {
    X[i]._cost=area(bb)*GLdouble((long)nrC);
    if(X[i]._left) {
      bb=unionBB(bb,bvh[X[i]._rid]._bb);
      nrC+=bvh[X[i]._rid]._nrCell;
    }
  }

  nrC=0;
  bb=resetBBD();
  for(int i=t-1; i>=f; i--) {
    X[i]._cost+=area(bb)*GLdouble((long)nrC);
    if(!X[i]._left) {
      bb=unionBB(bb,bvh[X[i]._rid]._bb);
      nrC+=bvh[X[i]._rid]._nrCell;
    }
  }
}
GLdouble BVHBuilder::debugCost(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int f,int t,int I) {
  GLdouble cost=0.0f;
  int nrC=0;
  Eigen::Matrix<GLdouble,6,1> bb=resetBBD();
  for(int i=f; i<I; i++)
    if(X[i]._left) {
      bb=unionBB(bb,bvh[X[i]._rid]._bb);
      nrC+=bvh[X[i]._rid]._nrCell;
    }
  cost+=area(bb)*(GLdouble)nrC;

  nrC=0;
  bb=resetBBD();
  for(int i=t-1; i>I; i--)
    if(!X[i]._left) {
      bb=unionBB(bb,bvh[X[i]._rid]._bb);
      nrC+=bvh[X[i]._rid]._nrCell;
    }
  cost+=area(bb)*(GLdouble)nrC;

  return cost;
}
void BVHBuilder::split(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int& lt,int& rf) {
  std::vector<BVHHandle> R;
  int j=lt;
  for(int i=lt; i<rf; i++) {
    int p=bvh[X[i]._rid]._parent;
    ASSERT(p == 1 || p == 2)
    if(p == 1) X[j++]=X[i];
    else R.push_back(X[i]);
  }
  lt=rf=j;
  std::copy(R.begin(),R.end(),X.begin()+rf);
}
}
