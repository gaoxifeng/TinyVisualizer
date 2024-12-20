#ifndef CAPTURE_GIF_PLUGIN_H
#define CAPTURE_GIF_PLUGIN_H

#include "Drawer.h"
#include "Texture.h"

namespace DRAWER {
class CaptureGIFPlugin : public Plugin {
  RTTI_DECLARE_TYPEINFO(CaptureGIFPlugin, Plugin);
 public:
  CaptureGIFPlugin(int hotkey,const std::string& name,int FPS,bool screenshot=false);
  virtual void finalize() override;
  virtual void frame(std::shared_ptr<SceneNode>&) override;
  virtual bool key(GLFWwindowPtr wnd,int key,int scan,int action,int mods) override;
  void getScreenshot(int& width,int& height,std::vector<unsigned char>& data);
  void takeScreenshot();
  void startRecording();
  void addFrame();
  void stopRecording();
  bool recording() const;
 private:
  void readScreenshot();
  void readPixels();
  int _key;
  int _recordFPS;
  void* _recordFile;
  std::string _recordFileName;
  std::vector<unsigned char> _recordFrame;
  std::vector<unsigned char> _tmpFrameLine;
  GLint _width,_height;
  bool _screenshot;
  bool _dither;
};
}

#endif
