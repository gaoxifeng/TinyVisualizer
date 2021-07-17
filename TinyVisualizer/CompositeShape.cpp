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
std::shared_ptr<Shape> CompositeShape::getChild(int id) const {
  return _shapes[id];
}
int CompositeShape::numChildren() const {
  return (int)_shapes.size();
}
}
