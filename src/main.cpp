#include <iostream>
#include "glall.h"
#include "Game.h"
#include "FpsCounter.h"
#include "Globals.h"

Game game;
FpsCounter fps;

void OnFramebufferSize(GLFWwindow * win, int width, int height)
{
  const float gameAspect = Globals::gameBkSize.x / Globals::gameBkSize.y;

  if (float(width) / height > gameAspect)
    glViewport((width - height) / 2, 0, height, height);
  else
    glViewport(width * (1 - 1 / gameAspect) / 2, (height - width / gameAspect) / 2, width / gameAspect, width / gameAspect);
}

void OnKeyClick(GLFWwindow * win, int key, int scancode, int action, int mods)
{
  Game::KeyMask keyMask = Game::kmNone;

  switch (key)
  {
  case GLFW_KEY_LEFT:
    keyMask = Game::kmLeft;
    break;
  case GLFW_KEY_RIGHT:
    keyMask = Game::kmRight;
    break;
  case GLFW_KEY_UP:
    keyMask = Game::kmRotLeft;
    break;
  case GLFW_KEY_DOWN:
    keyMask = Game::kmRotRight;
    break;
  case GLFW_KEY_SPACE:
    keyMask = Game::kmDrop;
    break;
  }

  switch (action)
  {
  case GLFW_REPEAT:
    game.keyState |= keyMask;
    break;
  case GLFW_PRESS:
    game.keyState |= keyMask;
    break;
  case GLFW_RELEASE:
    game.keyState &= ~keyMask;
    break;
  }
}

void OnMouseClick(GLFWwindow * win, int button, int action, int mods)
{

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{

}

int main()
{
  srand(GetTickCount());
  int retVal = -1;

  if (glfwInit())
  {
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    //glfwWindowHint(GLFW_SAMPLES, 16);
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
      glfwSwapInterval(0);

      if(game.init())
      {
        OnFramebufferSize(win, initWinWidth, initWinHeight);
        fps.init(win);

        while (!glfwWindowShouldClose(win))
        {
          game.pulse();
          fps.pulse();

          glfwSwapBuffers(win);
          glfwPollEvents();
          //Crosy::sleep(10);
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
