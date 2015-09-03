#include "Globals.h"


std::string Globals::glErrorMessage;
GLuint Globals::mainArrayTextureId = 0;
int Globals::mainArrayTextureSize = 64;
float Globals::mainArrayTexturePixelSize = 1.0f / Globals::mainArrayTextureSize;
int Globals::backgroundTexIndex = 0;
int Globals::openBlocksTexIndex = 1;
int Globals::closedBlocksTexIndex = 8;
int Globals::vertBlocksTexIndex = 15;
int Globals::horzBlocksTexIndex = 22;
int Globals::dropTrailsTexIndex = 29;
int Globals::shadowTexIndex = 36;
int Globals::dropSparklesTexIndex = 37;
glm::vec2 Globals::gameBkPos(-0.7f, 1.0f);
glm::vec2 Globals::gameBkSize(1.4f, 2.0f);
glm::vec2 Globals::glassPos(-0.4f, 0.75f);
glm::vec2 Globals::glassSize(0.8f, 1.6f);

Globals::Globals()
{
}


Globals::~Globals()
{
}

bool checkGlErrors()
{
  GLenum errCode = glGetError();

  if (errCode)
  {
    switch (errCode)
    {

    case GL_NO_ERROR:
      Globals::glErrorMessage = "No error";
      break;
    case GL_INVALID_ENUM:
      Globals::glErrorMessage = "Invalid enum";
      break;
    case GL_INVALID_VALUE:
      Globals::glErrorMessage = "Invalid value";
      break;
    case GL_INVALID_OPERATION:
      Globals::glErrorMessage = "Invalid operation";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      Globals::glErrorMessage = "Invalid framebuffer operation";
      break;
    case GL_OUT_OF_MEMORY:
      Globals::glErrorMessage = "Out of memory";
      break;
    case GL_STACK_UNDERFLOW:
      Globals::glErrorMessage = "Stack underflow";
      break;
    case GL_STACK_OVERFLOW:
      Globals::glErrorMessage = "Stack overflow";
      break;
    default:
      Globals::glErrorMessage = "Unknown error";
    }

    std::cout << Globals::glErrorMessage << "\n";
  }
  else
    Globals::glErrorMessage.clear();

  return errCode != 0;
}
