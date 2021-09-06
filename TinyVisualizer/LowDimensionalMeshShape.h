#ifndef LOW_DIMENSIONAL_MESH_SHAPE_H
#define LOW_DIMENSIONAL_MESH_SHAPE_H

#include "MeshShape.h"

namespace DRAWER {
class LowDimensionalMeshShape : public Shape {
 public:
  LowDimensionalMeshShape(std::shared_ptr<MeshShape> inner);
  virtual void setPointSize(GLfloat pointSize) override;
  virtual void setLineWidth(GLfloat lineWidth) override;
  virtual void setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B) override;
  virtual void setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) override;
  virtual void setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) override;
  virtual void setTexture(std::shared_ptr<Texture> tex) override;
  virtual void setDepth(GLfloat depth) override;
  virtual void setDrawer(Drawer* drawer) override;
  virtual void setShininess(GLenum mode,GLfloat S) override;
  virtual bool needRecomputeNormal() const override;
  virtual void draw(PASS_TYPE passType) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const;
  void setLowToHighDimensionalMapping(const Eigen::Matrix<GLfloat,-1,-1>& DHDL);
  void updateHighDimensionalMapping(const Eigen::Matrix<GLfloat,-1,1>& L);
 protected:
  std::shared_ptr<VBO> _transformFeedbackVBO;
  std::shared_ptr<Texture> _HMap,_LCoord;
  std::shared_ptr<MeshShape> _inner;
  bool _dirtyCPU;
  //bounding box
  Eigen::Matrix<GLfloat,-1,1> _L;
  Eigen::Matrix<GLfloat,6,1> _BBBase;
  Eigen::Matrix<GLfloat,3,-1> _DHDLMax;
  bool _dirtyBB;
};
}

#endif
