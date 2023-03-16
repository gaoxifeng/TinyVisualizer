#include "CameraExportPlugin.h"
#include "Camera3D.h"
#include <iostream>
#include <fstream>

namespace DRAWER {
CameraExportPlugin::CameraExportPlugin(int keySave,int keyLoad,const std::string& name):_keySave(keySave),_keyLoad(keyLoad),_fileName(name) {}
bool CameraExportPlugin::key(GLFWwindow*,int key,int,int action,int) {
  std::shared_ptr<Camera3D> cam=std::dynamic_pointer_cast<Camera3D>(_drawer->getCamera());
  if(key==_keySave && action==GLFW_PRESS) {
    if(!cam) {
      std::cout << "No camera detected, cannot serialize!" << std::endl;
      return true;
    }
    std::ofstream os(_fileName);
    os << cam->position()[0] << " " << cam->position()[1] << " " << cam->position()[2] << std::endl;
    os << cam->direction()[0] << " " << cam->direction()[1] << " " << cam->direction()[2] << std::endl;
    os << cam->up()[0] << " " << cam->up()[1] << " " << cam->up()[2] << std::endl;
    return false;
  } else if(key==_keyLoad && action==GLFW_PRESS) {
    if(!cam) {
      std::cout << "No camera detected, cannot serialize!" << std::endl;
      return true;
    }
    std::ifstream is(_fileName);
    Eigen::Matrix<GLfloat,3,1> v;
    is >> v[0] >> v[1] >> v[2];
    cam->setPosition(v);
    is >> v[0] >> v[1] >> v[2];
    cam->setDirection(v);
    is >> v[0] >> v[1] >> v[2];
    cam->setUp(v);
    return false;
  }
  return true;
}
}
