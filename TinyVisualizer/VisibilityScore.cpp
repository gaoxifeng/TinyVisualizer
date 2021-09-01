#include "VisibilityScore.h"
#include "SceneStructure.h"
#include "MakeMesh.h"
#include "Camera3D.h"
#include <iostream>

namespace DRAWER {
const std::string XORFrag=
  "#version 410 compatibility\n"
  "uniform sampler2D tex[2];\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  float aTexVal=texture2D(tex[0],gl_TexCoord[0].xy).x;\n"
  "  float bTexVal=texture2D(tex[1],gl_TexCoord[0].xy).x;\n"
  "  float xor=max(0,aTexVal-bTexVal)+max(0,bTexVal-aTexVal);\n"
  "  FragColor=vec4(xor,xor,xor,1);\n"
  "}\n";
const std::string TexCopyFrag=
  "#version 410 compatibility\n"
  "uniform sampler2D tex;\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  float aTexVal=texture2D(tex,gl_TexCoord[0].xy).x;\n"
  "  FragColor=vec4(aTexVal,aTexVal,aTexVal,1);\n"
  "}\n";
VisibilityScore::VisibilityScore(int levelMax,GLenum formatColor,GLenum formatDepth)
  :_shaderXOR("","",XORFrag),_shaderTexCopy("","",TexCopyFrag),
   _fboPP(0,levelMax,formatColor,formatDepth),
   _fboRef(1<<levelMax,1<<levelMax,formatColor,formatDepth) {}
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
  XOR.drawScreenQuad();
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
    shapeA->draw(false);
  };
  std::function<void(const FBO&)> curr=[&](const FBO&) {
    drawer.getCamera()->draw(glfwGetCurrentContext(),bb);
    shapeB->draw(false);
  };
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
    glColor3f(1,1,1);
    A.drawScreenQuad(-0.75,-0.75,0.5,0.5);
  };
  std::function<void(const FBO&)> curr=[](const FBO& B) {
    glColor3f(1,1,1);
    B.drawScreenQuad(-0.5,-0.5,0.75,0.75);
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
  _shaderXOR.begin();
  glActiveTexture(GL_TEXTURE0);
  A.getRBO().begin();
  _shaderXOR.setTexUnit("tex[0]",0);
  glActiveTexture(GL_TEXTURE1);
  B.getRBO().begin();
  _shaderXOR.setTexUnit("tex[1]",1);
  glActiveTexture(GL_TEXTURE2);
}
void VisibilityScore::endXOR() {
  const FBO& A=_fboRef;
  const FBO& B=_fboPP.getFBO(_fboPP.nrFBO()-1);
  _shaderXOR.end();
  A.getRBO().end();
  B.getRBO().end();
  glActiveTexture(GL_TEXTURE0);
}
void VisibilityScore::texCopy(int i) {
  const FBO& from=_fboPP.getFBO(i+1);
  const FBO& to=_fboPP.getFBO(i);
  _shaderTexCopy.begin();
  glActiveTexture(GL_TEXTURE0);
  from.getRBO().begin();
  _shaderTexCopy.setTexUnit("tex",0);
  glActiveTexture(GL_TEXTURE1);
  to.begin();
  to.drawScreenQuad();
  to.end();
  from.getRBO().end();
  _shaderTexCopy.end();
  glActiveTexture(GL_TEXTURE0);
}
}
