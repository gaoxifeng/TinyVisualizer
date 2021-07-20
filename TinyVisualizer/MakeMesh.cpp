#include "MakeMesh.h"

namespace DRAWER {
std::shared_ptr<MeshShape> makeSquare(bool fill,const Eigen::Matrix<GLfloat,2,1>& halfExt,GLfloat depth) {
  std::shared_ptr<MeshShape> shape(new MeshShape);
  Eigen::Matrix<GLfloat,2,1> tc;
  tc=Eigen::Matrix<GLfloat,2,1>(0,0);
  shape->addVertex(Eigen::Matrix<GLfloat,3,1>(-halfExt[0],-halfExt[1],depth),&tc);
  tc=Eigen::Matrix<GLfloat,2,1>(1,0);
  shape->addVertex(Eigen::Matrix<GLfloat,3,1>( halfExt[0],-halfExt[1],depth),&tc);
  tc=Eigen::Matrix<GLfloat,2,1>(1,1);
  shape->addVertex(Eigen::Matrix<GLfloat,3,1>( halfExt[0], halfExt[1],depth),&tc);
  tc=Eigen::Matrix<GLfloat,2,1>(0,1);
  shape->addVertex(Eigen::Matrix<GLfloat,3,1>(-halfExt[0], halfExt[1],depth),&tc);
  shape->addIndex(Eigen::Matrix<int,4,1>(0,1,2,3));
  shape->setMode(fill?GL_QUADS:GL_LINE_LOOP);
  return shape;
}
std::shared_ptr<MeshShape> makeCircle(int RES,bool fill,const Eigen::Matrix<GLfloat,2,1>& pos,GLfloat rad) {
  std::shared_ptr<MeshShape> shape(new MeshShape);
  for(int i=0; i<RES; i++) {
    GLfloat angle=M_PI*2*i/RES;
    shape->addVertex(Eigen::Matrix<GLfloat,3,1>(cos(angle)*rad+pos[0],sin(angle)*rad+pos[1],0));
    shape->addIndexSingle(i);
  }
  shape->setMode(fill?GL_TRIANGLE_FAN:GL_LINE_LOOP);
  return shape;
}
void makeGridLine(MeshShape& mesh,int RES,const Eigen::Matrix<GLfloat,3,1>& ctr,const Eigen::Matrix<GLfloat,3,1>& d) {
#define ID(X,Y) (Y)+off
  //skip degenerate case
  if(d.isZero())
    return;
  int off=mesh.nrVertex();
  for(int y=0; y<=RES; y++) {
    mesh.addVertex(ctr+d*(2*y/(GLfloat)RES-1));
    if(y<RES)
      mesh.addIndex(Eigen::Matrix<int,2,1>(ID(x,y),ID(x,y+1)));
  }
#undef ID
}
void makeGrid(MeshShape& mesh,int RESX,int RESY,bool fill,const Eigen::Matrix<GLfloat,3,1>& ctr,const Eigen::Matrix<GLfloat,3,1>& d0,const Eigen::Matrix<GLfloat,3,1>& d1) {
  if(fill) {
#define ID(X,Y) (X)*(RESY+1)+(Y)+off
    //skip degenerate case
    if(d0.isZero() || d1.isZero())
      return;
    GLfloat coefTC=1/(GLfloat)std::max(RESX,RESY);
    int off=mesh.nrVertex();
    for(int x=0; x<=RESX; x++)
      for(int y=0; y<=RESY; y++) {
        Eigen::Matrix<GLfloat,2,1> tc(x*coefTC,y*coefTC);
        mesh.addVertex(ctr+d0*(2*x/(GLfloat)RESX-1)+d1*(2*y/(GLfloat)RESY-1),&tc);
        if(x<RESX && y<RESY) {
          mesh.addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y),ID(x+1,y+1)));
          mesh.addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y+1),ID(x,y+1)));
        }
      }
#undef ID
  } else {
    makeGridLine(mesh,RESX,ctr-d1,d0);
    makeGridLine(mesh,RESX,ctr+d1,d0);
    makeGridLine(mesh,RESY,ctr-d0,d1);
    makeGridLine(mesh,RESY,ctr+d0,d1);
  }
}
void makeGrid(MeshShape& mesh,int RES,bool fill,const Eigen::Matrix<GLfloat,3,1>& ctr,const Eigen::Matrix<GLfloat,3,1>& d0,const Eigen::Matrix<GLfloat,3,1>& d1) {
  makeGrid(mesh,RES,RES,fill,ctr,d0,d1);
}
std::shared_ptr<MeshShape> makeBox(int RES,bool fill,const Eigen::Matrix<GLfloat,3,1>& halfExt) {
  std::shared_ptr<MeshShape> shape(new MeshShape);
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>(-halfExt[0],0,0),Eigen::Matrix<GLfloat,3,1>(0,0,halfExt[2]),Eigen::Matrix<GLfloat,3,1>(0,halfExt[1],0));
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>( halfExt[0],0,0),Eigen::Matrix<GLfloat,3,1>(0,halfExt[1],0),Eigen::Matrix<GLfloat,3,1>(0,0,halfExt[2]));
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>(0,-halfExt[1],0),Eigen::Matrix<GLfloat,3,1>(halfExt[0],0,0),Eigen::Matrix<GLfloat,3,1>(0,0,halfExt[2]));
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>(0, halfExt[1],0),Eigen::Matrix<GLfloat,3,1>(0,0,halfExt[2]),Eigen::Matrix<GLfloat,3,1>(halfExt[0],0,0));
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>(0,0,-halfExt[2]),Eigen::Matrix<GLfloat,3,1>(0,halfExt[1],0),Eigen::Matrix<GLfloat,3,1>(halfExt[0],0,0));
  makeGrid(*shape,RES,fill,Eigen::Matrix<GLfloat,3,1>(0,0, halfExt[2]),Eigen::Matrix<GLfloat,3,1>(halfExt[0],0,0),Eigen::Matrix<GLfloat,3,1>(0,halfExt[1],0));
  shape->setMode(fill?GL_TRIANGLES:GL_LINES);
  if(fill)
    shape->computeNormals();
  return shape;
}
std::shared_ptr<MeshShape> makeSphericalBox(int RES,bool fill,GLfloat rad,const Eigen::Matrix<GLfloat,3,1>& halfExt) {
  int off=0,off2=0;
  std::shared_ptr<MeshShape> shape(new MeshShape);
  for(int x=-1; x<=1; x++)
    for(int y=-1; y<=1; y++)
      for(int z=-1; z<=1; z++) {
        int nonZero=0;
        if(x!=0)
          nonZero++;
        if(y!=0)
          nonZero++;
        if(z!=0)
          nonZero++;
        if(nonZero>0) {
          //-x
          if(x==-1)
            makeGrid(*shape,RES*(2-std::abs(z))/2,RES*(2-std::abs(y))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x,y/2.,z/2.),
                     Eigen::Matrix<GLfloat,3,1>(0,0,z==0?halfExt[2]:1/2.),
                     Eigen::Matrix<GLfloat,3,1>(0,y==0?halfExt[1]:1/2.,0));
          //+x
          if(x==1)
            makeGrid(*shape,RES*(2-std::abs(y))/2,RES*(2-std::abs(z))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x,y/2.,z/2.),
                     Eigen::Matrix<GLfloat,3,1>(0,y==0?halfExt[1]:1/2.,0),
                     Eigen::Matrix<GLfloat,3,1>(0,0,z==0?halfExt[2]:1/2.));
          //-y
          if(y==-1)
            makeGrid(*shape,RES*(2-std::abs(x))/2,RES*(2-std::abs(z))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x/2.,y,z/2.),
                     Eigen::Matrix<GLfloat,3,1>(x==0?halfExt[0]:1/2.,0,0),
                     Eigen::Matrix<GLfloat,3,1>(0,0,z==0?halfExt[2]:1/2.));
          //+y
          if(y==1)
            makeGrid(*shape,RES*(2-std::abs(z))/2,RES*(2-std::abs(x))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x/2.,y,z/2.),
                     Eigen::Matrix<GLfloat,3,1>(0,0,z==0?halfExt[2]:1/2.),
                     Eigen::Matrix<GLfloat,3,1>(x==0?halfExt[0]:1/2.,0,0));
          //-z
          if(z==-1)
            makeGrid(*shape,RES*(2-std::abs(y))/2,RES*(2-std::abs(x))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x/2.,y/2.,z),
                     Eigen::Matrix<GLfloat,3,1>(0,y==0?halfExt[1]:1/2.,0),
                     Eigen::Matrix<GLfloat,3,1>(x==0?halfExt[0]:1/2.,0,0));
          //+z
          if(z==1)
            makeGrid(*shape,RES*(2-std::abs(x))/2,RES*(2-std::abs(y))/2,fill,
                     Eigen::Matrix<GLfloat,3,1>(x/2.,y/2.,z),
                     Eigen::Matrix<GLfloat,3,1>(x==0?halfExt[0]:1/2.,0,0),
                     Eigen::Matrix<GLfloat,3,1>(0,y==0?halfExt[1]:1/2.,0));
        }
        //assemble
        off2=shape->nrVertex();
        for(int i=off; i<off2; i++) {
          Eigen::Map<Eigen::Matrix<GLfloat,3,1>> n=shape->getNormal(i);
          Eigen::Map<Eigen::Matrix<GLfloat,3,1>> v=shape->getVertex(i);
          n.setZero();
          if(x!=0)
            n[0]=v[0];
          if(y!=0)
            n[1]=v[1];
          if(z!=0)
            n[2]=v[2];
          n.normalize();
          if(x!=0)
            v[0]=x*halfExt[0]+n[0]*rad;
          else v[0]+=x*rad;
          if(y!=0)
            v[1]=y*halfExt[1]+n[1]*rad;
          else v[1]+=y*rad;
          if(z!=0)
            v[2]=z*halfExt[2]+n[2]*rad;
          else v[2]+=z*rad;
        }
        off=off2;
      }
  shape->setMode(fill?GL_TRIANGLES:GL_LINES);
  return shape;
}
std::shared_ptr<MeshShape> makeCapsule(int RES,bool fill,GLfloat rad,GLfloat height) {
  return makeSphericalBox(RES,fill,rad,Eigen::Matrix<GLfloat,3,1>(0,0,height));
}
std::shared_ptr<MeshShape> makeSphere(int RES,bool fill,GLfloat rad) {
  return makeSphericalBox(RES,fill,rad,Eigen::Matrix<GLfloat,3,1>(0,0,0));
}
std::shared_ptr<MeshShape> makeTriMesh(bool fill,const Eigen::Matrix<GLfloat,-1,-1>& V,const Eigen::Matrix<int,-1,-1>& F) {
  std::shared_ptr<MeshShape> shape(new MeshShape);
  for (int i=0; i<V.rows(); i++)
    shape->addVertex(V.row(i));
  for (int i=0; i<F.rows(); i++)
    shape->addIndex(F.row(i));
  shape->setMode(fill?GL_TRIANGLES:GL_LINES);
  return shape;
}
std::shared_ptr<MeshShape> makeWires(const Eigen::Matrix<GLfloat,-1,-1>& V,const Eigen::Matrix<int,-1,-1>& E) {
  std::shared_ptr<MeshShape> shape(new MeshShape);
  for(int i=0; i<V.rows(); i++)
    shape->addVertex(V.row(i));
  for(int i=0; i<E.rows(); i++)
    shape->addIndex(E.row(i));
  shape->setMode(GL_LINES);
  return shape;
}
}
