#include "PatchDeformer.h"
#include <unordered_map>
#include <unordered_set>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/CompositeShape.h>
#include <iostream>

namespace DRAWER {
//boundary identifier
void hash_combine(std::size_t& seed,size_t v) {
  seed^=v+0x9e3779b9+(seed<<6)+(seed>>2);
}
struct HashPair {
  size_t operator()(const std::pair<int,int>& key) const {
    size_t seed=0;
    hash_combine(seed,std::hash<int>()(key.first));
    hash_combine(seed,std::hash<int>()(key.second));
    return seed;
  }
};
void addNeighbor(std::unordered_map<int,std::pair<int,int>>& bdNeighbor,std::pair<int,int> edge) {
  if(bdNeighbor.find(edge.first)==bdNeighbor.end())
    bdNeighbor[edge.first]=std::make_pair(edge.second,-1);
  else {
    ASSERT(bdNeighbor[edge.first].second==-1)
    bdNeighbor[edge.first].second=edge.second;
  }

  if(bdNeighbor.find(edge.second)==bdNeighbor.end())
    bdNeighbor[edge.second]=std::make_pair(edge.first,-1);
  else {
    ASSERT(bdNeighbor[edge.second].second==-1)
    bdNeighbor[edge.second].second=edge.first;
  }
}
std::vector<int> findRing(const std::unordered_map<int,std::pair<int,int>>& bdNeighbor) {
  std::vector<int> ret;
  std::unordered_set<int> visited;
  ret.push_back(bdNeighbor.begin()->first);
  visited.insert(bdNeighbor.begin()->first);
  while(true) {
    ASSERT(bdNeighbor.find(ret.back())!=bdNeighbor.end())
    std::pair<int,int> neigh=bdNeighbor.find(ret.back())->second;
    ASSERT(neigh.second!=-1)
    if(visited.find(neigh.first)!=visited.end() && visited.find(neigh.second)!=visited.end())
      break;
    else if(visited.find(neigh.first)!=visited.end()) {
      ret.push_back(neigh.second);
      visited.insert(neigh.second);
    } else {
      ret.push_back(neigh.first);
      visited.insert(neigh.first);
    }
  }
  return ret;
}
//PatchDeformer
PatchDeformer::PatchDeformer(const MeshVisualizer& patch2D,const MeshVisualizer& patch3D,T convexMargin) {
  ASSERT(patch2D.getComponents().size()==1 && patch3D.getComponents().size()==1)
  const MeshVisualizer::MeshComponent& comp2D=patch2D.getComponents().begin()->second;
  const MeshVisualizer::MeshComponent& comp3D=patch3D.getComponents().begin()->second;
  //vss
  _vss0.resize(comp2D._mesh->nrVertex()*2);
  for(int i=0; i<comp2D._mesh->nrVertex(); i++)
    _vss0.segment<2>(i*2)=comp2D._mesh->getVertex(i).segment<2>(0).cast<T>();
  //iss/bss
  Vert3 v[3];
  Eigen::Matrix<int,3,1> id;
  std::unordered_map<std::pair<int,int>,std::pair<int,int>,HashPair> bdEdge;
  ASSERT(comp2D._mesh->nrIndex() == comp3D._mesh->nrIndex())
  for(int i=0; i<comp2D._mesh->nrIndex(); i+=3) {
    for(int d=0; d<3; d++) {
      ASSERT(comp2D._mesh->getIndex(i+d) == comp3D._mesh->getIndex(i+d))
      id[d]=comp2D._mesh->getIndex(i+d);
      v[d]=comp3D._mesh->getVertex(id[d]).cast<T>();
      //add boundary label
      std::pair<int,int> edge(comp2D._mesh->getIndex(i+d),comp2D._mesh->getIndex(i+(d+1)%3));
      if(edge.first>edge.second)
        std::swap(edge.first,edge.second);
      if(bdEdge.find(edge)==bdEdge.end())
        bdEdge[edge]=std::make_pair(i,-1);
      else {
        ASSERT(bdEdge[edge].second==-1)
        bdEdge[edge].second=i;
      }
    }
    if(area(_vss0,id,NULL,NULL)<0)
      std::swap(id[1],id[2]);
    _Fss.push_back(F0(v).inverse());
    _iss.push_back(id);
  }
  //fetch boundary
  std::unordered_map<int,std::pair<int,int>> bdNeighbor;
  for(auto edge:bdEdge)
    if(edge.second.second==-1)
      addNeighbor(bdNeighbor,edge.first);
  //connect boundary into a ring
  std::vector<int> ring=findRing(bdNeighbor);
  //compute convex score
  T scoreForward=0,scoreBackward=0;
  for(int i=0; i<(int)ring.size(); i++) {
    int curr=ring[i];
    int next=ring[(i+1)%ring.size()];
    int last=ring[(i+ring.size()-1)%ring.size()];
    scoreForward+=area(_vss0,Eigen::Matrix<int,3,1>(curr,next,last),NULL,NULL);
    scoreBackward+=area(_vss0,Eigen::Matrix<int,3,1>(curr,last,next),NULL,NULL);
  }
  _convexMargin=0;
  for(int i=0; i<(int)ring.size(); i++) {
    T score=0;
    int curr=ring[i];
    int next=ring[(i+1)%ring.size()];
    int last=ring[(i+ring.size()-1)%ring.size()];
    if(scoreForward>scoreBackward) {
      _bss.push_back(Eigen::Matrix<int,3,1>(curr,next,last));
      score=area(_vss0,Eigen::Matrix<int,3,1>(curr,next,last),NULL,NULL);
    } else {
      _bss.push_back(Eigen::Matrix<int,3,1>(curr,last,next));
      score=area(_vss0,Eigen::Matrix<int,3,1>(curr,last,next),NULL,NULL);
    }
    _convexMargin=std::min(_convexMargin,score);
  }
  _convexMargin-=convexMargin;
  //compute l1 energy's edge length coefficient
  T maxL1Coefss=0;
  _l1Coefss.resize(_bss.size());
  for(int i=0; i<(int)_l1Coefss.size(); i++) {
    Vert2 v[2];
    for(int d=0; d<2; d++)
      v[d]=_vss0.segment<2>(_bss[i][d]*2);
    _l1Coefss[i]=(v[1]-v[0]).norm();
    maxL1Coefss=std::max<T>(maxL1Coefss,_l1Coefss[i]);
  }
  for(int i=0; i<(int)_l1Coefss.size(); i++)
    _l1Coefss[i]/=maxL1Coefss;

}
bool PatchDeformer::optimize(T epsl1,T wl1,T wArea,T wConvex,T wArap,int maxIter,T tol,bool callback,bool visualize) {
  int it=0;
  DVec grad;
  SMat hess,id;
  DVec vss=_vss0,dvss;
  T e=0,e2=0,diagReg=0,alpha=1.0;
  T diagRegInc=2.0,diagRegDec=0.8,diagRegMin=0,diagRegMax=0;
  T alphaInc=2.0,alphaDec=0.8,alphaMin=1e-8,lastAlpha=0;
  Eigen::SimplicialCholesky<SMat> sol;
  if(visualize)
    _history.clear();
  for(; it<maxIter; it++) {
    //compute system
    factorOutOrientation(epsl1,vss);
    e=buildEnergy(vss,epsl1,wl1,wArea,wConvex,wArap,&grad,&hess);
    if(callback)
      std::cout << "Iter=" << it << " energy=" << e << " alpha=" << alpha <<
                " LInf(grad)=" << grad.cwiseAbs().maxCoeff() <<
                " diagReg=" << diagReg << std::endl;
    if(grad.cwiseAbs().maxCoeff()<tol) {
      if(callback)
        std::cout << "Exit on LInf(grad)=" << grad.cwiseAbs().maxCoeff() << "<tol=" << tol << std::endl;
      break;
    }
    if(visualize)
      _history.push_back(createShape(vss));
    //problem specific initialization
    if(it==0) {
      //build identity
      std::vector<Eigen::Triplet<T>> trips;
      for(int r=0; r<hess.rows(); r++)
        trips.push_back(Eigen::Triplet<T>(r,r,1));
      id.resize(hess.rows(),hess.cols());
      id.setFromTriplets(trips.begin(),trips.end());
      //build diag0
      for(int k=0; k<hess.outerSize(); ++k)
        for(typename SMat::InnerIterator it(hess,k); it; ++it)
          diagReg=std::max<T>(diagReg,abs(it.value()));
      diagRegMin=diagReg*1e-4;
      diagRegMax=diagReg*1e4;
    }
    //compute search direction
    while(true) {
      //factorize hessian
      while(true) {
        sol.compute(hess+id*diagReg);
        if(sol.info()!=Eigen::Success) {
          diagReg*=diagRegInc;
        } else {
          diagReg=std::max<T>(diagReg*diagRegDec,diagRegMin);
          break;
        }
      }
      dvss=-sol.solve(grad);
      //line search
      lastAlpha=alpha;
      while(alpha>alphaMin) {
        e2=buildEnergy(vss+dvss*alpha,epsl1,wl1,wArea,wConvex,wArap,NULL,NULL);
        if(e2<e) {
          vss+=dvss*alpha;
          alpha*=alphaInc;
          break;
        } else {
          alpha*=alphaDec;
        }
      }
      if(alpha>alphaMin)
        break;
      else if(diagReg<diagRegMax) {
        diagReg*=diagRegInc;
        alpha=lastAlpha;
      } else break;
    }
    if(alpha<=alphaMin && diagReg>=diagRegMax) {
      if(callback)
        std::cout << "Exit on alpha=" << alpha << "<alphaMin=" << alphaMin <<
                  " and diagReg=" << diagReg << ">=diagRegMax=" << diagRegMax << std::endl;
      break;
    }
  }
  return it<maxIter;
}
const std::vector<std::shared_ptr<Shape>>& PatchDeformer::getOptimizeHistory() const {
  return _history;
}
void PatchDeformer::debugL1(T eps,T DELTA) const {
  {
    Hess hess;
    Grad grad,grad2;
    DVec vss=DVec::Random(4),dvss=DVec::Random(4);
    T e0=l1(eps,vss,Eigen::Matrix<int,2,1>(0,1),&grad,&hess);
    T e1=l1(eps,vss+dvss*DELTA,Eigen::Matrix<int,2,1>(0,1),&grad2,NULL);
    std::cout << "l1 gradient: " << grad.segment<4>(0).dot(dvss) << " error: " << grad.segment<4>(0).dot(dvss)-(e1-e0)/DELTA << std::endl;
    std::cout << "l1 hessian: " << (hess.block<4,4>(0,0)*dvss).norm() << " error: " << (hess.block<4,4>(0,0)*dvss-(grad2-grad).segment<4>(0)/DELTA).norm() << std::endl;
  }
  {
    SMat hess;
    DVec grad,grad2;
    DVec vss=_vss0,dvss=DVec::Random(_vss0.size());
    T e0=l1(eps,vss,&grad,&hess);
    T e1=l1(eps,vss+dvss*DELTA,&grad2,NULL);
    std::cout << "l1 gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
    std::cout << "l1 hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
  }
}
void PatchDeformer::debugConvex(T DELTA) const {
  SMat hess;
  DVec grad,grad2;
  DVec vss=_vss0,dvss=DVec::Random(_vss0.size());
  T e0=convexLogBarrier(vss,&grad,&hess);
  T e1=convexLogBarrier(vss+dvss*DELTA,&grad2,NULL);
  std::cout << "convexLogBarrier gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
  std::cout << "convexLogBarrier hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
}
void PatchDeformer::debugArea(T DELTA) const {
  {
    Hess hess;
    Grad grad,grad2;
    while(true) {
      DVec vss=DVec::Random(6),dvss=DVec::Random(6);
      //area
      T e0=area(vss,Eigen::Matrix<int,3,1>(0,1,2),&grad,&hess);
      T e1=area(vss+dvss*DELTA,Eigen::Matrix<int,3,1>(0,1,2),&grad2,NULL);
      if(e0<=0)
        continue;
      std::cout << "area gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
      std::cout << "area hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
      //areaLogBarrier
      e0=areaLogBarrier(vss,Eigen::Matrix<int,3,1>(0,1,2),&grad,&hess);
      e1=areaLogBarrier(vss+dvss*DELTA,Eigen::Matrix<int,3,1>(0,1,2),&grad2,NULL);
      std::cout << "areaLogBarrier gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
      std::cout << "areaLogBarrier hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
      break;
    }
  }
  //areaLogBarrier
  {
    SMat hess;
    DVec grad,grad2;
    DVec vss=_vss0,dvss=DVec::Random(_vss0.size());
    T e0=areaLogBarrier(vss,&grad,&hess);
    T e1=areaLogBarrier(vss+dvss*DELTA,&grad2,NULL);
    std::cout << "areaLogBarrier gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
    std::cout << "areaLogBarrier hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
  }
}
void PatchDeformer::debugArap(T DELTA) const {
  {
    FGrad fGrad;
    F d=F::Random();
    FFlat fFlat,fFlat2;
    DVec vss=DVec::Random(6),dvss=DVec::Random(6);
    fFunc(vss,d,Eigen::Matrix<int,3,1>(0,1,2),fFlat,&fGrad);
    fFunc(vss+dvss*DELTA,d,Eigen::Matrix<int,3,1>(0,1,2),fFlat2,NULL);
    std::cout << "fFunc gradient: " << (fGrad*dvss).norm() << " error: " << (fGrad*dvss-(fFlat2-fFlat)/DELTA).norm() << std::endl;
  }
  {
    Hess hess;
    Grad grad,grad2;
    F d=F::Random();
    DVec vss=DVec::Random(6),dvss=DVec::Random(6);
    T e0=arap(vss,d,Eigen::Matrix<int,3,1>(0,1,2),&grad,&hess);
    T e1=arap(vss+dvss*DELTA,d,Eigen::Matrix<int,3,1>(0,1,2),&grad2,NULL);
    std::cout << "arap gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
    std::cout << "arap hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
  }
  //areaLogBarrier
  {
    SMat hess;
    DVec grad,grad2;
    DVec vss=_vss0,dvss=DVec::Random(_vss0.size());
    T e0=arap(vss,&grad,&hess);
    T e1=arap(vss+dvss*DELTA,&grad2,NULL);
    std::cout << "arap gradient: " << grad.dot(dvss) << " error: " << grad.dot(dvss)-(e1-e0)/DELTA << std::endl;
    std::cout << "arap hessian: " << (hess*dvss).norm() << " error: " << (hess*dvss-(grad2-grad)/DELTA).norm() << std::endl;
  }
}
//helper
PatchDeformer::T PatchDeformer::buildEnergy(const DVec& vss,T epsl1,T wl1,T wArea,T wConvex,T wArap,DVec* grad,SMat* hess) {
  T ret=0;
  if(grad)
    grad->setZero(vss.size());
  if(hess)
    hess->resize(vss.size(),vss.size());
  if(wl1>0) {
    DVec g;
    SMat h;
    ret+=l1(epsl1,vss,grad?&g:NULL,hess?&h:NULL)*wl1;
    if(grad)
      *grad+=g*wl1;
    if(hess)
      *hess+=h*wl1;
  }
  if(wArea>0) {
    DVec g;
    SMat h;
    ret+=areaLogBarrier(vss,grad?&g:NULL,hess?&h:NULL)*wArea;
    if(grad)
      *grad+=g*wArea;
    if(hess)
      *hess+=h*wArea;
  }
  if(wConvex>0) {
    DVec g;
    SMat h;
    ret+=convexLogBarrier(vss,grad?&g:NULL,hess?&h:NULL)*wConvex;
    if(grad)
      *grad+=g*wConvex;
    if(hess)
      *hess+=h*wConvex;
  }
  if(wArap>0) {
    DVec g;
    SMat h;
    ret+=arap(vss,grad?&g:NULL,hess?&h:NULL)*wArap;
    if(grad)
      *grad+=g*wArap;
    if(hess)
      *hess+=h*wArap;
  }
  return ret;
}
void PatchDeformer::factorOutOrientation(T eps,DVec& vss,int RES) {
  F rot;
  T E,bestE;
  DVec vssTmp=vss,bestVss=vss;
  E=bestE=l1(eps,vss,NULL,NULL);
  for(int r=0; r<RES; r++) {
    //rotate
    T theta=M_PI/180*(r+0.5);
    rot(0,0)=rot(1,1)=cos(theta);
    rot(0,1)=rot(1,0)=sin(theta);
    rot(0,1)=-rot(0,1);
    for(int d=0; d<vss.size(); d+=2)
      vssTmp.segment<2>(d)=rot*vss.segment<2>(d);
    //compute energy
    E=l1(eps,vssTmp,NULL,NULL);
    if(E<bestE) {
      bestE=E;
      bestVss=vssTmp;
    }
  }
  vss.swap(bestVss);
}
PatchDeformer::T PatchDeformer::l1(T eps,const DVec& vss,DVec* grad,SMat* hess) const {
  Grad fi;
  Hess hi;
  Trips trips;
  T e=0;
  if(grad)
    grad->setZero(_vss0.size());
  for(int i=0; i<(int)_bss.size(); i++) {
    Eigen::Matrix<int,2,1> bi(_bss[i][0],_bss[i][1]);
    e+=l1(eps,vss,bi,grad?&fi:NULL,hess?&hi:NULL)*_l1Coefss[i];
    if(grad)
      addStructuredBlock(*grad,bi,fi*_l1Coefss[i]);
    if(hess)
      addStructuredBlock(trips,bi,hi*_l1Coefss[i]);
  }
  if(hess) {
    hess->resize(_vss0.size(),_vss0.size());
    hess->setFromTriplets(trips.begin(),trips.end());
  }
  return e;
}
PatchDeformer::T PatchDeformer::l1(T eps,const DVec& vss,const Eigen::Matrix<int,2,1>& iss,Grad* f,Hess* h) const {
#define ID(VID,DID) (iss[VID]*2+DID)
  Vert2 v[2];
  for(int d=0; d<2; d++)
    v[d]=vss.segment<2>(iss[d]*2);
  //temporary terms
  Vert2 d=v[1]-v[0];
  T sx=sqrt(d[0]*d[0]+eps);
  T sy=sqrt(d[1]*d[1]+eps);
  T sxy=sqrt(d[0]*d[0]+d[1]*d[1]+eps);
  //grad/hess
  F h2;
  Vert2 f2;
  if(f) {
    f2[0]=d[0]/sx-d[0]/sxy;
    f2[1]=d[1]/sy-d[1]/sxy;
    f->segment<2>(0)=-f2;
    f->segment<2>(2)= f2;
  }
  if(h) {
    h2(0,0)=d[0]*d[0]/sxy/sxy/sxy+1/sx-d[0]*d[0]/sx/sx/sx-1/sxy;
    h2(1,1)=d[1]*d[1]/sxy/sxy/sxy+1/sy-d[1]*d[1]/sy/sy/sy-1/sxy;
    h2(1,0)=d[0]*d[1]/sxy/sxy/sxy;
    h2(0,1)=d[0]*d[1]/sxy/sxy/sxy;

    h->block<2,2>(0,0)=h2;
    h->block<2,2>(2,2)=h2;
    h->block<2,2>(0,2)=-h2;
    h->block<2,2>(2,0)=-h2;
  }
  return sx+sy-sxy;
#undef ID
}
PatchDeformer::T PatchDeformer::convexLogBarrier(const DVec& vss,DVec* grad,SMat* hess) const {
  Grad fi;
  Hess hi;
  Trips trips;
  T e=0;
  if(grad)
    grad->setZero(_vss0.size());
  for(int i=0; i<(int)_bss.size(); i++) {
    e+=convexLogBarrier(vss,i,grad?&fi:NULL,hess?&hi:NULL);
    if(grad)
      addStructuredBlock(*grad,_bss[i],fi);
    if(hess)
      addStructuredBlock(trips,_bss[i],hi);
  }
  if(hess) {
    hess->resize(_vss0.size(),_vss0.size());
    hess->setFromTriplets(trips.begin(),trips.end());
  }
  return e;
}
PatchDeformer::T PatchDeformer::convexLogBarrier(const DVec& vss,int id,Grad* f,Hess* h) const {
  T a=area(vss,_bss[id],f,h);
  if(f)
    *f=-*f/a;
  if(h)
    *h=-*h/a+*f*f->transpose();
  return -log(a);
}
PatchDeformer::T PatchDeformer::areaLogBarrier(const DVec& vss,DVec* grad,SMat* hess) const {
  Grad fi;
  Hess hi;
  Trips trips;
  T e=0;
  if(grad)
    grad->setZero(_vss0.size());
  for(int i=0; i<(int)_iss.size(); i++) {
    e+=areaLogBarrier(vss,_iss[i],grad?&fi:NULL,hess?&hi:NULL);
    if(grad)
      addStructuredBlock(*grad,_iss[i],fi);
    if(hess)
      addStructuredBlock(trips,_iss[i],hi);
  }
  if(hess) {
    hess->resize(_vss0.size(),_vss0.size());
    hess->setFromTriplets(trips.begin(),trips.end());
  }
  return e;
}
PatchDeformer::T PatchDeformer::areaLogBarrier(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,Grad* grad,Hess* hess) const {
  T e=area(vss,iss,grad,hess);
  if(grad)
    *grad=-*grad/e;
  if(hess)
    *hess=-*hess/e+*grad*grad->transpose();
  return -log(e);
}
PatchDeformer::T PatchDeformer::area(const DVec& vss,const Eigen::Matrix<int,3,1>& iss,Grad* grad,Hess* hess) const {
#define ID(VID,DID) ((VID)*2+DID)
  Vert2 v[3];
  for(int d=0; d<3; d++)
    v[d]=vss.segment<2>(iss[d]*2);
  //area
  F d;
  d.col(0)=v[1]-v[0];
  d.col(1)=v[2]-v[0];
  //f
  if(grad) {
    grad->setZero();
    (*grad)[ID(1,0)]+=d(1,1);
    (*grad)[ID(0,0)]-=d(1,1);
    (*grad)[ID(2,1)]+=d(0,0);
    (*grad)[ID(0,1)]-=d(0,0);
    (*grad)[ID(1,1)]-=d(0,1);
    (*grad)[ID(0,1)]+=d(0,1);
    (*grad)[ID(2,0)]-=d(1,0);
    (*grad)[ID(0,0)]+=d(1,0);
  }
  //h
  if(hess) {
    hess->setZero();
    (*hess)(ID(1,0),ID(2,1))+=1;
    (*hess)(ID(2,1),ID(1,0))+=1;
    (*hess)(ID(0,0),ID(0,1))+=1;
    (*hess)(ID(0,1),ID(0,0))+=1;
    (*hess)(ID(0,0),ID(2,1))-=1;
    (*hess)(ID(2,1),ID(0,0))-=1;
    (*hess)(ID(1,0),ID(0,1))-=1;
    (*hess)(ID(0,1),ID(1,0))-=1;

    (*hess)(ID(1,1),ID(2,0))-=1;
    (*hess)(ID(2,0),ID(1,1))-=1;
    (*hess)(ID(0,1),ID(0,0))-=1;
    (*hess)(ID(0,0),ID(0,1))-=1;
    (*hess)(ID(0,1),ID(2,0))+=1;
    (*hess)(ID(2,0),ID(0,1))+=1;
    (*hess)(ID(1,1),ID(0,0))+=1;
    (*hess)(ID(0,0),ID(1,1))+=1;
  }
  return d.determinant();
#undef ID
}
PatchDeformer::T PatchDeformer::arap(const DVec& vss,DVec* grad,SMat* hess) const {
  Grad fi;
  Hess hi;
  Trips trips;
  T e=0;
  if(grad)
    grad->setZero(_vss0.size());
  for(int i=0; i<(int)_iss.size(); i++) {
    e+=arap(vss,_Fss[i],_iss[i],grad?&fi:NULL,hess?&hi:NULL);
    if(grad)
      addStructuredBlock(*grad,_iss[i],fi);
    if(hess)
      addStructuredBlock(trips,_iss[i],hi);
  }
  if(hess) {
    hess->resize(_vss0.size(),_vss0.size());
    hess->setFromTriplets(trips.begin(),trips.end());
  }
  return e;
}
PatchDeformer::T PatchDeformer::arap(const DVec& vss,const F& d,const Eigen::Matrix<int,3,1>& iss,Grad* grad,Hess* hess) const {
  FFlat fFlat;
  FGrad fgrad;
  fFunc(vss,d,iss,fFlat,grad?&fgrad:NULL);
  //SVD
  Eigen::JacobiSVD<Eigen::Matrix<double,2,2>> SVD
      (Eigen::Map<F>(fFlat.data()).cast<double>(),
       Eigen::ComputeFullU|Eigen::ComputeFullV);
  F Q=(SVD.matrixU()*SVD.matrixV().transpose()).cast<T>();
  Eigen::Map<FFlat> QFlat(Q.data());
  if(grad)
    *grad=fgrad.transpose()*(fFlat-QFlat);
  if(hess)
    *hess=fgrad.transpose()*fgrad;
  return (fFlat-QFlat).squaredNorm()/2;
}
void PatchDeformer::fFunc(const DVec& vss,const F& d,const Eigen::Matrix<int,3,1>& iss,FFlat& f,FGrad* fg) const {
#define ID(VID,DID) ((VID)*2+DID)
  Vert2 v[3];
  for(int d=0; d<3; d++)
    v[d]=vss.segment<2>(iss[d]*2);
  //F
  Eigen::Map<F>(f.data())=F0(v)*d;
  //FGrad
  if(fg) {
    (*fg).setZero();
    //00
    (*fg)(0,ID(1,0))+=d(0,0);
    (*fg)(0,ID(0,0))-=d(0,0);
    (*fg)(0,ID(2,0))+=d(1,0);
    (*fg)(0,ID(0,0))-=d(1,0);
    //10
    (*fg)(1,ID(1,1))+=d(0,0);
    (*fg)(1,ID(0,1))-=d(0,0);
    (*fg)(1,ID(2,1))+=d(1,0);
    (*fg)(1,ID(0,1))-=d(1,0);
    //01
    (*fg)(2,ID(1,0))+=d(0,1);
    (*fg)(2,ID(0,0))-=d(0,1);
    (*fg)(2,ID(2,0))+=d(1,1);
    (*fg)(2,ID(0,0))-=d(1,1);
    //11
    (*fg)(3,ID(1,1))+=d(0,1);
    (*fg)(3,ID(0,1))-=d(0,1);
    (*fg)(3,ID(2,1))+=d(1,1);
    (*fg)(3,ID(0,1))-=d(1,1);
  }
#undef ID
}
PatchDeformer::Vert2 PatchDeformer::Proj(const Vert3& a,const Vert3& t1,const Vert3& t2) const {
  return Vert2(a.dot(t1),a.dot(t2));
}
PatchDeformer::F PatchDeformer::F0(const Vert2 v[3]) const {
  F ret;
  ret.col(0)=v[1]-v[0];
  ret.col(1)=v[2]-v[0];
  return ret;
}
PatchDeformer::F PatchDeformer::F0(const Vert3 v[3]) const {
  Vert3 n=(v[1]-v[0]).cross(v[2]-v[0]).normalized();
  Vert3::StorageIndex id;
  n.cwiseAbs().minCoeff(&id);

  Vert3 t1=Vert3::Unit(id).cross(n).normalized();
  Vert3 t2=n.cross(t1).normalized();

  F ret;
  ret.col(0)=Proj(v[1],t1,t2)-Proj(v[0],t1,t2);
  ret.col(1)=Proj(v[2],t1,t2)-Proj(v[0],t1,t2);
  return ret;
}
void PatchDeformer::addStructuredBlock(DVec& grad,const Eigen::Matrix<int,2,1>& id,const Grad& gi) {
  for(int d=0; d<2; d++)
    grad.segment<2>(id[d]*2)+=gi.segment<2>(d*2);
}
void PatchDeformer::addStructuredBlock(Trips& trips,const Eigen::Matrix<int,2,1>& id,const Hess& hi) {
  for(int r=0; r<2; r++)
    for(int c=0; c<2; c++)
      addBlock(trips,id[r]*2,id[c]*2,hi.block<2,2>(r*2,c*2));
}
void PatchDeformer::addStructuredBlock(DVec& grad,const Eigen::Matrix<int,3,1>& id,const Grad& gi) {
  for(int d=0; d<3; d++)
    grad.segment<2>(id[d]*2)+=gi.segment<2>(d*2);
}
void PatchDeformer::addStructuredBlock(Trips& trips,const Eigen::Matrix<int,3,1>& id,const Hess& hi) {
  for(int r=0; r<3; r++)
    for(int c=0; c<3; c++)
      addBlock(trips,id[r]*2,id[c]*2,hi.block<2,2>(r*2,c*2));
}
void PatchDeformer::addBlock(Trips& trips,int row,int col,const DMat& D) {
  for(int r=0; r<D.rows(); r++)
    for(int c=0; c<D.cols(); c++)
      trips.push_back(Eigen::Triplet<T>(row+r,col+c,D(r,c)));
}
std::shared_ptr<Shape> PatchDeformer::createShape(const DVec& vss) const {
  std::shared_ptr<MeshShape> mesh(new MeshShape);
  mesh->setUseLight(false);
  mesh->setMode(GL_TRIANGLES);
  mesh->setColorAmbient(GL_TRIANGLES,1,0,0);
  for(int i=0; i<vss.size(); i+=2)
    mesh->addVertex(Eigen::Matrix<GLfloat,3,1>((GLfloat)vss[i+0],(GLfloat)vss[i+1],0));
  for(int i=0; i<(int)_iss.size(); i++)
    mesh->addIndex(_iss[i]);

  std::shared_ptr<MeshShape> line(new MeshShape);
  line->setUseLight(false);
  line->setLineWidth(2);
  line->setMode(GL_LINES);
  line->setColorAmbient(GL_LINES,0,0,0);
  for(int i=0; i<vss.size(); i+=2)
    line->addVertex(Eigen::Matrix<GLfloat,3,1>((GLfloat)vss[i+0],(GLfloat)vss[i+1],0));
  for(int i=0; i<(int)_iss.size(); i++) {
    line->addIndex(Eigen::Matrix<int,2,1>(_iss[i][0],_iss[i][1]));
    line->addIndex(Eigen::Matrix<int,2,1>(_iss[i][0],_iss[i][2]));
    line->addIndex(Eigen::Matrix<int,2,1>(_iss[i][1],_iss[i][2]));
  }

  std::shared_ptr<CompositeShape> ret(new CompositeShape);
  ret->addShape(mesh);
  ret->addShape(line);
  return ret;
}
}
