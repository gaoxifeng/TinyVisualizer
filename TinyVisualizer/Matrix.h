#ifndef MATRIX_H
#define MATRIX_H

#include "DrawerUtility.h"

namespace DRAWER {
enum GL_MATRIX_MODE {
  GL_NORMAL_MATRIX,
  GL_MODELVIEW_MATRIX,
  GL_PROJECTION_MATRIX,
};
extern void popMatrix();
extern void pushMatrix();
extern void loadIdentity();
extern void matrixMode(GLuint mode);
extern void getFloatv(GLuint mode,Eigen::Matrix<GLfloat,3,3>& m);
extern void getFloatv(GLuint mode,Eigen::Matrix<GLfloat,4,4>& m);
extern void multMatrixf(const Eigen::Matrix<GLfloat,4,4>& m);
extern void multMatrixf(const GLfloat* const m);
extern void translatef(GLfloat x,GLfloat y,GLfloat z);
extern void rotatef(GLfloat angle,GLfloat x,GLfloat y,GLfloat z);
extern void scalef(GLfloat x,GLfloat y,GLfloat z);
extern void zRangef(const Eigen::Matrix<GLfloat,6,1>& bb,
                    GLfloat& zNear,GLfloat& zFar,
                    GLfloat minZNear=0.1f,GLfloat maxZFar=100.0f);
extern void lookAtf(GLfloat eyex,GLfloat eyey,GLfloat eyez,
                    GLfloat centerx,GLfloat centery,GLfloat centerz,
                    GLfloat upx,GLfloat upy,GLfloat upz);
extern void orthof(GLfloat left,GLfloat right,
                   GLfloat bottom,GLfloat top,
                   GLfloat near,GLfloat far);
extern void perspectivef(GLfloat fovy,GLfloat aspect,GLfloat zNear,GLfloat zFar);
extern void setupMatrixInShader();
extern Eigen::Matrix<GLfloat,24,1> constructViewFrustum3D();
extern Eigen::Matrix<GLfloat,8,1> constructViewFrustum2D();
extern Eigen::Matrix<GLfloat,24,1> getViewFrustum3DPlanes();
extern Eigen::Matrix<GLfloat,16,1> getViewFrustum2DPlanes();
extern void drawViewFrustum3D(const Eigen::Matrix<GLfloat,24,1>& frustum,const Eigen::Matrix<GLfloat,4,1>& color);
extern void drawViewFrustum2D(const Eigen::Matrix<GLfloat,8,1>& frustum,const Eigen::Matrix<GLfloat,4,1>& color);
}

#endif
