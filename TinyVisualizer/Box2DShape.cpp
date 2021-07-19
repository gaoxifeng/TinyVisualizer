#include "Box2DShape.h"
#include "MakeMesh.h"
#include "SceneStructure.h"
#ifdef BOX2D_SUPPORT
#include <Box2D/Box2D.h>
#endif
#include <unordered_set>

namespace DRAWER {
Box2DShape::Box2DShape():_body(NULL),_posx(0),_posy(0),_scale(1) {}
#ifdef BOX2D_SUPPORT
Box2DShape::Box2DShape(const b2Body* b,int RES):_body(b),_posx(0),_posy(0),_scale(1) {
  createShape(b,RES);
}
void Box2DShape::syncWorld(std::shared_ptr<SceneNode>& root,const b2World* world,
                           const Eigen::Matrix<GLfloat,3,1>& c,const Eigen::Matrix<GLfloat,3,1>& cB,
                           GLfloat lineWidth) {
  //objects in shapes
  std::unordered_set<const b2Body*> shapeSet,worldSet;
  std::unordered_set<std::shared_ptr<Box2DShape>> currSet;
  if(root)
    root->visit([&](std::shared_ptr<Shape> s) {
    if(std::dynamic_pointer_cast<Box2DShape>(s)) {
      shapeSet.insert(std::dynamic_pointer_cast<Box2DShape>(s)->_body);
      currSet.insert(std::dynamic_pointer_cast<Box2DShape>(s));
    }
    return true;
  });
  //objects in world
  const b2Body* b=world->GetBodyList();
  while(b) {
    worldSet.insert(b);
    //add object if not in shapes
    if(shapeSet.find(b)==shapeSet.end()) {
      std::shared_ptr<Shape> s(new Box2DShape(b));
      s->setColor(GL_TRIANGLE_FAN,c[0],c[1],c[2]);
      s->setColor(GL_LINE_LOOP,cB[0],cB[1],cB[2]);
      s->setLineWidth(lineWidth);
      root=SceneNode::update(root,s);
      shapeSet.insert(b);
    }
    b=b->GetNext();
  }
  //delete object if not in world
  for(std::shared_ptr<Box2DShape> s:currSet)
    if(worldSet.find(s->_body)==worldSet.end())
      root=SceneNode::remove(root,s);
}
#endif
void Box2DShape::draw(bool shadowPass) const {
  if(!_enabled)
    return;
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  //global
#ifdef BOX2D_SUPPORT
  if(_body) {
    glTranslatef(_body->GetPosition().x,_body->GetPosition().y,0);
    glRotatef(_body->GetAngle()*180/M_PI,0,0,1);
  }
#endif
  //local
  glTranslatef(_posx,_posy,0);
  glScalef(_scale,_scale,_scale);
  CompositeShape::draw(shadowPass);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}
Eigen::Matrix<GLfloat,6,1> Box2DShape::getBB() const {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB(),retL=resetBB();
  //local
  for(std::shared_ptr<Shape> s:_shapes)
    retL=unionBB(retL,s->getBB());
  retL=scaleBB(retL,Eigen::Matrix<GLfloat,3,1>(_scale,_scale,_scale));
  retL=translateBB(retL,Eigen::Matrix<GLfloat,3,1>(_posx,_posy,0));
  //global
#ifdef BOX2D_SUPPORT
  if(_body) {
    for(GLfloat x: {
          retL[0],retL[3]
        })
      for(GLfloat y: {
            retL[1],retL[4]
          }) {
        b2Vec2 pos=b2Mul(_body->GetTransform(),b2Vec2(x,y));
        ret=unionBB(ret,Eigen::Matrix<GLfloat,3,1>(pos.x,pos.y,0));
      }
  } else ret=retL;
#else
  ret=retL;
#endif
  return ret;
}
void Box2DShape::setLocalTransform(GLfloat posx,GLfloat posy,GLfloat scale) {
  _posx=posx;
  _posy=posy;
  _scale=scale;
}
const b2Body* Box2DShape::getBody() const {
  return _body;
}
#ifdef BOX2D_SUPPORT
void Box2DShape::createShape(const b2Body* b,int RES) {
  const b2Fixture* f=b->GetFixtureList();
  while(f) {
    const b2Shape* s=f->GetShape();
    if(dynamic_cast<const b2EdgeShape*>(s)) {
      const b2EdgeShape* se=dynamic_cast<const b2EdgeShape*>(s);
      std::shared_ptr<MeshShape> border(new MeshShape);
      border->addVertex(Eigen::Matrix<GLfloat,3,1>(se->m_vertex1.x,se->m_vertex1.y,0));
      border->addVertex(Eigen::Matrix<GLfloat,3,1>(se->m_vertex2.x,se->m_vertex2.y,0));
      border->addIndexSingle(0);
      border->addIndexSingle(1);
      border->setMode(GL_LINES);
      addShape(border);
    } else if(dynamic_cast<const b2CircleShape*>(s)) {
      static std::shared_ptr<MeshShape> fill,border;
      if(!fill) {
        fill=makeCircle(RES,true,Eigen::Matrix<GLfloat,2,1>(0,0),1);
        border=makeCircle(RES,false,Eigen::Matrix<GLfloat,2,1>(0,0),1);
      }
      const b2CircleShape* sc=dynamic_cast<const b2CircleShape*>(s);
      _posx=sc->m_p.x;
      _posy=sc->m_p.y;
      _scale=sc->m_radius;
      addShape(fill);
      addShape(border);
    } else if(dynamic_cast<const b2PolygonShape*>(s)) {
      const b2PolygonShape* sp=dynamic_cast<const b2PolygonShape*>(s);
      std::shared_ptr<MeshShape> interior(new MeshShape);
      std::shared_ptr<MeshShape> border(new MeshShape);
      for(int i=0; i<sp->m_count; i++) {
        interior->addVertex(Eigen::Matrix<GLfloat,3,1>(sp->m_vertices[i].x,sp->m_vertices[i].y,0));
        border->addVertex(Eigen::Matrix<GLfloat,3,1>(sp->m_vertices[i].x,sp->m_vertices[i].y,0));
        interior->addIndexSingle(i);
        border->addIndexSingle(i);
      }
      interior->setMode(GL_TRIANGLE_FAN);
      border->setMode(GL_LINE_LOOP);
      addShape(interior);
      addShape(border);
    }
    f=f->GetNext();
  }
}
#endif
}