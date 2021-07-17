#ifndef TEXTURE_H
#define TEXTURE_H

#include "DrawerUtility.h"

namespace DRAWER {
class Texture {
 public:
  Texture(int width,int height,GLenum format=GL_RGB);
  Texture(const Texture& other);
  Texture& operator=(const Texture& other);
  virtual ~Texture();
  void begin() const;
  void end() const;
  int width() const;
  int height() const;
  GLuint id() const;
 protected:
  void reset(int width,int height);
  void clear();
  GLenum _format;
  GLuint _id;
};
}

#endif
