#include "CompositeShape.h"

namespace DRAWER {
//CompositeShape
void CompositeShape::addShape(std::shared_ptr<Shape> s) {
  _shapes.push_back(s);
}
void CompositeShape::setPointSize(GLfloat pointSize) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setPointSize(pointSize);
}
void CompositeShape::setLineWidth(GLfloat lineWidth) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setLineWidth(lineWidth);
}
void CompositeShape::setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorDiffuse(mode,R,G,B);
}
void CompositeShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorAmbient(mode,RA,GA,BA);
}
void CompositeShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorSpecular(mode,RS,GS,BS);
}
void CompositeShape::setShininess(GLenum mode,GLfloat S) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setShininess(mode,S);
}
void CompositeShape::setTextureDiffuse(std::shared_ptr<Texture> tex) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setTextureDiffuse(tex);
}
void CompositeShape::setTextureSpecular(std::shared_ptr<Texture> tex) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setTextureSpecular(tex);
}
void CompositeShape::setDepth(GLfloat depth) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setDepth(depth);
}
void CompositeShape::setDrawer(Drawer* drawer) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setDrawer(drawer);
}
bool CompositeShape::needRecomputeNormal() const {
  for(int i=0; i<(int)_shapes.size(); i++)
    if(_shapes[i]->needRecomputeNormal())
      return true;
  return false;
}
void CompositeShape::draw(PASS_TYPE passType) const {
  if(!enabled())
    return;
  if(!_castShadow && (passType&SHADOW_PASS)!=0)
    return;
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->draw(passType);
}
Eigen::Matrix<GLfloat,6,1> CompositeShape::getBB() const {
  Eigen::Matrix<GLfloat,6,1> bb=resetBB();
  for(std::shared_ptr<Shape> s:_shapes)
    bb=unionBB(bb,s->getBB());
  return bb;
}
bool CompositeShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
  bool ret=false;
  for(int i=0; i<(int)_shapes.size(); i++)
    if(_shapes[i]->rayIntersect(ray,alpha))
      ret=true;
  return ret;
}
void CompositeShape::updateChild(std::shared_ptr<Shape> s,int id) {
  ASSERT_MSGV(id<numChildren(),
              "User called CompositeShape::updateChild with id=%d, but there are only %d children!",
              id,numChildren())
  _shapes[id]=s;
}
std::shared_ptr<Shape> CompositeShape::getChild(int id) const {
  return _shapes[id];
}
bool CompositeShape::contain(std::shared_ptr<Shape> s) const {
  if(s.get()==this)
    return true;
  for(int i=0; i<numChildren(); i++)
    if(s==getChild(i))
      return true;
    else {
      std::shared_ptr<CompositeShape> c=std::dynamic_pointer_cast<CompositeShape>(getChild(i));
      if(c && c->contain(s))
        return true;
    }
  return false;
}
void CompositeShape::removeChild(std::shared_ptr<Shape> shape) {
  auto end=std::remove(_shapes.begin(),_shapes.end(),shape);
  _shapes.erase(end,_shapes.end());
}
int CompositeShape::numChildren() const {
  return (int)_shapes.size();
}
}
