#include "CameraManipulator.h"
#include "Camera3D.h"

namespace DRAWER {
CameraManipulator::CameraManipulator(Camera3D& camera):_camera(camera) {}
void CameraManipulator::init(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max())
    _camera.setPosition(Eigen::Matrix<GLfloat,3,1>(-1,0,0));
  else _camera.setPosition(Eigen::Matrix<GLfloat,3,1>(bb[0],(bb[1]+bb[4])/2,(bb[2]+bb[5])/2));
  _camera.setDirection(Eigen::Matrix<GLfloat,3,1>(1,0,0));
}
}
