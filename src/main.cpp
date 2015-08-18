#include <iostream>
#include "glall.h"
#include "Game.h"
#include "FpsCounter.h"

Game game;
FpsCounter fps;

void OnFramebufferSize(GLFWwindow * win, int width, int height)
{
  glViewport(0, 0, width, height);
  if (width && height)
    game.resize(float(width) / height);
}

//void OnWindowSize(GLFWwindow * win, int width, int height)
//{
//  glViewport(0, 0, width, height);
//}

void OnKeyClick(GLFWwindow * win, int key, int scancode, int action, int mods)
{

}

void OnMouseClick(GLFWwindow * win, int button, int action, int mods)
{

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{

}

int main()
{
  int retVal = -1;

  if (glfwInit())
  {
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int initWinWidth = 640;
    const int initWinHeight = 480;
    GLFWwindow * win = glfwCreateWindow(initWinWidth, initWinHeight, "glTetris", NULL, NULL);

    if (win)
    {
      glfwMakeContextCurrent(win);

//      glfwSetWindowSizeCallback(win, OnWindowSize);
      glfwSetFramebufferSizeCallback(win, OnFramebufferSize);
      glfwSetKeyCallback(win, OnKeyClick);
      glfwSetMouseButtonCallback(win, OnMouseClick);
      glfwSetCursorPosCallback(win, OnMouseMove);
      glfwSwapInterval(1);

      if(game.init())
      {
        game.resize(float(initWinWidth) / initWinHeight);
        fps.init(win);

        while (!glfwWindowShouldClose(win))
        {
          game.pulse();
          fps.pulse();

          glfwSwapBuffers(win);
          glfwPollEvents();
          Crosy::sleep(10);
        }

        retVal = 0;
      }

    }

    glfwTerminate();
  }

  if (retVal)
    std::cin.get();

  return retVal;
}
