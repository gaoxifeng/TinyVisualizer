#include "ShadowAndLight.h"
#include <iostream>

namespace DRAWER {
const std::string ShadowLightVert=
  "#version 410 compatibility\n"\
  "uniform mat4 modelViewMatrix;\n"\
  "uniform mat4 invModelViewMatrix;\n"\
  "uniform int MAX_LIGHTS;\n"\
  "out vec3 lightPosEye[16];\n"\
  "out vec3 vN,v,v0;\n"\
  "out vec2 tc;\n"\
  "void main(void)\n"\
  "{\n"\
  "  gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;\n"\
  "  vN=normalize(gl_NormalMatrix*gl_Normal);\n"\
  "  vec4 vMV=gl_ModelViewMatrix*gl_Vertex;\n"\
  "  v=vec3(vMV);\n"\
  "  v0=vec3(invModelViewMatrix*vMV);\n"\
  "  tc=gl_MultiTexCoord0.st;\n"\
  "  for (int i=0;i<MAX_LIGHTS;i++) {\n"\
  "    vec4 LH=modelViewMatrix*gl_LightSource[i].position;\n"\
  "    lightPosEye[i]=LH.xyz/LH.w;\n"\
  "  }\n"\
  "};\n";
const std::string ShadowLightFrag=
  "#version 410 compatibility\n"\
  "//shadow\n"\
  "uniform sampler2D diffuseMap;\n"\
  "uniform samplerCube depthMap[16];\n"\
  "uniform mat4 modelViewMatrix;\n"\
  "uniform float far_plane,bias;\n"\
  "uniform bool softShadow;\n"\
  "uniform bool useShadow;\n"\
  "//light\n"\
  "uniform float invShadowMapSize;\n"\
  "uniform int softShadowPass;\n"\
  "uniform int MAX_LIGHTS;\n"\
  "in vec3 lightPosEye[16];\n"\
  "in vec3 vN,v,v0;\n"\
  "in vec2 tc;\n"\
  "vec3 gridSamplingDisk[27]=vec3[]\n"\
  "(\n"\
  "  vec3(1, 1, 1), vec3( 0, 1, 1), vec3(-1, 1, 1),\n"\
  "  vec3(1, 0, 1), vec3( 0, 0, 1), vec3(-1, 0, 1),\n"\
  "  vec3(1,-1, 1), vec3( 0,-1, 1), vec3(-1,-1, 1),\n"\
  "  vec3(1, 1, 0), vec3( 0, 1, 0), vec3(-1, 1, 0),\n"\
  "  vec3(1, 0, 0), vec3( 0, 0, 0), vec3(-1, 0, 0),\n"\
  "  vec3(1,-1, 0), vec3( 0,-1, 0), vec3(-1,-1, 0),\n"\
  "  vec3(1, 1,-1), vec3( 0, 1,-1), vec3(-1, 1,-1),\n"\
  "  vec3(1, 0,-1), vec3( 0, 0,-1), vec3(-1, 0,-1),\n"\
  "  vec3(1,-1,-1), vec3( 0,-1,-1), vec3(-1,-1,-1)\n"\
  ");\n"\
  "float calcShadow(vec3 l,int lid)\n"\
  "{\n"\
  "  float shadow=0.;\n"\
  "  float currentDepth=length(v0-l);\n"\
  "  float closestDepth=texture(depthMap[lid],v0-l).r*far_plane;\n"\
  "  if(currentDepth-bias>closestDepth)\n"\
  "    shadow=1.;\n"\
  "  return shadow;\n"\
  "}\n"\
  "float calcSoftShadow(vec3 l,int lid)\n"\
  "{\n"\
  "  float shadow=0.;\n"\
  "  float currentDepth=length(v0-l);\n"\
  "  vec3 dir=normalize(v0-l);\n"\
  "  for(int pass=0;pass<softShadowPass;pass++)\n"\
  "    for(int i=0;i<27;++i) {\n"\
  "      float closestDepth=texture(depthMap[lid],dir+gridSamplingDisk[i]*invShadowMapSize*pass).r*far_plane;   // undo mapping [0;1]\n"\
  "      if(currentDepth-bias>closestDepth)\n"\
  "        shadow+=1.0;\n"\
  "    }\n"\
  "  shadow/=27*softShadowPass;\n"\
  "  return shadow;\n"\
  "}\n"\
  "void main(void)\n"\
  "{\n"\
  "  vec3 N=normalize(vN);\n"\
  "  vec4 colorMap=texture(diffuseMap,tc);\n"\
  "  vec4 finalColor=vec4(0.0,0.0,0.0,0.0);\n"\
  "  for (int i=0;i<MAX_LIGHTS;i++) {\n"\
  "    vec3 L=normalize(lightPosEye[i]-v);\n"\
  "    vec3 E=normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)\n"\
  "    vec3 R=normalize(-reflect(L,N));\n"\
  "    //calculate Ambient Term:\n"\
  "    vec4 Iamb=gl_FrontLightProduct[i].ambient*colorMap;\n"\
  "    //calculate Diffuse Term:\n"\
  "    vec4 Idiff=gl_FrontLightProduct[i].diffuse*colorMap*max(dot(N,L),0.0);\n"\
  "    Idiff=clamp(Idiff,0.0,1.0);\n"\
  "    //calculate Specular Term:\n"\
  "    vec4 Ispec=gl_FrontLightProduct[i].specular*colorMap*pow(max(dot(R,E),0.0),gl_FrontMaterial.shininess);\n"\
  "    Ispec=clamp(Ispec,0.0,1.0);\n"\
  "    //shadow\n"\
  "    if(useShadow) {\n"\
  "      float s;\n"\
  "      if(softShadow)\n"\
  "        s=calcSoftShadow(gl_LightSource[i].position.xyz,i);\n"\
  "      else s=calcShadow(gl_LightSource[i].position.xyz,i);\n"\
  "      finalColor+=Iamb+(1-s)*(Idiff+Ispec);\n"\
  "    } else finalColor+=Iamb+Idiff+Ispec;\n"\
  "  }\n"\
  "  //write Total Color:\n"\
  "  gl_FragColor=finalColor;\n"\
  "};\n";
const std::string ShadowVert=
  "#version 410 compatibility\n"\
  "uniform mat4 invModelViewMatrix;\n"\
  "out vec3 FragPos;\n"\
  "void main(void)\n"\
  "{\n"\
  "  vec4 vMV=gl_ModelViewMatrix*gl_Vertex;\n"\
  "  FragPos=vec3(invModelViewMatrix*vMV);\n"\
  "  gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;\n"\
  "}\n";
const std::string ShadowFrag=
  "#version 410 compatibility\n"\
  "uniform vec3 lightPos;\n"\
  "uniform float far_plane;\n"\
  "in vec3 FragPos;\n"\
  "void main() {\n"\
  "  float lightDistance=length(FragPos-lightPos);\n"\
  "  //map to [0;1] range by dividing by far_plane\n"\
  "  lightDistance=lightDistance/far_plane;\n"\
  "  //write this as modified depth\n"\
  "  gl_FragDepth=lightDistance;\n"\
  "}\n";
ShadowLight::ShadowLight(int shadow,int softShadow,bool autoAdjust)
  :_bias(0.1f),_softShadow(softShadow),_autoAdjust(autoAdjust),_shadow(shadow),_lightSz(0) {
  if(!_shader) {
    _shader.reset(new Shader(ShadowLightVert,"",ShadowLightFrag));
    _shaderShadow.reset(new Shader(ShadowVert,"",ShadowFrag));
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
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightPos(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(_lights[i]._position.data());
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightAmbient(int i) {
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_lights[i]._ambient.data());
}
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightAmbient(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(_lights[i]._ambient.data());
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightDiffuse(int i) {
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_lights[i]._diffuse.data());
}
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightDiffuse(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(_lights[i]._diffuse.data());
}
Eigen::Map<Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightSpecular(int i) {
  return Eigen::Map<Eigen::Matrix<GLfloat,3,1>>(_lights[i]._specular.data());
}
Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> ShadowLight::getLightSpecular(int i) const {
  return Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(_lights[i]._specular.data());
}
void ShadowLight::clear() {
  _lights.clear();
}
int ShadowLight::softShadow() const {
  return _softShadow;
}
int& ShadowLight::softShadow() {
  return _softShadow;
}
bool ShadowLight::autoAdjust() const {
  return _autoAdjust;
}
bool& ShadowLight::autoAdjust() {
  return _autoAdjust;
}
int ShadowLight::lightSz() const {
  return _lightSz;
}
int& ShadowLight::lightSz() {
  return _lightSz;
}
GLfloat ShadowLight::bias() const {
  return _bias;
}
GLfloat& ShadowLight::bias() {
  return _bias;
}
int ShadowLight::nrLight() const {
  return (int)_lights.size();
}
bool ShadowLight::hasShadow() const {
  return _shadow;
}
void ShadowLight::renderShadow(const Eigen::Matrix<GLfloat,6,1>& bb,std::function<void(const Eigen::Matrix<GLfloat,-1,1>&)> func) {
  if(_shadow>0) {
    GLfloat zNear,zFar,far_plane=(bb.segment<3>(3)-bb.segment<3>(0)).norm();
    _shaderShadow->begin();
    _shaderShadow->setUniformFloat("far_plane",far_plane);
    for(const Light& l:_lights) {
      _shaderShadow->setUniformFloat("lightPos",Eigen::Matrix<GLfloat,3,1>(l._position.segment<3>(0)));
      for(int d=0; d<6; d++) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf(l._MV[d].data());
        _shaderShadow->setUniformFloat("invModelViewMatrix",l._invMV[d]);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        zRange(bb,l._position.segment<3>(0),l._target[d],zNear,zFar);
        gluPerspective(90,1,zNear,far_plane);

        l._shadowMap->begin(d);
        glClear(GL_DEPTH_BUFFER_BIT);
        if(zNear<zFar)
          func(l._viewFrustum[d]);
        l._shadowMap->end();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        //l._shadowMap->saveImage(d,"depth"+std::to_string(d)+".png");
      }
    }
    _shaderShadow->end();
  }
}
void ShadowLight::begin(const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(_lightSz>0) {
    glColor3f(0,0,0);
    glPointSize(_lightSz);
    glBegin(GL_POINTS);
    for(const Light& l:_lights)
      glVertex3f(l._position[0],l._position[1],l._position[2]);
    glEnd();
  }

  int id=0;
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  for(const Light& l:_lights) {
    glEnable(GL_LIGHT0+id);
    glLightfv(GL_LIGHT0+id,GL_POSITION,&(l._position[0]));
    glLightfv(GL_LIGHT0+id,GL_AMBIENT,&(l._ambient[0]));
    glLightfv(GL_LIGHT0+id,GL_DIFFUSE,&(l._diffuse[0]));
    glLightfv(GL_LIGHT0+id,GL_SPECULAR,&(l._specular[0]));
    id++;
  }
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  _shader->begin();
  Eigen::Matrix<GLfloat,4,4> MV,invMV;
  glGetFloatv(GL_MODELVIEW_MATRIX,MV.data());
  invMV=MV.inverse();
  _shader->setUniformFloat("modelViewMatrix",MV);
  _shader->setUniformFloat("invModelViewMatrix",invMV);
  _shader->setUniformFloat("invShadowMapSize",1.0/_shadow);
  _shader->setUniformInt("softShadowPass",_softShadow);
  _shader->setUniformInt("MAX_LIGHTS",_lights.size());
  GLfloat far_plane=(bb.segment<3>(3)-bb.segment<3>(0)).norm();
  _shader->setUniformFloat("far_plane",far_plane);
  _shader->setUniformFloat("bias",_bias);
  _shader->setUniformBool("useShadow",_shadow>0);
  _shader->setUniformBool("softShadow",_softShadow>0);
  _shader->setTexUnit("diffuseMap",0);
  if(_shadow>0) {
    id=0;
    for(const Light& l:_lights) {
      glActiveTexture(GL_TEXTURE1+id);
      l._shadowMap->beginShadow();
      _shader->setTexUnit("depthMap["+std::to_string(id)+"]",id+1);
      id++;
    }
    glActiveTexture(GL_TEXTURE1+id);
  }
}
void ShadowLight::end() {
  _shader->end();
  glDisable(GL_LIGHTING);
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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(l._position[0],
              l._position[1],
              l._position[2],
              l._position[0]+target[d][0],
              l._position[1]+target[d][1],
              l._position[2]+target[d][2],
              up[d][0],up[d][1],up[d][2]);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(90,1,1,10);  //add too dummy values

    l._target[d]=Eigen::Map<const Eigen::Matrix<GLfloat,3,1>>(target[d]);
    glGetFloatv(GL_MODELVIEW_MATRIX,l._MV[d].data());
    l._invMV[d]=l._MV[d].inverse();
    l._viewFrustum[d]=getViewFrustum3DPlanes().segment<16>(0);  //we do not use zNear,zFar to do culling

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  }
}
std::shared_ptr<Shader> ShadowLight::_shader;
std::shared_ptr<Shader> ShadowLight::_shaderShadow;
}
