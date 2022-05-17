#ifndef SSP_H
#define SSP_H

#include "PatchDeformer.h"

namespace DRAWER {

class SSP {
 public:
  typedef PatchDeformer::T T;
  typedef PatchDeformer::Vert2 Vert2;
  typedef PatchDeformer::Vert3 Vert3;
  typedef PatchDeformer::Grad Grad;
  typedef PatchDeformer::Hess Hess;
  typedef PatchDeformer::DVec DVec;
  typedef PatchDeformer::F F;
  typedef PatchDeformer::FFlat FFlat;
  typedef PatchDeformer::FGrad FGrad;
  typedef PatchDeformer::DMat DMat;
  typedef PatchDeformer::SMat SMat;
  typedef PatchDeformer::Trips Trips;
  //for each triangle, the squared Jacobian energy is:
  //J_e^{-1}*J_h^T*J_h*J_e^{-1}
  //the arap energy is:
  //||J_h*J_e^{-1}-J_h*J_{e0}^{-1}*R||_F^2
  SSP(const PatchDeformer& deformer,std::shared_ptr<Texture> tex,T texelSize);
  SSP(const DVec& vss0,const DVec& tss0,std::shared_ptr<Texture> tex,
      const std::vector<Eigen::Matrix<int,3,1>>& iss0,T texelSize);
  T energy(const DVec& vss,DVec* grad,SMat* hess,bool areaScaled=true) const;
  void debug(T DELTA) const;
 private:
  void reset(const DVec& vss0,const DVec& tss0,std::shared_ptr<Texture> tex,
             const std::vector<Eigen::Matrix<int,3,1>>& iss0,T texelSize);
  F computeJHTJH(const Vert2 t[3],std::shared_ptr<Texture> tex,T len,T texelSize) const;
  T energy(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,const F H,Grad* grad,Hess* hess) const;
  T energy(const Vert2& a,const Vert2& b,const Vert2& c,const T H[2][2],Grad* grad,Hess* hess) const;
  T energyAreaScaled(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,const F H,Grad* grad,Hess* hess) const;
  T energyAreaScaled(const Vert2& a,const Vert2& b,const Vert2& c,const T H[2][2],Grad* grad,Hess* hess) const;
  //data
  std::vector<Eigen::Matrix<int,3,1>> _iss;
  std::vector<F> _JHTJHss;
  DVec _vss0;
};

}

#endif
