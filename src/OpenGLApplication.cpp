#include "static_headers.h"

#include "OpenGLApplication.h"
#include "Crosy.h"

#pragma warning(disable : 4100)

OpenGLApplication::OpenGLApplication(GameLogic & model) : 
  Application(model),
  render(model)
{
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

  const int defaultWinWidth = 640;
  const int defaultWinHeight = 480;
  wnd = glfwCreateWindow(defaultWinWidth, defaultWinHeight, "glTetris", NULL, NULL);

  if (!wnd)
    return false;

  glfwSetWindowUserPointer(wnd, this);
  glfwMakeContextCurrent(wnd);
  glfwSetFramebufferSizeCallback(wnd, OnFramebufferSize);
  glfwSetKeyCallback(wnd, OnKeyClick);
  glfwSetMouseButtonCallback(wnd, OnMouseClick);
  glfwSetCursorPosCallback(wnd, OnMouseMove);

  glewExperimental = GL_TRUE;

  GLenum glewInitResult = glewInit();
  assert(glewInitResult == GLEW_OK);

  // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
  glGetError();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  render.init(defaultWinWidth, defaultWinHeight);
  fps.init();

  return true;
}

void OpenGLApplication::run()
{
  while (!glfwWindowShouldClose(wnd))
  {
    model.update();
    render.update();

    glfwSetWindowTitle(wnd, fps.count(0.5f));
    glfwSwapInterval((int)vSync);
    glfwSwapBuffers(wnd);
    glfwPollEvents();
    //Crosy::sleep(5);
  }
}

void OpenGLApplication::quit()
{
  glfwTerminate();
}

void OpenGLApplication::OnFramebufferSize(GLFWwindow * wnd, int width, int height)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  app.winWidth = width;
  app.winHeight = height;

  app.render.resize(width, height);
}

void OpenGLApplication::OnKeyClick(GLFWwindow * wnd, int key, int scancode, int action, int mods)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  if (action == GLFW_REPEAT || action == GLFW_PRESS)
  {
    if (app.model.gameState == GameLogic::gsPlayingGame)
      switch (key)
    {
      case GLFW_KEY_LEFT:
        app.model.shiftCurrentFigureLeft();
        break;
      case GLFW_KEY_RIGHT:
        app.model.shiftCurrentFigureRight();
        break;
      case GLFW_KEY_UP:
        app.model.rotateCurrentFigureLeft();
        break;
      case GLFW_KEY_DOWN:
        app.model.rotateCurrentFigureRight();
        break;
      case GLFW_KEY_SPACE:
        app.model.dropCurrentFigure();
        break;
      case GLFW_KEY_RIGHT_CONTROL:
      case GLFW_KEY_LEFT_CONTROL:
        app.model.holdCurrentFigure();
        break;
      case GLFW_KEY_ENTER:
        if (action != GLFW_REPEAT)
          app.model.forceDown = true;
        break;
      case GLFW_KEY_LEFT_ALT:
      case GLFW_KEY_RIGHT_ALT:
        app.render.showWireframe = true;
        break;
      case GLFW_KEY_F11:
        app.vSync = !app.vSync;
        break;
    }
  }

  if (action == GLFW_RELEASE)
  {
    if (app.model.gameState == GameLogic::gsPlayingGame)
      switch (key)
    {
      case GLFW_KEY_ENTER:
        app.model.forceDown = false;
        break;
      case GLFW_KEY_LEFT_ALT:
      case GLFW_KEY_RIGHT_ALT:
        app.render.showWireframe = false;
        break;
    }
  }
}

void OpenGLApplication::OnMouseClick(GLFWwindow * wnd, int button, int action, int mods)
{

}

void OpenGLApplication::OnMouseMove(GLFWwindow* wnd, double xpos, double ypos)
{
  OpenGLApplication & app = *reinterpret_cast<OpenGLApplication *>(glfwGetWindowUserPointer(wnd));

  if (app.winWidth && app.winHeight)
  {
    const float gameAspect = Globals::gameBkSize.x / Globals::gameBkSize.y;

    if (float(app.winWidth) / app.winHeight > gameAspect)
    {
      app.mouseX = float((xpos - (app.winWidth - app.winHeight) / 2) / app.winHeight * 2.0f - 1.0f);
      app.mouseY = float(1.0f - ypos / app.winHeight * 2.0f);
    }
    else
    {
      app.mouseX = float(xpos * gameAspect / app.winWidth * 2.0f - 1.0f);
      app.mouseY = float(1.0f - (ypos - (app.winHeight - app.winWidth / gameAspect) / 2.0f) / (app.winWidth / gameAspect) * 2.0f);
    }
  }
}
