#include "VBO.h"
#include <iostream>

namespace DRAWER {
#define VERTEX_ATTRIBUTE_SIZE (3+3+2)
#define OFFSET_POSITION 0
#define OFFSET_NORMAL 3
#define OFFSET_TEXCOORD 6
VBO::VBO(int nV,int nI) {
  reset(nV,nI);
}
VBO::VBO(const VBO& other) {
  operator=(other);
}
VBO& VBO::operator=(const VBO& other) {
  clear();
  reset(other._nV,other._nI);
  if(_nV>0)
    glCopyBufferSubData(_VBO,other._VBO,0,0,sizeof(GLfloat)*_nV*(3+3+2));
  if(_nI>0)
    glCopyBufferSubData(_IBO,other._IBO,0,0,sizeof(GLuint)*_nI);
  return *this;
}
VBO::~VBO() {
  clear();
}
void VBO::setVertexPosition(const std::vector<GLfloat>& p) {
  ASSERT_MSG((int)p.size()==_nV*3,"Incorrect buffer data size!")
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  for(int i=0; i<_nV; i++)
    dataVec.segment<3>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_POSITION)=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(p.data()+i*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setVertexNormal(const std::vector<GLfloat>& n) {
  ASSERT_MSG((int)n.size()==_nV*3,"Incorrect buffer data size!")
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  for(int i=0; i<_nV; i++)
    dataVec.segment<3>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_NORMAL)=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(n.data()+i*3);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setVertexTexCoord(const std::vector<GLfloat>& tc) {
  ASSERT_MSG((int)tc.size()==_nV*2,"Incorrect buffer data size!")
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  for(int i=0; i<_nV; i++)
    dataVec.segment<2>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_TEXCOORD)=Eigen::Map<const Eigen::Matrix<GLfloat,2,1>>(tc.data()+i*2);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setVertexPosition(int i,const Eigen::Matrix<GLfloat,3,1>& p) {
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  dataVec.segment<3>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_POSITION)=p;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setVertexNormal(int i,const Eigen::Matrix<GLfloat,3,1>& n) {
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  dataVec.segment<3>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_NORMAL)=n;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setVertexTexCoord(int i,const Eigen::Matrix<GLfloat,2,1>& tc) {
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  GLfloat* dat=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>> dataVec(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  dataVec.segment<2>(i*VERTEX_ATTRIBUTE_SIZE+OFFSET_TEXCOORD)=tc;
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::setIndex(const std::vector<GLuint>& iss) {
  ASSERT_MSG((int)iss.size()==_nI,"Incorrect buffer data size!")
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
  GLuint* dat=(GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);
  Eigen::Map<Eigen::Matrix<GLuint,-1,1>>(dat,_nI)=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(iss.data(),_nI);
  ASSERT_MSG(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER),"Buffer mapping failed!")
}
void VBO::draw(GLenum mode) {
  glBindVertexArray(_VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*VERTEX_ATTRIBUTE_SIZE,(void*)(sizeof(GLfloat)*OFFSET_POSITION));
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*VERTEX_ATTRIBUTE_SIZE,(void*)(sizeof(GLfloat)*OFFSET_NORMAL));
  glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*VERTEX_ATTRIBUTE_SIZE,(void*)(sizeof(GLfloat)*OFFSET_TEXCOORD));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
  glDrawElements(mode,_nI,GL_UNSIGNED_INT,0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glBindVertexArray(0);
}
Eigen::Matrix<GLfloat,-1,1> VBO::VBOData() const {
  glBindBuffer(GL_ARRAY_BUFFER,_VBO);
  const GLfloat* dat=(const GLfloat*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLfloat,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLfloat,-1,1>>(dat,_nV*VERTEX_ATTRIBUTE_SIZE);
  ASSERT_MSG(glUnmapBuffer(GL_ARRAY_BUFFER),"Buffer mapping failed!")
  return ret;
}
Eigen::Matrix<GLuint,-1,1> VBO::IBOData() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
  const GLuint* dat=(const GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_ONLY);
  Eigen::Matrix<GLuint,-1,1> ret=Eigen::Map<const Eigen::Matrix<GLuint,-1,1>>(dat,_nI);
  ASSERT_MSG(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER),"Buffer mapping failed!")
  return ret;
}
void VBO::reset(int nV,int nI) {
  glGenVertexArrays(1,&_VAO);
  glBindVertexArray(_VAO);
  _nV=nV;
  if(_nV>0) {
    glGenBuffers(1,&_VBO);
    glBindBuffer(GL_ARRAY_BUFFER,_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*nV*VERTEX_ATTRIBUTE_SIZE,NULL,GL_STATIC_DRAW); //position/normal/texcoord
  }
  _nI=nI;
  if(_nI>0) {
    glGenBuffers(1,&_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*nI,NULL,GL_STATIC_DRAW);
  }
  glBindVertexArray(0);
}
void VBO::clear() {
  glDeleteVertexArrays(1,&_VAO);
  if(_nV>0)
    glDeleteBuffers(1,&_VBO);
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
