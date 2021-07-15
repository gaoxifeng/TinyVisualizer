#ifndef DRAWER_UTILITY_H
#define DRAWER_UTILITY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string>
#include <Eigen/Eigen>
#include <set>

namespace std {
inline std::string to_string(const std::string& str) {
  return str;
}
}
namespace DRAWER {
#define ASSERT(var) {do{if(!(var)){exit(EXIT_FAILURE);}}while(0);}
#define ASSERT_MSG(var,msg) {do{if(!(var)){printf(msg);exit(EXIT_FAILURE);}}while(0);}
#define ASSERT_MSGV(var,msg,...) {do{if(!(var)){printf(msg,__VA_ARGS__);exit(EXIT_FAILURE);}}while(0);}
inline void reportGLError() {
  GLenum ret=glGetError();
  ASSERT_MSGV(ret==GL_NO_ERROR,"OpenGL Error: %d",ret)
}
template <typename T,typename TF>
T interp1D(const T& v0,const T& v1,const TF& px) {
  return v0*(1.0f-px)+v1*px;
}
template <typename T,typename TF>
T interp2D(const T& v0,const T& v1,const T& v2,const T& v3,const TF& px,const TF& py) {
  return interp1D(interp1D(v0,v1,px),
                  interp1D(v2,v3,px),py);
}
inline Eigen::Matrix<GLfloat,6,1> resetBB() {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).setConstant(std::numeric_limits<GLfloat>::max());
  ret.segment<3>(3).setConstant(-std::numeric_limits<GLfloat>::max());
  return ret;
}
inline Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,6,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b.segment<3>(0));
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b.segment<3>(3));
  return ret;
}
inline Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b);
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b);
  return ret;
}
inline Eigen::Matrix<GLfloat,6,1> scaleBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()*s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()*s.array();
  return ret;
}
inline Eigen::Matrix<GLfloat,6,1> translateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()+s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()+s.array();
  return ret;
}
inline Eigen::Matrix<GLfloat,6,1> rotateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,3>& r) {
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
inline Eigen::Matrix<GLfloat,6,1> transformBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,4>& r) {
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
inline void zRange(const Eigen::Matrix<GLfloat,6,1>& bb,const Eigen::Matrix<GLfloat,3,1>& pos,const Eigen::Matrix<GLfloat,3,1>& dir,
                   GLfloat& zNear,GLfloat& zFar,GLfloat minZNear=0.1f,GLfloat maxZFar=100.0f) {
  if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max()) {
    zNear=minZNear;
    zFar=maxZFar;
    return;
  }

  Eigen::Matrix<GLfloat,3,1> ctr=(bb.segment<3>(0)+bb.segment<3>(3))/2-pos;
  Eigen::Matrix<GLfloat,3,1> rng=(bb.segment<3>(3)-bb.segment<3>(0))/2;

  zNear=ctr.dot(dir)-(rng.array()*dir.array()).abs().sum();
  zFar=ctr.dot(dir)+(rng.array()*dir.array()).abs().sum();
  zNear=std::max<GLfloat>(minZNear,zNear);
  zFar=std::min<GLfloat>(maxZFar,zFar);
}
inline Eigen::Matrix<GLfloat,24,1> constructViewFrustum3D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  glGetFloatv(GL_MODELVIEW_MATRIX,MV.data());
  glGetFloatv(GL_PROJECTION_MATRIX,P.data());
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,24,1> ret;
  for(int z=-1,i=0; z<=1; z+=2)
    for(int y=-1; y<=1; y+=2)
      for(int x=-1; x<=1; x+=2,i+=3) {
        Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>(x,y,z,1);
        ret.segment<3>(i)=V.segment<3>(0)/V[3];
      }
  return ret;
}
inline Eigen::Matrix<GLfloat,8,1> constructViewFrustum2D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  glGetFloatv(GL_MODELVIEW_MATRIX,MV.data());
  glGetFloatv(GL_PROJECTION_MATRIX,P.data());
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,8,1> ret;
  for(int y=-1,i=0; y<=1; y+=2)
    for(int x=-1; x<=1; x+=2,i+=2) {
      Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>(x,y,0,1);
      ret.segment<2>(i)=(V.segment<3>(0)/V[3]).segment<2>(0);
    }
  return ret;
}
inline Eigen::Matrix<GLfloat,24,1> getViewFrustum3DPlanes() {
  //left,right,bottom,top,back,front
  Eigen::Matrix<GLfloat,24,1> planes,corners=constructViewFrustum3D();
  std::function<Eigen::Matrix<GLfloat,4,1>(int,int,int)> PLANE=[&](int a,int b,int c) {
    Eigen::Matrix<GLfloat,4,1> ret;
    ret.segment<3>(0)=(corners.segment<3>(c*3)-corners.segment<3>(a*3)).cross(corners.segment<3>(b*3)-corners.segment<3>(a*3)).normalized();
    ret[3]=-corners.segment<3>(a*3).dot(ret.segment<3>(0));
    return ret;
  };
  planes.segment<4>(0)=PLANE(0,4,6);
  planes.segment<4>(4)=PLANE(1,3,7);
  planes.segment<4>(8)=PLANE(0,1,5);
  planes.segment<4>(12)=PLANE(2,6,7);
  planes.segment<4>(16)=PLANE(0,2,3);
  planes.segment<4>(20)=PLANE(4,5,7);
  return planes;
}
inline Eigen::Matrix<GLfloat,16,1> getViewFrustum2DPlanes() {
  //left,right,bottom,top
  Eigen::Matrix<GLfloat,8,1> corners=constructViewFrustum2D();
  Eigen::Matrix<GLfloat,16,1> planes;
  planes.segment<4>(0 )=Eigen::Matrix<GLfloat,4,1>(-1,0,0, corners.segment<2>(0)[0]);
  planes.segment<4>(4 )=Eigen::Matrix<GLfloat,4,1>( 1,0,0,-corners.segment<2>(2)[0]);
  planes.segment<4>(8 )=Eigen::Matrix<GLfloat,4,1>(0,-1,0, corners.segment<2>(0)[1]);
  planes.segment<4>(12)=Eigen::Matrix<GLfloat,4,1>(0, 1,0,-corners.segment<2>(4)[1]);
  return planes;
}
inline void drawViewFrustum3D(const Eigen::Matrix<GLfloat,24,1>& frustum) {
#define VERT(V) glVertex3f(frustum[V*3+0],frustum[V*3+1],frustum[V*3+2]);
#define QUAD(A,B,C,D) VERT(A) VERT(B) VERT(C) VERT(D)
  glBegin(GL_QUADS);
  QUAD(0,2,3,1)
  QUAD(4,5,7,6)
  QUAD(0,1,5,4)
  QUAD(2,6,7,3)
  QUAD(0,4,6,2)
  QUAD(1,3,7,5)
  glEnd();
#undef QUAD
#undef VERT
}
inline void drawViewFrustum2D(const Eigen::Matrix<GLfloat,8,1>& frustum) {
#define VERT(V) glVertex2f(frustum[V*2+0],frustum[V*2+1]);
#define QUAD(A,B,C,D) VERT(A) VERT(B) VERT(C) VERT(D)
  glBegin(GL_QUADS);
  QUAD(0,2,3,1)
  glEnd();
#undef QUAD
#undef VERT
}
template <typename ITER>
std::string listToStr(ITER beg,ITER end) {
  bool empty=true;
  std::string ret;
  ret.push_back('[');
  while(beg!=end) {
    empty=false;
    ret+=std::to_string(*beg++);
    ret+=",";
  }
  if(empty)
    ret+="]";
  else ret.back()=']';
  return ret;
}
//parse command line args
template <typename T>
T argparseChoice(int argc,char** argv,const std::string& name,std::function<T(const std::string& arg)> stoT,T def,const std::set<T>* choice) {
  for(int i=0; i<argc-1; i++)
    if(argv[i]=="--"+name) {
      def=stoT(argv[i+1]);
      if(choice) {
        ASSERT_MSGV(choice->find(def)!=choice->end(),"Variable range error (%s not in %s)",std::to_string(def).c_str(),listToStr(choice->begin(),choice->end()).c_str())
      }
      return def;
    }
  if(choice) {
    ASSERT_MSGV(choice->find(def)!=choice->end(),"Variable range error (%s not in %s)",std::to_string(def).c_str(),listToStr(choice->begin(),choice->end()).c_str())
  }
  return def;
}
template <typename T>
T argparseChoice(int argc,char** argv,const std::string& name,std::function<T(const std::string& arg)> stoT,T def,const std::set<T>& choice) {
  return argparseChoice(argc,argv,name,stoT,def,&choice);
}
template <typename T>
T argparseRange(int argc,char** argv,const std::string& name,std::function<T(const std::string& arg)> stoT,T def,const Eigen::Matrix<T,2,1>* range) {
  for(int i=0; i<argc-1; i++)
    if(argv[i]=="--"+name) {
      def=stoT(argv[i+1]);
      if(range) {
        ASSERT_MSGV(def>=(*range)[0] && def<=(*range)[1],"Variable range error (%s<=%s<%s)",std::to_string((*range)[0]).c_str(),std::to_string(def).c_str(),std::to_string((*range)[1]).c_str())
      }
      return def;
    }
  if(range) {
    ASSERT_MSGV(def>=(*range)[0] && def<(*range)[1],"Variable range error (%s<=%s<%s)",std::to_string((*range)[0]).c_str(),std::to_string(def).c_str(),std::to_string((*range)[1]).c_str())
  }
  return def;
}
template <typename T>
T argparseRange(int argc,char** argv,const std::string& name,std::function<T(const std::string& arg)> stoT,T def,const Eigen::Matrix<T,2,1>& range) {
  return argparseRange(argc,argv,name,stoT,def,&range);
}
//define for each time
#define ARGPARSE_T(T,LAMBDA)\
inline T argparseChoice(int argc,char** argv,const std::string& name,T def,const std::set<T>* choice=NULL){return argparseChoice<T>(argc,argv,name,LAMBDA,def,choice);}\
inline T argparseChoice(int argc,char** argv,const std::string& name,T def,const std::set<T>& choice){return argparseChoice<T>(argc,argv,name,LAMBDA,def,&choice);}\
inline T argparseRange(int argc,char** argv,const std::string& name,T def,const Eigen::Matrix<T,2,1>* range=NULL){return argparseRange<T>(argc,argv,name,LAMBDA,def,range);}\
inline T argparseRange(int argc,char** argv,const std::string& name,T def,const Eigen::Matrix<T,2,1>& range){return argparseRange<T>(argc,argv,name,LAMBDA,def,&range);}
ARGPARSE_T(std::string,[](const std::string& arg) {
  return arg;
})
ARGPARSE_T(int,[](const std::string& arg) {
  return stoi(arg);
})
}

#endif
