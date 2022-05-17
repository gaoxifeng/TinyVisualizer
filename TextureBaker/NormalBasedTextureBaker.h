#ifndef NORMAL_BASED_TEXTURE_BAKER_H
#define NORMAL_BASED_TEXTURE_BAKER_H

#include "TextureBaker.h"

namespace DRAWER {

class NormalBasedTextureBaker : public TextureBaker {
 public:
  NormalBasedTextureBaker(const MeshVisualizer& high,MeshVisualizer& low,int res);
  void bakeTexture() override;
};

}

#endif
