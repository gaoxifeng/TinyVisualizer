#include "DefaultLight.h"

namespace DRAWER {
const std::string DefaultLightVert=
  "#version 330 core\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "out vec2 tc;\n"
  "void main(void)\n"
  "{\n"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "  tc=TexCoord;\n"
  "}\n";
const std::string DefaultLightFrag=
  "#version 330 core\n"
  "uniform vec4 diffuse;\n"
  "uniform bool useTexture;\n"
  "uniform sampler2D diffuseMap;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  if(useTexture)\n"
  "    FragColor=diffuse*texture(diffuseMap,tc);\n"
  "  else FragColor=diffuse;\n"
  "}\n";
std::shared_ptr<Program> defaultLightProg;
std::shared_ptr<Program> getDefaultLightProg() {
  if(!defaultLightProg) {
    Shader::registerShader("DefaultLight",DefaultLightVert,"",DefaultLightFrag);
    Program::registerProgram("DefaultLight","DefaultLight","","DefaultLight");
    defaultLightProg=Program::findProgram("DefaultLight");
  }
  return defaultLightProg;
}
const std::string DebugDrawTexCoordFrag=
  "#version 330 core\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  FragColor=vec4(tc,0,1);\n"
  "}\n";
std::shared_ptr<Program> debugDrawTexCoordProg;
std::shared_ptr<Program> getDebugDrawTexCoordProg() {
  if(!debugDrawTexCoordProg) {
    Shader::registerShader("DebugDrawTexCoord","","",DebugDrawTexCoordFrag);
    Program::registerProgram("DebugDrawTexCoord","DefaultLight","","DebugDrawTexCoord");
    debugDrawTexCoordProg=Program::findProgram("DebugDrawTexCoord");
  }
  return debugDrawTexCoordProg;
}
const std::string RoundPointVert=
  "#version 330 core\n"
  "uniform float pointSize;\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "out vec2 tc;\n"
  "void main(void)\n"
  "{\n"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "  gl_PointSize=pointSize;\n"
  "  tc=TexCoord;\n"
  "}\n";
const std::string RoundPointFrag=
  "#version 330 core\n"
  "uniform vec4 diffuse;\n"
  "uniform bool useTexture;\n"
  "uniform sampler2D diffuseMap;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  vec2 circCoord=2.0*gl_PointCoord-1.0;\n"
  "  if(dot(circCoord,circCoord)>1.0)\n"
  "     discard;\n"
  "  if(useTexture)\n"
  "    FragColor=diffuse*texture(diffuseMap,tc);\n"
  "  else FragColor=diffuse;\n"
  "}\n";
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
std::string thickLineVert=
  "#version 330 core\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "void main(void) {"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "}\n";
std::string thickLineGeom=
  "#version 330 core\n"
  "uniform float sz;\n"
  "layout(lines) in;\n"
  "layout(triangle_strip,max_vertices=18) out;\n"
  "flat out vec2 v0,v1;\n"
  "out vec2 v;\n"
  "void emit_quad(vec4 vv0,vec4 vv1,vec4 a,vec4 b,vec4 c,vec4 d) {\n"
  "  gl_Position=a;\n"
  "  v=a.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  gl_Position=b;\n"
  "  v=b.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  gl_Position=c;\n"
  "  v=c.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  EndPrimitive();\n"
  "  \n"
  "  gl_Position=a;\n"
  "  v=a.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  gl_Position=c;\n"
  "  v=c.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  gl_Position=d;\n"
  "  v=d.xy;\n"
  "  v0=vv0.xy;\n"
  "  v1=vv1.xy;\n"
  "  EmitVertex();\n"
  "  EndPrimitive();\n"
  "}\n"
  "void main(void) {\n"
  "  vec4 a = gl_in[0].gl_Position;\n"
  "  vec4 b = gl_in[1].gl_Position;\n"
  "  if(a.x>b.x) {\n"
  "    vec4 tmp = a;\n"
  "    a = b;\n"
  "    b = tmp;\n"
  "  }\n"
  "  a=vec4(a.xyz/a.w,1);\n"
  "  b=vec4(b.xyz/b.w,1);\n"
  "  emit_quad(a,b,a+vec4(-sz,-sz,0,0),a+vec4( sz,-sz,0,0),a+vec4( sz, sz,0,0),a+vec4(-sz, sz,0,0));\n"
  "  emit_quad(a,b,b+vec4(-sz,-sz,0,0),b+vec4( sz,-sz,0,0),b+vec4( sz, sz,0,0),b+vec4(-sz, sz,0,0));\n"
  "  \n"
  "  vec4 ab=b-a;\n"
  "  float len=length(ab);\n"
  "  if(len<1e-6)\n"
  "    return;\n"
  "  vec2 perp=vec2(-ab.y,ab.x)*sz/len;\n"
  "  emit_quad(a,b,a-vec4(perp.xy,0,0),b-vec4(perp.xy,0,0),b+vec4(perp.xy,0,0),a+vec4(perp.xy,0,0));\n"
  "}\n";
std::string thickLineFrag=
  "#version 330 core\n"
  "uniform vec4 color0,color1;\n"
  "flat in vec2 v0,v1;\n"
  "uniform float sz;\n"
  "in vec2 v;\n"
  "out vec4 FragColor;\n"
  "void main (void) {\n"
  "  vec2 v01=v1-v0;\n"
  "  vec2 dir=v-v0;\n"
  "  float len=length(v01),dist=0;\n"
  "  if(len<1e-6) {\n"
  "    dist=length(dir);\n"
  "  } else {\n"
  "    v01/=len;\n"
  "    float vd=dot(v01,dir);\n"
  "    dist=length(dir-vd*v01);\n"
  "  }\n"
  "  if(dist>sz)\n"
  "    discard;\n"
  "  dist/=sz;\n"
  "  FragColor=color0*(1-dist)+color1*dist;\n"
  "}\n";
std::shared_ptr<Program> thickLineProg;
std::shared_ptr<Program> getThickLineProg() {
  if(!thickLineProg) {
    Shader::registerShader("ThickLine",thickLineVert,thickLineGeom,thickLineFrag);
    Program::registerProgram("ThickLine","ThickLine","ThickLine","ThickLine");
    thickLineProg=Program::findProgram("ThickLine");
  }
  return thickLineProg;
}
void setThickLineWidth(GLfloat size) {
  thickLineProg->setUniformFloat("sz",size,false);
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
  if(Program::currentProgram()->getName()=="ShadowLight" && mat._drawer)
    mat._drawer->getLight()->setupLightMaterial(mat);
  else setupMaterial(mat._tex,mat._diffuse);
  if(Program::currentProgram()->getName()=="RoundPoint")
    setRoundPointSize(mat._pointSize);
  if(Program::currentProgram()->getName()=="ThickLine")
    setThickLineWidth(mat._lineWidth);
}
}
