#include <glad/gl.h>
#include "VBO.h"
#include <iostream>

namespace DRAWER {
VBO::VBO(int nV,int nI,bool hasV,bool hasN,bool hasT,bool hasI,bool hasBI,bool hasBW) {
  reset(nV,nI,hasV,hasN,hasT,hasI,hasBI,hasBW);
}
VBO::VBO(const VBO& other) {
  operator=(other);
}
VBO& VBO::operator=(const VBO& other) {
  clear();
  reset(other._nV,other._nI,other._hasV,other._hasN,other._hasT,other._hasI,other._hasBI,other._hasBW);
  if(_nV>0 && _hasV)
    glCopyBufferSubData(_VBOV,other._VBOV,0,0,sizeof(GLfloat)*_nV*3);
  if(_nV>0 && _hasN)
    glCopyBufferSubData(_VBON,other._VBON,0,0,sizeof(GLfloat)*_nV*3);
  if(_nV>0 && _hasT)
    glCopyBufferSubData(_VBOT,other._VBOT,0,0,sizeof(GLfloat)*_nV*2);
  if(_nV>0 && _hasI)
    glCopyBufferSubData(_VBOI,other._VBOI,0,0,sizeof(GLuint)*_nV);
  if(_nV>0 && _hasBI)
    glCopyBufferSubData(_VBOBI,other._VBOBI,0,0,sizeof(GLuint)*_nV*4);
  if(_nV>0 && _hasBW)
    glCopyBufferSubData(_VBOBW,other._VBOBW,0,0,sizeof(GLfloat)*_nV*4);
  if(_nI>0)
    glCopyBufferSubData(_IBO,other._IBO,0,0,sizeof(GLuint)*_nI);
  return *this;
}
VBO::~VBO() {
  clear();
}
void VBO::setVertexPosition(const std::vector<GLfloat>& p) {
  ASSERT_MSG((int)p.size()==_nV*3,"Incorrect buffer data size!")
  ASSERT_MSG(_hasV,"No position data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*3)=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(p.data(),_nV*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexNormal(const std::vector<GLfloat>& n) {
  ASSERT_MSG((int)n.size()==_nV*3,"Incorrect buffer data size!")
  ASSERT_MSG(_hasN,"No normal data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBON);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*3)=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(n.data(),_nV*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexTexCoord(const std::vector<GLfloat>& tc) {
  ASSERT_MSG((int)tc.size()==_nV*2,"Incorrect buffer data size!")
  ASSERT_MSG(_hasT,"No texture coordinate data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*2)=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(tc.data(),_nV*2);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexId(const std::vector<GLuint>& id) {
  ASSERT_MSG((int)id.size()==_nV,"Incorrect buffer data size!")
  ASSERT_MSG(_hasI,"No index data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOI);
  GLuint* dat=(GLuint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLuint,-1,1>>(dat,_nV)=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(id.data(),_nV);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexBoneId(const std::vector<GLint>& bid) {
  ASSERT_MSG((int)bid.size()==_nV*4,"Incorrect buffer data size!")
  ASSERT_MSG(_hasBI,"No bone index data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBI);
  GLint* dat=(GLint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLint,-1,1>>(dat,_nV*4)=Eigen::Map<const Eigen::Matrix<GLint,-1,1>>(bid.data(),_nV*4);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexBoneWeight(const std::vector<GLfloat>& p) {
  ASSERT_MSG((int)p.size()==_nV*4,"Incorrect buffer data size!")
  ASSERT_MSG(_hasBW,"No bone weight data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBW);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*4)=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(p.data(),_nV*4);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexPosition(int i,const Eigen::Matrix<GLfloat,3,1>& p) {
  ASSERT_MSG(_hasV,"No position data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
  GLfloat* dat=(GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLfloat)*i*3,sizeof(GLfloat)*3,GL_MAP_WRITE_BIT);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+0)=p;
  //GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+i*3)=p;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexNormal(int i,const Eigen::Matrix<GLfloat,3,1>& n) {
  ASSERT_MSG(_hasN,"No normal data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBON);
  GLfloat* dat=(GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLfloat)*i*3,sizeof(GLfloat)*3,GL_MAP_WRITE_BIT);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+0)=n;
  //GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+i*3)=n;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexTexCoord(int i,const Eigen::Matrix<GLfloat,2,1>& tc) {
  ASSERT_MSG(_hasT,"No texture coordinate data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
  GLfloat* dat=(GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLfloat)*i*2,sizeof(GLfloat)*2,GL_MAP_WRITE_BIT);
  Eigen::Map<Eigen::Matrix<GLfloat,2,1>>(dat+0)=tc;
  //GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //Eigen::Map<Eigen::Matrix<GLfloat,2,1>>(dat+i*2)=tc;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexId(int i,int id) {
  ASSERT_MSG(_hasI,"No index data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOI);
  GLuint* dat=(GLuint*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLuint)*i,sizeof(GLuint),GL_MAP_WRITE_BIT);
  dat[0]=id;
  //GLuint* dat=(GLuint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //dat[i]=id;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexBoneId(int i,const Eigen::Matrix<GLint,4,1>& bid) {
  ASSERT_MSG(_hasBI,"No bone index data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBI);
  GLint* dat=(GLint*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLint)*i*4,sizeof(GLint)*4,GL_MAP_WRITE_BIT);
  Eigen::Map<Eigen::Matrix<GLint,4,1>>(dat+0)=bid;
  //GLint* dat=(GLint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //Eigen::Map<Eigen::Matrix<GLint,4,1>>(dat+i*4)=bid;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexBoneWeight(int i,const Eigen::Matrix<GLfloat,4,1>& bw) {
  ASSERT_MSG(_hasBW,"No bone weight data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBW);
  GLfloat* dat=(GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(GLfloat)*i*4,sizeof(GLfloat)*4,GL_MAP_WRITE_BIT);
  Eigen::Map<Eigen::Matrix<GLfloat,4,1>>(dat+0)=bw;
  //GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  //Eigen::Map<Eigen::Matrix<GLfloat,4,1>>(dat+i*4)=bw;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setIndex(const std::vector<GLuint>& iss) {
  if(_nI==0)
    return;
  ASSERT_MSG((int)iss.size()==_nI,"Incorrect buffer data size!")
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
  GLuint* dat=(GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLuint,-1,1>>(dat,_nI)=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(iss.data(),_nI);
  ASSERT_MSG(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::draw(GLenum mode) {
  glBindVertexArray(_VAO);
  if(_hasV) {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
  }
  if(_hasN) {
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,_VBON);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
  }
  if(_hasT) {
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,NULL);
  }
  if(_hasI) {
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOI);
    glVertexAttribIPointer(3,1,GL_UNSIGNED_INT,0,NULL);
  }
  if(_hasBI) {
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOBI);
    glVertexAttribIPointer(4,4,GL_INT,0,NULL);
  }
  if(_hasBW) {
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOBW);
    glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,0,NULL);
  }
  if(_nI>0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
    glDrawElements(mode,_nI,GL_UNSIGNED_INT,0);
  } else {
    glDrawArrays(mode,0,_nV);
  }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(5);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
}
Eigen::Matrix<GLfloat,-1,1> VBO::VBOVData() const {
  ASSERT_MSG(_hasV,"No texture coordinate data available!")
  if(_nV==0)
    return Eigen::Matrix<GLfloat,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
  const GLfloat* dat=(const GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLfloat,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLfloat,-1,1> VBO::VBONData() const {
  ASSERT_MSG(_hasN,"No texture coordinate data available!")
  if(_nV==0)
    return Eigen::Matrix<GLfloat,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBON);
  const GLfloat* dat=(const GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLfloat,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLfloat,-1,1> VBO::VBOTData() const {
  ASSERT_MSG(_hasT,"No texture coordinate data available!")
  if(_nV==0)
    return Eigen::Matrix<GLfloat,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
  const GLfloat* dat=(const GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLfloat,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*2);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLuint,-1,1> VBO::VBOIData() const {
  ASSERT_MSG(_hasI,"No index data available!")
  if(_nV==0)
    return Eigen::Matrix<GLuint,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBOI);
  const GLuint* dat=(const GLuint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLuint,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(dat,_nV);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLint,-1,1> VBO::VBOBIData() const {
  ASSERT_MSG(_hasBI,"No bone index data available!")
  if(_nV==0)
    return Eigen::Matrix<GLint,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBI);
  const GLint* dat=(const GLint*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLint,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLint,-1,1>>(dat,_nV*4);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLfloat,-1,1> VBO::VBOBWData() const {
  ASSERT_MSG(_hasBW,"No bone weight data available!")
  if(_nV==0)
    return Eigen::Matrix<GLfloat,-1,1>();
  glBindBuffer(GL_ARRAY_BUFFER,_VBOBW);
  const GLfloat* dat=(const GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLfloat,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*4);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
  return ret;
}
Eigen::Matrix<GLuint,-1,1> VBO::IBOData() const {
  if(_nI==0)
    return Eigen::Matrix<GLuint,-1,1>();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
  const GLuint* dat=(const GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLuint,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(dat,_nI);
  ASSERT_MSG(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  return ret;
}
GLuint VBO::VBOV() const {
  return _VBOV;
}
GLuint VBO::VBON() const {
  return _VBON;
}
GLuint VBO::VBOT() const {
  return _VBOT;
}
GLuint VBO::VBOI() const {
  return _VBOI;
}
GLuint VBO::VBOBI() const {
  return _VBOBI;
}
GLuint VBO::VBOBW() const {
  return _VBOBW;
}
void VBO::reset(int nV,int nI,bool hasV,bool hasN,bool hasT,bool hasI,bool hasBI,bool hasBW) {
  _VAO=(GLuint)-1;
  if(!glad_glGenVertexArrays) {
    ASSERT_MSG(glad_glGenVertexArrays,"OpenGL not initialized!")
    //throw std::runtime_error("VertexArray not supported!");
  }
  glGenVertexArrays(1,&_VAO);
  glBindVertexArray(_VAO);
  _nV=nV;
  _hasV=hasV;
  if(_nV>0 && _hasV) {
    glGenBuffers(1,&_VBOV);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*nV*3,NULL,GL_STATIC_DRAW); //position
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _hasN=hasN;
  if(_nV>0 && _hasN) {
    glGenBuffers(1,&_VBON);
    glBindBuffer(GL_ARRAY_BUFFER,_VBON);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*nV*3,NULL,GL_STATIC_DRAW); //normal
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _hasT=hasT;
  if(_nV>0 && _hasT) {
    glGenBuffers(1,&_VBOT);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*nV*2,NULL,GL_STATIC_DRAW); //texcoord
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _hasI=hasI;
  if(_nV>0 && _hasI) {
    glGenBuffers(1,&_VBOI);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOI);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLuint)*nV,NULL,GL_STATIC_DRAW); //index
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _hasBI=hasBI;
  if(_nV>0 && _hasBI) {
    glGenBuffers(1,&_VBOBI);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOBI);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLint)*nV*4,NULL,GL_STATIC_DRAW); //bone index
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _hasBW=hasBW;
  if(_nV>0 && _hasBW) {
    glGenBuffers(1,&_VBOBW);
    glBindBuffer(GL_ARRAY_BUFFER,_VBOBW);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*nV*4,NULL,GL_STATIC_DRAW); //bone weight
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
  _nI=nI;
  if(_nI>0) {
    glGenBuffers(1,&_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*nI,NULL,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  }
  glBindVertexArray(0);
}
void VBO::clear() {
  glDeleteVertexArrays(1,&_VAO);
  if(_nV>0 && _hasV)
    glDeleteBuffers(1,&_VBOV);
  if(_nV>0 && _hasN)
    glDeleteBuffers(1,&_VBON);
  if(_nV>0 && _hasT)
    glDeleteBuffers(1,&_VBOT);
  if(_nV>0 && _hasI)
    glDeleteBuffers(1,&_VBOI);
  if(_nV>0 && _hasBI)
    glDeleteBuffers(1,&_VBOBI);
  if(_nV>0 && _hasBW)
    glDeleteBuffers(1,&_VBOBW);
  if(_nI>0)
    glDeleteBuffers(1,&_IBO);
}
}
