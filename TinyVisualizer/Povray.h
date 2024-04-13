#ifndef POVRAY_H
#define POVRAY_H

#include "DrawerUtility.h"
#include "ShadowAndLight.h"
#include "Texture.h"
#include <fstream>

namespace DRAWER {
class Povray {
 public:
  struct Element : public RTTI::Enable {
    virtual void write(Povray& pov) const=0;
  };
  struct Camera : public Element {
    RTTI_DECLARE_TYPEINFO(Camera);
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,1> _pos,_dir,_up,_right;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
  };
  struct Background : public Element {
    RTTI_DECLARE_TYPEINFO(Background);
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,1> _color;
    Eigen::Matrix<GLfloat,2,1> _tcMult;
    std::shared_ptr<Texture> _tex;
  };
  struct ElementWithVertices {
    std::vector<GLfloat> _vertices;
  };
  struct Mesh : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Mesh);
    void write(Povray& pov) const override;
    static void write(Povray& pov,const Material& mat);
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLfloat> _normals,_texcoords;
    std::vector<GLuint> _indices;
    Material _mat;
  };
  struct Points : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Points);
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLuint> _indices;
    GLuint _res=100;
    Material _mat;
  };
  struct Lines : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Lines);
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLuint> _indices;
    bool _isStrip=false;
    GLuint _res=100;
    Material _mat;
  };
  struct LightSource : public Element {
    RTTI_DECLARE_TYPEINFO(LightSource);
    void write(Povray& pov) const override;
    Light _l;
  };
  Povray(const std::string& folder);
  ~Povray();
  std::shared_ptr<Element> getLastElement();
  void addElement(std::shared_ptr<Element> e);
  void writeTrans(const Eigen::Matrix<GLfloat,3,4>& m);
  std::string write(const Eigen::Matrix<GLfloat,3,1>& v) const;
  std::string write(const Eigen::Matrix<GLfloat,2,1>& v) const;
  std::string write(const Eigen::Matrix<GLuint,3,1>& v) const;
  std::string write(std::shared_ptr<Texture> t);
  template <typename T>
  std::shared_ptr<T> getElement() const;
  std::ofstream& getStream();
  std::string indent() const;
  void moreIndent();
  void lessIndent();
 private:
  std::vector<std::shared_ptr<Element>> _elements;
  std::unordered_map<std::shared_ptr<Texture>,int> _textures;
  std::string _folder;
  std::ofstream _os;
  int _indents;
};
}

#endif
