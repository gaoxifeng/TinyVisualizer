#include "CapturePlugin.h"
#include <jo_mpeg.h>
#include <iostream>

namespace DRAWER {
CapturePlugin::CapturePlugin(int hotkey,const std::string& name,int FPS)
  :_key(hotkey),_recordFPS(FPS),_recordFile(NULL),_recordFileName(name) {}
void CapturePlugin::finalize() {
  stopRecording();
}
void CapturePlugin::frame(std::shared_ptr<SceneNode>&) {
  addFrame();
}
void CapturePlugin::key(GLFWwindow*,int key,int,int action,int) {
  if(key==_key && action==GLFW_PRESS) {
    if(!_recordFile)
      startRecording();
    else stopRecording();
  }
}
//helper
void CapturePlugin::startRecording() {
  if(_recordFile!=NULL) {
    std::cout << "Alreading started recording, cannot accept duplicate calls!" << std::endl;
    return;
  } else {
    _recordFile=fopen(_recordFileName.c_str(),"wb");
    std::cout << "Starting recording to " << _recordFileName << std::endl;

    //size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    _width=viewport[2];
    _height=viewport[3];
  }
}
void CapturePlugin::addFrame() {
  if(!_recordFile)
    return;
  //read screen
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);
  if(viewport[2]!=_width || viewport[3]!=_height) {
    std::cout << "Screen resolution changed from " << _width << "x" << _height << " to " << viewport[2] << "x" << viewport[3] << " stop recording!" << std::endl;
    stopRecording();
    return;
  }
  _tmpFrameLine.resize(viewport[2]*3);
  _recordFrame.resize(viewport[2]*viewport[3]*3);
  glReadPixels(0,0,viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE,&_recordFrame[0]);
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
void CapturePlugin::stopRecording() {
  if(_recordFile) {
    fclose(_recordFile);
    std::cout << "Stopped recording to " << _recordFileName << std::endl;
    _recordFile=NULL;
  }
}
}
