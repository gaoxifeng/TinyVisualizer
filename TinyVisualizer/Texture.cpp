#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace DRAWER {
Texture::TextureCPUData::TextureCPUData():_data(NULL) {}
Texture::TextureCPUData::~TextureCPUData() {
  if(_data)
    delete [] _data;
}
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
Texture::TextureCPUData& Texture::loadCPUData() {
  if(_data._data)
    return _data;

  _data._width=width();
  _data._height=height();

  begin();
  _data._data=new unsigned char[_data._width*_data._height*4];
  glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,_data._data);
  end();
  return _data;
}
void Texture::syncGPUData() {
  if(!_data._data)
    return;

  begin();
  glTexImage2D(GL_TEXTURE_2D,0,_format,_data._width,_data._height,0,GL_RGBA,GL_UNSIGNED_BYTE,_data._data);
  end();
}
void Texture::save(const std::string& path) const {
  const_cast<Texture*>(this)->loadCPUData();
  ASSERT(path.size()>4 && path.substr(path.size()-4)==".png")
  stbi_write_png(path.c_str(),_data._width,_data._height,4,_data._data,_data._width*4);
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
template <typename T>
Eigen::Matrix<T,4,1> Texture::getData(int w,int h) const {
  //repeat w
  w=w%_data._width;
  if(w<0)
    w+=_data._width;
  //repeat h
  h=h%_data._height;
  if(h<0)
    h+=_data._height;
  //fetch
  int offset=4*(w+_data._width*h);
  return Eigen::Map<const Eigen::Matrix<unsigned char,4,1>>(_data._data+offset).cast<T>()/255;
}
template <typename T>
Eigen::Matrix<T,4,1> Texture::getData(const Eigen::Matrix<T,2,1>& tc) const {
  T w=tc[0]*_data._width-0.5;
  int wf=std::floor(w);
  T walpha=w-wf;

  T h=tc[1]*_data._height-0.5;
  int hf=std::floor(h);
  T halpha=h-hf;

  return (getData<T>(wf,hf)*(1-walpha)+getData<T>(wf+1,hf)*walpha)*(1-halpha)+
         (getData<T>(wf,hf+1)*(1-walpha)+getData<T>(wf+1,hf+1)*walpha)*halpha;
}
Eigen::Matrix<GLfloat,4,1> Texture::getData(const Eigen::Matrix<GLfloat,2,1>& tc) const {
  return getData<GLfloat>(tc);
}
Eigen::Matrix<GLdouble,4,1> Texture::getData(const Eigen::Matrix<GLdouble,2,1>& tc) const {
  return getData<GLdouble>(tc);
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
  if(_data._data)
    delete [] _data._data;
  _data=TextureCPUData();
}
void Texture::clear() {
  glDeleteTextures(1,&_id);
}
}
