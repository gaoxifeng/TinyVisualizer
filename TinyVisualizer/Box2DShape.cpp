#include "Box2DShape.h"
#include "MakeMesh.h"
#include "Matrix.h"
#include "SceneStructure.h"
#ifdef BOX2D_SUPPORT
#include <box2d/box2d.h>
#endif
#include <unordered_set>

namespace DRAWER {
Box2DShape::Box2DShape():_posx(0),_posy(0),_scale(1) {
#ifdef BOX2D_SUPPORT
  _bodyId = b2_nullBodyId;
#endif
}
#ifdef BOX2D_SUPPORT
Box2DShape::Box2DShape(b2BodyId bodyId,int RES):_bodyId(bodyId),_posx(0),_posy(0),_scale(1) {
  createShape(bodyId,RES);
}
void Box2DShape::syncWorld(std::shared_ptr<SceneNode>& root,b2WorldId worldId,
                           std::vector<b2BodyId>& bodyList,
                           const Eigen::Matrix<GLfloat,3,1>& c,const Eigen::Matrix<GLfloat,3,1>& cB,
                           GLfloat lineWidth) {
  //collect body IDs currently represented in scene shapes
  std::unordered_set<uint64_t> shapeSet,worldSet;
  std::unordered_set<std::shared_ptr<Box2DShape>> currSet;
  if(root)
    root->visit([&](std::shared_ptr<Shape> s) {
    if(std::custom_pointer_cast<Box2DShape>(s)) {
      shapeSet.insert(b2StoreBodyId(std::custom_pointer_cast<Box2DShape>(s)->_bodyId));
      currSet.insert(std::custom_pointer_cast<Box2DShape>(s));
    }
    return true;
  });
  //iterate caller-provided body list
  for(const b2BodyId& bid : bodyList) {
    if(!b2Body_IsValid(bid))
      continue;
    uint64_t key = b2StoreBodyId(bid);
    worldSet.insert(key);
    if(shapeSet.find(key)==shapeSet.end()) {
      std::shared_ptr<Shape> s(new Box2DShape(bid));
      s->setColorDiffuse(GL_TRIANGLE_FAN,c[0],c[1],c[2]);
      s->setColorDiffuse(GL_LINE_LOOP,cB[0],cB[1],cB[2]);
      s->setLineWidth(lineWidth);
      root=SceneNode::update(root,s);
      shapeSet.insert(key);
    }
  }
  //delete shapes whose bodies no longer exist in the world
  for(std::shared_ptr<Box2DShape> s:currSet)
    if(worldSet.find(b2StoreBodyId(s->_bodyId))==worldSet.end())
      root=SceneNode::remove(root,s);
}
b2BodyId Box2DShape::getBody() const {
  return _bodyId;
}
#else
const void* Box2DShape::getBody() const {
  return nullptr;
}
#endif
void Box2DShape::draw(PASS_TYPE passType) const {
  if(!_enabled)
    return;
  matrixMode(GLModelViewMatrix);
  pushMatrix();
  //global
#ifdef BOX2D_SUPPORT
  if(B2_IS_NON_NULL(_bodyId)) {
    b2Vec2 pos=b2Body_GetPosition(_bodyId);
    float angle=b2Rot_GetAngle(b2Body_GetRotation(_bodyId));
    translatef(pos.x,pos.y,0);
    rotatef(angle*180.0f/(float)M_PI,0,0,1);
  }
#endif
  //local
  translatef(_posx,_posy,0);
  scalef(_scale,_scale,_scale);
  CompositeShape::draw(passType);
  matrixMode(GLModelViewMatrix);
  popMatrix();
}
void Box2DShape::drawPovray(Povray& pov) const {
  if(!_enabled)
    return;
  matrixMode(GLModelViewMatrix);
  pushMatrix();
  //local
  translatef(_posx,_posy,0);
  scalef(_scale,_scale,_scale);
  CompositeShape::drawPovray(pov);
  matrixMode(GLModelViewMatrix);
  popMatrix();
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
  if(B2_IS_NON_NULL(_bodyId)) {
    for(GLfloat x: {
          retL[0],retL[3]
        })
      for(GLfloat y: {
            retL[1],retL[4]
          }) {
        b2Vec2 pt={x,y};
        b2Vec2 pos=b2TransformPoint(b2Body_GetTransform(_bodyId),pt);
        ret=unionBB(ret,Eigen::Matrix<GLfloat,3,1>(pos.x,pos.y,0));
      }
  } else ret=retL;
#else
  ret=retL;
#endif
  return ret;
}
bool Box2DShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
  Eigen::Matrix<GLfloat,6,1> rayLocal;
  Eigen::Matrix<GLfloat,4,4> localTrans;
  localTrans.setIdentity();
  localTrans(0,3)=_posx;
  localTrans(1,3)=_posy;
  localTrans(0,0)=_scale;
  localTrans(1,1)=_scale;

  Eigen::Matrix<GLfloat,3,3> invR=localTrans.block<3,3>(0,0).inverse();
  rayLocal.segment<3>(0)=invR*(ray.segment<3>(0)-localTrans.block<3,1>(0,3));
  rayLocal.segment<3>(3)=invR*ray.segment<3>(3);
  return CompositeShape::rayIntersect(rayLocal,alpha);
}
void Box2DShape::setLocalTransform(GLfloat posx,GLfloat posy,GLfloat scale) {
  _posx=posx;
  _posy=posy;
  _scale=scale;
}
#ifdef BOX2D_SUPPORT
void Box2DShape::createShape(b2BodyId bodyId,int RES) {
  int shapeCount=b2Body_GetShapeCount(bodyId);
  std::vector<b2ShapeId> shapes(shapeCount);
  b2Body_GetShapes(bodyId,shapes.data(),shapeCount);
  for(int i=0; i<shapeCount; i++) {
    b2ShapeType type=b2Shape_GetType(shapes[i]);
    if(type==b2_segmentShape) {
      b2Segment seg=b2Shape_GetSegment(shapes[i]);
      std::shared_ptr<MeshShape> border(new MeshShape);
      border->addVertex(Eigen::Matrix<GLfloat,3,1>(seg.point1.x,seg.point1.y,0));
      border->addVertex(Eigen::Matrix<GLfloat,3,1>(seg.point2.x,seg.point2.y,0));
      border->addIndexSingle(0);
      border->addIndexSingle(1);
      border->setMode(GL_LINES);
      addShape(border);
    } else if(type==b2_circleShape) {
      if(!_fill) {
        _fill=makeCircle(RES,true,Eigen::Matrix<GLfloat,2,1>(0,0),1);
        _border=makeCircle(RES,false,Eigen::Matrix<GLfloat,2,1>(0,0),1);
      }
      b2Circle circle=b2Shape_GetCircle(shapes[i]);
      _posx=circle.center.x;
      _posy=circle.center.y;
      _scale=circle.radius;
      addShape(_fill);
      addShape(_border);
    } else if(type==b2_polygonShape) {
      b2Polygon poly=b2Shape_GetPolygon(shapes[i]);
      std::shared_ptr<MeshShape> interior(new MeshShape);
      std::shared_ptr<MeshShape> border(new MeshShape);
      for(int j=0; j<poly.count; j++) {
        interior->addVertex(Eigen::Matrix<GLfloat,3,1>(poly.vertices[j].x,poly.vertices[j].y,0));
        border->addVertex(Eigen::Matrix<GLfloat,3,1>(poly.vertices[j].x,poly.vertices[j].y,0));
        interior->addIndexSingle(j);
        border->addIndexSingle(j);
      }
      interior->setMode(GL_TRIANGLE_FAN);
      border->setMode(GL_LINE_LOOP);
      addShape(interior);
      addShape(border);
    }
  }
}
#endif
}
