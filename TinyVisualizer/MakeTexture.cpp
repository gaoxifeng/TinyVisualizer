#include "MakeTexture.h"
#include "DefaultLight.h"
#include "Matrix.h"
#include "VBO.h"

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
    getDefaultLightProg()->begin();
    setupMatrixInShader();
    for(int x=0; x<density; x++)
      for(int y=0; y<density; y++) {
        if(((x+y)%2)==0)
          setupMaterial(NULL,c0[0],c0[1],c0[2]);
        else setupMaterial(NULL,c1[0],c1[1],c1[2]);
        GLfloat xf0=-1+2*x/(GLfloat)density,xf1=xf0+2/(GLfloat)density;
        GLfloat yf0=-1+2*y/(GLfloat)density,yf1=yf0+2/(GLfloat)density;
        drawQuadf(
          Eigen::Matrix<GLfloat,3,1>(xf0,yf0,0),
          Eigen::Matrix<GLfloat,3,1>(xf1,yf0,0),
          Eigen::Matrix<GLfloat,3,1>(xf1,yf1,0),
          Eigen::Matrix<GLfloat,3,1>(xf0,yf1,0)
        );
      }
    Program::currentProgram()->end();
  },levelMin,levelMax,formatColor);
}
std::shared_ptr<Texture> drawGrid
(int density,GLfloat t0,GLfloat t1,
 Eigen::Matrix<GLfloat,3,1> c0,
 Eigen::Matrix<GLfloat,3,1> c1,
 int levelMin,int levelMax,GLenum formatColor) {
  return drawTexture([&]() {
    getDefaultLightProg()->begin();
    setupMatrixInShader();
    //background
    setupMaterial(NULL,c0[0],c0[1],c0[2]);
    drawQuadf(
      Eigen::Matrix<GLfloat,2,1>(-1,-1),
      Eigen::Matrix<GLfloat,2,1>( 1,-1),
      Eigen::Matrix<GLfloat,2,1>( 1, 1),
      Eigen::Matrix<GLfloat,2,1>(-1, 1)
    );
    //line
    setupMaterial(NULL,c1[0],c1[1],c1[2]);
    for(int x=0; x<=density; x++) {
      GLfloat D=(x==0||x==density)?t1:t0;
      drawQuadf(
        Eigen::Matrix<GLfloat,2,1>(-1+2*x/(GLfloat)density-D,-1),
        Eigen::Matrix<GLfloat,2,1>(-1+2*x/(GLfloat)density+D,-1),
        Eigen::Matrix<GLfloat,2,1>(-1+2*x/(GLfloat)density+D, 1),
        Eigen::Matrix<GLfloat,2,1>(-1+2*x/(GLfloat)density-D, 1)
      );
    }
    for(int y=0; y<=density; y++) {
      GLfloat D=(y==0||y==density)?t1:t0;
      drawQuadf(
        Eigen::Matrix<GLfloat,2,1>(-1,-1+2*y/(GLfloat)density-D),
        Eigen::Matrix<GLfloat,2,1>( 1,-1+2*y/(GLfloat)density-D),
        Eigen::Matrix<GLfloat,2,1>( 1,-1+2*y/(GLfloat)density+D),
        Eigen::Matrix<GLfloat,2,1>(-1,-1+2*y/(GLfloat)density+D)
      );
    }
    Program::currentProgram()->end();
  },levelMin,levelMax,formatColor);
}
std::shared_ptr<Texture> getWhiteTexture() {
  static std::shared_ptr<Texture> tex=drawTexture([&]() {
    getDefaultLightProg()->begin();
    setupMatrixInShader();
    setupMaterial(NULL);
    drawQuadf(
      Eigen::Matrix<GLfloat,2,1>(-1,-1),
      Eigen::Matrix<GLfloat,2,1>( 1,-1),
      Eigen::Matrix<GLfloat,2,1>( 1, 1),
      Eigen::Matrix<GLfloat,2,1>(-1, 1)
    );
    Program::currentProgram()->end();
  },1,1,GL_RGB);
  return tex;
}
}
