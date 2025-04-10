#ifndef FIRST_PERSON_CAMERA_MANIPULATOR_H
#define FIRST_PERSON_CAMERA_MANIPULATOR_H

#include "CameraManipulator.h"

namespace DRAWER {
class EXPORT FirstPersonCameraManipulator : public CameraManipulator {
  RTTI_DECLARE_TYPEINFO(FirstPersonCameraManipulator, CameraManipulator);
 public:
  FirstPersonCameraManipulator(std::shared_ptr<Camera3D> camera);
  void frame(GLFWwindowPtr wnd,GLfloat time) override;
  void mouse(GLFWwindowPtr wnd,int button,int action,int,bool captured) override;
  void wheel(GLFWwindowPtr,double,double yoffset,bool captured) override;
  void motion(GLFWwindowPtr wnd,double x,double y,bool captured) override;
  void key(GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) override;
  void imGuiCallback() override;
  bool isSpeedMode() const;
 protected:
  void begin(GLfloat& theta,GLfloat& phi) const;
  void end(GLfloat theta,GLfloat phi);
  GLfloat clampMin(GLfloat val);
  //data
  Eigen::Matrix<GLfloat,3,1> _t1,_t2;
  bool _forward,_backward,_left,_right,_rise,_dive;
  GLfloat _angle,_sensitive,_speed;
  GLfloat _xLast,_yLast,_xCurr,_yCurr;
  bool _inMotion,_speedMode;
};
}

#endif
