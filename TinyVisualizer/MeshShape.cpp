#include "MeshShape.h"
#include "Texture.h"
#include "Matrix.h"
#include "MakeTexture.h"
#include "DefaultLight.h"

namespace DRAWER {
//MeshShape
#define DEFAULT_R 200/255.
#define DEFAULT_G 143/255.
#define DEFAULT_B 29/255.
#define DEFAULT_S 10.
MeshShape::MeshShape():_bb(resetBB()),_dirty(true) {
  initMaterial();
}
MeshShape::MeshShape(const std::vector<GLfloat>& vertices,const std::vector<GLuint>& indices,GLenum mode):_bb(resetBB()),_dirty(true) {
  _vertices=vertices;
  _indices=indices;
  _bb=resetBB();
  for(int i=0; i<(int)vertices.size(); i+=3) {
    Eigen::Matrix<GLfloat,3,1> v=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&vertices[i]);
    _bb=unionBB(_bb,v);
  }
  setMode(mode);
  initMaterial();
}
void MeshShape::addIndexSingle(int i) {
  _indices.push_back(i);
  _VBO=NULL;
}
void MeshShape::setMode(GLenum mode) {
  _mode=mode;
}
int MeshShape::nrVertex() const {
  return (int)_vertices.size()/3;
}
int MeshShape::nrIndex() const {
  return (int)_indices.size();
}
void MeshShape::clear() {
  _vertices.clear();
  _normals.clear();
  _texcoords.clear();
  _indices.clear();
  _VBO=NULL;
}
void MeshShape::computeNormals() {
  ASSERT_MSGV(_mode==GL_TRIANGLES,"Compute normals is only available when mode(%d)=GL_TRIANGLES!",_mode)
  _normals.assign(_vertices.size(),0);
  for(int i=0; i<(int)_indices.size(); i+=3) {
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v0(&_vertices[_indices[i+0]*3]);
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v1(&_vertices[_indices[i+1]*3]);
    Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> v2(&_vertices[_indices[i+2]*3]);
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n0(&_normals[_indices[i+0]*3]);
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n1(&_normals[_indices[i+1]*3]);
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n2(&_normals[_indices[i+2]*3]);
    Eigen::Matrix<GLfloat,3,1> n=(v1-v0).cross(v2-v0);
    n0+=n;
    n1+=n;
    n2+=n;
  }
  for(int i=0; i<(int)_normals.size(); i+=3)
    Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(&_normals[i]).normalize();
  if(_VBO)
    _VBO->setVertexNormal(_normals);
  else _VBO=NULL;
}
void MeshShape::setNormal(int i,const Eigen::Matrix<GLfloat,3,1>& normal) {
  if((int)_normals.size()<i*3+3)
    _normals.resize(i*3+3,0);
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_normals.data()+i*3)=normal;
  if(_VBO)
    _VBO->setVertexNormal(i,normal);
  else _VBO=NULL;
}
Eigen::Matrix<GLfloat,3,1> MeshShape::getNormal(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_normals[i*3]);
}
void MeshShape::setVertex(int i,const Eigen::Matrix<GLfloat,3,1>& vertex) {
  _dirty=true;
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_vertices.data()+i*3)=vertex;
  if(_VBO)
    _VBO->setVertexPosition(i,vertex);
  else _VBO=NULL;
}
Eigen::Matrix<GLfloat,3,1> MeshShape::getVertex(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_vertices[i*3]);
}
void MeshShape::setPointSize(GLfloat pointSize) {
  _mat._pointSize=pointSize;
}
void MeshShape::setLineWidth(GLfloat lineWidth) {
  _mat._lineWidth=lineWidth;
}
void MeshShape::setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B) {
  if(_mode!=mode)
    return;
  _mat._diffuse=Eigen::Matrix<GLfloat,4,1>(R,G,B,1);
}
void MeshShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) {
  if(_mode!=mode)
    return;
  _mat._ambient=Eigen::Matrix<GLfloat,4,1>(RA,GA,BA,1);
}
void MeshShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) {
  if(_mode!=mode)
    return;
  _mat._specular=Eigen::Matrix<GLfloat,4,1>(RS,GS,BS,1);
}
void MeshShape::setShininess(GLenum mode,GLfloat S) {
  if(_mode!=mode)
    return;
  _mat._shininess=S;
}
void MeshShape::setTexture(std::shared_ptr<Texture> tex) {
  if(tex)
    _mat._tex=tex;
  else _mat._tex=getWhiteTexture();
}
void MeshShape::setDepth(GLfloat depth) {
  for(int i=0; i<(int)_vertices.size(); i+=3)
    _vertices[i+2]=depth;
}
void MeshShape::setDrawer(Drawer* drawer) {
  _mat._drawer=drawer;
}
void MeshShape::draw(PASS_TYPE passType) const {
  if(passType&MESH_PASS) {
    if(_mode!=GL_TRIANGLES&&_mode!=GL_TRIANGLE_FAN&&_mode!=GL_TRIANGLE_STRIP)
      return;
  }
  if(passType&LINE_PASS) {
    if(_mode!=GL_LINES&&_mode!=GL_LINE_LOOP&&_mode!=GL_LINE_STRIP)
      return;
  }
  if(passType&POINT_PASS) {
    if(_mode!=GL_POINTS)
      return;
  }
  if(!_castShadow && (passType&SHADOW_PASS)!=0)
    return;
  if(_vertices.empty() || !enabled())
    return;
  if(_dirty) {
    const_cast<MeshShape*>(this)->refitBB();
    const_cast<MeshShape*>(this)->_dirty=false;
  }
  if(!_VBO) {
    const_cast<MeshShape*>(this)->_VBO.reset(new VBO(nrVertex(),nrIndex()));
    _VBO->setVertexPosition(_vertices);
    if(!_normals.empty())
      _VBO->setVertexNormal(_normals);
    if(!_texcoords.empty())
      _VBO->setVertexTexCoord(_texcoords);
    _VBO->setIndex(_indices);
  }
  //mode begin
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
  //turn on material
  if((passType&SHADOW_PASS)==0)
    setupMaterial(_mat);
  setupMatrixInShader();
  //turn on texture
  if(_mat._tex) {
    glActiveTexture(GL_TEXTURE0);
    _mat._tex->begin();
    glActiveTexture(GL_TEXTURE1);
  }
  _VBO->draw(_mode);
  //turn off texture
  if(_mat._tex) {
    _mat._tex->end();
    glActiveTexture(GL_TEXTURE0);
  }
}
Eigen::Matrix<GLfloat,6,1> MeshShape::getBB() const {
  if(_dirty) {
    const_cast<MeshShape*>(this)->refitBB();
    const_cast<MeshShape*>(this)->_dirty=false;
  }
  return _bb;
}
bool MeshShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
#define VERT(ID) getVertex(_indices[ID])
  bool ret=false;
  if(_mode==GL_TRIANGLES) {
    for(int i=0; i<(int)_indices.size(); i+=3)
      if(rayIntersectTri(ray,alpha,VERT(i),VERT(i+1),VERT(i+2)))
        ret=true;
  } else if(_mode==GL_TRIANGLE_FAN) {
    for(int i=2; i<(int)_indices.size(); i++)
      if(rayIntersectTri(ray,alpha,VERT(0),VERT(i-1),VERT(i)))
        ret=true;
  } else if(_mode==GL_TRIANGLE_STRIP) {
    for(int i=2; i<(int)_indices.size(); i++)
      if(rayIntersectTri(ray,alpha,VERT(i),VERT(i-1),VERT(i-2)))
        ret=true;
  }
  return ret;
#undef VERT
}
void MeshShape::refitBB() {
  _bb=resetBB();
  for(int i=0; i<nrVertex(); i++)
    _bb=unionBB(_bb,(Eigen::Matrix<GLfloat,3,1>)getVertex(i));
}
void MeshShape::initMaterial() {
  if(!_texWhite)
    _texWhite=getWhiteTexture();
  _mat._ambient=Eigen::Matrix<GLfloat,4,1>(0,0,0,1);
  _mat._diffuse=_mat._specular=Eigen::Matrix<GLfloat,4,1>(DEFAULT_R,DEFAULT_G,DEFAULT_B,1);
  _mat._shininess=DEFAULT_S;
  _mat._pointSize=1;
  _mat._lineWidth=0;
  _mat._tex=_texWhite;
  _mat._drawer=NULL;
}
std::shared_ptr<Texture> MeshShape::_texWhite;
}
