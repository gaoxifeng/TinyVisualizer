#ifndef TERRAIN_SHAPE_H
#define TERRAIN_SHAPE_H

#include "MeshShape.h"

namespace DRAWER {
class EXPORT TerrainShape : public MeshShape {
  RTTI_DECLARE_TYPEINFO(TerrainShape, MeshShape);
 public:
  TerrainShape(const Eigen::Matrix<GLfloat,-1,-1>& height,int upAxis,
               const Eigen::Matrix<GLfloat,3,1>& scale=Eigen::Matrix<GLfloat,3,1>(1,1,1),
               const Eigen::Matrix<GLfloat,2,1>& tcMult=Eigen::Matrix<GLfloat,2,1>(16,16));
  TerrainShape(std::function<GLfloat(GLfloat,GLfloat)> height,int upAxis,
               const Eigen::Matrix<GLfloat,6,1>& aabb,GLfloat RES,
               const Eigen::Matrix<GLfloat,2,1>& tcMult=Eigen::Matrix<GLfloat,2,1>(16,16));
};
}

#endif
