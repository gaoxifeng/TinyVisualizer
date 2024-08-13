#include "Povray.h"
#include <filesystem>

namespace DRAWER {
void Povray::Camera::write(Povray& pov) const {
  pov.getStream() << pov.indent() << "camera {" << std::endl;
  pov.moreIndent();
  pov.getStream() << pov.indent() << "location " << pov.write(_pos) << std::endl;
  pov.getStream() << pov.indent() << "direction " << pov.write(_dir) << std::endl;
  pov.getStream() << pov.indent() << "right " << pov.write(_right) << std::endl;
  pov.getStream() << pov.indent() << "up " << pov.write(_up) << std::endl;
  pov.getStream() << pov.indent() << "sky " << pov.write(_up.normalized().eval()) << std::endl;
  if(!_trans.isIdentity())
    pov.writeTrans(_trans);
  pov.lessIndent();
  pov.getStream() << pov.indent() << "}" << std::endl;
}
void Povray::Background::write(Povray& pov) const {
  Mesh m;
  std::shared_ptr<Camera> c=pov.getElement<Camera>();
  Eigen::Matrix<GLfloat,3,1> v[4];
  Eigen::Matrix<GLfloat,2,1> tc[4];
  v[0]=c->_pos+c->_dir+(-c->_up-c->_right)/2;
  v[1]=c->_pos+c->_dir+(-c->_up+c->_right)/2;
  v[2]=c->_pos+c->_dir+( c->_up+c->_right)/2;
  v[3]=c->_pos+c->_dir+( c->_up-c->_right)/2;
  for(int i=0; i<4; i++)
    v[i]=c->_trans.template block<3,3>(0,0)*v[i]+c->_trans.col(3);
  tc[0] << 0,0;
  tc[1] << 1,0;
  tc[2] << 1,1;
  tc[3] << 0,1;
  for(int i=0; i<4; i++) {
    for(int j=0; j<3; j++)
      m._vertices.push_back(v[i][j]);
    for(int j=0; j<2; j++)
      m._texcoords.push_back(tc[i][j]*_tcMult[j]);
  }
  m._indices=std::vector<GLuint>({0,1,2,0,2,3});
  m._mat._ambient.setOnes();
  m._mat._diffuse << _color[0],_color[1],_color[2],1;
  m._mat._specular.setZero();
  m._mat._texDiffuse=_tex;
  m._mat._shininess=0;
  m.write(pov);
}
void Povray::Mesh::write(Povray& pov) const {
  pov.getStream() << pov.indent() << "mesh2 {" << std::endl;
  pov.moreIndent();
  {
    //vertex_vectors
    pov.getStream() << pov.indent() << "vertex_vectors {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << _vertices.size()/3 << "," << std::endl;
    for(size_t i=0; i<_vertices.size(); i+=3)
      pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,3,1>(_vertices[i+0],_vertices[i+1],_vertices[i+2])) << (i+3<_vertices.size()?",":"") << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
  }
  //normal_vectors
  if(!_normals.empty()) {
    pov.getStream() << pov.indent() << "normal_vectors {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << _normals.size()/3 << "," << std::endl;
    for(size_t i=0; i<_normals.size(); i+=3)
      pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,3,1>(_normals[i+0],_normals[i+1],_normals[i+2])) << (i+3<_normals.size()?",":"") << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
  }
  //uv_vectors
  if(!_texcoords.empty()) {
    pov.getStream() << pov.indent() << "uv_vectors {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << _texcoords.size()/2 << "," << std::endl;
    for(size_t i=0; i<_texcoords.size(); i+=2)
      pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,2,1>(_texcoords[i+0],_texcoords[i+1])) << (i+2<_texcoords.size()?",":"") << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
  }
  //face_indices
  pov.getStream() << pov.indent() << "face_indices {" << std::endl;
  pov.moreIndent();
  pov.getStream() << pov.indent() << _indices.size()/3 << "," << std::endl;
  for(size_t i=0; i<_indices.size(); i+=3)
    pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,3,1>(_indices[i+0],_indices[i+1],_indices[i+2])) << (i+3<_indices.size()?",":"") << std::endl;
  pov.lessIndent();
  pov.getStream() << pov.indent() << "}" << std::endl;
  //normal_indices
  if(!_normals.empty()) {
    pov.getStream() << pov.indent() << "normal_indices {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << _indices.size()/3 << "," << std::endl;
    for(size_t i=0; i<_indices.size(); i+=3)
      pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,3,1>(_indices[i+0],_indices[i+1],_indices[i+2])) << (i+3<_indices.size()?",":"") << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
  }
  //uv_indices
  if(!_texcoords.empty()) {
    pov.getStream() << pov.indent() << "uv_indices {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << _indices.size()/3 << "," << std::endl;
    for(size_t i=0; i<_indices.size(); i+=3)
      pov.getStream() << pov.indent() << pov.write(Eigen::Matrix<GLfloat,3,1>(_indices[i+0],_indices[i+1],_indices[i+2])) << (i+3<_indices.size()?",":"") << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
  }
  //material
  write(pov,_mat);
  pov.lessIndent();
  if(!_trans.isIdentity())
    pov.writeTrans(_trans);
  pov.getStream() << pov.indent() << "}" << std::endl;
}
void Povray::Mesh::write(Povray& pov,const Material& mat) {
  //material
  pov.getStream() << pov.indent() << "texture {" << std::endl;
  if(mat._texDiffuse != NULL) {
    pov.moreIndent();
    pov.getStream() << pov.indent() << "uv_mapping" << std::endl;
    pov.getStream() << pov.indent() << "pigment {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << "image_map {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << "png \"" << pov.write(mat._texDiffuse,100,&(mat._diffuse))+"\"" << std::endl;
    pov.getStream() << pov.indent() << "map_type 0" << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
    pov.lessIndent();
  } else {
    pov.moreIndent();
    pov.getStream() << pov.indent() << "pigment {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << "rgb " << pov.write(mat._diffuse.template segment<3>(0).eval()) << std::endl;
    pov.getStream() << pov.indent() << "transmit " << 1-mat._diffuse[3] << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
    pov.lessIndent();
  }
  {
    pov.moreIndent();
    pov.getStream() << pov.indent() << "finish {" << std::endl;
    pov.moreIndent();
    pov.getStream() << pov.indent() << "ambient rgb " << pov.write(mat._ambient.template segment<3>(0).eval()) << std::endl;
    pov.getStream() << pov.indent() << "diffuse " << mat._diffuse.mean() << std::endl;
    pov.getStream() << pov.indent() << "phong " << mat._specular.mean() << std::endl;
    pov.getStream() << pov.indent() << "phong_size " << mat._shininess << std::endl;
    pov.lessIndent();
    pov.getStream() << pov.indent() << "}" << std::endl;
    pov.lessIndent();
  }
  pov.getStream() << pov.indent() << "}" << std::endl;
}
void Povray::Points::write(Povray& pov) const {
  pov.getStream() << pov.indent() << "union {" << std::endl;
  pov.moreIndent();
  for(size_t i=0; i<_indices.size(); i++) {
    Eigen::Matrix<GLfloat,3,1> a(_vertices[_indices[i]*3+0],_vertices[_indices[i]*3+1],_vertices[_indices[i]*3+2]);
    pov.getStream() << "sphere { " << pov.write(a) << "," << _mat._pointSize/_res << " }" << std::endl;
  }
  pov.lessIndent();
  //material
  Mesh::write(pov,_mat);
  if(!_trans.isIdentity())
    pov.writeTrans(_trans);
  pov.getStream() << pov.indent() << "}" << std::endl;
}
void Povray::Lines::write(Povray& pov) const {
  pov.getStream() << pov.indent() << "union {" << std::endl;
  pov.moreIndent();
  if(_isStrip) {
    for(size_t i=0; i<_indices.size()-1; i++) {
      Eigen::Matrix<GLfloat,3,1> a(_vertices[_indices[i+0]*3+0],_vertices[_indices[i+0]*3+1],_vertices[_indices[i+0]*3+2]);
      Eigen::Matrix<GLfloat,3,1> b(_vertices[_indices[i+1]*3+0],_vertices[_indices[i+1]*3+1],_vertices[_indices[i+1]*3+2]);
      if(a==b)
        continue;
      pov.getStream() << "sphere { " << pov.write(a) << "," << _mat._lineWidth/_res << " }" << std::endl;
      pov.getStream() << "sphere { " << pov.write(b) << "," << _mat._lineWidth/_res << " }" << std::endl;
      pov.getStream() << "cylinder { " << pov.write(a) << "," << pov.write(b) << "," << _mat._lineWidth/_res << " }" << std::endl;
    }
  } else {
    for(size_t i=0; i<_indices.size(); i+=2) {
      Eigen::Matrix<GLfloat,3,1> a(_vertices[_indices[i+0]*3+0],_vertices[_indices[i+0]*3+1],_vertices[_indices[i+0]*3+2]);
      Eigen::Matrix<GLfloat,3,1> b(_vertices[_indices[i+1]*3+0],_vertices[_indices[i+1]*3+1],_vertices[_indices[i+1]*3+2]);
      if(a==b)
        continue;
      pov.getStream() << "sphere { " << pov.write(a) << "," << _mat._lineWidth/_res << " }" << std::endl;
      pov.getStream() << "sphere { " << pov.write(b) << "," << _mat._lineWidth/_res << " }" << std::endl;
      pov.getStream() << "cylinder { " << pov.write(a) << "," << pov.write(b) << "," << _mat._lineWidth/_res << " }" << std::endl;
    }
  }
  //material
  Mesh::write(pov,_mat);
  pov.lessIndent();
  if(!_trans.isIdentity())
    pov.writeTrans(_trans);
  pov.getStream() << pov.indent() << "}" << std::endl;
}
void Povray::LightSource::write(Povray& pov) const {
  pov.getStream() << pov.indent() << "light_source {" << std::endl;
  pov.moreIndent();
  pov.getStream() << pov.indent() << pov.write(_l._position.template segment<3>(0).eval()) << std::endl;
  pov.getStream() << pov.indent() << "color rgb " << pov.write(_l._diffuse.template segment<3>(0).eval()) << std::endl;
  if(_size>0) {
    Eigen::Matrix<GLfloat,3,1> d,v1,v2;
    d=_ctr-_l._position.template segment<3>(0);
    if(d.isZero())
      return;
    int id;
    d.cwiseAbs().minCoeff(&id);
    v1=Eigen::Matrix<GLfloat,3,1>::Unit(id).cross(d).normalized()*_size;
    v2=d.cross(v1).normalized()*_size;
    pov.getStream() << pov.indent() << "area_light " << pov.write(v1) << ", " << pov.write(v2) << ", " << _nSample << ", " << _nSample << std::endl;
    pov.getStream() << pov.indent() << "adaptive 1" << std::endl;
    pov.getStream() << pov.indent() << "jitter" << std::endl;
  }
  pov.lessIndent();
  pov.getStream() << pov.indent() << "}" << std::endl;
}
Povray::Povray(const std::string& folder):_folder(folder),_indents(0) {}
Povray::~Povray() {
  //remove everything in folder
  std::filesystem::remove_all(_folder);
  //recreate folder
  std::filesystem::create_directory(_folder);
  //write elements
  _os=std::ofstream(_folder+"/scene.pov");
  for(const auto& e:_elements)
    e->write(*this);
}
std::shared_ptr<Povray::Element> Povray::getLastElement() {
  return _elements.back();
}
void Povray::addElement(std::shared_ptr<Element> e) {
  _elements.push_back(e);
}
void Povray::writeTrans(const Eigen::Matrix<GLfloat,3,4>& m) {
  getStream() << indent() << "matrix <"
              << m(0,0) << "," << m(1,0) << "," << m(2,0) << ","
              << m(0,1) << "," << m(1,1) << "," << m(2,1) << ","
              << m(0,2) << "," << m(1,2) << "," << m(2,2) << ","
              << m(0,3) << "," << m(1,3) << "," << m(2,3) << ">" << std::endl;
}
std::string Povray::write(const Eigen::Matrix<GLfloat,3,1>& v) const {
  std::ostringstream oss;
  oss << "<" << v[0] << "," << v[1] << "," << v[2] << ">";
  return oss.str();
}
std::string Povray::write(const Eigen::Matrix<GLfloat,2,1>& v) const {
  std::ostringstream oss;
  oss << "<" << v[0] << "," << v[1] << ">";
  return oss.str();
}
std::string Povray::write(const Eigen::Matrix<GLuint,3,1>& v) const {
  std::ostringstream oss;
  oss << "<" << v[0] << "," << v[1] << "," << v[2] << ">";
  return oss.str();
}
std::string Povray::write(std::shared_ptr<Texture> t,int quality,const Eigen::Matrix<GLfloat,4,1>* diffuse) {
  if(_textures.find(t)==_textures.end()) {
    int id=(int)_textures.size();
    std::string name=std::to_string(id)+".png";
    t->save(_folder+"/"+name,quality,diffuse);
    _textures[t]=id;
    return _folder+"/"+name;
  } else {
    return _folder+"/\""+std::to_string(_textures[t])+".png";
  }
}
template <typename T>
std::shared_ptr<T> Povray::getElement() const {
  for(const auto& e:_elements)
    if(std::custom_pointer_cast<T>(e))
      return std::custom_pointer_cast<T>(e);
  return NULL;
}
std::ofstream& Povray::getStream() {
  return _os;
}
std::string Povray::indent() const {
  std::ostringstream oss;
  for(int i=0; i<_indents; i++)
    oss << "  ";
  return oss.str();
}
void Povray::moreIndent() {
  _indents++;
}
void Povray::lessIndent() {
  _indents--;
}
}
