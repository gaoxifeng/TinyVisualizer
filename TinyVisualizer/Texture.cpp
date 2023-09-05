#include "Texture.h"
#ifdef ASSIMP_SUPPORT
#include <assimp/texture.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace DRAWER {
void writeFunc(void* context,void* data,int size) {
#ifdef ASSIMP_SUPPORT
  aiTexture* tex=(aiTexture*)context;
  delete [] tex->pcData;
  tex->pcData=(aiTexel*)new GLchar[size];
  memcpy(tex->pcData,data,size);
  tex->mWidth=size;
  tex->mHeight=0;
#else
  ASSERT_MSG(false,"Assimp not supported!")
#endif
}
void flipY(int w,int h,int c,unsigned char* data) {
  unsigned sz=w*c;
  std::vector<unsigned char> buffer(sz);
  for(int i=0; i<h/2; i++) {
    memcpy(buffer.data(),data+i*sz,sz);
    memcpy(data+i*sz,data+(h-1-i)*sz,sz);
    memcpy(data+(h-1-i)*sz,buffer.data(),sz);
  }
}
Texture::TextureCPUData::TextureCPUData():_data(NULL) {}
Texture::TextureCPUData::~TextureCPUData() {
  if(_data)
    delete [] _data;
}
Texture::Texture(int width,int height,GLenum format,bool CPUOnly):_format(format) {
  if(CPUOnly) {
    _data=TextureCPUData();
    _data._width=width;
    _data._height=height;
    _data._data=new unsigned char[_data._width*_data._height*4];
    memset(_data._data,0,_data._width*_data._height*4);
  } else reset(width,height);
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
  if(_data._data)
    return _data._width;
  else {
    GLint ret;
    begin();
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&ret);
    end();
    return ret;
  }
}
int Texture::height() const {
  if(_data._data)
    return _data._height;
  else {
    GLint ret;
    begin();
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&ret);
    end();
    return ret;
  }
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
void Texture::save(const std::string& path,int quality) const {
  const_cast<Texture*>(this)->loadCPUData();
  flipY(_data._width,_data._height,4,_data._data);
  if(path.size()>4 && (path.substr(path.size()-4)==".png" || path.substr(path.size()-4)==".PNG"))
    stbi_write_png(path.c_str(),_data._width,_data._height,4,_data._data,_data._width*4);
  else if(path.size()>4 && (path.substr(path.size()-4)==".bmp" || path.substr(path.size()-4)==".BMP"))
    stbi_write_bmp(path.c_str(),_data._width,_data._height,4,_data._data);
  else if(path.size()>4 && (path.substr(path.size()-4)==".tga" || path.substr(path.size()-4)==".TGA"))
    stbi_write_tga(path.c_str(),_data._width,_data._height,4,_data._data);
  else if(path.size()>4 && (path.substr(path.size()-4)==".jpg" || path.substr(path.size()-4)==".JPG"))
    stbi_write_jpg(path.c_str(),_data._width,_data._height,4,_data._data,quality);
  else {
    ASSERT_MSGV(false,"Unsupported texture save path: %s!",path.c_str())
  }
  //flip back
  flipY(_data._width,_data._height,4,_data._data);
}
void Texture::save(aiTexture& tex,int quality) const {
#ifdef ASSIMP_SUPPORT
  void* context=&tex;
  const_cast<Texture*>(this)->loadCPUData();
  flipY(width(),height(),4,(unsigned char*)_data._data);
  if(strcmp(tex.achFormatHint,"png")==0)
    stbi_write_png_to_func(writeFunc,context,_data._width,_data._height,4,_data._data,_data._width*4);
  else if(strcmp(tex.achFormatHint,"bmp")==0)
    stbi_write_bmp_to_func(writeFunc,context,_data._width,_data._height,4,_data._data);
  else if(strcmp(tex.achFormatHint,"tga")==0)
    stbi_write_tga_to_func(writeFunc,context,_data._width,_data._height,4,_data._data);
  else if(strcmp(tex.achFormatHint,"jpg")==0)
    stbi_write_jpg_to_func(writeFunc,context,_data._width,_data._height,4,_data._data,quality);
  else {
    ASSERT_MSGV(false,"Unsupported texture save format: %s!",tex.achFormatHint)
  }
  //flip back
  flipY(width(),height(),4,(unsigned char*)_data._data);
#else
  ASSERT_MSG(false,"Assimp not supported!")
#endif
}
std::shared_ptr<Texture> Texture::load(const std::string& path) {
  int w,h,c;
  stbi_uc* data=stbi_load(path.c_str(),&w,&h,&c,0);
  flipY(w,h,c,data);
  ASSERT(c==3 || c==4);
  std::shared_ptr<Texture> ret;
  try {
    ret.reset(new Texture(w,h,c==4?GL_RGBA:GL_RGB));
    ret->begin();
    glTexImage2D(GL_TEXTURE_2D,0,ret->_format,w,h,0,c==4?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,data);
    ret->end();
    free(data);
  } catch(...) {
    ret.reset(new Texture(w,h,c==4?GL_RGBA:GL_RGB,true));
    for(int idh=0; idh<h; idh++)
      for(int idw=0; idw<w; idw++)
        for(int d=0; d<c; d++)
          ret->_data._data[(idw+idh*w)*4+d]=data[(idw+idh*w)*c+d];
  }
  return ret;
}
std::shared_ptr<Texture> Texture::load(const aiTexture& tex) {
  std::shared_ptr<Texture> ret;
#ifdef ASSIMP_SUPPORT
  int width,height,BPP;
  void* data=stbi_load_from_memory((const stbi_uc*)tex.pcData,tex.mWidth,&width,&height,&BPP,0);
  flipY(width,height,BPP,(unsigned char*)data);
  try {
    ret.reset(new Texture(width,height,GL_RGB));
    ret->begin();
    switch(BPP) {
    case 1:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0,GL_RED,GL_UNSIGNED_BYTE,data);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
      break;
    default:
      ASSERT_MSG(false,"Image format not supported!")
    }
    ret->end();
    stbi_image_free(data);
  } catch(...) {
    ret.reset(new Texture(width,height,GL_RGB,true));
    for(int idh=0; idh<height; idh++)
      for(int idw=0; idw<width; idw++)
        for(int d=0; d<BPP; d++)
          ret->_data._data[(idw+idh*width)*4+d]=((stbi_uc*)data)[(idw+idh*width)*BPP+d];
  }
#else
  ASSERT_MSG(false,"Assimp not supported!")
#endif
  return ret;
}
//data channel access
void Texture::setDataChannel(int cid,const Eigen::Matrix<GLfloat,-1,-1>& data) {
  setDataChannel<GLfloat>(cid,data);
}
template <typename T>
void Texture::setDataChannel(int cid,const Eigen::Matrix<T,-1,-1>& data) {
  Eigen::Map<Eigen::Matrix<unsigned char,-1,-1>,0,Eigen::InnerStride<4>>(_data._data+cid,_data._width,_data._height)=(data*255).template cast<unsigned char>();
}
Eigen::Matrix<GLfloat,-1,-1> Texture::getDataChannel(int cid) const {
  return getDataChannel<GLfloat>(cid);
}
template <typename T>
Eigen::Matrix<T,-1,-1> Texture::getDataChannel(int cid) const {
  return Eigen::Map<const Eigen::Matrix<unsigned char,-1,-1>,0,Eigen::InnerStride<4>>(_data._data+cid,_data._width,_data._height).cast<T>()/255;
}
//data access
void Texture::setData(int w,int h,const Eigen::Matrix<GLfloat,4,1>& data) {
  setData<GLfloat>(w,h,data);
}
template <typename T>
void Texture::setData(int w,int h,const Eigen::Matrix<T,4,1>& data) {
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
  Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(_data._data+offset)=(data*255).template cast<unsigned char>();
}
Eigen::Matrix<GLfloat,4,1> Texture::getData(int w,int h) const {
  return getData<GLfloat>(w,h);
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
//interpolated data access
template <typename T>
Eigen::Matrix<T,4,1> Texture::getData(const Eigen::Matrix<T,2,1>& tc) const {
  T w=tc[0]*_data._width-0.5f;
  int wf=(int)std::floor(w);
  T walpha=w-wf;

  T h=tc[1]*_data._height-0.5f;
  int hf=(int)std::floor(h);
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
//helper
void Texture::reset(int width,int height) {
  _id=(GLuint)-1;
  if(!glad_glGenTextures)
    throw std::runtime_error("Texture not supported!");
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
  if(_id!=(GLuint)-1)
    glDeleteTextures(1,&_id);
  _id=-1;
}
}
