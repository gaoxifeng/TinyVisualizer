#include "ShadowAndLight.h"
#include "DefaultLight.h"
#include "Matrix.h"
#include "VBO.h"
#include <iostream>

namespace DRAWER {
#define MAX_LIGHT 8
ShadowLight::ShadowLight(int shadow,int softShadow,bool autoAdjust)
  :_bias(0.1f),_softShadow(softShadow),_autoAdjust(autoAdjust),_shadow(shadow),_lightSz(0) {}
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
  ASSERT_MSGV(_lights.size()<=MAX_LIGHT,"At most %d lights supported!",MAX_LIGHT)
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
std::shared_ptr<Program> ShadowLight::getLightProg() const {
#include "Shader/ShadowLightVert.h"
#include "Shader/ShadowLightFrag.h"
  std::shared_ptr<Program> lightProg=Program::findProgram("Light");
  if(!lightProg) {
    Shader::registerShader("Light",ShadowLightVert,"",LightFrag);
    Program::registerProgram("Light","Light","","Light");
    lightProg=Program::findProgram("Light");
  }
  return lightProg;
}
std::shared_ptr<Program> ShadowLight::getLightNormalProg() const {
#include "Shader/ShadowLightVertNoNormal.h"
#include "Shader/NormalGeom.h"
  std::shared_ptr<Program> lightNormalProg=Program::findProgram("LightNormal");
  if(!lightNormalProg) {
    getLightProg();
    Shader::registerShader("LightNormal",ShadowLightVertNoNormal,NormalGeom);
    Program::registerProgram("LightNormal","LightNormal","LightNormal","Light");
    lightNormalProg=Program::findProgram("LightNormal");
  }
  return lightNormalProg;
}
std::shared_ptr<Program> ShadowLight::getShadowLightProg() const {
#include "Shader/ShadowLightFrag.h"
  std::shared_ptr<Program> shadowLightProg=Program::findProgram("ShadowLight");
  if(!shadowLightProg) {
    getLightProg();
    Shader::registerShader("ShadowLight","","",ShadowLightFrag);
    Program::registerProgram("ShadowLight","Light","","ShadowLight");
    shadowLightProg=Program::findProgram("ShadowLight");
  }
  return shadowLightProg;
}
std::shared_ptr<Program> ShadowLight::getShadowLightNormalProg() const {
  std::shared_ptr<Program> shadowLightNormalProg=Program::findProgram("ShadowLightNormal");
  if(!shadowLightNormalProg) {
    getShadowLightProg();
    getLightNormalProg();
    Program::registerProgram("ShadowLightNormal","LightNormal","LightNormal","ShadowLight");
    shadowLightNormalProg=Program::findProgram("ShadowLightNormal");
  }
  return shadowLightNormalProg;
}
std::shared_ptr<Program> ShadowLight::getShadowProg() const {
#include "Shader/ShadowVert.h"
#include "Shader/ShadowFrag.h"
  std::shared_ptr<Program> shadowProg=Program::findProgram("Shadow");
  if(!shadowProg) {
    Shader::registerShader("Shadow",ShadowVert,"",ShadowFrag);
    Program::registerProgram("Shadow","Shadow","","Shadow");
    shadowProg=Program::findProgram("Shadow");
  }
  return shadowProg;
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
    getShadowProg()->begin();
    getShadowProg()->setUniformFloat("far_plane",far);
    for(const Light& l:_lights) {
      getShadowProg()->setUniformFloat("lightPos",Eigen::Matrix<GLfloat,3,1>(l._position.segment<3>(0)));
      for(int d=0; d<6; d++) {
        matrixMode(GL_MODELVIEW_MATRIX);
        pushMatrix();
        loadIdentity();
        multMatrixf(l._MV[d]);
        getShadowProg()->setUniformFloat("invModelViewMatrixShadow",l._invMV[d]);
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
void ShadowLight::begin(const Eigen::Matrix<GLfloat,6,1>& bb,bool recomputeNormal) {
  if(_lightSz>0) {
    getRoundPointProg()->begin();
    setRoundPointSize((GLfloat)_lightSz);
    setupMaterial(NULL,0,0,0);
    setupMatrixInShader();
    for(const Light& l:_lights)
      drawPointf(Eigen::Matrix<GLfloat,3,1>(l._position[0],l._position[1],l._position[2]));
    Program::currentProgram()->end();
  }

  std::shared_ptr<Program> prog;
  if(_shadow)
    prog=recomputeNormal?getShadowLightNormalProg():getShadowLightProg();
  else prog=recomputeNormal?getLightNormalProg():getLightProg();

  prog->begin();
  prog->setUniformInt("MAX_LIGHTS",(int)_lights.size());
  prog->setTexUnit("diffuseMap",0,false);
  getFloatv(GL_MODELVIEW_MATRIX,_MVShadow);
  _invMVShadow=_MVShadow.inverse();
  if(_shadow>0) {
    prog->setUniformFloat("invModelViewMatrixShadow",_invMVShadow);
    prog->setUniformFloat("invShadowMapSize",1.0f/(GLfloat)_shadow);
    prog->setUniformInt("softShadowPass",_softShadow);
    prog->setUniformFloat("far_plane",calculateFarPlane(bb));
    prog->setUniformFloat("bias",_bias);
    prog->setUniformBool("softShadow",_softShadow>0);
    ASSERT_MSG(!_lights.empty(),"No light exists!")
    for(int id=0; id<MAX_LIGHT; id++) {
      const Light& l=_lights[std::min(id,(int)_lights.size()-1)];
      glActiveTexture(GL_TEXTURE1+id);
      l._shadowMap->beginShadow();
      prog->setTexUnit("lights["+std::to_string(id)+"].depthMap",id+1);
    }
    glActiveTexture(GL_TEXTURE1+MAX_LIGHT);
  }
}
void ShadowLight::setupLightMaterial(const Material& mat) {
  //"struct LightMaterial {\n"
  //"  vec3 positionEye;\n"
  //"  vec4 position;\n"
  //"  vec4 ambient;\n"
  //"  vec4 diffuse;\n"
  //"  vec4 specular;\n"
  //"  float shininess;\n"
  //"};\n"
  std::shared_ptr<Program> prog=Program::currentProgram();
  ASSERT_MSG(!_lights.empty(),"No light exists!")
  for(int id=0; id<MAX_LIGHT; id++) {
    const Light& l=_lights[std::min(id,(int)_lights.size()-1)];
    Eigen::Matrix<GLfloat,4,1> positionEyeH=_MVShadow*l._position;
    Eigen::Matrix<GLfloat,3,1> positionEye=positionEyeH.segment<3>(0)/positionEyeH[3];
    prog->setUniformFloat("lights["+std::to_string(id)+"].positionEye",positionEye);
    prog->setUniformFloat("lights["+std::to_string(id)+"].position",l._position,false);
    prog->setUniformFloat("lights["+std::to_string(id)+"].ambient",Eigen::Matrix<GLfloat,4,1>((l._ambient.array()*mat._ambient.array()).matrix()));
    prog->setUniformFloat("lights["+std::to_string(id)+"].diffuse",Eigen::Matrix<GLfloat,4,1>((l._diffuse.array()*mat._diffuse.array()).matrix()));
    prog->setUniformFloat("lights["+std::to_string(id)+"].specular",Eigen::Matrix<GLfloat,4,1>((l._specular.array()*mat._specular.array()).matrix()));
    prog->setUniformFloat("lights["+std::to_string(id)+"].shininess",mat._shininess);
  }
}
void ShadowLight::end() {
  Program::currentProgram()->end();
  if(_shadow>0) {
    for(int id=0; id<MAX_LIGHT; id++) {
      const Light& l=_lights[std::min(id,(int)_lights.size()-1)];
      glActiveTexture(GL_TEXTURE1+id);
      l._shadowMap->endShadow();
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
}
