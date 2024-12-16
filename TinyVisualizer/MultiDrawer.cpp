#include "MultiDrawer.h"
#include "DrawerUtility.h"
#include "FBO.h"
#include <iostream>

namespace DRAWER {
extern void errFunc(int error,const char* description);
//MultiDrawer
MultiDrawer::MultiDrawer(const std::vector<std::string>& args):_args(args) {
  std::vector<char*> argv(args.size());
  for(int i=0; i<(int)args.size(); i++)
    argv[i]=(char*)args[i].c_str();
  init((int)args.size(),argv.data());
}
MultiDrawer::MultiDrawer(int argc,char** argv) {
  _args.resize(argc);
  for(int i=0; i<argc; i++)
    _args.push_back(argv[i]);
  init(argc,argv);
}
MultiDrawer::~MultiDrawer() {
  clear();
  _offScreen=NULL;
  glfwDestroyWindow(_window);
}
//multi-viewport
void MultiDrawer::setViewportLayout(int rows,int cols) {
  //clear
  _viewMap.clear();
  _views.clear();
  //construct
  _views.resize(rows);
  for(int r=0; r<rows; r++) {
    for(int c=0; c<cols; c++) {
      _views[r].push_back(std::shared_ptr<Drawer>(new Drawer(_args,_window,this)));
      _viewMap[_views[r][c].get()]=Eigen::Matrix<int,2,1>(r,c);
    }
  }
  glfwSetWindowUserPointer(_window,_views[0][0].get());
}
std::shared_ptr<Drawer> MultiDrawer::getDrawer(int row,int col) const {
  return _views[row][col];
}
Eigen::Matrix<int,4,1> MultiDrawer::getViewport(Drawer* drawer) const {
  auto it=_viewMap.find(drawer);
  ASSERT_MSG(it!=_viewMap.end(),"Cannot find sub-viewport!")
  //set viewport as whole window
  int width=0,height=0;
  glfwGetFramebufferSize(_window,&width,&height);
  //return viewport
  Eigen::Matrix<int,4,1> vp;
  int rows=(int)_views.size();
  int cols=(int)_views[0].size();
  int row=it->second[0];
  int col=it->second[1];
  vp[0]=col*width/cols;
  vp[1]=row*height/rows;
  vp[2]=(col+1)*width/cols-vp[0];
  vp[3]=(row+1)*height/rows-vp[1];
  return vp;
}
//common function
void MultiDrawer::setRes(int width,int height) {
  glfwSetWindowSize(_window,width,height);
}
void MultiDrawer::timer() {
  double t=glfwGetTime();
  if(t-_lastTime>1.0/getDrawer(0,0)->FPS()) {
    frame();
    _lastTime=t;
  }
}
void MultiDrawer::frame() {
  for(const auto& vrow:_views)
    for(const auto& v:vrow)
      v->timer();
  std::shared_ptr<SceneNode> nullRoot;
  for(std::shared_ptr<Plugin> pi:_plugins)
    pi->frame(nullRoot);
}
void MultiDrawer::draw() {
  FBO* offScreen=getOffScreenFBO();
  //if we have FBO bound, start using the FBO
  if(offScreen!=NULL)
    offScreen->begin();
  //Initialize
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  //if we have FBO bound, end using the FBO
  if(offScreen!=NULL)
    offScreen->end();

  //if we have FBO bound, start using the FBO
  if(offScreen!=NULL)
    offScreen->begin();
  //plugin pre-draw
  for(std::shared_ptr<Plugin> pi:_plugins)
    pi->preDraw();
  //if we have FBO bound, end using the FBO
  if(offScreen!=NULL)
    offScreen->end();

  //draw views
  for(const auto& vrow:_views)
    for(const auto& v:vrow) {
      glfwSetWindowUserPointer(_window,v.get());
      v->draw();
    }

  //if we have FBO bound, start using the FBO
  if(offScreen!=NULL)
    offScreen->begin();
  //plugin post-draw
  for(std::shared_ptr<Plugin> pi:_plugins)
    pi->postDraw();
  //if we have FBO bound, end using the FBO
  if(offScreen!=NULL)
    offScreen->end();
}
FBO* MultiDrawer::getOffScreenFBO() const {
  return _offScreen.get();
}
void MultiDrawer::mouse(GLFWwindow* wnd,int button,int action,int mods) {
  MultiDrawer* drawer=((Drawer*)glfwGetWindowUserPointer(wnd))->getParent();
  bool captured=false;
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->mouse(wnd,button,action,mods)) {
      captured=true;
      break;
    }
  if(captured)
    return;

  int w,h;
  double x,y;
  glfwGetCursorPos(wnd,&x,&y);
  glfwGetWindowSize(wnd,&w,&h);
  for(const auto& vrow:drawer->_views)
    for(const auto& v:vrow) {
      glfwSetWindowUserPointer(wnd,v.get());
      Eigen::Matrix<int,4,1> vp=drawer->getViewport(v.get());
      if(x>=vp[0] && x<vp[0]+vp[2])
        if((h-y)>=vp[1] && (h-y)<vp[1]+vp[3])
          v->mouse(wnd,button,action,mods);
    }
}
void MultiDrawer::wheel(GLFWwindow* wnd,double xoffset,double yoffset) {
  MultiDrawer* drawer=((Drawer*)glfwGetWindowUserPointer(wnd))->getParent();
  bool captured=false;
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->wheel(wnd,xoffset,yoffset)) {
      captured=true;
      break;
    }
  if(captured)
    return;

  int w,h;
  double x,y;
  glfwGetCursorPos(wnd,&x,&y);
  glfwGetWindowSize(wnd,&w,&h);
  for(const auto& vrow:drawer->_views)
    for(const auto& v:vrow) {
      glfwSetWindowUserPointer(wnd,v.get());
      Eigen::Matrix<int,4,1> vp=drawer->getViewport(v.get());
      if(x>=vp[0] && x<vp[0]+vp[2])
        if((h-y)>=vp[1] && (h-y)<vp[1]+vp[3])
          v->wheel(wnd,xoffset,yoffset);
    }
}
void MultiDrawer::motion(GLFWwindow* wnd,double x,double y) {
  MultiDrawer* drawer=((Drawer*)glfwGetWindowUserPointer(wnd))->getParent();
  bool captured=false;
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->motion(wnd,x,y)) {
      captured=true;
      break;
    }
  if(captured)
    return;

  int w,h;
  glfwGetWindowSize(wnd,&w,&h);
  for(const auto& vrow:drawer->_views)
    for(const auto& v:vrow) {
      glfwSetWindowUserPointer(wnd,v.get());
      Eigen::Matrix<int,4,1> vp=drawer->getViewport(v.get());
      if(x>=vp[0] && x<vp[0]+vp[2])
        if((h-y)>=vp[1] && (h-y)<vp[1]+vp[3])
          v->motion(wnd,x,y);
    }
}
void MultiDrawer::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  MultiDrawer* drawer=((Drawer*)glfwGetWindowUserPointer(wnd))->getParent();
  bool captured=false;
  for(std::shared_ptr<Plugin> pi:drawer->_plugins)
    if(!pi->key(wnd,key,scan,action,mods)) {
      captured=true;
      break;
    }
  if(captured)
    return;

  for(const auto& vrow:drawer->_views)
    for(const auto& v:vrow) {
      glfwSetWindowUserPointer(wnd,v.get());
      v->key(wnd,key,scan,action,mods);
    }
}
void MultiDrawer::nextFrame() {
  glfwPollEvents();
  draw();
  timer();
  glfwSwapBuffers(_window);
}
void MultiDrawer::mainLoop() {
  while (!glfwWindowShouldClose(_window))
    nextFrame();
}
//getter/setter
void MultiDrawer::addPlugin(std::shared_ptr<Plugin> pi) {
  if(std::find(_plugins.begin(),_plugins.end(),pi)==_plugins.end())
    _plugins.push_back(pi);
  pi->setDrawer(NULL);
  pi->init(_window);
}
void MultiDrawer::clear() {
  for(std::shared_ptr<Plugin> pi:_plugins) {
    pi->setDrawer(NULL);
    pi->finalize();
  }
  _plugins.clear();
  _viewMap.clear();
  _views.clear();
}
//helper
void MultiDrawer::init(int argc,char** argv) {
  _lastTime=0;

  ASSERT_MSG(glfwInit()==GLFW_TRUE,"Failed initializing GLFW!")
  bool visible=argparseRange(argc,argv,"headless",0,Eigen::Matrix<int,2,1>(0,2))==0;
  glfwDefaultWindowHints();
  glfwSetErrorCallback(errFunc);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES,argparseRange(argc,argv,"MSAA",4));
  glfwWindowHint(GLFW_VISIBLE,visible);
  std::string windowTitle=argparseRange(argc,argv,"title","Drawer");
  _window=glfwCreateWindow(argparseRange(argc,argv,"width",512),
                           argparseRange(argc,argv,"height",512),
                           windowTitle.c_str(),NULL,NULL);
  ASSERT_MSG(_window,"Failed initializing GLFW!")
  glfwMakeContextCurrent(_window);
  int version=gladLoadGL(glfwGetProcAddress);
  ASSERT_MSG(version!=0,"Failed initializing GLAD!")
  //add offscreen render target
  int width,height;
  glfwGetWindowSize(_window,&width,&height);
  if(!visible)
    _offScreen.reset(new FBO(width,height,GL_RGBA));
  //function override
  glfwSetWindowUserPointer(_window,NULL);
  glfwSetMouseButtonCallback(_window,MultiDrawer::mouse);
  glfwSetScrollCallback(_window,MultiDrawer::wheel);
  glfwSetCursorPosCallback(_window,MultiDrawer::motion);
  glfwSetKeyCallback(_window,MultiDrawer::key);
}
MultiDrawer::MultiDrawer(const MultiDrawer& other) {}
MultiDrawer& MultiDrawer::operator=(const MultiDrawer& other) {
  return *this;
}
}
