#ifndef CAMERA_MANIPULATOR_H
#define CAMERA_MANIPULATOR_H

#include "Drawer.h"

namespace DRAWER {
class Camera3D;
class CameraManipulator : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(CameraManipulator);
 public:
  CameraManipulator(std::shared_ptr<Camera3D> camera);
  virtual ~CameraManipulator() {}
  virtual void frame(GLFWwindowPtr wnd,GLfloat time) {}
  virtual void mouse(GLFWwindowPtr wnd,int button,int action,int mods,bool captured) {}
  virtual void wheel(GLFWwindowPtr wnd,double xoffset,double yoffset,bool captured) {}
  virtual void motion(GLFWwindowPtr wnd,double x,double y,bool captured) {}
  virtual void key(GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) {}
  virtual void preDraw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb) {}
  virtual Eigen::Matrix<GLfloat,4,4> postDraw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb);
  virtual void imGuiCallback() {}
  virtual void init(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>& bb);
 protected:
  std::shared_ptr<Camera3D> _camera;
};
}

#endif
