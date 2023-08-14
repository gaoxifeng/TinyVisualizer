#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "Drawer.h"

namespace DRAWER {
class CompositeShape : public Shape {
 public:
  void addShape(std::shared_ptr<Shape> s);
  virtual void setPointSize(GLfloat pointSize) override;
  virtual void setLineWidth(GLfloat lineWidth) override;
  virtual void setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B) override;
  virtual void setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) override;
  virtual void setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) override;
  virtual void setShininess(GLenum mode,GLfloat S) override;
  virtual void setTextureDiffuse(std::shared_ptr<Texture>) override;
  virtual void setTextureSpecular(std::shared_ptr<Texture>) override;
  virtual void setDepth(GLfloat depth) override;
  virtual void setDrawer(Drawer* drawer) override;
  virtual bool needRecomputeNormal() const override;
  virtual void draw(PASS_TYPE passType) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const override;
  void updateChild(std::shared_ptr<Shape> s,int id);
  std::shared_ptr<Shape> getChild(int id) const;
  bool contain(std::shared_ptr<Shape> s) const;
  void removeChild(std::shared_ptr<Shape> shape);
  int numChildren() const;
 protected:
  std::vector<std::shared_ptr<Shape>> _shapes;
};
}

#endif
