#ifndef IMGUI_PLUGIN_H
#define IMGUI_PLUGIN_H

#include "Drawer.h"

namespace DRAWER {
class ImGuiPlugin : public Plugin {
  RTTI_DECLARE_TYPEINFO(ImGuiPlugin, Plugin);
 public:
  ImGuiPlugin(PythonCallback* cb);
  ImGuiPlugin(std::function<void()> ImGuiSetup);
  virtual void init(GLFWwindowPtr window) override;
  virtual void finalize() override;
  virtual void preDraw() override;
  virtual void postDraw() override;
  virtual bool mouse(GLFWwindowPtr wnd,int button,int action,int mods) override;
  virtual bool wheel(GLFWwindowPtr wnd,double xoffset,double yoffset) override;
  virtual bool motion(GLFWwindowPtr wnd,double x,double y) override;
  virtual bool key(GLFWwindowPtr wnd,int key,int scan,int action,int mods) override;
 private:
  PythonCallback* _cb;
  std::function<void()> _ImGuiSetup;
};
}

#endif
