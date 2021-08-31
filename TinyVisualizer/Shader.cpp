#include "Shader.h"
#include <iostream>
#include <vector>

namespace DRAWER {
Shader& Shader::operator=(const Shader& other) {
  _vert=other._vert;
  _geom=other._geom;
  _frag=other._frag;
  _compute=other._frag;
  clear();
  reset();
  return *this;
}
Shader::~Shader() {
  clear();
}
void Shader::begin() {
  ASSERT_MSG(!_begin,"Shader already bound!")
  glUseProgram(_prog);
  _begin=true;
}
void Shader::end() {
  ASSERT_MSG(_begin,"Shader haven't been bound!")
  glUseProgram(0);
  _begin=false;
}
void Shader::setTexUnit(const std::string& name,int i) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for texture: %s!",name.c_str())
  glUniform1i(loc,i);
}
void Shader::setUniformInt(const std::string& name,int i) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for integer: %s!",name.c_str())
  glUniform1i(loc,i);
}
void Shader::setUniformBool(const std::string& name,bool b) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for bool: %s!",name.c_str())
  glUniform1i(loc,b);
}
void Shader::setUniformFloat(const std::string& name,GLfloat f) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniform1f(loc,f);
}
void Shader::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,1>& f) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniform3f(loc,f[0],f[1],f[2]);
}
void Shader::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,4>& f) {
  ASSERT_MSGV(_begin,"%s must be clled after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniformMatrix4fv(loc,1,false,f.data());
}
GLint Shader::compileShader(const std::string& src,GLenum type) {
  ASSERT_MSG(glad_glCreateShader,"GLSL not supported!")
  GLint s=glCreateShader(type);
  const char* v=src.c_str();
  const int len=src.size();
  glShaderSource(s,1,&v,&len);
  glCompileShader(s);
  GLint compiled;
  glGetShaderiv(s,GL_COMPILE_STATUS,&compiled);
  if(!compiled) {
    GLint len=0;
    glGetShaderiv(s,GL_INFO_LOG_LENGTH,&len);
    std::vector<char> info(len+1);
    glGetShaderInfoLog(s,len,&len,&info[0]);
    ASSERT_MSGV(false,"Shader compile error: %s!",&info[0])
  }
  return s;
}
void Shader::reset() {
  _vertS=compileShader(_vert,GL_VERTEX_SHADER);
  _geomS=compileShader(_geom,GL_GEOMETRY_SHADER);
  _fragS=compileShader(_frag,GL_FRAGMENT_SHADER);
  _computeS=compileShader(_compute,GL_COMPUTE_SHADER);

  //program
  _prog=glCreateProgram();
  if(!_vert.empty())
    glAttachShader(_prog,_vertS);
  if(!_geom.empty())
    glAttachShader(_prog,_geomS);
  if(!_frag.empty())
    glAttachShader(_prog,_fragS);
  if(!_compute.empty())
    glAttachShader(_prog,_computeS);
  glLinkProgram(_prog);
  GLint linked;
  glGetProgramiv(_prog,GL_LINK_STATUS,&linked);
  if(!linked) {
    GLint len=0;
    glGetProgramiv(_prog,GL_INFO_LOG_LENGTH,&len);
    std::vector<char> info(len+1);
    glGetProgramInfoLog(_prog,len,&len,&info[0]);
    ASSERT_MSGV(false,"Program link error: %s!",&info[0])
  }
}
void Shader::clear() {
  glDeleteProgram(_prog);
  if(!_vert.empty())
    glDeleteShader(_vertS);
  if(!_geom.empty())
    glDeleteShader(_geomS);
  if(!_frag.empty())
    glDeleteShader(_fragS);
  if(!_compute.empty())
    glDeleteShader(_computeS);
}
Shader::Shader(const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute)
  :_vert(vert),_geom(geom),_frag(frag),_compute(compute),_begin(false) {
  reset();
}
Shader::Shader(const Shader& other)
  :_vert(other._vert),_geom(other._geom),_frag(other._frag),_compute(other._compute),_begin(false) {
  reset();
}
}
