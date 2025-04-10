#include "CaptureGIFPlugin.h"
#include "FBO.h"
#include <iostream>
#include <gif.h>

namespace DRAWER {
CaptureGIFPlugin::CaptureGIFPlugin(int hotkey,const std::string& name,int FPS,bool screenshot)
  :_key(hotkey),_recordFPS(FPS),_recordFile(NULL),_recordFileName(name),_screenshot(screenshot),_dither(false) {}
void CaptureGIFPlugin::finalize() {
  stopRecording();
}
void CaptureGIFPlugin::frame(std::shared_ptr<SceneNode>&) {
  addFrame();
}
bool CaptureGIFPlugin::key(GLFWwindowPtr,int key,int,int action,int) {
  if(key==_key && action==GLFW_PRESS) {
    if(_screenshot) {
      takeScreenshot();
    } else {
      if(!_recordFile)
        startRecording();
      else stopRecording();
    }
    return false;
  } else return true;
}
//helper
void CaptureGIFPlugin::getScreenshot(int& width,int& height,std::vector<unsigned char>& data) {
  //read screen size
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&width,&height);
  readScreenshot();
  data=_recordFrame;    //data copy
}
void CaptureGIFPlugin::takeScreenshot() {
  startRecording();
  addFrame();
  stopRecording();
}
void CaptureGIFPlugin::startRecording() {
  if(_recordFile!=NULL) {
    std::cout << "Alreading started recording, cannot accept duplicate calls!" << std::endl;
    return;
  } else {
    //size
    GLFWwindowPtr wnd=glfwGetCurrentContext();
    glfwGetWindowSize(wnd._ptr,&_width,&_height);

    _recordFile=new GifWriter;
    GifBegin((GifWriter*)_recordFile,_recordFileName.c_str(),(uint32_t)_width,(uint32_t)_height,(uint32_t)(100.0/_recordFPS),8,_dither);
    std::cout << "Starting recording to " << _recordFileName << std::endl;
  }
}
void CaptureGIFPlugin::addFrame() {
  if(!_recordFile)
    return;
  GLint viewport[4];
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&viewport[2],&viewport[3]);
  if(viewport[2]!=_width || viewport[3]!=_height) {
    std::cout << "Screen resolution changed from " << _width << "x" << _height << " to " << viewport[2] << "x" << viewport[3] << " stop recording!" << std::endl;
    stopRecording();
    return;
  }
  readScreenshot();
  GifWriteFrame((GifWriter*)_recordFile,&_recordFrame[0],(GLuint)viewport[2],(GLuint)viewport[3],(GLuint)(100.0f/_recordFPS),8,_dither);
}
void CaptureGIFPlugin::stopRecording() {
  if(_recordFile) {
    GifEnd((GifWriter*)_recordFile);
    std::cout << "Stopped recording to " << _recordFileName << std::endl;
    _recordFile=NULL;
  }
}
bool CaptureGIFPlugin::recording() const {
  return _recordFile!=NULL;
}
void CaptureGIFPlugin::readScreenshot() {
  //read screen size
  GLint viewport[4];
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&viewport[2],&viewport[3]);
  //allocate space
  _tmpFrameLine.resize(viewport[2]*4);
  _recordFrame.resize(viewport[2]*viewport[3]*4);
  readPixels();
  //flip upside down
  for(int y=0,y2=viewport[3]-1; y<viewport[3]; y++,y2--)
    if(y<y2) {
      int off=y*viewport[2]*4;
      int off2=y2*viewport[2]*4;
      memcpy(&_tmpFrameLine[0],&_recordFrame[off],viewport[2]*4);
      memcpy(&_recordFrame[off],&_recordFrame[off2],viewport[2]*4);
      memcpy(&_recordFrame[off2],&_tmpFrameLine[0],viewport[2]*4);
    } else break;
}
void CaptureGIFPlugin::readPixels() {
  GLint viewport[4];
  GLFWwindowPtr wnd=glfwGetCurrentContext();
  glfwGetWindowSize(wnd._ptr,&viewport[2],&viewport[3]);

  //if we have FBO bound, start using the FBO
  Drawer* dd=((Drawer*)glfwGetWindowUserPointer(wnd._ptr));
  FBO* offScreen=((Drawer*)glfwGetWindowUserPointer(wnd._ptr))->getOffScreenFBO();
  if(offScreen) {
    offScreen->begin();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
  }
  //get pixels
  glReadPixels(0,0,viewport[2],viewport[3],GL_RGBA,GL_UNSIGNED_BYTE,&_recordFrame[0]);
  //if we have FBO bound, end using the FBO
  if(offScreen)
    offScreen->end();
}
}
