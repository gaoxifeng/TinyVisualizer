#ifndef VBO_H
#define VBO_H

#include "DrawerUtility.h"
#include "Texture.h"
#include <memory>
#include <vector>

namespace DRAWER {
class EXPORT VBO : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(VBO);
 public:
  VBO(int nV,int nI,bool hasV=true,bool hasN=true,bool hasT=true,bool hasI=false,bool hasBI=false,bool hasBW=false);
  VBO(const VBO& other);
  VBO& operator=(const VBO& other);
  virtual ~VBO();
  void setVertexPosition(const std::vector<GLfloat>& p);
  void setVertexNormal(const std::vector<GLfloat>& n);
  void setVertexTexCoord(const std::vector<GLfloat>& tc);
  void setVertexId(const std::vector<GLuint>& id);
  void setVertexBoneId(const std::vector<GLint>& bid);
  void setVertexBoneWeight(const std::vector<GLfloat>& bw);
  void setVertexPosition(int i,const Eigen::Matrix<GLfloat,3,1>& p);
  void setVertexNormal(int i,const Eigen::Matrix<GLfloat,3,1>& n);
  void setVertexTexCoord(int i,const Eigen::Matrix<GLfloat,2,1>& tc);
  void setVertexId(int i,int id);
  void setVertexBoneId(int i,const Eigen::Matrix<GLint,4,1>& bid);
  void setVertexBoneWeight(int i,const Eigen::Matrix<GLfloat,4,1>& gw);
  void setIndex(const std::vector<GLuint>& iss);
  void draw(GLenum mode);
  Eigen::Matrix<GLfloat,-1,1> VBOVData() const;
  Eigen::Matrix<GLfloat,-1,1> VBONData() const;
  Eigen::Matrix<GLfloat,-1,1> VBOTData() const;
  Eigen::Matrix<GLuint,-1,1> VBOIData() const;
  Eigen::Matrix<GLint,-1,1> VBOBIData() const;
  Eigen::Matrix<GLfloat,-1,1> VBOBWData() const;
  Eigen::Matrix<GLuint,-1,1> IBOData() const;
  GLuint VBOV() const;
  GLuint VBON() const;
  GLuint VBOT() const;
  GLuint VBOI() const;
  GLuint VBOBI() const;
  GLuint VBOBW() const;
 private:
  void reset(int nV,int nI,bool hasV,bool hasN,bool hasT,bool hasI,bool hasBI,bool hasBW);
  void clear();
  GLuint _VAO;
  GLuint _VBOV;
  GLuint _VBON;
  GLuint _VBOT;
  GLuint _VBOI;
  GLuint _VBOBI;
  GLuint _VBOBW;
  GLuint _IBO;
  int _nV,_nI;
  bool _hasV;
  bool _hasN;
  bool _hasT;
  bool _hasI;
  bool _hasBI;
  bool _hasBW;
};
}

#endif
