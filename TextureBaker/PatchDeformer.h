#ifndef PATCH_DEFORMER_H
#define PATCH_DEFORMER_H

#include <TinyVisualizer/MeshShape.h>
#include <TextureBaker/MeshVisualizer.h>
#include <boost/multiprecision/mpfr.hpp>
typedef boost::multiprecision::mpfr_float mpfr_float;

namespace DRAWER {

class SSP;
class PatchDeformer {
 public:
  friend class SSP;
  typedef double T;
  typedef Eigen::Matrix<T,2,1> Vert2;
  typedef Eigen::Matrix<T,3,1> Vert3;
  typedef Eigen::Matrix<T,6,1> Grad;
  typedef Eigen::Matrix<T,6,6> Hess;
  typedef Eigen::Matrix<T,-1,1> DVec;
  typedef Eigen::Matrix<T,2,2> F;
  typedef Eigen::Matrix<T,4,1> FFlat;
  typedef Eigen::Matrix<T,4,6> FGrad;
  typedef Eigen::Matrix<T,-1,-1> DMat;
  typedef Eigen::SparseMatrix<T> SMat;
  typedef std::vector<Eigen::Triplet<T>> Trips;
  PatchDeformer(const MeshVisualizer& patch2D,const MeshVisualizer& patch3D,T texelSize=0.01,T convexMargin=1e-4);
  bool optimize(DVec& vss,T epsl1=1e-2,T wl1=1e0,T wArea=1e-2,T wConvex=1e-2,T wArap=0,T wSSP=1e-2,int maxIter=1e4,T tol=1e-4,bool callback=true,bool visualize=true);
  const std::vector<std::shared_ptr<Shape>>& getOptimizeHistory() const;
  void debugL1(T eps,T DELTA) const;
  void debugConvex(T DELTA) const;
  void debugArea(T DELTA) const;
  void debugArap(T DELTA) const;
  void debugSSP(T DELTA) const;
 private:
  T buildEnergy(const DVec& vss,T epsl1,T wl1,T wArea,T wConvex,T wArap,T wSSP,DVec* grad,SMat* hess);
  void factorOutOrientation(T eps,DVec& vss,int RES=180);
  //l1
  T l1(T eps,const DVec& vss,DVec* grad,SMat* hess) const;
  T l1(T eps,const DVec& vss,const Eigen::Matrix<int,2,1>& iss,Grad* f,Hess* h) const;
  //convex
  T convexLogBarrier(const DVec& vss,DVec* grad,SMat* hess) const;
  T convexLogBarrier(const DVec& vss,int id,Grad* f,Hess* h) const;
  //area
  T areaLogBarrier(const DVec& vss,DVec* grad,SMat* hess) const;
  T areaLogBarrier(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,Grad* f,Hess* h) const;
  T area(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,Grad* f,Hess* h) const;
  //arap
  T arap(const DVec& vss,DVec* grad,SMat* hess) const;
  T arap(const DVec& vss,const F& d,const Eigen::Matrix<int,3,1>& iss,Grad* f,Hess* h) const;
  void fFunc(const DVec& vss,const F& d,const Eigen::Matrix<int,3,1>& iss,FFlat& f,FGrad* fg) const;
  Vert2 Proj(const Vert3& a,const Vert3& t1,const Vert3& t2) const;
  F F0(const Vert2 v[3]) const;
  F F0(const Vert3 v[3]) const;
  void computeWeights();
  static void addStructuredBlock(DVec& grad,const Eigen::Matrix<int,2,1>& id,const Grad& gi);
  static void addStructuredBlock(Trips& trips,const Eigen::Matrix<int,2,1>& id,const Hess& hi);
  static void addStructuredBlock(DVec& grad,const Eigen::Matrix<int,3,1>& id,const Grad& gi);
  static void addStructuredBlock(Trips& trips,const Eigen::Matrix<int,3,1>& id,const Hess& hi);
  static void addBlock(Trips& trips,int row,int col,const DMat& D);
  std::shared_ptr<Shape> createShape(const DVec& vss) const;
  //data
  T _convexMargin;
  DVec _vss0,_tss0;
  std::vector<F> _Fss;
  std::vector<T> _l1Coefss,_arapCoefss;
  std::vector<Eigen::Matrix<int,3,1>> _iss,_bss;
  std::vector<std::shared_ptr<Shape>> _history;
  std::shared_ptr<SSP> _SSP;
};

}

#endif
