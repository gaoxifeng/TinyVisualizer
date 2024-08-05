#include "Camera3D.h"
#include <iostream>
#include "Matrix.h"
#include "DefaultLight.h"
#include "Povray.h"
#include "VBO.h"

namespace DRAWER {
//Camera3D
Camera3D::Camera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up)
  :_debug(false),_angle(angle),_up(up) {
  _pos.setConstant(std::numeric_limits<GLfloat>::infinity());
  _dir.setConstant(std::numeric_limits<GLfloat>::infinity());
}
void Camera3D::setManipulator(std::shared_ptr<CameraManipulator> manipulator) {
  _manipulator=manipulator;
}
std::shared_ptr<CameraManipulator> Camera3D::getManipulator() const {
  return _manipulator;
}
void Camera3D::focusOn(std::shared_ptr<Shape> s) {
  _focus=s;
}
void Camera3D::frame(GLFWwindowPtr wnd,GLfloat time) {
  if(_manipulator)
    _manipulator->frame(wnd,time);
}
void Camera3D::mouse(GLFWwindowPtr wnd,int button,int action,int mods,bool captured) {
  if(_manipulator)
    _manipulator->mouse(wnd,button,action,mods,captured);
  if(captured)
    return;
  else if(button==GLFW_MOUSE_BUTTON_2) {
    if(action==GLFW_PRESS) {
      double x=0,y=0;
      loadCamera();
      glfwGetCursorPos(wnd._ptr,&x,&y);
      _debugLine=getCameraRay(wnd,x,y);
    }
  }
}
void Camera3D::wheel(GLFWwindowPtr wnd,double xoffset,double yoffset,bool captured) {
  if(_manipulator)
    _manipulator->wheel(wnd,xoffset,yoffset,captured);
}
void Camera3D::motion(GLFWwindowPtr wnd,double x,double y,bool captured) {
  if(_manipulator)
    _manipulator->motion(wnd,x,y,captured);
}
void Camera3D::key(GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) {
  if(_manipulator)
    _manipulator->key(wnd,key,scan,action,mods,captured);
  if(captured)
    return;
  else if(key==GLFW_KEY_V && action==GLFW_PRESS) {
    loadCamera();
    _debugFrustum=constructViewFrustum3D();
  } else if(key==GLFW_KEY_B && action==GLFW_PRESS)
    _debug=!_debug;
}
void Camera3D::draw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!std::isfinite(_pos[0]) && _manipulator)
    _manipulator->init(wnd,bb);
  if(_focus) {
    Eigen::Matrix<GLfloat,6,1> bbF=_focus->getBB();
    _dir=(bbF.segment<3>(0)+bbF.segment<3>(3))/2-_pos;
  }
  if(_manipulator)
    _manipulator->preDraw(wnd,bb);
  //model view
  matrixMode(GLModelViewMatrix);
  loadIdentity();
  Eigen::Matrix<GLfloat,3,1> up=_dir.cross(_up).cross(_dir).normalized();
  lookAtf(_pos[0],_pos[1],_pos[2],
          _pos[0]+_dir[0],_pos[1]+_dir[1],_pos[2]+_dir[2],
          up[0],up[1],up[2]);
  if(_manipulator)
    _manipulator->postDraw(wnd,bb);
  //projection
  GLfloat zNear=0,zFar=0;
  zRangef(bb,zNear,zFar);
  matrixMode(GLProjectionMatrix);
  loadIdentity();
  glGetIntegerv(GL_VIEWPORT,_vp);
  perspectivef(_angle,(GLfloat)_vp[2]/(GLfloat)_vp[3],zNear,zFar);
  saveCamera();

  if(_debug) {
    getDefaultProg()->begin();
    setupMatrixInShader();
    GLfloat scale=1;
    setupMaterial(NULL,1,0,0);
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(-scale,0,0),
      Eigen::Matrix<GLfloat,3,1>( scale,0,0)
    );
    setupMaterial(NULL,0,1,0);
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(0,-scale,0),
      Eigen::Matrix<GLfloat,3,1>(0, scale,0)
    );
    setupMaterial(NULL,0,0,1);
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(0,0,-scale),
      Eigen::Matrix<GLfloat,3,1>(0,0, scale)
    );
    if(_debugLine.size()==6) {
      setupMaterial(NULL,0,0,0);
      drawLinef(
        Eigen::Matrix<GLfloat,3,1>(_debugLine[0],_debugLine[1],_debugLine[2]),
        Eigen::Matrix<GLfloat,3,1>
        (_debugLine[0]+_debugLine[3],
         _debugLine[1]+_debugLine[4],
         _debugLine[2]+_debugLine[5])
      );
    }
    Program::currentProgram()->end();
    if(_debugFrustum.size()==24) {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      drawViewFrustum3D(_debugFrustum,Eigen::Matrix<GLfloat,4,1>(1,0,0,1));
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    std::cout << "zNear=" << zNear << " zFar=" << zFar << std::endl;
  }
}
void Camera3D::drawPovray(Povray& pov,GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  draw(wnd,bb);

  GLfloat zNear=0,zFar=0;
  zRangef(bb,zNear,zFar);
  Eigen::Matrix<GLfloat,3,1> up=_dir.cross(_up).cross(_dir).normalized();
  Eigen::Matrix<GLfloat,3,1> right=_dir.cross(_up).normalized();

  std::shared_ptr<Povray::Camera> c(new Povray::Camera);
  c->_pos=_pos;
  c->_dir=_dir.normalized()*zFar;
  c->_up=up*tan(_angle*M_PI/360)*zFar*2;
  c->_right=right*tan(_angle*M_PI/360)*zFar*2*_vp[2]/_vp[3];
  if(_manipulator)
    c->_trans=_manipulator->postDraw(wnd,bb).inverse().template block<3,4>(0,0);
  pov.addElement(c);
}
Eigen::Matrix<GLfloat,-1,1> Camera3D::getCameraRay(GLFWwindowPtr wnd,double x,double y) const {
  int w,h;
  glfwGetWindowSize(wnd._ptr,&w,&h);
  GLfloat ratioX=((GLfloat)(x-_vp[0])-(GLfloat)_vp[2]/2)/(GLfloat)(_vp[2]/2);
  GLfloat ratioY=-((GLfloat)(h-y-_vp[1])-(GLfloat)_vp[3]/2)/(GLfloat)(_vp[3]/2);

  Eigen::Matrix<GLfloat,4,1> dir(ratioX,-ratioY,0,1);
  Eigen::Matrix<GLfloat,4,4> mv,p;
  getFloatv(GLModelViewMatrix,mv);
  getFloatv(GLProjectionMatrix,p);
  dir=p.inverse()*dir;
  dir.segment<3>(0)/=dir[3];  //from homogeneous space

  Eigen::Matrix<GLfloat,6,1> ray;
  ray.segment<3>(0)=-mv.block<3,3>(0,0).transpose()*mv.block<3,1>(0,3);
  ray.segment<3>(3)= mv.block<3,3>(0,0).transpose()*dir.segment<3>(0);
  return ray;
}
Eigen::Matrix<GLfloat,-1,1> Camera3D::getViewFrustum() const {
  return getViewFrustum3DPlanes();
}
Eigen::Matrix<GLfloat,3,1> Camera3D::position() const {
  return _pos;
}
Eigen::Matrix<GLfloat,3,1> Camera3D::direction() const {
  return _dir;
}
Eigen::Matrix<GLfloat,3,1> Camera3D::up() const {
  return _up;
}
void Camera3D::setPosition(const Eigen::Matrix<GLfloat,3,1>& pos) {
  _pos=pos;
}
void Camera3D::setDirection(const Eigen::Matrix<GLfloat,3,1>& dir) {
  _dir=dir.normalized();
}
void Camera3D::setUp(const Eigen::Matrix<GLfloat,3,1>& up) {
  _up=up.normalized();
}
}
