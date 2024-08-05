#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "Drawer.h"
#include "CameraManipulator.h"

namespace DRAWER {
class Camera3D : public Camera {
  RTTI_DECLARE_TYPEINFO(Camera3D, Camera);
 public:
  Camera3D(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up);
  void setManipulator(std::shared_ptr<CameraManipulator> manipulator);
  std::shared_ptr<CameraManipulator> getManipulator() const;
  void focusOn(std::shared_ptr<Shape> s) override;
  void frame(GLFWwindowPtr wnd,GLfloat time) override;
  void mouse(GLFWwindowPtr wnd,int button,int action,int mods,bool captured) override;
  void wheel(GLFWwindowPtr wnd,double xoffset,double yoffset,bool captured) override;
  void motion(GLFWwindowPtr wnd,double x,double y,bool captured) override;
  void key(GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) override;
  void draw(GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>&) override;
  void drawPovray(Povray& pov,GLFWwindowPtr wnd,const Eigen::Matrix<GLfloat,6,1>&) override;
  Eigen::Matrix<GLfloat,-1,1> getCameraRay(GLFWwindowPtr wnd,double x,double y) const override;
  Eigen::Matrix<GLfloat,-1,1> getViewFrustum() const override;
  Eigen::Matrix<GLfloat,3,1> position() const;
  Eigen::Matrix<GLfloat,3,1> direction() const;
  Eigen::Matrix<GLfloat,3,1> up() const;
  void setPosition(const Eigen::Matrix<GLfloat,3,1>& pos);
  void setDirection(const Eigen::Matrix<GLfloat,3,1>& dir);
  void setUp(const Eigen::Matrix<GLfloat,3,1>& up);
 private:
  void begin(GLfloat& theta,GLfloat& phi) const;
  void end(GLfloat theta,GLfloat phi);
  static GLfloat clampMin(GLfloat val);
  //data
  bool _debug;
  GLint _vp[4];
  GLfloat _angle;
  Eigen::Matrix<GLfloat,3,1> _up,_pos,_dir;
  std::shared_ptr<CameraManipulator> _manipulator;
  Eigen::Matrix<GLfloat,-1,1> _debugLine,_debugFrustum;
  std::shared_ptr<Shape> _focus;
};
}

#endif
