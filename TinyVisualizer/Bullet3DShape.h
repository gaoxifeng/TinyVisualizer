#ifndef BULLET3D_SHAPE_H
#define BULLET3D_SHAPE_H

#include "Drawer.h"
#include "MeshShape.h"
#include "CompositeShape.h"
#include <unordered_map>
#include <unordered_set>

class btCollisionObject;
class btDiscreteDynamicsWorld;
namespace DRAWER {
class Texture;
class Bullet3DShape : public CompositeShape {
 public:
  Bullet3DShape();
#ifdef BULLET_SUPPORT
  Bullet3DShape(const btCollisionObject* b,std::shared_ptr<Texture> tex=NULL);
  static void syncWorld(std::shared_ptr<SceneNode>& root,const btDiscreteDynamicsWorld* world,std::shared_ptr<Texture> tex=NULL,
                        const Eigen::Matrix<GLfloat,3,1>& c=Eigen::Matrix<GLfloat,3,1>(1,1,1),
                        const Eigen::Matrix<GLfloat,3,1>& cB=Eigen::Matrix<GLfloat,3,1>(.7,.7,.7),
                        GLfloat lineWidth=5);
#endif
  virtual void draw(PASS_TYPE passType) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const override;
  void setLocalTransform(const Eigen::Matrix<GLfloat,4,4>& localTrans);
  void setLocalTranslate(const Eigen::Matrix<GLfloat,3,1>& localTrans);
  void setLocalRotate(const Eigen::Matrix<GLfloat,3,3>& localTrans);
  Eigen::Matrix<GLfloat,4,4> getLocalTransform() const;
  const btCollisionObject* getBody() const;
 private:
#ifdef BULLET_SUPPORT
  void createShape(const btCollisionObject* b,std::shared_ptr<Texture> tex=NULL,int RES=8);
  std::shared_ptr<MeshShape> _fillBox,_borderBox;
  std::shared_ptr<MeshShape> _fillSphere,_borderSphere;
  std::unordered_map<GLfloat,std::shared_ptr<MeshShape>> _fillCapsule,_borderCapsule;
#endif
  const btCollisionObject* _body;
  std::shared_ptr<Shape> _child;
  Eigen::Matrix<GLfloat,4,4> _localTrans;
};
}

#endif
