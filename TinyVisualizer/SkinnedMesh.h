#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "Bullet3DShape.h"

namespace DRAWER {
class SkinnedMeshShape : public Bullet3DShape {
 public:
  SkinnedMeshShape(const std::string& filename);
 private:
  struct BoneInfo {
    Eigen::Matrix<GLfloat,4,4> _offsetMatrix;
    Eigen::Matrix<GLfloat,4,4> _finalTransformation;
  };
  std::vector<BoneInfo> _bones;
};
}

#endif
