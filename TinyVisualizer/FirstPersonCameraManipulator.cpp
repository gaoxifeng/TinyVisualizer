#include "FirstPersonCameraManipulator.h"
#include "Camera3D.h"
#ifdef IMGUI_SUPPORT
#include <imgui/imgui.h>
#endif

namespace DRAWER {
FirstPersonCameraManipulator::FirstPersonCameraManipulator(std::shared_ptr<Camera3D> camera)
  :CameraManipulator(camera),_forward(false),_backward(false),_left(false),_right(false),_rise(false),_dive(false),
   _sensitive(1),_speed(1),_inMotion(false),_speedMode(true) {
  int id;
  _camera->up().cwiseAbs().minCoeff(&id);
  _t1=Eigen::Matrix<GLfloat,3,1>::Unit(id).cross(_camera->up()).normalized();
  _t2=_camera->up().cross(_t1);
}
void FirstPersonCameraManipulator::frame(GLFWwindow* wnd,GLfloat time) {
  if(_inMotion) {
    GLfloat theta=0,phi=0;
    begin(theta,phi);
    theta-=clampMin(_xCurr-_xLast)*_sensitive*time;
    phi-=clampMin(_yCurr-_yLast)*_sensitive*time;
    end(theta,phi);
    int w=0,h=0;
    glfwGetWindowSize(wnd,&w,&h);
    glfwSetCursorPos(wnd,w/2.0f,h/2.0f);
    _xLast=w/2.0f;
    _yLast=h/2.0f;
  }
  if(_forward)
    _camera->setPosition(_camera->position()+_camera->direction()*_speed*time);
  if(_backward)
    _camera->setPosition(_camera->position()-_camera->direction()*_speed*time);
  if(_left)
    _camera->setPosition(_camera->position()+_camera->up().cross(_camera->direction()).normalized()*_speed*time);
  if(_right)
    _camera->setPosition(_camera->position()-_camera->up().cross(_camera->direction()).normalized()*_speed*time);
  if(_rise)
    _camera->setPosition(_camera->position()+_camera->up()*_speed*time);
  if(_dive)
    _camera->setPosition(_camera->position()-_camera->up()*_speed*time);
}
void FirstPersonCameraManipulator::mouse(GLFWwindow* wnd,int button,int action,int,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(button==GLFW_MOUSE_BUTTON_1) {
    if(action==GLFW_PRESS) {
      _inMotion=true;
      int w=0,h=0;
      glfwGetWindowSize(wnd,&w,&h);
      glfwSetCursorPos(wnd,w/2.0f,h/2.0f);
      _xLast=_xCurr=w/2.0f;
      _yLast=_yCurr=h/2.0f;
    } else if(action==GLFW_RELEASE) {
      _inMotion=false;
    }
  } else if(button==GLFW_MOUSE_BUTTON_3) {
    if(action==GLFW_PRESS)
      _speedMode=!_speedMode;
  }
}
void FirstPersonCameraManipulator::wheel(GLFWwindow*,double,double yoffset,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(_speedMode)
    _speed*=(GLfloat)std::pow(1.1f,yoffset);
  else _sensitive*=(GLfloat)std::pow(1.1f,yoffset);
}
void FirstPersonCameraManipulator::motion(GLFWwindow* wnd,double x,double y,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(_inMotion) {
    _xCurr=(GLfloat)x;
    _yCurr=(GLfloat)y;
  }
}
void FirstPersonCameraManipulator::key(GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {
  if(captured) {
    _inMotion=false;
    return;
  } else if(key==GLFW_KEY_W)
    _forward=action==GLFW_PRESS||action==GLFW_REPEAT;
  else if(key==GLFW_KEY_S)
    _backward=action==GLFW_PRESS||action==GLFW_REPEAT;
  else if(key==GLFW_KEY_A)
    _left=action==GLFW_PRESS||action==GLFW_REPEAT;
  else if(key==GLFW_KEY_D)
    _right=action==GLFW_PRESS||action==GLFW_REPEAT;
  else if(key==GLFW_KEY_SPACE)
    _rise=action==GLFW_PRESS||action==GLFW_REPEAT;
  else if(key==GLFW_KEY_C)
    _dive=action==GLFW_PRESS||action==GLFW_REPEAT;
}
void FirstPersonCameraManipulator::imGuiCallback() {
  GLfloat speed=std::log(_speed);
  GLfloat sensitivity=std::log(_sensitive);
#ifdef IMGUI_SUPPORT
  ImGui::Begin("First Person Camera Manipulator");
  ImGui::Text("Usage: w/s/a/d to move, left button to change view");
  ImGui::Text("Mouse mode: %s, middle button to switch",_speedMode?"motion speed":"sensitivity");
  ImGui::SliderFloat("Camera speed",&speed,(GLfloat)std::log(0.01),(GLfloat)std::log(100));
  ImGui::SliderFloat("Angle sensitivity",&sensitivity,(GLfloat)std::log(1e-4f),(GLfloat)std::log(10.f));
  ImGui::End();
#endif
  //update speed
  _speed=std::exp(speed);
  //update sensitivity
  _sensitive=std::exp(sensitivity);
}
bool FirstPersonCameraManipulator::isSpeedMode() const {
  return _speedMode;
}
void FirstPersonCameraManipulator::begin(GLfloat& theta,GLfloat& phi) const {
  GLfloat x=_camera->direction().dot(_t1);
  GLfloat y=_camera->direction().dot(_t2);
  GLfloat z=_camera->direction().dot(_camera->up());
  theta=atan2(y,x);
  phi=std::min<GLfloat>((GLfloat)M_PI/2*0.99f,std::max<GLfloat>(-(GLfloat)M_PI/2*0.99f,atan2(z,std::sqrt(x*x+y*y))));
}
void FirstPersonCameraManipulator::end(GLfloat theta,GLfloat phi) {
  phi=std::min<GLfloat>((GLfloat)M_PI/2*0.99f,std::max<GLfloat>(-(GLfloat)M_PI/2*0.99f,phi));
  GLfloat x=std::cos(theta)*std::cos(phi);
  GLfloat y=std::sin(theta)*std::cos(phi);
  GLfloat z=std::sin(phi);
  _camera->setDirection(_t1*x+_t2*y+_camera->up()*z);
}
GLfloat FirstPersonCameraManipulator::clampMin(GLfloat val) {
  static GLfloat minVal=3;
  if(val>0)
    return (val<minVal)?0:val;
  else return (val>-minVal)?0:val;
}
}
