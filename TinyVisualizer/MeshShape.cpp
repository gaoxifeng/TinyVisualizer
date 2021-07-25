#include "MeshShape.h"
#include "Texture.h"
#include "MakeTexture.h"

namespace DRAWER {
//MeshShape
#define DEFAULT_R 200/255.
#define DEFAULT_G 143/255.
#define DEFAULT_B 29/255.
#define DEFAULT_S 10.
MeshShape::MeshShape()
  :_bb(resetBB()),_pointSize(1),_lineWidth(1),
   _rs(DEFAULT_R),_gs(DEFAULT_G),_bs(DEFAULT_B),_s(DEFAULT_S),
   _ra(0),_ga(0),_ba(0),
   _r(DEFAULT_R),_g(DEFAULT_G),_b(DEFAULT_B),_dirty(true) {
  if(!_texWhite)
    _texWhite=getWhiteTexture();
  _tex=_texWhite;
}
MeshShape::MeshShape(const std::vector<GLfloat>& vertices,const std::vector<GLuint>& indices,GLenum mode)
  :_bb(resetBB()),_pointSize(1),_lineWidth(1),
   _rs(DEFAULT_R),_gs(DEFAULT_G),_bs(DEFAULT_B),_s(DEFAULT_S),
   _ra(0),_ga(0),_ba(0),
   _r(DEFAULT_R),_g(DEFAULT_G),_b(DEFAULT_B),_dirty(true) {
  _vertices=vertices;
  _indices=indices;
  _bb=resetBB();
  for(int i=0; i<(int)vertices.size(); i+=3) {
    Eigen::Matrix<GLfloat,3,1> v=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&vertices[i]);
    _bb=unionBB(_bb,v);
  }
  setMode(mode);
  if(!_texWhite)
    _texWhite=getWhiteTexture();
  _tex=_texWhite;
}
void MeshShape::addIndexSingle(int i) {
  _indices.push_back(i);
}
void MeshShape::setMode(GLenum mode) {
  _mode=mode;
}
int MeshShape::nrVertex() const {
  return (int)_vertices.size()/3;
}
void MeshShape::clear() {
  _vertices.clear();
  _normals.clear();
  _texcoords.clear();
  _indices.clear();
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
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> MeshShape::getNormal(int i) {
  if((int)_normals.size()<i*3+3)
    _normals.resize(i*3+3,0);
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(&_normals[i*3]);
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> MeshShape::getVertex(int i) {
  _dirty=true;
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(&_vertices[i*3]);
}
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> MeshShape::getVertex(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_vertices[i*3]);
}
void MeshShape::setPointSize(GLfloat pointSize) {
  _pointSize=pointSize;
}
void MeshShape::setLineWidth(GLfloat lineWidth) {
  _lineWidth=lineWidth;
}
void MeshShape::setColor(GLenum mode,GLfloat R,GLfloat G,GLfloat B) {
  if(_mode!=mode)
    return;
  _r=R;
  _g=G;
  _b=B;
}
void MeshShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) {
  if(_mode!=mode)
    return;
  _ra=RA;
  _ga=GA;
  _ba=BA;
}
void MeshShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) {
  if(_mode!=mode)
    return;
  _rs=RS;
  _gs=GS;
  _bs=BS;
}
void MeshShape::setShininess(GLenum mode,GLfloat S) {
  if(_mode!=mode)
    return;
  _s=S;
}
void MeshShape::setTexture(std::shared_ptr<Texture> tex) {
  if(tex)
    _tex=tex;
  else _tex=getWhiteTexture();
}
void MeshShape::draw(bool shadowPass) const {
  if(_vertices.empty() || !enabled() || (!_castShadow && shadowPass))
    return;
  if(_dirty) {
    const_cast<MeshShape*>(this)->refitBB();
    const_cast<MeshShape*>(this)->_dirty=false;
  }
  glPointSize(_pointSize);
  glLineWidth(_lineWidth);
  if(!shadowPass) {
    glColor3f(_r,_g,_b);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,_s);
    GLfloat diffuse[4]= {_r,_g,_b,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
    GLfloat ambient[4]= {_ra,_ga,_ba,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient);
    GLfloat specular[4]= {_rs,_gs,_bs,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
  }
  //vertex
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,&_vertices[0]);
  //normal
  if(_normals.size()>0 && !shadowPass) {
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,0,&_normals[0]);
  }
  //texture
  if(_texcoords.empty())
    const_cast<std::vector<GLfloat>&>(_texcoords).assign(_vertices.size()*2/3,0.5f);
  if(_texcoords.size()>0 && !shadowPass) {
    ASSERT_MSG(_texcoords.size()==_vertices.size()*2/3,"Incorrect texture coordinate vector size!")
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT,0,&_texcoords[0]);
    if(_tex) {
      glActiveTexture(GL_TEXTURE0);
      _tex->begin();
      glActiveTexture(GL_TEXTURE1);
    }
  }
  //draw
  glDrawElements(_mode,(GLsizei)_indices.size(),GL_UNSIGNED_INT,&_indices[0]);
  //vertex
  glDisableClientState(GL_VERTEX_ARRAY);
  //normal
  if(_normals.size()>0 && !shadowPass)
    glDisableClientState(GL_NORMAL_ARRAY);
  //texture
  if(_texcoords.size()>0 && !shadowPass) {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if(_tex) {
      _tex->end();
      glActiveTexture(GL_TEXTURE0);
    }
  }
}
Eigen::Matrix<GLfloat,6,1> MeshShape::getBB() const {
  if(_dirty) {
    const_cast<MeshShape*>(this)->refitBB();
    const_cast<MeshShape*>(this)->_dirty=false;
  }
  return _bb;
}
void MeshShape::refitBB() {
  _bb=resetBB();
  for(int i=0; i<nrVertex(); i++)
    _bb=unionBB(_bb,(Eigen::Matrix<GLfloat,3,1>)getVertex(i));
}
std::shared_ptr<Texture> MeshShape::_texWhite;
}
