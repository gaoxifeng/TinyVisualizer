#include <glad/gl.h>
#include "LowDimensionalMeshShape.h"
#include <iostream>

namespace DRAWER {
LowDimensionalMeshShape::LowDimensionalMeshShape(std::shared_ptr<MeshShape> inner):_inner(inner),_dirtyCPU(false),_dirtyBB(false) {}
void LowDimensionalMeshShape::setPointSize(GLfloat pointSize) {
  _inner->setPointSize(pointSize);
}
void LowDimensionalMeshShape::setLineWidth(GLfloat lineWidth) {
  _inner->setLineWidth(lineWidth);
}
void LowDimensionalMeshShape::setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B) {
  _inner->setColorDiffuse(mode,R,G,B);
}
void LowDimensionalMeshShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) {
  _inner->setColorAmbient(mode,RA,GA,BA);
}
void LowDimensionalMeshShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) {
  _inner->setColorSpecular(mode,RS,GS,BS);
}
void LowDimensionalMeshShape::setTextureDiffuse(std::shared_ptr<Texture> tex) {
  _inner->setTextureDiffuse(tex);
}
void LowDimensionalMeshShape::setTextureSpecular(std::shared_ptr<Texture> tex) {
  _inner->setTextureSpecular(tex);
}
void LowDimensionalMeshShape::setDepth(GLfloat depth) {
  _inner->setDepth(depth);
}
void LowDimensionalMeshShape::setDrawer(Drawer* drawer) {
  _inner->setDrawer(drawer);
}
void LowDimensionalMeshShape::setShininess(GLenum mode,GLfloat S) {
  _inner->setShininess(mode,S);
}
bool LowDimensionalMeshShape::needRecomputeNormal() const {
  return true;
}
void LowDimensionalMeshShape::draw(PASS_TYPE passType) const {
  _inner->draw(passType);
}
Eigen::Matrix<GLfloat,6,1> LowDimensionalMeshShape::getBB() const {
  if(_dirtyBB) {
    Eigen::Matrix<GLfloat,6,1>& BB=_inner->_bb;
    BB=_BBBase;
    for(int i=0; i<_L.size(); i++) {
      GLfloat l=(_DHDLMax.col(i)*_L[i]).norm();
      BB.segment<3>(0).array()-=l;
      BB.segment<3>(3).array()+=l;
    }
  }
  return _inner->getBB();
}
bool LowDimensionalMeshShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
  if(_dirtyCPU) {
    Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(_inner->_vertices.data(),_inner->_vertices.size())=_inner->_VBO->VBOVData();
    const_cast<LowDimensionalMeshShape*>(this)->_dirtyCPU=false;
  }
  return _inner->rayIntersect(ray,alpha);
}
void LowDimensionalMeshShape::setLowToHighDimensionalMapping(const Eigen::Matrix<GLfloat,-1,-1>& DHDL) {
  _inner->initVBO();
  ASSERT_MSG(DHDL.rows()==(int)_inner->_vertices.size(),"Vertex size mismatch!")
  //HMap
  _HMap.reset(new Texture((int)DHDL.rows(),(int)DHDL.cols(),GL_R32F));
  _HMap->begin();
  glTexSubImage2D(GL_TEXTURE_2D,0,0,0,(int)DHDL.rows(),(int)DHDL.cols(),GL_RED,GL_FLOAT,DHDL.data());
  _HMap->end();
  //LCoord
  _LCoord.reset(new Texture((int)DHDL.cols(),1,GL_R32F));
  //transform feedback
  std::vector<GLuint> id;
  for(int i=0; i<DHDL.rows()/3; i++)
    id.push_back(i);
  _transformFeedbackVBO.reset(new VBO((int)DHDL.rows()/3,0,true,false,false,true));
  _transformFeedbackVBO->setVertexPosition(_inner->_vertices);
  _transformFeedbackVBO->setVertexId(id);
  //bounding box
  _BBBase=_inner->getBB();
  _DHDLMax.setZero(3,(int)DHDL.cols());
  for(int i=0; i<(int)DHDL.rows()/3; i++)
    _DHDLMax=_DHDLMax.cwiseMax(DHDL.block(i*3,0,3,(int)DHDL.cols()).cwiseAbs());
}
void LowDimensionalMeshShape::updateHighDimensionalMapping(const Eigen::Matrix<GLfloat,-1,1>& L) {
  ASSERT_MSG(L.size()==_LCoord->width(),"Low dimensional coordinate size mismatch!")
  _L=L;
  //fillin LCoord
  _LCoord->begin();
  glTexSubImage2D(GL_TEXTURE_2D,0,0,0,(int)L.size(),1,GL_RED,GL_FLOAT,L.data());
  _LCoord->end();
  //setup program
  getTransformFeedbackProg()->begin();
  getTransformFeedbackProg()->setUniformInt("nL",(int)L.size());
  glActiveTexture(GL_TEXTURE0);
  _HMap->begin();
  getTransformFeedbackProg()->setTexUnit("DHDL",0);
  glActiveTexture(GL_TEXTURE1);
  _LCoord->begin();
  getTransformFeedbackProg()->setTexUnit("LCoord",1);
  glActiveTexture(GL_TEXTURE2);
  //perform computation
  glEnable(GL_RASTERIZER_DISCARD);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,_inner->_VBO->VBOV());
  //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,_inner->_VBO->VBON());
  glBeginTransformFeedback(GL_POINTS);
  _transformFeedbackVBO->draw(GL_POINTS);
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glFlush();
  //finalize program
  _LCoord->end();
  _HMap->end();
  glActiveTexture(GL_TEXTURE0);
  Program::currentProgram()->end();
  _dirtyCPU=true;
  _dirtyBB=true;
}
std::shared_ptr<Program> LowDimensionalMeshShape::getTransformFeedbackProg() const {
#include "Shader/MatVecMultVert.h"
  std::shared_ptr<Program> transformFeedbackProgram=Program::findProgram("LowDimensional");
  if(!transformFeedbackProgram) {
    Shader::registerShader("LowDimensional",MatVecMultVert);
    Program::registerProgram("LowDimensional",[&](GLuint program) {
      const char* varyings[]= {"VertexOut"};
      glTransformFeedbackVaryings(program,1,varyings,GL_INTERLEAVED_ATTRIBS);
    },"LowDimensional");
    transformFeedbackProgram=Program::findProgram("LowDimensional");
  }
  return transformFeedbackProgram;
}
}
