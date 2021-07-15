#ifndef SHADER_H
#define SHADER_H

#include "DrawerUtility.h"

namespace DRAWER {
class Shader {
 public:
  Shader(const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  Shader(const Shader& other);
  Shader& operator=(const Shader& other);
  virtual ~Shader();
  void begin();
  void end();
  void setTexUnit(const std::string& name,int i);
  void setUniformInt(const std::string& name,int i);
  void setUniformBool(const std::string& name,bool b);
  void setUniformFloat(const std::string& name,GLfloat b);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,1>& f);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,4>& f);
 protected:
  GLint compileShader(const std::string& src,GLenum type);
  void reset();
  void clear();
  std::string _vert,_geom,_frag,_compute;
  GLint _vertS,_geomS,_fragS,_computeS,_prog;
  bool _begin;
};
}

#endif
