#include "Background.h"
#include "Matrix.h"
#include "Texture.h"
#include "DefaultLight.h"

namespace DRAWER {
void Background::draw() {
  glDisable(GL_DEPTH_TEST);
  matrixMode(GLModelViewMatrix);
  loadIdentity();
  orthof(0,1,0,1,0,1);
  matrixMode(GLProjectionMatrix);
  loadIdentity();
  getDefaultProg()->begin();
  setupMaterial(_tex,_color[0],_color[1],_color[2]);
  setupMatrixInShader();
  if(_tex) {
    glActiveTexture(GL_TEXTURE0);
    _tex->begin();
    glActiveTexture(GL_TEXTURE1);
  }
  drawQuadf(
    Eigen::Matrix<GLfloat,2,1>(0*_tcMult[0],0*_tcMult[1]),
    Eigen::Matrix<GLfloat,2,1>(0,0),
    Eigen::Matrix<GLfloat,2,1>(1*_tcMult[0],0*_tcMult[1]),
    Eigen::Matrix<GLfloat,2,1>(1,0),
    Eigen::Matrix<GLfloat,2,1>(1*_tcMult[0],1*_tcMult[1]),
    Eigen::Matrix<GLfloat,2,1>(1,1),
    Eigen::Matrix<GLfloat,2,1>(0*_tcMult[0],1*_tcMult[1]),
    Eigen::Matrix<GLfloat,2,1>(0,1));
  if(_tex) {
    _tex->end();
    glActiveTexture(GL_TEXTURE0);
  }
  Program::currentProgram()->end();
  glEnable(GL_DEPTH_TEST);
}
}
