#ifndef IMGUIPlUGIN_H
#define IMGUIPlUGIN_H

#include "Drawer.h"

struct ImGuiContext;

namespace DRAWER {
class ImGuiPlugin : public Plugin {
 public:
  virtual void init(GLFWwindow* window);
  void reloadFont(int font_size=13);
  virtual void clear() override;
  virtual void preDraw() override;
  virtual void postDraw() override;
  // Draw menu
  virtual void drawMenu();
  // Can be overwritten by `callback_draw_viewer_window`
  virtual void drawViewerWindow();
  // Can be overwritten by `callback_draw_viewer_menu`
  virtual void drawViewerMenu();
  // Can be overwritten by `callback_draw_custom_window`
  virtual void drawCustomWindow() {}
  // Easy-to-customize callbacks
  std::function<void(void)> callbackDrawViewerWindow;
  std::function<void(void)> callbackDrawViewerMenu;
  std::function<void(void)> callbackDrawCustomWindow;
  float pixelRatio();
  float hidpiScaling();
  float menuScaling();
 protected:
  //Hidpi scaling to be used for text rendering.
  float _hidpiScaling;
  //Ratio between the framebuffer size and the window size.
  //May be different from the hipdi scaling!
  float _pixelRatio;
  //ImGui Context
  ImGuiContext* _context=nullptr;
};
}
#endif
