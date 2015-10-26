#include "Crosy.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/glfw3.h>
//#include "Game.h"
#include "FpsCounter.h"
#include "Globals.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"

Model model;
View view(model);
Controller controller;
//Game game;
FpsCounter fps;
int winWidth;
int winHeight;

void OnFramebufferSize(GLFWwindow * win, int width, int height)
{
  winWidth = width;
  winHeight = height;
  view.resize(width, height);
}

void OnKeyClick(GLFWwindow * win, int key, int scancode, int action, int mods)
{
  if (action == GLFW_REPEAT || action == GLFW_PRESS)
  {
    if (model.gameState == Model::gsPlayingGame) 
    switch (key)
    {
    case GLFW_KEY_LEFT:
      model.shiftCurrentFigureLeft();
      break;
    case GLFW_KEY_RIGHT:
      model.shiftCurrentFigureRight();
      break;
    case GLFW_KEY_UP:
      model.rotateCurrentFigureLeft();
      break;
    case GLFW_KEY_DOWN:
      model.rotateCurrentFigureRight();
      break;
    case GLFW_KEY_SPACE:
      model.dropCurrentFigure();
      break;
    case GLFW_KEY_RIGHT_CONTROL:
    case GLFW_KEY_LEFT_CONTROL:
      model.holdCurrentFigure();
      break;
    case GLFW_KEY_ENTER:
      if(action != GLFW_REPEAT)
        model.forceDown = true;
      break;
    case GLFW_KEY_LEFT_ALT:
    case GLFW_KEY_RIGHT_ALT:
      model.showWireframe = true;
      break;
    }
  }

  if (action == GLFW_RELEASE)
  {
    if (model.gameState == Model::gsPlayingGame)
      switch (key)
    {
      case GLFW_KEY_ENTER:
        model.forceDown = false;
        break;
      case GLFW_KEY_LEFT_ALT:
      case GLFW_KEY_RIGHT_ALT:
        model.showWireframe = false;
        break;
    }
  }
  //Game::KeyMask keyMask = Game::kmNone;

  //switch (key)
  //{
  //case GLFW_KEY_LEFT:
  //  keyMask = Game::kmLeft;
  //  break;
  //case GLFW_KEY_RIGHT:
  //  keyMask = Game::kmRight;
  //  break;
  //case GLFW_KEY_UP:
  //  keyMask = Game::kmRotLeft;
  //  break;
  //case GLFW_KEY_DOWN:
  //  keyMask = Game::kmRotRight;
  //  break;
  //case GLFW_KEY_SPACE:
  //  keyMask = Game::kmDrop;
  //  break;
  //}

  //switch (action)
  //{
  //case GLFW_REPEAT:
  //  game.keyState |= keyMask;
  //  break;
  //case GLFW_PRESS:
  //  game.keyState |= keyMask;
  //  break;
  //case GLFW_RELEASE:
  //  game.keyState &= ~keyMask;
  //  break;
  //}
}

void OnMouseClick(GLFWwindow * win, int button, int action, int mods)
{

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  const float gameAspect = Globals::gameBkSize.x / Globals::gameBkSize.y;

  if (float(winWidth) / winHeight > gameAspect)
  {
    model.mouseX = (xpos - (winWidth - winHeight) / 2) / winHeight * 2.0f - 1.0f;
    model.mouseY = 1.0f - ypos / winHeight * 2.0f;
  }
  else
  {
    model.mouseX = xpos * gameAspect / winWidth * 2.0f - 1.0f;
    model.mouseY = 1.0f - (ypos - (winHeight - winWidth / gameAspect) / 2.0f) / (winWidth / gameAspect) * 2.0f;
  }
}

int main()
{
  srand((unsigned int)Crosy::getPerformanceCounter());
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

      fps.init(win);
      view.init(initWinWidth, initWinHeight);

      while (!glfwWindowShouldClose(win))
      {
        model.update();
        view.update();
        fps.pulse();

        glfwSwapBuffers(win);
        glfwPollEvents();
        //Crosy::sleep(5);
      }

      retVal = 0;

    }

    glfwTerminate();
  }

  if (retVal)
    std::cin.get();

  return retVal;
}
