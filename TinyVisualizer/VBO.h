#ifndef VBO_H
#define VBO_H

#include "DrawerUtility.h"
#include "Texture.h"
#include <memory>
#include <vector>

namespace DRAWER {
class VBO {
 public:
  VBO(int nV,int nI);
  VBO(const VBO& other);
  VBO& operator=(const VBO& other);
  virtual ~VBO();
  void setVertexPosition(const std::vector<GLfloat>& p);
  void setVertexNormal(const std::vector<GLfloat>& n);
  void setVertexTexCoord(const std::vector<GLfloat>& tc);
  void setVertexPosition(int i,const Eigen::Matrix<GLfloat,3,1>& p);
  void setVertexNormal(int i,const Eigen::Matrix<GLfloat,3,1>& n);
  void setVertexTexCoord(int i,const Eigen::Matrix<GLfloat,2,1>& tc);
  void setIndex(const std::vector<GLuint>& iss);
  void draw(GLenum mode);
  Eigen::Matrix<GLfloat,-1,1> VBOData() const;
  Eigen::Matrix<GLuint,-1,1> IBOData() const;
 private:
  void reset(int nV,int nI);
  void clear();
  GLuint _VAO,_VBO,_IBO;
  int _nV,_nI;
};
//draw simple shapes
extern void drawPointf(const Eigen::Matrix<GLfloat,2,1>& v0);
extern void drawPointf(const Eigen::Matrix<GLfloat,3,1>& v0);
extern void drawLinef(const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& v1);
extern void drawLinef(const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,3,1>& v1);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,3,1>& v1,
                      const Eigen::Matrix<GLfloat,3,1>& v2,
                      const Eigen::Matrix<GLfloat,3,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,2,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,2,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,2,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,2,1>& v3);
extern void drawQuadf(const Eigen::Matrix<GLfloat,2,1>& t0,const Eigen::Matrix<GLfloat,3,1>& v0,
                      const Eigen::Matrix<GLfloat,2,1>& t1,const Eigen::Matrix<GLfloat,3,1>& v1,
                      const Eigen::Matrix<GLfloat,2,1>& t2,const Eigen::Matrix<GLfloat,3,1>& v2,
                      const Eigen::Matrix<GLfloat,2,1>& t3,const Eigen::Matrix<GLfloat,3,1>& v3);
}

#endif
