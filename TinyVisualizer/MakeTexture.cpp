#include "MakeTexture.h"

namespace DRAWER {
std::shared_ptr<Texture> drawTexture
(std::function<void()> func,int levelMin,int levelMax,GLenum formatColor) {
  FBOPingPong FBOPP(levelMin,levelMax,formatColor);
  FBOPP.begin();
  FBOPP.getFBO(FBOPP.nrFBO()-1).drawScreenQuad(func);
  FBOPP.end();
  return FBOPP.getFBO(0).getRBORef();
}
std::shared_ptr<Texture> drawChecker
(int density,
 Eigen::Matrix<GLfloat,3,1> c0,
 Eigen::Matrix<GLfloat,3,1> c1,
 int levelMin,int levelMax,GLenum formatColor) {
  return drawTexture([&]() {
    glBegin(GL_QUADS);
    for(int x=0; x<density; x++)
      for(int y=0; y<density; y++) {
        if(((x+y)%2)==0)
          glColor3f(c0[0],c0[1],c0[2]);
        else glColor3f(c1[0],c1[1],c1[2]);
        GLfloat xf0=-1+2*x/(GLfloat)density,xf1=xf0+2/(GLfloat)density;
        GLfloat yf0=-1+2*y/(GLfloat)density,yf1=yf0+2/(GLfloat)density;
        glVertex2f(xf0,yf0);
        glVertex2f(xf1,yf0);
        glVertex2f(xf1,yf1);
        glVertex2f(xf0,yf1);
      }
    glEnd();
  },levelMin,levelMax,formatColor);
}
std::shared_ptr<Texture> drawGrid
(int density,GLfloat t0,GLfloat t1,
 Eigen::Matrix<GLfloat,3,1> c0,
 Eigen::Matrix<GLfloat,3,1> c1,
 int levelMin,int levelMax,GLenum formatColor) {
  return drawTexture([&]() {
    //background
    glBegin(GL_QUADS);
    glColor3f(c0[0],c0[1],c0[2]);
    glVertex2f(-1,-1);
    glVertex2f( 1,-1);
    glVertex2f( 1, 1);
    glVertex2f(-1, 1);
    glEnd();
    //line
    glBegin(GL_QUADS);
    glColor3f(c1[0],c1[1],c1[2]);
    for(int x=0; x<=density; x++) {
      GLfloat D=(x==0||x==density)?t1:t0;
      glVertex2f(-1+2*x/(GLfloat)density-D,-1);
      glVertex2f(-1+2*x/(GLfloat)density+D,-1);
      glVertex2f(-1+2*x/(GLfloat)density+D, 1);
      glVertex2f(-1+2*x/(GLfloat)density-D, 1);
    }
    for(int y=0; y<=density; y++) {
      GLfloat D=(y==0||y==density)?t1:t0;
      glVertex2f(-1,-1+2*y/(GLfloat)density-D);
      glVertex2f( 1,-1+2*y/(GLfloat)density-D);
      glVertex2f( 1,-1+2*y/(GLfloat)density+D);
      glVertex2f(-1,-1+2*y/(GLfloat)density+D);
    }
    glEnd();
  },levelMin,levelMax,formatColor);
}
std::shared_ptr<Texture> getWhiteTexture() {
  static std::shared_ptr<Texture> tex=drawTexture([&]() {
    //background
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glVertex2f(-1,-1);
    glVertex2f( 1,-1);
    glVertex2f( 1, 1);
    glVertex2f(-1, 1);
    glEnd();
  },1,1,GL_RGB);
  return tex;
}
}
