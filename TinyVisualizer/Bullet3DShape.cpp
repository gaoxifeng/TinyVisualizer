#include "Bullet3DShape.h"
#include "MakeMesh.h"
#include "SceneStructure.h"
#ifdef BULLET_SUPPORT
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#endif
#include <unordered_map>
#include <unordered_set>

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
void Bullet3DShape::draw(bool shadowPass) const {
  if(!_enabled)
    return;
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  //global
#ifdef BULLET_SUPPORT
  if(_body) {
    btScalar m[16];
    const btTransform& t=_body->getWorldTransform();
    t.getOpenGLMatrix(m);
    glMultMatrixf(m);
  }
#endif
  //local
  glMultMatrixf(_localTrans.data());
  CompositeShape::draw(shadowPass);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
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
void Bullet3DShape::setLocalTransform(const Eigen::Matrix<GLfloat,4,4>& localTrans) {
  _localTrans=localTrans;
}
void Bullet3DShape::setLocalTranslate(const Eigen::Matrix<GLfloat,3,1>& localTrans) {
  _localTrans.block<3,1>(0,3)=localTrans;
}
void Bullet3DShape::setLocalRotate(const Eigen::Matrix<GLfloat,3,3>& localTrans) {
  _localTrans.block<3,3>(0,0)=localTrans;
}
const btCollisionObject* Bullet3DShape::getBody() const {
  return _body;
}
#ifdef BULLET_SUPPORT
void Bullet3DShape::createShape(const btCollisionObject* b,std::shared_ptr<Texture> tex,int RES) {
  const btCollisionShape* shape=b->getCollisionShape();
  if(dynamic_cast<const btBoxShape*>(shape)) {
    static std::shared_ptr<MeshShape> fillBox,borderBox;
    if(!fillBox) {
      fillBox=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(1,1,1));
      borderBox=makeBox(1,false,Eigen::Matrix<GLfloat,3,1>(1,1,1));
      fillBox->setTexture(tex);
    }
    btVector3 he=dynamic_cast<const btBoxShape*>(shape)->getHalfExtentsWithMargin();
    _localTrans.block<3,3>(0,0).diagonal()=Eigen::Matrix<GLfloat,3,1>(he.x(),he.y(),he.z());
    addShape(fillBox);
    if(!tex)
      addShape(borderBox);
  } else if(dynamic_cast<const btSphereShape*>(shape)) {
    static std::shared_ptr<MeshShape> fillSphere,borderSphere;
    if(!fillSphere) {
      fillSphere=makeSphere(RES,true,1);
      borderSphere=makeSphere(RES,false,1);
      fillSphere->setTexture(tex);
    }
    GLfloat rad=dynamic_cast<const btSphereShape*>(shape)->getRadius();
    _localTrans.block<3,3>(0,0)*=rad;
    addShape(fillSphere);
    if(!tex)
      addShape(borderSphere);
  } else if(dynamic_cast<const btCapsuleShape*>(shape)) {
    static std::unordered_map<GLfloat,std::shared_ptr<MeshShape>> fillCapsule,borderCapsule;
    GLfloat rad=dynamic_cast<const btCapsuleShape*>(shape)->getRadius();
    GLfloat height=dynamic_cast<const btCapsuleShape*>(shape)->getHalfHeight()/rad;
    int upAxis=dynamic_cast<const btCapsuleShape*>(shape)->getUpAxis();
    if(fillCapsule.find(height)==fillCapsule.end()) {
      fillCapsule[height]=makeCapsule(RES,true,1,height);
      borderCapsule[height]=makeCapsule(RES,false,1,height);
      fillCapsule[height]->setTexture(tex);
    }
    _localTrans.col(0).setUnit((upAxis+1)%3);
    _localTrans.col(1).setUnit((upAxis+2)%3);
    _localTrans.col(2).setUnit((upAxis+3)%3);
    _localTrans.block<3,3>(0,0)*=rad;
    addShape(fillCapsule[height]);
    if(!tex)
      addShape(borderCapsule[height]);
  } else {
    ASSERT_MSG(false,"Supported btCollisionObject!")
  }
}
#endif
}
