#include "Camera2D.h"
#include "Texture.h"

namespace DRAWER {
//Camera2D
#define MAX_DEPTH 256
Camera2D::Camera2D(GLfloat xExt)
  :_xExt(xExt),_xCtr(0),_yCtr(0),_scale(1),_inMotion(false),_debug(false) {}
void Camera2D::focusOn(std::shared_ptr<Shape> s) {
  _focus=s;
}
void Camera2D::mouse(GLFWwindow* wnd,int button,int action,int) {
  if(button==GLFW_MOUSE_BUTTON_1) {
    if(action==GLFW_PRESS) {
      _inMotion=true;
      glfwGetCursorPos(wnd,&_xLast,&_yLast);
      _xCtrLast=_xCtr;
      _yCtrLast=_yCtr;
    } else if(action==GLFW_RELEASE) {
      _inMotion=false;
    }
  }
}
void Camera2D::wheel(GLFWwindow*,double,double yoffset) {
  _scale*=std::pow(1.1,-yoffset);
}
void Camera2D::motion(GLFWwindow* wnd,double x,double y) {
  if(_inMotion) {
    int w=0,h=0;
    glfwGetWindowSize(wnd,&w,&h);
    GLfloat coef=_xExt*_scale/(GLfloat)w;
    _xCtr=_xCtrLast-(x-_xLast)*coef;
    _yCtr=_yCtrLast+(y-_yLast)*coef;
  }
}
void Camera2D::key(GLFWwindow* wnd,int key,int scan,int action,int mods) {
  if(key==GLFW_KEY_H && action==GLFW_PRESS) {
    _inMotion=false;
    _xCtr=_yCtr=0;
    _scale=1;
  } else if(key==GLFW_KEY_V && action==GLFW_PRESS)
    _debugFrustum=constructViewFrustum2D();
  else if(key==GLFW_KEY_B && action==GLFW_PRESS) {
    _debug=!_debug;
  }
}
void Camera2D::draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>&) {
  if(_focus) {
    Eigen::Matrix<GLfloat,6,1> bbF=_focus->getBB();
    _xCtr=(bbF[0]+bbF[3])/2;
    _yCtr=(bbF[1]+bbF[4])/2;
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  int w=0,h=0;
  glfwGetWindowSize(wnd,&w,&h);
  _yExt=_xExt*(GLfloat)h/(GLfloat)w;
  glOrtho(_xCtr-_xExt*_scale,_xCtr+_xExt*_scale,
          _yCtr-_yExt*_scale,_yCtr+_yExt*_scale,0,MAX_DEPTH);

  if(_tex) {
    glActiveTexture(GL_TEXTURE0);
    _tex->begin();
    glActiveTexture(GL_TEXTURE1);

    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f((_xCtr-_xExt*_scale)*_tcMult[0],(_yCtr-_yExt*_scale)*_tcMult[1]);
    glVertex3f(_xCtr-_xExt*_scale,_yCtr-_yExt*_scale,-(MAX_DEPTH-1));

    glColor3f(1,1,1);
    glTexCoord2f((_xCtr+_xExt*_scale)*_tcMult[0],(_yCtr-_yExt*_scale)*_tcMult[1]);
    glVertex3f(_xCtr+_xExt*_scale,_yCtr-_yExt*_scale,-(MAX_DEPTH-1));

    glColor3f(1,1,1);
    glTexCoord2f((_xCtr+_xExt*_scale)*_tcMult[0],(_yCtr+_yExt*_scale)*_tcMult[1]);
    glVertex3f(_xCtr+_xExt*_scale,_yCtr+_yExt*_scale,-(MAX_DEPTH-1));

    glColor3f(1,1,1);
    glTexCoord2f((_xCtr-_xExt*_scale)*_tcMult[0],(_yCtr+_yExt*_scale)*_tcMult[1]);
    glVertex3f(_xCtr-_xExt*_scale,_yCtr+_yExt*_scale,-(MAX_DEPTH-1));
    glEnd();

    _tex->end();
    glActiveTexture(GL_TEXTURE0);
  }

  if(_debug) {
    GLfloat delta=0.3f;
    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex2f(-_scale,0);
    glVertex2f( _scale,0);
    glVertex2f( _scale-_scale*delta,-_scale*delta);
    glVertex2f( _scale,0);
    glVertex2f( _scale-_scale*delta, _scale*delta);
    glVertex2f( _scale,0);
    glColor3f(0,0,1);
    glVertex2f(0,-_scale);
    glVertex2f(0, _scale);
    glVertex2f(-_scale*delta, _scale-_scale*delta);
    glVertex2f(0,_scale);
    glVertex2f( _scale*delta, _scale-_scale*delta);
    glVertex2f(0,_scale);
    glEnd();
    if(_debugFrustum.size()==8) {
      glColor3f(1,0,0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      drawViewFrustum2D(_debugFrustum);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
  }
}
Eigen::Matrix<GLfloat,-1,1> Camera2D::getCameraRay(GLFWwindow* wnd,double x,double y) const {
  Eigen::Matrix<GLfloat,2,1> ret;
  int w=0,h=0;
  glfwGetWindowSize(wnd,&w,&h);
  ret[0]=interp1D<GLfloat,GLfloat>(_xCtr-_xExt*_scale,_xCtr+_xExt*_scale,x/(GLfloat)w);
  ret[1]=interp1D<GLfloat,GLfloat>(_yCtr-_yExt*_scale,_yCtr+_yExt*_scale,1-y/(GLfloat)h);
  return ret;
}
Eigen::Matrix<GLfloat,-1,1> Camera2D::getViewFrustum() const {
  return getViewFrustum2DPlanes();
}
void Camera2D::setTexture(std::shared_ptr<Texture> tex,const Eigen::Matrix<GLfloat,2,1>& tcMult) {
  _tex=tex;
  _tcMult=tcMult;
}
}
