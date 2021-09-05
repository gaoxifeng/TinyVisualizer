#ifndef ARROW_SHAPE_H
#define ARROW_SHAPE_H

#include "MeshShape.h"

namespace DRAWER {
class ArrowShape : public MeshShape {
 public:
  ArrowShape(GLfloat angle,GLfloat thickness,GLfloat thicknessOuter,int RES=32);
  void setArrow(const Eigen::Matrix<GLfloat,3,1>& from,
                const Eigen::Matrix<GLfloat,3,1>& to);
  virtual void draw(PASS_TYPE passType) const override;
 protected:
  Eigen::Matrix<GLfloat,4,4> _T;
  Eigen::Matrix<GLfloat,6,1> _bbRef;
  std::vector<GLfloat> _verticesRef;
  std::vector<GLuint> _indicesRef;
  GLfloat _tipLen;
};
}

#endif
