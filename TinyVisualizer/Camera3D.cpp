#include "Camera3D.h"
#include <iostream>

namespace DRAWER {
//Camera3D
Camera3D::Camera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up)
  :_forward(false),_backward(false),_left(false),_right(false),_rise(false),_dive(false),
   _angle(angle),_sensitive(1),_speed(1),_inMotion(false),_speedMode(true),_debug(false),_up(up) {
  _pos.setConstant(std::numeric_limits<GLfloat>::infinity());
  _dir.setConstant(std::numeric_limits<GLfloat>::infinity());

  int id;
  up.cwiseAbs().minCoeff(&id);
  _t1=Eigen::Matrix<GLfloat,3,1>::Unit(id).cross(_up).normalized();
  _t2=_up.cross(_t1);
}
void Camera3D::focusOn(std::shared_ptr<Shape> s) {
  _focus=s;
}
void Camera3D::mouse(GLFWwindow* wnd,int button,int action,int) {
  if(button==GLFW_MOUSE_BUTTON_1) {
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
  } else if(button==GLFW_MOUSE_BUTTON_2) {
    if(action==GLFW_PRESS) {
      double x=0,y=0;
      glfwGetCursorPos(wnd,&x,&y);
      _debugLine=getCameraRay(wnd,x,y);
    }
  }
}
void Camera3D::wheel(GLFWwindow*,double,double yoffset) {
  if(_speedMode)
    _speed*=std::pow(1.1,yoffset);
  else _sensitive*=std::pow(1.1,yoffset);
}
void Camera3D::motion(GLFWwindow* wnd,double x,double y) {
  if(_inMotion) {
    _xCurr=x;
    _yCurr=y;
  }
}
void Camera3D::frame(GLFWwindow* wnd,GLfloat time) {
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
    _pos+=_dir*_speed*time;
  if(_backward)
    _pos-=_dir*_speed*time;
  if(_left)
    _pos+=_up.cross(_dir).normalized()*_speed*time;
  if(_right)
    _pos-=_up.cross(_dir).normalized()*_speed*time;
  if(_rise)
    _pos+=_up*_speed*time;
  if(_dive)
    _pos-=_up*_speed*time;
}
void Camera3D::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  if(key==GLFW_KEY_W)
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
  else if(key==GLFW_KEY_V && action==GLFW_PRESS)
    _debugFrustum=constructViewFrustum3D();
  else if(key==GLFW_KEY_B && action==GLFW_PRESS)
    _debug=!_debug;
}
void Camera3D::draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!std::isfinite(_pos[0])) {
    if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max())
      _pos=Eigen::Matrix<GLfloat,3,1>(-1,0,0);
    else _pos=Eigen::Matrix<GLfloat,3,1>(bb[0],(bb[1]+bb[4])/2,(bb[2]+bb[5])/2);
    _dir=Eigen::Matrix<GLfloat,3,1>(1,0,0);
  }
  if(_focus) {
    Eigen::Matrix<GLfloat,6,1> bbF=_focus->getBB();
    _dir=(bbF.segment<3>(0)+bbF.segment<3>(3))/2-_pos;
    if(_dir.norm()>1e-5f) {
      GLfloat theta=0,phi=0;
      begin(theta,phi);
      end(theta,phi);
    }
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Eigen::Matrix<GLfloat,3,1> up=_dir.cross(_up).cross(_dir).normalized();
  gluLookAt(_pos[0],_pos[1],_pos[2],
            _pos[0]+_dir[0],_pos[1]+_dir[1],_pos[2]+_dir[2],
            up[0],up[1],up[2]);
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
void Camera3D::begin(GLfloat& theta,GLfloat& phi) const {
  GLfloat x=_dir.dot(_t1);
  GLfloat y=_dir.dot(_t2);
  GLfloat z=_dir.dot(_up);
  theta=atan2(y,x);
  phi=std::min<GLfloat>(M_PI/2*0.99,std::max<GLfloat>(-M_PI/2*0.99,atan2(z,std::sqrt(x*x+y*y))));
}
void Camera3D::end(GLfloat theta,GLfloat phi) {
  phi=std::min<GLfloat>(M_PI/2*0.99,std::max<GLfloat>(-M_PI/2*0.99,phi));
  GLfloat x=std::cos(theta)*std::cos(phi);
  GLfloat y=std::sin(theta)*std::cos(phi);
  GLfloat z=std::sin(phi);
  _dir=_t1*x+_t2*y+_up*z;
}
GLfloat Camera3D::clampMin(GLfloat val) {
  static GLfloat minVal=3;
  if(val>0)
    return (val<minVal)?0:val;
  else return (val>-minVal)?0:val;
}

void Camera3D::setPosition(Eigen::Matrix<GLfloat, 3, 1> pos){
    _pos = pos;
}
void Camera3D::setDirection(Eigen::Matrix<GLfloat, 3, 1> dir) {
    _dir = dir;
}
void Camera3D::setUp(Eigen::Matrix<GLfloat, 3, 1> up) {
    _up = up;
}

}
