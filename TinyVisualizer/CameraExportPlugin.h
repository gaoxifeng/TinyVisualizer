#ifndef CAMERA_EXPORT_PLUGIN_H
#define CAMERA_EXPORT_PLUGIN_H

#include "Drawer.h"

namespace DRAWER {
class CameraExportPlugin : public Plugin {
  RTTI_DECLARE_TYPEINFO(CameraExportPlugin, Plugin);
 public:
  CameraExportPlugin(int keySave,int keyLoad,const std::string& name);
  virtual bool key(GLFWwindowPtr wnd,int key,int scan,int action,int mods) override;
  bool saveCamera();
  bool loadCamera();
 private:
  int _keySave,_keyLoad;
  std::string _fileName;
};
}

#endif
