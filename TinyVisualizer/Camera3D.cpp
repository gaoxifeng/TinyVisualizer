#include "Camera3D.h"
#include <iostream>

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
void Camera3D::focusOn(std::shared_ptr<Shape> s) {
  _focus=s;
}
void Camera3D::mouse(GLFWwindow* wnd,int button,int action,int mods) {
  if(_manipulator)
    _manipulator->mouse(wnd,button,action,mods);
  if(button==GLFW_MOUSE_BUTTON_2) {
    if(action==GLFW_PRESS) {
      double x=0,y=0;
      glfwGetCursorPos(wnd,&x,&y);
      _debugLine=getCameraRay(wnd,x,y);
    }
  }
}
void Camera3D::wheel(GLFWwindow* wnd,double xoffset,double yoffset) {
  if(_manipulator)
    _manipulator->wheel(wnd,xoffset,yoffset);
}
void Camera3D::motion(GLFWwindow* wnd,double x,double y) {
  if(_manipulator)
    _manipulator->motion(wnd,x,y);
}
void Camera3D::frame(GLFWwindow* wnd,GLfloat time) {
  if(_manipulator)
    _manipulator->frame(wnd,time);
}
void Camera3D::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  if(_manipulator)
    _manipulator->key(wnd,key,scan,action,mods);
  if(key==GLFW_KEY_V && action==GLFW_PRESS)
    _debugFrustum=constructViewFrustum3D();
  else if(key==GLFW_KEY_B && action==GLFW_PRESS)
    _debug=!_debug;
}
void Camera3D::draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!std::isfinite(_pos[0]) && _manipulator)
    _manipulator->init(wnd,bb);
  if(_focus) {
    Eigen::Matrix<GLfloat,6,1> bbF=_focus->getBB();
    _dir=(bbF.segment<3>(0)+bbF.segment<3>(3))/2-_pos;
  }
  if(_manipulator)
    _manipulator->preDraw(wnd,bb);
  //model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Eigen::Matrix<GLfloat,3,1> up=_dir.cross(_up).cross(_dir).normalized();
  gluLookAt(_pos[0],_pos[1],_pos[2],
            _pos[0]+_dir[0],_pos[1]+_dir[1],_pos[2]+_dir[2],
            up[0],up[1],up[2]);
  if(_manipulator)
    _manipulator->postDraw(wnd,bb);
  //projection
  GLfloat zNear=0,zFar=0;
  zRange(bb,_pos,_dir,zNear,zFar);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int w=0,h=0;
  glfwGetWindowSize(wnd,&w,&h);
  gluPerspective(_angle,(GLfloat)w/(GLfloat)h,zNear,zFar);
  if(_debug) {
    GLfloat scale=1;
    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(-scale,0,0);
    glVertex3f( scale,0,0);
    glColor3f(0,1,0);
    glVertex3f(0,-scale,0);
    glVertex3f(0, scale,0);
    glColor3f(0,0,1);
    glVertex3f(0,0,-scale);
    glVertex3f(0,0, scale);
    if(_debugLine.size()==6) {
      glColor3f(0,0,0);
      glVertex3f(_debugLine[0],_debugLine[1],_debugLine[2]);
      glVertex3f(_debugLine[3],_debugLine[4],_debugLine[5]);
    }
    glEnd();
    if(_debugFrustum.size()==24) {
      glColor3f(1,0,0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      drawViewFrustum3D(_debugFrustum);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    std::cout << "zNear=" << zNear << " zFar=" << zFar << std::endl;
  }
}
Eigen::Matrix<GLfloat,-1,1> Camera3D::getCameraRay(GLFWwindow* wnd,double x,double y) const {
  Eigen::Matrix<GLfloat,6,1> ret;
  Eigen::Matrix<GLfloat,3,1> right,up;

  right=_dir.cross(_up).normalized();
  up=right.cross(_dir);

  int w=0,h=0;
  glfwGetWindowSize(wnd,&w,&h);
  up*=std::tan(_angle/2);
  right*=std::tan(_angle/2)*(GLfloat)w/(GLfloat)h;
  GLfloat ratioX=(x-w/2)/(GLfloat)(w/2);
  GLfloat ratioY=(y-h/2)/(GLfloat)(h/2);

  ret.segment<3>(0)=_pos;
  ret.segment<3>(3)=_dir+right*ratioX+up*ratioY;
  return ret;
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
