#pragma once
#include "Application.h"
#include "FpsCounter.h"
#include "OpenGLRender.h"

class OpenGLApplication : public Application
{
private:
  FpsCounter fps;
  OpenGLRender render;
  GLFWwindow * wnd;
  bool vSync;

  int winWidth;
  int winHeight;
  float mouseX;
  float mouseY;

  static void OnFramebufferSize(GLFWwindow * wnd, int width, int height);
  static void OnKeyClick(GLFWwindow * wnd, int key, int scancode, int action, int mods);
  static void OnMouseClick(GLFWwindow * wnd, int button, int action, int mods);
  static void OnMouseMove(GLFWwindow* wnd, double xpos, double ypos);

public:
  OpenGLApplication(GameLogic & model);
  virtual ~OpenGLApplication();

  bool init();
  void run();
  void quit();
};

