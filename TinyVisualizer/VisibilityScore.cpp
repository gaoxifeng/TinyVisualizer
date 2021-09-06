#include "VisibilityScore.h"
#include "SceneStructure.h"
#include "MakeMesh.h"
#include "Camera3D.h"
#include "Matrix.h"
#include "DefaultLight.h"
#include <iostream>

namespace DRAWER {
#include "Shader/XORFrag.h"
#include "Shader/TexCopyFrag.h"
VisibilityScore::VisibilityScore(int levelMax,GLenum formatColor,GLenum formatDepth)
  :_fboPP(0,levelMax,formatColor,formatDepth),
   _fboRef(1<<levelMax,1<<levelMax,formatColor,formatDepth) {
  if(!_XORProg) {
    getDefaultLightProg();
    Shader::registerShader("XOR","","",XORFrag);
    Program::registerProgram("XOR","DefaultLight","","XOR");
    _XORProg=Program::findProgram("XOR");

    Shader::registerShader("TexCopy","","",TexCopyFrag);
    Program::registerProgram("TexCopy","DefaultLight","","TexCopy");
    _texCopyProg=Program::findProgram("TexCopy");
  }
}
Eigen::Matrix<GLfloat,2,1> VisibilityScore::compute
(std::function<void(const FBO&)>* ref,
 std::function<void(const FBO&)>* curr,
 bool debugOutput) {
  GLfloat cc[4];
  glGetFloatv(GL_COLOR_CLEAR_VALUE,cc);
  glClearColor(0,0,0,0);
  //render A
  if(ref) {
    const FBO& A=_fboRef;
    A.begin();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    (*ref)(A);
    A.end();
    if(debugOutput)
      A.saveImage("A.png");
  }
  //render B
  if(curr) {
    const FBO& B=_fboPP.getFBO(_fboPP.nrFBO()-1);
    B.begin();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    (*curr)(B);
    B.end();
    if(debugOutput)
      B.saveImage("B.png");
  }
  //render XOR
  const FBO& XOR=_fboPP.getFBO(_fboPP.nrFBO()-2);
  beginXOR();
  XOR.begin();
  XOR.drawScreenQuad([&]() {
    setupMatrixInShader();
    XOR.screenQuad();
  });
  XOR.end();
  endXOR();
  if(debugOutput)
    XOR.saveImage("XOR.png");
  for(int i=_fboPP.nrFBO()-3; i>=0; i--) {
    texCopy(i);
    if(debugOutput)
      _fboPP.getFBO(i).saveImage("XOR"+std::to_string(i)+".png");
  }
  GLfloat s=_fboPP.getFBO(0).getPixel00();
  Eigen::Matrix<GLfloat,2,1> ret(s,s*_fboRef.width()*_fboRef.height());
  if(debugOutput)
    std::cout << "Visibility Score=" << ret.transpose() << std::endl;
  glClearColor(cc[0],cc[1],cc[2],cc[3]);
  return ret;
}
std::pair<Eigen::Matrix<GLfloat,2,1>,Eigen::Matrix<GLfloat,2,1>> VisibilityScore::compute
    (Drawer& drawer,const Eigen::Vector3f& up,
     const std::vector<Eigen::Matrix<GLfloat,3,1>>& eyes,
     const std::vector<Eigen::Matrix<GLfloat,3,1>>& dirs,
     std::shared_ptr<MeshShape> shapeA,std::shared_ptr<MeshShape> shapeB,
bool debugOutput) {
  Eigen::Matrix<GLfloat,6,1> bb=unionBB(shapeA->getBB(),shapeB->getBB());
  std::function<void(const FBO&)> ref=[&](const FBO&) {
    drawer.getCamera()->draw(glfwGetCurrentContext(),bb);
    shapeA->setDrawer(&drawer);
    getDefaultLightProg()->begin();
    shapeA->draw(Shape::MESH_PASS);
    Program::currentProgram()->end();
  };
  std::function<void(const FBO&)> curr=[&](const FBO&) {
    drawer.getCamera()->draw(glfwGetCurrentContext(),bb);
    shapeB->setDrawer(&drawer);
    getDefaultLightProg()->begin();
    shapeB->draw(Shape::MESH_PASS);
    Program::currentProgram()->end();
  };
  drawer.clearLight();  //force the use of default light
  Eigen::Matrix<GLfloat,2,1> maxS=Eigen::Matrix<GLfloat,2,1>::Zero();
  Eigen::Matrix<GLfloat,2,1> avgS=Eigen::Matrix<GLfloat,2,1>::Zero();
  for(int i=0; i<(int)eyes.size(); i++) {
    drawer.addCamera3D(90,up,eyes[i],dirs[i]);
    Eigen::Matrix<GLfloat,2,1> ret=compute(&ref,&curr,debugOutput);
    maxS=maxS.cwiseMax(ret);
    avgS+=ret;
  }
  avgS/=(int)eyes.size();
  if(debugOutput)
    std::cout << "Visibility score: ave=" << avgS.transpose() << " max=" << maxS.transpose() << "!" << std::endl;
  return std::make_pair(avgS,maxS);
}
void VisibilityScore::debugVisibility() {
  std::function<void(const FBO&)> ref=[](const FBO& A) {
    A.drawScreenQuad(NULL,-0.75,-0.75,0.5,0.5);
  };
  std::function<void(const FBO&)> curr=[](const FBO& B) {
    B.drawScreenQuad(NULL,-0.5,-0.5,0.75,0.75);
  };
  compute(&ref,&curr,true);
}
void VisibilityScore::debugVisibility(Drawer& drawer) {
  Eigen::Matrix<GLfloat,3,1> up(0,0,1);
  Eigen::Matrix<GLfloat,3,1> eye(2,2,2);
  Eigen::Matrix<GLfloat,3,1> dir(-1,-1,-1);
  std::shared_ptr<MeshShape> shapeA=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(1,1,1));
  std::shared_ptr<MeshShape> shapeB=makeBox(1,true,Eigen::Matrix<GLfloat,3,1>(.8,.8,.8));
  compute(drawer,up, {eye}, {dir},shapeA,shapeB,true);
}
void VisibilityScore::beginXOR() {
  const FBO& A=_fboRef;
  const FBO& B=_fboPP.getFBO(_fboPP.nrFBO()-1);
  _XORProg->begin();
  glActiveTexture(GL_TEXTURE0);
  A.getRBO().begin();
  _XORProg->setTexUnit("tex[0]",0);
  glActiveTexture(GL_TEXTURE1);
  B.getRBO().begin();
  _XORProg->setTexUnit("tex[1]",1);
  glActiveTexture(GL_TEXTURE2);
}
void VisibilityScore::endXOR() {
  const FBO& A=_fboRef;
  const FBO& B=_fboPP.getFBO(_fboPP.nrFBO()-1);
  Program::currentProgram()->end();
  A.getRBO().end();
  B.getRBO().end();
  glActiveTexture(GL_TEXTURE0);
}
void VisibilityScore::texCopy(int i) {
  const FBO& from=_fboPP.getFBO(i+1);
  const FBO& to=_fboPP.getFBO(i);
  _texCopyProg->begin();
  glActiveTexture(GL_TEXTURE0);
  from.getRBO().begin();
  _texCopyProg->setTexUnit("tex",0);
  glActiveTexture(GL_TEXTURE1);
  to.begin();
  to.drawScreenQuad([&]() {
    setupMatrixInShader();
    to.screenQuad();
  });
  to.end();
  from.getRBO().end();
  Program::currentProgram()->end();
  glActiveTexture(GL_TEXTURE0);
}
std::shared_ptr<Program> VisibilityScore::_XORProg;
std::shared_ptr<Program> VisibilityScore::_texCopyProg;
}
