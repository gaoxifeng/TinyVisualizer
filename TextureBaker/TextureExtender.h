#ifndef TEXTURE_EXTENDER_H
#define TEXTURE_EXTENDER_H

#include <TinyVisualizer/Texture.h>
#include "NormalBasedTextureBaker.h"

namespace DRAWER {

class TextureExtender {
 public:
  typedef NormalBasedTextureBaker::TexelIntersect TexelIntersect;
  TextureExtender(std::shared_ptr<Texture> texture);
  virtual ~TextureExtender();
  virtual void extend(const std::vector<TexelIntersect>& intersect)=0;
 protected:
  std::shared_ptr<Texture> _texture;
};
class NearestTextureExtender : public TextureExtender {
 public:
  NearestTextureExtender(std::shared_ptr<Texture> texture);
  void extend(const std::vector<TexelIntersect>& intersect) override;
};
class LaplaceTextureExtender : public NearestTextureExtender {
 public:
  LaplaceTextureExtender(std::shared_ptr<Texture> texture);
  void extend(const std::vector<TexelIntersect>& intersect) override;
};

}

#endif
