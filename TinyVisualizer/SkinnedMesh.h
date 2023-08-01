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
  GLuint nrAnimation() const;
  GLfloat duration(GLuint index) const;
 private:
  GLuint getBoneId(const aiBone* bone);
  GLfloat calcAnimationTimeTicks(GLfloat time,GLint index) const;
  Eigen::Matrix<GLfloat,3,1> calcInterpolatedScaling(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  Eigen::Matrix<GLfloat,3,3> calcInterpolatedRotation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  Eigen::Matrix<GLfloat,3,1> calcInterpolatedTranslation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  void readNodeHierarchy(GLfloat animationTimeTicks,const aiNode* pNode,const Eigen::Matrix<GLfloat,4,4>& parentTransform,const aiAnimation& animation);
  const aiScene* _scene;
  Assimp::Importer _importer;
  std::vector<BoneInfo> _bones;
  std::unordered_map<std::string,GLuint> _boneNameToIndexMap;
};
}

#endif
