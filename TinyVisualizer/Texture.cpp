#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
std::shared_ptr<Texture> Texture::load(const std::string& path) {
  int w,h,c;
  stbi_uc* data=stbi_load(path.c_str(),&w,&h,&c,0);
  ASSERT(c==3 || c==4);
  std::shared_ptr<Texture> ret(new Texture(w,h,c==4?GL_RGBA:GL_RGB));
  ret->begin();
  glTexImage2D(GL_TEXTURE_2D,0,ret->_format,w,h,0,c==4?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,data);
  ret->end();
  free(data);
  return ret;
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
