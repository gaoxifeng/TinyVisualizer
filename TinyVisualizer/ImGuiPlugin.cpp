#include "ImGuiPlugin.h"
#ifdef IMGUI_SUPPORT
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
//About Desktop OpenGL function loaders:
//Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
//Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#endif

namespace DRAWER {
void defaultImGuiSetup() {
  ASSERT_MSG(false,"Empty ImGuiSetup!")
}
ImGuiPlugin::ImGuiPlugin(PythonCallback* cb):_cb(cb),_ImGuiSetup(defaultImGuiSetup) {}
ImGuiPlugin::ImGuiPlugin(std::function<void()> ImGuiSetup):_cb(NULL),_ImGuiSetup(ImGuiSetup) {}
void ImGuiPlugin::init(GLFWwindow* window) {
#ifdef IMGUI_SUPPORT
  //Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  //GL ES 2.0 + GLSL 100
  const char* glsl_version="#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);
  glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  //GL 3.2 + GLSL 150
  const char* glsl_version="#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);            // Required on Mac
#else
  //GL 3.0 + GLSL 130
  const char* glsl_version="#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);            // 3.0+ only
#endif

  //Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
  bool err=gl3wInit()!=0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  bool err=glewInit()!=GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
  bool err=gladLoadGL()==0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
  bool err=gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
  bool err=false;
  glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
  bool err=false;
  glbinding::initialize([](const char* name) {
    return(glbinding::ProcAddress)glfwGetProcAddress(name);
  });
#else
  bool err=false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
  ASSERT_MSG(!err,"Failed to initialize OpenGL loader!")

  //Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io=ImGui::GetIO();
  (void)io;
  //io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  //Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  //Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window,true);
  ImGui_ImplOpenGL3_Init(glsl_version);
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
void ImGuiPlugin::finalize() {
#ifdef IMGUI_SUPPORT
  //Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
void ImGuiPlugin::preDraw() {
#ifdef IMGUI_SUPPORT
  //Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if(_cb)
    _cb->setup();
  else _ImGuiSetup();
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
void ImGuiPlugin::postDraw() {
#ifdef IMGUI_SUPPORT
  //render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
bool ImGuiPlugin::mouse(GLFWwindow*,int,int,int)  {
#ifdef IMGUI_SUPPORT
  return !ImGui::GetIO().WantCaptureMouse;
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
bool ImGuiPlugin::wheel(GLFWwindow*,double,double) {
#ifdef IMGUI_SUPPORT
  return !ImGui::GetIO().WantCaptureMouse;
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
bool ImGuiPlugin::motion(GLFWwindow*,double,double) {
#ifdef IMGUI_SUPPORT
  return !ImGui::GetIO().WantCaptureMouse;
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
bool ImGuiPlugin::key(GLFWwindow*,int,int,int,int) {
#ifdef IMGUI_SUPPORT
  return !ImGui::GetIO().WantCaptureKeyboard;
#else
  ASSERT_MSG(false,"ImGui not supported!")
#endif
}
}
