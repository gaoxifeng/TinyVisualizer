#ifndef BVH_BUILDER_H
#define BVH_BUILDER_H

#include <TinyVisualizer/DrawerUtility.h>

namespace DRAWER {
extern GLdouble area(const Eigen::Matrix<GLdouble,6,1>& bb);
extern Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> minCorner(const Eigen::Matrix<GLdouble,6,1>& bb);
extern Eigen::Map<const Eigen::Matrix<GLdouble,3,1>> maxCorner(const Eigen::Matrix<GLdouble,6,1>& bb);
extern Eigen::Map<Eigen::Matrix<GLdouble,3,1>> minCorner(Eigen::Matrix<GLdouble,6,1>& bb);
extern Eigen::Map<Eigen::Matrix<GLdouble,3,1>> maxCorner(Eigen::Matrix<GLdouble,6,1>& bb);
extern bool intersectBB(const Eigen::Matrix<GLdouble,6,1>& bb,const Eigen::Matrix<GLdouble,6,1>& ray);
extern bool intersectBB2D(const Eigen::Matrix<GLdouble,6,1>& bb,const Eigen::Matrix<GLdouble,2,1>& b);
extern Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,6,1>& b);
extern Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,3,1>& b);
extern Eigen::Matrix<GLdouble,6,1> unionBB(const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,2,1>& b);
extern Eigen::Matrix<GLdouble,6,1> resetBBD();
struct Node {
  Node():_l(-1),_r(-1),_parent(-1),_nrCell(-1) {}
  Eigen::Matrix<GLdouble,6,1> _bb;
  int _l,_r,_parent,_nrCell;
  int _cell;
};
struct BVHBuilder {
 public:
  struct BVHHandle {
    BVHHandle():_cost(0.0f),_left(false) {}
    BVHHandle(GLdouble val,int rid,bool left):_val(val),_cost(0.0f),_rid(rid),_left(left) {}
    bool operator<(const BVHHandle& other) const {
      return _val<other._val;
    }
    bool operator<=(const BVHHandle& other) const {
      return _val<=other._val;
    }
    bool operator>(const BVHHandle& other) const {
      return _val>other._val;
    }
    bool operator>=(const BVHHandle& other) const {
      return _val>=other._val;
    }
    bool operator==(const BVHHandle& other) const {
      return _val==other._val;
    }
    GLdouble _val,_cost;
    int _rid;
    bool _left;
  };
  int buildBVH(std::vector<Node>& bvh,int f,int t);
  int buildBVH(std::vector<Node>& bvh);
  static int depth(const std::vector<Node>& bvh,int root=-1);
  static GLdouble depthOptimal(const std::vector<Node>& bvh);
 private:
  int buildBVHInner(std::vector<Node>& bvh);
  int buildBVH(std::vector<Node>& bvh,int fr,int tr,Eigen::Matrix<int,3,1> f,Eigen::Matrix<int,3,1> t);
  static void calcCost(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int f,int t);
  static GLdouble debugCost(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int f,int t,int I);
  static void split(const std::vector<Node>& bvh,std::vector<BVHHandle>& X,int& lt,int& rf);
  std::vector<BVHHandle> _hdls[3];
  std::vector<int> _roots;
};
}

#endif
