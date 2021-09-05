#include "Texture.h"

namespace DRAWER {
Texture::Texture(int width,int height,GLenum format):_format(format) {
  reset(width,height);
}
Texture::Texture(const Texture& other):_format(other._format) {
  reset(other.width(),other.height());
}
Texture& Texture::operator=(const Texture& other) {
  clear();
  _format=other._format;
  reset(other.width(),other.height());
  return *this;
}
Texture::~Texture() {
  clear();
}
void Texture::begin() const {
  glBindTexture(GL_TEXTURE_2D,_id);
}
void Texture::end() const {
  glBindTexture(GL_TEXTURE_2D,0);
}
int Texture::width() const {
  GLint ret;
  begin();
  glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&ret);
  end();
  return ret;
}
int Texture::height() const {
  GLint ret;
  begin();
  glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&ret);
  end();
  return ret;
}
GLuint Texture::id() const {
  return _id;
}
void Texture::reset(int width,int height) {
  ASSERT_MSG(glad_glGenTextures,"Texture not supported!")
  glGenTextures(1,&_id);
  begin();
  glTexImage2D(GL_TEXTURE_2D,0,_format,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,0);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  end();
}
void Texture::clear() {
  glDeleteTextures(1,&_id);
}
}
