#ifndef CameraThirdPerson_H
#define CameraThirdPerson_H

#include "Drawer.h"

namespace DRAWER {
class CameraThirdPerson : public Camera {
 public:
     CameraThirdPerson(GLfloat angle,const Eigen::Matrix<GLfloat,3,1>& up);
  void focusOn(std::shared_ptr<Shape> s) override;
  void mouse(GLFWwindow* wnd,int button,int action,int mods) override;
  void wheel(GLFWwindow* wnd,double xoffset,double yoffset) override;
  void motion(GLFWwindow* wnd,double x,double y) override;
  void frame(GLFWwindow* wnd,GLfloat time) override;
  void key(GLFWwindow* wnd,int key,int scan,int action,int mods) override;
  void draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>&) override;
  Eigen::Matrix<GLfloat,-1,1> getCameraRay(GLFWwindow* wnd,double x,double y) const override;
  Eigen::Matrix<GLfloat,-1,1> getViewFrustum() const override;
  void setPosition(const Eigen::Matrix<GLfloat,3,1>& pos);
  void setDirection(const Eigen::Matrix<GLfloat,3,1>& dir);
  void setUp(const Eigen::Matrix<GLfloat,3,1>& up);
 private:
  void begin(GLfloat& theta,GLfloat& phi) const;
  void end(GLfloat theta,GLfloat phi);
  static GLfloat clampMin(GLfloat val);
  bool _forward,_backward,_left,_right,_rise,_dive;
  GLfloat _angle,_sensitive,_speed;
  GLfloat _xLast,_yLast,_xCurr,_yCurr;
  bool _inMotion,_speedMode,_debug;
  Eigen::Matrix<GLfloat,3,1> _up,_t1,_t2,_pos,_dir;
  Eigen::Matrix<GLfloat,-1,1> _debugLine,_debugFrustum;
  std::shared_ptr<Shape> _focus;
};
}

#endif
