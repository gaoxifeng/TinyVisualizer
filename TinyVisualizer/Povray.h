#ifndef POVRAY_H
#define POVRAY_H

#include "DrawerUtility.h"
#include "ShadowAndLight.h"
#include "Texture.h"
#include <fstream>

namespace DRAWER {
class EXPORT Povray {
 public:
  struct Element : public RTTI::Enable {
    RTTI_DECLARE_TYPEINFO(Element);
   public:
    virtual ~Element() {}
    virtual void write(Povray& pov) const=0;
  };
  struct Camera : public Element {
    RTTI_DECLARE_TYPEINFO(Camera);
   public:
    void write(Povray& pov) const override;
    bool _is2D=true;
    Eigen::Matrix<GLfloat,3,1> _pos,_dir,_up,_right;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
  };
  struct Background : public Element {
    RTTI_DECLARE_TYPEINFO(Background);
   public:
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,1> _color;
    Eigen::Matrix<GLfloat,2,1> _tcMult;
    std::shared_ptr<Texture> _tex;
  };
  struct ElementWithVertices {
   public:
    std::vector<GLfloat> _vertices;
  };
  struct Mesh : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Mesh);
   public:
    void write(Povray& pov) const override;
    static void write(Povray& pov,const Material& mat);
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLfloat> _normals,_texcoords;
    std::vector<GLuint> _indices;
    Material _mat;
  };
  struct Points : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Points);
   public:
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLuint> _indices;
    GLuint _res=100;
    Material _mat;
  };
  struct Lines : public Element, public ElementWithVertices {
    RTTI_DECLARE_TYPEINFO(Lines);
   public:
    void write(Povray& pov) const override;
    Eigen::Matrix<GLfloat,3,4> _trans=Eigen::Matrix<GLfloat,3,4>::Identity();
    std::vector<GLuint> _indices;
    bool _isStrip=false;
    GLuint _res=100;
    Material _mat;
  };
  struct LightSource : public Element {
    RTTI_DECLARE_TYPEINFO(LightSource);
   public:
    void write(Povray& pov) const override;
    static const int _nSample=10;
    Eigen::Matrix<GLfloat,3,1> _ctr;
    GLfloat _size;
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
  std::string write(std::shared_ptr<Texture> t,int quality=100,const Eigen::Matrix<GLfloat,4,1>* diffuse=NULL);
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
