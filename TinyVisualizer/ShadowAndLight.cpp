#include "ShadowAndLight.h"
#include "DefaultLight.h"
#include "Matrix.h"
#include "VBO.h"
#include <iostream>

namespace DRAWER {
const std::string ShadowLightVert=
  "#version 330 core\n"
  "uniform mat3 normalMatrix;\n"
  "uniform mat4 modelViewMatrix;\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "uniform mat4 invModelViewMatrixShadow;\n"
  "uniform int MAX_LIGHTS;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "out vec3 vN,v,v0;\n"
  "out vec2 tc;\n"
  "void main(void)\n"
  "{\n"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "  vN=normalize(normalMatrix*Normal);\n"
  "  vec4 vMV=modelViewMatrix*vec4(Vertex,1);\n"
  "  v=vec3(vMV);\n"
  "  v0=vec3(invModelViewMatrixShadow*vMV);\n"
  "  tc=TexCoord;\n"
  "};\n";
const std::string ShadowLightFrag=
  "#version 330 core\n"
  "#extension GL_NV_shadow_samplers_cube : enable\n"
  "struct LightMaterial {\n"
  "  vec3 positionEye;\n"
  "  vec4 position;\n"
  "  vec4 ambient;\n"
  "  vec4 diffuse;\n"
  "  vec4 specular;\n"
  "  float shininess;\n"
  "};\n"
  "//shadow\n"
  "uniform sampler2D diffuseMap;\n"
  "uniform samplerCube depthMap[16];\n"
  "uniform float far_plane,bias;\n"
  "uniform bool softShadow;\n"
  "uniform bool useShadow;\n"
  "//light\n"
  "uniform float invShadowMapSize;\n"
  "uniform int softShadowPass;\n"
  "uniform LightMaterial lights[16];\n"
  "uniform int MAX_LIGHTS;\n"
  "in vec3 vN,v,v0;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "vec3 gridSamplingDisk[27]=vec3[]\n"
  "(\n"
  "  vec3(1, 1, 1), vec3( 0, 1, 1), vec3(-1, 1, 1),\n"
  "  vec3(1, 0, 1), vec3( 0, 0, 1), vec3(-1, 0, 1),\n"
  "  vec3(1,-1, 1), vec3( 0,-1, 1), vec3(-1,-1, 1),\n"
  "  vec3(1, 1, 0), vec3( 0, 1, 0), vec3(-1, 1, 0),\n"
  "  vec3(1, 0, 0), vec3( 0, 0, 0), vec3(-1, 0, 0),\n"
  "  vec3(1,-1, 0), vec3( 0,-1, 0), vec3(-1,-1, 0),\n"
  "  vec3(1, 1,-1), vec3( 0, 1,-1), vec3(-1, 1,-1),\n"
  "  vec3(1, 0,-1), vec3( 0, 0,-1), vec3(-1, 0,-1),\n"
  "  vec3(1,-1,-1), vec3( 0,-1,-1), vec3(-1,-1,-1)\n"
  ");\n"
  "float calcShadow(vec3 l,int lid)\n"
  "{\n"
  "  float shadow=0.;\n"
  "  float currentDepth=length(v0-l);\n"
  "  float closestDepth=textureCube(depthMap[lid],v0-l).r*far_plane;\n"
  "  if(currentDepth-bias>closestDepth)\n"
  "    shadow=1.;\n"
  "  return shadow;\n"
  "}\n"
  "float calcSoftShadow(vec3 l,int lid)\n"
  "{\n"
  "  float shadow=0.;\n"
  "  float currentDepth=length(v0-l);\n"
  "  vec3 dir=normalize(v0-l);\n"
  "  for(int pass=0;pass<softShadowPass;pass++)\n"
  "    for(int i=0;i<27;++i) {\n"
  "      float closestDepth=textureCube(depthMap[lid],dir+gridSamplingDisk[i]*invShadowMapSize*pass).r*far_plane;   // undo mapping [0;1]\n"
  "      if(currentDepth-bias>closestDepth)\n"
  "        shadow+=1.0;\n"
  "    }\n"
  "  shadow/=27*softShadowPass;\n"
  "  return shadow;\n"
  "}\n"
  "void main(void)\n"
  "{\n"
  "  vec3 N=normalize(vN);\n"
  "  vec4 colorMap=texture2D(diffuseMap,tc);\n"
  "  vec4 finalColor=vec4(0.0,0.0,0.0,0.0);\n"
  "  for (int i=0;i<MAX_LIGHTS;i++) {\n"
  "    vec3 L=normalize(lights[i].positionEye-v);\n"
  "    vec3 E=normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)\n"
  "    vec3 R=normalize(-reflect(L,N));\n"
  "    //calculate Ambient Term:\n"
  "    vec4 Iamb=lights[i].ambient*colorMap;\n"
  "    //calculate Diffuse Term:\n"
  "    vec4 Idiff=lights[i].diffuse*colorMap*max(dot(N,L),0.0);\n"
  "    Idiff=clamp(Idiff,0.0,1.0);\n"
  "    //calculate Specular Term:\n"
  "    vec4 Ispec=lights[i].specular*colorMap*pow(max(dot(R,E),0.0),lights[i].shininess);\n"
  "    Ispec=clamp(Ispec,0.0,1.0);\n"
  "    //shadow\n"
  "    if(useShadow) {\n"
  "      float s;\n"
  "      if(softShadow)\n"
  "        s=calcSoftShadow(lights[i].position.xyz,i);\n"
  "      else s=calcShadow(lights[i].position.xyz,i);\n"
  "      finalColor+=Iamb+(1-s)*(Idiff+Ispec);\n"
  "    } else finalColor+=Iamb+Idiff+Ispec;\n"
  "  }\n"
  "  //write Total Color:\n"
  "  FragColor=finalColor;\n"
  "};\n";
const std::string ShadowVert=
  "#version 330 core\n"
  "uniform mat4 modelViewMatrix;\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "uniform mat4 invModelViewMatrixShadow;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "out vec3 FragPos;\n"
  "void main(void)\n"
  "{\n"
  "  vec4 vMV=modelViewMatrix*vec4(Vertex,1);\n"
  "  FragPos=vec3(invModelViewMatrixShadow*vMV);\n"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "}\n";
const std::string ShadowFrag=
  "#version 330 core\n"
  "uniform vec3 lightPos;\n"
  "uniform float far_plane;\n"
  "in vec3 FragPos;\n"
  "void main() {\n"
  "  float lightDistance=length(FragPos-lightPos);\n"
  "  //map to [0;1] range by dividing by far_plane\n"
  "  lightDistance=lightDistance/far_plane;\n"
  "  //write this as modified depth\n"
  "  gl_FragDepth=lightDistance;\n"
  "}\n";
ShadowLight::ShadowLight(int shadow,int softShadow,bool autoAdjust)
  :_bias(0.1f),_softShadow(softShadow),_autoAdjust(autoAdjust),_shadow(shadow),_lightSz(0) {
  if(!_shadowLightProg) {
    Shader::registerShader("ShadowLight",ShadowLightVert,"",ShadowLightFrag);
    Program::registerProgram("ShadowLight","ShadowLight","","ShadowLight");
    _shadowLightProg=Program::findProgram("ShadowLight");

    Shader::registerShader("Shadow",ShadowVert,"",ShadowFrag);
    Program::registerProgram("Shadow","Shadow","","Shadow");
    _shadowProg=Program::findProgram("Shadow");
  }
}
int ShadowLight::addLight(const Eigen::Matrix<GLfloat,3,1>& pos,
                          const Eigen::Matrix<GLfloat,3,1>& ambient,
                          const Eigen::Matrix<GLfloat,3,1>& diffuse,
                          const Eigen::Matrix<GLfloat,3,1>& specular) {
  Light l;
  l._position.segment<3>(0)=pos;
  l._position[3]=1;
  setMVLight(l);
  l._ambient.segment<3>(0)=ambient;
  l._ambient[3]=1;
  l._diffuse.segment<3>(0)=diffuse;
  l._diffuse[3]=1;
  l._specular.segment<3>(0)=specular;
  l._specular[3]=1;
  if(_shadow>0)
    l._shadowMap.reset(new FBOShadow(_shadow,_shadow));
  _lights.push_back(l);
  ASSERT_MSG(_lights.size()<=16,"At most 16 lights supported!")
  return (int)_lights.size()-1;
}
void ShadowLight::setDefaultLight(Eigen::Matrix<GLfloat,6,1> bb,
                                  const GLfloat relativeDistance,
                                  const Eigen::Matrix<GLfloat,3,1>& ambient,
                                  const Eigen::Matrix<GLfloat,3,1>& diffuse,
                                  const Eigen::Matrix<GLfloat,3,1>& specular) {
  while(_lights.size()>8)
    _lights.pop_back();
  while(_lights.size()<8)
    addLight(Eigen::Matrix<GLfloat,3,1>::Zero(),ambient,diffuse,specular);

  int index=0;
  Eigen::Matrix<GLfloat,3,1> ext=bb.segment<3>(3)-bb.segment<3>(0);
  bb.segment<3>(0)-=ext*relativeDistance;
  bb.segment<3>(3)+=ext*relativeDistance;
  for(GLfloat x: {
        bb[0],bb[3]
      })
    for(GLfloat y: {
          bb[1],bb[4]
        })
      for(GLfloat z: {
            bb[2],bb[5]
          }) {
        setLightPos(index,Eigen::Matrix<GLfloat,3,1>(x,y,z));
        getLightAmbient(index)=ambient;
        getLightDiffuse(index)=diffuse;
        getLightSpecular(index)=specular;
        index++;
      }
}
void ShadowLight::setLightPos(int i,const Eigen::Matrix<GLfloat,3,1>& pos) {
  _lights[i]._position.segment<3>(0)=pos;
  setMVLight(_lights[i]);
}
Eigen::Matrix<GLfloat,3,1> ShadowLight::getLightPos(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(_lights[i]._position.data());
}
void ShadowLight::setLightAmbient(int i,const Eigen::Matrix<GLfloat,3,1>& ambient) {
  _lights[i]._ambient.segment<3>(0)=ambient;
}
Eigen::Matrix<GLfloat,3,1> ShadowLight::getLightAmbient(int i) const {
  return _lights[i]._ambient.segment<3>(0);
}
void ShadowLight::setLightDiffuse(int i,const Eigen::Matrix<GLfloat,3,1>& diffuse) {
  _lights[i]._diffuse.segment<3>(0)=diffuse;
}
Eigen::Matrix<GLfloat,3,1> ShadowLight::getLightDiffuse(int i) const {
  return _lights[i]._diffuse.segment<3>(0);
}
void ShadowLight::setLightSpecular(int i,const Eigen::Matrix<GLfloat,3,1>& specular) {
  _lights[i]._specular.segment<3>(0)=specular;
}
Eigen::Matrix<GLfloat,3,1> ShadowLight::getLightSpecular(int i) const {
  return _lights[i]._specular.segment<3>(0);
}
void ShadowLight::clear() {
  _lights.clear();
}
int ShadowLight::softShadow() const {
  return _softShadow;
}
void ShadowLight::softShadow(int softShadow) {
  _softShadow=softShadow;
}
bool ShadowLight::autoAdjust() const {
  return _autoAdjust;
}
void ShadowLight::autoAdjust(bool autoAdjust) {
  _autoAdjust=autoAdjust;
}
int ShadowLight::lightSz() const {
  return _lightSz;
}
void ShadowLight::lightSz(int sz) {
  _lightSz=sz;
}
GLfloat ShadowLight::bias() const {
  return _bias;
}
void ShadowLight::bias(GLfloat bias) {
  _bias=bias;
}
int ShadowLight::nrLight() const {
  return (int)_lights.size();
}
bool ShadowLight::hasShadow() const {
  return _shadow;
}
void ShadowLight::renderShadow(const Eigen::Matrix<GLfloat,6,1>& bb,std::function<void(const Eigen::Matrix<GLfloat,-1,1>&)> func) {
  if(_shadow>0) {
    GLfloat zNear,zFar,far=calculateFarPlane(bb);
    _shadowProg->begin();
    _shadowProg->setUniformFloat("far_plane",far);
    for(const Light& l:_lights) {
      _shadowProg->setUniformFloat("lightPos",Eigen::Matrix<GLfloat,3,1>(l._position.segment<3>(0)));
      for(int d=0; d<6; d++) {
        matrixMode(GL_MODELVIEW_MATRIX);
        pushMatrix();
        loadIdentity();
        multMatrixf(l._MV[d]);
        _shadowProg->setUniformFloat("invModelViewMatrixShadow",l._invMV[d]);
        zRangef(bb,zNear,zFar);
        matrixMode(GL_PROJECTION_MATRIX);
        pushMatrix();
        loadIdentity();
        perspectivef(90,1,zNear,far);

        l._shadowMap->begin(d);
        glClear(GL_DEPTH_BUFFER_BIT);
        if(zNear<zFar)
          func(l._viewFrustum[d]);
        l._shadowMap->end();

        matrixMode(GL_MODELVIEW_MATRIX);
        popMatrix();
        matrixMode(GL_PROJECTION_MATRIX);
        popMatrix();
        //l._shadowMap->saveImage(d,"depth"+std::to_string(d)+".png");
      }
    }
    Program::currentProgram()->end();
  }
}
void ShadowLight::begin(const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(_lightSz>0) {
    getRoundPointProg()->begin();
    setRoundPointSize(_lightSz);
    setupMaterial(NULL,0,0,0);
    setupMatrixInShader();
    for(const Light& l:_lights)
      drawPointf(Eigen::Matrix<GLfloat,3,1>(l._position[0],l._position[1],l._position[2]));
    Program::currentProgram()->end();
  }

  _shadowLightProg->begin();
  getFloatv(GL_MODELVIEW_MATRIX,_MVShadow);
  _invMVShadow=_MVShadow.inverse();
  _shadowLightProg->setUniformFloat("invModelViewMatrixShadow",_invMVShadow);
  _shadowLightProg->setUniformFloat("invShadowMapSize",1.0/_shadow);
  _shadowLightProg->setUniformInt("softShadowPass",_softShadow);
  _shadowLightProg->setUniformInt("MAX_LIGHTS",_lights.size());
  _shadowLightProg->setUniformFloat("far_plane",calculateFarPlane(bb));
  _shadowLightProg->setUniformFloat("bias",_bias);
  _shadowLightProg->setUniformBool("useShadow",_shadow>0);
  _shadowLightProg->setUniformBool("softShadow",_softShadow>0);
  _shadowLightProg->setTexUnit("diffuseMap",0);
  if(_shadow>0) {
    int id=0;
    for(const Light& l:_lights) {
      glActiveTexture(GL_TEXTURE1+id);
      l._shadowMap->beginShadow();
      _shadowLightProg->setTexUnit("depthMap["+std::to_string(id)+"]",id+1);
      id++;
    }
    glActiveTexture(GL_TEXTURE1+id);
  }
}
void ShadowLight::setupLightMaterial(const Material& mat) {
  int id=0;
  //"struct LightMaterial {\n"
  //"  vec3 positionEye;\n"
  //"  vec4 position;\n"
  //"  vec4 ambient;\n"
  //"  vec4 diffuse;\n"
  //"  vec4 specular;\n"
  //"  float shininess;\n"
  //"};\n"
  for(const Light& l:_lights) {
    Eigen::Matrix<GLfloat,4,1> positionEyeH=_MVShadow*l._position;
    Eigen::Matrix<GLfloat,3,1> positionEye=positionEyeH.segment<3>(0)/positionEyeH[3];
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].positionEye",positionEye);
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].position",l._position);
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].ambient",Eigen::Matrix<GLfloat,4,1>((l._ambient.array()*mat._ambient.array()).matrix()));
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].diffuse",Eigen::Matrix<GLfloat,4,1>((l._diffuse.array()*mat._diffuse.array()).matrix()));
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].specular",Eigen::Matrix<GLfloat,4,1>((l._specular.array()*mat._specular.array()).matrix()));
    _shadowLightProg->setUniformFloat("lights["+std::to_string(id)+"].shininess",mat._shininess);
    id++;
  }
}
void ShadowLight::end() {
  Program::currentProgram()->end();
  if(_shadow>0) {
    int id=0;
    for(const Light& l:_lights) {
      glActiveTexture(GL_TEXTURE1+id);
      l._shadowMap->endShadow();
      id++;
    }
    glActiveTexture(GL_TEXTURE0);
  }
}
//helper
GLfloat ShadowLight::calculateFarPlane(const Eigen::Matrix<GLfloat,6,1>& bb) const {
  GLfloat farPlane=0;
  for(GLfloat x: {
        bb[0],bb[3]
      })
    for(GLfloat y: {
          bb[1],bb[4]
        })
      for(GLfloat z: {
            bb[2],bb[5]
          }) {
        Eigen::Matrix<GLfloat,4,1> pos(x,y,z,1);
        for(const Light& l:_lights)
          farPlane=std::max(farPlane,(pos-l._position).norm());
      }
  return farPlane;
}
void ShadowLight::setMVLight(Light& l) const {
  static const GLfloat target[6][3]= {
    {1.,0.,0.},{-1.,0.,0.},
    {0.,1.,0.},{0.,-1.,0.},
    {0.,0.,1.},{0.,0.,-1.}
  };
  static const GLfloat up[6][3]= {
    {0.,-1.,0.},{0.,-1.,0.},
    {0.,0.,1.},{0.,0.,-1.},
    {0.,-1.,0.},{0.,-1.,0.}
  };
  for(int d=0; d<6; d++) {
    matrixMode(GL_MODELVIEW_MATRIX);
    pushMatrix();
    loadIdentity();
    lookAtf(l._position[0],
            l._position[1],
            l._position[2],
            l._position[0]+target[d][0],
            l._position[1]+target[d][1],
            l._position[2]+target[d][2],
            up[d][0],up[d][1],up[d][2]);
    matrixMode(GL_PROJECTION_MATRIX);
    pushMatrix();
    loadIdentity();
    perspectivef(90,1,1,10);  //add too dummy values

    l._target[d]=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(target[d]);
    getFloatv(GL_MODELVIEW_MATRIX,l._MV[d]);
    l._invMV[d]=l._MV[d].inverse();
    l._viewFrustum[d]=getViewFrustum3DPlanes().segment<16>(0);  //we do not use zNear,zFar to do culling

    matrixMode(GL_MODELVIEW_MATRIX);
    popMatrix();
    matrixMode(GL_PROJECTION_MATRIX);
    popMatrix();
  }
}
std::shared_ptr<Program> ShadowLight::_shadowLightProg;
std::shared_ptr<Program> ShadowLight::_shadowProg;
}
