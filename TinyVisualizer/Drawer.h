#ifndef DRAWER_H
#define DRAWER_H

#include "DrawerUtility.h"
#include <functional>
#include <memory>
#include <vector>

typedef struct GLFWwindow GLFWwindow;
namespace DRAWER {
class Texture;
class SceneNode;
class ShadowLight;
class Drawer;
class Camera2D;
class Camera3D;
//swig only support class-based callback
class PythonCallback {
 public:
  virtual ~PythonCallback() {}
  virtual void mouse(int button,int action,int mods) {}
  virtual void wheel(double xoffset,double yoffset) {}
  virtual void motion(double x,double y) {}
  virtual void key(int key,int scan,int action,int mods) {}
  virtual void frame(std::shared_ptr<SceneNode>& root) {}
  virtual void draw() {}
  virtual void setup() {}
};
//Shape for OpenGL drawing
class Shape {
 public:
  enum PASS_TYPE {
    MESH_PASS   =1<<0,
    LINE_PASS   =1<<1,
    POINT_PASS  =1<<2,
    SHADOW_PASS =1<<3,
  };
  Shape();
  virtual ~Shape() {}
  virtual void setPointSize(GLfloat) {}
  virtual void setLineWidth(GLfloat) {}
  virtual void setColor(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorAmbient(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorSpecular(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setShininess(GLenum,GLfloat) {}
  virtual void setTexture(std::shared_ptr<Texture>) {}
  virtual void setDepth(GLfloat) {}
  virtual void setDrawer(Drawer*) {}
  virtual void setEnabled(bool enabled);
  virtual void setCastShadow(bool castShadow);
  virtual void setUseLight(bool useLight);
  virtual bool needRecomputeNormal() const;
  bool enabled() const;
  bool castShadow() const;
  bool useLight() const;
  virtual void draw(PASS_TYPE passType) const=0;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const=0;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const;
 protected:
  bool _enabled;
  bool _castShadow;
  bool _useLight;
 private:
  friend class SceneNode;
  std::shared_ptr<Shape> _next;
};
//Camera
class Camera {
 public:
  virtual ~Camera() {}
  virtual void focusOn(std::shared_ptr<Shape>) {}
  virtual void frame(GLFWwindow*,GLfloat) {}
  virtual void mouse(GLFWwindow*,int,int,int,bool) {}
  virtual void wheel(GLFWwindow*,double,double,bool) {}
  virtual void motion(GLFWwindow*,double,double,bool) {}
  virtual void key(GLFWwindow*,int,int,int,int,bool) {}
  virtual void draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>&) {}
  virtual Eigen::Matrix<GLfloat,-1,1> getCameraRay(GLFWwindow*,double,double) const {
    return Eigen::Matrix<GLfloat,-1,1>();
  }
  virtual Eigen::Matrix<GLfloat,-1,1> getViewFrustum() const {
    return Eigen::Matrix<GLfloat,-1,1>();
  }
};
//Plugin
class Plugin {
 public:
  Plugin();
  virtual ~Plugin() {}
  void setDrawer(Drawer* drawer);
  virtual void init(GLFWwindow* window) {}
  virtual void finalize() {}
  virtual void preDraw() {}
  virtual void postDraw() {}
  virtual void frame(std::shared_ptr<SceneNode>&) {}
  virtual bool mouse(GLFWwindow* wnd,int button,int action,int mods) {
    return true;
  }
  virtual bool wheel(GLFWwindow* wnd,double xoffset,double yoffset) {
    return true;
  }
  virtual bool motion(GLFWwindow* wnd,double x,double y) {
    return true;
  }
  virtual bool key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
    return true;
  }
 protected:
  Drawer* _drawer;
};
//Drawer
class Drawer {
 public:
  Drawer(std::vector<std::string> args);
  Drawer(int argc,char** argv);
  virtual ~Drawer();
  void setRes(int width,int height);
  void setBackground(GLfloat r=1,GLfloat g=1,GLfloat b=1);
  void addLightSystem(int shadow=1024,int softShadow=10,bool autoAdjust=false);
  void timer();
  void draw();
  static void mouse(GLFWwindow* wnd,int button,int action,int mods);
  static void wheel(GLFWwindow* wnd,double xoffset,double yoffset);
  static void motion(GLFWwindow* wnd,double x,double y);
  static void key(GLFWwindow* wnd,int key,int scan,int action,int mods);
  void setMouseFunc(std::function<void(GLFWwindow*,int,int,int,bool)> mouse);
  void setWheelFunc(std::function<void(GLFWwindow*,double,double,bool)> wheel);
  void setMotionFunc(std::function<void(GLFWwindow*,double,double,bool)> motion);
  void setKeyFunc(std::function<void(GLFWwindow*,int,int,int,int,bool)> key);
  void setFrameFunc(std::function<void(std::shared_ptr<SceneNode>&)> frame);
  void setDrawFunc(std::function<void()> draw);
  void setPythonCallback(PythonCallback* cb);
  void addCamera2D(GLfloat xExt);
  //addCamera3D-FirstPerson/TrackBall
  void addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up=Eigen::Matrix<GLfloat,3,1>(0,0,1));
  void addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up,const Eigen::Matrix<GLfloat,3,1>& pos,const Eigen::Matrix<GLfloat,3,1>& dir);
  bool rayIntersect(Eigen::Matrix<GLfloat,6,1>& ray,std::shared_ptr<Shape>& IShape,GLfloat& IAlpha) const;
  Eigen::Matrix<GLfloat,-1,1> getCameraRay(double x,double y);
  Eigen::Matrix<GLfloat,-1,1> getCameraRay();
  std::shared_ptr<SceneNode> root();
  std::shared_ptr<ShadowLight> getLight();
  void clearLight();
  std::shared_ptr<Camera> getCamera();
  std::shared_ptr<Camera2D> getCamera2D();
  std::shared_ptr<Camera3D> getCamera3D();
  void mainLoop();
  int FPS();
  //getter/setter
  void addPlugin(std::shared_ptr<Plugin> pi);
  bool contain(std::shared_ptr<Shape> s) const;
  void removeShape(std::shared_ptr<Shape> s);
  void addShape(std::shared_ptr<Shape> s);
  void focusOn(std::shared_ptr<Shape> s);
  void clearScene();
  void clear();
 private:
  void init(int argc,char** argv);
  Drawer(const Drawer& other);
  Drawer& operator=(const Drawer& other);
  //data
  std::shared_ptr<Camera> _camera;
  std::shared_ptr<SceneNode> _root;
  std::shared_ptr<ShadowLight> _light;
  std::vector<std::shared_ptr<Plugin>> _plugins;
  std::function<void(GLFWwindow*,int,int,int,bool)> _mouse;
  std::function<void(GLFWwindow*,double,double,bool)> _wheel;
  std::function<void(GLFWwindow*,double,double,bool)> _motion;
  std::function<void(GLFWwindow*,int,int,int,int,bool)> _key;
  std::function<void(std::shared_ptr<SceneNode>&)> _frame;
  std::function<void()> _draw;
  PythonCallback* _cb;
  GLFWwindow* _window;
  double _lastTime;
  bool _invoked;
  bool _debugBB;
  int _FPS;
};
}

#endif
