#ifndef MAKE_TEXTURE_H
#define MAKE_TEXTURE_H

#include "FBO.h"

namespace DRAWER {
extern std::shared_ptr<Texture> drawTexture(std::function<void()> func,
    int levelMin=9,int levelMax=11,GLenum formatColor=GL_RGB);
extern std::shared_ptr<Texture> drawChecker(int density=5,
    Eigen::Matrix<GLfloat,3,1> c0=Eigen::Matrix<GLfloat,3,1>(1.f,1.f,1.f),
    Eigen::Matrix<GLfloat,3,1> c1=Eigen::Matrix<GLfloat,3,1>(.7f,.7f,.7f),
    int levelMin=9,int levelMax=11,GLenum formatColor=GL_RGB);
extern std::shared_ptr<Texture> drawGrid(int density=10,GLfloat t0=0.01f,GLfloat t1=0.03f,
    Eigen::Matrix<GLfloat,3,1> c0=Eigen::Matrix<GLfloat,3,1>(1.f,1.f,1.f),
    Eigen::Matrix<GLfloat,3,1> c1=Eigen::Matrix<GLfloat,3,1>(.7f,.7f,.7f),
    int levelMin=9,int levelMax=11,GLenum formatColor=GL_RGB);
extern std::shared_ptr<Texture> getWhiteTexture();
}

#endif
