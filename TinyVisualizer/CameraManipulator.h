#ifndef CAMERA_MANIPULATOR_H
#define CAMERA_MANIPULATOR_H

#include "Drawer.h"

namespace DRAWER {
class Camera3D;
class CameraManipulator {
 public:
  CameraManipulator(std::shared_ptr<Camera3D> camera);
  virtual void frame(GLFWwindow* wnd,GLfloat time) {}
  virtual void mouse(GLFWwindow* wnd,int button,int action,int mods,bool captured) {}
  virtual void wheel(GLFWwindow* wnd,double xoffset,double yoffset,bool captured) {}
  virtual void motion(GLFWwindow* wnd,double x,double y,bool captured) {}
  virtual void key(GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {}
  virtual void preDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {}
  virtual void postDraw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {}
  virtual void imGuiCallback() {}
  virtual void init(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>& bb);
 protected:
  std::shared_ptr<Camera3D> _camera;
};
}

#endif
