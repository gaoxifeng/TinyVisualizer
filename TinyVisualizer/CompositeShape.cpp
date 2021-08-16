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
void CompositeShape::setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColor(mode,R,G,B);
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
void CompositeShape::setTexture(std::shared_ptr<Texture> tex) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setTexture(tex);
}
void CompositeShape::setDepth(GLfloat depth) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setDepth(depth);
}
void CompositeShape::draw(bool shadowPass) const {
  if(!enabled() || (!_castShadow && shadowPass))
    return;
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->draw(shadowPass);
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
      return c && c->contain(s);
    }
  return false;
}
int CompositeShape::numChildren() const {
  return (int)_shapes.size();
}
}
