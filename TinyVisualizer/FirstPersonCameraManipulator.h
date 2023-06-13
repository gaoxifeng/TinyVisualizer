#ifndef FIRST_PERSON_CAMERA_MANIPULATOR_H
#define FIRST_PERSON_CAMERA_MANIPULATOR_H

#include "CameraManipulator.h"

namespace DRAWER {
class FirstPersonCameraManipulator : public CameraManipulator {
 public:
  FirstPersonCameraManipulator(std::shared_ptr<Camera3D> camera);
  void frame(GLFWwindow* wnd,GLfloat time) override;
  void mouse(GLFWwindow* wnd,int button,int action,int,bool captured) override;
  void wheel(GLFWwindow*,double,double yoffset,bool captured) override;
  void motion(GLFWwindow* wnd,double x,double y,bool captured) override;
  void key(GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) override;
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
