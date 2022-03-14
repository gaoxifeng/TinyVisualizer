#ifndef VISUAL_TEXTURE_BAKER_H
#define VISUAL_TEXTURE_BAKER_H

#include "TextureBaker.h"

namespace DRAWER {

class VisualTextureBaker : public TextureBaker {
 public:
  VisualTextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res,int resSphere,
                     const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ());
  void bakeTexture() override;
 protected:
  std::vector<Eigen::Matrix<GLdouble,3,1>> _dirs;
};

}

#endif
