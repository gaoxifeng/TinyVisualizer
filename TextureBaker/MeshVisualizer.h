#ifndef MESH_VISUALIZER_H
#define MESH_VISUALIZER_H

#include <tinyobjloader/tiny_obj_loader.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/MeshShape.h>
#include <unordered_map>

namespace DRAWER {

struct MeshComponent {
  std::shared_ptr<MeshShape> _mesh;
  std::shared_ptr<Texture> _texture;
};

class MeshVisualizer {
 public:
  MeshVisualizer(const std::string& path,const Eigen::Matrix<GLfloat,3,1>& diffuse=Eigen::Matrix<GLfloat,3,1>::Constant(0.5));
  std::string replaceTexturePath(std::string path) const;
  std::shared_ptr<CompositeShape> getShape() const;
 protected:
  void initializeComponent(const std::string& path,MeshComponent& component,const tinyobj::material_t& material);
  void initializeComponent(MeshComponent& component);
  std::unordered_map<GLuint,MeshComponent> _components;
  Eigen::Matrix<GLfloat,3,1> _diffuse;
};

}

#endif
