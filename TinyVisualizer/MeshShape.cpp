#include "MeshShape.h"
#include "Texture.h"
#include "Matrix.h"
#include "Povray.h"
#include "MakeTexture.h"
#include "DefaultLight.h"
#include <fstream>
#include <iostream>

namespace DRAWER {
MeshShape::BoneData::BoneData():_maxNrBone(0) {}
bool MeshShape::BoneData::empty() const {
  return _maxNrBone==0;
}
GLfloat MeshShape::BoneData::findWeight(int vertexId,int boneId) const {
  for(int d=0; d<(int)_maxNrBone; d++)
    if(_boneId[vertexId*_maxNrBone+d]==boneId)
      return _boneWeight[vertexId*_maxNrBone+d];
  ASSERT_MSGV(false,"Cannot find bone weight for vertexId=%d and boneId=%d!",vertexId,boneId)
  return 0.0f;
}
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
MeshShape::~MeshShape() {
  _mat->_texDiffuse=NULL;
  _mat->_texSpecular=NULL;
}
void MeshShape::addVertex(const Eigen::Matrix<GLfloat,-1,1>& v,const Eigen::Matrix<GLfloat,-1,1>* tc) {
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
  _VBO=NULL;
  _dirty=true;
}
void MeshShape::addIndex(const Eigen::Matrix<GLuint,-1,1>& I) {
  for(int i=0; i<I.size(); i++)
    _indices.push_back(I[i]);
  _VBO=NULL;
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
void MeshShape::clearIndex() {
  _indices.clear();
  _VBO=NULL;
}
void MeshShape::clearTexWhite() {
    _texWhite = NULL;
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
  _dirty=true;
  if((int)_normals.size()<i*3+3) {
    _normals.resize(i*3+3,0);
    _VBO=NULL;
  }
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_normals.data()+i*3)=normal;
  if(_VBO)
    _VBO->setVertexNormal(i,normal);
  else _VBO=NULL;
}
Eigen::Matrix<GLfloat,3,1> MeshShape::getNormal(int i) const {
  ASSERT_MSGV((int)_normals.size()>=i*3+3,"Normal index %d does not exist!",i)
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_normals[i*3]);
}
void MeshShape::setVertex(int i,const Eigen::Matrix<GLfloat,3,1>& vertex) {
  _dirty=true;
  if((int)_vertices.size()<i*3+3) {
    _vertices.resize(i*3+3,0);
    _VBO=NULL;
  }
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_vertices.data()+i*3)=vertex;
  if(_VBO)
    _VBO->setVertexPosition(i,vertex);
  else _VBO=NULL;
}
void MeshShape::setVertices(const std::vector<GLfloat>& vertices) {
  _dirty=true;
  ASSERT_MSG(vertices.size()==_vertices.size(),"Vertices array size mismatch!")
  _vertices=vertices;
  if(_VBO)
    _VBO->setVertexPosition(vertices);
  else _VBO=NULL;
}
Eigen::Matrix<GLfloat,3,1> MeshShape::getVertex(int i) const {
  ASSERT_MSGV((int)_vertices.size()>=i*3+3,"Vertex index %d does not exist!",i)
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(&_vertices[i*3]);
}
void MeshShape::setTexcoord(int i,const Eigen::Matrix<GLfloat,2,1>& texcoord) {
  _dirty=true;
  if((int)_texcoords.size()<i*2+2) {
    _texcoords.resize(i*2+2,0);
    _VBO=NULL;
  }
  Eigen::Map<Eigen::Matrix<GLfloat,2,1>>(_texcoords.data()+i*2)=texcoord;
  if(_VBO)
    _VBO->setVertexTexCoord(i,texcoord);
  else _VBO=NULL;
}
Eigen::Matrix<GLfloat,2,1> MeshShape::getTexcoord(int i) const {
  ASSERT_MSGV((int)_texcoords.size()>=i*2+2,"Texcoord index %d does not exist!",i)
  return Eigen::Map<const Eigen::Matrix<GLfloat,2,1>>(&_texcoords[i*2]);
}
GLuint MeshShape::getIndex(int i) const {
  ASSERT_MSGV((int)_indices.size()>i,"Index %d does not exist!",i)
  return _indices[i];
}
void MeshShape::debugWriteObj(const std::string& path) {
  ASSERT_MSG(path.size()>4,"obj file path.size()<=4!");
  ASSERT_MSG(path.substr(path.size()-4)==".obj" ||
             path.substr(path.size()-4)==".OBJ",
             "obj file path does not end with .obj or .OBJ");
  std::string pathMtl=path.substr(0,path.size()-4)+".mtl";
  {
    std::ofstream os(pathMtl);
    os<<"Ka "<<_mat->_ambient[0]<<" "<<_mat->_ambient[1]<<" "<<_mat->_ambient[2]<<std::endl;
    os<<"Kd "<<_mat->_diffuse[0]<<" "<<_mat->_diffuse[1]<<" "<<_mat->_diffuse[2]<<std::endl;
    os<<"Ks "<<_mat->_specular[0]<<" "<<_mat->_specular[1]<<" "<<_mat->_specular[2]<<std::endl;
    if(_mat->_texDiffuse) {
      std::string pathTex=path.substr(0,path.size()-4)+"Kd.png";
      os<<"map_Kd "<<pathTex<<std::endl;
      _mat->_texDiffuse->save(pathTex);
    }
    if(_mat->_texSpecular) {
      std::string pathTex=path.substr(0,path.size()-4)+"Ks.png";
      os<<"map_Ks "<<pathTex<<std::endl;
      _mat->_texSpecular->save(pathTex);
    }
  }
  {
    std::ofstream os(path);
    os<<"mtllib "<<pathMtl<<std::endl;
    //vertex
    for(int i=0; i<nrVertex(); i++) {
      auto v=getVertex(i);
      os<<"v "<<v[0]<<" "<<v[1]<<" "<<v[2]<<std::endl;
    }
    if((int)_texcoords.size()==nrVertex()*2)
      for(int i=0; i<nrVertex(); i++) {
        auto v=getTexcoord(i);
        os<<"vt "<<v[0]<<" "<<v[1]<<std::endl;
      }
    if((int)_normals.size()==nrVertex()*3)
      for(int i=0; i<nrVertex(); i++) {
        auto v=getNormal(i);
        os<<"vn "<<v[0]<<" "<<v[1]<<" "<<v[2]<<std::endl;
      }
    //index
    if(_mode==GL_TRIANGLES)
      for(int i=0; i<nrIndex(); i+=3) {
        os<<"f ";
        for(int d=0; d<3; d++)
          os<<(getIndex(i+d)+1)<<"/"<<(getIndex(i+d)+1)<<"/"<<(getIndex(i+d)+1)<<" ";
        os<<std::endl;
      }
    if(_mode==GL_LINES)
      for(int i=0; i<nrIndex(); i+=2) {
        os<<"l ";
        for(int d=0; d<2; d++)
          os<<(getIndex(i+d)+1)<<" ";
        os<<std::endl;
      }
  }
}
std::shared_ptr<Texture> MeshShape::getTextureDiffuse() const {
  return _mat->_texDiffuse;
}
std::shared_ptr<Texture> MeshShape::getTextureSpecular() const {
  return _mat->_texSpecular;
}
std::shared_ptr<Material> MeshShape::getMaterial() const {
  return _mat;
}
void MeshShape::setMaterial(std::shared_ptr<Material> mat) {
  _mat=mat;
}
MeshShape::BoneData& MeshShape::getBoneData() {
  _dirty=true;
  initVBO();
  return _bone;
}
const MeshShape::BoneData& MeshShape::getBoneData() const {
  return _bone;
}
void MeshShape::setBoneData(const BoneData& bone) {
  _dirty=true;
  _bone=bone;
  _VBO=NULL;
}
std::shared_ptr<VBO> MeshShape::getVBO() {
  _dirty=true;
  initVBO();
  return _VBO;
}
void MeshShape::setAlpha(GLfloat alpha) {
  _mat->_diffuse[3]=alpha;
}
void MeshShape::setPointSize(GLfloat pointSize) {
  _mat->_pointSize=pointSize;
}
void MeshShape::setLineWidth(GLfloat lineWidth) {
  _mat->_lineWidth=lineWidth;
}
void MeshShape::setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B) {
  if(_mode!=mode)
    return;
  _mat->_diffuse.template segment<3>(0)=Eigen::Matrix<GLfloat,3,1>(R,G,B);
}
void MeshShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA) {
  if(_mode!=mode)
    return;
  _mat->_ambient=Eigen::Matrix<GLfloat,4,1>(RA,GA,BA,1);
}
void MeshShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS) {
  if(_mode!=mode)
    return;
  _mat->_specular=Eigen::Matrix<GLfloat,4,1>(RS,GS,BS,1);
}
void MeshShape::setShininess(GLenum mode,GLfloat S) {
  if(_mode!=mode)
    return;
  _mat->_shininess=S;
}
void MeshShape::setTextureDiffuse(std::shared_ptr<Texture> tex) {
  _mat->_texDiffuse=tex;
}
void MeshShape::setTextureSpecular(std::shared_ptr<Texture> tex) {
  _mat->_texSpecular=tex;
}
void MeshShape::setDepth(GLfloat depth) {
  for(int i=0; i<(int)_vertices.size(); i+=3)
    _vertices[i+2]=depth;
}
void MeshShape::setDrawer(Drawer* drawer) {
  _mat->_drawer=drawer;
}
void MeshShape::draw(PASS_TYPE passType) const {
  if(passType&MESH_PASS)
    if(_mode!=GL_TRIANGLES&&_mode!=GL_TRIANGLE_FAN&&_mode!=GL_TRIANGLE_STRIP)
      return;
  if(passType&LINE_PASS)
    if(_mode!=GL_LINES&&_mode!=GL_LINE_LOOP&&_mode!=GL_LINE_STRIP)
      return;
  if(passType&POINT_PASS)
    if(_mode!=GL_POINTS)
      return;
  if(!_castShadow && (passType&SHADOW_PASS)!=0)
    return;
  if(_vertices.empty() || !enabled())
    return;
  if(_dirty) {
    const_cast<MeshShape*>(this)->refitBB();
    const_cast<MeshShape*>(this)->_dirty=false;
  }
  //mode begin
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
  const_cast<MeshShape*>(this)->initVBO();
  //turn on material
  if((passType&SHADOW_PASS)==0)
    setupMaterial(*_mat);
  setupMatrixInShader();
  //turn on texture
  glActiveTexture(GL_TEXTURE0);
  (_mat->_texDiffuse?_mat->_texDiffuse:_texWhite)->begin();
  glActiveTexture(GL_TEXTURE1);
  (_mat->_texSpecular?_mat->_texSpecular:_texWhite)->begin();
  glActiveTexture(GL_TEXTURE2);
  //draw
  _VBO->draw(_mode);
  //turn off texture
  glActiveTexture(GL_TEXTURE0);
  (_mat->_texDiffuse?_mat->_texDiffuse:_texWhite)->end();
  glActiveTexture(GL_TEXTURE1);
  (_mat->_texSpecular?_mat->_texSpecular:_texWhite)->end();
  glActiveTexture(GL_TEXTURE0);
}
void MeshShape::drawPovray(Povray& pov) const {
  Eigen::Matrix<GLfloat,4,4> mv;
  getFloatv(GLModelViewMatrix,mv);
  if(_mode==GL_TRIANGLES) {
    std::shared_ptr<Povray::Mesh> m(new Povray::Mesh);
    m->_vertices=_vertices;
    m->_normals=_normals;
    m->_texcoords=_texcoords;
    m->_indices=_indices;
    m->_trans=mv.template block<3,4>(0,0);
    m->_mat=*_mat;
    pov.addElement(m);
  } else if(_mode==GL_POINTS) {
    std::shared_ptr<Povray::Points> p(new Povray::Points);
    p->_vertices=_vertices;
    p->_indices=_indices;
    p->_trans=mv.template block<3,4>(0,0);
    p->_mat=*_mat;
    pov.addElement(p);
  } else if(_mode==GL_LINES || _mode==GL_LINE_STRIP) {
    std::shared_ptr<Povray::Lines> l(new Povray::Lines);
    l->_vertices=_vertices;
    l->_indices=_indices;
    l->_trans=mv.template block<3,4>(0,0);
    l->_isStrip=_mode==GL_LINE_STRIP;
    l->_mat=*_mat;
    pov.addElement(l);
  } else {
    ASSERT_MSG(false,"Unsupported GL_MODE for povray!")
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
void MeshShape::initVBO() {
  if(!_VBO) {
    if(glad_glGenVertexArrays) {    //OpenGL initialized
      const_cast<MeshShape*>(this)->_VBO.reset(new VBO(nrVertex(),nrIndex(),true,true,true,false,!_bone.empty(),!_bone.empty()));
    } else {
      const_cast<MeshShape*>(this)->_VBO=NULL;
      return;
    }
    _VBO->setVertexPosition(_vertices);
    if(!_normals.empty())
      _VBO->setVertexNormal(_normals);
    if(!_texcoords.empty())
      _VBO->setVertexTexCoord(_texcoords);
    if(!_bone.empty()) {
      ASSERT_MSG(_bone._maxNrBone==4,"Our system only support skinned mesh with 4 bones!")
      _VBO->setVertexBoneId(_bone._boneId);
      _VBO->setVertexBoneWeight(_bone._boneWeight);
    }
    _VBO->setIndex(_indices);
  }
}
void MeshShape::refitBB() {
  _bb=resetBB();
  for(int i=0; i<nrVertex(); i++)
    _bb=unionBB(_bb,(Eigen::Matrix<GLfloat,3,1>)getVertex(i));
}
void MeshShape::initMaterial() {
  _mat.reset(new Material);
  _mat->_ambient=Eigen::Matrix<GLfloat,4,1>(0,0,0,1);
  _mat->_diffuse=Eigen::Matrix<GLfloat,4,1>((GLfloat)DEFAULT_R,(GLfloat)DEFAULT_G,(GLfloat)DEFAULT_B,(GLfloat)1);
  _mat->_specular=Eigen::Matrix<GLfloat,4,1>(0,0,0,1);
  _mat->_shininess=DEFAULT_S;
  _mat->_pointSize=1;
  _mat->_lineWidth=0;
  _mat->_texDiffuse=NULL;
  _mat->_texSpecular=NULL;
  _mat->_drawer=NULL;
  if(!glad_glGenTextures)
    return; //this is CPU only, just return
  if(!_texWhite)
    _texWhite=getWhiteTexture();
}
std::shared_ptr<Texture> MeshShape::_texWhite;
}
