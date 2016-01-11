#include "static_headers.h"

#include "Layout.h"
#include "Globals.h"
#include "Crosy.h"

std::string Globals::glErrorMessage;

int Globals::nextFiguresCount = 3;
float Globals::rowsDeletionEffectTime = 0.8f;
float Globals::menuShowingTime = 0.4f;
float Globals::menuHidingTime = 0.3f;
float Globals::settingsShowingTime = 0.3f;
float Globals::settingsHidingTime = 0.2f;
float Globals::leaderboardShowingTime = 0.3f;
float Globals::leaderboardHidingTime = 0.2f;
int Globals::smallFontSize = 24;
int Globals::midFontSize = 32;
int Globals::bigFontSize = 48;

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
