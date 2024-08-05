#include "Camera2D.h"
#include "Texture.h"
#include "Matrix.h"
#include "DefaultLight.h"
#include "Povray.h"
#include "VBO.h"

namespace DRAWER {
//Camera2D
#define MAX_DEPTH 256
Camera2D::Camera2D(GLfloat xExt)
  :_xExt(xExt),_xCtr(0),_yCtr(0),_scale(1),_inMotion(false),_debug(false) {}
void Camera2D::focusOn(std::shared_ptr<Shape> s) {
  _focus=s;
}
void Camera2D::mouse(GLFWwindowPtr wnd,int button,int action,int,bool captured) {
  if(captured)
    return;
  else if(button==GLFW_MOUSE_BUTTON_1) {
    if(action==GLFW_PRESS) {
      _inMotion=true;
      glfwGetCursorPos(wnd._ptr,&_xLast,&_yLast);
      _xCtrLast=_xCtr;
      _yCtrLast=_yCtr;
    } else if(action==GLFW_RELEASE) {
      _inMotion=false;
    }
  }
}
void Camera2D::wheel(GLFWwindowPtr,double,double yoffset,bool captured) {
  if(captured)
    return;
  else _scale*=(GLfloat)std::pow(1.1,-yoffset);
}
void Camera2D::motion(GLFWwindowPtr wnd,double x,double y,bool captured) {
  if(captured)
    return;
  else if(_inMotion) {
    GLfloat coef=2*_xExt*_scale/(GLfloat)_vp[2];
    _xCtr=_xCtrLast-(GLfloat)(x-_xLast)*coef;
    _yCtr=_yCtrLast+(GLfloat)(y-_yLast)*coef;
  }
}
void Camera2D::key(GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) {
  if(captured)
    return;
  else if(key==GLFW_KEY_H && action==GLFW_PRESS) {
    _inMotion=false;
    _xCtr=_yCtr=0;
    _scale=1;
  } else if(key==GLFW_KEY_V && action==GLFW_PRESS) {
    loadCamera();
    _debugFrustum=constructViewFrustum2D();
  } else if(key==GLFW_KEY_B && action==GLFW_PRESS) {
    _debug=!_debug;
  }
}
void Camera2D::draw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>&) {
  if(_focus) {
    Eigen::Matrix<GLfloat,6,1> bbF=_focus->getBB();
    _xCtr=(bbF[0]+bbF[3])/2;
    _yCtr=(bbF[1]+bbF[4])/2;
  }
  matrixMode(GLModelViewMatrix);
  loadIdentity();
  glGetIntegerv(GL_VIEWPORT,_vp);
  _yExt=_xExt*(GLfloat)_vp[3]/(GLfloat)_vp[2];
  orthof(_xCtr-_xExt*_scale,_xCtr+_xExt*_scale,
         _yCtr-_yExt*_scale,_yCtr+_yExt*_scale,0,MAX_DEPTH);
  matrixMode(GLProjectionMatrix);
  loadIdentity();
  saveCamera();

  if(_debug) {
    getDefaultProg()->begin();
    setupMatrixInShader();
    GLfloat delta=0.3f;
    setupMaterial(NULL,1,0,0);
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(-_scale,0,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>( _scale,0,-(MAX_DEPTH-1))
    );
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>( _scale-_scale*delta,-_scale*delta,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>( _scale,0,-(MAX_DEPTH-1))
    );
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>( _scale-_scale*delta, _scale*delta,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>( _scale,0,-(MAX_DEPTH-1))
    );
    setupMaterial(NULL,0,0,1);
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(0,-_scale,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>(0, _scale,-(MAX_DEPTH-1))
    );
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>(-_scale*delta, _scale-_scale*delta,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>(0,_scale,-(MAX_DEPTH-1))
    );
    drawLinef(
      Eigen::Matrix<GLfloat,3,1>( _scale*delta, _scale-_scale*delta,-(MAX_DEPTH-1)),
      Eigen::Matrix<GLfloat,3,1>(0,_scale,-(MAX_DEPTH-1))
    );
    Program::currentProgram()->end();
    if(_debugFrustum.size()==8) {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      drawViewFrustum2D(_debugFrustum,Eigen::Matrix<GLfloat,4,1>(1,0,0,1));
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
  }
}
void Camera2D::drawPovray(Povray& pov,GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  draw(wnd,bb);

  Eigen::Matrix<GLfloat,3,1> pos(_xCtr,_yCtr,1);
  Eigen::Matrix<GLfloat,3,1> dir(0,0,-1);
  Eigen::Matrix<GLfloat,3,1> up(0,_yExt*_scale*2,0);
  Eigen::Matrix<GLfloat,3,1> right(_xExt*_scale*2,0,0);

  std::shared_ptr<Povray::Camera> c(new Povray::Camera);
  c->_pos=pos;
  c->_dir=dir.normalized();
  c->_up=up;
  c->_right=right;
  pov.addElement(c);
}
Eigen::Matrix<GLfloat,-1,1> Camera2D::getCameraRay(GLFWwindowPtr wnd,double x,double y) const {
  int w,h;
  Eigen::Matrix<GLfloat,2,1> ret;
  glfwGetWindowSize(wnd._ptr,&w,&h);
  ret[0]=interp1D<GLfloat,GLfloat>(_xCtr-_xExt*_scale,_xCtr+_xExt*_scale,(GLfloat)(x-_vp[0])/(GLfloat)_vp[2]);
  ret[1]=interp1D<GLfloat,GLfloat>(_yCtr-_yExt*_scale,_yCtr+_yExt*_scale,(GLfloat)(h-y-_vp[1])/(GLfloat)_vp[3]);
  return ret;
}
Eigen::Matrix<GLfloat,-1,1> Camera2D::getViewFrustum() const {
  return getViewFrustum2DPlanes();
}
}
