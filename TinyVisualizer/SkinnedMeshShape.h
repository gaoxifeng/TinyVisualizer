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
  struct BoneData {
    GLuint _maxNrBone;
    std::vector<GLint> _boneId;
    std::vector<GLfloat> _boneWeight;
  };
  SkinnedMeshShape(const std::string& filename);
  void setAnimatedFrame(GLuint index,GLfloat time,bool updateMesh=true);
  std::shared_ptr<MeshShape> getMeshRef(int id) const;
  const std::vector<BoneInfo>& getBoneInfo() const;
  const std::vector<BoneData>& getBoneData() const;
  GLfloat duration(GLuint index) const;
  GLuint nrAnimation() const;
 private:
  GLuint getBoneId(const aiBone* bone);
  GLfloat calcAnimationTimeTicks(GLfloat time,GLint index) const;
  Eigen::Matrix<GLfloat,3,1> calcInterpolatedScaling(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  Eigen::Matrix<GLfloat,3,3> calcInterpolatedRotation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  Eigen::Matrix<GLfloat,3,1> calcInterpolatedTranslation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const;
  void readNodeHierarchy(GLfloat animationTimeTicks,const aiNode* pNode,const Eigen::Matrix<GLfloat,4,4>& parentTransform,const aiAnimation& animation);
  void updateMeshVertices(std::shared_ptr<MeshShape> out,std::shared_ptr<MeshShape> in,const BoneData& boneData) const;
  //data
  const aiScene* _scene;
  Assimp::Importer _importer;
  std::vector<BoneInfo> _bones;
  std::unordered_map<std::string,GLuint> _boneNameToIndexMap;
  std::vector<std::shared_ptr<MeshShape>> _refMeshes;
  std::vector<BoneData> _boneDatas;
};
}

#endif
