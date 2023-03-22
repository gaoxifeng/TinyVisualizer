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
  Eigen::Matrix<GLfloat,2,1> compute
  (std::function<void(const FBO&)>* ref,
   std::function<void(const FBO&)>* curr,
   bool debugOutput=false);
  std::pair<Eigen::Matrix<GLfloat,2,1>,Eigen::Matrix<GLfloat,2,1>> compute
      (Drawer& drawer,const Eigen::Vector3f& up,
       const std::vector<Eigen::Matrix<GLfloat,3,1>>& eyes,
       const std::vector<Eigen::Matrix<GLfloat,3,1>>& dirs,
       std::shared_ptr<MeshShape> shapeA,std::shared_ptr<MeshShape> shapeB,
       bool debugOutput=false);
  void debugVisibility();
  void debugVisibility(Drawer& drawer);
  std::shared_ptr<Program> getXORProg() const;
  std::shared_ptr<Program> getTexCopyProg() const;
 private:
  void beginXOR();
  void endXOR();
  void texCopy(int i);
  FBOPingPong _fboPP;
  FBO _fboRef;
};
}

#endif
