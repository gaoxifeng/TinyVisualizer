#ifndef MESH_VISUALIZER_H
#define MESH_VISUALIZER_H

#include <tinyobjloader/tiny_obj_loader.h>
#include <TinyVisualizer/DrawerUtility.h>
#include <unordered_map>
#include <memory>

namespace DRAWER {

class Texture;
class MeshShape;
class CompositeShape;
class MeshVisualizer {
 public:
  struct MeshComponent {
    std::shared_ptr<MeshShape> _mesh;
    std::shared_ptr<Texture> _texture;
    Eigen::Matrix<GLfloat,4,1> _defaultColor;
  };
  MeshVisualizer(const std::string& path,const Eigen::Matrix<GLfloat,3,1>& diffuse=Eigen::Matrix<GLfloat,3,1>::Constant(0.5));
  const std::unordered_map<int,MeshComponent>& getComponents() const;
  std::shared_ptr<CompositeShape> getTextureCoordShape() const;
  std::shared_ptr<CompositeShape> getShape() const;
  Eigen::Matrix<GLfloat,6,1> getBB() const;
  void saveAllTexture(const std::string& path);
  void setTexture(std::shared_ptr<Texture> texture,bool rescale,GLfloat margin);
 protected:
  std::string replaceTexturePath(std::string path) const;
  void initializeComponent(const std::string& path,MeshComponent& component,const tinyobj::material_t& material);
  void initializeComponent(MeshComponent& component);
  std::unordered_map<int,MeshComponent> _components;
  Eigen::Matrix<GLfloat,3,1> _diffuse;
};

}

#endif
