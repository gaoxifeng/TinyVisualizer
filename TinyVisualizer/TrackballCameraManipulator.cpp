#include "TrackballCameraManipulator.h"
#include "ImGuiPlugin.h"
#include "Camera3D.h"
#include "Matrix.h"
#ifdef IMGUI_SUPPORT
#include <imgui/imgui.h>
#endif

namespace DRAWER {
TrackballCameraManipulator::TrackballCameraManipulator(std::shared_ptr<Camera3D> camera)
  :CameraManipulator(camera),_scaleCoef(1.1f),_sensitive(.01f),_scaleMode(true),_inMotion(false) {
  _rot.setIdentity();
}
void TrackballCameraManipulator::frame(GLFWwindow* wnd,GLfloat time) {
  if(_inMotion) {
    Eigen::AngleAxis<GLfloat> rotUp,rotRight;
    rotUp.angle()=(GLfloat)(_xCurr-_posX)*_sensitive;
    rotUp.axis()=_camera->up();
    rotRight.angle()=(GLfloat)(_yCurr-_posY)*_sensitive;
    rotRight.axis()=_camera->direction().cross(_camera->up()).normalized();
    _rot=rotUp.toRotationMatrix()*rotRight.toRotationMatrix()*_rot0;
    //renormalize
    _rot.col(0).normalize();
    _rot.col(2)=_rot.col(0).cross(_rot.col(1)).normalized();
    _rot.col(1)=_rot.col(2).cross(_rot.col(0)).normalized();
  }
}
void TrackballCameraManipulator::mouse(GLFWwindow* wnd,int button,int action,int,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(button==GLFW_MOUSE_BUTTON_1) {
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
void TrackballCameraManipulator::wheel(GLFWwindow*,double,double yoffset,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(_scaleMode) {
    Eigen::Matrix<GLfloat,3,1> ctr=(_bb.template segment<3>(0)+_bb.template segment<3>(3))/2;
    Eigen::Matrix<GLfloat,3,1> dir=_camera->position()-ctr;
    _camera->setPosition(ctr+dir*std::pow(_scaleCoef,-yoffset));
  } else _sensitive*=(GLfloat)std::pow(1.1f,yoffset);
}
void TrackballCameraManipulator::motion(GLFWwindow* wnd,double x,double y,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(_inMotion) {
    _xCurr=x;
    _yCurr=y;
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

  getFloatv(GLModelViewMatrix,mv);
  mv*=r;
  loadIdentity();
  multMatrixf(mv);
}
void TrackballCameraManipulator::imGuiCallback() {
  Eigen::Matrix<GLfloat,3,1> ctr=(_bb.template segment<3>(0)+_bb.template segment<3>(3))/2;
  GLfloat range=std::max<GLfloat>(1e-5f,(_bb.template segment<3>(0)-_bb.template segment<3>(3)).norm());
  GLfloat dist=(GLfloat)std::log((_camera->position()-ctr).norm());
  GLfloat sensitivity=(GLfloat)std::log(_sensitive);
#ifdef IMGUI_SUPPORT
  ImGui::Begin("Trackball Camera Manipulator");
  ImGui::Text("Usage: left button to change view direction, wheel to zoom");
  ImGui::Text("Mouse mode: %s, middle button to switch",_scaleMode?"distance":"sensitivity");
  ImGui::SliderFloat("Camera distance",&dist,(GLfloat)std::log(range*0.01),(GLfloat)std::log(range*100));
  ImGui::SliderFloat("Angle sensitivity",&sensitivity,(GLfloat)std::log(1e-4f),(GLfloat)std::log(10.f));
  ImGui::End();
#endif
  //update distance
  Eigen::Matrix<GLfloat,3,1> dir=_camera->position()-ctr;
  _camera->setPosition(ctr+dir.normalized()*std::exp(dist));
  //update sensitivity
  _sensitive=std::exp(sensitivity);
}
}
