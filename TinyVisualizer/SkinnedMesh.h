#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "Bullet3DShape.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace DRAWER {
class SkinnedMeshShape : public Bullet3DShape {
 public:
  struct BoneInfo {
    Eigen::Matrix<GLfloat,4,4> _offsetTrans;
    Eigen::Matrix<GLfloat,4,4> _finalTrans;
  };
  SkinnedMeshShape(const std::string& filename);
  void setAnimatedFrame(GLuint index,GLfloat time);
 private:
  GLfloat calcAnimationTimeTicks(GLfloat time,GLint index) const;
  void readNodeHierarchy(GLfloat animationTimeTicks,const aiNode* pNode,const Eigen::Matrix<GLfloat,4,4>& parentTransform,const aiAnimation& Animation);
  const aiScene* _scene;
  Assimp::Importer _importer;
  std::vector<BoneInfo> _bones;

};
}

#endif
