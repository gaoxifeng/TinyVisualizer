#include "CameraManipulator.h"
#include "Camera3D.h"

namespace DRAWER {
CameraManipulator::CameraManipulator(std::shared_ptr<Camera3D> camera):_camera(camera) {}
void CameraManipulator::init(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max()) {
    _camera->setPosition(Eigen::Matrix<GLfloat,3,1>(1,1,1));
    _camera->setDirection(Eigen::Matrix<GLfloat,3,1>(-1,-1,-1));
  } else {
    _camera->setPosition(bb.template segment<3>(3)*1.5f-bb.template segment<3>(0)*0.5f);
    _camera->setDirection(bb.template segment<3>(0)-bb.template segment<3>(3));
  }
}
Eigen::Matrix<GLfloat,4,4> CameraManipulator::postDraw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {
  return Eigen::Matrix<GLfloat,4,4>::Identity();
}
}
