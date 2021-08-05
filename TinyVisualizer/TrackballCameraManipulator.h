#ifndef TRACKBALL_CAMERA_MANIPULATOR_H
#define TRACKBALL_CAMERA_MANIPULATOR_H

#include "CameraManipulator.h"

namespace DRAWER {
class TrackballCameraManipulator : public CameraManipulator {
 public:
  TrackballCameraManipulator(std::shared_ptr<Camera3D> camera);
  void mouse(GLFWwindow* wnd,int button,int action,int) override;
  void wheel(GLFWwindow*,double,double yoffset) override;
  void motion(GLFWwindow* wnd,double x,double y) override;
  void frame(GLFWwindow* wnd,GLfloat time) override;
  void preDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) override;
  void postDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) override;
 protected:
  Eigen::Matrix<GLfloat,6,1> _bb;
  Eigen::Matrix<GLfloat,3,3> _rot,_rot0;
  GLfloat _scaleCoef,_sensitive;
  bool _scaleMode,_inMotion;
  double _posX,_posY,_xCurr,_yCurr;
};
}

#endif
