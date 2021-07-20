#ifndef SHADOW_AND_LIGHT_H
#define SHADOW_AND_LIGHT_H

#include "Shader.h"
#include "FBO.h"

namespace DRAWER {
class ShadowLight {
 public:
  struct Light {
    Eigen::Matrix<GLfloat,4,1> _position;
    Eigen::Matrix<GLfloat,4,1> _ambient;
    Eigen::Matrix<GLfloat,4,1> _diffuse;
    Eigen::Matrix<GLfloat,4,1> _specular;
    std::shared_ptr<FBOShadow> _shadowMap;
    Eigen::Matrix<GLfloat,3,1> _target[6];
    Eigen::Matrix<GLfloat,4,4> _MV[6],_invMV[6];
    Eigen::Matrix<GLfloat,-1,1> _viewFrustum[6];
  };
  ShadowLight(int shadow,int softShadow,bool autoAdjust=true);
  int addLight(const Eigen::Matrix<GLfloat,3,1>& pos,
               const Eigen::Matrix<GLfloat,3,1>& ambient=Eigen::Matrix<GLfloat,3,1>(1,1,1),
               const Eigen::Matrix<GLfloat,3,1>& diffuse=Eigen::Matrix<GLfloat,3,1>(1,1,1),
               const Eigen::Matrix<GLfloat,3,1>& specular=Eigen::Matrix<GLfloat,3,1>(1,1,1));
  void setDefaultLight(Eigen::Matrix<GLfloat,6,1> bb,const GLfloat relativeDistance=.2,
                       const Eigen::Matrix<GLfloat,3,1>& ambient=Eigen::Matrix<GLfloat,3,1>(1,1,1),
                       const Eigen::Matrix<GLfloat,3,1>& diffuse=Eigen::Matrix<GLfloat,3,1>(1,1,1),
                       const Eigen::Matrix<GLfloat,3,1>& specular=Eigen::Matrix<GLfloat,3,1>(1,1,1));
  void setLightPos(int i,const Eigen::Matrix<GLfloat,3,1>& pos);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getLightPos(int i) const;
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getLightAmbient(int i);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getLightAmbient(int i) const;
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getLightDiffuse(int i);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getLightDiffuse(int i) const;
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getLightSpecular(int i);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getLightSpecular(int i) const;
  void clear();
  int softShadow() const;
  int& softShadow();
  bool autoAdjust() const;
  bool& autoAdjust();
  int lightSz() const;
  int& lightSz();
  GLfloat bias() const;
  GLfloat& bias();
  int nrLight() const;
  bool hasShadow() const;
  void renderShadow(const Eigen::Matrix<GLfloat,6,1>& bb,std::function<void(const Eigen::Matrix<GLfloat,-1,1>&)> func);
  void begin(const Eigen::Matrix<GLfloat,6,1>& bb);
  void end();
 protected:
  void setMVLight(Light& l) const;
  GLfloat _bias;
  int _softShadow;
  bool _autoAdjust;
  int _shadow,_lightSz;
  std::vector<Light> _lights;
  static std::shared_ptr<Shader> _shader;
  static std::shared_ptr<Shader> _shaderShadow;
};
}

#endif
