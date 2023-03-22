#include "DrawerUtility.h"
#include "DefaultLight.h"
#include "Matrix.h"
#include "VBO.h"

namespace DRAWER {
Eigen::Matrix<GLfloat,6,1> resetBB() {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).setConstant(std::numeric_limits<GLfloat>::max());
  ret.segment<3>(3).setConstant(-std::numeric_limits<GLfloat>::max());
  return ret;
}
void drawBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,1>& color) {
#define VERT(X,Y,Z) Eigen::Matrix<GLfloat,3,1>(X==0?a[0]:a[3],Y==0?a[1]:a[4],Z==0?a[2]:a[5])
  getDefaultProg()->begin();
  setupMaterial(NULL,color);
  setupMatrixInShader();
  for(int x=0; x<2; x++)
    for(int y=0; y<2; y++) {
      drawLinef(VERT(x,y,0),VERT(x,y,1));
      drawLinef(VERT(x,0,y),VERT(x,1,y));
      drawLinef(VERT(0,x,y),VERT(1,x,y));
    }
  Program::currentProgram()->end();
#undef VERT
}
Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,6,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b.segment<3>(0));
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b.segment<3>(3));
  return ret;
}
Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b);
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b);
  return ret;
}
Eigen::Matrix<GLfloat,6,1> scaleBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()*s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()*s.array();
  return ret;
}
Eigen::Matrix<GLfloat,6,1> translateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()+s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()+s.array();
  return ret;
}
Eigen::Matrix<GLfloat,6,1> rotateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,3>& r) {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB();
  for(GLfloat x: {
        a[0],a[3]
      })
    for(GLfloat y: {
          a[1],a[4]
        })
      for(GLfloat z: {
            a[2],a[5]
          }) {
        Eigen::Matrix<GLfloat,3,1> pos(x,y,z);
        pos=r*pos;
        ret=unionBB(ret,pos);
      }
  return ret;
}
Eigen::Matrix<GLfloat,6,1> transformBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,4>& r) {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB();
  for(GLfloat x: {
        a[0],a[3]
      })
    for(GLfloat y: {
          a[1],a[4]
        })
      for(GLfloat z: {
            a[2],a[5]
          }) {
        Eigen::Matrix<GLfloat,4,1> pos(x,y,z,1);
        pos=r*pos;
        ret=unionBB(ret,Eigen::Matrix<GLfloat,3,1>(pos.segment<3>(0)/pos[3]));
      }
  return ret;
}
bool rayIntersectBB(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat alpha,const Eigen::Matrix<GLfloat,6,1>& bb) {
  return rayIntersectBB(ray.segment<3>(0),ray.segment<3>(0)+ray.segment<3>(3)*alpha,bb);
}
bool rayIntersectBB(const Eigen::Matrix<GLfloat,3,1>& p,const Eigen::Matrix<GLfloat,3,1>& q,const Eigen::Matrix<GLfloat,6,1>& bb) {
  GLfloat s=0,t=1;
  Eigen::Matrix<GLfloat,3,1> minC=bb.segment<3>(0);
  Eigen::Matrix<GLfloat,3,1> maxC=bb.segment<3>(3);
  for(int i=0; i<3; ++i) {
    GLfloat D=q[i]-p[i];
    if(p[i]<q[i]) {
      GLfloat s0=(minC[i]-p[i])/D, t0=(maxC[i]-p[i])/D;
      if(s0>s) s=s0;
      if(t0<t) t=t0;
    } else if(p[i]>q[i]) {
      GLfloat s0=(maxC[i]-p[i])/D, t0=(minC[i]-p[i])/D;
      if(s0>s) s=s0;
      if(t0<t) t=t0;
    } else {
      if(p[i]<minC[i] || p[i]>maxC[i])
        return false;
    }
    if(s>t)
      return false;
  }
  return true;
}
bool rayIntersectTri(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha,const Eigen::Matrix<GLfloat,3,1>& a,const Eigen::Matrix<GLfloat,3,1>& b,const Eigen::Matrix<GLfloat,3,1>& c) {
  Eigen::Matrix<GLfloat,3,1> abt;
  Eigen::Matrix<GLfloat,3,3> mat;
  mat.col(0)=a-c;
  mat.col(1)=b-c;
  mat.col(2)=-ray.segment<3>(3);
  if(std::abs(mat.determinant())<std::numeric_limits<GLfloat>::min())
    return false;

  abt=mat.inverse()*(ray.segment<3>(0)-c);
  bool ret=(abt.x()>=0.0f && abt.y()>=0.0f && (abt.x()+abt.y())<=1.0f) && //in triangle
           (abt.z()>=0.0f && abt.z()<alpha);  //in segment
  if(ret)
    alpha=abt.z();
  return ret;
}
//draw simple shapes
std::shared_ptr<VBO> VBOPoint;
std::shared_ptr<VBO> VBOLine;
std::shared_ptr<VBO> VBOQuad;
void clearVBO() {
  VBOPoint=NULL;
  VBOLine=NULL;
  VBOQuad=NULL;
}
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
