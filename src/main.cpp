#include <iostream>
#include "glall.h"


void OnFramebufferSize(GLFWwindow * win, int width, int height)
{
  glViewport(0, 0, width, height);
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
    GLFWwindow * win = glfwCreateWindow(initWinWidth, initWinHeight, "TxCube", NULL, NULL);

    if (win)
    {
      glfwMakeContextCurrent(win);

//      glfwSetWindowSizeCallback(win, OnWindowSize);
      glfwSetFramebufferSizeCallback(win, OnFramebufferSize);
      glfwSetKeyCallback(win, OnKeyClick);
      glfwSetMouseButtonCallback(win, OnMouseClick);
      glfwSetCursorPosCallback(win, OnMouseMove);

      glfwSwapInterval(1);
      glewExperimental = GL_TRUE;

      if (glewInit() == GLEW_OK)
      {
        // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
        glGetError();

        GLuint vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);       assert(!glGetError());
        glBindVertexArray(vertexArrayId);           assert(!glGetError());
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  assert(!glGetError());
        glEnable(GL_CULL_FACE);                     assert(!glGetError());
        glCullFace(GL_FRONT);                       assert(!glGetError());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0f);

        int cnt = 0;
        uint64_t freq = Crosy::getPerformanceFrequency();
        uint64_t ticks = Crosy::getPerformanceCounter();

        while (!glfwWindowShouldClose(win))
        {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     assert(!glGetError());

          glfwSwapBuffers(win);
          glfwPollEvents();
          cnt++;

          Crosy::sleep(10);

          float timePass = float(Crosy::getPerformanceCounter() - ticks) / freq;

          if (timePass > 10)
          {
            float fps = cnt / timePass;
            const int bufSize = 256;
            char str[bufSize];
            Crosy::snprintf(str, bufSize, "Fps: %.3f", fps);
            glfwSetWindowTitle(win, str);
            ticks = Crosy::getPerformanceCounter();
            cnt = 0;
          }
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
