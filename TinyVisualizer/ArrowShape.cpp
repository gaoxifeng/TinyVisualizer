#include "ArrowShape.h"
#include "Matrix.h"

namespace DRAWER {
ArrowShape::ArrowShape(GLfloat angle,GLfloat thickness,GLfloat thicknessOuter,int RES) {
  setMode(GL_TRIANGLES);
  int off=-1;
  //bottom
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*-i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thickness,std::sin(angle)*thickness,-1));
    addIndex(Eigen::Matrix<GLuint,3,1>(0,i,(i+1)%RES));
  }
  //side
  addVertex(Eigen::Matrix<GLfloat,3,1>(0,0,-1));
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thickness,std::sin(angle)*thickness,-1));
  }
  off=(int)_vertices.size()/3;
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thickness,std::sin(angle)*thickness,1));
    addIndex(Eigen::Matrix<GLuint,3,1>(i-RES+off,((i+1)%RES)-RES+off,((i+1)%RES)+off));
    addIndex(Eigen::Matrix<GLuint,3,1>(i-RES+off,((i+1)%RES)+off,i+off));
  }
  //arrow side
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thickness,std::sin(angle)*thickness,1));
  }
  off=(int)_vertices.size()/3;
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thicknessOuter,std::sin(angle)*thicknessOuter,1));
    addIndex(Eigen::Matrix<GLuint,3,1>(i-RES+off,((i+1)%RES)-RES+off,((i+1)%RES)+off));
    addIndex(Eigen::Matrix<GLuint,3,1>(i-RES+off,((i+1)%RES)+off,i+off));
  }
  //arrow top
  for(int i=0; i<RES; i++) {
    GLfloat angle=(GLfloat)M_PI*2*i/RES;
    addVertex(Eigen::Matrix<GLfloat,3,1>(std::cos(angle)*thicknessOuter,std::sin(angle)*thicknessOuter,1));
  }
  off=(int)_vertices.size()/3;
  _tipLen=(GLfloat)std::tan(angle*M_PI/180)*thicknessOuter;
  addVertex(Eigen::Matrix<GLfloat,3,1>(0,0,1+_tipLen));
  for(int i=0; i<RES; i++)
    addIndex(Eigen::Matrix<GLuint,3,1>(i-RES+off,((i+1)%RES)-RES+off,off));
  computeNormals();
  _verticesRef=_vertices;
  _indicesRef=_indices;
  _bbRef=getBB();
  _T.setIdentity();
}
void ArrowShape::setArrow(const Eigen::Matrix<GLfloat,3,1>& from,
                          const Eigen::Matrix<GLfloat,3,1>& to) {
  GLfloat halfLen=(to-from).norm()/2;
  int off=(int)_vertices.size()/3-1;
  _vertices=_verticesRef;
  for(int i=0; i<off; i++)
    _vertices[i*3+2]*=halfLen;
  _vertices[off*3+2]=_vertices[off*3-3+2]+_tipLen;

  //trans
  int id;
  Eigen::Matrix<GLfloat,3,3> R;
  R.col(2)=(to-from).normalized();
  R.col(2).cwiseAbs().minCoeff(&id);
  R.col(1)=Eigen::Matrix<GLfloat,3,1>::Unit(id).cross(R.col(2)).normalized();
  R.col(0)=R.col(1).cross(R.col(2));
  _T.block<3,3>(0,0)=R;
  _T.block<3,1>(0,3)=(from+to)/2;

  //BB
  _bb=_bbRef;
  _bb[2]=_vertices[2];
  _bb[5]=_vertices.back();
  _bb=transformBB(_bb,_T);
}
void ArrowShape::draw(PASS_TYPE passType) const {
  matrixMode(GL_MODELVIEW_MATRIX);
  pushMatrix();
  multMatrixf(_T);
  MeshShape::draw(passType);
  matrixMode(GL_MODELVIEW_MATRIX);
  popMatrix();
}
}
