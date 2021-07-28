#ifndef CAPTURE_GIF_PLUGIN_H
#define CAPTURE_GIF_PLUGIN_H

#include "Drawer.h"
#include <gif.h>

namespace DRAWER {
class CaptureGIFPlugin : public Plugin {
 public:
  CaptureGIFPlugin(int hotkey,const std::string& name,int FPS);
  virtual void finalize() override;
  virtual void frame(std::shared_ptr<SceneNode>&) override;
  virtual void key(GLFWwindow* wnd,int key,int scan,int action,int mods) override;
 protected:
  void startRecording();
  void addFrame();
  void stopRecording();
 private:
  int _key;
  int _recordFPS;
  std::shared_ptr<GifWriter> _recordFile;
  std::string _recordFileName;
  std::vector<unsigned char> _recordFrame;
  std::vector<unsigned char> _tmpFrameLine;
  GLint _width,_height;
};
}

#endif
