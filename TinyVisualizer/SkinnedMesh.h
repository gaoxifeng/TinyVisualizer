#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "Bullet3DShape.h"

namespace DRAWER {
class SkinnedMeshShape : public Bullet3DShape {
 public:
  SkinnedMeshShape(const std::string& filename);
 private:
};
}

#endif
