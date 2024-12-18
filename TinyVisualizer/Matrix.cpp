#include "Matrix.h"
#include "DrawerUtility.h"
#include "DefaultLight.h"
#include "Shader.h"
#include "VBO.h"
#include <stack>

namespace DRAWER {
Eigen::Matrix<GLfloat,4,4> modelViewMatrix=Eigen::Matrix<GLfloat,4,4>::Identity();
Eigen::Matrix<GLfloat,4,4> projectionMatrix=Eigen::Matrix<GLfloat,4,4>::Identity();
Eigen::Matrix<GLfloat,4,4>* currentMatrix=&modelViewMatrix;
std::stack<Eigen::Matrix<GLfloat,4,4>> modelViewStack;
std::stack<Eigen::Matrix<GLfloat,4,4>> projectionStack;
std::stack<Eigen::Matrix<GLfloat,4,4>>* currentStack=&modelViewStack;
void popMatrix() {
  ASSERT_MSG(!currentStack->empty(),"Matrix stack is empty!")
  *currentMatrix=currentStack->top();
  currentStack->pop();
}
void pushMatrix() {
  currentStack->push(*currentMatrix);
}
void loadIdentity() {
  currentMatrix->setIdentity();
}
void matrixMode(GLuint mode) {
  if(mode==GLModelViewMatrix) {
    currentMatrix=&modelViewMatrix;
    currentStack=&modelViewStack;
  } else if(mode==GLProjectionMatrix) {
    currentMatrix=&projectionMatrix;
    currentStack=&projectionStack;
  } else {
    ASSERT_MSGV(false,"unsupported mode %d for matrixMode!",mode)
  }
}
void getFloatv(GLuint mode,Eigen::Matrix<GLfloat,3,3>& m) {
  if(mode==GLNormalMatrix) {
    m=modelViewMatrix.block<3,3>(0,0).inverse().transpose();
  } else {
    ASSERT_MSGV(false,"unsupported mode %d for getFloatv!",mode)
  }
}
void getFloatv(GLuint mode,Eigen::Matrix<GLfloat,4,4>& m) {
  if(mode==GLModelViewMatrix) {
    m=modelViewMatrix;
  } else if(mode==GLProjectionMatrix) {
    m=projectionMatrix;
  } else {
    ASSERT_MSGV(false,"unsupported mode %d for getFloatv!",mode)
  }
}
Eigen::Matrix<GLfloat,3,3> getFloatv3(GLuint mode) {
  Eigen::Matrix<GLfloat,3,3> ret;
  getFloatv(mode,ret);
  return ret;
}
Eigen::Matrix<GLfloat,4,4> getFloatv4(GLuint mode) {
  Eigen::Matrix<GLfloat,4,4> ret;
  getFloatv(mode,ret);
  return ret;
}
void multMatrixf(const Eigen::Matrix<GLfloat,4,4>& m) {
  *currentMatrix*=m;
}
void multMatrixf(const GLfloat* const m) {
  multMatrixf(Eigen::Map<const Eigen::Matrix<GLfloat,4,4>>(m));
}
void translatef(GLfloat x,GLfloat y,GLfloat z) {
  Eigen::Matrix<GLfloat,4,4> m;
  m.setIdentity();
  m(0,3)=x;
  m(1,3)=y;
  m(2,3)=z;
  multMatrixf(m);
}
void rotatef(GLfloat angle,GLfloat x,GLfloat y,GLfloat z) {
  Eigen::Matrix<GLfloat,4,4> m;
  m.setIdentity();
  m.block<3,3>(0,0)=Eigen::AngleAxisf(angle*(GLfloat)M_PI/180.0f,Eigen::Matrix<GLfloat,3,1>(x,y,z).normalized()).toRotationMatrix();
  multMatrixf(m);
}
void scalef(GLfloat x,GLfloat y,GLfloat z) {
  Eigen::Matrix<GLfloat,4,4> m;
  m.setIdentity();
  m(0,0)=x;
  m(1,1)=y;
  m(2,2)=z;
  multMatrixf(m);
}
void zRangef(const Eigen::Matrix<GLfloat,6,1>& bb,
             GLfloat& zNear,GLfloat& zFar,
             GLfloat minZNearRelative,GLfloat maxZFar) {
  if(!std::isfinite(bb[0]) || std::abs(bb[0])==std::numeric_limits<GLfloat>::max()) {
    zFar=maxZFar;
    zNear=minZNearRelative*zFar;
    return;
  }
  zNear=maxZFar;
  zFar=0;
  Eigen::Matrix<GLfloat,4,4> mv;
  getFloatv(GLModelViewMatrix,mv);
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
  zFar=std::min<GLfloat>(maxZFar,zFar);
  zNear=std::max<GLfloat>(minZNearRelative*zFar,zNear);
}
void lookAtf(GLfloat eyex,GLfloat eyey,GLfloat eyez,
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

  multMatrixf(m);
  translatef(-eyex,-eyey,-eyez);
}
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
void orthof(GLfloat left,GLfloat right,
            GLfloat bottom,GLfloat top,
            GLfloat near,GLfloat far) {
  GLfloat m[16];
  GLfloat rml=right-left;
  GLfloat fmn=far-near;
  GLfloat tmb=top-bottom;
  GLfloat _1over_rml;
  GLfloat _1over_fmn;
  GLfloat _1over_tmb;

  if (rml == 0.0 || fmn == 0.0 || tmb == 0.0) {
    ASSERT_MSG(false,"GL_INVALID_VALUE")
    return;
  }

  _1over_rml=1.0f/rml;
  _1over_fmn=1.0f/fmn;
  _1over_tmb=1.0f/tmb;

  m[0]=2.0f*_1over_rml;
  m[1]=0.0f;
  m[2]=0.0f;
  m[3]=0.0f;

  m[4]=0.0f;
  m[5]=2.0f*_1over_tmb;
  m[6]=0.0f;
  m[7]=0.0f;

  m[8]=0.0f;
  m[9]=0.0f;
  m[10]=-2.0f*_1over_fmn;
  m[11]=0.0f;

  m[12]=-(right+left)*_1over_rml;
  m[13]=-(top+bottom)*_1over_tmb;
  m[14]=-(far+near)*_1over_fmn;
  m[15]=1.0f;

  multMatrixf(m);
}
void makeIdentityf(GLfloat m[16]) {
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
void perspectivef(GLfloat fovy,GLfloat aspect,GLfloat zNear,GLfloat zFar) {
  GLfloat m[4][4];
  GLfloat sine,cotangent,deltaZ;
  GLfloat radians=fovy/2*3.14f/180;
  deltaZ=zFar-zNear;
  sine=std::sin(radians);
  if((deltaZ==0) || (sine==0) || (aspect==0))
    return;
  cotangent=std::cos(radians)/sine;

  makeIdentityf(&m[0][0]);
  m[0][0]=cotangent/aspect;
  m[1][1]=cotangent;
  m[2][2]=-(zFar+zNear)/deltaZ;
  m[2][3]=-1;
  m[3][2]=-2*zNear*zFar/deltaZ;
  m[3][3]=0;
  multMatrixf((GLfloat*)m);
}
void setupMatrixInShader() {
  std::shared_ptr<Program> prog=Program::currentProgram();
  if(!prog)
    return;

  Eigen::Matrix<GLfloat,3,3> N;
  getFloatv(GLNormalMatrix,N);
  prog->setUniformFloat("normalMatrix",N,false);

  Eigen::Matrix<GLfloat,4,4> MV;
  getFloatv(GLModelViewMatrix,MV);
  prog->setUniformFloat("modelViewMatrix",MV,false);

  Eigen::Matrix<GLfloat,4,4> P,MVP;
  getFloatv(GLProjectionMatrix,P);
  prog->setUniformFloat("modelViewProjectionMatrix",MVP=P*MV,false);
}
Eigen::Matrix<GLfloat,24,1> constructViewFrustum3D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  getFloatv(GLModelViewMatrix,MV);
  getFloatv(GLProjectionMatrix,P);
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,24,1> ret;
  for(int z=-1,i=0; z<=1; z+=2)
    for(int y=-1; y<=1; y+=2)
      for(int x=-1; x<=1; x+=2,i+=3) {
        Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)1);
        ret.segment<3>(i)=V.segment<3>(0)/V[3];
      }
  return ret;
}
Eigen::Matrix<GLfloat,8,1> constructViewFrustum2D() {
  Eigen::Matrix<GLfloat,4,4> MV,P,invMVP;
  getFloatv(GLModelViewMatrix,MV);
  getFloatv(GLProjectionMatrix,P);
  invMVP=(P*MV).inverse();

  Eigen::Matrix<GLfloat,8,1> ret;
  for(int y=-1,i=0; y<=1; y+=2)
    for(int x=-1; x<=1; x+=2,i+=2) {
      Eigen::Matrix<GLfloat,4,1> V=invMVP*Eigen::Matrix<GLfloat,4,1>((GLfloat)x,(GLfloat)y,(GLfloat)0,(GLfloat)1);
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
void drawViewFrustum3D(const Eigen::Matrix<GLfloat,24,1>& frustum,const Eigen::Matrix<GLfloat,4,1>& color) {
#define VERT(V) Eigen::Matrix<GLfloat,3,1>(frustum.segment<3>(V*3))
#define QUAD(A,B,C,D) drawQuadf(VERT(A),VERT(B),VERT(C),VERT(D));
  getDefaultProg()->begin();
  setupMaterial(NULL,color);
  setupMatrixInShader();
  QUAD(0,2,3,1)
  QUAD(4,5,7,6)
  QUAD(0,1,5,4)
  QUAD(2,6,7,3)
  QUAD(0,4,6,2)
  QUAD(1,3,7,5)
  Program::currentProgram()->end();
#undef QUAD
#undef VERT
}
void drawViewFrustum2D(const Eigen::Matrix<GLfloat,8,1>& frustum,const Eigen::Matrix<GLfloat,4,1>& color) {
#define VERT(V) Eigen::Matrix<GLfloat,2,1>(frustum.segment<2>(V*2))
#define QUAD(A,B,C,D) drawQuadf(VERT(A),VERT(B),VERT(C),VERT(D));
  getDefaultProg()->begin();
  setupMaterial(NULL,color);
  setupMatrixInShader();
  QUAD(0,2,3,1)
  Program::currentProgram()->end();
#undef QUAD
#undef VERT
}
}
