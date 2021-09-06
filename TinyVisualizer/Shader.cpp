#include "Shader.h"
#include <iostream>
#include <vector>

namespace DRAWER {
//shader
std::shared_ptr<Shader> Shader::findShader(const std::string& name) {
  ASSERT_MSGV(_shaders.find(name)!=_shaders.end(),"Shader(%s) already exists!",name.c_str())
  return _shaders[name];
}
void Shader::registerShader(const std::string& name,const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute) {
  ASSERT_MSGV(_shaders.find(name)==_shaders.end(),"Shader(%s) already exists!",name.c_str())
  _shaders[name]=std::shared_ptr<Shader>(new Shader(name,vert,geom,frag,compute));
}
Shader::~Shader() {
  clear();
}
bool Shader::hasVert() const {
  return !_vert.empty();
}
bool Shader::hasGeom() const {
  return !_geom.empty();
}
bool Shader::hasFrag() const {
  return !_frag.empty();
}
bool Shader::hasCompute() const {
  return !_compute.empty();
}
GLint Shader::vertId() const {
  return _vertS;
}
GLint Shader::geomId() const {
  return _geomS;
}
GLint Shader::fragId() const {
  return _fragS;
}
GLint Shader::computeId() const {
  return _computeS;
}
Shader::Shader(const std::string& name,const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute)
  :_name(name),_vert(vert),_geom(geom),_frag(frag),_compute(compute) {
  reset();
}
Shader::Shader(const Shader&) {
  ASSERT(false)
}
Shader& Shader::operator=(const Shader&) {
  ASSERT(false)
  return *this;
}
GLint Shader::compileShader(std::string src,GLenum type) {
  ASSERT_MSG(glad_glCreateShader,"GLSL not supported!")
  GLint s=glCreateShader(type);
  const char* v=src.c_str();
  glShaderSource(s,1,&v,0);
  glCompileShader(s);
  GLint compiled;
  glGetShaderiv(s,GL_COMPILE_STATUS,&compiled);
  if(!compiled) {
    GLint len=0;
    glGetShaderiv(s,GL_INFO_LOG_LENGTH,&len);
    std::vector<char> info(len+1);
    glGetShaderInfoLog(s,len,&len,&info[0]);
    std::cout << "Compiling shader: " << src << std::endl;
    ASSERT_MSGV(false,"Shader compile error: %s!",&info[0])
  }
  return s;
}
void Shader::reset() {
  if(!_vert.empty())
    _vertS=compileShader(_vert,GL_VERTEX_SHADER);
  if(!_geom.empty())
    _geomS=compileShader(_geom,GL_GEOMETRY_SHADER);
  if(!_frag.empty())
    _fragS=compileShader(_frag,GL_FRAGMENT_SHADER);
  if(!_compute.empty())
    _computeS=compileShader(_compute,GL_COMPUTE_SHADER);
}
void Shader::clear() {
  if(!_vert.empty())
    glDeleteShader(_vertS);
  if(!_geom.empty())
    glDeleteShader(_geomS);
  if(!_frag.empty())
    glDeleteShader(_fragS);
  if(!_compute.empty())
    glDeleteShader(_computeS);
}
std::map<std::string,std::shared_ptr<Shader>> Shader::_shaders;
//program
std::shared_ptr<Program> Program::currentProgram() {
  return _currentProgram;
}
std::shared_ptr<Program> Program::findProgram(const std::string& name) {
  ASSERT_MSGV(_programs.find(name)!=_programs.end(),"Program(%s) already exists!",name.c_str())
  return _programs[name];
}
void Program::registerProgram(const std::string& name,const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute) {
  ASSERT_MSGV(_programs.find(name)==_programs.end(),"Progrm(%s) already exists!",name.c_str())
  _programs[name]=std::shared_ptr<Program>(new Program(name,[&](GLuint) {},vert,geom,frag,compute));
}
void Program::registerProgram(const std::string& name,std::function<void(GLuint)> callback,const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute) {
  ASSERT_MSGV(_programs.find(name)==_programs.end(),"Progrm(%s) already exists!",name.c_str())
  _programs[name]=std::shared_ptr<Program>(new Program(name,callback,vert,geom,frag,compute));
}
Program::~Program() {
  clear();
}
void Program::begin() {
  ASSERT_MSG(!_currentProgram,"Other program has not exited!")
  ASSERT_MSG(!_begin,"Shader already bound!")
  glUseProgram(_prog);
  _begin=true;
  _currentProgram=_programs[_name];
}
void Program::end() {
  ASSERT_MSG(_currentProgram,"No program in use!")
  ASSERT_MSG(_begin,"Shader haven't been bound!")
  glUseProgram(0);
  _begin=false;
  _currentProgram=NULL;
}
std::string Program::getName() const {
  return _name;
}
void Program::setTexUnit(const std::string& name,int i,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for texture: %s!",name.c_str())
  glUniform1i(loc,i);
}
void Program::setUniformInt(const std::string& name,int i,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for integer: %s!",name.c_str())
  glUniform1i(loc,i);
}
void Program::setUniformUint(const std::string& name,GLuint i,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for integer: %s!",name.c_str())
  glUniform1ui(loc,i);
}
void Program::setUniformBool(const std::string& name,bool b,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for bool: %s!",name.c_str())
  glUniform1i(loc,b);
}
void Program::setUniformFloat(const std::string& name,GLfloat f,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniform1f(loc,f);
}
void Program::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,1>& f,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniform3f(loc,f[0],f[1],f[2]);
}
void Program::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,1>& f,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniform4f(loc,f[0],f[1],f[2],f[3]);
}
void Program::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,3,3>& f,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniformMatrix3fv(loc,1,false,f.data());
}
void Program::setUniformFloat(const std::string& name,const Eigen::Matrix<GLfloat,4,4>& f,bool mustHave) {
  ASSERT_MSGV(_begin,"%s must be called after binding!",__FUNCTION__)
  int loc=glGetUniformLocation(_prog,name.c_str());
  if(!mustHave && loc<0)
    return;
  ASSERT_MSGV(loc>=0,"Cannot find location for float: %s!",name.c_str())
  glUniformMatrix4fv(loc,1,false,f.data());
}
Program::Program(const std::string& name,std::function<void(GLuint)> callback,const std::string& vert,const std::string& geom,const std::string& frag,const std::string& compute) {
  if(!vert.empty())
    _vertS=Shader::findShader(vert);
  if(!geom.empty())
    _geomS=Shader::findShader(geom);
  if(!frag.empty())
    _fragS=Shader::findShader(frag);
  if(!compute.empty())
    _computeS=Shader::findShader(compute);
  _name=name;
  _begin=false;
  reset(callback);
}
Program::Program(const Program&) {
  ASSERT(false)
}
Program& Program::operator=(const Program&) {
  ASSERT(false)
  return *this;
}
void Program::reset(std::function<void(GLuint)> callback) {
  _prog=glCreateProgram();
  if(_vertS && _vertS->hasVert())
    glAttachShader(_prog,_vertS->vertId());
  if(_geomS && _geomS->hasGeom())
    glAttachShader(_prog,_geomS->geomId());
  if(_fragS && _fragS->hasFrag())
    glAttachShader(_prog,_fragS->fragId());
  if(_computeS && _computeS->hasCompute())
    glAttachShader(_prog,_computeS->computeId());
  callback(_prog);
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
void Program::clear() {
  glDeleteProgram(_prog);
}
std::shared_ptr<Program> Program::_currentProgram;
std::map<std::string,std::shared_ptr<Program>> Program::_programs;
}
