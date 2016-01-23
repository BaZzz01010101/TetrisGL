#include "static_headers.h"

#include "Globals.h"

std::string Globals::glErrorMessage;

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

    std::cout << "OpenGL: " << Globals::glErrorMessage << "\n";
  }
  else
    Globals::glErrorMessage.clear();

  return errCode != 0;
}
