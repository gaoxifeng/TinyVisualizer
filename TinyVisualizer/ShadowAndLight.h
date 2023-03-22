#ifndef SHADOW_AND_LIGHT_H
#define SHADOW_AND_LIGHT_H

#include "Drawer.h"
#include "Shader.h"
#include "FBO.h"

namespace DRAWER {
class ShadowLight {
 public:
  struct Material {
    Eigen::Matrix<GLfloat,4,1> _ambient;
    Eigen::Matrix<GLfloat,4,1> _diffuse;
    Eigen::Matrix<GLfloat,4,1> _specular;
    std::shared_ptr<Texture> _tex;
    GLfloat _pointSize,_lineWidth;
    GLfloat _shininess;
    Drawer* _drawer;
  };
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
  void setDefaultLight(Eigen::Matrix<GLfloat,6,1> bb,const GLfloat relativeDistance=2.,
                       const Eigen::Matrix<GLfloat,3,1>& ambient=Eigen::Matrix<GLfloat,3,1>(1,1,1),
                       const Eigen::Matrix<GLfloat,3,1>& diffuse=Eigen::Matrix<GLfloat,3,1>(1,1,1),
                       const Eigen::Matrix<GLfloat,3,1>& specular=Eigen::Matrix<GLfloat,3,1>(1,1,1));
  void setLightPos(int i,const Eigen::Matrix<GLfloat,3,1>& pos);
  Eigen::Matrix<GLfloat,3,1> getLightPos(int i) const;
  void setLightAmbient(int i,const Eigen::Matrix<GLfloat,3,1>& ambient);
  Eigen::Matrix<GLfloat,3,1> getLightAmbient(int i) const;
  void setLightDiffuse(int i,const Eigen::Matrix<GLfloat,3,1>& diffuse);
  Eigen::Matrix<GLfloat,3,1> getLightDiffuse(int i) const;
  void setLightSpecular(int i,const Eigen::Matrix<GLfloat,3,1>& specular);
  Eigen::Matrix<GLfloat,3,1> getLightSpecular(int i) const;
  std::shared_ptr<Program> getLightProg() const;
  std::shared_ptr<Program> getLightNormalProg() const;
  std::shared_ptr<Program> getShadowLightProg() const;
  std::shared_ptr<Program> getShadowLightNormalProg() const;
  std::shared_ptr<Program> getShadowProg() const;
  void clear();
  int softShadow() const;
  void softShadow(int softPass);
  bool autoAdjust() const;
  void autoAdjust(bool autoAdjust);
  int lightSz() const;
  void lightSz(int sz);
  GLfloat bias() const;
  void bias(GLfloat bias);
  int nrLight() const;
  bool hasShadow() const;
  void renderShadow(const Eigen::Matrix<GLfloat,6,1>& bb,std::function<void(const Eigen::Matrix<GLfloat,-1,1>&)> func);
  void begin(const Eigen::Matrix<GLfloat,6,1>& bb,bool recomputeNormal);
  void setupLightMaterial(const Material& mat);
  void end();
 protected:
  GLfloat calculateFarPlane(const Eigen::Matrix<GLfloat,6,1>& bb) const;
  void setMVLight(Light& l) const;
  GLfloat _bias;
  int _softShadow;
  bool _autoAdjust;
  int _shadow,_lightSz;
  std::vector<Light> _lights;
  Eigen::Matrix<GLfloat,4,4> _MVShadow,_invMVShadow;
};
}

#endif
