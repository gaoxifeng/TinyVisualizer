#ifndef DRAWER_H
#define DRAWER_H

#include "Background.h"
#include <functional>
#include <vector>

typedef struct GLFWwindow GLFWwindow;
namespace DRAWER {
class Texture;
class SceneNode;
class ShadowLight;
class Drawer;
class Povray;
class Camera2D;
class Camera3D;
struct GLFWwindowPtr {
  GLFWwindowPtr() {}
  GLFWwindowPtr(GLFWwindow* ptr):_ptr(ptr) {}
  GLFWwindowPtr& operator=(GLFWwindow* ptr) {
    _ptr=ptr;
    return *this;
  }
  GLFWwindow* _ptr=NULL;
};
//swig only support class-based callback
class PythonCallback : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(PythonCallback);
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
class Shape : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(Shape);
 public:
  enum PASS_TYPE {
    MESH_PASS   =1<<0,
    LINE_PASS   =1<<1,
    POINT_PASS  =1<<2,
    SHADOW_PASS =1<<3,
  };
  Shape();
  virtual ~Shape() {}
  virtual void setAlpha(GLfloat) {}
  virtual void setPointSize(GLfloat) {}
  virtual void setLineWidth(GLfloat) {}
  virtual void setColorDiffuse(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorAmbient(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setColorSpecular(GLenum,GLfloat,GLfloat,GLfloat) {}
  virtual void setShininess(GLenum,GLfloat) {}
  virtual void setTextureDiffuse(std::shared_ptr<Texture>) {}
  virtual void setTextureSpecular(std::shared_ptr<Texture>) {}
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
  virtual void drawPovray(Povray& pov) const=0;
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
class Camera : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(Camera);
 public:
  virtual ~Camera() {}
  virtual void focusOn(std::shared_ptr<Shape>) {}
  virtual void frame(GLFWwindowPtr,GLfloat) {}
  virtual void mouse(GLFWwindowPtr,int,int,int,bool) {}
  virtual void wheel(GLFWwindowPtr,double,double,bool) {}
  virtual void motion(GLFWwindowPtr,double,double,bool) {}
  virtual void key(GLFWwindowPtr,int,int,int,int,bool) {}
  virtual void draw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>&) {}
  virtual void drawPovray(Povray& pov,GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>&)=0;
  virtual Eigen::Matrix<GLfloat,-1,1> getCameraRay(GLFWwindowPtr,double,double) const {
    return Eigen::Matrix<GLfloat,-1,1>();
  }
  virtual Eigen::Matrix<GLfloat,-1,1> getViewFrustum() const {
    return Eigen::Matrix<GLfloat,-1,1>();
  }
  void saveCamera();
  void loadCamera();
 private:
  Eigen::Matrix<GLfloat,4,4> _mv,_p;
};
//Plugin
class Plugin : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(Plugin);
 public:
  Plugin();
  virtual ~Plugin() {}
  void setDrawer(Drawer* drawer);
  virtual void init(GLFWwindowPtr window) {}
  virtual void finalize() {}
  virtual void preDraw() {}
  virtual void postDraw() {}
  virtual void frame(std::shared_ptr<SceneNode>&) {}
  virtual bool mouse(GLFWwindowPtr wnd,int button,int action,int mods) {
    return true;
  }
  virtual bool wheel(GLFWwindowPtr wnd,double xoffset,double yoffset) {
    return true;
  }
  virtual bool motion(GLFWwindowPtr wnd,double x,double y) {
    return true;
  }
  virtual bool key(GLFWwindowPtr wnd,int key,int scan,int action,int mods) {
    return true;
  }
 protected:
  Drawer* _drawer;
};
//Drawer
class FBO;
class MultiDrawer;
class Drawer : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(Drawer);
 public:
  Drawer(const std::vector<std::string>& args,GLFWwindow* wnd=NULL,MultiDrawer* parent=NULL);
  Drawer(int argc,char** argv,GLFWwindow* wnd=NULL,MultiDrawer* parent=NULL);
  virtual ~Drawer();
  bool isVisible() const;
  void setRes(int width,int height);
  void setBackground(GLfloat r=1,GLfloat g=1,GLfloat b=1);
  void setBackground(std::shared_ptr<Texture> tex,const Eigen::Matrix<GLfloat,2,1>& tcMult=Eigen::Matrix<GLfloat,2,1>(1,1));
  void addLightSystem(int shadow=1024,int softShadow=10,bool autoAdjust=false);
  void timer();
  void frame();
  void draw();
  void drawPovray(Povray& pov);
  FBO* getOffScreenFBO() const;
  static void mouse(GLFWwindow* wnd,int button,int action,int mods);
  static void wheel(GLFWwindow* wnd,double xoffset,double yoffset);
  static void motion(GLFWwindow* wnd,double x,double y);
  static void key(GLFWwindow* wnd,int key,int scan,int action,int mods);
  void setMouseFunc(std::function<void(GLFWwindowPtr,int,int,int,bool)> mouse);
  void setWheelFunc(std::function<void(GLFWwindowPtr,double,double,bool)> wheel);
  void setMotionFunc(std::function<void(GLFWwindowPtr,double,double,bool)> motion);
  void setKeyFunc(std::function<void(GLFWwindowPtr,int,int,int,int,bool)> key);
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
  MultiDrawer* getParent() const;
  GLFWwindow* getWindow() const;
  void nextFrame();
  void mainLoop();
  int FPS() const;
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
  Background _background;
  std::shared_ptr<Camera> _camera;
  std::shared_ptr<SceneNode> _root;
  std::shared_ptr<ShadowLight> _light;
  std::vector<std::shared_ptr<Plugin>> _plugins;
  std::function<void(GLFWwindowPtr,int,int,int,bool)> _mouse;
  std::function<void(GLFWwindowPtr,double,double,bool)> _wheel;
  std::function<void(GLFWwindowPtr,double,double,bool)> _motion;
  std::function<void(GLFWwindowPtr,int,int,int,int,bool)> _key;
  std::function<void(std::shared_ptr<SceneNode>&)> _frame;
  std::function<void()> _draw;
  MultiDrawer* _parent=NULL;    //this is the multi-viewport pointer
  std::shared_ptr<FBO> _offScreen;
  GLFWwindow* _window=NULL;
  PythonCallback* _cb;
  double _lastTime;
  bool _debugBB;
  int _FPS;
};
}

#endif
