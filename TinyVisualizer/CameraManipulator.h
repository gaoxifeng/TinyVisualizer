#ifndef CAMERA_MANIPULATOR_H
#define CAMERA_MANIPULATOR_H

#include "Drawer.h"

namespace DRAWER {
class Camera3D;
class CameraManipulator {
 public:
  CameraManipulator(Camera3D& camera);
  virtual void mouse(GLFWwindow* wnd,int button,int action,int mods) {}
  virtual void wheel(GLFWwindow* wnd,double xoffset,double yoffset) {}
  virtual void motion(GLFWwindow* wnd,double x,double y) {}
  virtual void frame(GLFWwindow* wnd,GLfloat time) {}
  virtual void key(GLFWwindow* wnd,int key,int scan,int action,int mods) {}
  virtual void preDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {}
  virtual void postDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {}
  virtual void init(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb);
 protected:
  Camera3D& _camera;
};
}

#endif
