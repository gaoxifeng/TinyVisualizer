#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include "Drawer.h"

namespace DRAWER {
class Camera2D : public Camera {
 public:
  Camera2D(GLfloat xExt);
  void focusOn(std::shared_ptr<Shape> s) override;
  void mouse(GLFWwindow* wnd,int button,int action,int mods) override;
  void wheel(GLFWwindow* wnd,double xoffset,double yoffset) override;
  void motion(GLFWwindow* wnd,double x,double y) override;
  void key(GLFWwindow* wnd,int key,int scan,int action,int mods) override;
  void draw(GLFWwindow* wnd,const Eigen::Matrix<GLfloat,6,1>&) override;
  Eigen::Matrix<GLfloat,-1,1> getCameraRay(GLFWwindow* wnd,double x,double y) const override;
  Eigen::Matrix<GLfloat,-1,1> getViewFrustum() const override;
  void setTexture(std::shared_ptr<Texture> tex,const Eigen::Matrix<GLfloat,2,1>& tcMult=Eigen::Matrix<GLfloat,2,1>(1,1));
 private:
  GLfloat _xExt,_yExt;
  GLfloat _xCtr,_yCtr;
  GLfloat _xCtrLast,_yCtrLast;
  GLfloat _scale;
  double _xLast,_yLast;
  bool _inMotion,_debug;
  std::shared_ptr<Shape> _focus;
  std::shared_ptr<Texture> _tex;
  Eigen::Matrix<GLfloat,2,1> _tcMult;
  Eigen::Matrix<GLfloat,-1,1> _debugFrustum;
};
}

#endif
