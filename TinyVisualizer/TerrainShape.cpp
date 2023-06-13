#include "TerrainShape.h"

namespace DRAWER {
TerrainShape::TerrainShape(const Eigen::Matrix<GLfloat,-1,-1>& height,int upAxis,
                           const Eigen::Matrix<GLfloat,3,1>& scale,
                           const Eigen::Matrix<GLfloat,2,1>& tcMult) {
#define ID(X,Y) (X)*(int)height.cols()+(Y)
  Eigen::Matrix<GLfloat,3,3> rotate;
  rotate.col(0)=Eigen::Matrix<GLfloat,3,1>::Unit((upAxis+1)%3);
  rotate.col(1)=Eigen::Matrix<GLfloat,3,1>::Unit((upAxis+2)%3);
  rotate.col(2)=Eigen::Matrix<GLfloat,3,1>::Unit((upAxis+3)%3);
  ASSERT_MSG(height.size()>0,"Empty terrain!")
  for(int x=0; x<height.rows(); x++)
    for(int y=0; y<height.cols(); y++) {
      Eigen::Matrix<GLfloat,-1,1> pos=Eigen::Matrix<GLfloat,3,1>((GLfloat)x,(GLfloat)y,height(x,y));
      Eigen::Matrix<GLfloat,-1,1> tc=Eigen::Matrix<GLfloat,2,1>((GLfloat)x*tcMult[0],(GLfloat)y*tcMult[1]);
      addVertex(((rotate*pos).array()*scale.array()).matrix(),&tc);
      if(x<height.rows()-1 && y<height.cols()-1) {
        addIndex(Eigen::Matrix<GLuint,3,1>((GLuint)ID(x,y),(GLuint)ID(x+1,y),(GLuint)ID(x+1,y+1)));
        addIndex(Eigen::Matrix<GLuint,3,1>((GLuint)ID(x,y),(GLuint)ID(x+1,y+1),(GLuint)ID(x,y+1)));
      }
    }
  setMode(GL_TRIANGLES);
  computeNormals();
#undef ID
}
TerrainShape::TerrainShape(std::function<GLfloat(GLfloat,GLfloat)> height,int upAxis,
                           const Eigen::Matrix<GLfloat,6,1>& aabb,GLfloat RES,
                           const Eigen::Matrix<GLfloat,2,1>& tcMult) {
#define ID(X,Y) (X)*(nrSegB+1)+(Y)
  int axisA=(upAxis+1)%3;
  int axisB=(upAxis+2)%3;
  GLfloat rangeA=aabb[axisA+3]-aabb[axisA];
  GLfloat rangeB=aabb[axisB+3]-aabb[axisB];
  int nrSegA=(int)ceil(rangeA/RES);
  int nrSegB=(int)ceil(rangeB/RES);
  ASSERT_MSG(nrSegA>0 && nrSegB>0,"Empty terrain!")
  for(int x=0; x<=nrSegA; x++)
    for(int y=0; y<=nrSegB; y++) {
      GLfloat xP=rangeA/nrSegA*x+aabb[axisA];
      GLfloat yP=rangeB/nrSegB*y+aabb[axisB];
      Eigen::Matrix<GLfloat,3,1> pos;
      pos=height(xP,yP)*Eigen::Matrix<GLfloat,3,1>::Unit(upAxis);
      pos+=xP*Eigen::Matrix<GLfloat,3,1>::Unit(axisA);
      pos+=yP*Eigen::Matrix<GLfloat,3,1>::Unit(axisB);
      Eigen::Matrix<GLfloat,-1,1> tc=Eigen::Matrix<GLfloat,2,1>(x*tcMult[0],y*tcMult[1]);
      addVertex(pos,&tc);
      if(x<nrSegA && y<nrSegB) {
        addIndex(Eigen::Matrix<GLuint,3,1>(ID(x,y),ID(x+1,y),ID(x+1,y+1)));
        addIndex(Eigen::Matrix<GLuint,3,1>(ID(x,y),ID(x+1,y+1),ID(x,y+1)));
      }
    }
  setMode(GL_TRIANGLES);
  computeNormals();
#undef ID
}
}
