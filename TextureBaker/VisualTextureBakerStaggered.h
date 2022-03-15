#ifndef VISUAL_TEXTURE_BAKER_STAGGERED_H
#define VISUAL_TEXTURE_BAKER_STAGGERED_H

#include "TextureBaker.h"
#include <TinyVisualizer/Texture.h>

namespace DRAWER {

class VisualTextureBakerStaggered : public TextureBaker {
 public:
  VisualTextureBakerStaggered(const MeshVisualizer& high,MeshVisualizer& low,int res,int resSphere,
                              const Eigen::Matrix<GLdouble,3,1>& g=-Eigen::Matrix<GLdouble,3,1>::UnitZ());
  void setLaplaceRegularization(GLdouble LReg);
  void bakeTexture() override;
 protected:
  void solveLinearSystem(const Eigen::Matrix<GLdouble,4,1>* num,const GLdouble* denom) const;
  bool applyGaussSeidel(int w,int h,int width,int height,Eigen::Matrix<unsigned char,4,1>* data,
                        const Eigen::Matrix<GLdouble,4,1>* num,const GLdouble* denom) const;
  std::vector<Eigen::Matrix<GLdouble,3,1>> _dirs;
  GLdouble _LReg;
};

}

#endif
