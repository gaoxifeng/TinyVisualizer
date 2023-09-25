#ifndef DRAWER_UTILITY_H
#define DRAWER_UTILITY_H

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#ifdef CUSTOM_RTTI_SUPPORT
#include <rtti/rtti.hh>
#else
namespace RTTI {
class Enable {};
#define RTTI_DECLARE_TYPEINFO(T, ...)
}
#endif
#include <memory>
#include <GLFW/glfw3.h>
#include <string>
#include <Eigen/Eigen>
#include <set>

namespace std {
inline std::string to_string(const std::string& str) {
  return str;
}
#ifdef CUSTOM_RTTI_SUPPORT
template<typename _Tp, typename _Up>
inline shared_ptr<_Tp> custom_pointer_cast(const shared_ptr<_Up>& __r) noexcept {
  using _Sp = shared_ptr<_Tp>;
  if (auto* __p = __r.get()->template cast<typename _Sp::element_type>())
    return _Sp(__r, __p);
  return _Sp();
}
#else
template<typename _Tp, typename _Up>
inline shared_ptr<_Tp> custom_pointer_cast(const shared_ptr<_Up>& __r) noexcept {
  return dynamic_pointer_cast<_Tp, _Up>(__r);
}
#endif
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
enum class CameraType {
  camera2D=0,
  cameraFirstPerson,
  cameraThirdPerson
};
extern Eigen::Matrix<GLfloat,6,1> resetBB();
extern bool isEmptyBB(const Eigen::Matrix<GLfloat,6,1>& bb);
extern void drawBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,1>& color);
extern Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,6,1>& b);
extern Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& b);
extern Eigen::Matrix<GLfloat,6,1> scaleBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s);
extern Eigen::Matrix<GLfloat,6,1> translateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s);
extern Eigen::Matrix<GLfloat,6,1> rotateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,3>& r);
extern Eigen::Matrix<GLfloat,6,1> transformBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,4>& r);
extern bool rayIntersectBB(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat alpha,const Eigen::Matrix<GLfloat,6,1>& bb);
extern bool rayIntersectBB(const Eigen::Matrix<GLfloat,3,1>& p,const Eigen::Matrix<GLfloat,3,1>& q,const Eigen::Matrix<GLfloat,6,1>& bb);
extern bool rayIntersectTri(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha,const Eigen::Matrix<GLfloat,3,1>& a,const Eigen::Matrix<GLfloat,3,1>& b,const Eigen::Matrix<GLfloat,3,1>& c);
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
//draw simple shapes
extern void clearVBO();
extern void drawPointf(const Eigen::Matrix<GLfloat,2,1>& v0);
extern void drawPointf(const Eigen::Matrix<GLfloat,3,1>& v0);
extern void drawLinef(const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& v1);
extern void drawLinef(const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,3,1>& v1);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,3,1>& v1,
                      const Eigen::Matrix<GLfloat,3,1>& v2,
                      const Eigen::Matrix<GLfloat,3,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,2,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,2,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,2,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,3,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,3,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,3,1>& v3);
}

#endif
