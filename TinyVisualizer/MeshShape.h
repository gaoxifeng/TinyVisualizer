#ifndef MESH_SHAPE_H
#define MESH_SHAPE_H

#include "VBO.h"
#include "Drawer.h"
#include "ShadowAndLight.h"

namespace DRAWER {
class Texture;
class EXPORT MeshShape : public Shape {
  RTTI_DECLARE_TYPEINFO(MeshShape, Shape);
  friend class LowDimensionalMeshShape;
  friend class SkinnedMeshShape;
 public:
  struct BoneData {
    BoneData();
    bool empty() const;
    GLfloat findWeight(int vertexId,int boneId) const;
    GLuint _maxNrBone;
    std::vector<GLint> _boneId;
    std::vector<GLfloat> _boneWeight;
  };
  MeshShape();
  MeshShape(const std::vector<GLfloat>& vertices,const std::vector<GLuint>& indices,GLenum mode);
  virtual ~MeshShape();
  void addVertex(const Eigen::Matrix<GLfloat,-1,1>& v,const Eigen::Matrix<GLfloat,-1,1>* tc=NULL);
  void addIndex(const Eigen::Matrix<GLuint,-1,1>& I);
  void addIndexSingle(int i);
  void setMode(GLenum mode);
  int nrVertex() const;
  int nrIndex() const;
  void clear();
  void clearIndex();
  virtual void computeNormals();
  void setNormal(int i,const Eigen::Matrix<GLfloat,3,1>& normal);
  Eigen::Matrix<GLfloat,3,1> getNormal(int i) const;
  void setVertex(int i,const Eigen::Matrix<GLfloat,3,1>& vertex);
  void setVertices(const std::vector<GLfloat>& vertices);  //batched update of vertices
  Eigen::Matrix<GLfloat,3,1> getVertex(int i) const;
  void setTexcoord(int i,const Eigen::Matrix<GLfloat,2,1>& texcoord);
  Eigen::Matrix<GLfloat,2,1> getTexcoord(int i) const;
  GLuint getIndex(int i) const;
  void debugWriteObj(const std::string& path);
  std::shared_ptr<Texture> getTextureDiffuse() const;
  std::shared_ptr<Texture> getTextureSpecular() const;
  std::shared_ptr<Material> getMaterial() const;
  void setMaterial(std::shared_ptr<Material> mat);
  BoneData& getBoneData();
  const BoneData& getBoneData() const;
  void setBoneData(const BoneData& bone);
  std::shared_ptr<VBO> getVBO();    //a dangerous method, we will flag mesh as dirty when this is called
  virtual void setAlpha(GLfloat alpha) override;
  virtual void setPointSize(GLfloat pointSize) override;
  virtual void setLineWidth(GLfloat lineWidth) override;
  virtual void setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B) override;
  virtual void setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) override;
  virtual void setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) override;
  virtual void setTextureDiffuse(std::shared_ptr<Texture> tex) override;
  virtual void setTextureSpecular(std::shared_ptr<Texture> tex) override;
  virtual void setDepth(GLfloat depth) override;
  virtual void setDrawer(Drawer* drawer) override;
  virtual void setShininess(GLenum mode,GLfloat S) override;
  virtual void draw(PASS_TYPE passType) const override;
  virtual void drawPovray(Povray& pov) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const override;
 protected:
  void initVBO();
  void refitBB();
  void initMaterial();
  std::shared_ptr<VBO> _VBO;
  static std::shared_ptr<Texture> _texWhite;
  std::vector<GLfloat> _vertices,_normals,_texcoords;
  std::shared_ptr<Material> _mat;
  Eigen::Matrix<GLfloat,6,1> _bb;
  std::vector<GLuint> _indices;
  BoneData _bone;
  GLenum _mode;
  bool _dirty;
};
}

#endif
