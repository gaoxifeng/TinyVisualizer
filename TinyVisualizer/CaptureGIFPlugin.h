#ifndef CAPTURE_GIF_PLUGIN_H
#define CAPTURE_GIF_PLUGIN_H

#include "Drawer.h"

namespace DRAWER {
class CaptureGIFPlugin : public Plugin {
  RTTI_DECLARE_TYPEINFO(CaptureGIFPlugin, Plugin);
 public:
  CaptureGIFPlugin(int hotkey,const std::string& name,int FPS);
  virtual void finalize() override;
  virtual void frame(std::shared_ptr<SceneNode>&) override;
  virtual bool key(GLFWwindow* wnd,int key,int scan,int action,int mods) override;
  void startRecording();
  void addFrame();
  void stopRecording();
  bool recording() const;
 private:
  int _key;
  int _recordFPS;
  void* _recordFile;
  std::string _recordFileName;
  std::vector<unsigned char> _recordFrame;
  std::vector<unsigned char> _tmpFrameLine;
  GLint _width,_height;
  bool _dither;
};
}

#endif
