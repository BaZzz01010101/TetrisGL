#pragma once
#include "Application.h"
#include "FpsCounter.h"
#include "OpenGLRender.h"
#include "Control.h"

class OpenGLApplication : public Application
{
private:
  FpsCounter fps;
  OpenGLRender render;
  Control control;
  GLFWwindow * wnd;
  bool vSync;
  int wndWidth;
  int wndHeight;
  std::map<int, Key> glfwKeyMap;

  void initGlfwKeyMap();
  static void OnFramebufferSize(GLFWwindow * wnd, int width, int height);
  static void OnKeyClick(GLFWwindow * wnd, int key, int scancode, int action, int mods);
  static void OnMouseClick(GLFWwindow * wnd, int button, int action, int mods);
  static void OnMouseMove(GLFWwindow* wnd, double xpos, double ypos);
  static void OnMouseScroll(GLFWwindow* wnd, double dx, double dy);

public:
  OpenGLApplication();
  virtual ~OpenGLApplication();

  bool init();
  void run();
  void quit();
};

