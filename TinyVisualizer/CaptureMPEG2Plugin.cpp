#include "CaptureMPEG2Plugin.h"
#include "FBO.h"
#include <jo_mpeg.h>
#include <iostream>

namespace DRAWER {
CaptureMPEG2Plugin::CaptureMPEG2Plugin(int hotkey,const std::string& name,int FPS)
  :_key(hotkey),_recordFPS(FPS),_recordFile(NULL),_recordFileName(name) {}
void CaptureMPEG2Plugin::finalize() {
  stopRecording();
}
void CaptureMPEG2Plugin::frame(std::shared_ptr<SceneNode>&) {
  addFrame();
}
bool CaptureMPEG2Plugin::key(GLFWwindowPtr,int key,int,int action,int) {
  if(key==_key && action==GLFW_PRESS) {
    if(!_recordFile)
      startRecording();
    else stopRecording();
    return false;
  } else return true;
}
//helper
void CaptureMPEG2Plugin::startRecording() {
  if(_recordFile!=NULL) {
    std::cout << "Alreading started recording, cannot accept duplicate calls!" << std::endl;
    return;
  } else {
#if defined(WIN32)
    fopen_s(&_recordFile,_recordFileName.c_str(),"wb");
#else
    _recordFile=fopen(_recordFileName.c_str(),"wb");
#endif
    std::cout << "Starting recording to " << _recordFileName << std::endl;

    //size
    GLFWwindowPtr wnd=glfwGetCurrentContext();
    glfwGetWindowSize(wnd._ptr,&_width,&_height);
  }
}
void CaptureMPEG2Plugin::addFrame() {
  if(!_recordFile)
    return;
  //read screen
  GLint viewport[4];
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&viewport[2],&viewport[3]);
  if(viewport[2]!=_width || viewport[3]!=_height) {
    std::cout << "Screen resolution changed from " << _width << "x" << _height << " to " << viewport[2] << "x" << viewport[3] << " stop recording!" << std::endl;
    stopRecording();
    return;
  }
  _tmpFrameLine.resize(viewport[2]*3);
  _recordFrame.resize(viewport[2]*viewport[3]*3);
  readPixels();
  //flip upside down
  for(int y=0,y2=viewport[3]-1; y<viewport[3]; y++,y2--)
    if(y<y2) {
      int off=y*viewport[2]*3;
      int off2=y2*viewport[2]*3;
      memcpy(&_tmpFrameLine[0],&_recordFrame[off],viewport[2]*3);
      memcpy(&_recordFrame[off],&_recordFrame[off2],viewport[2]*3);
      memcpy(&_recordFrame[off2],&_tmpFrameLine[0],viewport[2]*3);
    } else break;
  jo_write_mpeg(_recordFile,&_recordFrame[0],viewport[2],viewport[3],_recordFPS);
}
void CaptureMPEG2Plugin::stopRecording() {
  if(_recordFile) {
    fclose(_recordFile);
    std::cout << "Stopped recording to " << _recordFileName << std::endl;
    _recordFile=NULL;
  }
}
bool CaptureMPEG2Plugin::recording() const {
  return _recordFile!=NULL;
}
void CaptureMPEG2Plugin::readPixels() {
  GLint viewport[4];
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&viewport[2],&viewport[3]);

  //if we have FBO bound, start using the FBO
  FBO* offScreen=((Drawer*)glfwGetWindowUserPointer(wnd._ptr))->getOffScreenFBO();
  if(offScreen) {
    offScreen->begin();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
  }
  //get pixels
  glReadPixels(0,0,viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE,&_recordFrame[0]);
  //if we have FBO bound, end using the FBO
  if(offScreen)
    offScreen->end();
}
}
