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
//Shape for OpenGL drawing
class Shape {
 public:
  Shape();
  virtual ~Shape() {}
  virtual void setPointSize(GLfloat) {}
  virtual void setLineWidth(GLfloat) {}
  virtual void setColor(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorAmbient(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorSpecular(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setShininess(GLenum,GLfloat) {}
  virtual void setTexture(std::shared_ptr<Texture>) {}
  virtual void setEnabled(bool enabled);
  virtual void setCastShadow(bool castShadow);
  virtual void setUseLight(bool useLight);
  bool enabled() const;
  bool castShadow() const;
  bool useLight() const;
  virtual void draw(bool shadowPass) const=0;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const=0;
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
  virtual void mouse(GLFWwindow*,int,int,int) {}
  virtual void wheel(GLFWwindow*,double,double) {}
  virtual void motion(GLFWwindow*,double,double) {}
  virtual void frame(GLFWwindow*,GLfloat) {}
  virtual void key(GLFWwindow*,int,int,int,int) {}
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
  virtual void init(GLFWwindow* window) {}
  virtual void finalize() {}
  virtual void preDraw() {}
  virtual void postDraw() {}
  virtual void frame(std::shared_ptr<SceneNode>&) {}
  virtual void mouse(GLFWwindow* wnd,int button,int action,int mods) {}
  virtual void wheel(GLFWwindow* wnd,double xoffset,double yoffset) {}
  virtual void motion(GLFWwindow* wnd,double x,double y) {}
  virtual void key(GLFWwindow* wnd,int key,int scan,int action,int mods) {}
  virtual void clear() {}
};
//Drawer
class Drawer {
 public:
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
  void setMouseFunc(std::function<void(GLFWwindow*,int,int,int)> mouse);
  void setWheelFunc(std::function<void(GLFWwindow*,double,double)> wheel);
  void setMotionFunc(std::function<void(GLFWwindow*,double,double)> motion);
  void setKeyFunc(std::function<void(GLFWwindow*,int,int,int,int)> key);
  void setFrameFunc(std::function<void(std::shared_ptr<SceneNode>&)> frame);
  void setDrawFunc(std::function<void()> draw);
  void addCamera2D(GLfloat xExt);
  //addCamera3D-FirstPerson/TrackBall
  void addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up=Eigen::Matrix<GLfloat,3,1>(0,0,1));
  void addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up,const Eigen::Matrix<GLfloat,3,1>& pos,const Eigen::Matrix<GLfloat,3,1>& dir);
  Eigen::Matrix<GLfloat,2,1> getWorldPos(double x,double y);
  Eigen::Matrix<GLfloat,2,1> getWorldPos();
  std::shared_ptr<SceneNode> root();
  ShadowLight& getLight();
  Camera& getCamera();
  void mainLoop();
  int FPS();
  //getter/setter
  void addPlugin(std::shared_ptr<Plugin> pi);
  void removeShape(std::shared_ptr<Shape> s);
  void addShape(std::shared_ptr<Shape> s);
  void focusOn(std::shared_ptr<Shape> s);
  void clearScene();
  void clear();
 private:
  Drawer(const Drawer& other) {}
  Drawer& operator=(const Drawer& other) {
    return *this;
  }
  //data
  CameraType cameraType;
  std::shared_ptr<Camera> _camera;
  std::shared_ptr<SceneNode> _root;
  std::shared_ptr<ShadowLight> _light;
  std::vector<std::shared_ptr<Plugin>> _plugins;
  std::function<void(GLFWwindow*,int,int,int)> _mouse;
  std::function<void(GLFWwindow*,double,double)> _wheel;
  std::function<void(GLFWwindow*,double,double)> _motion;
  std::function<void(GLFWwindow*,int,int,int,int)> _key;
  std::function<void(std::shared_ptr<SceneNode>&)> _frame;
  std::function<void()> _draw;
  GLFWwindow* _window;
  double _lastTime;
  bool _invoked;
  int _FPS;
};
}

#endif
