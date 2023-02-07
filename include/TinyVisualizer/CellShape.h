#ifndef CELL_SHAPE_H
#define CELL_SHAPE_H

#include "MeshShape.h"
#include <unordered_set>

namespace DRAWER {
class CellShape : public MeshShape {
 public:
  CellShape();
  CellShape(const std::vector<Eigen::Matrix<int,3,1>>& ids,GLfloat res,bool discreteNormal=true);
  void reset(const std::vector<Eigen::Matrix<int,3,1>>& ids,GLfloat res,bool discreteNormal=true);
 private:
  static Eigen::Matrix<int,4,1> FACE(int d);
};
}

#endif
