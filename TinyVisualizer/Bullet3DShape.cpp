#include "Bullet3DShape.h"
#include "MakeMesh.h"
#include "Matrix.h"
#include "SceneStructure.h"
#ifdef BULLET_SUPPORT
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#endif

namespace DRAWER {
Bullet3DShape::Bullet3DShape():_body(NULL),_localTrans(Eigen::Matrix<GLfloat,4,4>::Identity()) {}
#ifdef BULLET_SUPPORT
Bullet3DShape::Bullet3DShape(const btCollisionObject* b,std::shared_ptr<Texture> tex):_body(b),_localTrans(Eigen::Matrix<GLfloat,4,4>::Identity()) {
  createShape(b,tex);
}
void Bullet3DShape::syncWorld(std::shared_ptr<SceneNode>& root,const btDiscreteDynamicsWorld* world,std::shared_ptr<Texture> tex,
                              const Eigen::Matrix<GLfloat,3,1>& c,const Eigen::Matrix<GLfloat,3,1>& cB,GLfloat lineWidth) {
  //objects in shapes
  std::unordered_set<const btCollisionObject*> shapeSet,worldSet;
  std::unordered_set<std::shared_ptr<Bullet3DShape>> currSet;
  if(root)
    root->visit([&](std::shared_ptr<Shape> s) {
    if(std::dynamic_pointer_cast<Bullet3DShape>(s)) {
      shapeSet.insert(std::dynamic_pointer_cast<Bullet3DShape>(s)->_body);
      currSet.insert(std::dynamic_pointer_cast<Bullet3DShape>(s));
    }
    return true;
  });
  //objects in world
  const btCollisionObjectArray& bss=world->getCollisionObjectArray();
  for(int i=0; i<bss.size(); i++) {
    const btCollisionObject* b=bss.at(i);
    worldSet.insert(b);
    //add object if not in shapes
    if(shapeSet.find(b)==shapeSet.end()) {
      std::shared_ptr<Shape> s(new Bullet3DShape(b,tex));
      s->setColor(GL_TRIANGLE_FAN,c[0],c[1],c[2]);
      s->setColor(GL_LINES,cB[0],cB[1],cB[2]);
      s->setLineWidth(lineWidth);
      root=SceneNode::update(root,s);
      shapeSet.insert(b);
    }
  }
  //delete object if not in world
  for(std::shared_ptr<Bullet3DShape> s:currSet)
    if(worldSet.find(s->_body)==worldSet.end())
      root=SceneNode::remove(root,s);
}
#endif
void Bullet3DShape::draw(PASS_TYPE passType) const {
  if(!_enabled)
    return;
  matrixMode(GL_MODELVIEW_MATRIX);
  pushMatrix();
  //global
#ifdef BULLET_SUPPORT
  if(_body) {
    btScalar m[16];
    const btTransform& t=_body->getWorldTransform();
    t.getOpenGLMatrix(m);
    multMatrixf(m);
  }
#endif
  //local
  multMatrixf(_localTrans);
  CompositeShape::draw(passType);
  matrixMode(GL_MODELVIEW_MATRIX);
  popMatrix();
}
Eigen::Matrix<GLfloat,6,1> Bullet3DShape::getBB() const {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB(),retL=resetBB();
  //local
  for(std::shared_ptr<Shape> s:_shapes)
    retL=unionBB(retL,s->getBB());
  retL=transformBB(retL,_localTrans);
  //global
#ifdef BULLET_SUPPORT
  if(_body) {
    const btTransform& t=_body->getWorldTransform();
    for(GLfloat x: {
          retL[0],retL[3]
        })
      for(GLfloat y: {
            retL[1],retL[4]
          })
        for(GLfloat z: {
              retL[2],retL[5]
            }) {
          btVector3 pos=t*btVector3(x,y,z);
          ret=unionBB(ret,Eigen::Matrix<GLfloat,3,1>(pos.x(),pos.y(),pos.z()));
        }
  } else ret=retL;
#else
  ret=retL;
#endif
  return ret;
}
bool Bullet3DShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
  Eigen::Matrix<GLfloat,6,1> rayLocal;
  Eigen::Matrix<GLfloat,3,3> invR=_localTrans.block<3,3>(0,0).inverse();
  rayLocal.segment<3>(0)=invR*(ray.segment<3>(0)-_localTrans.block<3,1>(0,3));
  rayLocal.segment<3>(3)=invR*ray.segment<3>(3);
  return CompositeShape::rayIntersect(rayLocal,alpha);
}
void Bullet3DShape::setLocalTransform(const Eigen::Matrix<GLfloat,4,4>& localTrans) {
  _localTrans=localTrans;
}
void Bullet3DShape::setLocalTranslate(const Eigen::Matrix<GLfloat,3,1>& localTrans) {
  _localTrans.block<3,1>(0,3)=localTrans;
}
void Bullet3DShape::setLocalRotate(const Eigen::Matrix<GLfloat,3,3>& localTrans) {
  _localTrans.block<3,3>(0,0)=localTrans;
}
Eigen::Matrix<GLfloat,4,4> Bullet3DShape::getLocalTransform() const {
  return _localTrans;
}
const btCollisionObject* Bullet3DShape::getBody() const {
  return _body;
}
#ifdef BULLET_SUPPORT
void Bullet3DShape::createShape(const btCollisionObject* b,std::shared_ptr<Texture> tex,int RES) {
  const btCollisionShape* shape=b->getCollisionShape();
  if(dynamic_cast<const btBoxShape*>(shape)) {
    if(!_fillBox) {
      _fillBox=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(1,1,1));
      _borderBox=makeBox(1,false,Eigen::Matrix<GLfloat,3,1>(1,1,1));
      _fillBox->setTexture(tex);
    }
    btVector3 he=dynamic_cast<const btBoxShape*>(shape)->getHalfExtentsWithMargin();
    _localTrans.block<3,3>(0,0).diagonal()=Eigen::Matrix<GLfloat,3,1>(he.x(),he.y(),he.z());
    addShape(_fillBox);
    if(!tex)
      addShape(_borderBox);
  } else if(dynamic_cast<const btSphereShape*>(shape)) {
    if(!_fillSphere) {
      _fillSphere=makeSphere(RES,true,1);
      _borderSphere=makeSphere(RES,false,1);
      _fillSphere->setTexture(tex);
    }
    GLfloat rad=dynamic_cast<const btSphereShape*>(shape)->getRadius();
    _localTrans.block<3,3>(0,0)*=rad;
    addShape(_fillSphere);
    if(!tex)
      addShape(_borderSphere);
  } else if(dynamic_cast<const btCapsuleShape*>(shape)) {
    GLfloat rad=dynamic_cast<const btCapsuleShape*>(shape)->getRadius();
    GLfloat height=dynamic_cast<const btCapsuleShape*>(shape)->getHalfHeight()/rad;
    int upAxis=dynamic_cast<const btCapsuleShape*>(shape)->getUpAxis();
    if(_fillCapsule.find(height)==_fillCapsule.end()) {
      _fillCapsule[height]=makeCapsule(RES,true,1,height);
      _borderCapsule[height]=makeCapsule(RES,false,1,height);
      _fillCapsule[height]->setTexture(tex);
    }
    _localTrans.col(0).setUnit((upAxis+1)%3);
    _localTrans.col(1).setUnit((upAxis+2)%3);
    _localTrans.col(2).setUnit((upAxis+3)%3);
    _localTrans.block<3,3>(0,0)*=rad;
    addShape(_fillCapsule[height]);
    if(!tex)
      addShape(_borderCapsule[height]);
  } else {
    ASSERT_MSG(false,"Supported btCollisionObject!")
  }
}
#endif
}
