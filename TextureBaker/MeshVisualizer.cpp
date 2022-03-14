#include "MeshVisualizer.h"
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/MeshShape.h>
#include <experimental/filesystem>

namespace DRAWER {
MeshVisualizer::MeshVisualizer(const std::string& path,const Eigen::Matrix<GLfloat,3,1>& diffuse):_diffuse(diffuse) {
  tinyobj::ObjReader reader;
  reader.ParseFromFile(path);
  for(const tinyobj::shape_t& shape:reader.GetShapes()) { //shape
    const tinyobj::mesh_t& mesh=shape.mesh;
    for(GLuint fid=0,voff=0; fid<mesh.num_face_vertices.size(); fid++) { //mesh
      GLuint nV=mesh.num_face_vertices[fid];
      MeshComponent& component=_components[mesh.material_ids[fid]];
      //appearance
      if(mesh.material_ids[fid]==-1)
        initializeComponent(component);
      else initializeComponent(path,component,reader.GetMaterials()[mesh.material_ids[fid]]);
      //geometry
      for(GLuint i=1; i<(GLuint)nV-1; i++) { //face
        for(GLuint vid: std::vector<GLuint>({0,i,i+1})) {  //triangle
          const tinyobj::index_t& a=mesh.indices[voff+vid];
          Eigen::Matrix<GLfloat,3,1> v(reader.GetAttrib().vertices[a.vertex_index*3+0],
                                       reader.GetAttrib().vertices[a.vertex_index*3+1],
                                       reader.GetAttrib().vertices[a.vertex_index*3+2]);
          Eigen::Matrix<GLfloat,3,1> n(reader.GetAttrib().normals[a.normal_index*3+0],
                                       reader.GetAttrib().normals[a.normal_index*3+1],
                                       reader.GetAttrib().normals[a.normal_index*3+2]);
          Eigen::Matrix<GLfloat,2,1> t(reader.GetAttrib().texcoords[a.texcoord_index*2+0],
                                       reader.GetAttrib().texcoords[a.texcoord_index*2+1]);
          component._mesh->addVertex(v,&t);
          component._mesh->setNormal(component._mesh->nrVertex()-1,n);
        }
        Eigen::Matrix<GLuint,3,1> I(component._mesh->nrVertex()-3,
                                    component._mesh->nrVertex()-2,
                                    component._mesh->nrVertex()-1);
        component._mesh->addIndex(I);
      }
      voff+=nV;
    }
  }
  //scale
  Eigen::Matrix<GLfloat,6,1> bb=getBB();
  GLfloat coef=1/(bb.segment<3>(3)-bb.segment<3>(0)).maxCoeff();
  for(auto& component:_components) {
    for(int i=0; i<component.second._mesh->nrVertex(); i++) {
      Eigen::Matrix<GLfloat,3,1> v=component.second._mesh->getVertex(i);
      component.second._mesh->setVertex(i,v*coef);
    }
  }
}
const std::unordered_map<int,MeshVisualizer::MeshComponent>& MeshVisualizer::getComponents() const {
  return _components;
}
std::shared_ptr<CompositeShape> MeshVisualizer::getTextureCoordShape() const {
  std::shared_ptr<CompositeShape> ret(new CompositeShape);
  for(auto& component:_components) {
    std::shared_ptr<MeshShape> mesh=component.second._mesh;
    std::shared_ptr<MeshShape> meshTC(new MeshShape);
    meshTC->setMode(GL_TRIANGLES);
    meshTC->setTexture(component.second._texture);
    meshTC->setMaterial(mesh->getMaterial());
    for(int i=0; i<mesh->nrVertex(); i++) {
      Eigen::Matrix<GLfloat,2,1> tc=mesh->getTexcoord(i);
      meshTC->addVertex(Eigen::Matrix<GLfloat,3,1>(tc[0],tc[1],0),&tc);
      meshTC->setNormal(i,Eigen::Matrix<GLfloat,3,1>(0,0,1));
    }
    for(int i=0; i<mesh->nrIndex(); i++)
      meshTC->addIndexSingle(mesh->getIndex(i));
    ret->addShape(meshTC);
  }
  return ret;
}
std::shared_ptr<CompositeShape> MeshVisualizer::getShape() const {
  std::shared_ptr<CompositeShape> ret(new CompositeShape);
  for(auto comp:_components)
    ret->addShape(comp.second._mesh);
  return ret;
}
Eigen::Matrix<GLfloat,6,1> MeshVisualizer::getBB() const {
  Eigen::Matrix<GLfloat,6,1> bb=resetBB();
  for(auto& component:_components)
    bb=unionBB(bb,component.second._mesh->getBB());
  return bb;
}
void MeshVisualizer::saveAllTexture(const std::string& path) {
  std::experimental::filesystem::create_directory(path);
  for(auto& component:_components)
    if(component.second._texture)
      component.second._texture->save(path+"/"+std::to_string(component.first)+".png");
}
void MeshVisualizer::setTexture(std::shared_ptr<Texture> texture,bool rescale,GLfloat margin) {
  ASSERT_MSG(_components.size()==1,"setTexture can only be called for mesh with a single component!")
  MeshComponent& component=_components.begin()->second;
  component._mesh->setTexture(texture);
  component._texture=texture;

  if(rescale) {
    Eigen::Matrix<GLfloat,2,1> tcmin;
    Eigen::Matrix<GLfloat,2,1> tcmax;
    tcmin.setConstant( std::numeric_limits<GLfloat>::max());
    tcmax.setConstant(-std::numeric_limits<GLfloat>::max());
    for(int i=0; i<component._mesh->nrVertex(); i++) {
      tcmin=tcmin.cwiseMin(component._mesh->getTexcoord(i));
      tcmax=tcmax.cwiseMax(component._mesh->getTexcoord(i));
    }
    GLfloat scale=(1-margin*2)/(tcmax-tcmin).maxCoeff();
    Eigen::Matrix<GLfloat,2,1> translation=Eigen::Matrix<GLfloat,2,1>::Constant(margin)-tcmin*scale;
    for(int i=0; i<component._mesh->nrVertex(); i++) {
      auto tc=component._mesh->getTexcoord(i);
      component._mesh->setTexcoord(i,tc*scale+translation);
    }
  }
}
//helper
std::string MeshVisualizer::replaceTexturePath(std::string path) const {
  bool more=true;
  while(more) {
    more=false;
    for(size_t i=0; i<path.length()-1; i++)
      if(path[i]=='\\' && path[i+1]=='\\') {
        path.erase(path.begin()+i);
        more=true;
        break;
      }
  }
  for(size_t i=0; i<path.length(); i++)
    if(path[i]=='\\')
      path[i]='/';
  return path;
}
void MeshVisualizer::initializeComponent(const std::string& path,MeshComponent& component,const tinyobj::material_t& material) {
  if(!component._mesh) {
    component._mesh.reset(new MeshShape);
    component._mesh->setMode(GL_TRIANGLES);
    component._mesh->setColor(GL_TRIANGLES,material.diffuse[0],material.diffuse[1],material.diffuse[2]);
    component._defaultColor=Eigen::Matrix<GLfloat,4,1>(material.diffuse[0],material.diffuse[1],material.diffuse[2],1);
  }
  if(!component._texture) {
    std::experimental::filesystem::path p(path);
    p=p.parent_path()/replaceTexturePath(material.diffuse_texname);
    ASSERT_MSGV(std::experimental::filesystem::exists(p),"File %s does not exist!",p.c_str());
    component._texture=Texture::load(p.string());
    component._mesh->setTexture(component._texture);
  }
}
void MeshVisualizer::initializeComponent(MeshComponent& component) {
  if(!component._mesh) {
    component._mesh.reset(new MeshShape);
    component._mesh->setMode(GL_TRIANGLES);
    component._mesh->setColor(GL_TRIANGLES,_diffuse[0],_diffuse[1],_diffuse[2]);
    component._defaultColor=Eigen::Matrix<GLfloat,4,1>(_diffuse[0],_diffuse[1],_diffuse[2],1);
  }
}
}
