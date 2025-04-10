#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "DrawerUtility.h"

namespace DRAWER {
class Texture;
class Povray;
struct EXPORT Background {
  void draw();
  void drawPovray(Povray& pov) const;
  Eigen::Matrix<GLfloat,3,1> _color=Eigen::Matrix<GLfloat,3,1>(1,1,1);
  Eigen::Matrix<GLfloat,2,1> _tcMult=Eigen::Matrix<GLfloat,2,1>(1,1);
  std::shared_ptr<Texture> _tex;
};
}

#endif
