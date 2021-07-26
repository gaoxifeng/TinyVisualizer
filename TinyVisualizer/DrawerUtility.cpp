#include "DrawerUtility.h"

namespace DRAWER {
Eigen::Matrix<GLfloat,6,1> resetBB() {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).setConstant(std::numeric_limits<GLfloat>::max());
  ret.segment<3>(3).setConstant(-std::numeric_limits<GLfloat>::max());
  return ret;
}
Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,6,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b.segment<3>(0));
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b.segment<3>(3));
  return ret;
}
Eigen::Matrix<GLfloat,6,1> unionBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& b) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0)=a.segment<3>(0).cwiseMin(b);
  ret.segment<3>(3)=a.segment<3>(3).cwiseMax(b);
  return ret;
}
Eigen::Matrix<GLfloat,6,1> scaleBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()*s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()*s.array();
  return ret;
}
Eigen::Matrix<GLfloat,6,1> translateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,1>& s) {
  Eigen::Matrix<GLfloat,6,1> ret;
  ret.segment<3>(0).array()=a.segment<3>(0).array()+s.array();
  ret.segment<3>(3).array()=a.segment<3>(3).array()+s.array();
  return ret;
}
Eigen::Matrix<GLfloat,6,1> rotateBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,3,3>& r) {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB();
  for(GLfloat x: {
        a[0],a[3]
      })
    for(GLfloat y: {
          a[1],a[4]
        })
      for(GLfloat z: {
            a[2],a[5]
          }) {
        Eigen::Matrix<GLfloat,3,1> pos(x,y,z);
        pos=r*pos;
        ret=unionBB(ret,pos);
      }
  return ret;
}
Eigen::Matrix<GLfloat,6,1> transformBB(const Eigen::Matrix<GLfloat,6,1>& a,const Eigen::Matrix<GLfloat,4,4>& r) {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB();
  for(GLfloat x: {
        a[0],a[3]
      })
    for(GLfloat y: {
          a[1],a[4]
        })
      for(GLfloat z: {
            a[2],a[5]
          }) {
        Eigen::Matrix<GLfloat,4,1> pos(x,y,z,1);
        pos=r*pos;
        ret=unionBB(ret,Eigen::Matrix<GLfloat,3,1>(pos.segment<3>(0)/pos[3]));
      }
  return ret;
}
void zRange(const Eigen::Matrix<GLfloat,6,1>& bb,GLfloat& zNear,GLfloat& zFar,GLfloat minZNear,GLfloat maxZFar) {
  if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max()) {
    zNear=minZNear;
    zFar=maxZFar;
    return;
  }
  zNear=maxZFar;
  zFar=minZNear;
  Eigen::Matrix<GLfloat,4,4> mv;
  glGetFloatv(GL_MODELVIEW_MATRIX,mv.data());
  for(GLfloat x: {
        bb[0],bb[3]
      })
    for(GLfloat y: {
          bb[1],bb[4]
        })
      for(GLfloat z: {
            bb[2],bb[5]
          }) {
        Eigen::Matrix<GLfloat,4,1> pos=mv*Eigen::Matrix<GLfloat,4,1>(x,y,z,1);
        GLfloat posZ=pos[2]/pos[3];
        zNear=std::min(zNear,-posZ);
        zFar=std::max(zFar,-posZ);
      }
  zNear=std::max<GLfloat>(minZNear,zNear);
  zFar=std::min<GLfloat>(maxZFar,zFar);
}
void gluLookAt(GLfloat eyex,GLfloat eyey,GLfloat eyez,
               GLfloat centerx,GLfloat centery,GLfloat centerz,
               GLfloat upx,GLfloat upy,GLfloat upz) {
  GLfloat m[16];
  GLfloat x[3],y[3],z[3];
  GLfloat mag;

  /* Make rotation matrix */

  /* Z vector */
  z[0]=eyex-centerx;
  z[1]=eyey-centery;
  z[2]=eyez-centerz;
  mag=std::sqrt(z[0]*z[0]+z[1]*z[1]+z[2]*z[2]);
  if(mag) {          /* mpichler, 19950515 */
    z[0]/=mag;
    z[1]/=mag;
    z[2]/=mag;
  }

  /* Y vector */
  y[0]=upx;
  y[1]=upy;
  y[2]=upz;

  /* X vector=Y cross Z */
  x[0]=y[1]*z[2]-y[2]*z[1];
  x[1]=-y[0]*z[2]+y[2]*z[0];
  x[2]=y[0]*z[1]-y[1]*z[0];

  /* Recompute Y=Z cross X */
  y[0]=z[1]*x[2]-z[2]*x[1];
  y[1]=-z[0]*x[2]+z[2]*x[0];
  y[2]=z[0]*x[1]-z[1]*x[0];

  /* mpichler, 19950515 */
  /* cross product gives area of parallelogram, which is < 1.0 for
  *non-perpendicular unit-length vectors; so normalize x, y here
   */

  mag=std::sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
  if(mag) {
    x[0]/=mag;
    x[1]/=mag;
    x[2]/=mag;
  }

  mag=std::sqrt(y[0]*y[0]+y[1]*y[1]+y[2]*y[2]);
  if(mag) {
    y[0]/=mag;
    y[1]/=mag;
    y[2]/=mag;
  }

#define M(row,col) m[col*4+row]
  M(0,0)=x[0];
  M(0,1)=x[1];
  M(0,2)=x[2];
  M(0,3)=0.0;
  M(1,0)=y[0];
  M(1,1)=y[1];
  M(1,2)=y[2];
  M(1,3)=0.0;
  M(2,0)=z[0];
  M(2,1)=z[1];
  M(2,2)=z[2];
  M(2,3)=0.0;
  M(3,0)=0.0;
  M(3,1)=0.0;
  M(3,2)=0.0;
  M(3,3)=1.0;
#undef M

  glMultMatrixf(m);
  glTranslatef(-eyex,-eyey,-eyez);
}
static void gluMakeIdentityf(GLfloat m[16]) {
  m[0+4*0]=1;
  m[0+4*1]=0;
  m[0+4*2]=0;
  m[0+4*3]=0;
  m[1+4*0]=0;
  m[1+4*1]=1;
  m[1+4*2]=0;
  m[1+4*3]=0;
  m[2+4*0]=0;
  m[2+4*1]=0;
  m[2+4*2]=1;
  m[2+4*3]=0;
  m[3+4*0]=0;
  m[3+4*1]=0;
  m[3+4*2]=0;
  m[3+4*3]=1;
}
void gluPerspective(GLfloat fovy,GLfloat aspect,GLfloat zNear,GLfloat zFar) {
  GLfloat m[4][4];
  GLfloat sine,cotangent,deltaZ;
  GLfloat radians=fovy/2*3.14/180;
  deltaZ=zFar-zNear;
  sine=std::sin(radians);
  if((deltaZ==0) || (sine==0) || (aspect==0))
    return;
  cotangent=std::cos(radians)/sine;

  gluMakeIdentityf(&m[0][0]);
  m[0][0]=cotangent/aspect;
  m[1][1]=cotangent;
  m[2][2]=-(zFar+zNear)/deltaZ;
  m[2][3]=-1;
  m[3][2]=-2*zNear*zFar/deltaZ;
  m[3][3]=0;
  glMultMatrixf((GLfloat*)m);
}
Eigen::Matrix<GLfloat,24,1> constructViewFrustum3D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  glGetFloatv(GL_MODELVIEW_MATRIX,MV.data());
  glGetFloatv(GL_PROJECTION_MATRIX,P.data());
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,24,1> ret;
  for(int z=-1,i=0; z<=1; z+=2)
    for(int y=-1; y<=1; y+=2)
      for(int x=-1; x<=1; x+=2,i+=3) {
        Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>(x,y,z,1);
        ret.segment<3>(i)=V.segment<3>(0)/V[3];
      }
  return ret;
}
Eigen::Matrix<GLfloat,8,1> constructViewFrustum2D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  glGetFloatv(GL_MODELVIEW_MATRIX,MV.data());
  glGetFloatv(GL_PROJECTION_MATRIX,P.data());
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,8,1> ret;
  for(int y=-1,i=0; y<=1; y+=2)
    for(int x=-1; x<=1; x+=2,i+=2) {
      Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>(x,y,0,1);
      ret.segment<2>(i)=(V.segment<3>(0)/V[3]).segment<2>(0);
    }
  return ret;
}
Eigen::Matrix<GLfloat,24,1> getViewFrustum3DPlanes() {
  //left,right,bottom,top,back,front
  Eigen::Matrix<GLfloat,24,1> planes,corners=constructViewFrustum3D();
  std::function<Eigen::Matrix<GLfloat,4,1>(int,int,int)> PLANE=[&](int a,int b,int c) {
    Eigen::Matrix<GLfloat,4,1> ret;
    ret.segment<3>(0)=(corners.segment<3>(c*3)-corners.segment<3>(a*3)).cross(corners.segment<3>(b*3)-corners.segment<3>(a*3)).normalized();
    ret[3]=-corners.segment<3>(a*3).dot(ret.segment<3>(0));
    return ret;
  };
  planes.segment<4>(0)=PLANE(0,4,6);
  planes.segment<4>(4)=PLANE(1,3,7);
  planes.segment<4>(8)=PLANE(0,1,5);
  planes.segment<4>(12)=PLANE(2,6,7);
  planes.segment<4>(16)=PLANE(0,2,3);
  planes.segment<4>(20)=PLANE(4,5,7);
  return planes;
}
Eigen::Matrix<GLfloat,16,1> getViewFrustum2DPlanes() {
  //left,right,bottom,top
  Eigen::Matrix<GLfloat,8,1> corners=constructViewFrustum2D();
  Eigen::Matrix<GLfloat,16,1> planes;
  planes.segment<4>(0 )=Eigen::Matrix<GLfloat,4,1>(-1,0,0, corners.segment<2>(0)[0]);
  planes.segment<4>(4 )=Eigen::Matrix<GLfloat,4,1>( 1,0,0,-corners.segment<2>(2)[0]);
  planes.segment<4>(8 )=Eigen::Matrix<GLfloat,4,1>(0,-1,0, corners.segment<2>(0)[1]);
  planes.segment<4>(12)=Eigen::Matrix<GLfloat,4,1>(0, 1,0,-corners.segment<2>(4)[1]);
  return planes;
}
void drawViewFrustum3D(const Eigen::Matrix<GLfloat,24,1>& frustum) {
#define VERT(V) glVertex3f(frustum[V*3+0],frustum[V*3+1],frustum[V*3+2]);
#define QUAD(A,B,C,D) VERT(A) VERT(B) VERT(C) VERT(D)
  glBegin(GL_QUADS);
  QUAD(0,2,3,1)
  QUAD(4,5,7,6)
  QUAD(0,1,5,4)
  QUAD(2,6,7,3)
  QUAD(0,4,6,2)
  QUAD(1,3,7,5)
  glEnd();
#undef QUAD
#undef VERT
}
void drawViewFrustum2D(const Eigen::Matrix<GLfloat,8,1>& frustum) {
#define VERT(V) glVertex2f(frustum[V*2+0],frustum[V*2+1]);
#define QUAD(A,B,C,D) VERT(A) VERT(B) VERT(C) VERT(D)
  glBegin(GL_QUADS);
  QUAD(0,2,3,1)
  glEnd();
#undef QUAD
#undef VERT
}
}
