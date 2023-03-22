#ifndef SHADER_H
#define SHADER_H

#include "DrawerUtility.h"
#include <memory>

namespace DRAWER {
class Shader {
 public:
  static void clearShader();
  static std::shared_ptr<Shader> findShader(const std::string& name);
  static void registerShader(const std::string& name,const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  virtual ~Shader();
  bool hasVert() const;
  bool hasGeom() const;
  bool hasFrag() const;
  bool hasCompute() const;
  GLint vertId() const;
  GLint geomId() const;
  GLint fragId() const;
  GLint computeId() const;
 private:
  Shader(const std::string& name,const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  Shader(const Shader& other);
  Shader& operator=(const Shader& other);
  GLint compileShader(std::string src,GLenum type);
  void reset();
  void clear();
  //data
  static std::map<std::string,std::shared_ptr<Shader>> _shaders;
  std::string _name,_vert,_geom,_frag,_compute;
  GLint _vertS,_geomS,_fragS,_computeS;
};
class Program {
 public:
  static void clearProgram();
  static std::shared_ptr<Program> currentProgram();
  static std::shared_ptr<Program> findProgram(const std::string& name);
  static void registerProgram(const std::string& name,const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  static void registerProgram(const std::string& name,std::function<void(GLuint)> callback,const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  virtual ~Program();
  void begin();
  void end();
  std::string getName() const;
  void setTexUnit(const std::string& name,int i,bool mustHave=true);
  void setUniformInt(const std::string& name,GLint i,bool mustHave=true);
  void setUniformUint(const std::string& name,GLuint i,bool mustHave=true);
  void setUniformBool(const std::string& name,bool b,bool mustHave=true);
  void setUniformFloat(const std::string& name,GLfloat b,bool mustHave=true);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,1>& f,bool mustHave=true);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,1>& f,bool mustHave=true);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,3>& f,bool mustHave=true);
  void setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,4>& f,bool mustHave=true);
 protected:
  Program(const std::string& name,std::function<void(GLuint)> callback,const std::string& vert="",const std::string& geom="",const std::string& frag="",const std::string& compute="");
  Program(const Program& other);
  Program& operator=(const Program& other);
  void reset(std::function<void(GLuint)> callback);
  void clear();
  //data
  static std::shared_ptr<Program> _currentProgram;
  static std::map<std::string,std::shared_ptr<Program>> _programs;
  std::shared_ptr<Shader> _vertS,_geomS,_fragS,_computeS;
  std::string _name;
  GLint _prog;
  bool _begin;
};
}

#endif
