#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "Drawer.h"

namespace DRAWER {
class CompositeShape : public Shape {
 public:
  void addShape(std::shared_ptr<Shape> s);
  virtual void setPointSize(GLfloat pointSize) override;
  virtual void setLineWidth(GLfloat lineWidth) override;
  virtual void setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B) override;
  virtual void setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) override;
  virtual void setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) override;
  virtual void setShininess(GLenum mode,GLfloat S) override;
  virtual void setTexture(std::shared_ptr<Texture>) override;
  virtual void draw(bool shadowPass) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  std::shared_ptr<Shape> getChild(int id) const;
  int numChildren() const;
 protected:
  std::vector<std::shared_ptr<Shape>> _shapes;
};
}

#endif
