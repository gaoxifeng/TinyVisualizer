#include "TrackballCameraManipulator.h"
#include "Camera3D.h"

namespace DRAWER {
TrackballCameraManipulator::TrackballCameraManipulator(std::shared_ptr<Camera3D> camera)
  :CameraManipulator(camera),_scaleCoef(1.1f),_sensitive(.01f),_scaleMode(true),_inMotion(false) {
  _rot.setIdentity();
}
void TrackballCameraManipulator::mouse(GLFWwindow* wnd,int button,int action,int) {
  if(button==GLFW_MOUSE_BUTTON_1) {
    if(action==GLFW_PRESS) {
      _inMotion=true;
      glfwGetCursorPos(wnd,&_posX,&_posY);
      _xCurr=_posX;
      _yCurr=_posY;
      _rot0=_rot;
    } else if(action==GLFW_RELEASE)
      _inMotion=false;
  } else if(button==GLFW_MOUSE_BUTTON_3) {
    if(action==GLFW_PRESS)
      _scaleMode=!_scaleMode;
  }
}
void TrackballCameraManipulator::wheel(GLFWwindow*,double,double yoffset) {
  if(_scaleMode) {
    Eigen::Matrix<GLfloat,3,1> ctr=(_bb.template segment<3>(0)+_bb.template segment<3>(3))/2;
    Eigen::Matrix<GLfloat,3,1> dir=_camera->position()-ctr;
    _camera->setPosition(ctr+dir*std::pow(_scaleCoef,-yoffset));
  } else _sensitive*=std::pow(1.1f,yoffset);
}
void TrackballCameraManipulator::motion(GLFWwindow* wnd,double x,double y) {
  if(_inMotion) {
    _xCurr=x;
    _yCurr=y;
  }
}
void TrackballCameraManipulator::frame(GLFWwindow* wnd,GLfloat time) {
  if(_inMotion) {
    Eigen::AngleAxis<GLfloat> rotUp,rotRight;
    rotUp.angle()=(_xCurr-_posX)*_sensitive;
    rotUp.axis()=_camera->up();
    rotRight.angle()=(_yCurr-_posY)*_sensitive;
    rotRight.axis()=_camera->direction().cross(_camera->up()).normalized();
    _rot=rotUp.toRotationMatrix()*rotRight.toRotationMatrix()*_rot0;
    //renormalize
    _rot.col(0).normalize();
    _rot.col(2)=_rot.col(0).cross(_rot.col(1)).normalized();
    _rot.col(1)=_rot.col(2).cross(_rot.col(0)).normalized();
  }
}
void TrackballCameraManipulator::preDraw(GLFWwindow*,const Eigen::Matrix<GLfloat,6,1>& bb) {
  _bb=bb;
  Eigen::Matrix<GLfloat,3,1> ctr=(_bb.segment<3>(0)+_bb.segment<3>(3))/2;
  _camera->setDirection(ctr-_camera->position());
}
void TrackballCameraManipulator::postDraw(GLFWwindow*,const Eigen::Matrix<GLfloat,6,1>& bb) {
  Eigen::Matrix<GLfloat,3,1> ctr=(_bb.segment<3>(0)+_bb.segment<3>(3))/2;
  Eigen::Matrix<GLfloat,4,4> mv,r;
  r.setIdentity();
  r.template block<3,3>(0,0)=_rot;
  r.template block<3,1>(0,3)=ctr-_rot*ctr;

  glGetFloatv(GL_MODELVIEW_MATRIX,mv.data());
  mv*=r;
  glLoadIdentity();
  glMultMatrixf(mv.data());
}
}
