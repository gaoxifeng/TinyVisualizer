#ifndef TEXTURE_H
#define TEXTURE_H

#include "DrawerUtility.h"
#include <assimp/texture.h>
#include <memory>

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
  Texture(int width,int height,GLenum format=GL_RGB);
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
  void save(const std::string& path) const;
  static std::shared_ptr<Texture> load(const std::string& path);
  static std::shared_ptr<Texture> load(const aiTexture& tex);
  Eigen::Matrix<GLubyte,4,1> getDataRGBA(int w,int h) const;
  template <typename T>
  Eigen::Matrix<T,4,1> getData(int w,int h) const;
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
