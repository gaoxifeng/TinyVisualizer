#include "Drawer.h"
#include "Camera2D.h"
#include "Camera3D.h"
#include "ShadowAndLight.h"
#include "SceneStructure.h"
#include <iostream>

namespace DRAWER {
//Shape
Shape::Shape():_enabled(true),_castShadow(true),_useLight(true) {}
void Shape::setEnabled(bool enabled) {
  _enabled=enabled;
}
void Shape::setCastShadow(bool castShadow) {
  _castShadow=castShadow;
}
void Shape::setUseLight(bool useLight) {
  _useLight=useLight;
}
bool Shape::enabled() const {
  return _enabled;
}
bool Shape::castShadow() const {
  return _castShadow;
}
bool Shape::useLight() const {
  return _useLight;
}
//Drawer
static void errFunc(int error, const char* description) {
  ASSERT_MSGV(false,"GLFW error=%d, message: %s!",error,description)
}
void mouseNothing(GLFWwindow*,int,int,int) {}
void wheelNothing(GLFWwindow*,double,double) {}
void motionNothing(GLFWwindow*,double,double) {}
void keyNothing(GLFWwindow*,int,int,int,int) {}
void doNothing(std::shared_ptr<SceneNode>&) {}
void drawNothing() {}
Drawer::Drawer(int argc,char** argv)
  :_mouse(mouseNothing),
   _wheel(wheelNothing),
   _motion(motionNothing),
   _key(keyNothing),
   _frame(doNothing),
   _draw(drawNothing),
   _lastTime(0),_invoked(false) {
  if(_invoked)
    return;
  _invoked=true;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  ASSERT_MSG(glfwInit()==GLFW_TRUE,"Failed initializing GLFW!")
  glfwSetErrorCallback(errFunc);
  glfwDefaultWindowHints();
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_SAMPLES, argparseRange(argc,argv,"MSAA",4));
  glfwWindowHint(GLFW_VISIBLE, argparseRange(argc,argv,"headless",0,Eigen::Matrix<int,2,1>(0,2))==0);
  std::string windowTitle=argparseRange(argc,argv,"title","Drawer");
  _window=glfwCreateWindow(argparseRange(argc,argv,"width",512),
                           argparseRange(argc,argv,"height",512),
                           windowTitle.c_str(), NULL, NULL);
  ASSERT_MSG(_window,"Failed initializing GLFW!")
  glfwMakeContextCurrent(_window);
  ASSERT_MSG(glewInit()==GLEW_OK,"Failed initializing GLEW!")
  glfwSwapInterval(1);
  glClearDepth(1.0f);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_POINT_SMOOTH);
  glClearColor(argparseRange(argc,argv,"backgroundR",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,
               argparseRange(argc,argv,"backgroundG",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,
               argparseRange(argc,argv,"backgroundB",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,1);
  glfwSetWindowUserPointer(_window,this);
  glfwSetMouseButtonCallback(_window,Drawer::mouse);
  glfwSetScrollCallback(_window,Drawer::wheel);
  glfwSetCursorPosCallback(_window,Drawer::motion);
  glfwSetKeyCallback(_window,Drawer::key);
  _FPS=argparseRange(argc,argv,"FPS",60,Eigen::Matrix<int,2,1>(10,200));
  //multi-sample not supported
  GLint samples;
  glGetIntegerv(GL_SAMPLES,&samples);
  if(samples)
    std::cout << "Context reports MSAA is available with " << samples << " samples" << std::endl;
  else std::cout << "Context reports MSAA is unavailable" << std::endl;
}
Drawer::~Drawer() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}
void Drawer::setRes(int width,int height) {
  glfwSetWindowSize(_window,width,height);
}
void Drawer::setBackground(GLfloat r,GLfloat g,GLfloat b) {
  glClearColor(r,g,b,1);
}
void Drawer::addLightSystem(int shadow,int softShadow) {
  _light.reset(new ShadowLight(shadow,softShadow));
}
void Drawer::timer() {
  double t=glfwGetTime();
  if(t-_lastTime>1.0/_FPS) {
    if(_camera)
      _camera->frame(_window,1.0f/FPS());
    _frame(_root);
    if(_root) {
      _root=SceneNode::update(_root);
      //_root->check();
    }
    _lastTime=t;
  }
}
void Drawer::draw() {
  int width=0,height=0;
  glfwGetFramebufferSize(_window,&width,&height);
  glViewport(0,0,width,height);

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);

  //calculate BB
  Eigen::Matrix<GLfloat,6,1> bb=_root?_root->getBB():resetBB();
  //setup camera
  if(_camera)
    _camera->draw(_window,bb);
  //draw shadow
  if(_light)
    _light->renderShadow(bb,[&](const Eigen::Matrix<GLfloat,-1,1>& viewFrustum) {
    if(_root)
      _root->draw([&](std::shared_ptr<Shape> s) {
      s->draw(true);
    },&viewFrustum);
  });
  //draw appearance
  if(_root) {
    Eigen::Matrix<GLfloat,-1,1> viewFrustum;
    if(_camera)
      viewFrustum=_camera->getViewFrustum();
    _root->draw([&](std::shared_ptr<Shape> s) {
      if(_light && s->useLight())
        _light->begin(bb);
      s->draw(false);
      if(_light && s->useLight())
        _light->end();
    },_camera?&viewFrustum:NULL);
  }
  //custom
  _draw();
}
void Drawer::mouse(GLFWwindow* wnd,int button,int action,int mods) {
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_camera)
    drawer->_camera->mouse(wnd,button,action,mods);
  drawer->_mouse(wnd,button,action,mods);
}
void Drawer::wheel(GLFWwindow* wnd,double xoffset,double yoffset) {
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_camera)
    drawer->_camera->wheel(wnd,xoffset,yoffset);
  drawer->_wheel(wnd,xoffset,yoffset);
}
void Drawer::motion(GLFWwindow* wnd,double x,double y) {
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_camera)
    drawer->_camera->motion(wnd,x,y);
  drawer->_motion(wnd,x,y);
}
void Drawer::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_camera)
    drawer->_camera->key(wnd,key,scan,action,mods);
  drawer->_key(wnd,key,scan,action,mods);
  switch (key) {
  case GLFW_KEY_ESCAPE:
    if(action==GLFW_PRESS)
      glfwSetWindowShouldClose(wnd,GLFW_TRUE);
    break;
  }
}
void Drawer::setMouseFunc(std::function<void(GLFWwindow*,int,int,int)> mouse) {
  _mouse=mouse;
}
void Drawer::setWheelFunc(std::function<void(GLFWwindow*,double,double)> wheel) {
  _wheel=wheel;
}
void Drawer::setMotionFunc(std::function<void(GLFWwindow*,double,double)> motion) {
  _motion=motion;
}
void Drawer::setKeyFunc(std::function<void(GLFWwindow*,int,int,int,int)> key) {
  _key=key;
}
void Drawer::setFrameFunc(std::function<void(std::shared_ptr<SceneNode>&)> frame) {
  _frame=frame;
}
void Drawer::setDrawFunc(std::function<void()> draw) {
  _draw=draw;
}
void Drawer::addCamera2D(GLfloat xExt) {
  _camera.reset(new Camera2D(xExt));
}
void Drawer::addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up) {
  _camera.reset(new Camera3D(angle,up));
}
Eigen::Matrix<GLfloat,2,1> Drawer::getWorldPos(double x,double y) {
  ASSERT(_camera);
  return _camera->getCameraRay(_window,x,y);
}
Eigen::Matrix<GLfloat,2,1> Drawer::getWorldPos() {
  double x=0,y=0;
  glfwGetCursorPos(_window,&x,&y);
  return getWorldPos(x,y);
}
ShadowLight& Drawer::getLight() {
  return *_light;
}
Camera& Drawer::getCamera() {
  ASSERT(_camera);
  return *_camera;
}
void Drawer::mainLoop() {
  while (!glfwWindowShouldClose(_window)) {
    draw();
    timer();
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}
int Drawer::FPS() {
  return _FPS;
}
//getter/setter
void Drawer::addShape(std::shared_ptr<Shape> s) {
  _root=SceneNode::update(_root,s);
}
void Drawer::focusOn(std::shared_ptr<Shape> s) {
  if(_camera)
    _camera->focusOn(s);
}
void Drawer::clear() {
  _root=NULL;
}
}
