#ifndef CAPTURE_MPEG2_PLUGIN_H
#define CAPTURE_MPEG2_PLUGIN_H

#include "Drawer.h"

namespace DRAWER {
class CaptureMPEG2Plugin : public Plugin {
 public:
  CaptureMPEG2Plugin(int hotkey,const std::string& name,int FPS);
  virtual void finalize() override;
  virtual void frame(std::shared_ptr<SceneNode>&) override;
  virtual bool key(GLFWwindow* wnd,int key,int scan,int action,int mods) override;
 protected:
  void startRecording();
  void addFrame();
  void stopRecording();
 private:
  int _key;
  int _recordFPS;
  FILE* _recordFile;
  std::string _recordFileName;
  std::vector<unsigned char> _recordFrame;
  std::vector<unsigned char> _tmpFrameLine;
  GLint _width,_height;
};
}

#endif
