#ifndef DEFAULT_LIGHT_H
#define DEFAULT_LIGHT_H

#include "ShadowAndLight.h"

namespace DRAWER {
extern std::shared_ptr<Program> getDefaultProg();
extern std::shared_ptr<Program> getDebugDrawTexCoordProg();
extern std::shared_ptr<Program> getRoundPointProg();
extern void setRoundPointSize(GLfloat size);
extern std::shared_ptr<Program> getThickLineProg();
extern void setThickLineWidth(GLfloat size);
extern void setupMaterial(std::shared_ptr<Texture> tex,const Eigen::Matrix<GLfloat,4,1>& diffuse=Eigen::Matrix<GLfloat,4,1>(1,1,1,1));
extern void setupMaterial(std::shared_ptr<Texture> tex,GLfloat r,GLfloat g,GLfloat b);
extern void setupMaterial(const Material& mat);
}

#endif
