#ifndef SCENE_STRUCTURE_H
#define SCENE_STRUCTURE_H

#include "DrawerUtility.h"
#include <memory>

namespace DRAWER {
class Shape;
class SceneNode {
 public:
  SceneNode();
  SceneNode(const Eigen::Matrix<int,3,1>& loc,int sz);
  static std::shared_ptr<SceneNode> update(std::shared_ptr<SceneNode> root,std::shared_ptr<Shape> s=NULL);
  static std::shared_ptr<SceneNode> remove(std::shared_ptr<SceneNode> root,std::shared_ptr<Shape> s);
  void update(std::shared_ptr<Shape>& toBeAdjusted);
  bool remove(std::shared_ptr<Shape> s);
  void tryAssign(std::shared_ptr<SceneNode> s);
  bool tryAssign(std::shared_ptr<Shape> s,const Eigen::Matrix<GLfloat,6,1>& bb);
  void tryAssignInner(std::shared_ptr<Shape> s,const Eigen::Matrix<GLfloat,6,1>& bb);
  void merge(std::shared_ptr<SceneNode> s);
  int nrShape() const;
  int nrChildren() const;
  bool isLeaf() const;
  bool empty() const;
  void check() const;
  Eigen::Matrix<GLfloat,6,1> getBB() const;
  void visit(std::function<bool(const SceneNode&)> f) const;
  bool visit(std::function<bool(std::shared_ptr<Shape>)> f) const;
  bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,std::shared_ptr<Shape>& IShape,GLfloat& IAlpha) const;
  void draw(std::function<void(std::shared_ptr<Shape>)> f,const Eigen::Matrix<GLfloat,-1,1>* viewFrustum) const;
 protected:
  static bool contain(const Eigen::Matrix<int,3,1>& loc,int sz,const Eigen::Matrix<GLfloat,6,1>& bb);
  static bool contain(const Eigen::Matrix<int,3,1>& loc,int sz,const SceneNode& sc);
  static bool contain(const SceneNode& sc0,const Eigen::Matrix<GLfloat,6,1>& bb);
  static bool contain(const SceneNode& sc0,const SceneNode& sc);
  static std::shared_ptr<Shape> insertList(std::shared_ptr<Shape> s0,std::shared_ptr<Shape> sNew);
  static int sizeList(std::shared_ptr<Shape> s);
  int _sz,_nrShape;
  Eigen::Matrix<int,3,1> _loc;
  std::shared_ptr<Shape> _shapes;
  std::shared_ptr<SceneNode> _children[8];
  static GLfloat _RES;
};
}

#endif
