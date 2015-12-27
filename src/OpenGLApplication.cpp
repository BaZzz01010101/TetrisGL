#include "static_headers.h"

#include "OpenGLApplication.h"
#include "Logic.h"
#include "Crosy.h"
#include "Time.h"
#include "Layout.h"
#include "Palette.h"
#include "Sound.h"

#pragma warning(disable : 4100)

OpenGLApplication::OpenGLApplication()
{
  initGlfwKeyMap();
}

OpenGLApplication::~OpenGLApplication()
{
}

bool OpenGLApplication::init()
{
  if (createCounter > 1)
    return false;

  vSync = true;

  if (!glfwInit())
    return false;

  //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  //glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  wndWidth = 640;
  wndHeight = 480;
  wnd = glfwCreateWindow(wndWidth, wndHeight, "glTetris", NULL, NULL);

  if (!wnd)
    return false;

  glfwSetWindowUserPointer(wnd, this);
  glfwMakeContextCurrent(wnd);
  glfwSetFramebufferSizeCallback(wnd, OnFramebufferSize);
  glfwSetKeyCallback(wnd, OnKeyClick);
  glfwSetMouseButtonCallback(wnd, OnMouseClick);
  glfwSetCursorPosCallback(wnd, OnMouseMove);
  glfwSetScrollCallback(wnd, OnMouseScroll);

  glewExperimental = GL_TRUE;

  GLenum glewInitResult = glewInit();
  assert(glewInitResult == GLEW_OK);

  // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
  glGetError();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  Layout::load("default");
  Palette::load("default");
  Sound::init();
  render.init(wndWidth, wndHeight);
  fps.init();

  return true;
}

void OpenGLApplication::run()
{
  bool exitFlag = false;

  while (!exitFlag)
  {
    Time::update();
    control.update();
    Logic::update();
    Sound::update();
    render.update();

    glfwSetWindowTitle(wnd, fps.count(0.5f));
    glfwSwapInterval((int)vSync);
    glfwSwapBuffers(wnd);
    glfwPollEvents();

    if (!glfwGetWindowAttrib(wnd, GLFW_FOCUSED) && GameLogic::state == GameLogic::stPlaying)
    {
      GameLogic::pauseGame();
      InterfaceLogic::showInGameMenu();
    }

    if (glfwWindowShouldClose(wnd) || Logic::result == Logic::resExitApp)
      exitFlag = true;
  }
}

void OpenGLApplication::quit()
{
  glfwTerminate();
}

void OpenGLApplication::OnFramebufferSize(GLFWwindow * wnd, int width, int height)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  app.wndWidth = width;
  app.wndHeight = height;

  app.render.resize(width, height);
}

void OpenGLApplication::OnKeyClick(GLFWwindow * wnd, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_UNKNOWN)
    return;

  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  if (action == GLFW_PRESS && key == GLFW_KEY_F11)
    app.vSync = !app.vSync;

  if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_ALT)
    app.render.showWireframe = true;

  if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT_ALT)
    app.render.showWireframe = false;

  switch (action)
  {
  case GLFW_PRESS:    app.control.keyDown(app.glfwKeyMap[key]);   break;
  case GLFW_RELEASE:  app.control.keyUp(app.glfwKeyMap[key]);     break;
  default: break;
  }
}

void OpenGLApplication::OnMouseClick(GLFWwindow * wnd, int button, int action, int mods)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  switch (button)
  {
  case GLFW_MOUSE_BUTTON_LEFT:
    action == GLFW_PRESS ? app.control.mouseDown(MOUSE_LEFT) : app.control.mouseUp(MOUSE_LEFT);
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    action == GLFW_PRESS ? app.control.mouseDown(MOUSE_RIGHT) : app.control.mouseUp(MOUSE_RIGHT);
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    action == GLFW_PRESS ? app.control.mouseDown(MOUSE_MIDDLE) : app.control.mouseUp(MOUSE_MIDDLE);
    break;
  default:
    break;
  }

}

void OpenGLApplication::OnMouseMove(GLFWwindow* wnd, double xpos, double ypos)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  if (app.wndWidth && app.wndHeight)
  {
    const float gameAspect = Layout::backgroundWidth / Layout::backgroundHeight;
    const float screenAspect = float(app.wndWidth) / app.wndHeight;

    if (screenAspect > gameAspect)
    {
      float unitPixCount = app.wndHeight / Layout::backgroundHeight;
      float mouseX = (float(xpos) - 0.5f * (app.wndWidth - unitPixCount)) / unitPixCount;
      float mouseY = float(ypos) / unitPixCount + 0.5f * (1.0f - Layout::backgroundHeight);
      app.control.mouseMove(mouseX, mouseY);
    }
    else
    {
      float unitPixCount = app.wndWidth / Layout::backgroundWidth;
      float mouseX = float(xpos) / unitPixCount + 0.5f * (1.0f - Layout::backgroundWidth);
      float mouseY = (float(ypos) - 0.5f * (app.wndHeight - unitPixCount)) / unitPixCount;
      app.control.mouseMove(mouseX, mouseY);
    }
  }
}

void OpenGLApplication::OnMouseScroll(GLFWwindow* wnd, double dx, double dy)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  app.control.mouseScroll((float)dx, (float)dy);
}

void OpenGLApplication::initGlfwKeyMap()
{
  glfwKeyMap[GLFW_KEY_UNKNOWN] = KB_NONE;
  glfwKeyMap[GLFW_KEY_SPACE] = KB_SPACE;
  glfwKeyMap[GLFW_KEY_APOSTROPHE] = KB_APOSTROPHE;
  glfwKeyMap[GLFW_KEY_COMMA] = KB_COMMA;
  glfwKeyMap[GLFW_KEY_MINUS] = KB_MINUS;
  glfwKeyMap[GLFW_KEY_PERIOD] = KB_PERIOD;
  glfwKeyMap[GLFW_KEY_SLASH] = KB_SLASH;
  glfwKeyMap[GLFW_KEY_0] = KB_0;
  glfwKeyMap[GLFW_KEY_1] = KB_1;
  glfwKeyMap[GLFW_KEY_2] = KB_2;
  glfwKeyMap[GLFW_KEY_3] = KB_3;
  glfwKeyMap[GLFW_KEY_4] = KB_4;
  glfwKeyMap[GLFW_KEY_5] = KB_5;
  glfwKeyMap[GLFW_KEY_6] = KB_6;
  glfwKeyMap[GLFW_KEY_7] = KB_7;
  glfwKeyMap[GLFW_KEY_8] = KB_8;
  glfwKeyMap[GLFW_KEY_9] = KB_9;
  glfwKeyMap[GLFW_KEY_SEMICOLON] = KB_SEMICOLON;
  glfwKeyMap[GLFW_KEY_EQUAL] = KB_EQUAL;
  glfwKeyMap[GLFW_KEY_A] = KB_A;
  glfwKeyMap[GLFW_KEY_B] = KB_B;
  glfwKeyMap[GLFW_KEY_C] = KB_C;
  glfwKeyMap[GLFW_KEY_D] = KB_D;
  glfwKeyMap[GLFW_KEY_E] = KB_E;
  glfwKeyMap[GLFW_KEY_F] = KB_F;
  glfwKeyMap[GLFW_KEY_G] = KB_G;
  glfwKeyMap[GLFW_KEY_H] = KB_H;
  glfwKeyMap[GLFW_KEY_I] = KB_I;
  glfwKeyMap[GLFW_KEY_J] = KB_J;
  glfwKeyMap[GLFW_KEY_K] = KB_K;
  glfwKeyMap[GLFW_KEY_L] = KB_L;
  glfwKeyMap[GLFW_KEY_M] = KB_M;
  glfwKeyMap[GLFW_KEY_N] = KB_N;
  glfwKeyMap[GLFW_KEY_O] = KB_O;
  glfwKeyMap[GLFW_KEY_P] = KB_P;
  glfwKeyMap[GLFW_KEY_Q] = KB_Q;
  glfwKeyMap[GLFW_KEY_R] = KB_R;
  glfwKeyMap[GLFW_KEY_S] = KB_S;
  glfwKeyMap[GLFW_KEY_T] = KB_T;
  glfwKeyMap[GLFW_KEY_U] = KB_U;
  glfwKeyMap[GLFW_KEY_V] = KB_V;
  glfwKeyMap[GLFW_KEY_W] = KB_W;
  glfwKeyMap[GLFW_KEY_X] = KB_X;
  glfwKeyMap[GLFW_KEY_Y] = KB_Y;
  glfwKeyMap[GLFW_KEY_Z] = KB_Z;
  glfwKeyMap[GLFW_KEY_LEFT_BRACKET] = KB_LEFT_BRACKET;
  glfwKeyMap[GLFW_KEY_BACKSLASH] = KB_BACKSLASH;
  glfwKeyMap[GLFW_KEY_RIGHT_BRACKET] = KB_RIGHT_BRACKET;
  glfwKeyMap[GLFW_KEY_GRAVE_ACCENT] = KB_GRAVE_ACCENT;
  glfwKeyMap[GLFW_KEY_WORLD_1] = KB_WORLD_1;
  glfwKeyMap[GLFW_KEY_WORLD_2] = KB_WORLD_2;
  glfwKeyMap[GLFW_KEY_ESCAPE] = KB_ESCAPE;
  glfwKeyMap[GLFW_KEY_ENTER] = KB_ENTER;
  glfwKeyMap[GLFW_KEY_TAB] = KB_TAB;
  glfwKeyMap[GLFW_KEY_BACKSPACE] = KB_BACKSPACE;
  glfwKeyMap[GLFW_KEY_INSERT] = KB_INSERT;
  glfwKeyMap[GLFW_KEY_DELETE] = KB_DELETE;
  glfwKeyMap[GLFW_KEY_RIGHT] = KB_RIGHT;
  glfwKeyMap[GLFW_KEY_LEFT] = KB_LEFT;
  glfwKeyMap[GLFW_KEY_DOWN] = KB_DOWN;
  glfwKeyMap[GLFW_KEY_UP] = KB_UP;
  glfwKeyMap[GLFW_KEY_PAGE_UP] = KB_PAGE_UP;
  glfwKeyMap[GLFW_KEY_PAGE_DOWN] = KB_PAGE_DOWN;
  glfwKeyMap[GLFW_KEY_HOME] = KB_HOME;
  glfwKeyMap[GLFW_KEY_END] = KB_END;
  glfwKeyMap[GLFW_KEY_CAPS_LOCK] = KB_CAPS_LOCK;
  glfwKeyMap[GLFW_KEY_SCROLL_LOCK] = KB_SCROLL_LOCK;
  glfwKeyMap[GLFW_KEY_NUM_LOCK] = KB_NUM_LOCK;
  glfwKeyMap[GLFW_KEY_PRINT_SCREEN] = KB_PRINT_SCREEN;
  glfwKeyMap[GLFW_KEY_PAUSE] = KB_PAUSE;
  glfwKeyMap[GLFW_KEY_F1] = KB_F1;
  glfwKeyMap[GLFW_KEY_F2] = KB_F2;
  glfwKeyMap[GLFW_KEY_F3] = KB_F3;
  glfwKeyMap[GLFW_KEY_F4] = KB_F4;
  glfwKeyMap[GLFW_KEY_F5] = KB_F5;
  glfwKeyMap[GLFW_KEY_F6] = KB_F6;
  glfwKeyMap[GLFW_KEY_F7] = KB_F7;
  glfwKeyMap[GLFW_KEY_F8] = KB_F8;
  glfwKeyMap[GLFW_KEY_F9] = KB_F9;
  glfwKeyMap[GLFW_KEY_F10] = KB_F10;
  glfwKeyMap[GLFW_KEY_F11] = KB_F11;
  glfwKeyMap[GLFW_KEY_F12] = KB_F12;
  glfwKeyMap[GLFW_KEY_F13] = KB_F13;
  glfwKeyMap[GLFW_KEY_F14] = KB_F14;
  glfwKeyMap[GLFW_KEY_F15] = KB_F15;
  glfwKeyMap[GLFW_KEY_F16] = KB_F16;
  glfwKeyMap[GLFW_KEY_F17] = KB_F17;
  glfwKeyMap[GLFW_KEY_F18] = KB_F18;
  glfwKeyMap[GLFW_KEY_F19] = KB_F19;
  glfwKeyMap[GLFW_KEY_F20] = KB_F20;
  glfwKeyMap[GLFW_KEY_F21] = KB_F21;
  glfwKeyMap[GLFW_KEY_F22] = KB_F22;
  glfwKeyMap[GLFW_KEY_F23] = KB_F23;
  glfwKeyMap[GLFW_KEY_F24] = KB_F24;
  glfwKeyMap[GLFW_KEY_F25] = KB_F25;
  glfwKeyMap[GLFW_KEY_KP_0] = KB_KP_0;
  glfwKeyMap[GLFW_KEY_KP_1] = KB_KP_1;
  glfwKeyMap[GLFW_KEY_KP_2] = KB_KP_2;
  glfwKeyMap[GLFW_KEY_KP_3] = KB_KP_3;
  glfwKeyMap[GLFW_KEY_KP_4] = KB_KP_4;
  glfwKeyMap[GLFW_KEY_KP_5] = KB_KP_5;
  glfwKeyMap[GLFW_KEY_KP_6] = KB_KP_6;
  glfwKeyMap[GLFW_KEY_KP_7] = KB_KP_7;
  glfwKeyMap[GLFW_KEY_KP_8] = KB_KP_8;
  glfwKeyMap[GLFW_KEY_KP_9] = KB_KP_9;
  glfwKeyMap[GLFW_KEY_KP_DECIMAL] = KB_KP_DECIMAL;
  glfwKeyMap[GLFW_KEY_KP_DIVIDE] = KB_KP_DIVIDE;
  glfwKeyMap[GLFW_KEY_KP_MULTIPLY] = KB_KP_MULTIPLY;
  glfwKeyMap[GLFW_KEY_KP_SUBTRACT] = KB_KP_SUBTRACT;
  glfwKeyMap[GLFW_KEY_KP_ADD] = KB_KP_ADD;
  glfwKeyMap[GLFW_KEY_KP_ENTER] = KB_KP_ENTER;
  glfwKeyMap[GLFW_KEY_KP_EQUAL] = KB_KP_EQUAL;
  glfwKeyMap[GLFW_KEY_LEFT_SHIFT] = KB_LEFT_SHIFT;
  glfwKeyMap[GLFW_KEY_LEFT_CONTROL] = KB_LEFT_CONTROL;
  glfwKeyMap[GLFW_KEY_LEFT_ALT] = KB_LEFT_ALT;
  glfwKeyMap[GLFW_KEY_LEFT_SUPER] = KB_LEFT_SUPER;
  glfwKeyMap[GLFW_KEY_RIGHT_SHIFT] = KB_RIGHT_SHIFT;
  glfwKeyMap[GLFW_KEY_RIGHT_CONTROL] = KB_RIGHT_CONTROL;
  glfwKeyMap[GLFW_KEY_RIGHT_ALT] = KB_RIGHT_ALT;
  glfwKeyMap[GLFW_KEY_RIGHT_SUPER] = KB_RIGHT_SUPER;
  glfwKeyMap[GLFW_KEY_MENU] = KB_MENU;
}