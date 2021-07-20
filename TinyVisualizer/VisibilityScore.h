#ifndef VISIBILITY_SCORE_H
#define VISIBILITY_SCORE_H

#include "Drawer.h"
#include "FBO.h"
#include "MeshShape.h"
#include "Shader.h"

namespace DRAWER {
class VisibilityScore {
 public:
  VisibilityScore(int levelMax,GLenum formatColor=GL_R32F,GLenum formatDepth=GL_DEPTH_COMPONENT24);
  Eigen::Matrix<GLfloat,2,1> compute(std::function<void(const FBO&)>* ref,std::function<void(const FBO&)>* curr,bool debugOutput=false);
  void debugVisibility();
  void debugVisibility(Drawer& drawer,const Eigen::Vector3f& up,
                       const std::vector<Eigen::Vector3f>& eyes,const std::vector<Eigen::Vector3f>& dirs,
                       std::shared_ptr<MeshShape> shapeA,std::shared_ptr<MeshShape> shapeB);
 private:
  void beginXOR();
  void endXOR();
  void texCopy(int i);
  Shader _shaderXOR,_shaderTexCopy;
  FBOPingPong _fboPP;
  FBO _fboRef;
};
}

#endif
