#ifndef BEZIER_CURVE_SHAPE_H
#define BEZIER_CURVE_SHAPE_H

#include "MeshShape.h"

namespace DRAWER {
class BezierCurveShape : public MeshShape {
 public:
  BezierCurveShape(GLfloat thickness,bool isHermite=true,int RES=8);
  int nrControlPoint() const;
  Eigen::Map<Eigen::Matrix<GLfloat,3,1>> getControlPoint(int i);
  Eigen::Map<const Eigen::Matrix<GLfloat,3,1>> getControlPoint(int i) const;
  template <typename VEC>
  void addControlPoint(const VEC& v) {
    _controlPoints.reserve(_controlPoints.size()+v.size());
    for(int i=0,j=0; i<v.size(); i+=3,j+=2) {
      _controlPoints.push_back(v[i+0]);
      _controlPoints.push_back(v[i+1]);
      _controlPoints.push_back(v[i+2]);
    }
    _dirty=true;
  }
  void clearControlPoint();
  virtual void computeNormals() override;
  virtual void draw(PASS_TYPE passType) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
 private:
  void subdivide();
  void subdivide(const Eigen::Matrix<GLfloat,3,4>& vss,GLfloat a,GLfloat b);
  void tessellate();
  std::vector<GLfloat> _controlPoints;
  bool _isHermite;
  GLfloat _thickness;
  int _RES;
  static Eigen::Matrix<GLfloat,4,4> _toCP;
  static bool _buildMat;
};
}

#endif
