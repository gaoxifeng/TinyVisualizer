#ifndef MESH_SHAPE_H
#define MESH_SHAPE_H

#include "Drawer.h"

namespace DRAWER {
class Texture;
class MeshShape : public Shape {
 public:
  MeshShape();
  MeshShape(const std::vector<GLfloat>& vertices,const std::vector<GLuint>& indices,GLenum mode);
  template <typename VEC,typename VECTC=Eigen::Matrix<GLfloat,2,1>>
  void addVertex(const VEC& v,const VECTC* tc=NULL) {
    _vertices.reserve(_vertices.size()+v.size());
    if(tc) {
      _texcoords.reserve(_texcoords.size()+tc->size());
      ASSERT_MSG(_texcoords.size()/2==_vertices.size()/3,"_texcoords.size() != vertices.size()!")
    }
    for(int i=0,j=0; i<v.size(); i+=3,j+=2) {
      _vertices.push_back(v[i+0]);
      _vertices.push_back(v[i+1]);
      _vertices.push_back(v[i+2]);
      if(tc) {
        _texcoords.push_back((*tc)[j+0]);
        _texcoords.push_back((*tc)[j+1]);
      }
    }
    _dirty=true;
  }
  template <typename VEC>
  void addIndex(const VEC& I) {
    for(int i=0; i<I.size(); i++)
      _indices.push_back(I[i]);
  }
  void addIndexSingle(int i);
  void setMode(GLenum mode);
  int nrVertex() const;
  virtual void computeNormals();
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getNormal(int i);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getVertex(int i);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getVertex(int i) const;
  virtual void setPointSize(GLfloat pointSize) override;
  virtual void setLineWidth(GLfloat lineWidth) override;
  virtual void setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B) override;
  virtual void setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) override;
  virtual void setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) override;
  virtual void setTexture(std::shared_ptr<Texture> tex) override;
  virtual void setShininess(GLenum mode,GLfloat S) override;
  virtual void draw(bool shadowPass) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
 protected:
  void refitBB();
  std::shared_ptr<Texture> _tex;
  static std::shared_ptr<Texture> _texWhite;
  std::vector<GLfloat> _vertices,_normals,_texcoords;
  Eigen::Matrix<GLfloat,6,1> _bb;
  std::vector<GLuint> _indices;
  GLfloat _pointSize,_lineWidth;
  GLfloat _rs,_gs,_bs,_s;
  GLfloat _ra,_ga,_ba;
  GLfloat _r,_g,_b;
  GLenum _mode;
  bool _dirty;
};
}

#endif
