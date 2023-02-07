#ifndef FBO_H
#define FBO_H

#include "DrawerUtility.h"
#include "Texture.h"
#include <memory>
#include <vector>

namespace DRAWER {
class FBO {
 public:
  FBO(int width,int height,GLenum formatColor=GL_RGB,GLenum formatDepth=GL_DEPTH_COMPONENT24);
  FBO(const FBO& other);
  FBO& operator=(const FBO& other);
  virtual ~FBO();
  static void screenQuad(GLfloat minx=-1,GLfloat miny=-1,GLfloat maxx=1,GLfloat maxy=1,bool colorTex=false);
  static void screenQuad(std::shared_ptr<Texture> tex,GLfloat minx=-1,GLfloat miny=-1,GLfloat maxx=1,GLfloat maxy=1,bool colorTex=false);
  void drawScreenQuad(std::shared_ptr<Texture> tex,GLfloat minx=-1,GLfloat miny=-1,GLfloat maxx=1,GLfloat maxy=1,bool colorTex=false) const;
  void drawScreenQuad(std::function<void()> func) const;
  void begin() const;
  void end() const;
  GLfloat getPixel00() const;
  const Texture& getRBO() const;
  std::shared_ptr<Texture> getRBORef() const;
  std::vector<std::uint8_t> read() const;
  void saveImage(const std::string& path) const;
  int width() const;
  int height() const;
 private:
  void reset(int width,int height);
  void clear();
  std::shared_ptr<Texture> _rbo;
  GLenum _formatColor;
  GLenum _formatDepth;
  GLuint _fbo,_dbo;
  GLint _vp[4];
};
class FBOShadow {
 public:
  FBOShadow(int res,GLenum formatDepth=GL_DEPTH_COMPONENT24);
  FBOShadow(const FBOShadow& other);
  FBOShadow& operator=(const FBOShadow& other);
  virtual ~FBOShadow();
  void begin(int d) const;
  void end() const;
  void beginShadow() const;
  void endShadow() const;
  std::vector<std::uint8_t> read(int d) const;
  void saveImage(int d,const std::string& path) const;
  int width() const;
  int height() const;
 private:
  void reset(int res);
  void clear();
  GLenum _formatDepth;
  GLuint _fbo,_dbo;
  GLint _vp[4];
};
class FBOPingPong {
 public:
  FBOPingPong(int levelMin,int levelMax,GLenum formatColor=GL_RGB,GLenum formatDepth=GL_DEPTH_COMPONENT24);
  void saveImage(int i,const std::string& path) const;
  const FBO& getFBO(int i) const;
  int nrFBO() const;
  void begin();
  void end();
 private:
  std::vector<FBO> _fbos;
};
}

#endif
