#include "DefaultLight.h"

namespace DRAWER {
#include "Shader/DefaultVert.h"
#include "Shader/DefaultFrag.h"
std::shared_ptr<Program> defaultProg;
std::shared_ptr<Program> getDefaultProg() {
  if(!defaultProg) {
    Shader::registerShader("Default",DefaultVert,"",DefaultFrag);
    Program::registerProgram("Default","Default","","Default");
    defaultProg=Program::findProgram("Default");
  }
  return defaultProg;
}
#include "Shader/DebugDrawTexCoordFrag.h"
std::shared_ptr<Program> debugDrawTexCoordProg;
std::shared_ptr<Program> getDebugDrawTexCoordProg() {
  if(!debugDrawTexCoordProg) {
    Shader::registerShader("DebugDrawTexCoord","","",DebugDrawTexCoordFrag);
    Program::registerProgram("DebugDrawTexCoord","Default","","DebugDrawTexCoord");
    debugDrawTexCoordProg=Program::findProgram("DebugDrawTexCoord");
  }
  return debugDrawTexCoordProg;
}
#include "Shader/RoundPointVert.h"
#include "Shader/RoundPointFrag.h"
std::shared_ptr<Program> roundPointProg;
std::shared_ptr<Program> getRoundPointProg() {
  if(!roundPointProg) {
    Shader::registerShader("RoundPoint",RoundPointVert,"",RoundPointFrag);
    Program::registerProgram("RoundPoint","RoundPoint","","RoundPoint");
    roundPointProg=Program::findProgram("RoundPoint");
  }
  return roundPointProg;
}
void setRoundPointSize(GLfloat size) {
  glEnable(GL_PROGRAM_POINT_SIZE);
  roundPointProg->setUniformFloat("pointSize",size,false);
}
#include "Shader/ThickLineVert.h"
#include "Shader/ThickLineGeom.h"
#include "Shader/ThickLineFrag.h"
std::shared_ptr<Program> thickLineProg;
std::shared_ptr<Program> getThickLineProg() {
  if(!thickLineProg) {
    Shader::registerShader("ThickLine",ThickLineVert,ThickLineGeom,ThickLineFrag);
    Program::registerProgram("ThickLine","ThickLine","ThickLine","ThickLine");
    thickLineProg=Program::findProgram("ThickLine");
  }
  return thickLineProg;
}
void setThickLineWidth(GLfloat size) {
  int width=0,height=0;
  GLFWwindow* wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd,&width,&height);
  thickLineProg->setUniformFloat("sz",size/std::max(width,height),false);
}
void setupMaterial(std::shared_ptr<Texture> tex,const Eigen::Matrix<GLfloat,4,1>& diffuse) {
  Program::currentProgram()->setUniformBool("useTexture",tex!=NULL,false);
  Program::currentProgram()->setUniformFloat("diffuse",diffuse,false);
  if(tex!=NULL)
    Program::currentProgram()->setTexUnit("diffuseMap",0,false);
  if(Program::currentProgram()->getName()=="ThickLine") {
    Program::currentProgram()->setUniformFloat("color0",diffuse,false);
    Program::currentProgram()->setUniformFloat("color1",diffuse,false);
  }
}
void setupMaterial(std::shared_ptr<Texture> tex,GLfloat r,GLfloat g,GLfloat b) {
  setupMaterial(tex,Eigen::Matrix<GLfloat,4,1>(r,g,b,1));
}
void setupMaterial(const ShadowLight::Material& mat) {
  if(Program::currentProgram()->getName().find("Light")!=std::string::npos && mat._drawer)
    mat._drawer->getLight()->setupLightMaterial(mat);
  else setupMaterial(mat._tex,mat._diffuse);
  if(Program::currentProgram()->getName()=="RoundPoint")
    setRoundPointSize(mat._pointSize);
  if(Program::currentProgram()->getName()=="ThickLine")
    setThickLineWidth(mat._lineWidth);
}
}
