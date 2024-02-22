#include <glad/gl.h>
#include "MultiDrawer.h"
#include "Camera2D.h"
#include "Camera3D.h"
#include "MeshShape.h"
#include "DefaultLight.h"
#include "DrawerUtility.h"
#include "ShadowAndLight.h"
#include "SceneStructure.h"
#include "FirstPersonCameraManipulator.h"
#include "TrackballCameraManipulator.h"
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
bool Shape::needRecomputeNormal() const {
  return false;
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
bool Shape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>&,GLfloat&) const {
  ASSERT_MSG(false,"rayIntersect function not implemented!")
  return false;
}
//Plugin
Plugin::Plugin():_drawer(NULL) {}
void Plugin::setDrawer(Drawer* drawer) {
  _drawer=drawer;
}
//Drawer
void errFunc(int error,const char* description) {
  ASSERT_MSGV(false,"GLFW error=%d, message: %s!",error,description)
}
void mouseNothing(GLFWwindow*,int,int,int,bool) {}
void wheelNothing(GLFWwindow*,double,double,bool) {}
void motionNothing(GLFWwindow*,double,double,bool) {}
void keyNothing(GLFWwindow*,int,int,int,int,bool) {}
void doNothing(std::shared_ptr<SceneNode>&) {}
void drawNothing() {}
Drawer::Drawer(const std::vector<std::string>& args,GLFWwindow* wnd,MultiDrawer* parent):_parent(parent),_window(wnd) {
  std::vector<char*> argv(args.size());
  for(int i=0; i<(int)args.size(); i++)
    argv[i]=(char*)args[i].c_str();
  init((int)args.size(),argv.data());
}
Drawer::Drawer(int argc,char** argv,GLFWwindow* wnd,MultiDrawer* parent):_parent(parent),_window(wnd) {
  init(argc,argv);
}
Drawer::~Drawer() {
  clear();
  if(_parent==NULL)
    //we are not using viewport
    glfwDestroyWindow(_window);
}
void Drawer::setRes(int width,int height) {
  glfwSetWindowSize(_window,width,height);
}
void Drawer::setBackground(GLfloat r,GLfloat g,GLfloat b) {
  glClearColor(r,g,b,1);
}
void Drawer::addLightSystem(int shadow,int softShadow,bool autoAdjust) {
  _light.reset(new ShadowLight(shadow,softShadow,autoAdjust));
}
void Drawer::timer() {
  double t=glfwGetTime();
  if(t-_lastTime>1.0/_FPS) {
    if(_cb)
      _cb->frame(_root);
    if(_camera)
      _camera->frame(_window,1.0f/FPS());
    _frame(_root);
    for(std::shared_ptr<Plugin> pi:_plugins)
      pi->frame(_root);
    if(_root && _light && _light->autoAdjust())
      _light->setDefaultLight(_root->getBB());
    if(_root) {
      _root=SceneNode::update(_root);
      //_root->check();
    }
    _lastTime=t;
  }
}
void Drawer::draw() {
  if(_parent) {
    Eigen::Matrix<int,4,1> vp=_parent->getViewport(this);
    glViewport(vp[0],vp[1],vp[2],vp[3]);
  } else {
    //this is for whole window
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //set viewport as whole window
    int width=0,height=0;
    glfwGetFramebufferSize(_window,&width,&height);
    glViewport(0,0,width,height);
  }

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
      s->setDrawer(this);
      s->draw(MeshShape::SHADOW_PASS);
    },&viewFrustum);
  });
  //plugin predraw
  for(std::shared_ptr<Plugin> pi:_plugins)
    pi->preDraw();
  //draw appearance
  if(_root) {
    Eigen::Matrix<GLfloat,-1,1> viewFrustum;
    if(_camera)
      viewFrustum=_camera->getViewFrustum();
    //mesh
    _root->draw([&](std::shared_ptr<Shape> s) {
      if(_light && s->useLight())
        _light->begin(bb,s->needRecomputeNormal());
      else getDefaultProg()->begin();
      s->setDrawer(this);
      s->draw(Shape::MESH_PASS);
      Program::currentProgram()->end();
    },_camera?&viewFrustum:NULL);
    //point
    getRoundPointProg()->begin();
    _root->draw([&](std::shared_ptr<Shape> s) {
      s->setDrawer(this);
      s->draw(Shape::POINT_PASS);
    },_camera?&viewFrustum:NULL);
    Program::currentProgram()->end();
    //line
    getThickLineProg()->begin();
    _root->draw([&](std::shared_ptr<Shape> s) {
      s->setDrawer(this);
      s->draw(Shape::LINE_PASS);
    },_camera?&viewFrustum:NULL);
    Program::currentProgram()->end();
    //debug draw bounding box
    if(_debugBB)
      _root->visit([&](std::shared_ptr<Shape> sRef)->bool{
      drawBB(sRef->getBB(),Eigen::Matrix<GLfloat,4,1>(0,0,0,1));
      return true;
    });
  }
  //custom
  _draw();
  if(_cb)
    _cb->draw();
  for(std::shared_ptr<Plugin> pi:_plugins)
    pi->postDraw();
}
void Drawer::mouse(GLFWwindow* wnd,int button,int action,int mods) {
  bool captured=false;
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_cb)
    drawer->_cb->mouse(button,action,mods);
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->mouse(wnd,button,action,mods)) {
      captured=true;
      break;
    }
  if(drawer->_camera)
    drawer->_camera->mouse(wnd,button,action,mods,captured);
  drawer->_mouse(wnd,button,action,mods,captured);
}
void Drawer::wheel(GLFWwindow* wnd,double xoffset,double yoffset) {
  bool captured=false;
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_cb)
    drawer->_cb->wheel(xoffset,yoffset);
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->wheel(wnd,xoffset,yoffset)) {
      captured=true;
      break;
    }
  if(drawer->_camera)
    drawer->_camera->wheel(wnd,xoffset,yoffset,captured);
  drawer->_wheel(wnd,xoffset,yoffset,captured);
}
void Drawer::motion(GLFWwindow* wnd,double x,double y) {
  bool captured=false;
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_cb)
    drawer->_cb->motion(x,y);
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->motion(wnd,x,y)) {
      captured=true;
      break;
    }
  if(drawer->_camera)
    drawer->_camera->motion(wnd,x,y,captured);
  drawer->_motion(wnd,x,y,captured);
}
void Drawer::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  bool captured=false;
  Drawer* drawer=(Drawer*)glfwGetWindowUserPointer(wnd);
  if(drawer->_cb)
    drawer->_cb->key(key,scan,action,mods);
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->key(wnd,key,scan,action,mods)) {
      captured=true;
      break;
    }
  if(drawer->_camera)
    drawer->_camera->key(wnd,key,scan,action,mods,captured);
  drawer->_key(wnd,key,scan,action,mods,captured);
  switch (key) {
  case GLFW_KEY_ESCAPE:
    if(action==GLFW_PRESS)
      glfwSetWindowShouldClose(wnd,GLFW_TRUE);
  case GLFW_KEY_N:
    if(action==GLFW_PRESS)
      drawer->_debugBB=!drawer->_debugBB;
    break;
  }
}
void Drawer::setMouseFunc(std::function<void(GLFWwindow*,int,int,int,bool)> mouse) {
  _mouse=mouse;
}
void Drawer::setWheelFunc(std::function<void(GLFWwindow*,double,double,bool)> wheel) {
  _wheel=wheel;
}
void Drawer::setMotionFunc(std::function<void(GLFWwindow*,double,double,bool)> motion) {
  _motion=motion;
}
void Drawer::setKeyFunc(std::function<void(GLFWwindow*,int,int,int,int,bool)> key) {
  _key=key;
}
void Drawer::setFrameFunc(std::function<void(std::shared_ptr<SceneNode>&)> frame) {
  _frame=frame;
}
void Drawer::setDrawFunc(std::function<void()> draw) {
  _draw=draw;
}
void Drawer::setPythonCallback(PythonCallback* cb) {
  _cb=cb;
}
void Drawer::addCamera2D(GLfloat xExt) {
  _camera.reset(new Camera2D(xExt));
}
void Drawer::addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up) {
  _camera.reset(new Camera3D(angle,up));
}
void Drawer::addCamera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up,const Eigen::Matrix<GLfloat,3,1>& pos,const Eigen::Matrix<GLfloat,3,1>& dir) {
  addCamera3D(angle,up);
  std::custom_pointer_cast<Camera3D>(_camera)->setDirection(dir);
  std::custom_pointer_cast<Camera3D>(_camera)->setPosition(pos);
}
bool Drawer::rayIntersect(Eigen::Matrix<GLfloat,6,1>& ray,std::shared_ptr<Shape>& IShape,GLfloat& IAlpha) const {
  IAlpha=1;
  if(!_root)
    return false;
  //find maximal distance
  GLfloat dist=0;
  Eigen::Matrix<GLfloat,6,1> bb=_root->getBB();
  for(GLfloat x: {
        bb[0],bb[3]
      })
    for(GLfloat y: {
          bb[1],bb[4]
        })
      for(GLfloat z: {
            bb[2],bb[5]
          })
        dist=std::max(dist,(Eigen::Matrix<GLfloat,3,1>(x,y,z)-ray.segment<3>(0)).norm());
  ray.segment<3>(3)=ray.segment<3>(3).normalized()*dist;
  return _root->rayIntersect(ray,IShape,IAlpha);
}
Eigen::Matrix<GLfloat,-1,1> Drawer::getCameraRay(double x,double y) {
  ASSERT(_camera);
  return _camera->getCameraRay(_window,x,y);
}
Eigen::Matrix<GLfloat,-1,1> Drawer::getCameraRay() {
  double x=0,y=0;
  glfwGetCursorPos(_window,&x,&y);
  return getCameraRay(x,y);
}
std::shared_ptr<SceneNode> Drawer::root() {
  return _root;
}
std::shared_ptr<ShadowLight> Drawer::getLight() {
  return _light;
}
void Drawer::clearLight() {
  _light=NULL;
}
std::shared_ptr<Camera> Drawer::getCamera() {
  ASSERT(_camera);
  return _camera;
}
std::shared_ptr<Camera2D> Drawer::getCamera2D() {
  ASSERT(_camera);
  return std::custom_pointer_cast<Camera2D>(_camera);
}
std::shared_ptr<Camera3D> Drawer::getCamera3D() {
  ASSERT(_camera);
  return std::custom_pointer_cast<Camera3D>(_camera);
}
void Drawer::mainLoop() {
  ASSERT_MSG(_parent==NULL,"mainLoop() of a multi-viewport Drawer cannot be called!")
  while (!glfwWindowShouldClose(_window)) {
    glfwPollEvents();
    draw();
    timer();
    glfwSwapBuffers(_window);
  }
}
int Drawer::FPS() {
  return _FPS;
}
//getter/setter
void Drawer::addPlugin(std::shared_ptr<Plugin> pi) {
  if(std::find(_plugins.begin(),_plugins.end(),pi)==_plugins.end())
    _plugins.push_back(pi);
  pi->setDrawer(this);
  pi->init(_window);
}
bool Drawer::contain(std::shared_ptr<Shape> s) const {
  bool ret=false;
  if(_root)
    _root->visit([&](std::shared_ptr<Shape> sRef)->bool{
    if(sRef==s)
      ret=true;
    return true;
  });
  return ret;
}
void Drawer::removeShape(std::shared_ptr<Shape> s) {
  bool alreadyAdded=false;
  if(_root)
    _root->visit([&](std::shared_ptr<Shape> sRef)->bool{
    if(sRef==s) {
      alreadyAdded=true;
      return false;
    }
    return true;
  });
  if(!alreadyAdded)
    return;
  _root=SceneNode::remove(_root,s);
  if(_root && _light && _light->autoAdjust())
    _light->setDefaultLight(_root->getBB());
}
void Drawer::addShape(std::shared_ptr<Shape> s) {
  bool alreadyAdded=false;
  if(_root)
    _root->visit([&](std::shared_ptr<Shape> sRef)->bool{
    if(sRef==s) {
      alreadyAdded=true;
      return false;
    }
    return true;
  });
  if(alreadyAdded)
    return;
  _root=SceneNode::update(_root,s);
  timer();
}
void Drawer::focusOn(std::shared_ptr<Shape> s) {
  if(_camera)
    _camera->focusOn(s);
}
void Drawer::clearScene() {
  _root=NULL;
}
void Drawer::clear() {
  for(std::shared_ptr<Plugin> pi:_plugins) {
    pi->setDrawer(NULL);
    pi->finalize();
  }
  _plugins.clear();
  clearLight();
  clearScene();
  clearVBO();
  Program::clearProgram();
}
//helper
void Drawer::init(int argc,char** argv) {
  _mouse=mouseNothing;
  _wheel=wheelNothing;
  _motion=motionNothing;
  _key=keyNothing;
  _frame=doNothing;
  _draw=drawNothing;
  _cb=NULL;
  _lastTime=0;
  _debugBB=false;

  if(_window==NULL) {
    ASSERT_MSG(glfwInit()==GLFW_TRUE,"Failed initializing GLFW!")
    glfwDefaultWindowHints();
    glfwSetErrorCallback(errFunc);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,argparseRange(argc,argv,"MSAA",4));
    glfwWindowHint(GLFW_VISIBLE,argparseRange(argc,argv,"headless",0,Eigen::Matrix<int,2,1>(0,2))==0);
    std::string windowTitle=argparseRange(argc,argv,"title","Drawer");
    _window=glfwCreateWindow(argparseRange(argc,argv,"width",512),
                             argparseRange(argc,argv,"height",512),
                             windowTitle.c_str(),NULL,NULL);
    ASSERT_MSG(_window,"Failed initializing GLFW!")
    glfwMakeContextCurrent(_window);
    int version=gladLoadGL(glfwGetProcAddress);
    ASSERT_MSG(version!=0,"Failed initializing GLAD!")
  }
  glfwSwapInterval(1);
  glClearDepth(1.0f);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_LINE_SMOOTH);
  glClearColor(argparseRange(argc,argv,"backgroundR",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,
               argparseRange(argc,argv,"backgroundG",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,
               argparseRange(argc,argv,"backgroundB",255,Eigen::Matrix<int,2,1>(0,256))/255.0f,1);
  if(_parent==NULL) {
    glfwSetWindowUserPointer(_window,this);
    glfwSetMouseButtonCallback(_window,Drawer::mouse);
    glfwSetScrollCallback(_window,Drawer::wheel);
    glfwSetCursorPosCallback(_window,Drawer::motion);
    glfwSetKeyCallback(_window,Drawer::key);
  }
  _FPS=argparseRange(argc,argv,"FPS",60,Eigen::Matrix<int,2,1>(10,200));
  //multi-sample not supported
  GLint samples;
  glGetIntegerv(GL_SAMPLES,&samples);
  if(samples)
    std::cout << "Context reports MSAA is available with " << samples << " samples" << std::endl;
  else std::cout << "Context reports MSAA is unavailable" << std::endl;
  //force initial MeshShape::Texture
  MeshShape();
  //add default light
  if(argparseRange(argc,argv,"defaultLight",1,Eigen::Matrix<int,2,1>(0,2))) {
    addLightSystem(argparseRange(argc,argv,"defaultShadow",0,Eigen::Matrix<int,2,1>(0,2049)),
                   argparseRange(argc,argv,"defaultShadowSoftness",20,Eigen::Matrix<int,2,1>(0,21)),
                   argparseRange(argc,argv,"defaultAutoAdjust",1,Eigen::Matrix<int,2,1>(0,2)));
    getLight()->lightSz(argparseRange(argc,argv,"defaultLightSz",20,Eigen::Matrix<int,2,1>(0,101)));
  }
  //add default camera
  if(argparseRange(argc,argv,"defaultCamera2D",0,Eigen::Matrix<int,2,1>(0,2)))
    addCamera2D((GLfloat)argparseRange(argc,argv,"defaultCamera2DExt",10,Eigen::Matrix<int,2,1>(0,11)));
  else if(argparseRange(argc,argv,"defaultCamera3D",1,Eigen::Matrix<int,2,1>(0,2))) {
    addCamera3D((GLfloat)argparseRange(argc,argv,"defaultCamera3DFovy",90,Eigen::Matrix<int,2,1>(0,271)),
                Eigen::Matrix<GLfloat,3,1>::Unit(argparseRange(argc,argv,"defaultCamera3DUp",2,Eigen::Matrix<int,2,1>(0,3))));
    std::string manipulatorType=argparseChoice(argc,argv,"defaultCamera3DManipulator","Trackball", {"FPS","Trackball","None"});
    if(manipulatorType=="FPS")
      getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(getCamera3D())));
    else if(manipulatorType=="Trackball")
      getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new TrackballCameraManipulator(getCamera3D())));
  }
}
Drawer::Drawer(const Drawer& other) {}
Drawer& Drawer::operator=(const Drawer& other) {
  return *this;
}
}
