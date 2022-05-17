#include "RayCaster.h"
#include "MeshVisualizer.h"
#include <TinyVisualizer/MakeMesh.h>
#include <stb/stb_image_write.h>
#include <iostream>
#include <stack>

namespace DRAWER {
RayCaster::Triangle::Triangle() {}
RayCaster::Triangle::Triangle(int v0,int v1,int v2,int t,const Eigen::Matrix<GLdouble,4,1>& defaultColor):_defaultColor(defaultColor),_vid(v0,v1,v2),_tid(t) {}
void RayCaster::Triangle::computeNormal(const std::vector<Eigen::Matrix<GLdouble,3,1>>& vss) {
  const Eigen::Matrix<GLdouble,3,1>& a=vss[_vid[0]];
  const Eigen::Matrix<GLdouble,3,1>& b=vss[_vid[1]];
  const Eigen::Matrix<GLdouble,3,1>& c=vss[_vid[2]];
  _normal=(b-a).cross(c-a).normalized();
}
RayCaster::RayCaster(const MeshVisualizer& mesh,bool useTC) {
  for(const auto& component:mesh.getComponents()) {
    const MeshVisualizer::MeshComponent& comp=component.second;
    int off=(int)_vss.size();
    //build Vertex
    for(int i=0; i<comp._mesh->nrVertex(); i++) {
      _vss.push_back(comp._mesh->getVertex(i).cast<GLdouble>());
      _tcss.push_back(comp._mesh->getTexcoord(i).cast<GLdouble>());
    }
    //build Triangle
    for(int i=0; i<comp._mesh->nrIndex(); i+=3) {
      int tid=comp._texture ? (int)_textures.size() : -1;
      _triss.push_back(Triangle(comp._mesh->getIndex(i+0)+off,
                                comp._mesh->getIndex(i+1)+off,
                                comp._mesh->getIndex(i+2)+off,
                                tid,comp._defaultColor.cast<GLdouble>()));
      _triss.back().computeNormal(_vss);
      //build BVH
      Node n;
      n._nrCell=1;
      n._cell=(int)_triss.size()-1;
      n._bb=resetBBD();
      if(useTC) {
        n._bb=unionBB(n._bb,_tcss[_triss.back()._vid[0]]);
        n._bb=unionBB(n._bb,_tcss[_triss.back()._vid[1]]);
        n._bb=unionBB(n._bb,_tcss[_triss.back()._vid[2]]);
      } else {
        n._bb=unionBB(n._bb,_vss[_triss.back()._vid[0]]);
        n._bb=unionBB(n._bb,_vss[_triss.back()._vid[1]]);
        n._bb=unionBB(n._bb,_vss[_triss.back()._vid[2]]);
      }
      _bvh.push_back(n);
    }
    if(comp._texture) {
      comp._texture->loadCPUData();
      _textures.push_back(comp._texture);
    }
  }
  BVHBuilder().buildBVH(_bvh);
  for(int i=(int)_triss.size(); i<(int)_bvh.size(); i++)
    _bvh[i]._cell=-1;
  std::cout << "BVH optimal depth=" << BVHBuilder::depthOptimal(_bvh) << " depth=" << BVHBuilder::depth(_bvh) << std::endl;
}
RayCaster::RayIntersect RayCaster::castRay(Eigen::Matrix<GLdouble,6,1>& ray) const {
  RayIntersect ret=RayIntersectNone;
  if(!ray.array().isFinite().all())
    return ret;

  std::stack<int> ss;
  ss.push((int)_bvh.size()-1);
  while(!ss.empty()) {
    int id=ss.top();
    ss.pop();
    if(!intersectBB(_bvh[id]._bb,ray))
      continue;
    else if(_bvh[id]._cell>=0) {
      if(intersect(_triss[_bvh[id]._cell],ray,ret))
        ret.first=_bvh[id]._cell;
    } else {
      ss.push(_bvh[id]._l);
      ss.push(_bvh[id]._r);
    }
  }
  return ret;
}
std::vector<RayCaster::RayIntersect> RayCaster::castRayBatched(std::vector<Eigen::Matrix<GLdouble,6,1>>& ray) const {
  std::vector<int> indices(ray.size());
  for(int i=0; i<(int)ray.size(); i++)
    indices[i]=i;
  std::stack<std::pair<int,std::vector<int>>> ss;
  ss.push(std::make_pair((int)_bvh.size()-1,indices));
  std::vector<RayCaster::RayIntersect> ret(ray.size(),RayIntersectNone);
  while(!ss.empty()) {
    int id=ss.top().first;
    indices=ss.top().second;
    ss.pop();
    //batched intersectBB/intersect
    #pragma omp parallel for
    for(int i=0; i<(int)indices.size(); i++) {
      if(!ray[indices[i]].array().isFinite().all())
        indices[i]=-1;
      else if(!intersectBB(_bvh[id]._bb,ray[indices[i]]))
        indices[i]=-1;
      else if(_bvh[id]._cell>=0) {
        if(intersect(_triss[_bvh[id]._cell],ray[indices[i]],ret[indices[i]]))
          ret[indices[i]].first=_bvh[id]._cell;
      }
    }
    //compact
    int newSz=0;
    for(int i=0; i<(int)indices.size(); i++)
      if(indices[i]>=0)
        indices[newSz++]=indices[i];
    indices.resize(newSz);
    //descend
    if(!indices.empty() && _bvh[id]._l>=0) {
      ss.push(std::make_pair(_bvh[id]._l,indices));
      ss.push(std::make_pair(_bvh[id]._r,indices));
    }
  }
  return ret;
}
Eigen::Matrix<GLdouble,3,1> RayCaster::getIntersectVert(const RayIntersect& I) const {
  ASSERT(I.first>=0)
  return _vss[_triss[I.first]._vid[0]]*I.second[0]+
         _vss[_triss[I.first]._vid[1]]*I.second[1]+
         _vss[_triss[I.first]._vid[2]]*I.second[2];
}
Eigen::Matrix<GLdouble,2,1> RayCaster::getIntersectTexcoord(const RayIntersect& I) const {
  return _tcss[_triss[I.first]._vid[0]]*I.second[0]+
         _tcss[_triss[I.first]._vid[1]]*I.second[1]+
         _tcss[_triss[I.first]._vid[2]]*I.second[2];
}
Eigen::Matrix<GLdouble,4,1> RayCaster::getIntersectColor(const RayIntersect& I) const {
  if(_triss[I.first]._tid==-1)
    return _triss[I.first]._defaultColor;
  return _textures[_triss[I.first]._tid]->getData<GLdouble>(getIntersectTexcoord(I));
}
const std::vector<Node>& RayCaster::getBVH() const {
  return _bvh;
}
const std::vector<RayCaster::Triangle>& RayCaster::getTriss() const {
  return _triss;
}
const std::vector<Eigen::Matrix<GLdouble,3,1>>& RayCaster::getVss() const {
  return _vss;
}
const std::vector<Eigen::Matrix<GLdouble,2,1>>& RayCaster::getTcss() const {
  return _tcss;
}
bool RayCaster::intersect(const Triangle& tri,Eigen::Matrix<GLdouble,6,1>& ray,RayIntersect& ret) const {
  Eigen::Matrix<GLdouble,3,1> abt,a=_vss[tri._vid[0]],b=_vss[tri._vid[1]],c=_vss[tri._vid[2]];
  Eigen::Matrix<GLdouble,3,3> mat;
  mat.col(0)=a-c;
  mat.col(1)=b-c;
  mat.col(2)=-(maxCorner(ray)-minCorner(ray));
  if(std::abs(mat.determinant())<std::numeric_limits<GLdouble>::min())
    return false;

  abt=mat.inverse()*(minCorner(ray)-c);
  bool succ=(abt.x()>=0 && abt.y()>=0 && (abt.x()+abt.y())<=1) && //in triangle
            (abt.z()>=0 && abt.z()<1);  //in segment
  if(succ) {
    ret.second[0]=abt.x();
    ret.second[1]=abt.y();
    ret.second[2]=1-abt.x()-abt.y();
    maxCorner(ray)=minCorner(ray)-abt.z()*mat.col(2);
    return true;
  }
  return false;
}
void RayCaster::castRayImage(const std::string& path,int resw,Eigen::Matrix<GLdouble,3,1> z,
                             const Eigen::Matrix<GLdouble,3,1>& g,const Eigen::Matrix<GLdouble,4,1>& bkg) const {
  z.normalize();
  Eigen::Matrix<GLdouble,3,1> x=g.cross(z).normalized();
  Eigen::Matrix<GLdouble,3,1> y=x.cross(z).normalized();

  //cast bounding box to x,y axes
  GLdouble xmin= std::numeric_limits<GLdouble>::max();
  GLdouble xmax=-std::numeric_limits<GLdouble>::max();
  GLdouble ymin= std::numeric_limits<GLdouble>::max();
  GLdouble ymax=-std::numeric_limits<GLdouble>::max();
  GLdouble zmin= std::numeric_limits<GLdouble>::max();
  GLdouble zmax=-std::numeric_limits<GLdouble>::max();
  Eigen::Matrix<GLdouble,6,1> bb=_bvh.back()._bb;
  for(GLdouble X: {
        bb[0],bb[3]
      })
    for(GLdouble Y: {
          bb[1],bb[4]
        })
      for(GLdouble Z: {
            bb[2],bb[5]
          }) {
        Eigen::Matrix<GLdouble,3,1> pt(X,Y,Z);
        GLdouble ptx=pt.dot(x),pty=pt.dot(y),ptz=pt.dot(z);
        xmin=std::min(xmin,ptx),xmax=std::max(xmax,ptx);
        ymin=std::min(ymin,pty),ymax=std::max(ymax,pty);
        zmin=std::min(ymin,ptz),zmax=std::max(ymax,ptz);
      }

  //cast rays
  int resh=resw*(ymax-ymin)/(xmax-xmin);
  std::vector<Eigen::Matrix<GLdouble,6,1>> ray(resw*resh);
  for(int h=0,off=0; h<resh; h++) {
    GLdouble ypos=interp1D(ymax,ymin,(h+0.5)/resh);
    for(int w=0; w<resw; w++,off++) {
      GLdouble xpos=interp1D(xmin,xmax,(w+0.5)/resw);
      minCorner(ray[off])=z*zmin+x*xpos+y*ypos;
      maxCorner(ray[off])=minCorner(ray[off])+z*(zmax-zmin);
    }
  }
  std::vector<RayIntersect> rayI=castRayBatched(ray);

  //fill color
  unsigned char* data=new unsigned char[resw*resh*4];
  #pragma omp parallel for
  for(int h=0; h<resh; h++)
    for(int w=0,off=h*resw; w<resw; w++,off++) {
      Eigen::Map<Eigen::Matrix<unsigned char,4,1>> color(data+4*(w+resw*h));
      if(rayI[off].first<0)
        color=(bkg*255).cast<unsigned char>();
      else color=(getIntersectColor(rayI[off])*255).cast<unsigned char>();
    }
  stbi_write_png(path.c_str(),resw,resh,4,data,resw*4);
  free(data);
}
std::vector<Eigen::Matrix<GLdouble,3,1>> RayCaster::sampleDir(int res,const Eigen::Matrix<GLdouble,3,1>& g) const {
  std::vector<Eigen::Matrix<GLdouble,3,1>> dirs,dirsUnique;
  auto mesh=makeSphere(res,true,1);
  for(int i=0; i<mesh->nrVertex(); i++) {
    Eigen::Matrix<GLdouble,3,1> v=mesh->getVertex(i).cast<GLdouble>();
    if(g.isZero() || v.dot(g)<=0)
      dirs.push_back(-v);
  }

  //find unique dir
  std::sort(dirs.begin(),dirs.end(),[&](const Eigen::Matrix<GLdouble,3,1>& a,const Eigen::Matrix<GLdouble,3,1>& b) {
    for(int d=0; d<3; d++)
      if(a[d]<b[d])
        return true;
      else if(a[d]>b[d])
        return false;
    return false;
  });
  for(int i=0; i<(int)dirs.size(); i++)
    if(i==0 || (dirs[i]-dirs[i-1]).norm()>1e-4)
      dirsUnique.push_back(dirs[i]);
  return dirsUnique;
}
std::vector<Eigen::Matrix<GLdouble,6,1>> RayCaster::sampleRay(int res,const Eigen::Matrix<GLdouble,3,1>& g) const {
  Eigen::Matrix<GLdouble,3,1> ext=(maxCorner(_bvh.back()._bb)-minCorner(_bvh.back()._bb));
  Eigen::Matrix<GLdouble,3,1> ctr=(maxCorner(_bvh.back()._bb)+minCorner(_bvh.back()._bb))/2;
  GLfloat rad=(GLfloat)ext.norm()/2;

  Eigen::Matrix<GLdouble,6,1> ray;
  std::vector<Eigen::Matrix<GLdouble,6,1>> rays,raysUnique;
  auto mesh=makeSphere(res,true,rad);
  for(int i=0; i<mesh->nrVertex(); i++) {
    Eigen::Matrix<GLdouble,3,1> v=mesh->getVertex(i).cast<GLdouble>();
    if(g.isZero() || v.dot(g)<=0) {
      maxCorner(ray)=ctr;
      minCorner(ray)=ctr+v;
      rays.push_back(ray);
    }
  }

  //find unique dir
  std::sort(rays.begin(),rays.end(),[&](const Eigen::Matrix<GLdouble,6,1>& a,const Eigen::Matrix<GLdouble,6,1>& b) {
    for(int d=0; d<3; d++)
      if(a[d]<b[d])
        return true;
      else if(a[d]>b[d])
        return false;
    return false;
  });
  for(int i=0; i<(int)rays.size(); i++)
    if(i==0 || (rays[i]-rays[i-1]).norm()>1e-4*rad)
      raysUnique.push_back(rays[i]);
  return raysUnique;
}
std::shared_ptr<MeshShape> RayCaster::drawRay(int res,const Eigen::Matrix<GLdouble,3,1>& g,bool batched) const {
  std::shared_ptr<MeshShape> mesh(new MeshShape);
  mesh->setMode(GL_LINES);
  mesh->setColor(GL_LINES,1,0,0);
  mesh->setUseLight(false);
  mesh->setLineWidth(2);

  std::vector<Eigen::Matrix<GLdouble,6,1>> ray=sampleRay(res,g);
  std::vector<RayIntersect> rayI;
  if(batched)
    rayI=castRayBatched(ray);
  else {
    for(int i=0; i<(int)ray.size(); i++)
      rayI.push_back(castRay(ray[i]));
  }

  for(int i=0; i<(int)ray.size(); i++) {
    Eigen::Matrix<GLdouble,6,1> r=ray[i];
    if(rayI[i].first<0)
      continue;
    maxCorner(r)=getIntersectVert(rayI[i]);
    mesh->addIndexSingle(mesh->nrVertex());
    mesh->addVertex(minCorner(r).cast<GLfloat>());
    mesh->addIndexSingle(mesh->nrVertex());
    mesh->addVertex(maxCorner(r).cast<GLfloat>());
  }
  return mesh;
}
RayCaster::RayIntersect RayCaster::RayIntersectNone=RayCaster::RayIntersect(-1,Eigen::Matrix<GLdouble,3,1>());
}
