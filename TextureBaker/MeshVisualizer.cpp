#include "MeshVisualizer.h"
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
          component._mesh->addVertex(v*0.001,&t);
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
}
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
std::shared_ptr<CompositeShape> MeshVisualizer::getShape() const {
  std::shared_ptr<CompositeShape> ret(new CompositeShape);
  for(auto comp:_components)
    ret->addShape(comp.second._mesh);
  return ret;
}
void MeshVisualizer::initializeComponent(const std::string& path,MeshComponent& component,const tinyobj::material_t& material) {
  if(!component._mesh) {
    component._mesh.reset(new MeshShape);
    component._mesh->setMode(GL_TRIANGLES);
    component._mesh->setColor(GL_TRIANGLES,material.diffuse[0],material.diffuse[1],material.diffuse[2]);
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
  }
}
}
