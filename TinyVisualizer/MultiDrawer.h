#ifndef MULTI_DRAWER_H
#define MULTI_DRAWER_H

#include "Drawer.h"

namespace DRAWER {
//Drawer
class MultiDrawer : public RTTI::Enable {
  RTTI_DECLARE_TYPEINFO(MultiDrawer);
 public:
  MultiDrawer(const std::vector<std::string>& args);
  MultiDrawer(int argc,char** argv);
  virtual ~MultiDrawer();
  //multi-viewport
  void setViewportLayout(int rows,int cols);
  std::shared_ptr<Drawer> getDrawer(int row,int col) const;
  Eigen::Matrix<int,4,1> getViewport(Drawer* drawer) const;
  //common function
  void setRes(int width,int height);
  void timer();
  void frame();
  void draw();
  FBO* getOffScreenFBO() const;
  static void mouse(GLFWwindow* wnd,int button,int action,int mods);
  static void wheel(GLFWwindow* wnd,double xoffset,double yoffset);
  static void motion(GLFWwindow* wnd,double x,double y);
  static void key(GLFWwindow* wnd,int key,int scan,int action,int mods);
  void nextFrame();
  void mainLoop();
  //getter/setter
  void addPlugin(std::shared_ptr<Plugin> pi);
  void clear();
 private:
  void init(int argc,char** argv);
  MultiDrawer(const MultiDrawer& other);
  MultiDrawer& operator=(const MultiDrawer& other);
  //data
  std::vector<std::shared_ptr<Plugin>> _plugins;
  std::unordered_map<Drawer*,Eigen::Matrix<int,2,1>> _viewMap;
  std::vector<std::vector<std::shared_ptr<Drawer>>> _views;
  std::vector<std::string> _args;
  std::shared_ptr<FBO> _offScreen;
  GLFWwindow* _window;
  double _lastTime;
};
}

#endif
