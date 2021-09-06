#include "VBO.h"
#include <iostream>

namespace DRAWER {
VBO::VBO(int nV,int nI,bool hasV,bool hasN,bool hasT,bool hasI) {
  reset(nV,nI,hasV,hasN,hasT,hasI);
}
VBO::VBO(const VBO& other) {
  operator=(other);
}
VBO& VBO::operator=(const VBO& other) {
  clear();
  reset(other._nV,other._nI,other._hasV,other._hasN,other._hasT,other._hasI);
  if(_nV>0 && _hasV)
    glCopyBufferSubData(_VBOV,other._VBOV,0,0,sizeof(GLfloat)*_nV*3);
  if(_nV>0 && _hasN)
    glCopyBufferSubData(_VBON,other._VBON,0,0,sizeof(GLfloat)*_nV*3);
  if(_nV>0 && _hasT)
    glCopyBufferSubData(_VBOT,other._VBOT,0,0,sizeof(GLfloat)*_nV*2);
  if(_nV>0 && _hasI)
    glCopyBufferSubData(_VBOI,other._VBOI,0,0,sizeof(GLuint)*_nV);
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
void VBO::setVertexPosition(int i,const Eigen::Matrix<GLfloat,3,1>& p) {
  ASSERT_MSG(_hasV,"No position data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOV);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+i*3)=p;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexNormal(int i,const Eigen::Matrix<GLfloat,3,1>& n) {
  ASSERT_MSG(_hasN,"No normal data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBON);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(dat+i*3)=n;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::setVertexTexCoord(int i,const Eigen::Matrix<GLfloat,2,1>& tc) {
  ASSERT_MSG(_hasT,"No texture coordinate data available!")
  if(_nV==0)
    return;
  glBindBuffer(GL_ARRAY_BUFFER,_VBOT);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,2,1>>(dat+i*2)=tc;
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
void VBO::reset(int nV,int nI,bool hasV,bool hasN,bool hasT,bool hasI) {
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
  if(_nI>0)
    glDeleteBuffers(1,&_IBO);
}
//draw simple shapes
std::shared_ptr<VBO> VBOPoint;
void drawPointf(const Eigen::Matrix<GLfloat,2,1>& v0) {
  drawPointf(Eigen::Matrix<GLfloat,3,1>(v0[0],v0[1],0));
}
void drawPointf(const Eigen::Matrix<GLfloat,3,1>& v0) {
  if(!VBOPoint) {
    VBOPoint.reset(new VBO(1,1));
    VBOPoint->setIndex({0});
  }
  VBOPoint->setVertexPosition(0,v0);
  VBOPoint->draw(GL_POINTS);
}
std::shared_ptr<VBO> VBOLine;
void drawLinef(const Eigen::Matrix<GLfloat,2,1>& v0,
               const Eigen::Matrix<GLfloat,2,1>& v1) {
  drawLinef(Eigen::Matrix<GLfloat,3,1>(v0[0],v0[1],0),
            Eigen::Matrix<GLfloat,3,1>(v1[0],v1[1],0));
}
void drawLinef(const Eigen::Matrix<GLfloat,3,1>& v0,
               const Eigen::Matrix<GLfloat,3,1>& v1) {
  if(!VBOLine) {
    VBOLine.reset(new VBO(2,2));
    VBOLine->setIndex({0,1});
  }
  VBOLine->setVertexPosition(0,v0);
  VBOLine->setVertexPosition(1,v1);
  VBOLine->draw(GL_LINES);
}
std::shared_ptr<VBO> VBOQuad;
void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& v0,
               const Eigen::Matrix<GLfloat,2,1>& v1,
               const Eigen::Matrix<GLfloat,2,1>& v2,
               const Eigen::Matrix<GLfloat,2,1>& v3) {
  drawQuadf(Eigen::Matrix<GLfloat,3,1>(v0[0],v0[1],0),
            Eigen::Matrix<GLfloat,3,1>(v1[0],v1[1],0),
            Eigen::Matrix<GLfloat,3,1>(v2[0],v2[1],0),
            Eigen::Matrix<GLfloat,3,1>(v3[0],v3[1],0));
}
void drawQuadf(const Eigen::Matrix<GLfloat,3,1>& v0,
               const Eigen::Matrix<GLfloat,3,1>& v1,
               const Eigen::Matrix<GLfloat,3,1>& v2,
               const Eigen::Matrix<GLfloat,3,1>& v3) {
  if(!VBOQuad) {
    VBOQuad.reset(new VBO(4,4));
    VBOQuad->setIndex({0,1,2,3});
  }
  VBOQuad->setVertexPosition(0,v0);
  VBOQuad->setVertexPosition(1,v1);
  VBOQuad->setVertexPosition(2,v2);
  VBOQuad->setVertexPosition(3,v3);
  VBOQuad->draw(GL_TRIANGLE_FAN);
}
void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,2,1>& v0,
               const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,2,1>& v1,
               const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,2,1>& v2,
               const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,2,1>& v3) {
  drawQuadf(t0,Eigen::Matrix<GLfloat,3,1>(v0[0],v0[1],0),
            t1,Eigen::Matrix<GLfloat,3,1>(v1[0],v1[1],0),
            t2,Eigen::Matrix<GLfloat,3,1>(v2[0],v2[1],0),
            t3,Eigen::Matrix<GLfloat,3,1>(v3[0],v3[1],0));
}
void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,3,1>& v0,
               const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,3,1>& v1,
               const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,3,1>& v2,
               const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,3,1>& v3) {
  if(!VBOQuad) {
    VBOQuad.reset(new VBO(4,4));
    VBOQuad->setIndex({0,1,2,3});
  }
  VBOQuad->setVertexTexCoord(0,t0);
  VBOQuad->setVertexTexCoord(1,t1);
  VBOQuad->setVertexTexCoord(2,t2);
  VBOQuad->setVertexTexCoord(3,t3);
  VBOQuad->setVertexPosition(0,v0);
  VBOQuad->setVertexPosition(1,v1);
  VBOQuad->setVertexPosition(2,v2);
  VBOQuad->setVertexPosition(3,v3);
  VBOQuad->draw(GL_TRIANGLE_FAN);
}
}
