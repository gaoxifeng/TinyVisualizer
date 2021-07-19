#ifndef MAKE_MESH_H
#define MAKE_MESH_H

#include "MeshShape.h"

namespace DRAWER {
extern std::shared_ptr<MeshShape> makeSquare(bool fill,const Eigen::Matrix<GLfloat,2,1>& halfExt,GLfloat depth=0);
extern std::shared_ptr<MeshShape> makeCircle(int RES,bool fill,const Eigen::Matrix<GLfloat,2,1>& pos,GLfloat rad);
extern void makeGridLine(MeshShape& mesh,int RES,const Eigen::Matrix<GLfloat,3,1>& ctr,const Eigen::Matrix<GLfloat,3,1>& d);
extern void makeGrid(MeshShape& mesh,int RES,bool fill,const Eigen::Matrix<GLfloat,3,1>& ctr,const Eigen::Matrix<GLfloat,3,1>& d0,const Eigen::Matrix<GLfloat,3,1>& d1);
extern std::shared_ptr<MeshShape> makeBox(int RES,bool fill,const Eigen::Matrix<GLfloat,3,1>& halfExt);
extern std::shared_ptr<MeshShape> makeSphericalBox(int RES,bool fill,GLfloat rad,const Eigen::Matrix<GLfloat,3,1>& halfExt);
extern std::shared_ptr<MeshShape> makeCapsule(int RES,bool fill,GLfloat rad,GLfloat height);
extern std::shared_ptr<MeshShape> makeSphere(int RES,bool fill,GLfloat rad);
extern std::shared_ptr<MeshShape> makeTriMesh(bool fill, const Eigen::Matrix<GLfloat, -1, -1>& V, const Eigen::Matrix<int, -1, -1>& F);
extern std::shared_ptr<MeshShape> makeWires(const Eigen::Matrix<GLfloat, -1, -1>& V, const Eigen::Matrix<int, -1, -1>& E);
}

#endif
