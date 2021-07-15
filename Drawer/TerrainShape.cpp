#include "TerrainShape.h"

namespace DRAWER {
TerrainShape::TerrainShape(const Eigen::Matrix<GLfloat,-1,-1>& height,int upAxis,
                           const Eigen::Matrix<GLfloat,3,1>& scale,
                           const Eigen::Matrix<GLfloat,2,1>& tcMult) {
#define ID(X,Y) (X)*height.cols()+(Y)
  Eigen::Matrix<GLfloat,3,3> rotate;
  rotate.col(0).setUnit((upAxis+1)%3);
  rotate.col(1).setUnit((upAxis+2)%3);
  rotate.col(2).setUnit((upAxis+3)%3);
  ASSERT_MSG(height.size()>0,"Empty terrain!")
  for(int x=0; x<height.rows(); x++)
    for(int y=0; y<height.cols(); y++) {
      Eigen::Matrix<GLfloat,3,1> pos(x,y,height(x,y));
      Eigen::Matrix<GLfloat,2,1> tc(x*tcMult[0],y*tcMult[1]);
      addVertex(((rotate*pos).array()*scale.array()).matrix(),&tc);
      if(x<height.rows()-1 && y<height.cols()-1) {
        addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y),ID(x+1,y+1)));
        addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y+1),ID(x,y+1)));
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
  int nrSegA=ceil(rangeA/RES);
  int nrSegB=ceil(rangeB/RES);
  ASSERT_MSG(nrSegA>0 && nrSegB>0,"Empty terrain!")
  for(int x=0; x<=nrSegA; x++)
    for(int y=0; y<=nrSegB; y++) {
      GLfloat xP=rangeA/nrSegA*x+aabb[axisA];
      GLfloat yP=rangeB/nrSegB*y+aabb[axisB];
      Eigen::Matrix<GLfloat,3,1> pos;
      pos=height(xP,yP)*Eigen::Matrix<GLfloat,3,1>::Unit(upAxis);
      pos+=xP*Eigen::Matrix<GLfloat,3,1>::Unit(axisA);
      pos+=yP*Eigen::Matrix<GLfloat,3,1>::Unit(axisB);
      Eigen::Matrix<GLfloat,2,1> tc(x*tcMult[0],y*tcMult[1]);
      addVertex(pos,&tc);
      if(x<nrSegA && y<nrSegB) {
        addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y),ID(x+1,y+1)));
        addIndex(Eigen::Matrix<int,3,1>(ID(x,y),ID(x+1,y+1),ID(x,y+1)));
      }
    }
  setMode(GL_TRIANGLES);
  computeNormals();
#undef ID
}
}
