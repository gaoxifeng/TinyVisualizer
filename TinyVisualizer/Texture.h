#ifndef TEXTURE_H
#define TEXTURE_H

#include "DrawerUtility.h"
#include <memory>

extern "C" {
  struct aiTexture;
}

namespace DRAWER {
class Texture {
 public:
  struct TextureCPUData {
    TextureCPUData();
    ~TextureCPUData();
    unsigned char* _data;
    unsigned int _width;
    unsigned int _height;
  };
  Texture(int width,int height,GLenum format=GL_RGB,bool CPUOnly=false);
  Texture(const Texture& other);
  Texture& operator=(const Texture& other);
  virtual ~Texture();
  void begin() const;
  void end() const;
  int width() const;
  int height() const;
  GLuint id() const;
  TextureCPUData& loadCPUData();
  void syncGPUData();
  void save(const std::string& path,int quality=100) const;
  void save(aiTexture& tex,int quality=100) const;
  static std::shared_ptr<Texture> load(const std::string& path);
  static std::shared_ptr<Texture> load(const aiTexture& tex);
  //data channel access
  void setDataChannel(int cid,const Eigen::Matrix<GLfloat,-1,-1>& data);
  template <typename T>
  void setDataChannel(int cid,const Eigen::Matrix<T,-1,-1>& data);
  Eigen::Matrix<GLfloat,-1,-1> getDataChannel(int cid) const;
  template <typename T>
  Eigen::Matrix<T,-1,-1> getDataChannel(int cid) const;
  //data access
  void setData(int w,int h,const Eigen::Matrix<GLfloat,4,1>& data);
  template <typename T>
  void setData(int w,int h,const Eigen::Matrix<T,4,1>& data);
  Eigen::Matrix<GLfloat,4,1> getData(int w,int h) const;
  template <typename T>
  Eigen::Matrix<T,4,1> getData(int w,int h) const;
  //interpolated data access
  template <typename T>
  Eigen::Matrix<T,4,1> getData(const Eigen::Matrix<T,2,1>& tc) const;
  Eigen::Matrix<GLfloat,4,1> getData(const Eigen::Matrix<GLfloat,2,1>& tc) const;
  Eigen::Matrix<GLdouble,4,1> getData(const Eigen::Matrix<GLdouble,2,1>& tc) const;
 protected:
  void reset(int width,int height);
  void clear();
  TextureCPUData _data;
  GLenum _format;
  GLuint _id;
};
}

#endif
