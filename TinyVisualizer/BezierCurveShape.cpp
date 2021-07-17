#include "BezierCurveShape.h"
#include "Shader.h"
#include <iostream>

namespace DRAWER {
Eigen::Matrix<GLfloat,4,1> CPStencil(GLfloat s) {
  return Eigen::Matrix<GLfloat,4,1>(std::pow(1-s,3),
                                    3*std::pow(1-s,2)*s,
                                    3*(1-s)*std::pow(s,2),
                                    std::pow(s,3));
}
Eigen::Matrix<GLfloat,4,1> PTStencil(GLfloat s) {
  return Eigen::Matrix<GLfloat,4,1>(2*std::pow(s,3)-3*std::pow(s,2)+1,
                                    std::pow(s,3)-2*std::pow(s,2)+s,
                                    -2*std::pow(s,3)+3*std::pow(s,2),
                                    std::pow(s,3)-std::pow(s,2));
}
BezierCurveShape::BezierCurveShape(GLfloat thickness,bool isHermite,int RES)
  :_isHermite(isHermite),_thickness(thickness),_RES(RES) {
  setMode(RES==0?GL_LINE_STRIP:GL_QUADS);
  if(!_buildMat) {
    Eigen::Matrix<GLfloat,4,4> sCP,sPT;
    for(int d=0; d<4; d++) {
      sCP.col(d)=CPStencil(d/3.0f);
      sPT.col(d)=PTStencil(d/3.0f);
    }
    _toCP=sPT*sCP.inverse();
  }
  //default to not cast shadow
  setCastShadow(false);
}
int BezierCurveShape::nrControlPoint() const {
  return (int)_controlPoints.size()/3;
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> BezierCurveShape::getControlPoint(int i) {
  _dirty=true;
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(&_controlPoints[i*3]);
}
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> BezierCurveShape::getControlPoint(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_controlPoints[i*3]);
}
void BezierCurveShape::clearControlPoint() {
  _controlPoints.clear();
  _dirty=true;
}
void BezierCurveShape::computeNormals() {
  _normals.assign(_vertices.size(),0);
  for(int i=0; i+3<(int)_vertices.size(); i+=3) {
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v0(&_vertices[i]);
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v1(&_vertices[i+3]);
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n0(&_normals[i]);
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n1(&_normals[i+3]);
    Eigen::Matrix<GLfloat,3,1> d=v1-v0;
    if(i==0) {
      int id;
      d.cwiseAbs().minCoeff(&id);
      n0=Eigen::Matrix<GLfloat,3,1>::Unit(id).cross(d);
    } else {
      Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> nLast(&_normals[i-3]);
      n0=d.cross(nLast).cross(d);
    }
    GLfloat len=n0.norm();
    if(len>0)
      n0/=len;
    else n0.setRandom();
    if(i==(int)_indices.size()-2)
      n1=n0;
  }
}
void BezierCurveShape::draw(bool shadowPass) const {
  if(_dirty) {
    const_cast<BezierCurveShape*>(this)->subdivide();
    const_cast<BezierCurveShape*>(this)->refitBB();
    const_cast<BezierCurveShape*>(this)->_dirty=false;
  }
  MeshShape::draw(shadowPass);
}
void BezierCurveShape::subdivide() {
  _vertices.clear();
  if(_isHermite) {
    for(int i=0; i+3<nrControlPoint(); i+=2) {
      Eigen::Map<const Eigen::Matrix<GLfloat,3,4>> PC(&_controlPoints[(i+0)*3]);
      subdivide(PC*_toCP,0,1);
    }
  } else {
    for(int i=0; i+3<nrControlPoint(); i+=3) {
      Eigen::Map<const Eigen::Matrix<GLfloat,3,4>> PC(&_controlPoints[(i+0)*3]);
      subdivide(PC,0,1);
    }
  }
  computeNormals();
  if(_RES==0)
    for(int i=0; i<nrVertex()-1; i++)
      addIndex(Eigen::Matrix<GLfloat,2,1>(i,i+1));
  else tessellate();
}
Eigen::Matrix<GLfloat,6,1> BezierCurveShape::getBB() const {
  if(_dirty) {
    const_cast<BezierCurveShape*>(this)->subdivide();
    const_cast<BezierCurveShape*>(this)->refitBB();
    const_cast<BezierCurveShape*>(this)->_dirty=false;
  }
  return MeshShape::getBB();
}
void BezierCurveShape::subdivide(const Eigen::Matrix<GLfloat,3,4>& vss,GLfloat a,GLfloat b) {
  Eigen::Matrix<GLfloat,4,4> sCP,sCPSub;
  for(int d=0; d<4; d++) {
    sCP.col(d)=CPStencil(d/3.0f);
    sCPSub.col(d)=PTStencil(a+(b-a)*d/3.0f);
  }
  Eigen::Matrix<GLfloat,3,4> cps=vss*sCPSub*sCP.inverse();
  //caculate polygonal length
  GLfloat len=0;
  for(int i=0; i<3; i++)
    len+=(cps.col(i)-cps.col(i+1)).norm();
  //recurse
  if(len<_thickness*5)
    addVertex(vss*CPStencil(a));
  else {
    subdivide(vss,a,(a+b)/2);
    subdivide(vss,(a+b)/2,b);
  }
}
void BezierCurveShape::tessellate() {
  int off=0;
  _indices.clear();
  std::vector<GLfloat> vertices,normals;
  for(int i=0; i+3<(int)_vertices.size(); i+=3) {
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v0(&_vertices[i]);
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v1(&_vertices[i+3]);
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> n0(&_normals[i]);
    Eigen::Matrix<GLfloat,3,1> n1=(v1-v0).cross(n0).normalized();
    for(int r=0; r<_RES; r++) {
      GLfloat theta=r*M_PI*2/_RES;
      Eigen::Matrix<GLfloat,3,1> n=n0*std::cos(theta)+n1*std::sin(theta);
      normals.push_back(n[0]);
      normals.push_back(n[1]);
      normals.push_back(n[2]);
      Eigen::Matrix<GLfloat,3,1> v=(v0+n*_thickness);
      addIndex(Eigen::Matrix<int,4,1>(off+r,off+r+_RES,off+(r+1)%_RES+_RES,off+(r+1)%_RES));
      vertices.push_back(v[0]);
      vertices.push_back(v[1]);
      vertices.push_back(v[2]);
    }
    off+=_RES;
    if(i+6==(int)_vertices.size()) {
      for(int r=0; r<_RES; r++) {
        GLfloat theta=r*M_PI*2/_RES;
        Eigen::Matrix<GLfloat,3,1> n=n0*std::cos(theta)+n1*std::sin(theta);
        normals.push_back(n[0]);
        normals.push_back(n[1]);
        normals.push_back(n[2]);
        Eigen::Matrix<GLfloat,3,1> v=(v1+n*_thickness);
        vertices.push_back(v[0]);
        vertices.push_back(v[1]);
        vertices.push_back(v[2]);
      }
    }
  }
  _vertices.swap(vertices);
  _normals.swap(normals);
  _dirty=true;
}
Eigen::Matrix<GLfloat,4,4> BezierCurveShape::_toCP;
bool BezierCurveShape::_buildMat=false;
}
