#include "CaptureGIFPlugin.h"
#include <iostream>

namespace DRAWER {
CaptureGIFPlugin::CaptureGIFPlugin(int hotkey,const std::string& name,int FPS)
  :_key(hotkey),_recordFPS(FPS),_recordFile(NULL),_recordFileName(name) {}
void CaptureGIFPlugin::finalize() {
  stopRecording();
}
void CaptureGIFPlugin::frame(std::shared_ptr<SceneNode>&) {
  addFrame();
}
void CaptureGIFPlugin::key(GLFWwindow*,int key,int,int action,int) {
  if(key==_key && action==GLFW_PRESS) {
    if(!_recordFile)
      startRecording();
    else stopRecording();
  }
}
//helper
void CaptureGIFPlugin::startRecording() {
  if(_recordFile!=NULL) {
    std::cout << "Alreading started recording, cannot accept duplicate calls!" << std::endl;
    return;
  } else {
    //size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    _width=viewport[2];
    _height=viewport[3];

    _recordFile.reset(new GifWriter);
    GifBegin(_recordFile.get(),_recordFileName.c_str(),_width,_height,100.0/_recordFPS,8,true);
    std::cout << "Starting recording to " << _recordFileName << std::endl;
  }
}
void CaptureGIFPlugin::addFrame() {
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
  _tmpFrameLine.resize(viewport[2]*4);
  _recordFrame.resize(viewport[2]*viewport[3]*4);
  glReadPixels(0,0,viewport[2],viewport[3],GL_RGBA,GL_UNSIGNED_BYTE,&_recordFrame[0]);
  //flip upside down
  for(int y=0,y2=viewport[3]-1; y<viewport[3]; y++,y2--)
    if(y<y2) {
      int off=y*viewport[2]*4;
      int off2=y2*viewport[2]*4;
      memcpy(&_tmpFrameLine[0],&_recordFrame[off],viewport[2]*4);
      memcpy(&_recordFrame[off],&_recordFrame[off2],viewport[2]*4);
      memcpy(&_recordFrame[off2],&_tmpFrameLine[0],viewport[2]*4);
    } else break;
  GifWriteFrame(_recordFile.get(),&_recordFrame[0],viewport[2],viewport[3],100.0/_recordFPS,8,true);
}
void CaptureGIFPlugin::stopRecording() {
  if(_recordFile) {
    GifEnd(_recordFile.get());
    std::cout << "Stopped recording to " << _recordFileName << std::endl;
    _recordFile=NULL;
  }
}
}
